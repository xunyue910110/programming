#ifndef PO_Item_H
#define PO_Item_H


#include "dbparam/PO_Common.h"
#include "CompositItem.h"

class PO_Item
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_Item(){}
  		
  		PO_Item(const PO_Item& right);
  		
  		void operator = (const PO_Item& right);
  		
  		friend int operator == (const PO_Item &left,
  		                        const PO_Item &right);
  		
  		friend int operator < (const PO_Item &left,
  		                       const PO_Item &right);
  		
  		friend ostream& operator << (ostream& os,const PO_Item& po);
  		
  		Item m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

