#ifndef PO_PriceExtend_H
#define PO_PriceExtend_H


#include "dbparam/PO_Common.h"
#include "PriceExtend.h"

class PO_PriceExtend
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_PriceExtend(){}
  		
  		PO_PriceExtend(const PO_PriceExtend& right);
  		
  		void operator = (const PO_PriceExtend& right);
  		
  		friend int operator == (const PO_PriceExtend &left,
  		                        const PO_PriceExtend& right);
  		
  		friend int operator < (const PO_PriceExtend &left,
  		                       const PO_PriceExtend& right);
  		
  		friend ostream& operator << (ostream& os,const PO_PriceExtend& po);
  		
  		PriceExtend m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

