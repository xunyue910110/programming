#ifndef PO_EventFeePolicy_H
#define PO_EventFeePolicy_H


#include "dbparam/PO_Common.h"
#include "EventFeePolicy.h"

class PO_EventFeePolicy
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_EventFeePolicy(){}
  		
  		PO_EventFeePolicy(const PO_EventFeePolicy& right);
  		
  		void operator = (const PO_EventFeePolicy& right);
  		
  		friend int operator == (const PO_EventFeePolicy &left,
  		                        const PO_EventFeePolicy& right);
  		
  		friend int operator < (const PO_EventFeePolicy &left,
  		                       const PO_EventFeePolicy& right);
  		
  		friend ostream& operator << (ostream& os,const PO_EventFeePolicy& po);
  		
  		class EventFeePolicy	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

