// ##########################################
// Source file : FixedCdrExtractor.cpp
// System      : Mobile Billing System
// Version     : 0.1.10
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020410
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FixedCdrExtractor.h"

//##ModelId=3B51520D03AB
int FixedCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
	int cdrSize;
	
	if(m_CdrSize==0) 
	{
	   cdrSize=getcdrSize(m_BlockOffset+m_BlockBuffer);
	} 
	else 
	{
	   cdrSize=m_CdrSize;
	}

	if ( (m_RealBlockLength - m_BlockOffset) < cdrSize ) 
	{
		return 0;			//返回"0"表示本块内的记录已经取完。
	} 
	else if ( cdrSize<=0 ) 
	{
		return 0;			//返回"0"表示本块内的记录已经取完。
	}

	memcpy( srcCdrBuffer,m_BlockOffset+m_BlockBuffer ,cdrSize );
	m_BlockOffset+=cdrSize;

	return cdrSize;		// 返回原始记录长度
}

//##ModelId=3BFE0CED02DD
int FixedCdrExtractor::getcdrSize(const unsigned char *srcCdrBuffer)
{
	string tmp;
	
	if (  m_dataType.compare(1,3,"BCD") == 0 ) 
	{
	   m_DataFormat.getFmtValue(tmp,srcCdrBuffer+m_StartByte,m_Length,m_padding,m_dataType);
	} 
	else if ( (m_dataType).compare("Binary") == 0 ) 
	{
		m_DataFormat.getFmtValue(tmp,srcCdrBuffer+m_StartByte,m_Length,m_order);
	} 
	else if ( (m_dataType).compare("HEX")==0) 
	{
		m_DataFormat.getFmtValue(tmp,srcCdrBuffer+m_StartByte,m_Length,(char *)"0",0);
	} 
	else if ( (m_dataType).compare("Ascii")==0) 
	{
		m_DataFormat.getFmtValue(tmp,srcCdrBuffer+m_StartByte,m_Length,(char *)"1",0);
	}

    return atoi(tmp.c_str());
}

//##ModelId=3B8209FF020F
bool FixedCdrExtractor::initialize(const string theCdrParameter,const int theBlockHeaderSize)
{
	// 参数theCdrParameter有两种可能：1. cdr长度；2. 长度表达式(例："FIX,0,0,16,binary,1")
	// 第一种是定长，第二种是变长，实际长度从原始话单文件中读取，读取方法如表达式指定。
	char *tok;
	char tmp[80];
	
	m_BlockOffset	    = m_BlockHeaderSize;
	m_BlockHeaderSize   = theBlockHeaderSize;
	
	if(theCdrParameter.compare(0,3,"FIX")==0) 
	{
    	m_CdrSize   = 0;
    	// example : tmp is "0,0,16,binary,1"
    	strcpy(tmp,theCdrParameter.c_str());

		strtok( tmp , "," )	;
		tok	= strtok(NULL,",");
		if (tok!=NULL)
			m_StartByte	= atoi( tok )	;	            //得到开始字节
		tok	= strtok(NULL,",");                         //跳过开始比特
		tok	= strtok(NULL,",");
		if (tok!=NULL)
			m_Length	= atoi( tok ) / 8	;	         //得到字段长度
		tok	= strtok(NULL,",");
		if (tok!=NULL) 
		{
			m_dataType		= tok			;	         //得到数据类型
			if(m_dataType.substr(1,3)=="BCD") 
			{
				tok	= strtok(NULL,",");
				if ( tok!=NULL && strlen(tok)==1 )
				{
					m_padding = tok[0];			         //得到BCD填充字符
				}
			} 
			else if (m_dataType=="Binary") 
			{
				tok	= strtok(NULL,",");
				if(tok!=NULL)
				{
					m_order	= atoi( tok )	;	         //得到高低位交换标志
				}
			}
		}
    } 
    else 
    {
    	m_CdrSize 	= atoi( theCdrParameter.c_str() );
    }

	if ( m_CdrSize >= 0 ) 
	{
		return true;
	} 
	else 
	{
	    theErrorMessages->insert(PP_ERROR_PARAMETER, "cdr size error!");
		return false;
	}
}

