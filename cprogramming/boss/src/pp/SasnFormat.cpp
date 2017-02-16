// ##########################################
// Source file : SasnFormat.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include <math.h>
#include "SasnFormat.h"
#include "FieldInfo.h"

//##ModelId=3B97195503D7
SasnFormat::SasnFormat()
{
   m_FormatType = 1;
}

//##ModelId=3B978CA6010F
SasnFormat::~SasnFormat()
{
	vector <RecordFormat *> :: iterator iter;

    while(m_RecordFormatPointer.size())
    {
    	iter = m_RecordFormatPointer.end() - 1;
    	m_RecordFormatPointer.pop_back();
    	delete *iter;
    }
}

//##ModelId=3B51546C03C1
void SasnFormat::getContents()
{
}

//##ModelId=3B84FD0C004A
int SasnFormat::getCdrSize(const char *theSign, const char *cdrTmp, int &aBlockOffset)
{
	int asnTagSize, lengthSize, asnCdrSize, valueSize;
	unsigned char tmp[10],tagBuffer[4];

	memcpy(tmp, (unsigned char*)cdrTmp, 8);
	
	unsigned const char *tmp1;
	tmp1 = (unsigned char *)cdrTmp;
	
	asnTagSize		= getAsnTag(tmp,tagBuffer);
	lengthSize		= getAsnLength(tmp1, asnCdrSize, valueSize);
	aBlockOffset	= aBlockOffset;

	return asnCdrSize;
}

//##ModelId=3B85155A0137
int SasnFormat::getAsnLength(const unsigned char *lengthBuffer,int &aAsnCdrSize, int &aValueSize)
{
	int lengthSize;
	char *lenBuffer1, *lenBuffer2, *lenBuffer;

    if (lengthBuffer[0] == 132)
    {
        lengthSize = 2;
        
        lenBuffer1 = new char[10];
        lenBuffer2 = new char[10];
        lenBuffer  = new char[10];
        sprintf(lenBuffer1, "%x", lengthBuffer[1]);
        sprintf(lenBuffer2, "%x", lengthBuffer[2]);
        strcpy(lenBuffer, lenBuffer2);
        strcat(lenBuffer, lenBuffer1);
        aAsnCdrSize	= strtol(lenBuffer, NULL, 16);
        delete lenBuffer1;
        delete lenBuffer2;
        delete lenBuffer ;
        aValueSize = 5;
    }
    else if (lengthBuffer[0] == 103)
    {
        lengthSize = 0;
        aValueSize = 3;
    }
    else if (lengthBuffer[0] == 110)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 116)
    {
        lengthSize = 0;
        aValueSize = 7;
    }
    else if (lengthBuffer[0] == 117)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 122)
    {
        lengthSize = 0;
        aValueSize = 2;
    }
    else if (lengthBuffer[0] == 130)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 142)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 168)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 176)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }
    else if (lengthBuffer[0] == 198)
    {
        lengthSize = 1;
        aValueSize = lengthBuffer[1] - 2;
    }

	return lengthSize;
}

//##ModelId=3C047E8700B1
int SasnFormat::getAsnTag(const unsigned char *buffer, unsigned char *aTag)
{
	int i;
	unsigned char tmp;

	tmp = buffer[0];
	tmp &= 31;			// 31 is B11111

	if(tmp == 31)
	{
	    // 处理两个以上BYTE的TAG值
	    int j = 2;
	    while(buffer[j - 1]  >= 128)
	    {
	    	++j;	
	    }
	    i = j;
	}
	else
	{
		i = 1;
	}

	memcpy(aTag, buffer, i);

	return i;
}

//##ModelId=3B94E59900C6
int SasnFormat::getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll)
{
    int count = 0;
	int returnValue = 0;
	unsigned char signTmp[10];
	int tagSize, valueSize, valueLength;

	while(m_subOffset < m_cdrSize)
	
	{
		tagSize = getAsnTag((unsigned char *)m_CdrBuffer + m_subOffset + m_bufferOffset, signTmp);

		valueLength  = getAsnLength(m_CdrBuffer + m_subOffset + m_bufferOffset, m_cdrSize, valueSize);
		
		m_subOffset += tagSize;
		m_subOffset += valueLength;		    //skip value size's length

		if( valueSize > m_cdrSize || valueSize < 0)
		{		    
			theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
	                             m_PpLog_->getSrcFileName() + " 字段值长度超过文件长度错误!");
			return -1;
			
		}

		if(memcmp(theSign, signTmp, tagSize) == 0) 
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
			if (count == position) 
			{
			    pFieldInfo->setSrcValue(m_CdrBuffer + m_bufferOffset + m_subOffset, valueSize);
			    m_subOffset += valueSize;
			    returnValue  = valueSize;
			    break;
			}
			count++;
		} 
		else 
		{
			returnValue = 0;
		}
		m_subOffset += valueSize;	   //skip value
	}

	return returnValue;
}

//##ModelId=3B95E83C0203
bool SasnFormat::isCompositeTag(int tag)
{
	if( ((tag&32) >> 5 ) == 1)
	{
		return true;			// tag第3位是1,是复合字段
	}
	else
	{
		return false;			// tag第3位是1,不是复合字段
	}
}

//##ModelId=3B96E434014E
void SasnFormat::initialize()
{
	for(int i = 0; i < 3; i++)
	{
		m_RecordFormatPointer.push_back(new SasnFormat);
	}
}

