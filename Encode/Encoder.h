/**
 * @file   Encoder.h
 * @author findeway <findewayz@gmail.com>
 * @date   Wed May 28 09:31:11 2014
 * 
 * @brief  各种编码转换
 * 
 * 
 */

#ifndef _ENCODER_H_
#define _ENCODER_H_
#include <string>

std::wstring urlencode(const std::wstring& original);
std::wstring urldecode(const std::wstring& original);

//16进制字符串转换
unsigned int hex2int(char c);
std::wstring hex2string(const char* hex);
std::wstring string2hex(const char* string);

//unicode编码
std::wstring escape(const std::wstring& source);
std::wstring unescape(const std::wstring& source);

//base64相关
std::string convert2base64(const std::string& sourcestring);
std::string convertfrombase64(const std::string& base64);

//md5相关
std::string MD5Hash(char *instr);

#endif /* _ENCODER_H_ */


















