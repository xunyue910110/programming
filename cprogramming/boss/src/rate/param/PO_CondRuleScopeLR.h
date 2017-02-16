#ifndef PO_CondRuleScopeLR_H
#define PO_CondRuleScopeLR_H


#include "dbparam/PO_Common.h"
#include "CondRuleScopeLR.h"

class PO_CondRuleScopeLR
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleScopeLR(){}
  		
  		PO_CondRuleScopeLR(const PO_CondRuleScopeLR& right);
  		
  		void operator = (const PO_CondRuleScopeLR& right);
  		
  		friend int operator == (const PO_CondRuleScopeLR &left,
  		                        const PO_CondRuleScopeLR& right);
  		
  		friend int operator < (const PO_CondRuleScopeLR &left,
  		                       const PO_CondRuleScopeLR& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleScopeLR& po);
  		
  		CondRuleScopeLR	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

