#ifndef PO_Object_H
#define PO_Object_H


#include "dbparam/PO_Common.h"
#include "ObjectBase.h"

class PO_Object
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_Object(){}
  		
  		PO_Object(const PO_Object& right);
  		
  		void operator = (const PO_Object& right);
  		
  		friend int operator == (const PO_Object &left,
  		                        const PO_Object& right);
  		
  		friend int operator < (const PO_Object &left,
  		                       const PO_Object& right);
  		
  		friend ostream& operator << (ostream& os,const PO_Object& po);
  		
  		Object m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

