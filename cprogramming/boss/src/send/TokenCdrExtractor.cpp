// ##########################################
// Source file : TokenCdrExtractor.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020607
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "TokenCdrExtractor.h"

//##ModelId=3B60136001BA
int TokenCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
    int cdrSize,controlCount;

    cdrSize=getcdrSize(m_BlockOffset+m_BlockBuffer,controlCount);

	if ( (m_RealBlockLength - m_BlockOffset) < cdrSize ) 
	{
		return 0;			//返回"0"表示本块内的记录已经取完。
	}
    if(cdrSize<=0) 
    {
	    return 0;
	}
	if(cdrSize>1024)
	{
		cdrSize=1024;	
	}
	memcpy( srcCdrBuffer,m_BlockOffset+m_BlockBuffer ,cdrSize );
	//#ifdef _DEBUG_
	//cout<<"SRCCDRBUFFER!!: "<<srcCdrBuffer<<endl;
	//cout<<"BLOCKBUFFER!!!: "<<m_BlockBuffer<<endl;
	//	#endif
	
	//if( !checkTokenCount(srcCdrBuffer,cdrSize) )
	//{
	//	return -1;	
	//}
	m_BlockOffset = m_BlockOffset + cdrSize + controlCount  ;

	return cdrSize;		    // 返回原始记录长度
}

//校验该条读入的cdr记录中的字段数是否和配置文件中所配置的相同
bool TokenCdrExtractor::checkTokenCount(const unsigned char *buffer,const int len)
{
    int j=0;
    
    //配置文件中的字段数为“0”时，不校验原始cdr中的字段数
    if( m_CdrSize == 0 )
    	return true;
    
    for(int i=0;i<len;i++)
    {
       if( buffer[i] == m_token ) 
       {
            j++;
       }
    }
	
	if( j == (m_CdrSize-1) ) 
    {
    	return true;
    }
	else
	{
		return false;
	}
}


//##ModelId=3C30275E00A5
int TokenCdrExtractor::getcdrSize(const unsigned char *srcCdrBuffer,int &controlCount)
{
	int i,j;
	
	for(i=0 ; i < (m_RealBlockLength - m_BlockOffset) ; i++)
	{
	   if( srcCdrBuffer[i]<0x20 && srcCdrBuffer[i] != '\t' )    // 是控制字符，即到行尾
	   {
	      break;
	   }
	}
	for(j=0;(j+i)<(m_RealBlockLength - m_BlockOffset) ; j++)
	{
	   if(srcCdrBuffer[j+i]>=0x20)                              // 不是控制字符，得到控制字符个数
	   {
	      break;
	   }
	}
	controlCount = j ;
	
	return i;
}


//##ModelId=3B820A00030B
bool TokenCdrExtractor::initialize(const string theCdrParameter,const int theBlockHeaderSize)
{
    m_CdrSize 	= atoi( theCdrParameter.c_str() );  // 字段数
    m_BlockHeaderSize   = theBlockHeaderSize;

	return true;
}

void TokenCdrExtractor::setToken(const string token)
{
	char tmp[10];
	strcpy(tmp,token.c_str());
	m_token = tmp[0];
}





