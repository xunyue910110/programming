// ##########################################
// Source file : GasnFieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.35
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : xu jun
// E_mail      : xujun@lianchuang.com
// Create      : 20031113
// ##########################################

#include "GasnFieldExtractRule.h"

//##ModelId=3B7D257F010E
bool GasnFieldExtractRule::initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor)
{
	char *tok;
	int i,j,len;
	string theArgname;
	char Tmp[100],xxx[10];

    // CDR_NO                  GASN,DF30,0,Binary,0  
	theArgname=*(argname->begin());
	len=theArgname.size();

    Tmp[0]=0;
	strcpy( Tmp,theArgname.c_str() );

	for (i=0;i<strlen(Tmp);i++)  //得到空格前的串,即字段名称
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
		m_Tag_	= tok ;			//得到字段Tag码
	tok	= strtok(NULL,",");
	if (tok!=NULL)
		m_position	= atoi(tok) ;			//得到字段位置
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
		} else if (m_dataType=="HEX") 
		{
			tok	= strtok(NULL,",");
			if(tok!=NULL)
		        m_order	= atoi( tok )	;	//得到高低位交换标志
		}
	}

    // 转换tag 码为二进制码
    strcpy(Tmp,m_Tag_.c_str());
    strcpy(m_ss,(char *)"0123456789ABCDEF");
    len=m_Tag_.size();

    for(i=0;i<len;i++)
    {
        for(j=0;j<16;j++)
        {
            if(m_ss[j]==Tmp[i])
                break;
        }
        xxx[i]=j;
    }

    for(i=0,j=0;i<len;i+=2,j++)
    {
       m_tag[j]=xxx[i]*16+xxx[i+1];
    }

	return true;
}

//##ModelId=3B77DE4700E4
bool GasnFieldExtractRule::extract(const unsigned char *buffer, int length,
					   multimap<MyString,FieldInfo *> &afieldInfoMaps,RecordFormat &aRecordFormat)
{
	int theFieldLength;
	unsigned char theValue[20480];
	char cTmp[5];
	
    aRecordFormat.reset(buffer,length,0,1);

	int i_num = 0;
	while( (theFieldLength = aRecordFormat.getValue(m_tag,theValue,0)) > 0 )
    {
    	m_FieldInfo = new FieldInfo(theFieldLength+1);
    	//sprintf(cTmp,"%d",i_num);
    	m_FieldInfo->m_FieldName_ 	= m_FieldName_ ;//+ cTmp;
		m_FieldInfo->m_FieldLength 	= theFieldLength;
		m_FieldInfo->m_padding	    = m_padding;
		m_FieldInfo->m_order		= m_order;
		m_FieldInfo->m_dataType		= m_dataType;
		if(theFieldLength>0) 
		{   
	    	memcpy(m_FieldInfo->m_FieldSrcValue, theValue, theFieldLength);
		}
	                                                            	
	  	afieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_FieldInfo->m_FieldName_.c_str(),m_FieldInfo) );
	  	++i_num;
	}
	
	return true;
}
