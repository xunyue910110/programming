#ifndef _REMINDINFO_H_INCLUDE_20090808_
#define _REMINDINFO_H_INCLUDE_20090808_

#include "config-all.h"
#include "DateTime.h"
#include "FieldsXX.h"
#include <vector>
#include "RateConstDef.h"

class RemindInfo
{
  public:
  	T_SERVER_ID  m_userId;         //USERID
  	T_MSISDN     m_msisdn;         //用户号码
    T_DATE_TIME  m_startDateTime;  //通话起始时间
    T_DATE_TIME  m_endDateTime;    //通话结束时间
    long         m_duration;       //时长
    int          m_billType;       //账单类型 1:UserSumBill1 2:UserSumBill2
    int          m_cycleId;        //周期
    T_DATE_TIME  m_cycleBeginTime; //周期开始时间
    T_DATE_TIME  m_cycleEndTime;   //周期结束时间
    T_TP_ID      m_feePolicyId;    //资费编码
    long         m_tpObjId;        //资费实例
    int          m_addupId;        //累计量编码
    int          m_remindLevel;    //提醒级别
    long         m_remindValue;    //提醒阀值
    long         m_maxValue;       //优惠最大值
    long         m_curValue;       //当前值
  public:
  	void getOuputStr(char *r_str);
  friend ostream& operator<<(ostream& os,const RemindInfo& po);
};

#endif

