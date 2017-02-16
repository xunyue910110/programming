#ifndef PO_CompositItem_H
#define PO_CompositItem_H


#include "dbparam/PO_Common.h"
#include "CompositItem.h"

class PO_CompositItem
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CompositItem(){}
  		
  		PO_CompositItem(const PO_CompositItem& right);
  		
  		void operator = (const PO_CompositItem& right);
  		
  		friend int operator == (const PO_CompositItem &left,
  		                        const PO_CompositItem &right);
  		
  		friend int operator < (const PO_CompositItem &left,
  		                       const PO_CompositItem &right);
  		
  		friend ostream& operator << (ostream& os,const PO_CompositItem& po);
  		
  		CompositItem m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

