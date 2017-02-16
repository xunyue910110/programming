#ifndef _CONDRULERELA_H_
#define _CONDRULERELA_H_

#include "CondBase.h"

class CondRuleRela :public CondBase
{
  public:
	  CondRuleRela(){}
	  virtual ~CondRuleRela(){}	

  	bool operator < (const CondRuleRela& right) const
  	{
  		if( m_id == right.m_id)				
  		{
  			return (m_orderNo < right.m_orderNo);
  		}
  		else
  			return (m_id < right.m_id);
  			
  	}

  	bool operator == (const CondRuleRela& right) const
  	{
  		return (m_id == right.m_id);
  	}    
  public:
	  virtual bool execute();
	  virtual void dump(char *);
	
	  virtual bool  judge(vector<CdrSeg> *r_pCdrSegList,
                        vector<CdrSeg> *r_pSepCdrSegList,
                        bool           &r_splitFlag,
                        SumTpRule      &r_sumRuleInfo);
  public:	
  	char          m_andOrTag;//与或标志 0:并且  1:或者
  	short         m_orderNo; //顺序号
  	T_RULE_ID     m_condId;  //子条件标识		
  	CondBase     *m_condRule;//子条件指针
  	CondRuleRela *m_next;    //下一个条件
};

#endif
