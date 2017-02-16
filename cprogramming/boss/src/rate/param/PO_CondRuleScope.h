#ifndef PO_CondRuleScope_H
#define PO_CondRuleScope_H


#include "dbparam/PO_Common.h"
#include "CondRuleScope.h"

class PO_CondRuleScope
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleScope(){}
  		
  		PO_CondRuleScope(const PO_CondRuleScope& right);
  		
  		void operator = (const PO_CondRuleScope& right);
  		
  		friend int operator == (const PO_CondRuleScope &left,
  		                        const PO_CondRuleScope& right);
  		
  		friend int operator < (const PO_CondRuleScope &left,
  		                       const PO_CondRuleScope& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleScope& po);
  		
  		class CondRuleScope	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

