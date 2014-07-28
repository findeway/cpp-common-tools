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
#include "Encode/Encode.h"

//
std::vector<std::wstring> SpliterString(const std::wstring& data,const std::wstring& spliter);
std::vector<std::string>  SpliterString(const std::string& data,const std::string& spliter);

std::wstring Utf82W_win32(LPCSTR szContent,int size);
std::string  W2Utf8_win32(LPCWSTR szContent,int size);

std::string W2Utf8_unix(const wchar_t* wsource);
std::wstring Utf82W_unix(const char* source);

#ifdef _WIN32
#define Utf82W Utf82W_win32
#define W2Utf8 W2Utf8_win32
#else
#define Utf82W Utf82W_unix
#define W2Utf8 W2Utf8_unix
#endif

