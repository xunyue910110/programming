#ifndef PO_EffectAddup_H
#define PO_EffectAddup_H


#include "dbparam/PO_Common.h"
#include "EffectAddup.h"

class PO_EffectAddup
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_EffectAddup(){}
  		
  		PO_EffectAddup(const PO_EffectAddup& right);
  		
  		void operator = (const PO_EffectAddup& right);
  		
  		friend bool operator == (const PO_EffectAddup &left,
  		                        const PO_EffectAddup& right);
  		
  		friend bool operator < (const PO_EffectAddup &left,
  		                       const PO_EffectAddup& right);
  		
  		friend ostream& operator << (ostream& os,const PO_EffectAddup& po);
  		
  		class EffectAddup	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

