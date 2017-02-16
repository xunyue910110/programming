// ##########################################
// Source file : AsnCdrExtractor.cpp
// System      : Mobile Billing System
// Version     : 0.1.17
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020108
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "AsnCdrExtractor.h"

//##ModelId=3B5152020084
int AsnCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
	char *ep;
	char tmp2[10];
	unsigned char *tmp1;
	int i,tagSize,theCdrSize,nTag;

	tagSize=m_Tag.size();
	nTag=strtol(m_Tag.c_str(),&ep,16);

	if ( (tagSize % 2) != 0 )
		return 0;

	// 定位到tag标志位。用于在话单存储异常时可以自行寻找tag码。
	for ( i=0 ; i<(m_RealBlockLength - m_BlockOffset) ; i++ )
	{
		tmp1 = m_BlockBuffer + m_BlockOffset + i;
		if ( tmp1[0]==nTag )		// is cdr tag
			break;
	}
	m_BlockOffset += i;

	if ( m_BlockOffset >= m_RealBlockLength )
		return 0;		//返回"0"表示本块内的记录已经取完。

	theCdrSize = theRecordFormat->getCdrSize(m_Tag.c_str(),(char *)tmp1,m_BlockOffset);
	if( theCdrSize<=0 || (theCdrSize > (m_RealBlockLength-m_BlockOffset)) )   //modified by chenmin 2005-6-21
	{
	   theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, ("文件") + m_RetLog_->getSrcFileName()
	                            + " 话单记录长度错误!");
	   return 0;
	}

	memcpy( srcCdrBuffer,m_BlockBuffer+m_BlockOffset ,theCdrSize );
	m_BlockOffset += theCdrSize;

	return theCdrSize;		// 返回原始记录长度
}

//##ModelId=3B8209FE0181
bool AsnCdrExtractor::initialize(const string theCdrParameter,const int theBlockHeaderSize)
{
	m_BlockHeaderSize 	= theBlockHeaderSize;
	m_BlockOffset		   = m_BlockHeaderSize;
	m_Tag				      = theCdrParameter;
	theRecordFormat		= new AsnFormat;
	if(theRecordFormat==0) 
	{
	   theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create theRecordFormat object!");
	   return false;
	}
	return true;
}

//##ModelId=3B97358703D6
AsnCdrExtractor::~AsnCdrExtractor()
{
	delete theRecordFormat;
}

