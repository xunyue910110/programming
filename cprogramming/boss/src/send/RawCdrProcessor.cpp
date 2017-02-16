// ##########################################
// Source file : RawCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.16
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020520
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <sys/time.h>

#include "RawCdrProcessor.h"
#include "CompositeFieldExtractRule.h"
#include "SendApp.h"
#include "FieldExtractRule.h"

extern double gettime();

//##ModelId=3B948D41001B
RawCdrProcessor::RawCdrProcessor()
{
	m_SendLog_			= SendLog::getInstance();
	m_cdrFlag			= CdrFlag::getInstance();
	m_theRecordFormat	= 0;
	m_cdrFlag->clear();
}

//##ModelId=3B6A0D9E0214
RawCdrProcessor::~RawCdrProcessor()
{
	if ( m_theRecordFormat != 0 ) 
	{
		delete m_theRecordFormat;
	}
}

//##ModelId=3B60CE4B03AA
bool RawCdrProcessor::initialize(vector<string> aParameterList)
{
    m_FieldValidators.setValidType("fields");
	m_theCompositeField.setSectionName("ROOT");

	return m_theCompositeField.initialize( &aParameterList , &m_theCompositeField , 0 );
}

//##ModelId=3C329EBB025D
bool RawCdrProcessor::setToken(string token)
{
   return m_theCompositeField.setToken(token);
}

//##ModelId=3BAEFEF20326
bool RawCdrProcessor::setRule(vector<string> aConvertRule,vector<string> aValidRule)
{
	if (!m_FieldConvertors.initialize(aConvertRule)) 
	{
	   theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize convert rule!");
	   return false;
	}

	if (!m_FieldValidators.initialize(aValidRule)) 
	{
	   theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize field Valid rule!");
	   return false;
	}

	return true;
}

//##ModelId=3B4EB5EC00E5
bool RawCdrProcessor::validCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    m_FieldValidators.validField(theFieldInfoMaps);
    return true;
}

//##ModelId=3BAF001901EA
bool RawCdrProcessor::convertCdr(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	if(!m_FieldConvertors.convertField(theFieldInfoMaps)) 
	{
	   theErrorMessages->insert(PP_ERROR_PARAMETER, "convert rule error!");
	   return false;
	}
	return true;
}

//##ModelId=3B9F334F0289
bool RawCdrProcessor::formatField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len;
	bool flag=true;
	FieldInfo *pFieldInfo;                  // add by zhangy in 2002.4.22

	multimap<MyString,FieldInfo *>::iterator itr;
//FP_BEGIN(formatField)
    for(itr=theFieldInfoMaps.begin();itr!=theFieldInfoMaps.end();++itr)
	{
		FieldInfo &rd = *(itr->second);

		if( ((rd.m_FieldName_).compare("Module_name") == 0)
		    ||(rd.m_dataType == "") )
			continue;
		if (  rd.m_dataType.compare(1,3,"BCD") == 0 ) 
		{
			flag=m_DataFormat.getFmtValue(rd.m_FieldDesValue,rd.m_FieldSrcValue,
							 			   rd.m_FieldLength,rd.m_padding,rd.m_dataType);
		} 
		else if ( (rd.m_dataType).compare("Binary") == 0 ) 
		{
			flag=m_DataFormat.getFmtValue(rd.m_FieldDesValue,rd.m_FieldSrcValue,
							 			   rd.m_FieldLength,rd.m_order);
		} 
		else if ( (rd.m_dataType).compare("HEX")==0) 
		{
			flag=m_DataFormat.getFmtValue(rd.m_FieldDesValue,rd.m_FieldSrcValue,
					 			  		rd.m_FieldLength,(char *)"0",rd.m_order);
		} 
		else if ( (rd.m_dataType).compare("Ascii")==0) 
		{
			flag=m_DataFormat.getFmtValue(rd.m_FieldDesValue,rd.m_FieldSrcValue,
					 			  		rd.m_FieldLength,(char *)"1",rd.m_order);
		} 
		else if ( (rd.m_dataType).compare("HexToDec")==0) 
		{
			flag=m_DataFormat.getFmtValue(rd.m_FieldDesValue,rd.m_FieldSrcValue,
					 			  		rd.m_FieldLength,(char *)"2",0);
		} 
		else if ( (rd.m_dataType).compare("GprsAsc")==0) 
		{
			flag=m_DataFormat.getGprsAscii(rd.m_FieldDesValue,rd.m_FieldSrcValue,
					 			  		rd.m_FieldLength);
		}
		else if ( (rd.m_dataType).compare("GprsHexIp")==0) 
		{
			flag=m_DataFormat.getGprsHexIp(rd.m_FieldDesValue,rd.m_FieldSrcValue,
					 			  		rd.m_FieldLength);
		}
		else 
		{
		   continue;
		}
//		rd.m_IsFormat		         = true;
		if(!flag) 
		{
		   rd.m_errorCode	         = FORBIDDEN_CHARACTER;
		   if(m_cdrFlag->m_errorCode==0) 
		   {
		      m_cdrFlag->m_isError    = true;
		      m_cdrFlag->m_fieldName  = rd.m_FieldName_;
              m_cdrFlag->m_errorCode  = FORBIDDEN_CHARACTER;
           }
		}
	}
