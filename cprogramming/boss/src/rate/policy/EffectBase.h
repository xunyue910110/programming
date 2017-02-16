#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include "NodeBase.h"

class RateGlobalInfo;

class EffectBase : public NodeBase
{
public:
	EffectBase(){}
	virtual ~EffectBase(){}
	virtual bool execute() = 0;
	virtual void dump(char *) = 0;
  /* 清单式作用，对话单进行批价，并将结果存在
   * RateInfo.m_curFeeInfoList[m_curBranchIdx]中
   *@param r_pCdrSegList 要计算话单段
   *@return true 成功 false 失败
   */
  virtual bool rateExecute(const vector<CdrSeg> &r_pCdrSegList);
	
protected:
	//void getCurrentDate(string &);
	//void getDuration(const string &, const string &,int &);
protected:
	static RateGlobalInfo* m_globalInfo;
};

#endif
