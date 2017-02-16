#ifndef _CONDRULESCOPER_H_
#define _CONDRULESCOPER_H_

#include "CondRule.h"
//(]
class CondRuleScopeR :public CondRule
{
public:
	CondRuleScopeR(){}
	~CondRuleScopeR(){}
	virtual void dump(char *);
	bool operator < (const CondRuleScopeR &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleScopeR &right) const
	{
		return (m_id == right.m_id);
	}  
protected:
	virtual bool judgeCond(long &value);
	virtual bool judgeCond(T_OBJECT_ATTR &value);
};

#endif