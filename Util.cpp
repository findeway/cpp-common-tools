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