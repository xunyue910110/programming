// ##########################################
// Source file : FieldConvertRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################


#include "FieldConvertRule.h"
#include "base/supdebug.h"

//##ModelId=3BAFF7BD00BF
FieldConvertRule::FieldConvertRule()
{
	m_RetLog	  = RetLog::getInstance();
	m_CdrFlag = CdrFlag::getInstance();
}

//##ModelId=3BAF2A7B03DE
FieldInfo * FieldConvertRule::getFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const string &theFieldName)
{
    multimap<MyString,FieldInfo *>::iterator itr;
    itr = theFieldInfoMaps.find(theFieldName.c_str());      
    if( (itr != theFieldInfoMaps.end() )
             && ((itr->second)->m_FieldName_.compare(theFieldName) == 0)
       )
    {
        return itr->second;
    }
	return 0;
}

//##ModelId=3BAFF7BD0173
string FieldConvertRule::lrTrim(string theString)
{
	int len,size;

	size	= theString.size();
	len		= theString.find_first_not_of(" \t");
	if(len>0)
		theString	= theString.substr(len,size-len);	// 去掉头部空格和Tab

	len		= theString.find_last_not_of(" \t");
	if(len>0)
		theString	= theString.substr(0,len+1);		   // 去掉尾部空格和Tab

	return theString;
}


//##ModelId=3C244E6C0006
// created by yaop at 2001.12.22 in hainan.
int FieldConvertRule::strnrps(const char* src,const char* token,const char *rps,char *des,int max)
{
    int l1;
	char *p=0;
	char *s3;
	char *s2=des;
	char *s1=(char *)src;

    l1 = strlen(token);
	if (!l1)
		return 0;

	while((p=strstr(s1,token))!='\0')
	{
		for (;s1<p;*s2++=*s1++,max--)
		{
			if (!max) 	
			{
				*s2='\0';
	         	return s2-des;
			}
		}
		p+=strlen(token);
		s1=p;
		for (s3=(char *)rps;*s3 !='\0';*s2++=*s3++,max--)
		{
			if (!max) 	
			{
				*s2='\0';
	         	return s2-des;
			}
		}
	}

	for(;(*s2=*s1)!='\0';s2++,s1++,max--)
	{
		if (!max) 	
		{
			*s2='\0';
		    return s2-des;
		}
	}

	return s2-des;
}

//##ModelId=3CE312990130
//##Documentation
//## 用于根据开始时间和被修改的时长，重新计算出结束时间。
//## add by zhangy in 2002.5.16
void FieldConvertRule::editTime(int duration, multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    char tmp[10];
    char startDate[9],startTime[7];
    int st_yr,st_mo,st_da,st_hr,st_mi,st_se;
    FieldInfo *startFieldInfo,*endFieldInfo;
    DateTime endDateTime;
    FieldInfo *pFieldInfo;
    static string st_date("START_DATE"),st_time("START_TIME");
    static string ed_date("END_DATE"),ed_time("END_TIME");


    if( (startFieldInfo=getFieldMap(theFieldInfoMaps,st_date))==0 )
		return ;
	strcpy(startDate,startFieldInfo->m_FieldDesValue.c_str());

	if( (startFieldInfo=getFieldMap(theFieldInfoMaps,st_time))==0 )
		return ;
    strcpy(startTime,startFieldInfo->m_FieldDesValue.c_str());

    strncpy(tmp,startDate,4);
    tmp[4]=0;
    st_yr=atoi(tmp);
    strncpy(tmp,startDate+4,2);
    tmp[2]=0;
    st_mo=atoi(tmp);
    strncpy(tmp,startDate+6,2);
    tmp[2]=0;
    st_da=atoi(tmp);

    strncpy(tmp,startTime,2);
    tmp[2]=0;
    st_hr=atoi(tmp);
    strncpy(tmp,startTime+2,2);
    tmp[2]=0;
    st_mi=atoi(tmp);
    strncpy(tmp,startTime+4,2);
    tmp[2]=0;
    st_se=atoi(tmp);

    if(!endDateTime.setDateTime(st_yr,st_mo,st_da,st_hr,st_mi,st_se)) 
    {
		return ;
    }
	endDateTime.addSecond( duration );

    if( (endFieldInfo=getFieldMap(theFieldInfoMaps,ed_date))==0 ) 
    {
	    pFieldInfo = new FieldInfo(0);
	    pFieldInfo->m_FieldName_		= ed_date;
	    pFieldInfo->m_FieldLength	    = 0;
//	    pFieldInfo->m_IsFormat		= true;
//	    pFieldInfo->m_IsCheck 		= true;
	    pFieldInfo->m_FieldDesValue   = endDateTime.getDate();

	    theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(ed_date.c_str(),pFieldInfo) );
    } 
    else 
    {
	    endFieldInfo->m_FieldDesValue=endDateTime.getDate();
	}

	if( (endFieldInfo=getFieldMap(theFieldInfoMaps,ed_time))==0 ) 
	{
	    pFieldInfo = new FieldInfo(0);
	    pFieldInfo->m_FieldName_		= ed_time;
	    pFieldInfo->m_FieldLength	    = 0;
//	    pFieldInfo->m_IsFormat		= true;
//	    pFieldInfo->m_IsCheck 		= true;
	    pFieldInfo->m_FieldDesValue   = endDateTime.getTime();

	    theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(ed_time.c_str(),pFieldInfo) );
    } 
    else 
    {
	    endFieldInfo->m_FieldDesValue=endDateTime.getTime();
	}
	
	// by chenm 2002-12-24 防止时长的错误代码再带出
	if( m_CdrFlag->m_errorCode == INVALID_DATE_TIME )
	{
		m_CdrFlag->clear();	
	}

}

// add by chenm 获取多个同名字段的转换后的值,结果由desValues带出
void FieldConvertRule::getMultiFieldValues(multimap<MyString,FieldInfo *> &theFieldInfoMaps,
	                     const string &theFieldName,
	                     vector<string> &desValues)
{
	multimap<MyString,FieldInfo *>::iterator itr;
    itr = theFieldInfoMaps.find(theFieldName.c_str());      
    while( (itr != theFieldInfoMaps.end() )
             && ((itr->second)->m_FieldName_.compare(theFieldName) == 0)
       )
    {
        desValues.push_back( (itr->second)->m_FieldDesValue );
        ++itr;
    }	
	return;	
}

