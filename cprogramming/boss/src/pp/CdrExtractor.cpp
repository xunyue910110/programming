// ##########################################
// Source file : CdrExtractor.cpp
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

#include "CdrExtractor.h"
#include "PpLog.h"

//##ModelId=48522A4600DA
CdrExtractor::CdrExtractor()
{
    m_PpLog_ = PpLog::getInstance();
    m_iCdrBufferSize = 0;
}

//##ModelId=48522A49004E
CdrExtractor::~CdrExtractor()
{
}

//##ModelId=48522A8600AB
bool CdrExtractor::setBlockBuffer(const unsigned char *BlockBuffer, int RealBlockLength)
{
    m_RealBlockLength = RealBlockLength;
    m_BlockBuffer     = (unsigned char *)BlockBuffer;
    m_BlockOffset     = m_BlockHeaderSize;

    return true;
}


