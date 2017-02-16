#ifndef PO_ObjectBill_H
#define PO_ObjectBill_H


#include "dbparam/PO_Common.h"
#include "ObjectBill.h"

class PO_ObjectBill
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ObjectBill(){}
  		
  		PO_ObjectBill(const PO_ObjectBill& right);
  		
  		void operator = (const PO_ObjectBill& right);
  		
  		friend int operator == (const PO_ObjectBill &left,
  		                        const PO_ObjectBill &right);
  		
  		friend int operator < (const PO_ObjectBill &left,
  		                       const PO_ObjectBill &right);
  		
  		friend ostream& operator << (ostream& os,const PO_ObjectBill& po);
  		
  		ObjectBill m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

