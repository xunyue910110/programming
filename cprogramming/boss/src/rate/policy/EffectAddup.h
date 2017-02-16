#ifndef _EFFECTADDUP_H_
#define _EFFECTADDUP_H_

#include "EffectBase.h"

class AddupItem;

class EffectAddup :public EffectBase
{
public:
	EffectAddup();
	virtual ~EffectAddup();
	
	bool operator < (const EffectAddup &right) const
	{
		if(m_id==right.m_id)
			return (m_orderNo < right.m_orderNo);
		else
			return (m_id < right.m_id);
	}

	bool operator == (const EffectAddup &right) const
	{
		return (m_id==right.m_id);
	}
public:	
	virtual bool execute() ;
	virtual void dump(char *);
	bool acctExecute(long &sumBeginTime, long &sumEndTime);
	
	//获取当前帐务优惠剩余的累积值
	bool getAcctMargin(const int &itemCode,long &marginFee,
	                   long &sumBeginTime, long &sumEndTime);
  
	//added by yinb 2011/2/16 12:39:29 增加计费累计
	bool rateExecute(const vector<CdrSeg> &r_pCdrSegList);
	void commitAddupBillInfo(const RateBillInfo &r_billInfo);
public:
	char m_clearTag;             //清理标志
	short m_orderNo;             //顺序号
	int m_clearCycleNum;         //清理周期数量
	int m_addupItemCode;         //累计帐目标识
	T_MEMBER_TYPE m_addupRoleId; //累计属主角色编码
	char m_addupUpper[MIN_STR_LENGTH+1]; //累计值上限	
	AddupItem *m_addupItem;      //累计帐目指针
	EffectAddup *m_next;	
};

#endif
