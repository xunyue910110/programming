#include "AddupCycleRule.h"
#include "RateGlobalInfo.h"
#include "RateError.h"

void AddupCycleRule::dump(char *prefix)
{
	char myPrefix[256] = "  ";	
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----AddupCycleRule : " <<m_id<<" ----" <<endl;
	cout <<myPrefix<<"m_id          = "<< m_id <<endl;
	cout <<myPrefix<<"m_cycleType   = "<< m_cycleType<<endl;
	cout <<myPrefix<<"m_defineTag   = "<< m_defineTag<<endl;
	cout <<myPrefix<<"m_cycleNum    = "<< m_cycleNum<<endl;
	cout <<myPrefix<<"m_offsetValue = "<< m_offsetValue<<endl;
	cout <<myPrefix<<"}" <<endl;	
}

//根据话单起始时间、周期定义信息 计算 当前话单所在的周期信息
//Modified By Sunam 2009/12/29 10:20:16 根据指定的时间r_cdrStartTime 求累计量周期
//bool AddupCycleRule::getSumTimeInfo(RateGlobalInfo       *r_pGlobalInfo,
//                                    const RateTpInfo     *r_rateTpInfo,
//                                          long           &r_cycleBeginTime,
//                                          long           &r_cycleEndTime) const
bool AddupCycleRule::getSumTimeInfo(RateGlobalInfo       *r_pGlobalInfo,
                                    const RateTpInfo     *r_rateTpInfo,
                                    const T_DATE_TIME    &r_cdrStartTime,
                                          long           &r_cycleBeginTime,
                                          long           &r_cycleEndTime) const
{
  int   t_offFlag;
  char  t_offSetStr[32+1],t_tmpStr[2+1];
  char  t_errMsg[64];
  const T_DATE_TIME &t_tpObjTime = r_rateTpInfo->m_pUserTp->m_beginTime;
  //1. 取偏移量信息
  if(m_defineTag == ADDUP_CYCLE_TAG_FIX)
  {
    sprintf(t_offSetStr,"%010s",m_offsetValue.c_str());
  }else if(m_defineTag == ADDUP_CYCLE_TAG_USER)
  {
    T_TP_ID t_paramId = atoi(m_offsetValue.c_str());
    const char * t_pOffSet = r_rateTpInfo->getParamValue(t_paramId);
    if(t_pOffSet == NULL)
    {
      sprintf(t_errMsg,"周期个性化偏移量参数未实例化 t_paramId=%s",
                              m_offsetValue.c_str());
      #ifdef _DEBUG_
        cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
      #endif
      r_pGlobalInfo->setErrInfo(E_CYCRULE_OFFSET,t_errMsg);
      return false;
    }else
    { 
      sprintf(t_offSetStr,"%010s",t_pOffSet);
    }
  }else if(m_defineTag == ADDUP_CYCLE_TAG_TP)
  {
    //对于用户资费时间,先取MMDDHHMISS 10位
    sprintf(t_offSetStr,"%s",t_tpObjTime.c_str()+4);
    //将月和天 -1
    int t_month,t_day;
    //int t_hour,t_min,t_sec;
    snprintf(t_tmpStr,2+1,"%s",t_offSetStr);  t_month=atoi(t_tmpStr)-1;
    snprintf(t_tmpStr,2+1,"%s",t_offSetStr+2);t_day  =atoi(t_tmpStr)-1;
    //snprintf(t_tmpStr,2+1,"%s",t_offSetStr+4);t_hour =atoi(t_tmpStr);
    //snprintf(t_tmpStr,2+1,"%s",t_offSetStr+6);t_min  =atoi(t_tmpStr);
    //snprintf(t_tmpStr,2+1,"%s",t_offSetStr+8);t_sec  =atoi(t_tmpStr);
    if(m_cycleType == ADDUP_CYCLE_TYPE_YEAR)
    { 
    }else if(m_cycleType == ADDUP_CYCLE_TYPE_YEAR_HALF)
    {
      t_month = t_month%6;
    }else if(m_cycleType == ADDUP_CYCLE_TYPE_QUARTER)
    {
      t_month = t_month%3;
    }else if(m_cycleType == ADDUP_CYCLE_TYPE_MONTH_DOUBLE)
    {
      t_month = t_month%2;
    }else if(m_cycleType == ADDUP_CYCLE_TYPE_MONTH)
    {
      t_month = 0;
    }else if(m_cycleType == ADDUP_CYCLE_TYPE_DAY)
    {
      t_month = 0;
      t_day   = 0;
    }else //其他暂不支持  
    { 
      sprintf(t_errMsg,"周期类型不支持周期=%d 类型为%c!",
                       m_id,
                       m_cycleType);
      #ifdef _DEBUG_
        cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
      #endif
      r_pGlobalInfo->setErrInfo(E_CYCRULE_NOTSUPPORT,t_errMsg);
      return false;
    }
    sprintf(t_offSetStr,"%02d%02d%s",t_month,t_day,t_tpObjTime.c_str()+8);
    t_offFlag = atoi(m_offsetValue.c_str());
    if(t_offFlag == 1) //精确到分
      sprintf(t_offSetStr+8,"00");
    else if(t_offFlag == 2) //精确到小时
      sprintf(t_offSetStr+6,"0000");
    else if(t_offFlag == 3) //精确到天
      sprintf(t_offSetStr+4,"000000");
    else if(t_offFlag == 4) //精确到月
      sprintf(t_offSetStr+2,"00000000");
    //else if(t_offFlag == 0) //精确到秒 不需要调整
  }else
  {
    sprintf(t_errMsg,"周期偏移量来源类型错误 周期=%d 偏移量来源类型为%c!",
                     m_id,
                     m_defineTag);
    #ifdef _DEBUG_
      cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
    #endif
    r_pGlobalInfo->setErrInfo(E_CYCRULE_NOTSUPPORT,t_errMsg);
    return false;
  }
  long t_startDateTime;
  //2. 根据资费实例计算第一个周期时间点(不计偏移量) [t_startDateTime,t_endDateTime]
  if(getFSumTimeInfo(r_pGlobalInfo,t_tpObjTime,t_startDateTime)==false)
  {
    return false;
  }    
  //3. 增加偏移量的第一个周期时间点
  addOffSetTime(t_offSetStr,t_startDateTime);
  //4. 结合话单时间计算当前话单所处的时间点
  //Modified By Sunam 2009/12/29 10:21:57 根据指定的时间r_cdrStartTime 求累计量周期
  //getSumTimeInfo(r_pGlobalInfo,r_pGlobalInfo->m_beginDateTime,t_startDateTime,r_cycleBeginTime,r_cycleEndTime);
  getSumTimeInfo(r_pGlobalInfo,r_cdrStartTime,t_startDateTime,r_cycleBeginTime,r_cycleEndTime);
  
  return true;
}


