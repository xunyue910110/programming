// ##########################################
// Source file : RawCdrProcessor.cpp
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

#include "RawCdrProcessor.h"
#include "PreProcApplication.h"
#include "CompositeFieldExtractRule.h"
#include "PpLog.h"
#include "CdrFlag.h"
#include "RecordFormat.h"
#include "FieldInfos.h"
#include "RepeatableFieldInfo.h"
#include <sys/time.h>

extern double gettime();

//##ModelId=4850FB96029F
RawCdrProcessor::RawCdrProcessor(const int iSubChannelNum)
{
    m_PpLog_			= PpLog::getInstance();
	m_cdrFlag			= CdrFlag::getInstance();
	m_theRecordFormat	= 0;
	m_cdrFlag->clear();
	m_iSubChannelNum = iSubChannelNum;
	m_pCompositeField = new CompositeFieldExtractRule(iSubChannelNum);
	
	m_pFieldInfos = FieldInfos::getInstance(iSubChannelNum);
}

//##ModelId=4850FB990148
RawCdrProcessor::~RawCdrProcessor()
{
    if ( m_theRecordFormat != 0 ) 
	{
		delete m_theRecordFormat;
	}
	
	if( m_pCompositeField != 0 )
	{
		delete 	m_pCompositeField;
	}
}

//##ModelId=4850FBB00280
bool RawCdrProcessor::validCdr()
{
    m_FieldValidators.validField();
    return true;
}

//##ModelId=4850FBCD002E
bool RawCdrProcessor::convertCdr()
{
    if(!m_FieldConvertors.convertField()) 
	{
        theErrorMessages->insert(PP_ERROR_PARAMETER, "convert rule error!");
        return false;
	}
	return true;
}

//##ModelId=4850FC24029F
bool RawCdrProcessor::initialize(vector<string> aParameterList)
{
  m_FieldValidators.setValidType("fields");
	m_pCompositeField->setSectionName("ROOT");
	m_pCompositeField->initialize(&aParameterList, m_pCompositeField, 0);
	
	m_pFieldStart_ye     = m_pFieldInfos->getFieldInfoByName(START_YEAR);
	m_pFieldStart_mo     = m_pFieldInfos->getFieldInfoByName(START_MONTH);
	m_pFieldStart_dy     = m_pFieldInfos->getFieldInfoByName(START_DAY);
	m_pFieldStart_ho     = m_pFieldInfos->getFieldInfoByName(START_HOUR);
	m_pFieldStart_mi     = m_pFieldInfos->getFieldInfoByName(START_MINUTE);
	m_pFieldStart_se     = m_pFieldInfos->getFieldInfoByName(START_SECOND);
	m_pFieldStart_ntime  = m_pFieldInfos->getFieldInfoByName(START_NTIME);
	m_pFieldEnd_ye       = m_pFieldInfos->getFieldInfoByName(END_YEAR);
	m_pFieldEnd_mo       = m_pFieldInfos->getFieldInfoByName(END_MONTH);
	m_pFieldEnd_dy       = m_pFieldInfos->getFieldInfoByName(END_DAY);
	m_pFieldEnd_ho       = m_pFieldInfos->getFieldInfoByName(END_HOUR);
	m_pFieldEnd_mi       = m_pFieldInfos->getFieldInfoByName(END_MINUTE);
	m_pFieldEnd_se       = m_pFieldInfos->getFieldInfoByName(END_SECOND);
	m_pFieldDu           = m_pFieldInfos->getFieldInfoByName(DURATION);
	m_pFieldDu_cy        = m_pFieldInfos->getFieldInfoByName(DURATION_CARRY);
  m_pFieldDu_rd        = m_pFieldInfos->getFieldInfoByName(DURATION_ROUND);
	m_pFieldDu_ncy       = m_pFieldInfos->getFieldInfoByName(DURATION_NOCARRY);
	m_pFieldDu_3ncy      = m_pFieldInfos->getFieldInfoByName(DURATION_3NOCARRY);	
	m_pFieldStartDate    = m_pFieldInfos->getFieldInfoByName(START_DATE);
  m_pFieldStartTime    = m_pFieldInfos->getFieldInfoByName(START_TIME);
	m_pFieldEndDate      = m_pFieldInfos->getFieldInfoByName(END_DATE);
	m_pFieldEndTime      = m_pFieldInfos->getFieldInfoByName(END_TIME);

	m_pFieldModule_Name  = m_pFieldInfos->getFieldInfoByName(MODULE_NAME);
	
	return true;
}

