#ifndef PO_CondRuleRela_H
#define PO_CondRuleRela_H


#include "dbparam/PO_Common.h"
#include "CondRuleRela.h"

class PO_CondRuleRela
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CondRuleRela(){}
  		
  		PO_CondRuleRela(const PO_CondRuleRela& right);
  		
  		void operator = (const PO_CondRuleRela& right);
  		
  		friend int operator == (const PO_CondRuleRela &left,
  		                        const PO_CondRuleRela& right);
  		
  		friend int operator < (const PO_CondRuleRela &left,
  		                       const PO_CondRuleRela& right);
  		
  		friend ostream& operator << (ostream& os,const PO_CondRuleRela& po);
  		
  		class CondRuleRela	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

