// ##########################################
// Source file : PublicFunction.cpp
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

#include "PublicFunction.h"
#include "base/StringUtil.h"

// 功能：是将一个由符号分隔的string按顺序转换为一个vector<string>
// 输入：被转换的字符串 inPutStr,符号分割符 token
// 输出：按照分隔符提取转换后的 vector<string>
void splitStrByToken(const string inPutStr,const char * token,vector<string> &outPutStrs)
{
	char * buffer = 0;
	int iStrLenth = 0;
	string tmpStr;
	
	iStrLenth = inPutStr.length();
	buffer = new char[iStrLenth+1];
	memset(buffer,0,iStrLenth+1);
	strcpy(buffer,inPutStr.c_str());
	
	outPutStrs.clear();
	for(char *p = strtok(buffer, token); p != NULL; p = strtok(NULL, token))
    {
    	tmpStr = p;
        StringUtil::trim(tmpStr);
    	outPutStrs.push_back(tmpStr);
    }
    delete buffer;
}

