// ##########################################
// Source file : AsnFormat.cpp
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

#include "AsnFormat.h"
#include "FieldInfo.h"
#include <math.h>
extern "C++" double pow(double,int); //added by zhongh 2011/1/18 11:45:13
//##ModelId=4855C00403A9
AsnFormat::AsnFormat()
{
    m_FormatType = 1;
}

//##ModelId=4855C0070138
AsnFormat::~AsnFormat()
{
    vector <RecordFormat *> :: iterator iter;

    while(m_RecordFormatPointer.size())
    {
        iter = m_RecordFormatPointer.end() - 1;
        m_RecordFormatPointer.pop_back();
        delete *iter;
    }
}

//##ModelId=4855C01D030D
void AsnFormat::getContents()
{
}

//##ModelId=4855C03801A5
int AsnFormat::getCdrSize(const char *theSign, const char *cdrTmp, int &aBlockOffset)
{
    int asnTagSize, lengthSize, asnCdrSize;
    unsigned char tmp[10], tagBuffer[4];

    memcpy(tmp, (unsigned char*)cdrTmp, 8);

    unsigned const char *tmp1;
    tmp1 = (unsigned char *)cdrTmp;

    asnTagSize   = getAsnTag(tmp, tagBuffer);
    lengthSize   = getAsnLength(tmp1 + asnTagSize, asnCdrSize);
    aBlockOffset = aBlockOffset + lengthSize + asnTagSize;

    return asnCdrSize;
}

//##ModelId=4855C0660138
int AsnFormat::getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll)
{
    int count = 0;
    int returnValue = 0;
    unsigned char signTmp[10];
    int tagSize, valueSize, valueLength;

    while(m_subOffset < m_cdrSize)
    {
    	  tagSize = getAsnTag((unsigned char *)m_CdrBuffer + m_subOffset + m_bufferOffset, signTmp);
        theLength = theLength - tagSize;
        m_subOffset += tagSize;                 //skip tag
        valueLength = getAsnLength(m_CdrBuffer + m_subOffset + m_bufferOffset, valueSize);
        if(valueLength > theLength)
        {
		       valueLength = 1;
		       valueSize = theLength - 1;
        }
        m_subOffset += valueLength;             //skip value size's length

        if( valueSize > (m_cdrSize - m_subOffset)|| valueSize<0)
        {
            theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
                                 m_PpLog_->getSrcFileName() + " 字段值长度超过文件长度错误!");
            return -1;
        }

        if( memcmp(theSign, signTmp, tagSize) == 0)
        {
            if(valueSize < 0)
            {
                theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
                                 m_PpLog_->getSrcFileName() + " 字段值长度错误!");
                return -1;
            }
            // added by zaiww 2009-09-25 tagSize/valueSize都有值,且valueSize为0
            if(valueSize == 0)
            {
                tagAll=true;
            }
            if(count == position)
            {
                pFieldInfo->setSrcValue(m_CdrBuffer + m_bufferOffset + m_subOffset, valueSize);
                m_subOffset += valueSize;       //skip value
                returnValue = valueSize;
                break;
            }
            count++;
        }
        else
        {
            returnValue = 0;
        }

        m_subOffset += valueSize;               //skip value
        
    }

    return returnValue;
}

//##ModelId=4855C0A6032C
void AsnFormat::initialize()
{
    for(int i = 0; i < 3; i++)
    {
        m_RecordFormatPointer.push_back(new AsnFormat);
    }
}

//##ModelId=4855C0C001F4
int AsnFormat::getAsnLength(const unsigned char *lengthBuffer, int &aAsnCdrSize)
{
    int lengthSize;

    aAsnCdrSize = 0;
    if(lengthBuffer[0] > 128)
    {
        lengthSize = lengthBuffer[0] & 127;
        for(int i = 1; i <= lengthSize; i++)
        {
            aAsnCdrSize += (int)((double)lengthBuffer[i] * (pow((double)256, (lengthSize - i))));
        }
        lengthSize++;
    }
    // 对于asn.1标准的不定长形式
    // 由一个字节组成的LENGTH取值为10000000B。CONTENTS的长度不确定，其结束标志为 end-of-contents 字节（由2个值为零的字节组成）
    else if( lengthBuffer[0] == 128 )
    {
        aAsnCdrSize = getZeroLen(lengthBuffer);
        return 1;
    }
    else
    {
        aAsnCdrSize = lengthBuffer[0];
        lengthSize  = 1;
    }

    return lengthSize;
}

//##ModelId=4855C0DF031C
int AsnFormat::getAsnTag(const unsigned char *buffer, unsigned char *aTag)
{
    int i;
    unsigned char tmp;

    tmp = buffer[0];
    tmp &= 31;            // 31 is B11111

    if(tmp == 31)
    {
        // 处理两个以上BYTE的TAG值
        int j = 2;
        while( buffer[j-1]  >= 128 )
        {
            ++j;
        }
        i = j;
    }
    else
    {
        i = 1;
    }

    memcpy(aTag,buffer,i);

    return i;
}

//##ModelId=4855C106000F
int AsnFormat::getZeroLen(const unsigned char * buffer)
{
    char tmp[80];
    char ans_end[2];
    int length, len, len1, buffer_len;
    unsigned char *buff;
    unsigned char aTag[10];

    memset(ans_end, 0, 2);

    length = 0;
    buff = (unsigned char *)buffer + 1;
    buffer_len = strlen((char*)buff);
    while(1)
    {
        if(memcmp( buff, ans_end, 2) == 0)
        {
            length += 2;
            break;
        }
        else
        {
            len = getAsnTag(buff, aTag);
            buff += len;
            length += len;

            len1 = getAsnLength(buff, len);
            len += len1;
            length += len;

            buff += len;
        }
    }
    return length;
}

//##ModelId=4855C11A007D
bool AsnFormat::isCompositeTag(int tag)
{
    if(((tag&32) >> 5) == 1)
    {
        return true;            // tag第3位是1,是复合字段
    }
    else
    {
        return false;           // tag第3位是1,不是复合字段
    }
}

