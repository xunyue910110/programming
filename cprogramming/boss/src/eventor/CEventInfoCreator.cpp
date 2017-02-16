#include "CEventInfoCreator.h"

CEventInfoCreator::CEventInfoCreator() 
{
    m_pEventParamMgr = NULL;
}


CEventInfoCreator::~CEventInfoCreator()
{    
  
}


bool CEventInfoCreator::initialize(const T_CYCLE& cycId,   
                                   //const String<8>& dealDate,
                                   PM_Billing * pEventParamMgr)
{
    m_minRealCycId = cycId; 
    m_pEventParamMgr = pEventParamMgr; 
    
    //m_dealDate = dealDate;
    
    return true;
}


//处理重出帐文件
bool CEventInfoCreator::deal(ReAccountEventInfo& eventInfo)
{
    //cout << "CEventInfoCreator::deal eventInfo.m_cycId" <<  eventInfo.m_cycId << endl;
    m_eventInfo = eventInfo; 
    T_DATE_TIME t_startDateTime, t_endDateTime, t_halfTime;
    
    if(!m_pEventParamMgr->getCycleTime(eventInfo.m_cycId,  
                         t_startDateTime, t_halfTime, t_endDateTime))
    {
        return false;
    }
    else
    {
        //当月是否新开户
        if(eventInfo.m_openDate>t_startDateTime)
        {
            m_startDate = eventInfo.m_openDate.substr(0,8);
            m_startTime = eventInfo.m_openDate.substr(8,6);
        }
        else
        {
            m_startDate = t_startDateTime.substr(0,8);
            m_startTime = t_startDateTime.substr(8,6);
        }
    }
    
    return true;
}

