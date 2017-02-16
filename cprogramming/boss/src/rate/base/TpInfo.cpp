#include "TpInfo.h"
#include "RateConstDef.h"
#include "EventFeePolicy.h"
#include "base/StringUtil.h"

ostream& operator<<(ostream& r_os,const UserTpParam &r_right)
{
  r_os<<"tpObjId="<<r_right.m_tpObjId<<" , "
      <<"tpParamId="<<r_right.m_tpParamId<<" , "
      <<"valueType="<<r_right.m_valueType<<" , "
      <<"tpValue="<<r_right.m_tpValue<<" , "
      <<"beginTime="<<r_right.m_beginTime<<" , "
      <<"endTime="<<r_right.m_endTime<<" , "
      <<"relatinType="<<r_right.m_relatinType<<" , "
      <<"levelType="<<r_right.m_levelType<<" , "
      <<"memberType="<<r_right.m_memberType<<endl;
  return r_os;
}

UserTp::UserTp()
{
  m_validFlag = true;
}
UserTp::UserTp(const T_TP_ID &r_tpId,const T_SERVER_ID &r_userId)
{
  m_paramList.clear();
  m_tpObjId   = 0;
  m_tpId      = r_tpId;
  m_beginTime = 20000101000000;
  m_endTime   = 20990101000000;
  m_idType    = USER_ID_TYPE;
  m_userId    = r_userId; 
  m_validFlag = true;
  m_productId = 0;
  m_servBund_Id=0;
  m_servId     =0;
  m_feeBund_Id =0;
  m_relationVpnId = NULL_VPN_ID;
  m_halfInNetFlag = '0';
  m_firstMonthFlag= '0';
  m_subTpType     = TP_TYPE_NORMAL;
  m_cellFlag      = FLAG_NO;
  m_vpnFlag       = VPN_FLAG_OUT;
  m_memberType_A  = VPN_MEMTYPE_NULL;
  m_memberType_B  = VPN_MEMTYPE_NULL;
	m_lastVpnParentIDA	= NULL_VPN_ID;
	m_lastVpnParentIDB	= NULL_VPN_ID;
}

bool UserTp::isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const
{
  if(m_beginTime > r_endTime ||
     m_endTime   < r_beginTime)
     // modified by liugj 20090523m_endTime   <= r_beginTime)
  {
    return false;
  }
  return true;
}

