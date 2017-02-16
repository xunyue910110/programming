#ifndef PO_Discount_H_INCLUDE
#define PO_Discount_H_INCLUDE

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 
#include "BillingCommon.h"

class PO_Discount
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_Discount(){}
  		
  		PO_Discount(const PO_Discount& right);
  		
  		void operator=(const PO_Discount& right);
  		
  		friend int operator==(const PO_Discount &left,
  		                      const PO_Discount& right);
  		
  		friend int operator<(const PO_Discount &left,
  		                     const PO_Discount& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_Discount& po);
  		
  		DiscountInfo m_record;
  	
  	private:
  	  	static PO_Property m_property;
};


#endif
