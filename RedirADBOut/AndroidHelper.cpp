#include "StdAfx.h"
#include "AndroidHelper.h"
#include <cfgmgr32.h>
#include <Dbt.h>
#include "Util.h"
#include <algorithm>

#pragma comment(lib,"setupapi")

const wchar_t* CMD_GET_BRAND = L"adb.exe -d shell getprop ro.product.brand";
const wchar_t* CMD_GET_MODEL = L"adb.exe -d shell getprop ro.product.model";
const wchar_t* CMD_GET_VERSION = L"adb.exe -d shell getprop ro.build.version.release";
const wchar_t* CMD_GET_IMEI = L"adb.exe -d shell dumpsys iphonesubinfo";
const wchar_t* CMD_GET_MAC = L"adb.exe -d shell cat /sys/class/net/wlan0/address";
const wchar_t* CMD_PUSH_FILE = L"adb.exe push %s %s";

const wchar_t* ERROR_INFO_GETIMEI_FAILED = L"Failed to get IMEI code";
const wchar_t* ERROR_INFO_GETMODEL_FAILED = L"Failed to get Model";
const wchar_t* ERROR_INFO_GETVERSION_FAILED = L"Failed to get Version";
const wchar_t* ERROR_INFO_GETMAC_FAILED = L"Failed to get MAC";
const wchar_t* ERROR_INFO_GETBRAND_FAILED = L"Failed to get Brand";

#define ADB_BUFFER_SIZE		2048
#define DURATION_WAIT_ADB	(5*1000)

CAndroidHelper::CAndroidHelper(void)
{
    m_hDevInfo = NULL;
}

CAndroidHelper::~CAndroidHelper(void)
{
}

void CAndroidHelper::NotifyDeviceChanged(DWORD wParam, DWORD lParam)
{
    if (wParam == DBT_DEVNODES_CHANGED || wParam == DBT_DEVICEARRIVAL)
    {
        //有新设备插入，这里有些设备不会触发DBT_DEVICEARRIVAL,所以要使用DBT_DEVNODES_CHANGED
        SP_DEVINFO_DATA deviceInfo = {0};
        deviceInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SearchPhone(&deviceInfo))
        {
            if (CheckDeviceDriver(GetDeviceInstanceId(m_hDevInfo, deviceInfo).c_str()))
            {
                std::string strResult;
				PushFile(_T("D:\\workspace\\sohu2\\Lightening_adb\\bin\\Debug\\letvgetkey1"),_T("/sdcard/video"),NULL);
            }
        }
    }
}

bool CAndroidHelper::SearchPhone(SP_DEVINFO_DATA* pDeviceInfo)
{
    bool bResult = false;
    m_hDevInfo = SetupDiGetClassDevs(NULL, L"USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (NULL != m_hDevInfo)
    {
        for (int index = 0; SetupDiEnumDeviceInfo(m_hDevInfo, index, pDeviceInfo); index++)
        {
            std::wstring strCompatibleId = GetDeviceRegisterProperty(m_hDevInfo, *pDeviceInfo, SPDRP_COMPATIBLEIDS);
            //如果兼容ID包含"usb\\class_ff&subclass_42"这个设备是手机(不区分大小写)
            std::transform(strCompatibleId.begin(), strCompatibleId.end(), strCompatibleId.begin(), ::tolower);
            if (_tcsstr(strCompatibleId.c_str(), _T("usb\\class_ff&subclass_42")) != NULL)
            {
                bResult = true;
                break;
            }

            //有些手机的兼容ID不是"usb\\class_ff&subclass_42",那就比较麻烦了,需要匹配 硬件ID
            std::wstring strHardwareID = GetDeviceRegisterProperty(m_hDevInfo, *pDeviceInfo, SPDRP_HARDWAREID);	//VID_1234&PID_4321
            std::vector<std::wstring> vecPVIDs = SpliterString(strHardwareID, L"&");
            if (vecPVIDs.size() >= 2)
            {
                std::vector<std::wstring> pairVid = SpliterString(vecPVIDs[0], L"_");
                std::vector<std::wstring> pairPid = SpliterString(vecPVIDs[1], L"_");
                //开始匹配PID VID,暂时未实现
            }
        }
    }
    return bResult;
}

std::wstring CAndroidHelper::GetDeviceRegisterProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA deviceInfo, DWORD categoryId)
{
    DWORD dataType = 0;
    DWORD buffSize = 0;
    SetupDiGetDeviceRegistryProperty(hDevInfo, &deviceInfo, categoryId, &dataType, NULL, buffSize, &buffSize);
    DWORD dwErr = GetLastError();
    if (dwErr != ERROR_INSUFFICIENT_BUFFER)
    {
        return L"";
    }
    LPTSTR szParamID = (LPTSTR)LocalAlloc(LPTR, buffSize + 1);
    SetupDiGetDeviceRegistryProperty(hDevInfo, &deviceInfo, categoryId, &dataType, (PBYTE)szParamID, buffSize, &buffSize);
    std::wstring strParamId = szParamID;
    ::LocalFree(szParamID);
    return strParamId;
}

