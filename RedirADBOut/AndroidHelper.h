/********************************************************************
	created:	2014/07/18
	created:	18:7:2014   13:46
	filename: 	AndroidHelper.h
	file path:	
	file base:	AndroidHelper
	file ext:	h
	author:		findeway
	
	purpose:	
*********************************************************************/
#pragma once
#include "Singleton.h"
#include <SetupAPI.h>
#include <boost/function.hpp>
#include <map>

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
typedef boost::function<void(bool)> ConnectCallback;

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

	/*
	 *	通过adb操作手机
	 *  _T("adb -d shell getprop ro.product.brand"); 		//获取厂商名称
	 *	_T("adb -d shell getprop ro.product.model")			//设备型号
	 *	_T("adb -d shell getprop ro.build.version.release")	//android版本
	 *	_T("adb -d shell dumpsys iphonesubinfo")			//IMEI码
	 *	_T("adb -d shell cat /sys/class/net/wlan0/address")	//MAC地址
	 */
	bool PostAdbCommand(const wchar_t* szCMD,std::string& strResult,bool bWaitResult = true);

	/*
	 *	获取手机存储目录
	 */
	std::wstring GetStorageDir();
	
	/*
	 *	注册手机登陆状态变化的回调函数
	 */
	int RegisterConnectCallback(ConnectCallback callback);

	/*
	 *	解注册手机登陆状态变化的回调函数
	 */
	bool UnRegisterConnectCallback(int id);

protected:

	/*
	 *	更新手机连接状态
	 */
	void UpdateConnectionState();
	/*
	 *	获取设备相关ID信息
	 */
	std::wstring GetDeviceRegisterProperty(HDEVINFO hDevInfo,SP_DEVINFO_DATA deviceInfo,DWORD categoryId);

	/*
	 *	实例ID是电脑给设备分配的ID便于电脑对I/O设备的管理
	 */
	std::wstring GetDeviceInstanceId(HDEVINFO hDevInfo,SP_DEVINFO_DATA deviceInfo);

	/*
	 *	过滤adb的输出结果
	 */
	std::string FilterResult(const wchar_t* szCMD, std::string strResultMsg);

	/*
	 * 过滤掉输出结果中的无用信息，如重启输出等	
	 */
	std::string FilterUselessMsg(std::string strResultMsg);

	/*
	 *	从adb管道读取结果
	 */
	std::string ReadResponseFromPipe(HANDLE hStdOutRead,const wchar_t* szCMD);

	/*
	 *	判断是否是耗时操作
	 */
	bool NeedWaitProcess(const wchar_t* szCMD);

	/*
	 *	通知耗时操作执行的百分比
	 */
	void NotifyProgress(float fPercent, const wchar_t* szCMD);

	/*
	 *	通知手机连接状态变化
	 */
	void NotifyConnect(bool bConnect);

	//************************************
	// FullName:  CAndroidHelper::UpdateProgress
	// Access:    protected 
	// Returns:   bool
	// Parameter: const std::string & strMsg
	// Parameter: const wchar_t * szCMD
	// Desc:	  获取耗时操作进度 called from ReadResponseFromPipe
	// Return:	  true-处理完毕 false-未处理完毕
	//************************************
	bool UpdateProgress(const std::string& strMsg, const wchar_t* szCMD);

	std::map<std::wstring,ProgressCallback>  m_mapProgressCallback;
	std::map<int,ConnectCallback>			 m_mapConnectCallback;
private:
	HDEVINFO			m_hDevInfo;
	bool				m_bDeviceConnected;
	std::wstring		m_strStorageDir;
};
