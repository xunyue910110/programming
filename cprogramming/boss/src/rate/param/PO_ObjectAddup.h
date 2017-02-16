#ifndef PO_ObjectAddup_H
#define PO_ObjectAddup_H

#include "dbparam/PO_Common.h"
#include "ObjectAddup.h"

class PO_ObjectAddup
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ObjectAddup(){}
  		
  		PO_ObjectAddup(const PO_ObjectAddup &right);
  		
  		void operator = (const PO_ObjectAddup &right);
  		
  		friend int operator == (const PO_ObjectAddup &left,
  		                        const PO_ObjectAddup &right);
  		
  		friend int operator < (const PO_ObjectAddup &left,
  		                       const PO_ObjectAddup &right);
  		
  		friend ostream& operator << (ostream& os,const PO_ObjectAddup& po);
  		
  		ObjectAddup m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