//##ModelId=4850FC39002E
bool RawCdrProcessor::setRule(vector<string> aConvertRule, vector<string> aValidRule)
{
 	if (!m_FieldConvertors.initialize(aConvertRule,m_iSubChannelNum)) 
	{
	   theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize convert rule!");
	   return false;
	}

	if (!m_FieldValidators.initialize(aValidRule,m_iSubChannelNum)) 
	{
	   theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize field Valid rule!");
	   return false;
	}

	return true;
}

//##ModelId=4850FC600222
bool RawCdrProcessor::formatField()
{
    int len;
	FieldInfo *aFieldInfo;
	multimap<MyString, FieldInfo *>::iterator itr;

FP_BEGIN(formatField_RawCdr)
	int iFieldCount;
    for(iFieldCount = 0; iFieldCount < m_pFieldInfos->getFieldNum(); ++iFieldCount)
	{
		this->convertDataType( (*m_pFieldInfos)[iFieldCount]);
	}
    for (iFieldCount = 0; iFieldCount<m_pFieldInfos->getRepeatableFieldNum(); ++iFieldCount)
    {
        RepeatableFieldInfo *pTmpFieldInfo;
        
        pTmpFieldInfo = dynamic_cast<RepeatableFieldInfo*>( (*m_pFieldInfos)[iFieldCount+DIVIDE] );
        for(int j=0;j<pTmpFieldInfo->getNumCount();++j)
        {
        	this->convertDataType( pTmpFieldInfo->m_fieldInfoVect[j]);
    	}
	}	

FP_END(formatField_RawCdr)

FP_BEGIN(convertDateTime)
	if(!convertDateTime()) 
	{                                                                   // 时间转换错误
	    if(m_cdrFlag->m_errorCode == 0) 
	    {
	         m_cdrFlag->m_isError       = true;
	         m_cdrFlag->m_fieldName     = START_DATE;
	         m_cdrFlag->m_errorCode     = INVALID_DATE_TIME;
	    }
    }
FP_END(convertDateTime)

	return true;
}

//##ModelId=4850FC7500CB
FieldExtractRule *RawCdrProcessor::getNextCompositeField(string &theCdrSign)
{
    int len;
	FieldExtractRule *theFieldExtractRule = 0;

    vector <FieldExtractRule *> &rd = m_pCompositeField->m_theChildField_;
    len = rd.size();
	for (int i = 0; i < len; i++)
	{
		if ((rd[i]->getSectionName()).compare(theCdrSign) == 0) 
		{
            theFieldExtractRule = rd[i];
			break;
		}
	}

	return theFieldExtractRule;
}

//##ModelId=4850FC9201D4
bool RawCdrProcessor::setToken(string token)
{
    return m_pCompositeField->setToken(token);
}