//对获取的订购关系进行后续处理，生成扩展字段(一次将同一用户的同一包月费全部取出合并处理);
bool CEventInfoCreator::deal(vector<MonfeeSpEventInfo>& t_monfeeSpEventInfo)
{
	  
	  T_DATE_TIME t_startDateTime, t_endDateTime, t_halfTime;
    int pp = t_monfeeSpEventInfo[0].m_cycle;
    if(!m_pEventParamMgr->getCycleTime(t_monfeeSpEventInfo[0].m_cycle,  
                         t_startDateTime, t_halfTime, t_endDateTime))
    {
        return false;
    }
    
	  
	  int	  isValid = 0;//是否存在有效的订购记录
	  int   isNormalValid = 0;//订购时间,1:往月订购;2:本月上半月订购;3:本月下半月订购 
	  int   subStopSign = 0;//终止时间,1:当月上半月终止;2:当月下半月终止;3:非当月终止
	  int   subUseDayNum =0;//当月正常使用天数
	  int		isRepeat = 0;//是否重复订购,1:当月重复订购
	  int   isPause = 0;//是否全月暂停，1：当月全月暂停

	  String<14>	cancelTime="20501231000000";//当月订购后的第一次终止的开始时间（暂停也算）（状态‘E’，‘P’）
	  String<14>	priorTime="";//首次订购时间
	  
    String<14>  startTime="";//当月第一次订购开始时间（状态‘A’，‘N’）
    String<14>  endTime="";//当月第一次订购结束时间（状态‘A’，‘N’）
    String<14>  laststartTime="";//当月最后一次订购开始时间（状态‘A’，‘N’）
    String<14>  lastendTime="";//当月最后一次订购结束时间（状态‘A’，‘N’）

  	vector<MonfeeSpEventInfo>::iterator iter;
  		
  	for (iter=t_monfeeSpEventInfo.begin();iter!=t_monfeeSpEventInfo.end();++iter)
  	{
  				if ((iter->m_bizStateCode == 'N' ||
  	  	  	   iter->m_bizStateCode == 'P')&&
  	  	  	   iter->m_startDate <= t_startDateTime &&
  	  	  	   iter->m_endDate >= t_endDateTime )
  	  	  {
  	  	      isPause = 1;
  	  	  }
  		
  				if ( (iter->m_bizStateCode == 'A'||iter->m_bizStateCode == 'N') && 
              iter->m_endDate >= t_startDateTime &&
              iter->m_startDate < t_endDateTime )
  				{
  						isValid= 1;
  
  						if (priorTime == "" )
  						{
  							priorTime= iter->m_priorTime;
  							startTime= iter->m_startDate;
  							endTime= iter->m_endDate;
  							laststartTime= iter->m_startDate;
  							lastendTime= iter->m_endDate;
  						}
  						
  						if (iter->m_startDate < startTime)
  						{
  							startTime= iter->m_startDate;
  							endTime = iter->m_endDate;
  						}
  						
  						if (iter->m_startDate > laststartTime)
  						{
  							laststartTime = iter->m_startDate;
  							lastendTime = iter->m_endDate;
  						}
  				}
  	}
  	
	 for (iter=t_monfeeSpEventInfo.begin();iter!=t_monfeeSpEventInfo.end();++iter)
	  {        
      if ( (iter->m_bizStateCode == 'E') ||(iter->m_bizStateCode == 'P') &&
      		iter->m_startDate >= endTime)
      {
      	  if (iter->m_startDate < cancelTime)
      	  {
      	      cancelTime = iter->m_startDate;
      	  }
      }
    }
    
  if (priorTime < t_startDateTime)
	{
		isNormalValid = 1;
	}
	else if(priorTime <= t_halfTime)
	{
		isNormalValid = 2;
	}
	else if(priorTime > t_halfTime)
	{
		isNormalValid = 3;
	}
		
	if (lastendTime>endTime)
	 {
	 		for (iter=t_monfeeSpEventInfo.begin();iter!=t_monfeeSpEventInfo.end();++iter)
	 	  {
	 				if ( ( iter->m_bizStateCode != 'N' && iter->m_bizStateCode != 'A') && 
	 						 iter->m_startDate >= endTime &&
	 						iter->m_startDate < lastendTime )
	 				{
	 					isRepeat=true;
	 				}
	     }
	 } 
	 
	String<14>  startDate = "20501231000000";//最小开始时间A状态       
  String<14>  endDate   = "19870129000000"  ;//最大结束时间A状态 
  
  String<14>  begintime   = "19870129000000"  ;
  String<14>  endtime   = "19870129000000"  ;
  for (iter=t_monfeeSpEventInfo.begin();iter!=t_monfeeSpEventInfo.end();++iter)
	  { 
	  	int tt = 0;       
      if ( iter->m_bizStateCode == 'A' )
      {
      	  if(iter->m_startDate < t_startDateTime)
      	  	{
      	  		begintime = t_startDateTime;
      	  	}
      	  	
      	  if(iter->m_endDate > t_endDateTime)
      	  	{
      	  		endtime =  t_endDateTime;
      	  	}
      	  	
      	  	getDuration(endtime,begintime,tt);
      	  	subUseDayNum += tt;     	  	
      	  	      	  	
      	  if (iter->m_startDate < startDate)
      	  {
      	      startDate = iter->m_startDate;
      	  }
      	  
      	  if( iter->m_endDate > endDate)
      	  {
      	  	endDate = iter->m_endDate;
      	  }
      }    
    }
    int kk = 0;
    getDuration(t_endDateTime , t_startDateTime ,kk);
    //如果算出来的正常商用天数大于本于实际天数，需要核查！
    if(subUseDayNum > kk)
    	{
    		subUseDayNum = kk;
    		#ifdef _DEBUG_       
         cout<<__FILE__<<__LINE__<<":正常商用天数大于本月实际天数! userId="<<t_monfeeSpEventInfo[0].m_userId.c_str()<<endl;
        #endif
    	}
    
  if (endDate <= t_halfTime)
	{
		subStopSign = 1;
	}
	else if(endDate > t_halfTime)
	{
		subStopSign = 2;
	}
	else if(endDate > t_endDateTime)
	{
		subStopSign = 3;
	}
	    
  char tempchr[512];
  sprintf(tempchr,"%d|%d|%d|%d|%d|%s|%d|%s|%s|%s|%s",isValid,isNormalValid,
          subStopSign,subUseDayNum,isRepeat,cancelTime.c_str(),isPause,startTime.c_str(),endTime.c_str(),
          laststartTime.c_str(),lastendTime.c_str());
  
  
   //合成一条记录
  m_monfeeEventInfo.m_userId         = t_monfeeSpEventInfo[0].m_userId         ; 
  m_monfeeEventInfo.m_spBizCode      = t_monfeeSpEventInfo[0].m_spBizCode      ; 
  m_monfeeEventInfo.m_spCode         = t_monfeeSpEventInfo[0].m_spCode         ; 
  m_monfeeEventInfo.m_spServCode     = t_monfeeSpEventInfo[0].m_spServCode     ; 
  m_monfeeEventInfo.m_sourceType     = t_monfeeSpEventInfo[0].m_sourceType     ; 
  m_monfeeEventInfo.m_startDate      = t_monfeeSpEventInfo[0].m_startDate      ; 
  m_monfeeEventInfo.m_endDate        = t_monfeeSpEventInfo[0].m_endDate        ; 
  m_monfeeEventInfo.m_bizStateCode   = t_monfeeSpEventInfo[0].m_bizStateCode   ; 
  m_monfeeEventInfo.m_billFlag       = t_monfeeSpEventInfo[0].m_billFlag       ; 
  m_monfeeEventInfo.m_ecSerialNumber = t_monfeeSpEventInfo[0].m_ecSerialNumber ; 
  m_monfeeEventInfo.m_ecUserId       = t_monfeeSpEventInfo[0].m_ecUserId       ; 
  m_monfeeEventInfo.m_priorTime      = t_monfeeSpEventInfo[0].m_priorTime      ; 
  m_monfeeEventInfo.m_bizTypeCode    = t_monfeeSpEventInfo[0].m_bizTypeCode    ; 
  m_monfeeEventInfo.m_lastTime       = t_monfeeSpEventInfo[0].m_lastTime       ; 
  strcpy(m_monfeeEventInfo.m_preString,tempchr);                               ;
  
  if( subUseDayNum > 0)
  	m_monfeeEventInfo.m_bizStateCode = 'A';
  else
  	m_monfeeEventInfo.m_bizStateCode = 'N';
   if(m_monfeeEventInfo.m_startDate < t_startDateTime)
   	{
   		m_startDate = t_startDateTime.substr(0,8);
      m_startTime = t_startDateTime.substr(8,6);
   	}
   else
   	{
   		m_startDate = m_monfeeEventInfo.m_startDate.substr(0,8);
      m_startTime = m_monfeeEventInfo.m_startDate.substr(8,6);
   	}
   
   m_endDate   = m_monfeeEventInfo.m_endDate.substr(0,8);
   m_endTime   = m_monfeeEventInfo.m_endDate.substr(8,6);
  	return true;
} 

