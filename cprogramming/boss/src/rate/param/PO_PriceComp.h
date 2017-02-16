#ifndef PO_PriceComp_H
#define PO_PriceComp_H


#include "dbparam/PO_Common.h"
#include "PriceComp.h"

class PO_PriceComp
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_PriceComp(){}
  		
  		PO_PriceComp(const PO_PriceComp& right);
  		
  		void operator = (const PO_PriceComp& right);
  		
  		friend bool operator == (const PO_PriceComp &left,
  		                        const PO_PriceComp& right);
  		
  		friend bool operator < (const PO_PriceComp &left,
  		                       const PO_PriceComp& right);
  		
  		friend ostream& operator << (ostream& os,const PO_PriceComp& po);
  		
  		PriceComp	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