//##ModelId=4850FD610271
bool RawCdrProcessor::convertDateTime()
{
	char cduration[6], cYr[5];
	int iduration, iYr, i_sMo, i_eMo;
	int iDu_carry,iDu_round,iDu_nocarry,iDu_3nocarry;
	DateTime startDateTime, endDateTime;
	int i_sdLen, i_stLen, i_edLen, i_etLen, i_dLen;

    // 针对北电的时间特殊处理
	int iStartYear,iStartMonth,iStartDay,iStartHour,iStartMinute,iStartSecond,st_das,iStartNtimes;
	// 针对CDMA1X的时间特殊处理
	int iEndYear,iEndMonth,iEndDay,iEndHour,iEndMinute,iEndSecond,iDuration;
	// 针对固网1/10秒的特殊处理
	int iDuration_cry,iDuration_rd,iDuration_ncry,iDuration_3ncy;

	iStartYear	 = m_pFieldStart_ye->m_iDesValueLen==0?-3:atoi(m_pFieldStart_ye->m_FieldDesValue);
	iStartMonth	 = m_pFieldStart_mo->m_iDesValueLen==0?-3:atoi(m_pFieldStart_mo->m_FieldDesValue);
	iStartDay	 = m_pFieldStart_dy->m_iDesValueLen==0?-3:atoi(m_pFieldStart_dy->m_FieldDesValue);
	 
	iStartHour	 = m_pFieldStart_ho->m_iDesValueLen==0?-3:atoi(m_pFieldStart_ho->m_FieldDesValue);
	iStartMinute = m_pFieldStart_mi->m_iDesValueLen==0?-3:atoi(m_pFieldStart_mi->m_FieldDesValue);
	iStartSecond = m_pFieldStart_se->m_iDesValueLen==0?-3:atoi(m_pFieldStart_se->m_FieldDesValue);
	
	iStartNtimes = m_pFieldStart_ntime->m_iDesValueLen==0?-3:atoi(m_pFieldStart_ntime->m_FieldDesValue);
	if (iStartNtimes > 0)
	{
	    char ctemp[10];
	    
	    string stemp;
	    
	    memset(ctemp,0,sizeof(ctemp));
	    sprintf(ctemp,"%d",iStartNtimes);	
	    stemp = ctemp;
	        
	    int len = 9 - strlen(ctemp);
	    if (len != 0)
	    {
	        for(int i=0; i<len; i++)
	        {
	            stemp = "0" + stemp;
	        }
	    }
	    st_das = atoi(stemp.substr(0, 3).c_str());
	    if ((st_das >= 1) && (st_das <= 366))
	    {   
	        iStartMonth = 1;
	        iStartDay = 1;
	        iStartHour = atoi(stemp.substr(3, 2).c_str());
	        iStartMinute = atoi(stemp.substr(5, 2).c_str());
	        iStartSecond = atoi(stemp.substr(7, 2).c_str()); 
	    
	        if(!startDateTime.setDateTime(iStartMonth, iStartDay, iStartHour, iStartMinute, iStartSecond)) 
		    {
			    return false;
	        }
	        startDateTime.addDay(st_das - 1);
	    
	    
	        iStartMonth   = startDateTime.GetMonth();
            iStartDay     = startDateTime.GetDay();
            iStartHour    = startDateTime.GetHour();
            iStartMinute  = startDateTime.GetMinute();
            iStartSecond  = startDateTime.GetSecond();
	    }
	}
	
	iEndYear	= m_pFieldEnd_ye->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_ye->m_FieldDesValue);
	iEndMonth	= m_pFieldEnd_mo->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_mo->m_FieldDesValue);
	iEndDay	    = m_pFieldEnd_dy->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_dy->m_FieldDesValue);

	iEndHour	= m_pFieldEnd_ho->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_ho->m_FieldDesValue);
	iEndMinute	= m_pFieldEnd_mi->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_mi->m_FieldDesValue);
	iEndSecond	= m_pFieldEnd_se->m_iDesValueLen==0?-3:atoi(m_pFieldEnd_se->m_FieldDesValue);
	
	iDuration   = m_pFieldDu->m_iDesValueLen==0?-3:atoi(m_pFieldDu->m_FieldDesValue);
	
	iDuration_cry    = m_pFieldDu_cy->m_iDesValueLen==0?-3:atoi(m_pFieldDu_cy->m_FieldDesValue)/10+1;
    iDuration_rd     = m_pFieldDu_rd->m_iDesValueLen==0?-3:(atoi(m_pFieldDu_rd->m_FieldDesValue)+5)/10;
    iDuration_ncry   = m_pFieldDu_ncy->m_iDesValueLen==0?-3:atoi(m_pFieldDu_ncy->m_FieldDesValue)/10;
    iDuration_3ncy   = m_pFieldDu_3ncy->m_iDesValueLen==0?-3:(atoi(m_pFieldDu_3ncy->m_FieldDesValue)+4)/10;


    if(iStartMonth == -3)                 // 如果开始月份没有定义,则不作校验
    {
        return true;                // 如此可对没有配置的话单不作校验,如无效话单
    }

	if (iStartYear < 0) 
	{
		if(!startDateTime.setDateTime(iStartMonth, iStartDay, iStartHour, iStartMinute, iStartSecond)) 
		{
			return false;
	    }
	} 
	else 
	{
		if(!startDateTime.setDateTime(iStartYear, iStartMonth, iStartDay, iStartHour, iStartMinute, iStartSecond))
		{
			return false;
		}
	}

	m_pFieldStartDate->setDesValue(startDateTime.getDate());
	m_pFieldStartTime->setDesValue(startDateTime.getTime());

	if(iDuration < 0) 
	{
		if(iDuration_cry >= 0)
		{
		    iDuration = iDuration_cry;
		    endDateTime=startDateTime;
		    endDateTime.addSecond( iDuration );
		    sprintf(cduration,"%d",iDuration);
		    m_pFieldDu->setDesValue(cduration);
			m_pFieldEndDate->setDesValue(endDateTime.getDate());
			m_pFieldEndTime->setDesValue(endDateTime.getTime());

	        return true;
		}
		else if(iDuration_rd >= 0)
		{
		    iDuration = iDuration_rd;
		    endDateTime=startDateTime;
		    endDateTime.addSecond( iDuration );
		    sprintf(cduration,"%d",iDuration);
		    m_pFieldDu->setDesValue(cduration);
			m_pFieldEndDate->setDesValue(endDateTime.getDate());
			m_pFieldEndTime->setDesValue(endDateTime.getTime());

	        return true;
		}
		else if(iDuration_ncry >= 0)
		{
		    iDuration = iDuration_ncry;
		    endDateTime=startDateTime;
		    endDateTime.addSecond( iDuration );
		    sprintf(cduration,"%d",iDuration);
		    m_pFieldDu->setDesValue(cduration);
			m_pFieldEndDate->setDesValue(endDateTime.getDate());
			m_pFieldEndTime->setDesValue(endDateTime.getTime());

	        return true;
		}
		else if(iDuration_3ncy >= 0)
		{
		    iDuration = iDuration_3ncy;
		    endDateTime=startDateTime;
		    endDateTime.addSecond( iDuration );
		    sprintf(cduration,"%d",iDuration);
		    m_pFieldDu->setDesValue(cduration);
			m_pFieldEndDate->setDesValue(endDateTime.getDate());
			m_pFieldEndTime->setDesValue(endDateTime.getTime());

	        return true;
		}
		else
		{
    		if(iEndYear < 0 && iEndHour >= 0) 
    		{
    			iYr   = iStartYear;
    			i_sMo = iStartMonth;
    			i_eMo = iEndMonth;
    			if(i_eMo > 0 && i_eMo < i_sMo) 
    			{
    				iYr--;
    			}
    			iEndYear = iYr;
    			if (iEndMonth < 0 || iEndDay < 0)
    			{
    			    iEndMonth = iStartMonth;
    			    iEndDay = iStartDay;
    			}
    			else
    			{
    			    iEndMonth = iEndMonth;
                    iEndDay = iEndDay; 
    			}
    		}
    		if(iEndYear < 0) 
    		{
    		    		    
    			if(!endDateTime.setDateTime(iEndMonth, iEndDay, iEndHour, iEndMinute, iEndSecond)) 
    			{
    				return false;
    			}
    		} 
    		else 
    		{
    
    			if(!endDateTime.setDateTime(iEndYear, iEndMonth, iEndDay, iEndHour, iEndMinute, iEndSecond)) 
    			{
    				return false;
    			}
    		}
    	}

		iduration = endDateTime.secondDiff(&startDateTime);
		sprintf(cduration, "%d", iduration);
		m_pFieldDu->setDesValue(cduration);
	} 
	else 
	{
		endDateTime = startDateTime;
		endDateTime.addSecond(iDuration);
	}
	m_pFieldEndDate->setDesValue(endDateTime.getDate());
	m_pFieldEndTime->setDesValue(endDateTime.getTime());

	return true;
}

void RawCdrProcessor::convertDataType(FieldInfo *pFieldInfo)
{
	bool flag = true;

	if( (pFieldInfo->m_iSrcValueLen == 0) 
		 || (pFieldInfo->m_dataType.compare("")==0) )
	{
		return;
	}
	flag = m_DataFormat.getFmtValue(pFieldInfo);
	
	if(!flag) 
	{
        pFieldInfo->m_errorCode = FORBIDDEN_CHARACTER;
	    if(m_cdrFlag->m_errorCode == 0) 
	    {
	        m_cdrFlag->m_isError    = true;
            m_cdrFlag->m_errorCode  = FORBIDDEN_CHARACTER;
        }
	}
}
