#ifndef PO_SubCycle_H
#define PO_SubCycle_H

#include "dbparam/PO_Common.h"
#include "BillingCommon.h"


class PO_SubCycle
{
	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_SubCycle(){}
  		
  		PO_SubCycle(const PO_SubCycle& right);
  		
  		void operator=(const PO_SubCycle& right);
  		
  		friend int operator==(const PO_SubCycle &left,
  		                      const PO_SubCycle& right);
  		
  		friend int operator<(const PO_SubCycle &left,
  		                     const PO_SubCycle& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_SubCycle& po);
  		
  		SubCycPara m_record;
  	
  	private:
  	  	static PO_Property m_property;
};

#endif
