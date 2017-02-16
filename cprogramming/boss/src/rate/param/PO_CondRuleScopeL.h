#ifndef PO_CondRuleScopeL_H
#define PO_CondRuleScopeL_H


#include "dbparam/PO_Common.h"
#include "CondRuleScopeL.h"

class PO_CondRuleScopeL
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleScopeL(){}
  		
  		PO_CondRuleScopeL(const PO_CondRuleScopeL& right);
  		
  		void operator = (const PO_CondRuleScopeL& right);
  		
  		friend int operator == (const PO_CondRuleScopeL &left,
  		                        const PO_CondRuleScopeL& right);
  		
  		friend int operator < (const PO_CondRuleScopeL &left,
  		                       const PO_CondRuleScopeL& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleScopeL& po);
  		
  		CondRuleScopeL	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

