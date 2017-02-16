#include "IdInfo.h"
#include "BillingConstDef.h"

//根据参数ID取对应的值，找不到返回NULL
const char* IdInfo::getParamValue(const T_TP_ID &r_paramId) const
{
  for(vector<UserParam>::const_iterator r_itr=m_userParamList.begin();
      r_itr!=m_userParamList.end();++r_itr)
  {
    if(r_itr->m_paramId == r_paramId)
    {
    	if (r_itr->isValid(m_cycBeginTime,m_cycEndTime,m_eventTypeId))
       return r_itr->m_value.c_str();
    }
  }
  
 // return NULL;
 //return "-1";//modify by xuf 2010-1-14 23:50:11
 return  "0"; //modify by xuf 2010-3-1 16:58:10
}

ostream& operator<<(ostream& r_os,const UserParam &r_right)
{  
      r_os<<"userId="<<r_right.m_userId<<" , "
      <<"ParamId="<<r_right.m_paramId<<" , "
      <<"value="<<r_right.m_value<<" , "
      <<"beginTime="<<r_right.m_beginTime<<" , "
      <<"endTime="<<r_right.m_endTime<<endl;
  		return r_os;	
}

int operator<(const UserParam &r_left, const UserParam &r_right)
{
	//Modified by Malone 2009-11-28 17:03 start
	/*
	return (r_left.m_paramId < r_right.m_paramId);
	*/
	if ( r_left.m_paramId == r_right.m_paramId )
	{
			//参数ID相同时，根据开始逆向排序，获取时取排好序后的第一条
			return (r_left.m_beginTime>r_right.m_beginTime);
	}
	else
	{
			return (r_left.m_paramId < r_right.m_paramId);
	}
	//Modified by Malone 2009-11-28 17:03 end
}

//根据事件判断是否有效
bool UserParam::isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime,const T_BIZ_TYPE &r_eventTypeId) const
{   
	//Modified by xuf 2009-12-22 15:01:36
	if(m_beginTime > r_endTime ||
     m_endTime   <= r_beginTime)
    return false;

  return true;	
	
}


void IdInfo::init(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime,const T_BIZ_TYPE &r_eventTypeId)
{
	m_cycBeginTime = r_beginTime;
	m_cycEndTime   = r_endTime;
	m_eventTypeId  = r_eventTypeId;
}

//获取参数的值和起始时间(本帐期有效的） add by xuf 2010-1-11 10:34:17
bool IdInfo::getParamValue(const T_TP_ID &r_paramId,UserParam &r_userParam) const
{
  for(vector<UserParam>::const_iterator r_itr=m_userParamList.begin();
      r_itr!=m_userParamList.end();++r_itr)
  {
    if(r_itr->m_paramId == r_paramId)
    {
    	if (r_itr->isValid(m_cycBeginTime,m_cycEndTime,m_eventTypeId))
    	{
    		r_userParam = *r_itr;
        return true;
      }
    }
  }
  return false;
}