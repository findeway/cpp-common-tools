#pragma once
#include "Singleton.h"
#include <SetupAPI.h>
#include <boost/function.hpp>

extern const wchar_t* CMD_GET_BRAND;
extern const wchar_t* CMD_GET_MODEL;
extern const wchar_t* CMD_GET_VERSION;
extern const wchar_t* CMD_GET_IMEI;
extern const wchar_t* CMD_GET_MAC;

extern const wchar_t* ERROR_INFO_GETIMEI_FAILED;
extern const wchar_t* ERROR_INFO_GETMODEL_FAILED;
extern const wchar_t* ERROR_INFO_GETVERSION_FAILED;
extern const wchar_t* ERROR_INFO_GETMAC_FAILED;
extern const wchar_t* ERROR_INFO_GETBRAND_FAILED;

typedef boost::function<void(int)> ProgressCallback;
class CAndroidHelper:
	public SohuTool::SingletonImpl<CAndroidHelper>
{
public:
	CAndroidHelper(void);
	~CAndroidHelper(void);

	/*
	 *	收到设备变更通知
	 */
	void NotifyDeviceChanged(DWORD wParam,DWORD lParam);

	/*
	 *	查找手机设备 
	 */
	bool SearchPhone(SP_DEVINFO_DATA* pDeviceInfo);

	/*
	 *	检查设备驱动是否正常 
	 */
	bool CheckDeviceDriver(const wchar_t* szInstanceId);

	/*
	 *	获取手机相关信息 
	 */
	void GetPhoneInfo();
	
	/*
	 *	向手机传送文件
	 */
	bool PushFile(const wchar_t* szSourcefile, const wchar_t* szDest, ProgressCallback callback);
protected:

	/*
	 *	获取设备相关ID信息
	 */
	std::wstring GetDeviceRegisterProperty(HDEVINFO hDevInfo,SP_DEVINFO_DATA deviceInfo,DWORD categoryId);

	/*
	 *	实例ID是电脑给设备分配的ID便于电脑对I/O设备的管理
	 */
	std::wstring GetDeviceInstanceId(HDEVINFO hDevInfo,SP_DEVINFO_DATA deviceInfo);

	/*
	 *	通过adb操作手机
	 *  _T("adb -d shell getprop ro.product.brand"); 		//获取厂商名称
	 *	_T("adb -d shell getprop ro.product.model")			//设备型号
	 *	_T("adb -d shell getprop ro.build.version.release")	//android版本
	 *	_T("adb -d shell dumpsys iphonesubinfo")			//IMEI码
	 *	_T("adb -d shell cat /sys/class/net/wlan0/address")	//MAC地址
	 */
	bool PostAdbCommand(const wchar_t* szCMD,std::string& strResult);

	/*
	 *	过滤adb的输出结果
	 */
	std::string FilterResult(const wchar_t* szCMD, std::string strResultMsg);

	/*
	 *	从adb管道读取结果
	 */
	std::string ReadResponseFromPipe(HANDLE hStdOutRead);
private:
	HDEVINFO			m_hDevInfo;
};
