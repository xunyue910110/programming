#ifndef _CONDRULESCOPEL_H_
#define _CONDRULESCOPEL_H_

#include "CondRule.h"
//[)
class CondRuleScopeL:public CondRule
{
public:
	CondRuleScopeL(){}
	~CondRuleScopeL(){}
	virtual void dump(char *);
	bool operator < (const CondRuleScopeL &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleScopeL &right) const
	{
		return (m_id == right.m_id);
	}  
protected:
	virtual bool judgeCond(long &value);
	virtual bool judgeCond(T_OBJECT_ATTR &value);
};

#endif