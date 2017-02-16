#ifndef _CONDRULEEXISTS_H_
#define _CONDRULEEXISTS_H_

#include "CondRule.h"

class CondRuleExists;
typedef	bool (*FuncJudgeExists)(CondRuleExists &condRuleExists);

class CondRuleExists :public CondRule
{
public:
	enum 
  	{
  	  TAG_EXISTS_TYPE_TP,              //存在tpID
      TAG_EXISTS_TYPE_STATE,           //停机状态持续了n个月
      TAG_EXISTS_TYPE_STATE_CURMONTH , //当月状态
      TAG_EXISTS_TYPE_STATE_DEALED_RATETP , // 存在成功作用过的tpID
      TAG_EXISTS_TYPE_SERV_CURMONTH,	 			//当月存在某个服务
      TAG_EXISTS_AreaType,                  //区内、区间属性
	  TAG_EXISTS_TYPE_OPENDTA_TPSTARTDATE,  //资费起始时间等于用户开户时间
      //add by yuxk for shanxi begin
      TAG_EXISTS_Serv_Start_FirstHalf,                  //服务开始时间是当前帐期上半月
      TAG_EXISTS_Serv_Start_LastHalf,                  //服务开始时间是当前帐期下半月
      TAG_EXISTS_Serv_End_FirstHalf,                  //服务结束时间是当前帐期上半月
      TAG_EXISTS_Serv_End_LastHalf,                  //服务结束时间是当前帐期下半月
      TAG_SERV_HALF_MONTH,                  //服务时间和停保停保复话时间组合判断是哪个半月 2010-1-27 16:39:15
      //add by yuxk for shanxi end
      TAG_EXISTS_TYPE_MAX
  	};
	CondRuleExists(){}
	virtual ~CondRuleExists(){}
	virtual bool execute();
	virtual void dump(char *);
	bool operator < (const CondRuleExists &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleExists &right) const
	{
		return (m_id == right.m_id);
	}
	
	const char *getEnumValue(); 
protected:
	virtual bool judgeCond(long &value){ return false;}
	virtual bool judgeCond(T_OBJECT_ATTR &value){ return false;}
private:	
	static FuncJudgeExists m_funcJudgeExists[TAG_EXISTS_TYPE_MAX];
	
	static bool judgeExistsTp(CondRuleExists &condRuleExists);
	static bool judgeExistsState(CondRuleExists &condRuleExists);
	static bool judgeExistsStateCurMonth(CondRuleExists &condRuleExists);
	static bool judgeExistsDealedRateTp(CondRuleExists &condRuleExists);
	static bool judgeExistsServCurMonth(CondRuleExists &condRuleExists);
	static bool judgeExistsAreaType(CondRuleExists &condRuleExists);
	static bool judgeExistsOpenDateTpStartDate(CondRuleExists &condRuleExists);
 //add by yuxk for shanxi begin
  static bool judgeExistsServStartFirstHalf(CondRuleExists &condRuleExists);
  static bool judgeExistsServStartLastHalf(CondRuleExists &condRuleExists);
  static bool judgeExistsServEndFirstHalf(CondRuleExists &condRuleExists);
  static bool judgeExistsServEndLastHalf(CondRuleExists &condRuleExists);
  static bool judgeServHalfMonth(CondRuleExists &condRuleExists);
 //add by yuxk for shanxi end

public:
	char m_enumValue[MAX_STR_LENGTH+1];//枚举基准值
	vector<T_STR_MIN> m_enumVector;
	map<T_STR_MIN, T_STR_MIN> m_enumMap;
};

#endif