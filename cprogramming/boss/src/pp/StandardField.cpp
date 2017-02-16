// ##########################################
// Source file : StandardField.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################
#include <string.h>
#include "StandardField.h"
#include "FieldInfos.h"

//##ModelId=4852390D0128
bool StandardField::initialize(FieldInfos *pFieldInfos,const string &theString)
{
    char *tok;
	char tmp[100];
	vector<string> tmpStrs, tmpStrs1;
	
	m_inputFieldNames.clear();
	m_isLeftFill = NO_FILL;
	
	// SOURCE_TYPE           , 2  ,source_type,L,\t
	StringUtil::split(theString, ",", tmpStrs,true);

	if(tmpStrs.size() < 3) //2个分号3个字段
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	// get 'SOURCE_TYPE           , 2'
	m_outputFieldName = tmpStrs[0];
	m_fieldLength = atoi(tmpStrs[1].c_str());
	
	// get 'source_type'
	tmpStrs1.clear();
	StringUtil::split(tmpStrs[2], "+", tmpStrs1,true);
	m_inputFieldNames = tmpStrs1;
    m_inFieldCount = m_inputFieldNames.size();
    
    if(tmpStrs.size() == 5)
    {
    	// 获取左填充,还是右填充
    	m_isLeftFill = false;
    	if(tmpStrs[3] == "L")
    	{
    		m_isLeftFill = LEFT_FILL;    			
    	}
    	else
    	{
    		m_isLeftFill = RIGHT_FILL;	
    	}
    	
    	// 获取填充字符
    	char cTmp[10];
    	memset(m_fillChar, 0, sizeof(m_fillChar));
    	memset(cTmp, 0, sizeof(cTmp));
    	strcpy(cTmp, tmpStrs[4].c_str());
    	// "\"
    	if(cTmp[0] == 0x5c)
    	{
	    	m_fillChar[0] = atoi(cTmp + 1);
	    }
	    else
	    {
	    	m_fillChar[0] = cTmp[0];	
	    }
    }
    
    m_inPutFieldSeq.clear();
    for(int i=0;i<m_inFieldCount;++i)
   	{
   		m_inPutFieldSeq.push_back( (pFieldInfos->getSequnceByName(m_inputFieldNames[i].c_str())) );	
   	}
	return true;
}

StandardField &StandardField:: operator=(const StandardField &right)
{
	m_outputFieldName = right.m_outputFieldName;
	m_fieldLength = right.m_fieldLength;
	m_inputFieldNames   = right.m_inputFieldNames;
	m_inFieldCount = right.m_inFieldCount;
	m_isLeftFill   = right.m_isLeftFill;
	strcpy(m_fillChar,right.m_fillChar);
	return *this;
}
