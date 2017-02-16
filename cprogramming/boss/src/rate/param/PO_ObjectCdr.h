#ifndef PO_ObjectCdr_H
#define PO_ObjectCdr_H


#include "dbparam/PO_Common.h"
#include "ObjectCdr.h"

class PO_ObjectCdr
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ObjectCdr(){}
  		
  		PO_ObjectCdr(const PO_ObjectCdr& right);
  		
  		void operator = (const PO_ObjectCdr& right);
  		
  		friend int operator == (const PO_ObjectCdr &left,
  		                        const PO_ObjectCdr& right);
  		
  		friend int operator < (const PO_ObjectCdr &left,
  		                       const PO_ObjectCdr& right);
  		
  		friend ostream& operator << (ostream& os,const PO_ObjectCdr& po);
  		
  		ObjectCdr m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

