#ifndef PO_PriceParamImpl_H
#define PO_PriceParamImpl_H


#include "dbparam/PO_Common.h"
#include "PriceParamImpl.h"

class PO_PriceParamImpl
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_PriceParamImpl(){}
  		
  		PO_PriceParamImpl(const PO_PriceParamImpl& right);
  		
  		void operator = (const PO_PriceParamImpl& right);
  		
  		friend int operator == (const PO_PriceParamImpl &left,
  		                        const PO_PriceParamImpl& right);
  		
  		friend int operator < (const PO_PriceParamImpl &left,
  		                       const PO_PriceParamImpl& right);
  		
  		friend ostream& operator << (ostream& os,const PO_PriceParamImpl& po);
  		
  		class PriceParamImpl	m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