//处理一次性费用文件
bool CEventInfoCreator::deal(OtpEventInfo& otpEventInfo)
{
    //cout << "CEventInfoCreator::deal eventInfo.m_cycId" <<  eventInfo.m_cycId << endl;
    m_otpEventInfo = otpEventInfo; 
    T_DATE_TIME t_startDateTime, t_endDateTime, t_halfTime;
    
    if(!m_pEventParamMgr->getCycleTime(otpEventInfo.m_cycId,  
                         t_startDateTime, t_halfTime, t_endDateTime))
    {
        return false;
    }
    else
    {
        //当月是否新开户
        if(otpEventInfo.m_openDate>t_startDateTime)
        {
            m_startDate = otpEventInfo.m_openDate.substr(0,8);
            m_startTime = otpEventInfo.m_openDate.substr(8,6);
        }
        else
        {
            m_startDate = t_startDateTime.substr(0,8);
            m_startTime = t_startDateTime.substr(8,6);
        }
    }
    
    return true;
}


//输出重出帐文件
void CEventInfoCreator::outReAcct(const int chNo, ostream& os, const int biz_type)
{  
    const string SPLITSTR = ",";
    #ifdef _DEBUG_
    /*
    cout << "99"                      << SPLITSTR   //sourceType 写死，固定99
         << "21"                      << SPLITSTR   //BizType 写死，固定21
         << m_eventInfo.m_monthEndTag << SPLITSTR
         << m_eventInfo.m_userId      << SPLITSTR
         << m_eventInfo.m_msisdn      << SPLITSTR
         << m_startDate               << SPLITSTR   //开始通话日期
         << m_startTime               << SPLITSTR   //开始通话时间
         << m_eventInfo.m_eparchyCode << SPLITSTR
         << m_eventInfo.m_openDate    << SPLITSTR
         << 0                         << SPLITSTR   //错误编码 写死，固定0
         << chNo                      << endl;
    */
    #endif
    
    os << "99"                      << SPLITSTR   //sourceType 写死，固定99
       << biz_type                      << SPLITSTR   //
       << m_eventInfo.m_monthEndTag << SPLITSTR
       << m_eventInfo.m_userId      << SPLITSTR
       << m_eventInfo.m_msisdn      << SPLITSTR
       << m_startDate               << SPLITSTR   //开始通话日期
       << m_startTime               << SPLITSTR   //开始通话时间
       << m_eventInfo.m_eparchyCode << SPLITSTR
       << m_eventInfo.m_openDate    << SPLITSTR
       << 0                         << SPLITSTR   //错误编码 写死，固定0
       << chNo                      << SPLITSTR
       << m_acctDay                 << endl;
}

