#ifndef _CONDRULEIN_H_
#define _CONDRULEIN_H_

#include "CondRule.h"
//in
class CondRuleIn :public CondRule
{
public:
	CondRuleIn(){}
	virtual ~CondRuleIn(){}
	virtual void dump(char *);
	bool operator < (const CondRuleIn &right) const
	{
		return (m_id < right.m_id);			
	}

	bool operator == (const CondRuleIn &right) const
	{
		return (m_id == right.m_id);
	}
	
	const char *getEnumValue();
	int getEnumValue(vector<const T_STR_MIN *> &vValue);
protected:
	virtual bool judgeCond(long &value);
	virtual bool judgeCond(T_OBJECT_ATTR &value);
public:
	char m_enumValue[MAX_STR_LENGTH+1];//枚举基准值
	vector<T_STR_MIN> m_enumVector;
	map<T_STR_MIN, T_STR_MIN> m_enumMap;
};

#endif