std::wstring CAndroidHelper::GetDeviceInstanceId(HDEVINFO hDevInfo, SP_DEVINFO_DATA deviceInfo)
{
    DWORD dataType = 0;
    DWORD buffSize = 0;
    wchar_t szInstanceId[1024] = {0};
    SetupDiGetDeviceInstanceId(hDevInfo, &deviceInfo, NULL, buffSize, &buffSize);
    DWORD dwErr = GetLastError();
    if (dwErr != ERROR_INSUFFICIENT_BUFFER)
    {
        return L"";
    }
    SetupDiGetDeviceInstanceId(hDevInfo, &deviceInfo, szInstanceId, buffSize, &buffSize);
    return szInstanceId;
}

bool CAndroidHelper::CheckDeviceDriver(const wchar_t* szInstanceId)
{
    DEVINST deviceInstance = 0;
    if (CM_Locate_DevNode(&deviceInstance, (DEVINSTID_W)szInstanceId, CM_LOCATE_DEVNODE_NORMAL) == CR_SUCCESS)
    {
        DWORD status = 0;
        DWORD problemNumber = 0;
        if (CM_Get_DevNode_Status(&status, &problemNumber, deviceInstance, 0) == CR_SUCCESS)
        {
            if (!(status & DN_HAS_PROBLEM))
            {
                //驱动正常安装
                return true;
            }
            else
            {
                if (problemNumber == CM_PROB_DRIVER_FAILED_LOAD || problemNumber == CM_PROB_DRIVER_FAILED_PRIOR_UNLOAD)
                {
                    //驱动加载失败
                    return false;
                }
                else
                {
                    //驱动未安装
                    return false;
                }
            }
        }
    }
    return false;
}

bool CAndroidHelper::PostAdbCommand(const wchar_t* szCMD, std::string& strResult)
{
	bool bResult = false;
    SECURITY_ATTRIBUTES securityAttributes = {0};
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttributes.lpSecurityDescriptor = NULL;
	securityAttributes.bInheritHandle = TRUE;

   
    HANDLE hStdoutReadTmp;				// parent stdout read handle
    HANDLE hStdoutWrite, hStderrWrite;	// child stdout write handle
    HANDLE hStdinWriteTmp;				// parent stdin write handle
    HANDLE hStdinRead;					// child stdin read handle
    HANDLE hStdinWrite, hStdoutRead;
    if (!::CreatePipe(&hStdoutReadTmp, &hStdoutWrite, &securityAttributes, 0))
    {
        return false;
    }
    // Create a duplicate of the stdout write handle for the std
    // error write handle. This is necessary in case the child
    // application closes one of its std output handles.
    if (!::DuplicateHandle(::GetCurrentProcess(),hStdoutWrite,::GetCurrentProcess(),&hStderrWrite,0,TRUE,DUPLICATE_SAME_ACCESS))
    {
        return false;
    }

    // Create a child stdin pipe.
    if (!::CreatePipe(&hStdinRead, &hStdinWriteTmp, &securityAttributes, 0))
    {
        return false;
    }

    // Create new stdout read handle and the stdin write handle.
    // Set the inheritance properties to FALSE. Otherwise, the child
    // inherits the these handles; resulting in non-closeable
    // handles to the pipes being created.
    if (!::DuplicateHandle(::GetCurrentProcess(),hStdoutReadTmp,::GetCurrentProcess(),&hStdoutRead,0,FALSE,DUPLICATE_SAME_ACCESS))
    {
        return false;
    }

    if (!::DuplicateHandle(::GetCurrentProcess(),hStdinWriteTmp,::GetCurrentProcess(),&hStdinWrite,0, FALSE,DUPLICATE_SAME_ACCESS))
    {
        return false;
    }

    // Close inheritable copies of the handles we do not want to
    // be inherited.
    CloseHandle(hStdoutReadTmp);
    CloseHandle(hStdinWriteTmp);

    PROCESS_INFORMATION processInfo = {0};
    STARTUPINFO startInfo;
    startInfo.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&startInfo);
    startInfo.wShowWindow = SW_HIDE;
    startInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    startInfo.hStdError = hStderrWrite;			//把子进程的标准错误输出重定向到管道输入
    startInfo.hStdOutput = hStdoutWrite;		//把子进程的标准输出重定向到管道输入
    startInfo.hStdInput = hStdinRead;			//把子进程的标准输入重定向到管道输出

    TCHAR szCommand[MAX_PATH] = {0};
    wcscpy_s(szCommand, MAX_PATH, szCMD);
    if (!CreateProcess(NULL, szCommand, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &processInfo))
    {
        CloseHandle(hStderrWrite);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStdinRead);
        return false;
    }
    if(WAIT_TIMEOUT == WaitForSingleObject(processInfo.hProcess,DURATION_WAIT_ADB))
    {
    	TerminateProcess(processInfo.hProcess,0);
    	CloseHandle(processInfo.hProcess);
    	return false;
    }
	std::string pipeResult = ReadResponseFromPipe(hStdoutRead);
	strResult = FilterResult(szCMD,pipeResult);

    CloseHandle(hStderrWrite);
    CloseHandle(hStdoutWrite);
    CloseHandle(hStdinRead);

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    TerminateProcess(processInfo.hProcess, 0);
    return bResult;
}

