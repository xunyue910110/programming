#ifndef PO_FeePolicyComp_H
#define PO_FeePolicyComp_H


#include "dbparam/PO_Common.h"
#include "FeePolicyComp.h"

class PO_FeePolicyComp
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_FeePolicyComp(){}
  		
  		PO_FeePolicyComp(const PO_FeePolicyComp& right);
  		
  		void operator = (const PO_FeePolicyComp& right);
  		
  		friend int operator == (const PO_FeePolicyComp &left,
  		                        const PO_FeePolicyComp& right);
  		
  		friend int operator < (const PO_FeePolicyComp &left,
  		                       const PO_FeePolicyComp& right);
  		
  		friend ostream& operator << (ostream& os,const PO_FeePolicyComp& po);
  		
  		class FeePolicyComp	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

