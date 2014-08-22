/********************************************************************
	created:	2014/07/18
	created:	18:7:2014   15:16
	filename: 	Util.cpp
	file path:	
	file base:	Util
	file ext:	cpp
	author:		findeway
	
	purpose:	
*********************************************************************/
#include "Util.h"
#include <stdlib.h>

std::vector<std::wstring> SpliterString(const std::wstring& data,const std::wstring& spliter)
{
	size_t pos,start = 0;
	std::vector<std::wstring> strs;
	while ((pos = data.find(spliter,start)) != std::wstring::npos)
	{
		strs.push_back(data.substr(start,pos-start));
		start = pos + spliter.size();
	}
	if(start < data.size())
		strs.push_back(data.substr(start));
	return strs;
}

std::vector<std::string> SpliterString(const std::string& data,const std::string& spliter)
{	
	size_t pos,start = 0;
	std::vector<std::string> strs;
	while ((pos = data.find(spliter,start)) != std::string::npos)
	{
		strs.push_back(data.substr(start,pos-start));
		start = pos + spliter.size();
	}
	if(start < data.size())
		strs.push_back(data.substr(start));
	return strs;
}

#ifdef _WIN32
std::wstring Utf82W_win32(const char* szContent,int size)
{
	 std::wstring strContent;
	 int len = MultiByteToWideChar(CP_UTF8,0,szContent,size,NULL,0);
	 strContent.resize(len);
	 MultiByteToWideChar(CP_UTF8,0,szContent,size,(wchar_t*)strContent.c_str(),len);
	 if(len > 0 && size == -1)
		 strContent.resize(len-1);
	 return strContent;
}

std::string  W2Utf8_win32(const wchar_t* szContent,int size)
{
	 std::string strContent;
	 int len = WideCharToMultiByte(CP_UTF8,0,szContent,size,NULL,0,NULL,NULL);
	 strContent.resize(len);
	 WideCharToMultiByte(CP_UTF8,0,szContent,size,(char *)strContent.c_str(),len,NULL,NULL);
	 if(len > 0 && size == -1)
		 strContent.resize(len-1);
	 return strContent;
}
#else
size_t mbs2wcs(wchar_t *dest, size_t dest_size,const char *src, size_t src_size)
{
    const char* current_locale= setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    size_t count=mbstowcs(NULL,src,src_size);
    size_t ret=0;
    if(dest_size<count)
    {
        ret=-1;
    }
    else
    {
        ret=mbstowcs(dest,src,count);
        if(ret==count)
        {
            ret =-1;
        }
    }
    setlocale(LC_ALL, current_locale);
    return ret;
}

size_t wcs2mbs(char *dest,size_t dest_size,const wchar_t *src, size_t src_size)
{
    const char* current_locale= setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    size_t ret=0;
    size_t count=wcstombs(NULL,src,src_size);
    if(dest_size<count)
    {
        ret=-1;
    }
    else
    {
        ret=wcstombs(dest,src,count);
        if(ret==count)
        {
            ret =-1;
        }
    }
    setlocale(LC_ALL, current_locale);
    return ret;
}


std::string W2Utf8_unix(const wchar_t* wsource)
{
	const wchar_t* _Source = wsource;
    size_t _Dsize = 2 * wcslen(wsource) + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
	std::string result;
	if(wcstombs(_Dest,_Source,_Dsize) > 0)
	{
		result = _Dest;
	}
    delete []_Dest;	
	return result;
}

std::wstring Utf82W_unix(const char* source)
{
	const char* _Source = source;
    size_t _Dsize = strlen(source) + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
	std::wstring result;
    if(mbstowcs(_Dest,_Source,_Dsize) > 0)
	{
		result = _Dest;
	}
    delete []_Dest;
	return result;
}
#endif