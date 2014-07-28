#include "StdAfx.h"
#include "AsyncHttpClient.h"
#include "Util.h"

#ifdef _WIN32
#include <WinBase.h>
#endif

CAsyncHttpClient::CAsyncHttpClient(void)
{
    m_handleCurl = NULL;
    m_bFollowLocation = true;
    m_receiveCallback = NULL;
    m_responseCallback = NULL;
    m_statusCode = 0;
    m_errCode = 0;
    m_requestMethod = EHttp_Get;
    m_httpHeaders = NULL;
    m_bNoBody = false;
    m_contentSize = 0;
    m_hRequestThread = NULL;
    m_recvUserData = NULL;
    m_respUserData = NULL;
}

CAsyncHttpClient::~CAsyncHttpClient(void)
{
    m_handleCurl = NULL;
    if (m_hRequestThread)
    {
        CloseHandle(m_hRequestThread);
        m_hRequestThread = NULL;
    }
    if (m_handleCurl)
    {
        curl_easy_cleanup(m_handleCurl);
        m_handleCurl = NULL;
    }
}

void CAsyncHttpClient::AddHeader(const std::wstring& strHeader)
{
    m_httpHeaders = curl_slist_append(m_httpHeaders, W2Utf8(strHeader.c_str()).c_str());
}

void CAsyncHttpClient::SetRecvCallback(OnReceiveCallback callback, void* userdata)
{
    m_receiveCallback = callback;
    m_recvUserData = userdata;
}

void CAsyncHttpClient::Request(const wchar_t* szUrl, EHttp_Request requestMethod, OnResponseCallback callback, void* userdata)
{
    m_requestMethod = requestMethod;
    m_responseCallback = callback;
    m_strUrl = szUrl;
    m_respUserData = userdata;

    m_hRequestThread = ::CreateThread(NULL, 0, &CAsyncHttpClient::StartRequest, this, 0, NULL);
}

bool CAsyncHttpClient::Init()
{
    CURLcode cCode = curl_global_init(CURL_GLOBAL_ALL);
    return CURLE_OK == cCode;
}

bool CAsyncHttpClient::UnInit()
{
    curl_global_cleanup();
    return true;
}

void CAsyncHttpClient::InnerRequest()
{
    m_handleCurl = curl_easy_init();
    if (m_handleCurl)
    {
        //初始化curl设置
        curl_easy_setopt(m_handleCurl, CURLOPT_URL, W2Utf8(m_strUrl.c_str()).c_str());
        if (m_requestMethod == EHttp_Post)
        {
            curl_easy_setopt(m_handleCurl, CURLOPT_POST, 1);
        }
        if (!m_strProxy.empty())
        {
            curl_easy_setopt(m_handleCurl, CURLOPT_PROXY, W2Utf8(m_strProxy.c_str()).c_str());
        }
        curl_easy_setopt(m_handleCurl, CURLOPT_WRITEFUNCTION, OnRecvData);
        curl_easy_setopt(m_handleCurl, CURLOPT_WRITEDATA, this);
        if (!m_strUseragent.empty())
        {
            curl_easy_setopt(m_handleCurl, CURLOPT_USERAGENT, W2Utf8(m_strUseragent.c_str()).c_str());
        }
        curl_easy_setopt(m_handleCurl, CURLOPT_FOLLOWLOCATION, int(m_bFollowLocation));
        curl_easy_setopt(m_handleCurl, CURLOPT_TIMEOUT, 30);
        if (m_httpHeaders != NULL)
        {
            curl_easy_setopt(m_handleCurl, CURLOPT_HTTPHEADER, m_httpHeaders);
        }
        if (m_bNoBody)
        {
            curl_easy_setopt(m_handleCurl, CURLOPT_NOBODY, 1);
        }
        if (CURLE_OK == curl_easy_perform(m_handleCurl))
        {
            //执行请求成功
            curl_easy_getinfo(m_handleCurl, CURLINFO_RESPONSE_CODE, &m_statusCode);
            curl_easy_getinfo(m_handleCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &m_contentSize);
        }
        else
        {
            m_errCode = -1;
        }

        //调用请求完成回调
        if (m_responseCallback)
        {
            m_responseCallback(m_statusCode, m_errCode, m_strResponse, m_respUserData);
        }

        //清理http头
        if (m_httpHeaders)
        {
            curl_slist_free_all(m_httpHeaders);
            m_httpHeaders = NULL;
        }
        curl_easy_cleanup(m_handleCurl);
        m_handleCurl = NULL;
    }
}

size_t CAsyncHttpClient::OnRecvData(void* buffer, size_t size, size_t nmemb, void* userp)
{
    CAsyncHttpClient* pClient = (CAsyncHttpClient*)userp;
    if (pClient != NULL)
    {
        pClient->DoRecvCallback(buffer, size * nmemb);
    }
    return size * nmemb;
}

void CAsyncHttpClient::SetUserAgent(const wchar_t* szUserAgent)
{
    m_strUseragent = szUserAgent;
}

void CAsyncHttpClient::SetFollowLocation(bool bFollow)
{
    m_bFollowLocation = bFollow;
}

void CAsyncHttpClient::DoRecvCallback(void* buffer, size_t size)
{
    if (buffer)
    {
        m_strResponse += (char*)buffer;
    }
    if (m_receiveCallback)
    {
        m_receiveCallback(m_statusCode, m_errCode, buffer, size, m_recvUserData);
    }
}

void CAsyncHttpClient::SetProxy(const wchar_t* szProxy)
{
    m_strProxy = szProxy;
}

void CAsyncHttpClient::SetNoBody(bool bNoBody)
{
    m_bNoBody = bNoBody;
}

DWORD CAsyncHttpClient::StartRequest(LPVOID param)
{
    CAsyncHttpClient* pClient = (CAsyncHttpClient*)param;
    if (pClient)
    {
        pClient->InnerRequest();
    }
    return 0;
}
