#ifndef _OBJECTUSER_H_
#define _OBJECTUSER_H_

#include "ObjectBase.h"
#include "IdInfo.h"
	
class ObjectUser;
typedef	void (*FuncGetAttr)(T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);

class ObjectUser :public ObjectBase
{
public:
    enum ATTR_ENUM
  	{
  	  ATTR_MIN = 199,
  	  ATTR_CYCLE_ID,                        //帐期id   
      ATTR_MOTNTH_END_TAG  ,                //月末处理标志
      ATTR_MONTH_HALF_TIME ,                //半月时间点                                   
      ATTR_MONTH_BEGIN_TIME,                //帐期起始时间
      ATTR_MONTH_END_TIME  ,                //帐期结束时间                                                                   
      ATTR_FIRST_CALL_TIME ,                //第一开打时间
      ATTR_TP_BEGIN_TIME   ,                //资费开始时间
      ATTR_TP_END_TIME     ,                //资费结束时间  
      ATTR_GROUP_MEMBER_NUM,                //集团下成员数
      ATTR_ACCOUNT_USER_NUM,                //账户下的用户数
      ATTR_CUR_STATESET,                    //最后状态集
      ATTR_IS_MOTNTH_END,                   //是否是月末
      ATTR_TP_DURATION_MONTHS,              //资费持续的月数
      ATTR_LAST_TIME_STATE,                 //当月最后时刻状态
      // add by yuxk begin 2009-9-27 20:45:27          
	  	ATTR_LASTSTOP_TIME_I,                 //最后一次停保时间，后面开机也算
	  	ATTR_I_DURATION_MONTHS,                 //停保状态持续的月数开始月即算1(停保之后不存在开机和拆机状态)
	  	ATTR_LASTSTOPOPEN_TIME,                 //最后停保复话时间（可以是当月停保，复话后面没有停保状态）
		// add by yuxk end 2009-9-27 20:45:27          
	  	ATTR_MEMBER_NUM,                      //参与优惠的账户或群下用户数
	  	ATTR_LASTSTOP_DURATION_MONTHS,        //停机的帐期数（停机当月为1月）
	  //added by dingr for jiLin 2009-10-21
      ATTR_STOPSTATE_DURATION_TIME ,//最后停包时间且至当前账期无其它状态(停保状态'2','3','8','B','C','D') 219
      ATTR_ADJUST_DURATION_MONTHS, //调帐持续的月数    220
         //add by yuxk for shanxi begin
	  ATTR_MONTH_DAY_NUM,  //当月天数 221
	  ATTR_ACTUAL_NUMBER_OF_DAYS,  //当月实际使用天数（山西日租）222
	  ATTR_CURRENT_ACTUAL_NUMBER_OF_DAYS,  //当前实际使用天数（山西日租）223
         //add by yuxk for shanxi end
      ATTR_ONLINE_MONTHS,        	//在网月数 224                
	   ATTR_VALID_TP_DAYS,   //当月生效的资费天数 225 add by taijq 2009-12-15
	   ATTR_TP_MONTHS,			//资费实例有效月数 226
	   ATTR_FINISH_DELAY_MONTHS, //工单延迟月数( 当月竣工不算延迟，延迟月数为0) 227 add by xuf 2010-1-3 22:17:01 
	   ATTR_FINISH_VALID_DAYS,   //完工当月有效天数（从完工当日算起到月底时间，完工当日算1天） 228 add by xuf 2010-1-3 22:18:11	 
     ATTR_POSTPAID_ACCT_TAG,   //固网后付费是否出帐标识 229 
	  	ATTR_LASTSTOP_TIME_I_NEW,                 //230 最后一次停保时间，开始停保之后无开通状态，可以存在拆机状态 取本帐期末之前最后一条1状态的开始时间 
	  	ATTR_LASTSTOPOPEN_TIME_NEW,                 //231 最后停保复话时间 往月停保
    ATTR_MINUSERID_BYROLE,						//232 指定角色的群下最小用户  
    ATTR_GROUPSTATS_ISSTOP,   //233 虚拟用户下所有成员状态为停保  add by yangyb 2010-10-25
      ATTR_GROUP_MEMBER_NUM_ROLE,        	//234 集团下指定角色成员数
      ATTR_MAX
  	};
public:  	
	ObjectUser(){}
	~ObjectUser(){}

	bool operator == (const ObjectUser &right) const
    {
        return (m_id == right.m_id);
    }

    bool operator < (const ObjectUser &right) const
    {
        return (m_id < right.m_id);       
    }	

	virtual bool getObjectValue(long &value);
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual void dump(char *);
public:
	int m_key;	
private:
	//最后停机时间	
	void getLastStopTime  ();
	
	static IdInfo *m_idInfo;	
	static FuncGetAttr m_funcGetAttr[ATTR_MAX-IdInfo::ID_ATTR_MAX];		
public:	
	static void getCycleId       (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getMonthEndTag   (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getMonthHalfTime (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getMonthBeginTime(T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getMonthEndTime  (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getFirstCallTime (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void getTpBeginTime   (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getTpEndTime     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getRelaCycleTime (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getCurCycleTime  (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getGroupMemberNum(T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getUserNumByAcctId(T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getCurStateSet   (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void isMonthEnd       (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void tpDurationMonths (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void getLastState     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
    // add by yuxk begin 2009-9-27 20:45:27          
	static void getLastITime     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void IDurationMonths     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void getLastStopOpenTime     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	// add by yuxk end 2009-9-27 20:45:27 
	static void getUserNumByDetailBill (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
  	static void lastStopDurationMonths(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	//added by dingr for jiLin 2009-10-21
	static void stopStateDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser);
  	static void adjustDurationMonths(T_OBJECT_ATTR &attrValue, const ObjectUser *objectUser);	
       //add by yuxk for shanxi begin
	static void getMonthDayNum (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getActualNumberOfDays (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
	static void getCurrentActualNumberOfDays (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);
       //add by yuxk for shanxi end
	static void getOnlineMonths(T_OBJECT_ATTR &attrValue, const ObjectUser *objectUser);
	//add by taijq begin 2009-12-15
	static void getValidTpDays(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	//add by taijq begin 2009-12-15
	//add by xuf 2009-12-19 10:51:17
	static void getTpValidMonthNum(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
  	static void getFinishDelayMonths(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	static void getFinishValidDays(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	static void getPostPaidAcctTag(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	static void getLastITimenew     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	static void getLastStopOpenTimenew     (T_OBJECT_ATTR &attrValue,const ObjectUser *pObject);	
	//add by xuf end 2010-1-3 22:18:54	
	static void getMinUserIdByRole(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser);
	// add by yangyb 2010-10-25 start
	static void judgeGroupUsersStatsIsStop(T_OBJECT_ATTR &attrValue, const ObjectUser *objectUser);
	// add by yangyb 2010-10-25 end
	//add by yangyb 20101101 start
	static void getGroupMemberNumByROle(T_OBJECT_ATTR &attrValue, const ObjectUser *objectUser);
	//add by yangyb 20101101 end	
};

#endif
