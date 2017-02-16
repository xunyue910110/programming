// ##########################################
// Source file : AsnFormat.cpp
// System      : Mobile Billing System
// Version     : 0.1.14
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <math.h>

#include "AsnFormat.h"

//##ModelId=3B97195503D7
AsnFormat::AsnFormat()
{
   m_FormatType=1;
}

//##ModelId=3B978CA6010F
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

//##ModelId=3B51546C03C1
void AsnFormat::getContents()
{
}

//##ModelId=3B84FD0C004A
int AsnFormat::getCdrSize(const char *theSign,const char *cdrTmp,int &aBlockOffset)
{
	int asnTagSize,lengthSize,asnCdrSize;
	unsigned char tmp[10],tagBuffer[4];

	memcpy(tmp,(unsigned char*)cdrTmp,8);
	
	unsigned const char *tmp1;
	tmp1 = (unsigned char *)cdrTmp;
	
	asnTagSize		= getAsnTag(tmp,tagBuffer);
	lengthSize		= getAsnLength(tmp1+asnTagSize,asnCdrSize);
	aBlockOffset	= aBlockOffset + lengthSize + asnTagSize  ;

	return asnCdrSize;
}

//##ModelId=3B85155A0137
int AsnFormat::getAsnLength(const unsigned char *lengthBuffer,int &aAsnCdrSize)
{
	int lengthSize;

	aAsnCdrSize = 0;

	if( lengthBuffer[0] > 128 ) 
	{
		lengthSize	= lengthBuffer[0] & 127;
		for( int i=1 ; i <= lengthSize ; i++ ) 
		{
			aAsnCdrSize += (int) ((double)lengthBuffer[i] * ( pow((double)256,(lengthSize - i)) ));
		}
		lengthSize++;
	}
	// add by chenm 2005-3-10 
	// 对于asn.1标准的不定长形式
    //由一个字节组成的LENGTH取值为10000000B。CONTENTS的长度不确定，其结束标志为 end-of-contents 字节（由2个值为零的字节组成）
	else if( lengthBuffer[0] == 128 )
	{
		aAsnCdrSize = getZeroLen(lengthBuffer);
        return 1;
	}
	// over 2005-3-10 
	else 
	{
		aAsnCdrSize	= lengthBuffer[0];
		lengthSize	= 1;
	}

	return lengthSize;
}

//##ModelId=3C047E8700B1
int AsnFormat::getAsnTag(const unsigned char *buffer,unsigned char *aTag)
{
	int i;
	unsigned char tmp;

	tmp=buffer[0];
	tmp&=31;			// 31 is B11111

	if(tmp==31)
	{
	    // update by chenm 2005-12-28 处理两个以上BYTE的TAG值
	    int j=2;
	    while( buffer[j-1]  >= 128 )
	    {
	    	++j;	
	    }
	    i = j;
	    
	    //if(buffer[1]  >= 128 ) 
	    //{
	    //    i	= buffer[1] - 128 + 2;
	    //} 
	    //else 
	    //{
	    //    i = 2;
	    //}
	}
	else
		i = 1;

	memcpy(aTag,buffer,i);

	return i;
}

//##ModelId=3B94E59900C6
int AsnFormat::getValue(unsigned char * theSign,unsigned char *aValue,int position)
{
    int count=0;
	int returnValue=0;
	unsigned char signTmp[10];
	int tagSize,valueSize,valueLength;

	while(m_subOffset<m_cdrSize)
	{
		tagSize      = getAsnTag((unsigned char *)m_CdrBuffer+m_subOffset+m_bufferOffset,signTmp);

		m_subOffset += tagSize;			    //skip tag

		valueLength  = getAsnLength(m_CdrBuffer+m_subOffset+m_bufferOffset,valueSize);
		m_subOffset += valueLength;		    //skip value size's length

		// add by chenm 2005-6-20 
		if( valueSize > (m_cdrSize-m_subOffset) || valueSize < 0)
		{
			theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
	                             m_RetLog_->getSrcFileName() + " 字段值长度超过文件长度错误!");
			return -1;	
		}
		// over 2005-6-20

		if( memcmp(theSign,signTmp,tagSize) == 0  ) 
		{
		    if(valueSize<0) 
		    {
		        theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
	                             m_RetLog_->getSrcFileName() + " 字段值长度错误!");
	            return -1;
		    }
			if ( count == position ) 
			{
			    memcpy(aValue,m_CdrBuffer+m_bufferOffset+m_subOffset,valueSize);
			    m_subOffset += valueSize;	//skip value
			    returnValue  = valueSize;
			    break;
			}
			count++;
		} 
		else 
		{
			returnValue  = 0;
		}
		m_subOffset += valueSize;	   //skip value
	}

	return returnValue;
}

//##ModelId=3B95E83C0203
bool AsnFormat::isCompositeTag(int tag)
{
	if( ( (tag&32) >> 5 ) == 1 )
		return true;			// tag第3位是1,是复合字段
	else
		return false;			// tag第3位是1,不是复合字段
}

//##ModelId=3B96E434014E
void AsnFormat::initialize()
{
	for(int i=0;i<3;i++) 
	{
		m_RecordFormatPointer.push_back(new AsnFormat);
	}
}

int AsnFormat::getZeroLen(const unsigned char * buffer)
{   
    char tmp[80];
    char ans_end[2];
    int length,len,len1,buffer_len;
    unsigned char *buff;
    unsigned char aTag[10];
         
    memset(ans_end,0,2);
    
    length = 0;
    buff = (unsigned char *)buffer + 1;
    buffer_len = strlen((char*)buff);
    while(1)
    {
        if( memcmp( buff , ans_end , 2) == 0 ) 
        {
            length +=2;
            break;
        } 
        else 
        {
            len      = getAsnTag(buff,aTag);
            
            buff    += len;
            length  += len;
            
            len1     = getAsnLength(buff,len);

            len     += len1;
            length  += len;
            //if( length > buffer_len )
            //{
            //	length = buffer_len;
            //	break;	
            //}

            buff    += len;              
        }                        
    }
    
    return length;   
}
