// ##########################################
// Source file : GAsnCdrExtractor.cpp
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

#include "GAsnCdrExtractor.h"

//##ModelId=3B5152020084
int GAsnCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
	char tmp2[10];
	unsigned char *tmp1;
	int i,j,theCdrSize;
	bool isMatch = false;

	// 如果是华为gprs,则需要跳过每个cdr开始的4个字节
	if( m_isHuawGprs )
	{
		m_BlockOffset+=4;	
	}
	
	// 定位到tag标志位。用于在话单存储异常时可以自行寻找tag码。
	for ( i=0 ; i<(m_RealBlockLength - m_BlockOffset) ; i++ )
	{
		tmp1 = m_BlockBuffer + m_BlockOffset + i;
		for(j=0;j<m_nTags.size();j++)
		{
			if ( tmp1[0]==m_nTags[j] )		// is cdr tag
			{
				isMatch = true;
				break;
			}
		}
		if( isMatch == true )
		{
			break;
		}
	}
	m_BlockOffset += i;

	if ( m_BlockOffset >= m_RealBlockLength )
		return 0;		//返回"0"表示本块内的记录已经取完。

	int iBlockOffset;
	iBlockOffset = m_BlockOffset;
	theCdrSize = theRecordFormat->getCdrSize(m_Tags[j].c_str(),(char *)tmp1,iBlockOffset);
	if( theCdrSize<=0 || ((theCdrSize+iBlockOffset) > m_RealBlockLength) )   //modified by chenmin 2002-10-12 17:19
	{
	   theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, ("文件") + m_RetLog_->getSrcFileName()
	                            + " 话单记录长度错误!");
	   return 0;
	}
	// 需要把话单的起始tag和lenth都取出来,by chenm 2005-3-18
	theCdrSize+=iBlockOffset-m_BlockOffset;
	memcpy( srcCdrBuffer,m_BlockBuffer+m_BlockOffset ,theCdrSize );
	m_BlockOffset += theCdrSize;

	
	return theCdrSize;		// 返回原始记录长度
}

//##ModelId=3B8209FE0181
bool GAsnCdrExtractor::initialize(const string theCdrParameter,const int theBlockHeaderSize)
{
	vector<string> tmpStrs;
	int tagSize,nTag;
	char *ep;
	
	m_isHuawGprs = false;
	// 华为gprs没个cdr前面有4个字节,标识长度,需要把这四个字节跳过去 2005-11-17
	if( theBlockHeaderSize == 11111111)
	{
		m_BlockOffset = 0;
		m_BlockHeaderSize = 0;
		m_isHuawGprs  = true;
	}
	else
	{
		m_BlockHeaderSize 	= theBlockHeaderSize;
		m_BlockOffset		= m_BlockHeaderSize;
	}
	// 提取由','分割的cdr起始 tag
	splitStrByToken(theCdrParameter,",",tmpStrs);
	for(int i=0;i<tmpStrs.size();i++)
	{
		// tag码必须是2的整数倍
		tagSize=tmpStrs[i].size();
		if ( (tagSize % 2) != 0 )
		{
			theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "the GAsn cdr's tag size error!");
			return false;
		}
		
		nTag=strtol(tmpStrs[i].c_str(),&ep,16);
		m_nTags.push_back(nTag);
		m_Tags.push_back(tmpStrs[i]);	
	}
	
	theRecordFormat	= new AsnFormat;
	
	if(theRecordFormat==0) 
	{
	   theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create theRecordFormat object!");
	   return false;
	}
	return true;
}

//##ModelId=3B97358703D6
GAsnCdrExtractor::~GAsnCdrExtractor()
{
	delete theRecordFormat;
}

