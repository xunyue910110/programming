// ##########################################
// Source file : TokenFieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020118
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "TokenFieldExtractRule.h"

//##ModelId=3B7D2580021E
bool TokenFieldExtractRule::initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor)
{
	char *tok;
	int i,j,len;
	char Tmp[100];
	string theArgname;


   // CDR_NO            TOKEN,1,Ascii

	theArgname=*(argname->begin());
	len=theArgname.size();

    Tmp[0]=0;
	strcpy( Tmp,theArgname.c_str() );

	for (i=0;i<strlen(Tmp);i++) 			//得到空格前的串,即字段名称
	{
		if (Tmp[i]==' ' || Tmp[i]=='\t')
			break;
	}
	m_FieldName_	= theArgname.substr(0,i);
	theArgname		= theArgname.substr(i,len-i);

	len				= theArgname.size();
	strcpy( Tmp,theArgname.c_str() );
	for (i=0;i<strlen(Tmp);i++) 			//得到空格后的串,即格式串
	{
		if (Tmp[i]!=' ' && Tmp[i]!='\t')
			break;
	}
	theArgname		= theArgname.substr(i,len-i);

	strtok( (char *)theArgname.c_str() , "," );
	tok	= strtok(NULL,",");
	if (tok!=NULL)
		m_position	= atoi(tok) ;			//得到字段Tag码
	tok	= strtok(NULL,",");
	if (tok!=NULL) 
	{
		m_dataType		= tok			;	//得到数据类型
        if(m_dataType.compare(1,3,(char *)"BCD")==0) 
        {
			tok	= strtok(NULL,",");
			if ( tok!=NULL && strlen(tok)==1 ) 
			{
				m_padding = tok[0];			//得到BCD填充字符
			}
		} 
		else if (m_dataType.compare((char *)"Binary")==0) 
		{
			tok	= strtok(NULL,",");
			if(tok!=NULL)
				m_order	= atoi( tok )	;	//得到高低位交换标志
		} 
		else if (m_dataType=="HEX") 
		{
			tok	= strtok(NULL,",");
			if(tok!=NULL)
				m_order	= atoi( tok )	;	//得到高低位交换标志
		}
	}

	return true;
}

//##ModelId=3B77DFCB03A9
bool TokenFieldExtractRule::extract(const unsigned char *buffer, int length,
					   		 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat)
{
	int theFieldLength;
	unsigned char theValue[1024];

	theFieldLength = getValue(buffer,theValue,length);

    m_FieldInfo = new FieldInfo(theFieldLength+1);
	m_FieldInfo->m_FieldName_ 	= m_FieldName_;
	m_FieldInfo->m_FieldLength 	= theFieldLength;
	m_FieldInfo->m_padding	    = m_padding;
	m_FieldInfo->m_order		= m_order;
	m_FieldInfo->m_dataType		= m_dataType;

	//当theFieldLength的大小为m_FieldSrcValue的允许范围时,才赋值
	if(theFieldLength>0 && theFieldLength<900)  
	{
	   memcpy(m_FieldInfo->m_FieldSrcValue, theValue, theFieldLength);
	}

	afieldMap.insert( multimap<MyString,FieldInfo *>::value_type(m_FieldName_.c_str(),m_FieldInfo) );

	return true;

}

//##ModelId=3C316DBD00B7
int TokenFieldExtractRule::getValue(const unsigned char *buffer,unsigned char *aValue,int len)
{
    int n,offset,offset1,fieldLen;
    
    n=1;
    offset=0;
    fieldLen=-1;

    while( ( offset1=getNextToken((char *)buffer,offset,len) ) <= len )
    {
       fieldLen=offset1-offset;
       if(n==m_position) 
       {
          memcpy(aValue,buffer+offset,fieldLen);
          aValue[fieldLen]=0;
          break;
       }
       offset1=getNextToken((char *)buffer,offset,len) ;
       offset+=(fieldLen+1);
       n++;
    }
    if(offset1>len) 
    {
       fieldLen--;
       aValue[fieldLen]=0;
    }
    
    return fieldLen;
}

//##ModelId=3C31B553038A
int TokenFieldExtractRule::getNextToken(const char *cdrBuffer,int offset,int len)
{
    int i;
    
    for( i=offset; i<len; i++ )
    {
       if(cdrBuffer[i]==m_token)
          return i;
    }
    
    if(i==offset) 
    {
       len++;
    }
    
    return len;
}






