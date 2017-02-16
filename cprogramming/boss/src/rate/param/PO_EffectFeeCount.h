#ifndef PO_EffectFeeCount_H
#define PO_EffectFeeCount_H

#include "dbparam/PO_Common.h"
#include "EffectFeeCount.h"

class PO_EffectFeeCount
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_EffectFeeCount(){}
  		
  		PO_EffectFeeCount(const PO_EffectFeeCount &right);
  		
  		void operator = (const PO_EffectFeeCount &right);
  		
  		friend int operator == (const PO_EffectFeeCount &left,
  		                        const PO_EffectFeeCount &right);
  		
  		friend int operator < (const PO_EffectFeeCount &left,
  		                       const PO_EffectFeeCount &right);
  		
  		friend ostream& operator << (ostream& os,const PO_EffectFeeCount& po);
  		
  		EffectFeeCount m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

