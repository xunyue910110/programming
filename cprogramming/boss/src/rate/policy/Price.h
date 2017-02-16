#ifndef _PRICE_H_
#define _PRICE_H_

#include "NodeBase.h"

class PriceComp;
class PolicyInfo;
class PriceExtend;

class Price
{
public:
	Price() { m_priceComp = 0; m_priceExtend = 0;}
  virtual ~Price() {}
	bool execute();
	bool rateExecute(int &r_errCode);
	void dump(char *);
public:
	bool operator < (const Price &right) const
	{
		return (m_id < right.m_id);
	}

	bool operator == (const Price &right) const
	{
		return (m_id == right.m_id);
	}      
	
public:	
	int          m_id;       //批价政策标识
	char         m_carryTag;//进位标志  0:不处理（保持现状）1:舍去  2:进位 3:四舍五入       
	char         m_sumToIntFee[MIN_STR_LENGTH+1]; //固定费归整费用
	char         m_sumToIntType[MIN_STR_LENGTH+1]; //固定费归整类型       
	PriceComp   *m_priceComp;      //批价构成指针
	PriceExtend *m_priceExtend;  //批价政策扩展
private:
	static PolicyInfo	*m_policyInfo;	
};                                                     
                                                   
#endif