bool CAndroidHelper::PushFile( const wchar_t* szSourcefile, const wchar_t* szDest, ProgressCallback callback )
{
	std::string strResult;
	CString strCMD;
	if(_taccess(szSourcefile,0) == -1)
	{
		return false;
	}
	strCMD.Format(CMD_PUSH_FILE,szSourcefile,szDest);
	PostAdbCommand(LPCTSTR(strCMD), strResult);
	if(!strResult.empty())
	{
		return true;
	}
	return false;
}

std::string CAndroidHelper::FilterResult( const wchar_t* szCMD, std::string strResultMsg )
{
	std::string strResult;
	CStringA strBuffer = strResultMsg.c_str();
	if (strBuffer.Find("error:") == -1)
	{
		if(_tcsicmp(szCMD,CMD_GET_IMEI) == 0)
		{
			if(strBuffer.Find("Device ID =") >= 0)
			{
				//获取IMEI
				CStringA strIMEI = strBuffer.Mid(strBuffer.Find("Device ID = ") + strlen("Device ID = "));
				strIMEI.ReleaseBuffer();
				strResult = (LPCSTR)strIMEI;
			}
			else
			{
				strResult = W2Utf8(ERROR_INFO_GETIMEI_FAILED);
			}
		}
		else if(_tcsicmp(szCMD,CMD_GET_MODEL) == 0)
		{
			strResult = LPCSTR(strBuffer);
		}
		else if(_tcsicmp(szCMD,CMD_GET_VERSION) == 0)
		{
			strResult = LPCSTR(strBuffer);
		}
		else if(_tcsicmp(szCMD,CMD_GET_BRAND) == 0)
		{
			strResult = LPCSTR(strBuffer);
		}
		else if(_tcsicmp(szCMD,CMD_GET_MAC) == 0)
		{
			strResult = LPCSTR(strBuffer);
		}
		else
		{
			strResult = LPCSTR(strBuffer);
		}
	}
	else
	{
		strResult = LPCSTR(strBuffer);
	}
	return strResult;
}

std::string CAndroidHelper::ReadResponseFromPipe( HANDLE hStdOutRead )
{
	std::string strResult;
	char recvBuffer[ADB_BUFFER_SIZE] = {0};
	DWORD recvLen = 0;
	DWORD occupyLen = 0;

	for (;;)
	{
		recvLen = 0;
		if (!::PeekNamedPipe(hStdOutRead, NULL, 0, NULL,&recvLen, NULL))			// error
		{
			break;
		}
		if (recvLen > 0)					// not data available
		{
			if (ReadFile(hStdOutRead, recvBuffer, MAX_PATH, &recvLen, NULL))
			{
				strResult = recvBuffer;
			}
			break;
		}
	}
	return strResult;
}
