/********************************************************************
	created:	2014/07/18
	created:	18:7:2014   15:15
	filename: 	Util.h
	file path:	
	file base:	Util
	file ext:	h
	author:		findeway
	
	purpose:	
*********************************************************************/
#pragma once
#include <string>
#include <vector>
#ifdef _WIN32
#include <WTypes.h>
#endif
//
std::vector<std::wstring> SpliterString(const std::wstring& data,const std::wstring& spliter);
std::vector<std::string>  SpliterString(const std::string& data,const std::string& spliter);

#ifdef _WIN32

std::wstring Utf82W_win32(const char* szContent,int size = -1);
std::string  W2Utf8_win32(const wchar_t* szContent,int size = -1);

#define Utf82W Utf82W_win32
#define W2Utf8 W2Utf8_win32

#else

#define Utf82W Utf82W_unix
#define W2Utf8 W2Utf8_unix

std::string W2Utf8_unix(const wchar_t* wsource);
std::wstring Utf82W_unix(const char* source);

#endif