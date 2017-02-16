// ##########################################
// Source file : PublicFunction.h
// System      : BOSS
// Version     : 1.5
// Language	   : ANSI C++
// OS Platform : UX
// Authors     : CHEN MIN
// E_mail      : chenm@lianchuang.com
// Create      : 2005-3-14 
// Update      : 2005-3-14
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef PUBLICFUNCTION_H
#define PUBLICFUNCTION_H

#include "config-all.h"
#include "base/StringUtil.h"
#include <string>
#include <vector>

USING_NAMESPACE(std)

// 功能：是将一个由符号分隔的string按顺序转换为一个vector<string>
// 输入：被转换的字符串 inPutStr,符号分割符 token
// 输出：按照分隔符提取转换后的 vector<string>
void splitStrByToken(const string inPutStr,const char* token,vector<string> &outPutStrs);

class MyString
{
	public:
		char m_pStr[32];
		MyString(const char * pStr)
		{
			strcpy(m_pStr,pStr);	
		}
		friend bool operator < (const MyString &left,const MyString &right)
		{
			return (strcmp(left.m_pStr,right.m_pStr)<0);	
		}
};

#endif
