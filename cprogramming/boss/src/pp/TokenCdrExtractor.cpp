// ##########################################
// Source file : TokenCdrExtractor.cpp
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

#include "TokenCdrExtractor.h"

//##ModelId=48522F7200AB
int TokenCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
    int cdrSize, controlCount;
    
    cdrSize = getcdrSize(m_BlockOffset + m_BlockBuffer, controlCount);
    
    if ((m_RealBlockLength - m_BlockOffset) < cdrSize)
    {
        return 0;                       //返回"0"表示本块内的记录已经取完。
    }
    if(cdrSize <= 0)
    {
        return 0;
    }

    if(cdrSize > m_iCdrBufferSize)
    {
		if( srcCdrBuffer != NULL)
			free(srcCdrBuffer);
		srcCdrBuffer = (unsigned char *)malloc(cdrSize);
		m_iCdrBufferSize = cdrSize;
    }
    
    memcpy(srcCdrBuffer, m_BlockOffset + m_BlockBuffer, cdrSize);

    m_BlockOffset = m_BlockOffset + cdrSize + controlCount  ;

    return cdrSize;                     // 返回原始记录长度
}

//##ModelId=48522F820138
bool TokenCdrExtractor::initialize(const string &theCdrParameter, const int theBlockHeaderSize)
{
    m_CdrSize           = atoi(theCdrParameter.c_str());                    // 字段数
    m_BlockHeaderSize   = theBlockHeaderSize;

    return true;
}

//##ModelId=48522F9300EA
void TokenCdrExtractor::setToken(const string token)
{
    char tmp[10];
    strcpy(tmp, token.c_str());
    m_token = tmp[0];
}

//##ModelId=48522FBD001F
int TokenCdrExtractor::getcdrSize(const unsigned char *srcCdrBuffer, int &controlCount)
{
    int i, j;

    for(i = 0; i < (m_RealBlockLength - m_BlockOffset); i++)
    {
        if (srcCdrBuffer[i] >= 0x81)
        {
            i++;                                                            // 出现乱码中文字符时,取两个字节
            continue;
        }
        else if (srcCdrBuffer[i] < 0x20 && srcCdrBuffer[i] != 13 && srcCdrBuffer[i] != 10 )           // 控制字符如果不是回车换行,则继续读取后面字节
        {
            continue;
        }
        else if (srcCdrBuffer[i] == 10 || srcCdrBuffer[i] == 13)                                     // 回车换行控制字符,即到行尾
        {
            break;
        }
    }
    for(j = 0; (j + i) < (m_RealBlockLength - m_BlockOffset); j++)
    {
        if(srcCdrBuffer[j+i] >= 0x20)                                       // 不是控制字符，得到控制字符个数
        {
            break;
        }
    }
    controlCount = j ;

    return i;
}