bool UserTp::isValid4Event24(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const
{
  if(m_beginTime > r_endTime ||
     m_endTime   <= r_beginTime)
  {
    return false;
  }
  return true;
}


bool UserTp::isValid(const int &r_beginDate) const
{
  char t_dayStr[8+1];
  int  t_iDay;
  snprintf(t_dayStr,8+1,"%s",m_beginTime.c_str()); t_iDay=atoi(t_dayStr);
  if(t_iDay>r_beginDate) return false;
  snprintf(t_dayStr,8+1,"%s",m_endTime.c_str()); t_iDay=atoi(t_dayStr);
  if(t_iDay<r_beginDate) return false;
  return true;
}
ostream& operator<<(ostream& r_os,const UserTp &r_right)
{
  r_os<<" m_tpObjId="<<r_right.m_tpObjId
      <<" m_tpId="<<r_right.m_tpId
      <<" m_beginTime="<<r_right.m_beginTime
      <<" m_endTime="<<r_right.m_endTime
      <<" m_idType="<<r_right.m_idType
      <<" m_userId="<<r_right.m_userId
      <<" m_productId="<<r_right.m_productId
      <<" m_servBund_Id="<<r_right.m_servBund_Id
      <<" m_servId="<<r_right.m_servId
      <<" m_feeBund_Id="<<r_right.m_feeBund_Id
      <<" m_relationVpnId="<<r_right.m_relationVpnId
      <<" m_halfInNetFlag="<<r_right.m_halfInNetFlag
      <<" m_firstMonthFlag="<<r_right.m_firstMonthFlag
      <<" m_subTpType="<<r_right.m_subTpType
      <<" m_cellFlag="<<r_right.m_cellFlag
      <<" m_vpnFlag ="<<r_right.m_vpnFlag
      <<" m_memberType_A="<<r_right.m_memberType_A
      <<" m_memberType_B="<<r_right.m_memberType_B
      <<" m_lastVpnParentIDA="<<r_right.m_lastVpnParentIDA
      <<" m_lastVpnParentIDB="<<r_right.m_lastVpnParentIDB
      <<endl;
  for(vector<UserTpParam>::const_iterator t_itr=r_right.m_paramList.begin();
      t_itr!=r_right.m_paramList.end();++t_itr)
  {
    r_os<<" CRM参数： "<<*t_itr;
  }
  return r_os;
}

int operator==(const RateTpParam &r_left,const RateTpParam &r_right)
{
  return (r_left.m_rateParamId == r_right.m_rateParamId);
}

int operator<(const RateTpParam &r_left,const RateTpParam &r_right)
{
  return (r_left.m_rateParamId < r_right.m_rateParamId);
}

//根据事件判断是否有效
bool RateTpInfo::isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const
{
  return m_pUserTp->isValid(r_beginTime,r_endTime);
}
//added by liugj 20090523
bool RateTpInfo::isValid4Event24(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const
{
  return m_pUserTp->isValid4Event24(r_beginTime,r_endTime);
}
bool RateTpInfo::isValid(const int &r_beginDate) const
{
  return m_pUserTp->isValid(r_beginDate);
}
ostream& operator<<(ostream& r_os,const RateTpParam &r_right)
{
  r_os<<"rateTpNo="<<r_right.m_rateTpNo<<" , "
      <<"rateParamId="<<r_right.m_rateParamId<<" , "
      <<"source="<<r_right.m_source<<" , "
      <<"valueType="<<r_right.m_valueType<<" , "
      <<"userParamId="<<r_right.m_userParamId<<" , "
      <<"value="<<r_right.m_value<<" , "
      <<"UserTpParamId="<<r_right.m_pUserParam->m_tpParamId<<endl;
  return r_os;
}
//根据参数ID取对应的值，找不到返回NULL
const char* RateTpInfo::getParamValue(const T_TP_ID &r_paramId) const
{
  for(vector<RateTpParam>::const_iterator r_itr=m_rateTpParam.begin();
      r_itr!=m_rateTpParam.end();++r_itr)
  {
    if(r_itr->m_rateParamId == r_paramId)
    {
      //Modified By Sunam 2009/9/11 19:30:58
      /*
      if(r_itr->m_source == PARAM_SOURCE_CRM)
      {
        return r_itr->m_pUserParam->m_tpValue.c_str();
      }else //固定值
      {
        return r_itr->m_value.c_str();
      }
      */
      if(r_itr->m_source == PARAM_SOURCE_CRM1)
      {
        //资费实例中无有效值时，取fixedValue
        if (r_itr->m_pUserParam->m_tpValue == "?")
        {
            return r_itr->m_value.c_str();
        }
        return r_itr->m_pUserParam->m_tpValue.c_str();
      }
      if(r_itr->m_source == PARAM_SOURCE_CRM2)
      {
        //充许参数缺失，取fixedValue
        if (r_itr->m_pUserParam == NULL)
        {
            return r_itr->m_value.c_str();
        }
        return r_itr->m_pUserParam->m_tpValue.c_str();
      }
      else //固定值
      {
        return r_itr->m_value.c_str();
      }
    }
  }
  return NULL;
}

int operator<(const RateTpInfo &r_left,const RateTpInfo &r_right)
{
  if(r_left.m_priority < r_right.m_priority) return true;
  if(r_left.m_priority > r_right.m_priority) return false;
    
  return (r_left.m_rateTpNo < r_right.m_rateTpNo) ;
}

ostream& operator<<(ostream& r_os,const RateTpInfo &r_right)
{
  r_os<<"用户资费实例="<<*(r_right.m_pUserTp);
  r_os<<"m_rateTpId="<<r_right.m_rateTpId<<" , "
      <<"m_eventType="<<r_right.m_eventType<<" , "
      <<"m_rateTpNo="<<r_right.m_rateTpNo<<" , "
      <<"m_priority="<<r_right.m_priority<<" , "
      <<"m_compMethod="<<r_right.m_compMethod<<" , "
      <<"m_execMode="<<r_right.m_execMode<<" , "
      <<"m_realRentFlag="<<r_right.m_realRentFlag<<" , "
      <<"m_monthRent="<<r_right.m_monthRent<<" , "
      <<"m_rentCompMethod="<<r_right.m_rentCompMethod<<" , "
      <<"m_rateRoleCode="<<r_right.m_rateRoleCode<<" , "
      <<"EventFeePolicyId="<<r_right.m_pEventTp->m_id<<" , "
      <<"m_isDealed="<<r_right.m_isDealed<<endl;
  for(vector<RateTpParam>::const_iterator t_itr=r_right.m_rateTpParam.begin(); 
      t_itr!=r_right.m_rateTpParam.end();++t_itr)
  {
    r_os<<"批价政策参数: "<<*t_itr;
  }
  return r_os;
}


//根据日期计算实扣月租 add by gaojf 
//r_compMethod: 计算方法 
//r_openDate  : 开户日期 
//Modified By Sunam 2009/11/10 10:51:47
//long RateTpInfo::getRentValue(const char &r_openFlag,const T_DATE_TIME &r_openDate,
//                              const int  &r_monthDay) const
long RateTpInfo::getRentValue(const char &r_openFlag,const T_DATE_TIME &r_openDate,
                              const int  &r_monthDay,const T_DATE_TIME &r_earliestOpenDate) const
{
  //Modified By Sunam 2009/11/10 10:52:39
  //long t_rentValue = getValue(m_monthRent,m_rentCompMethod,r_openFlag,r_openDate,r_monthDay);
  long t_rentValue = getValue(m_monthRent,m_rentCompMethod,r_openFlag,r_openDate,r_monthDay,r_earliestOpenDate);
  t_rentValue -= t_rentValue&10;
  return t_rentValue;
}

//Modified By Sunam 2009/11/10 10:54:27
//long RateTpInfo::getValue(const long        &r_value,
//                          const char        &r_compMethod,
//                          const char        &r_openFlag,
//                          const T_DATE_TIME &r_openDate,
//                          const int         &r_monthDay) const
long RateTpInfo::getValue(const long        &r_value,
                          const char        &r_compMethod,
                          const char        &r_openFlag,
                          const T_DATE_TIME &r_openDate,
                          const int         &r_monthDay,
                          const T_DATE_TIME &r_earliestOpenDate,
                          const int         &r_billCycle,
                          const char        *r_param) const
{
  //modified by gaojf 2009-10-7 11:38:11
  //if(m_realRentFlag== FLAG_NO) return 0;
  int  t_validDay,t_tpValidDay;
  char t_chDay[8+1];
  long t_value;
  int  t_tpEndDay;
  //Added By Sunam 2009/12/24 16:34:22
  t_tpValidDay = r_monthDay;
  if (strlen(r_param) >= 2)
  {
  	  if (r_param[1] == '1')
  	  {
  	      snprintf(t_chDay,8+1,"%s",m_pUserTp->m_endTime.c_str());
  	      //如果结束日期为当前账期，则有效天数应为结日期
  	      if (r_billCycle == atoi(t_chDay) / 100)
  	      {
          	  t_tpValidDay = atoi(t_chDay+6);
          }
      }
  }
  switch(r_compMethod)
  {
    case RENT_COMPMETHOD_NORMAL: //全月
      return r_value;
    case RENT_COMPMETHOD_HALF: //上下半月
      if(m_pUserTp->m_halfInNetFlag[0] =='1')
      { //除以2并按10规整
        return r_value/2;
      }else
      {
        return r_value;
      }
      break;
    case RENT_COMPMETHOD_OPENDATE: //入网日期
      if(r_openFlag=='1')
      {
        snprintf(t_chDay,8+1,"%s",r_openDate.c_str());
        //Modified By Sunam 2009/12/24 16:39:49
        //t_validDay = r_monthDay-atoi(t_chDay+6)+1;
        t_validDay = t_tpValidDay-atoi(t_chDay+6)+1;
        t_validDay = (t_validDay>r_monthDay)?r_monthDay:
                      ((t_validDay<0)?0:t_validDay);
        return (r_value*t_validDay)/r_monthDay;
      }else
      {
        return r_value;
      } 
    case RENT_COMPMETHOD_TPDATE://套餐日期
      if(m_pUserTp->m_firstMonthFlag=='1')
      {
        snprintf(t_chDay,8+1,"%s",m_pUserTp->m_beginTime.c_str());
        //Modified By Sunam 2009/12/24 16:39:49
        //t_validDay = r_monthDay-atoi(t_chDay+6)+1;
        t_validDay = t_tpValidDay-atoi(t_chDay+6)+1;
        t_validDay = (t_validDay>r_monthDay)?r_monthDay:
                      ((t_validDay<0)?0:t_validDay);
        return (r_value*t_validDay)/r_monthDay;
      }else
      {
        return r_value;
      }
    //Added By Sunam 2009/11/10 10:59:28
    case RENT_COMPMETHOD_EARLIESTOPENDATE: //按当月最早入网时间折算
      if(m_pUserTp->m_firstMonthFlag=="1")
      {
      	  //首月不折算
          if ((r_param == NULL) || (r_param[0] == '0'))
          {
          	  return r_value;
          }
          else if (r_param[0] == '1')
          {
          	  snprintf(t_chDay,8+1,"%s",m_pUserTp->m_beginTime.c_str());
          	  //Modified By Sunam 2009/12/24 16:39:49
          	  //t_validDay = r_monthDay-atoi(t_chDay+6)+1;
          	  t_validDay = t_tpValidDay-atoi(t_chDay+6)+1;
          	  t_validDay = (t_validDay>r_monthDay) ? r_monthDay : ((t_validDay<0)?0:t_validDay);
          	  return (r_value*t_validDay)/r_monthDay;
          }
          else
		  {
		  	  return r_value;
		  }
      }
      //非首月
      else
      {
      	  //当月无开机状态
      	  if (r_earliestOpenDate == "0")
      	  {
      	  	  return 0;
      	  }
          snprintf(t_chDay,8+1,"%s",r_earliestOpenDate.c_str());
          //Modified By Sunam 2009/12/24 16:39:49
          //t_validDay = r_monthDay-atoi(t_chDay+6)+1;
          t_validDay = t_tpValidDay-atoi(t_chDay+6)+1;
          t_validDay = (t_validDay>r_monthDay) ? r_monthDay : ((t_validDay<0)?0:t_validDay);
          return (r_value*t_validDay)/r_monthDay;
      }
    //Added By Sunam 2009/11/10 10:59:28
    case RENT_COMPMETHOD_SPEACIALDATE: //按当月最早入网时间折算
    	snprintf(t_chDay,8+1,"%s",r_earliestOpenDate.c_str());
    	//Modified By Sunam 2009/12/24 16:39:49
        //t_validDay = r_monthDay-atoi(t_chDay+6)+1;
        t_validDay = t_tpValidDay-atoi(t_chDay+6)+1;
        t_validDay = (t_validDay>r_monthDay) ? r_monthDay : ((t_validDay<0)?0:t_validDay);
        return (r_value*t_validDay)/r_monthDay;
    default:
      return 0;
  }
  return 0;
}
//Added By Sunam 2009/10/6 19:49:43
/*
 r_param : p1,p2,p3,p4,p5,p6
        p1[0:按入网日期计算 1:按套餐日期计算]
        p2[0:首个周期不按天折算 1:首个周期按天折算]
        p3[周期类型,同周期类型定义]
        p4[周期数]
        p5[规整方法,同批价政策进位标记]
        p6[单位]
1,1,C,1,2,60 - 
*/
long RateTpInfo::getValue(const long        &r_value,
                          const char        &r_compMethod,
                          const char        &r_openFlag,
                          const T_DATE_TIME &r_openDate,
                          const T_DATE_TIME &r_startDateTime,
                          const int         &r_monthDay,
                          const int         &r_billCycle,
                          const char        *r_param) const
{
    if (r_value == 0)
    {
        return 0;
    }
    int  t_validDay;
    char t_chDay[8+1];
    long t_value,t_firstMonthValue,t_monthValue,t_remain;
    int t_tpYear,t_tpMonth,t_tpDay;
    int t_cycleYear,t_cycleMonth;
    
    char t_param1 = '1',t_param2 = '0',t_param3,t_param5 = '0';
    int  t_param4,t_param6 = 1;
    vector<string> t_paramVec;
    StringUtil::split(r_param,",",t_paramVec,false);
    if (t_paramVec.size() != 6)
    {
        return 0;
    }
    //开始时间
    if ((t_paramVec[0].length() > 0) && (t_paramVec[0][0] == '0'))
    {
        t_param1 = '0';
    }
    if (t_param1 == '0')
    {
        snprintf(t_chDay,8+1,"%s",r_openDate.c_str());
    }
    else
    {
        snprintf(t_chDay,8+1,"%s",m_pUserTp->m_beginTime.c_str());
    }
    //首个周期是否按天折算
    if ((t_paramVec[1].length() > 0) && (t_paramVec[1][0] == '1'))
    {
        t_param2 = '1';
    }
    //归整方法
    if (t_paramVec[4].length() > 0)
    {
        t_param5 = t_paramVec[4][0];
    }
    if (t_paramVec[5].length() > 0)
    {
        t_param6 = atoi(t_paramVec[5].c_str());
    }
    
    switch(r_compMethod)
    {
        case RENT_COMPMETHOD_SUM2 :
            t_tpYear     = atoi(t_chDay) / 10000;
            t_tpMonth    = atoi(t_chDay+4) / 100;
            t_tpDay      = atoi(t_chDay+6);
            t_cycleYear  = r_billCycle / 100;
            t_cycleMonth = r_billCycle % 100;
            t_monthValue = r_value / 12;
            //第一年入网
            if (t_cycleYear == t_tpYear)
            {
                //第一个月入网实际使用天数
                if (t_param2 == '1')
                {
                    t_validDay = r_monthDay-t_tpDay+1;
                    t_validDay = (t_validDay>r_monthDay)?r_monthDay:((t_validDay<0)?0:t_validDay);
                    t_firstMonthValue = (t_monthValue*t_validDay)/r_monthDay;
                }
                else
                {
                    t_firstMonthValue = t_monthValue;
                }
                //第一个月入网 按天折算
                if (t_cycleMonth == t_tpMonth)
                {
                    t_value = t_firstMonthValue;
                }
                else if (t_cycleMonth > t_tpMonth)
                {
                    t_value = (t_cycleMonth - t_tpMonth) * t_monthValue + t_firstMonthValue;
                }
                else
                {
                    return 0;
                }
            }
            else if (t_cycleYear > t_tpYear)
            {
                t_value = t_monthValue * t_cycleMonth;
            }
            else
            {
                return 0;
            }
            break;
        default :
            return 0;
    }
    
    switch (t_param5)
    {
        case CARRY_TAG_DEL   :
            return (t_value / t_param6) * t_param6;
        case CARRY_TAG_ADD   :
            return ((t_value + t_param6 - 1) / t_param6) * t_param6;
        case CARRY_TAG_ROUND :
            t_remain = t_value % t_param6;
            if (t_remain * 2 >= t_param6)
                return ((t_value / t_param6) + 1) * t_param6;
            else
                return (t_value / t_param6) * t_param6;
        default :
            return (t_value / t_param6) * t_param6;
    }
    return 0;
    //char *pValue,t_chDay[8+1];
    //char t_param1 = '1',t_param2 = '0',t_param3,t_param5;
    //int  t_param4,t_param6;
    //vector<string> t_paramVec;
    //StringUtil::split(r_param,",",t_paramVec,false);
    //if (t_paramVec.size() != 6)
    //{
    //    return 0;
    //}
    ////开始时间
    //if ((t_paramVec[0].length() > 0) && (t_paramVec[0][0] == '0'))
    //{
    //    t_param1 = '0';
    //}
    //if (t_param1 == '0')
    //{
    //    snprintf(t_chDay,8+1,"%s",r_openDate.c_str());
    //}
    //else
    //{
    //    snprintf(t_chDay,8+1,"%s",m_pUserTp->m_beginTime.c_str());
    //}
    ////首个周期是否按天折算
    //if ((t_paramVec[0].length() > 0) && (t_paramVec[0][0] == '0'))
}