//输出一次性费用文件
void CEventInfoCreator::outOtp(const int chNo, ostream& os)
{  
    const string SPLITSTR = ",";
    #ifdef _DEBUG_
    /*
    cout << "98"                           << SPLITSTR   //sourceType 写死，固定99
         << "10"                           << SPLITSTR   //BizType 写死，固定21
         //<< m_otpEventInfo.m_monthEndTag << SPLITSTR
         << m_otpEventInfo.m_userId        << SPLITSTR
         << m_otpEventInfo.m_msisdn        << SPLITSTR
         << m_startDate                    << SPLITSTR   //开始通话日期
         << m_startTime                    << SPLITSTR   //开始通话时间
         << m_otpEventInfo.m_eparchyCode   << SPLITSTR
         << m_otpEventInfo.m_openDate      << SPLITSTR
         << 0                              << SPLITSTR   //错误编码 写死，固定0
         << chNo                           << SPLITSTR
         << m_otpEventInfo.m_itemCode      << SPLITSTR
         << m_otpEventInfo.m_fee           << endl;
    */
    #endif
    
    os << "98"                         << SPLITSTR   //sourceType 写死，固定99
       << "10"                         << SPLITSTR   //BizType 写死，固定21
       //<< m_otpEventInfo.m_monthEndTag << SPLITSTR
       << m_otpEventInfo.m_userId      << SPLITSTR
       << m_otpEventInfo.m_msisdn      << SPLITSTR
       << m_startDate                  << SPLITSTR   //开始通话日期
       << m_startTime                  << SPLITSTR   //开始通话时间
       << m_otpEventInfo.m_eparchyCode << SPLITSTR
       << m_otpEventInfo.m_openDate    << SPLITSTR
       << 0                            << SPLITSTR   //错误编码 写死，固定0
       << chNo                         << SPLITSTR
       << m_otpEventInfo.m_itemCode    << SPLITSTR
       << m_otpEventInfo.m_fee         << SPLITSTR
       << m_acctDay                    << endl;
}

