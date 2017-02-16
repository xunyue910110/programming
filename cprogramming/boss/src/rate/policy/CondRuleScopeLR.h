#ifndef _CONDRULESCOPELR_H_
#define _CONDRULESCOPELR_H_

#include "CondRule.h"
//[]
class CondRuleScopeLR :public CondRule
{
public:
	CondRuleScopeLR(){}
	~CondRuleScopeLR(){}
	virtual void dump(char *);
	bool operator < (const CondRuleScopeLR &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleScopeLR &right) const
	{
		return (m_id == right.m_id);
	} 
protected:
	virtual bool judgeCond(long &value);
	virtual bool judgeCond(T_OBJECT_ATTR &value);
};

#endif