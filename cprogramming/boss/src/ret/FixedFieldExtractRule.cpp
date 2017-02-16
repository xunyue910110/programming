// ##########################################
// Source file : FixedFieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.29
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020415
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "Baf.h"
#include "FixedFieldExtractRule.h"
#include "RetApp.h"

extern Baf g_theBaf;        //处理nortel等话单中的module by chenmin at 20020415

//##ModelId=3B7D257F0385
bool FixedFieldExtractRule::initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor)
{
	int i,len;
	char *tok;
	char Tmp[100];
	string theArgname;

    // msisdn               FIX,29,0,96,TBCD,F

	theArgname=*(argname->begin());
	len=theArgname.size();

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

	strtok( (char *)theArgname.c_str() , "," )	;
	tok	= strtok(NULL,",");
	if (tok!=NULL)
		m_StartByte_	= atoi( tok )	;	//得到开始字节
	tok	= strtok(NULL,",");
	if (tok!=NULL)
		m_StartBit_		= atoi( tok )	;	//得到开始比特
	tok	= strtok(NULL,",");
	if (tok!=NULL)
		m_BitLength_	= atoi( tok )	;	//得到字段长度
	tok	= strtok(NULL,",");
	if (tok!=NULL) 
	{
		m_dataType		= tok			;	//得到数据类型
		if(m_dataType.substr(1,3)=="BCD") 
		{
			tok	= strtok(NULL,",");
			if ( tok!=NULL && strlen(tok)==1 ) 
			{
				m_padding = tok[0];			//得到BCD填充字符
			}
		} 
		else if (m_dataType=="Binary") 
		{
			tok	= strtok(NULL,",");
			if(tok!=NULL) 
			{
				m_order	= atoi( tok )	;	//得到高低位交换标志
			}
		} 
		else if (m_dataType=="HEX") 
		{
			tok	= strtok(NULL,",");
			if(tok!=NULL) 
			{
				m_order	= atoi( tok )	;	//得到高低位交换标志
			}
		}
	}
	// add by chenm 2005-3-7 为nort交换机，标识附加模块在话单中出现的位置
	tok	= strtok(NULL,",");
	if (tok!=NULL) 	
	{
		m_position = atoi( tok );	
	}

	return true;
}

//##ModelId=3B77E2AB03B1
bool FixedFieldExtractRule::extract(const unsigned char *buffer, int length,
					   	  multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat)
{
	unsigned char tmp[1024];
	//char c_module_id[5];
	int filedLength,real_StartByte;

    //****************************************************//
    //modified by chenmin 2002-04-15 for Nortel Module contents
    real_StartByte=m_StartByte_;
    if( ( m_FieldName_.compare(0,4,"NORT")==0 ) ) 
    {
        if ( m_StartByte_ <= (length-2) )
        {
	        real_StartByte=g_theBaf.getLenth(buffer,length,m_FieldName_,m_StartByte_,m_position);
	        if( real_StartByte == 0 )  
	        {
	           	return false;
	        }
		}
		else
		{
			return false;	
		}
    }
    //*********************over*****************************
    memset(tmp,0,sizeof(tmp));


	filedLength = m_BitLength_ / 8 ;

	if (m_BitLength_ % 8 != 0)  
	{
	    filedLength++;
        if(filedLength<=0)  
        {
	         theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
	                                  m_RetLog_->getSrcFileName() + " 字段长度错误!");
	         return false;
	    }

        memcpy(tmp, buffer + real_StartByte, filedLength);
	    //对单字节中的长度不足一个字节的字段提取

	    if( filedLength == 1 ) 
	    {
	        tmp[0]=tmp[0] << m_StartBit_;
	        tmp[0]=tmp[0] >> 8 - m_BitLength_;
		    m_FieldInfo = new FieldInfo(filedLength+1);
	        memcpy(m_FieldInfo->m_FieldSrcValue, tmp , filedLength);

	    }                                           //modified by chenmin
	    else 
	    {        
	        tmp[filedLength-1]=(tmp[filedLength-1] | 0x0f);
		    m_FieldInfo = new FieldInfo(filedLength+1);
	        memcpy(m_FieldInfo->m_FieldSrcValue, tmp , filedLength);
	    }
    } 
    else  
    {
	    if(m_BitLength_==0)              // 此方法用于对变长字段的子串长度的定义。
	    {
	        filedLength=length-real_StartByte;    // 字段长度＝原长度－子串的开始长度。
	    }
	    
	    //当传入的buffer长度大于0时,才对m_FieldSrcValue赋值 by chenm 2002-11-4 16:25
	    if(filedLength>0 && length>0 )  
	    {
		    m_FieldInfo = new FieldInfo(filedLength+1);
	        memcpy(m_FieldInfo->m_FieldSrcValue, buffer + real_StartByte, filedLength);
	        m_FieldInfo->m_FieldLength=filedLength;
	    } 
	    else 
	    {
		    m_FieldInfo = new FieldInfo(0);
	        memset( m_FieldInfo->m_FieldSrcValue,0,sizeof(m_FieldInfo->m_FieldSrcValue) );
	        m_FieldInfo->m_FieldLength = 0;
	        filedLength = 0;
	    }
	}

	m_FieldInfo->m_FieldName_ = m_FieldName_;
	m_FieldInfo->m_FieldLength 	= filedLength;
	m_FieldInfo->m_padding		= m_padding;
	m_FieldInfo->m_order		    = m_order;
	m_FieldInfo->m_dataType		= m_dataType;
	afieldMap.insert( multimap<MyString,FieldInfo *>::value_type(m_FieldName_.c_str(),m_FieldInfo) );

	return true;
}


