#ifndef PO_AddupCycleRule_H
#define PO_AddupCycleRule_H

#include "dbparam/PO_Common.h"
#include "AddupCycleRule.h"

class PO_AddupCycleRule
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_AddupCycleRule(){}
  		
  		PO_AddupCycleRule(const PO_AddupCycleRule &right);
  		
  		void operator = (const PO_AddupCycleRule &right);
  		
  		friend int operator == (const PO_AddupCycleRule &left,
  		                        const PO_AddupCycleRule &right);
  		
  		friend int operator < (const PO_AddupCycleRule &left,
  		                       const PO_AddupCycleRule &right);
  		
  		friend ostream& operator << (ostream& os,const PO_AddupCycleRule& po);
  		
  		AddupCycleRule m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

