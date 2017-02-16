// ##########################################
// Source file : CdrExtractor
// System      : Mobile Billing System
// Version     : 1.02
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010827
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "CdrExtractor.h"

//##ModelId=3B846DDD0075
bool CdrExtractor::setBlockBuffer(const unsigned char *BlockBuffer,int RealBlockLength)
{
	m_RealBlockLength	= RealBlockLength;
	m_BlockBuffer		= (unsigned char *)BlockBuffer;
	m_BlockOffset 		= m_BlockHeaderSize;

	return true;
}

//##ModelId=3B84F1AF02DC
CdrExtractor::~CdrExtractor()
{

}


//##ModelId=3C3A5AFF032A
CdrExtractor::CdrExtractor()
{
    m_RetLog_	= RetLog::getInstance();
}

