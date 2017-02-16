#ifndef PO_AddupItem_H
#define PO_AddupItem_H

#include "dbparam/PO_Common.h"
#include "AddupItem.h"

class PO_AddupItem
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_AddupItem(){}
  		
  		PO_AddupItem(const PO_AddupItem &right);
  		
  		void operator = (const PO_AddupItem &right);
  		
  		friend int operator == (const PO_AddupItem &left,
  		                        const PO_AddupItem &right);
  		
  		friend int operator < (const PO_AddupItem &left,
  		                       const PO_AddupItem &right);
  		
  		friend ostream& operator << (ostream& os,const PO_AddupItem& po);
  		
  		AddupItem m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