//FP_END(formatField)

//FP_BEGIN(convertDateTime)
	if(!convertDateTime(theFieldInfoMaps)) 
	{                                                              // 时间转换错误
	    if(m_cdrFlag->m_errorCode==0) 
	    {
	         m_cdrFlag->m_isError       = true;
	         m_cdrFlag->m_fieldName     = "START_DATE";
	         m_cdrFlag->m_errorCode     = INVALID_DATE_TIME;

	         pFieldInfo = new FieldInfo(0);
	         pFieldInfo->m_FieldName_	= m_cdrFlag->m_fieldName;   // add by zhangy in 2002.4.22
	         pFieldInfo->m_FieldLength	= 0;                        // add by zhangy in 2002.4.22
//	         pFieldInfo->m_IsFormat		= true;                     // add by zhangy in 2002.4.22
//	         pFieldInfo->m_IsCheck 		= true;                     // add by zhangy in 2002.4.22
	         //aFieldInfo.m_FieldDesValue.clear();                     // add by zhangy in 2002.4.22
             pFieldInfo->m_FieldDesValue="";                          //by chenmin 08-27  

	         theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );                      // add by zhangy in 2002.4.22
	    }
    }
//FP_END(convertDateTime)


	return true;
}

//##ModelId=3BA6F2CB01FA
bool RawCdrProcessor::convertDateTime(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	FieldInfo *aFieldInfo;
	char cduration[6],cYr[5];
	int iduration,iYr,i_sMo,i_eMo;
	DateTime startDateTime,endDateTime;
	int i_sdLen,i_stLen,i_edLen,i_etLen,i_dLen;
	unsigned char s_dsrcValue[20],s_tsrcValue[20];
	unsigned char e_dsrcValue[20],e_tsrcValue[20],dur[10];

	int st_yr,st_mo,st_da,st_hr,st_mi,st_se;
	int en_yr,en_mo,en_da,en_hr,en_mi,en_se,duration;

	static string start_ye="START_YEAR";
	static string start_mo="START_MONTH";
	static string start_dy="START_DAY";
	static string start_ho="START_HOUR";
	static string start_mi="START_MINUTE";
	static string start_se="START_SECOND";

	static string end_ye="END_YEAR";
	static string end_mo="END_MONTH";
	static string end_dy="END_DAY";
	static string end_ho="END_HOUR";
	static string end_mi="END_MINUTE";
	static string end_se="END_SECOND";

	static string du="DURATION";

	i_sdLen	= 0;

	st_yr	= getFieldValue(theFieldInfoMaps,i_sdLen,s_dsrcValue,start_ye );
	st_mo	= getFieldValue(theFieldInfoMaps,i_sdLen,s_dsrcValue,start_mo );
	st_da	= getFieldValue(theFieldInfoMaps,i_sdLen,s_dsrcValue,start_dy );
	i_stLen	= 0;
	st_hr	= getFieldValue(theFieldInfoMaps,i_stLen,s_tsrcValue,start_ho );
	st_mi	= getFieldValue(theFieldInfoMaps,i_stLen,s_tsrcValue,start_mi );
	st_se	= getFieldValue(theFieldInfoMaps,i_stLen,s_tsrcValue,start_se );
	i_edLen	= 0;
	en_yr	= getFieldValue(theFieldInfoMaps,i_edLen,e_dsrcValue,end_ye );
	en_mo	= getFieldValue(theFieldInfoMaps,i_edLen,e_dsrcValue,end_mo );
	en_da	= getFieldValue(theFieldInfoMaps,i_edLen,e_dsrcValue,end_dy );
	i_etLen	= 0;
	en_hr	= getFieldValue(theFieldInfoMaps,i_etLen,e_tsrcValue,end_ho );
	en_mi	= getFieldValue(theFieldInfoMaps,i_etLen,e_tsrcValue,end_mi );
	en_se	= getFieldValue(theFieldInfoMaps,i_etLen,e_tsrcValue,end_se);
	i_dLen	= 0;
	duration = getFieldValue(theFieldInfoMaps,i_dLen,dur,du	);

    if(st_mo==-3)              // 如果开始月份没有定义，则不作校验，edit by zhangy in 2002.4.22
      return true;            // 如此可对没有配置的话单不作校验，如无效话单。

	if (st_yr<0) 
	{
		if(!startDateTime.setDateTime(st_mo,st_da,st_hr,st_mi,st_se)) 
		{
			return false;
	    }
	} 
	else 
	{
		if(!startDateTime.setDateTime(st_yr,st_mo,st_da,st_hr,st_mi,st_se))
		{
			return false;
		}
	}
	addFieldMap(theFieldInfoMaps,(char *)"START_DATE",
	            startDateTime.getDate(),i_sdLen,s_dsrcValue);   // move to here by zhangy in 2002.5.20
	addFieldMap(theFieldInfoMaps,(char *)"START_TIME",
	            startDateTime.getTime(),i_stLen,s_tsrcValue);   // move to here by zhangy in 2002.5.20

	if(duration<0) 
	{
		if(en_yr<0 && en_hr>=0) 
		{
			iYr  =st_yr;
			i_sMo=st_mo;
			i_eMo=en_mo;
			if(i_eMo>0 && i_eMo<i_sMo) 
			{
				iYr--;
			}
			en_yr=iYr;
			en_mo=st_mo;
			en_da=st_da;
		}

		if(en_yr<0) 
		{
			if(!endDateTime.setDateTime(en_mo,en_da,en_hr,en_mi,en_se)) 
			{
				return false;
			}
		} 
		else 
		{
			if(!endDateTime.setDateTime(en_yr,en_mo,en_da,en_hr,en_mi,en_se)) 
			{
				return false;
			}
		}

		iduration=endDateTime.secondDiff(&startDateTime);
		sprintf(cduration,"%d",iduration);
		addFieldMap(theFieldInfoMaps,(char *)"DURATION",cduration,i_dLen,dur);
	} 
	else 
	{
		endDateTime=startDateTime;
		endDateTime.addSecond( duration );
	}
	addFieldMap(theFieldInfoMaps,(char *)"END_DATE",endDateTime.getDate(),i_edLen,e_dsrcValue);
	addFieldMap(theFieldInfoMaps,(char *)"END_TIME",endDateTime.getTime(),i_etLen,e_tsrcValue);

	return true;
}

