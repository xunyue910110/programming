#ifndef _CONDRULESCOPE_H_
#define _CONDRULESCOPE_H_

#include "CondRule.h"
//()
class CondRuleScope :public CondRule
{
public:
	CondRuleScope(){}
	~CondRuleScope(){}
	virtual void dump(char *);
	
	bool operator < (const CondRuleScope &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleScope &right) const
	{
		return (m_id == right.m_id);
	}  
protected:
	virtual bool judgeCond(long &value);
	virtual bool judgeCond(T_OBJECT_ATTR &value);
};

#endif