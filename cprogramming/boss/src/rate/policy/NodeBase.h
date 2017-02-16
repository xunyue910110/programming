#ifndef _NODEBASE_H_
#define _NODEBASE_H_

#include "FieldsXX.h"
#include "FeeInfo.h"
#include "RateError.h"


const int MAX_STR_LENGTH = 200;
//Modified By Sunam 2009/8/15 10:05:10 td_b_simplecond中的min_value,max_value由varchar2(20)->varchar2(50)
//const int MIN_STR_LENGTH = 20;
const int MIN_STR_LENGTH = 100;
const int STR_LENGTH = 100; //表达式长度

const int CONDITION_PASS     = 0; //完全通过
const int CONDITION_PARTPASS = 1; //非总量型,部分通过
const int CONDITION_SUMPASS  = 2; //总量型通过
const int CONDITION_NOTPASS  = 3; //不通过

class PolicyInfo;
class RateGlobalInfo;

class NodeBase
{
  public:
	  NodeBase(){}
	  virtual ~NodeBase(){}
	
	  virtual bool execute() = 0;
	  virtual void dump(char *) = 0;
	  /* 判断条件是否通过
	   *@param  r_pCdrSegList     输入的话单段列表
	   *@param  r_pSepCdrSegList  如果部分通过，则存放通过的话单段列表
	   *@param  r_splitFlag       是否分割标记
	   *@param  r_sumRuleInfo     存放总量条件信息
	   *@return true 通过,	   
	   *        false 不通过
	   */
	  virtual bool  judge(vector<CdrSeg> *r_pCdrSegList,
	                      vector<CdrSeg> *r_pSepCdrSegList,
	                      bool           &r_splitFlag,
	                      SumTpRule      &r_sumRuleInfo);
    /* 清单式作用，对话单进行批价，并将结果存在
     * RateInfo.m_curFeeInfoList[m_curBranchIdx]中
     *@param r_pCdrSegList 要计算话单段
     *@return true 成功 false 失败
     */
    virtual bool rateExecute(const vector<CdrSeg> &r_pCdrSegList);
  public:	
  	static PolicyInfo      m_policyInfo;
  	static RateGlobalInfo *m_globalInfo;
  public:
  	T_RULE_ID m_id;
};

#endif