//##ModelId=3BA71AF10285
void RawCdrProcessor::addFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,char *fieldName,
					string fieldValue,int filenLength,const unsigned char *FieldSrc)
{
	FieldInfo *pFieldInfo;

	pFieldInfo = new FieldInfo(filenLength+1);
	pFieldInfo->m_FieldName_		= fieldName;
	pFieldInfo->m_FieldDesValue		= fieldValue;
	pFieldInfo->m_FieldLength	    = filenLength;
//	pFieldInfo->m_IsFormat			= true;
//	pFieldInfo->m_IsCheck 			= true;

	if(filenLength>0) 
	{
      	memcpy(pFieldInfo->m_FieldSrcValue,FieldSrc,filenLength);
	}

	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(fieldName,pFieldInfo) );
}

//##ModelId=3BA708560240
int RawCdrProcessor::getFieldValue(multimap<MyString,FieldInfo *> &theFieldInfoMaps,int &size,
						unsigned char * const &FieldSrcValue,const string &fieldName)
{
    // modified by chenm 2007-3-5
    FieldInfo *pFieldInfo;
    
    if( (pFieldInfo=FieldConvertRule::getFieldMap(theFieldInfoMaps,fieldName)) != 0)
    {
		if( pFieldInfo->m_FieldDesValue.empty() ) 
		{
		    return -1;
		}
		if(pFieldInfo->m_FieldLength<=0 && fieldName.compare("DURATION")!=0) 
		{ 
	        theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
	                                 m_SendLog_->getSrcFileName() + " 字段长度错误!");
	        return -2;              // edit by zhangy in 2002.4.22
	    }
	    if(pFieldInfo->m_FieldLength>0) 
	    {   
		    memcpy( FieldSrcValue + size , pFieldInfo->m_FieldSrcValue , pFieldInfo->m_FieldLength );
		    size+=pFieldInfo->m_FieldLength;
		}
		return atoi( pFieldInfo->m_FieldDesValue.c_str() );
    }
    else
    {
    	return -3;	
    }
     // modified over by chenm 2007-3-5
}

//##ModelId=3B9D92EA028A
FieldExtractRule * RawCdrProcessor::getNextCompositeField(string &theCdrSign)
{
    int len;
	FieldExtractRule *theFieldExtractRule = 0;

    vector <FieldExtractRule *>&rd=m_theCompositeField.m_theChildField_;
    len=rd.size();
	for ( int i=0;i<len; i++ )
	{
		if ( (rd[i]->getSectionName()).compare(theCdrSign)==0 ) 
		{
			theFieldExtractRule=rd[i];
			break;
		}
	}

	return theFieldExtractRule;
}