//根据计费实例时间计算第一个周期的时间点
bool AddupCycleRule::getFSumTimeInfo( RateGlobalInfo      *r_pGlobalInfo,
                                     const T_DATE_TIME    &r_tpObjTime,
                                           long           &r_fStartTime) const
{
  char t_timeStr[14+1];
  long t_lValue;
  switch(m_cycleType )//周期类型
  {
    case ADDUP_CYCLE_TYPE_MONTH         : //月
      {
        snprintf(t_timeStr,6+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+6,"01000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    case ADDUP_CYCLE_TYPE_YEAR          ://年
      {
        snprintf(t_timeStr,4+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+4,"0101000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    case ADDUP_CYCLE_TYPE_YEAR_HALF     ://半年
    case ADDUP_CYCLE_TYPE_QUARTER       ://季度
    case ADDUP_CYCLE_TYPE_MONTH_DOUBLE  ://双月
      {
        int t_monthUnit = 1;
        if(m_cycleType == ADDUP_CYCLE_TYPE_YEAR_HALF)
          t_monthUnit = 6;
        else if(m_cycleType == ADDUP_CYCLE_TYPE_QUARTER)  
          t_monthUnit = 3;
        else if(m_cycleType == ADDUP_CYCLE_TYPE_MONTH_DOUBLE)  
          t_monthUnit = 2;
        snprintf(t_timeStr,6+1,"%s",r_tpObjTime.c_str());
        t_lValue = atoi(t_timeStr);
        t_lValue = ((t_lValue/100)*100)+((t_lValue%100-1)/t_monthUnit)*t_monthUnit + 1;
        sprintf(t_timeStr,"%06ld01000000",t_lValue);
        r_fStartTime = atol(t_timeStr);
        break;
      }
    case ADDUP_CYCLE_TYPE_DAY           : //天 
      {
        snprintf(t_timeStr,8+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+8,"000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    //以下周期类型暂不实现
    case ADDUP_CYCLE_TYPE_SECOND        : //秒
    case ADDUP_CYCLE_TYPE_MINUTE        : //分种
    case ADDUP_CYCLE_TYPE_HOUR          : //小时
    case ADDUP_CYCLE_TYPE_WEEK          : //周
    case ADDUP_CYCLE_TYPE_DAY_TEN       : //旬
    case ADDUP_CYCLE_TYPE_MONTH_HALF    : //半月
    default:
      {
        char t_errMsg[64];
        sprintf(t_errMsg,"周期类型不支持 类型为%c!",m_cycleType);
        #ifdef _DEBUG_
          cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
        #endif
        r_pGlobalInfo->setErrInfo(E_CYCRULE_NOTSUPPORT,t_errMsg);
        return false;
        break;
      }
  }  
  return true;
}
//偏移量r_offSetValue格式MMDDHHMISS,
//r_dateTime  YYYYMMDDHHMISS
void AddupCycleRule::addOffSetTime(const char *r_offSetValue,long &r_dateTime) const
{
  //将月和天 -1
  int t_year0,t_month0,t_day0,t_hour0,t_min0,t_sec0;
  int t_month1,t_day1,t_hour1,t_min1,t_sec1;
  char  t_timeStr[14+1],t_tmpStr[4+1];
  sprintf(t_timeStr,"%014ld",r_dateTime);
  snprintf(t_tmpStr,4+1,"%s",t_timeStr);   t_year0 =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",t_timeStr+4); t_month0=atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",t_timeStr+6); t_day0  =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",t_timeStr+8); t_hour0 =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",t_timeStr+10);t_min0  =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",t_timeStr+12);t_sec0  =atoi(t_tmpStr);

  snprintf(t_tmpStr,2+1,"%s",r_offSetValue);  t_month1=atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",r_offSetValue+2);t_day1  =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",r_offSetValue+4);t_hour1 =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",r_offSetValue+6);t_min1  =atoi(t_tmpStr);
  snprintf(t_tmpStr,2+1,"%s",r_offSetValue+8);t_sec1  =atoi(t_tmpStr);
  struct tm t_tm;
  t_tm.tm_year=t_year0-1900;
  t_tm.tm_mon =t_month0+t_month1-1;
  t_tm.tm_mday=t_day0+t_day1;
  t_tm.tm_hour=t_hour0+t_hour1;
  t_tm.tm_min =t_min0+t_min1;
  t_tm.tm_sec =t_sec0+t_sec1;
  t_tm.tm_isdst = -1;
  mktime(&t_tm);
  
  
  sprintf(t_timeStr,"%04d%02d%02d%02d%02d%02d",t_tm.tm_year+1900,
          t_tm.tm_mon+1,t_tm.tm_mday,t_tm.tm_hour,t_tm.tm_min,t_tm.tm_sec);
  r_dateTime = atol(t_timeStr);
}
  	//取余
long AddupCycleRule::absMode(const long &r_number,const int &r_mode) const
{
  assert(r_mode>0);
  if(r_mode==1) return 0;
  if(r_number>=0) 
    return (r_number%r_mode);
  else 
    return (r_mode+r_number%r_mode);
}
//根据第一个周期、话单时间、周期定义计算当前话单所在周期
void AddupCycleRule::getSumTimeInfo( RateGlobalInfo      *r_pGlobalInfo,
                                    const T_DATE_TIME    &r_cdrTime,
                                    const long           &r_fStartTime,
                                          long           &r_cycleBeginTime,
                                          long           &r_cycleEndTime) const
{
  char t_timeStr[14+1];
  long t_lValue0,t_lValue,t_diffValue;
  long t_dValue0,t_dValue;
  switch(m_cycleType )//周期类型
  {
    case ADDUP_CYCLE_TYPE_MONTH         : //月
      {
        snprintf(t_timeStr,6+1,"%s",r_cdrTime.c_str());
        t_lValue  = atol(t_timeStr);
        t_dValue  = atol(r_cdrTime.c_str()+6);
        t_lValue0 = r_fStartTime/100000000;
        t_dValue0 = r_fStartTime%100000000;
        //两个时间相差月数
        t_diffValue = (t_lValue/100)*12+t_lValue%100 - 
                      ((t_lValue0/100)*12+t_lValue0%100 ) -
                      (t_dValue<t_dValue0?1:0);
        //两个周期起始时间相差的月数(规整)
        t_diffValue-= absMode(t_diffValue,m_cycleNum);
        r_pGlobalInfo->addMonth(t_diffValue,t_lValue0);
        r_cycleBeginTime = t_lValue0*100000000 + r_fStartTime%100000000;
        r_pGlobalInfo->addMonth(m_cycleNum,t_lValue0);
        r_cycleEndTime   = t_lValue0*100000000 + r_fStartTime%100000000;
        break;
      }
    case ADDUP_CYCLE_TYPE_YEAR          ://年
      {
        snprintf(t_timeStr,4+1,"%s",r_cdrTime.c_str());
        t_lValue = atol(t_timeStr);
        t_dValue = atol(r_cdrTime.c_str()+4);
        t_lValue0 = r_fStartTime/10000000000;
        t_dValue0 = r_fStartTime%10000000000;
        t_diffValue = t_lValue - t_lValue0 -(t_dValue<t_dValue0?1:0);
        t_diffValue-= absMode(t_diffValue,m_cycleNum);
        t_lValue0 += t_diffValue;
        r_cycleBeginTime = t_lValue0*10000000000+r_fStartTime%10000000000;
        t_lValue0+=m_cycleNum;
        r_cycleEndTime   = t_lValue0*10000000000+r_fStartTime%10000000000;
        break;
      }
    case ADDUP_CYCLE_TYPE_YEAR_HALF     ://半年
    case ADDUP_CYCLE_TYPE_QUARTER       ://季度
    case ADDUP_CYCLE_TYPE_MONTH_DOUBLE  ://双月
      {
        int t_monthUnit = 1;
        if(m_cycleType == ADDUP_CYCLE_TYPE_YEAR_HALF)
          t_monthUnit = 6;
        else if(m_cycleType == ADDUP_CYCLE_TYPE_QUARTER)  
          t_monthUnit = 3;
        else if(m_cycleType == ADDUP_CYCLE_TYPE_MONTH_DOUBLE)  
          t_monthUnit = 2;
        snprintf(t_timeStr,6+1,"%s",r_cdrTime.c_str());
        t_lValue    = atol(t_timeStr);
        t_dValue    = atol(r_cdrTime.c_str()+6);
        t_lValue0   = r_fStartTime/100000000;
        t_dValue0   = r_fStartTime%100000000;

        t_diffValue = t_lValue - t_lValue0 -(t_dValue<t_dValue0?1:0);
        t_diffValue-= absMode(t_diffValue,t_monthUnit*m_cycleNum );
        //按照周期跨度规整
        r_pGlobalInfo->addMonth(t_diffValue,t_lValue0);
        r_cycleBeginTime = t_lValue0*100000000 + r_fStartTime%100000000;
        r_pGlobalInfo->addMonth(t_monthUnit*m_cycleNum,t_lValue0);
        r_cycleEndTime   = t_lValue0*100000000 + r_fStartTime%100000000;
        break;
      }
    case ADDUP_CYCLE_TYPE_DAY           : //天 
      {
        snprintf(t_timeStr,8+1,"%s",r_cdrTime.c_str());
        t_lValue    = atol(t_timeStr); //YYYYMMDD
        t_dValue    = atol(r_cdrTime.c_str()+8);  //HHMISS
        t_lValue0   = r_fStartTime/1000000;  //资费实例YYYYMM
        t_dValue0   = r_fStartTime%1000000;//资费实例HHMISS
        t_diffValue = r_pGlobalInfo->diffDays(t_lValue,t_lValue0) -(t_dValue<t_dValue0?1:0);
        t_diffValue-= absMode(t_diffValue,m_cycleNum);
        r_pGlobalInfo->addDay(t_diffValue,t_lValue0);
        r_cycleBeginTime = t_lValue0*1000000 + r_fStartTime%1000000;
        r_pGlobalInfo->addDay(m_cycleNum,t_lValue0);
        r_cycleEndTime = t_lValue0*1000000 + r_fStartTime%1000000;
        break;
      }
    default:
        break;
  }  
}



