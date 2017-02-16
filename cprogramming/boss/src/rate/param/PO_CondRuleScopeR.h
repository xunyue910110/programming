#ifndef PO_CondRuleScopeR_H
#define PO_CondRuleScopeR_H


#include "dbparam/PO_Common.h"
#include "CondRuleScopeR.h"

class PO_CondRuleScopeR
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleScopeR(){}
  		
  		PO_CondRuleScopeR(const PO_CondRuleScopeR& right);
  		
  		void operator = (const PO_CondRuleScopeR& right);
  		
  		friend int operator == (const PO_CondRuleScopeR &left,
  		                        const PO_CondRuleScopeR& right);
  		
  		friend int operator < (const PO_CondRuleScopeR &left,
  		                       const PO_CondRuleScopeR& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleScopeR& po);
  		
  		CondRuleScopeR	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

