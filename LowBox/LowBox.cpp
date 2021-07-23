// LowBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "userenv.h"
#include "sddl.h"
#include "stdio.h"
#include "time.h"

wchar_t* ContainerName = L"waliedcontainer";





int _tmain(int argc, _TCHAR* argv[])
{
	PSID appContainerSid;

	HRESULT hr = 
	CreateAppContainerProfile(ContainerName,L"Walied Assar",L"My first container",0,0,&appContainerSid);

	if(hr==S_OK)
	{
		wprintf(L"hr: %I64X (created successfully.\r\n",hr);
	}
	else if(hr == 0x800700B7)
	{
		wprintf(L"hr: %I64X (Already exists).\r\n",hr);

		hr = DeriveAppContainerSidFromAppContainerName(ContainerName,&appContainerSid);

		if(hr != S_OK)
		{
			wprintf(L"Error getting AppContainer SID, err: %X\r\n",GetLastError());
			return -1;
		}
	}

	wchar_t* appContainerSid_s = (wchar_t*)LocalAlloc(LMEM_ZEROINIT,0x200);
	if(appContainerSid_s)
	{
		if(ConvertSidToStringSid(appContainerSid,&appContainerSid_s))
		{
			wprintf(L"Sid: %s\r\n",appContainerSid_s);
		}
		else
		{
			printf("ConvertSidToStringSid, err: %X\r\n",GetLastError());
		}
		LocalFree(appContainerSid_s);
		appContainerSid_s = 0;
	}
	printf("SID created/extracted successfully.\r\n");

	SECURITY_CAPABILITIES sc = { 0 };
	sc.AppContainerSid = appContainerSid;

	PROCESS_INFORMATION pi;

	STARTUPINFOEX si = { sizeof(si) };
	si.StartupInfo.cb = sizeof(si);

	
	SIZE_T size = 0;

	BOOL bX = InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
	printf("%X\r\n",bX);
	printf("%I64X\r\n",size);
	

	

	LPPROC_THREAD_ATTRIBUTE_LIST pAttList = (LPPROC_THREAD_ATTRIBUTE_LIST)VirtualAlloc(0,size,MEM_COMMIT,PAGE_READWRITE);
	if(pAttList)
	{
		printf("Allocated\r\n");
		si.lpAttributeList = pAttList;
		
		bX = InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &size);
		printf("%X\r\n",bX);
		printf("%I64X\r\n",size);
		
		
		bX = UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES, &sc, sizeof(sc), nullptr, nullptr);
		printf("%X\r\n",bX);

		wchar_t exePath[MAX_PATH] = L"c:\\windows\\system32\\notepad.exe";

		if(argc > 1)
		{
			wcscpy(exePath,argv[1]);
		}
		
		if(CreateProcess(0, exePath, nullptr, nullptr, FALSE,
					EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, 
					(LPSTARTUPINFO)&si, &pi))
		{
			printf("Process created successfully.\r\n");
		}
		else
		{
			printf("Process creation failed.\r\n");
		}

		VirtualFree(pAttList,0,MEM_RELEASE);
	}

	printf("Done");
	Sleep(-1);
	return 0;
}

