#ifndef _CONDBASE_H_
#define _CONDBASE_H_

#include "NodeBase.h"

class CondBase : public NodeBase
{
  public:
  	CondBase(){}
  	virtual ~CondBase(){}
  public:
  	virtual bool execute() = 0;
  	virtual void dump(char *) = 0;
  public:
      char m_condType; //条件类型   0:简单条件  1:复合条件	
};

class Condition
{
  public:
  	Condition(){}
  	virtual ~Condition(){}
  	
  	bool operator < (const Condition& right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const Condition& right) const
  	{
  		return (m_id == right.m_id);
  	}    
  public:    
  	T_RULE_ID m_id;
  	CondBase *m_cond;
};

#endif