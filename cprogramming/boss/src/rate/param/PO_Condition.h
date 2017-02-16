#ifndef PO_Condition_H
#define PO_Condition_H


#include "dbparam/PO_Common.h"
#include "CondBase.h"

class PO_Condition
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_Condition(){}
  		
  		PO_Condition(const PO_Condition& right);
  		
  		void operator = (const PO_Condition& right);
  		
  		friend int operator == (const PO_Condition &left,
  		                        const PO_Condition& right);
  		
  		friend int operator < (const PO_Condition &left,
  		                       const PO_Condition& right);
  		
  		friend ostream& operator << (ostream& os,const PO_Condition& po);
  		
  		Condition m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

