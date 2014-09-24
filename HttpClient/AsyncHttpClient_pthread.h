#ifndef _ASYNCHTTPCLIENT_PTHREAD_H_
#define _ASYNCHTTPCLIENT_PTHREAD_H_

#include <curl/curl.h>
#include <pthread.h>
#include <string>

typedef void (*OnResponseCallback)(unsigned int statusCode, unsigned int errCode, const std::string& response, void* userdata);
typedef void (*OnReceiveCallback)(unsigned int statusCode, unsigned int errCode, void* buffer, size_t size, void* userdata);

enum EHttp_Request
{
    EHttp_Get = 0,
    EHttp_Post = 1
};

class CAsyncHttpClient
{
public:
    static bool Init();
    static bool UnInit();

    CAsyncHttpClient(void);
    ~CAsyncHttpClient(void);

    //设置一些请求选项
    void AddHeader(const std::wstring& strHeader);
    void SetUserAgent(const wchar_t* szUserAgent);
    void SetFollowLocation(bool bFollow);
    void SetProxy(const wchar_t* szProxy);

    /*
     *	不获取响应体，用于查询大文件的响应头信息
     */
    void SetNoBody(bool bNoBody);

    /*
     *	接收响应回调
     */
    void SetRecvCallback(OnReceiveCallback callback, void* userdata);

    /*
     *	异步开始请求
     */
    void Request(const wchar_t* szUrl, EHttp_Request requestMethod, OnResponseCallback callback, void* userdata);

    /*
     *	执行接收数据回调
     */
    void DoRecvCallback(void* buffer, size_t size);

    void InnerRequest();
protected:
    static void* StartRequest(void* param);
    static size_t OnRecvData(void* buffer, size_t size, size_t nmemb, void* userp);

protected:
    CURL*						m_handleCurl;
    OnResponseCallback			m_responseCallback;				//请求完成回调
    OnReceiveCallback			m_receiveCallback;				//接收数据回调

    std::wstring				m_strUseragent;
    bool						m_bFollowLocation;
    std::wstring				m_strProxy;
    std::wstring				m_strUrl;
    EHttp_Request				m_requestMethod;
    curl_slist*					m_httpHeaders;
    bool						m_bNoBody;

    unsigned int				m_statusCode;
    unsigned int				m_errCode;
    long long					m_contentSize;
    std::string					m_strResponse;

    void*						m_recvUserData;
    void*						m_respUserData;

    pthread_t					m_hRequestThread;
};

#endif /* _ASYNCHTTPCLIENT_PTHREAD_H_ */
