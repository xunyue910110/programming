#ifndef PO_RateEventExclusiveRela_H
#define PO_RateEventExclusiveRela_H


#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class PO_RateEventExclusiveRela
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_RateEventExclusiveRela(){}
  		
  		PO_RateEventExclusiveRela(const PO_RateEventExclusiveRela& right);
  		
  		void operator = (const PO_RateEventExclusiveRela& right);
  		
  		friend int operator == (const PO_RateEventExclusiveRela &left,
  		                        const PO_RateEventExclusiveRela& right);
  		
  		friend int operator < (const PO_RateEventExclusiveRela &left,
  		                       const PO_RateEventExclusiveRela& right);
  		
  		friend ostream& operator << (ostream& os,const PO_RateEventExclusiveRela& po);
  		
  		struct
  		{
  		  int             m_EFeepolicyID;  
  		  int             m_ExclusivedEFID;  		 		
  		}m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

