#ifndef PO_ObjectUser_H
#define PO_ObjectUser_H


#include "dbparam/PO_Common.h"
#include "ObjectUser.h"

class PO_ObjectUser
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ObjectUser(){}
  		
  		PO_ObjectUser(const PO_ObjectUser& right);
  		
  		void operator = (const PO_ObjectUser& right);
  		
  		friend int operator == (const PO_ObjectUser &left,
  		                        const PO_ObjectUser& right);
  		
  		friend int operator < (const PO_ObjectUser &left,
  		                       const PO_ObjectUser& right);
  		
  		friend ostream& operator << (ostream& os,const PO_ObjectUser& po);
  		
  		ObjectUser m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