void CEventInfoCreator::outMonfeeSp(const int chNo, ostream& os,const T_BIZ_TYPE &eventtype)
{  
    const string SPLITSTR = ",";
    #ifdef _DEBUG_
    cout << "61"                      << SPLITSTR   //sourceType 写死，固定61
       << eventtype.c_str()         << SPLITSTR 
       << m_monfeeEventInfo.m_userId        << SPLITSTR
       << m_monfeeEventInfo.m_spCode        << SPLITSTR
       << m_monfeeEventInfo.m_spBizCode     << SPLITSTR
       << m_monfeeEventInfo.m_spServCode    << SPLITSTR
       << m_monfeeEventInfo.m_bizTypeCode   << SPLITSTR 
       << m_monfeeEventInfo.m_bizStateCode  << SPLITSTR        
       << m_startDate                       << SPLITSTR  
       << m_startTime                       << SPLITSTR
       << m_endDate                         << SPLITSTR  
       << m_endTime                         << SPLITSTR 
       << m_monfeeEventInfo.m_billFlag      << SPLITSTR
       << m_monfeeEventInfo.m_ecUserId      << SPLITSTR
       << m_monfeeEventInfo.m_ecSerialNumber<< SPLITSTR
       << m_monfeeEventInfo.m_priorTime     << SPLITSTR
       << m_monfeeEventInfo.m_lastTime      << SPLITSTR
       <<m_monfeeEventInfo.m_sourceType     << SPLITSTR
       << m_monfeeEventInfo.m_preString     << endl; 
    #endif
    
    os << "61"                      << SPLITSTR   //sourceType 写死，固定61
       << eventtype.c_str()         << SPLITSTR 
       << m_monfeeEventInfo.m_userId        << SPLITSTR
       << m_monfeeEventInfo.m_spCode        << SPLITSTR
       << m_monfeeEventInfo.m_spBizCode     << SPLITSTR
       << m_monfeeEventInfo.m_spServCode    << SPLITSTR
       << m_monfeeEventInfo.m_bizTypeCode   << SPLITSTR 
       << m_monfeeEventInfo.m_bizStateCode  << SPLITSTR        
       << m_startDate                       << SPLITSTR  
       << m_startTime                       << SPLITSTR
       << m_endDate                         << SPLITSTR  
       << m_endTime                         << SPLITSTR 
       << m_monfeeEventInfo.m_billFlag      << SPLITSTR
       << m_monfeeEventInfo.m_ecUserId      << SPLITSTR
       << m_monfeeEventInfo.m_ecSerialNumber<< SPLITSTR
       << m_monfeeEventInfo.m_priorTime     << SPLITSTR
       << m_monfeeEventInfo.m_lastTime      << SPLITSTR
       <<m_monfeeEventInfo.m_sourceType     << SPLITSTR
       << m_monfeeEventInfo.m_preString     << endl; 
}

void CEventInfoCreator::getDuration(const String<14>& endDate, 
                          const String<14>& startDate,
                          int& dayNum)
{
    time_t endsecond,startsecond;  
    int    endyear,endmonth,endday,endhour,endminute,endseconds;
    int    startyear,startmonth,startday;
    struct tm p_end;
    struct tm p_start;
    
    endyear = atoi(endDate.substr(0,4).c_str());
    endmonth = atoi(endDate.substr(4,2).c_str());
    endday = atoi(endDate.substr(6,2).c_str());
    endhour = atoi(endDate.substr(8,2).c_str());
    endminute = atoi(endDate.substr(10,2).c_str());
    endseconds = atoi(endDate.substr(12,2).c_str());
    
    startyear = atoi(startDate.substr(0,4).c_str());
    startmonth = atoi(startDate.substr(4,2).c_str());
    startday = atoi(startDate.substr(6,2).c_str());  
    
    p_end.tm_sec = 0;
    p_end.tm_min = 0;
    p_end.tm_hour = 0;
    p_end.tm_mday = endday;
    p_end.tm_mon = endmonth - 1;
    p_end.tm_year = endyear - 1900;
    
    endsecond = mktime(&p_end);
    
    p_start.tm_sec = 0;
    p_start.tm_min = 0;
    p_start.tm_hour = 0;
    p_start.tm_mday = startday;
    p_start.tm_mon = startmonth - 1;
    p_start.tm_year = startyear - 1900;  
    
    startsecond = mktime(&p_start);
    
    if (endhour==0 && endminute==0 && endseconds==0)
    {
        dayNum = difftime(endsecond,startsecond)/24/3600;
    }
    else
    {
        dayNum = difftime(endsecond,startsecond)/24/3600 + 1;
    }
}