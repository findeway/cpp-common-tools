/********************************************************************
	created:	2014/07/09
	created:	9:7:2014   10:17
	filename: 	IATHookImpl.h
	file base:	IATHookImpl
	file ext:	h
	author:		findeway
	
	purpose:	IAT HOOK	
*********************************************************************/
#pragma once
#include <WinDef.h>
#include <vector>
#include <string>
#include <map>

typedef struct tagIATHookInfo
{
    PROC originalFuncEntry;
    PROC replaceFuncEntry;
    HMODULE hTargetModule;		//目标模块的宿主模块句柄
    wchar_t szTargetModuleName[MAX_PATH];
    tagIATHookInfo()
    {
        originalFuncEntry = NULL;
        replaceFuncEntry = NULL;
        hTargetModule = NULL;
        memset(szTargetModuleName, 0, MAX_PATH * sizeof(wchar_t));
    }
} IATHookInfo;

class CIATHookImpl
{
public:
    CIATHookImpl(void);
    ~CIATHookImpl(void);


    //************************************
    // Method:    SetHook
    // FullName:  CIATHookImpl::SetHook
    // Access:    public 
    // Returns:   int  错误返回-1,成功返回hookId
    // Parameter: PROC replaceFuncEntry
    // Parameter: PROC originalFuncEntry
    // Parameter: HMODULE hModule hook的目标模块所在模块的句柄
    // Parameter: const wchar_t * szTargetModuleName
	// Desc:	  
    //************************************
    int SetHook(PROC replaceFuncEntry, PROC originalFuncEntry, HMODULE hModule, const wchar_t* szTargetModuleName);

    //************************************
    // Method:    UnHook
    // FullName:  CIATHookImpl::UnHook
    // Access:    public 
    // Returns:   bool
    // Parameter: int idHook
	// Desc:
    //************************************
    bool UnHook(int idHook);

	//************************************
	// FullName:  CIATHookImpl::GetModuleByTag
	// Access:    public 
	// Returns:   void
	// Parameter: const wchar_t * tag
	// Parameter: std::vector<HMODULE> & vModules
	// Desc:	从当前模块中查找带指定标记的模块
	//************************************
	void GetModuleByTag(const wchar_t* tag, std::vector<HMODULE>& vModules);

protected:
    //工具函数
    std::wstring ToLower(const std::wstring& str);
	std::string ToLower(const std::string& str);
	std::wstring Utf82W(LPCSTR szContent,int size = -1);
	std::string W2Utf8(LPCWSTR szContent,int size = -1);

    std::map<int, IATHookInfo>							m_hookMap;
};
