#ifndef PO_EffectDiscnt_H
#define PO_EffectDiscnt_H


#include "dbparam/PO_Common.h"
#include "EffectDiscnt.h"

class PO_EffectDiscnt
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_EffectDiscnt(){}
  		
  		PO_EffectDiscnt(const PO_EffectDiscnt& right);
  		
  		void operator = (const PO_EffectDiscnt& right);
  		
  		friend bool operator == (const PO_EffectDiscnt &left,
  		                        const PO_EffectDiscnt& right);
  		
  		friend bool operator < (const PO_EffectDiscnt &left,
  		                       const PO_EffectDiscnt& right);
  		
  		friend ostream& operator << (ostream& os,const PO_EffectDiscnt& po);
  		
  		class EffectDiscnt	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

