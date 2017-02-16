#ifndef _FEEPOLICY_H_
#define _FEEPOLICY_H_

#include "FieldsXX.h"

class FeePolicyComp;

class FeePolicy
{
public:
    FeePolicy() {m_comp = NULL;}
    ~FeePolicy() {}
public:
	bool operator < (const FeePolicy &right) const
	{
		return (m_id < right.m_id);		
	}

	bool operator == (const FeePolicy &right) const
	{
		return (m_id == right.m_id);
	}	
	
public:	
	char m_source;	
	T_TP_ID m_id;
	FeePolicyComp *m_comp;	
};

#endif
