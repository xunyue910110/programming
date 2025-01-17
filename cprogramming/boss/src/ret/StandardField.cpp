// ##########################################
// Source file : StandardField.cpp
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20011025
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include <string.h>
#include "PublicFunction.h"
#include "StandardField.h"

//##ModelId=3BB415E7008D
//从配置文件中的<output_file>段中获得输出字段信息
bool StandardField::initialize(string theString)
{
	char *tok;
	char tmp[100];
	string theTmp;
	vector<string> tmpStrs,tmpStrs1;
	
	m_inputField.clear();
	m_isLeftFill = NO_FILL;
	
	// SOURCE_TYPE           , 2  ,source_type,L,\t
	splitStrByToken(theString,",",tmpStrs);

	if(tmpStrs.size() < 3)
	{
		// 小于3个分号的,不符合规则的书写格式
		return false;	
	}
	// get 'SOURCE_TYPE           , 2'
	m_outputField = tmpStrs[0];
	m_fieldLength = atoi( tmpStrs[1].c_str() );
	
	// get 'source_type'
	splitStrByToken(tmpStrs[2],"+",tmpStrs1);
	m_inputField = tmpStrs1;
    m_inFieldCount=m_inputField.size();
    
    if( tmpStrs.size() == 5 )
    {
    	// 获取左填充,还是右填充
    	m_isLeftFill = false;
    	if( tmpStrs[3] == "L")
    	{
    		m_isLeftFill = LEFT_FILL;    			
    	}
    	else
    	{
    		m_isLeftFill = RIGHT_FILL;	
    	}
    	
    	// 获取填充字符
    	char cTmp[10];
    	memset(m_fillChar,0,sizeof(m_fillChar));
    	memset(cTmp,0,sizeof(cTmp));
    	strcpy(cTmp,tmpStrs[4].c_str());
    	if( cTmp[0]==0x5c)  // "\"
    	{
	    	m_fillChar[0] = atoi(cTmp+1);
	    }
	    else
	    {
	    	m_fillChar[0] = cTmp[0];	
	    }
    }

	return true;
}

//##ModelId=3BB4195402D3
string StandardField::lrTrim(string TheString)
{
	int len,size;

	size	= TheString.size();
	len	= TheString.find_first_not_of(" \t");
	if( len > 0 )
		TheString	= TheString.substr(len,size-len);	// 去掉头部空格和Tab

	len	= TheString.find_last_not_of(" \t");
	if( len > 0 )
		TheString	= TheString.substr(0,len+1);		  // 去掉尾部空格和Tab

	return TheString;
}


