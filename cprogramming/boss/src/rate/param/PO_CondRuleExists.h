#ifndef PO_CondRuleExists_H
#define PO_CondRuleExists_H


#include "dbparam/PO_Common.h"
#include "CondRuleExists.h"

class PO_CondRuleExists
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleExists(){}
  		
  		PO_CondRuleExists(const PO_CondRuleExists& right);
  		
  		void operator = (const PO_CondRuleExists& right);
  		
  		friend int operator == (const PO_CondRuleExists &left,
  		                        const PO_CondRuleExists& right);
  		
  		friend int operator < (const PO_CondRuleExists &left,
  		                       const PO_CondRuleExists& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleExists& po);
  		
  		CondRuleExists	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

