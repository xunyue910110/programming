#ifndef PO_FeePolicy_H
#define PO_FeePolicy_H


#include "dbparam/PO_Common.h"
#include "policy/FeePolicy.h"

class PO_FeePolicy
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_FeePolicy(){}
  		
  		PO_FeePolicy(const PO_FeePolicy& right);
  		
  		void operator = (const PO_FeePolicy& right);
  		
  		friend int operator == (const PO_FeePolicy &left,
  		                        const PO_FeePolicy& right);
  		
  		friend int operator < (const PO_FeePolicy &left,
  		                       const PO_FeePolicy& right);
  		
  		friend ostream& operator << (ostream& os,const PO_FeePolicy& po);
  		
  		class FeePolicy	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

