#include "StdAfx.h"
#include "IATHookImpl.h"
#include <DbgHelp.h>
#include <TLHelp32.h>
#pragma comment(lib,"dbghelp.lib")

CIATHookImpl::CIATHookImpl(void)
{
	m_hookMap.clear();
}

CIATHookImpl::~CIATHookImpl(void)
{
	CleanupHook();
}

std::wstring CIATHookImpl::Utf82W(LPCSTR szContent,int size)
{
	std::wstring strContent;
	int len = MultiByteToWideChar(CP_UTF8,0,szContent,size,NULL,0);
	strContent.resize(len);
	MultiByteToWideChar(CP_UTF8,0,szContent,size,(wchar_t*)strContent.c_str(),len);
	if(len > 0 && size == -1)
		strContent.resize(len-1);
	return strContent;
}

std::string CIATHookImpl::W2Utf8(LPCWSTR szContent,int size)
{
	std::string strContent;
	int len = WideCharToMultiByte(CP_UTF8,0,szContent,size,NULL,0,NULL,NULL);
	strContent.resize(len);
	WideCharToMultiByte(CP_UTF8,0,szContent,size,(char *)strContent.c_str(),len,NULL,NULL);
	if(len > 0 && size == -1)
		strContent.resize(len-1);
	return strContent;
}

std::wstring CIATHookImpl::ToLower(const std::wstring& str)
{
	std::wstring strTemp = str;
	for (int i = 0; i < str.length(); i++)
	{
		strTemp[i] = tolower(str[i]);
	}
	return strTemp;
}

std::string CIATHookImpl::ToLower(const std::string& str)
{
	std::string strTemp = str;
	for (int i = 0; i < str.length(); i++)
	{
		strTemp[i] = tolower(str[i]);
	}
	return strTemp;
}

void CIATHookImpl::GetModuleByTag(const wchar_t* tag, std::vector<HMODULE>& vModules)
{
	HANDLE hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

	if (hSnapModule == INVALID_HANDLE_VALUE)return;
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);
	Module32First(hSnapModule, &modEntry);
	HMODULE hModfirst = modEntry.hModule;

	wchar_t modName[MAX_PATH] = {0};

	if (!GetModuleFileName(hModfirst, modName, sizeof(modName)))
	{
		CloseHandle(hSnapModule);
		return;
	}
	else
	{
		if (ToLower(std::wstring(modName)).find(tag) != -1)
		{
			vModules.push_back(hModfirst);
		}
	}
	while (Module32Next(hSnapModule, &modEntry))
	{
		HMODULE hModNext = modEntry.hModule;
		if (!GetModuleFileName(hModNext, modName, sizeof(modName)))
		{
			CloseHandle(hSnapModule);
			return;
		}
		else
		{
			if (ToLower(std::wstring(modName)).find(tag) != -1)
			{
				vModules.push_back(hModNext);
			}
		}
	}
	CloseHandle(hSnapModule);
}

int CIATHookImpl::SetHook(PROC replaceFuncEntry, PROC originalFuncEntry, HMODULE hModule, const wchar_t* szTargetModuleName)
{
	if(ReplaceFunc(replaceFuncEntry,originalFuncEntry,hModule,szTargetModuleName))
	{
		IATHookInfo hookInfo;
		hookInfo.originalFuncEntry = originalFuncEntry;
		hookInfo.replaceFuncEntry = replaceFuncEntry;
		hookInfo.hTargetModule = hModule;
		wcscpy_s(hookInfo.szTargetModuleName, MAX_PATH, szTargetModuleName);
		m_hookMap.insert(std::make_pair(m_hookMap.size(), hookInfo));
		return m_hookMap.size() - 1;
	}
	return -1;
}

bool CIATHookImpl::UnHook(int idHook,bool bDelete)
{
	if (m_hookMap.find(idHook) != m_hookMap.end())
	{
		IATHookInfo hookInfo = m_hookMap.find(idHook)->second;
		if(ReplaceFunc(hookInfo.originalFuncEntry, hookInfo.replaceFuncEntry, hookInfo.hTargetModule, hookInfo.szTargetModuleName))
		{
			if(bDelete)
			{
				m_hookMap.erase(m_hookMap.find(idHook));
			}
		}
	}
	return false;
}

bool CIATHookImpl::ReplaceFunc( PROC replaceFuncEntry, PROC originalFuncEntry, HMODULE hModule, const wchar_t* szTargetModuleName )
{
	ULONG uSize = 0;
	//查找目标模块
	PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &uSize);
	if (pID == NULL)
	{
		return -1;
	}
	while (pID && pID->Name)
	{
		PSTR pszModName = (PSTR)((PBYTE)hModule + pID->Name);
		std::string strModName = pszModName;
		if (wcsicmp(Utf82W(strModName.c_str()).c_str(), szTargetModuleName) == 0)
		{
			//找到目标模块
			break;
		}
		pID++;
	}
	if (pID == NULL || pID->Name == NULL)
	{
		return -1;
	}
	//遍历IAT表，查找目标函数地址
	PIMAGE_THUNK_DATA pThunk = PIMAGE_THUNK_DATA((PBYTE)hModule + pID->FirstThunk);
	if (pThunk == NULL)
	{
		return -1;
	}
	while (pThunk && pThunk->u1.Function)
	{
		PROC* ppfnEntry = (PROC*) & (pThunk->u1.Function);
		if ((DWORD)pThunk->u1.Function == (DWORD)originalFuncEntry)
		{
			//找到目标函数，修改IAT
			SIZE_T sBytesWritten = 0;
			BOOL bProtectResult = FALSE;
			DWORD dwOldProtect = 0;
			MEMORY_BASIC_INFORMATION memInfo = {0};
			if (::VirtualQuery(ppfnEntry, &memInfo, sizeof(memInfo)))
			{
				//修改内存为可读写
				bProtectResult = ::VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, PAGE_READWRITE, &dwOldProtect);
				//修改内存
				::WriteProcessMemory(::GetCurrentProcess(), ppfnEntry, &replaceFuncEntry, sizeof(PROC*), &sBytesWritten);
				//恢复内存默认权限
				bProtectResult = ::VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, PAGE_READONLY, &dwOldProtect);
				return true;
			}
		}
		pThunk++;
	}
	return false;
}

void CIATHookImpl::CleanupHook()
{
	for (std::map<int, IATHookInfo>::iterator iter = m_hookMap.begin(); iter != m_hookMap.end();)
	{
		UnHook(iter->first,false);
		m_hookMap.erase(iter++);
	}
	m_hookMap.clear();
}
