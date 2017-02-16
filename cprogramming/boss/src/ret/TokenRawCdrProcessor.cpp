// ##########################################
// Source file : TokenRawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20020112
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "TokenRawCdrProcessor.h"

//##ModelId=3B60214600F8
int TokenRawCdrProcessor::getDesCdr(const unsigned char *aCdrValue,int length,
						      multimap<MyString,FieldInfo *> &m_theFieldInfoMaps)
{
	if(!m_theCompositeField.extract(aCdrValue,length,m_theFieldInfoMaps,*m_theRecordFormat))
	   return -1;
	else
	   return 0;
}

//##ModelId=3B66B74F0002
int TokenRawCdrProcessor::getCdrSign()
{
	return 0;
}


//##ModelId=3B96EBE500C4
bool TokenRawCdrProcessor::initialize( vector<string> aParameterList)
{
	return RawCdrProcessor::initialize(aParameterList);
}




