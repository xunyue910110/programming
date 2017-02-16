#ifndef PO_CondRuleIn_H
#define PO_CondRuleIn_H


#include "dbparam/PO_Common.h"
#include "CondRuleIn.h"

class PO_CondRuleIn
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleIn(){}
  		
  		PO_CondRuleIn(const PO_CondRuleIn& right);
  		
  		void operator = (const PO_CondRuleIn& right);
  		
  		friend int operator == (const PO_CondRuleIn &left,
  		                        const PO_CondRuleIn& right);
  		
  		friend int operator < (const PO_CondRuleIn &left,
  		                       const PO_CondRuleIn& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleIn& po);
  		
  		CondRuleIn	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

