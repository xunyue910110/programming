#ifndef PO_Price_H
#define PO_Price_H


#include "dbparam/PO_Common.h"
#include "Price.h"

class PO_Price
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_Price(){}
  		
  		PO_Price(const PO_Price& right);
  		
  		void operator = (const PO_Price& right);
  		
  		friend int operator == (const PO_Price &left,
  		                        const PO_Price& right);
  		
  		friend int operator < (const PO_Price &left,
  		                       const PO_Price& right);
  		
  		friend ostream& operator << (ostream& os,const PO_Price& po);
  		
  		class Price	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

