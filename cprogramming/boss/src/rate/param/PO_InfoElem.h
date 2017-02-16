#ifndef PO_ElemInfo_H
#define PO_ElemInfo_H


#include "dbparam/PO_Common.h"
#include "ElemInfo.h"

class PO_ElemInfo
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ElemInfo(){}
  		
  		PO_ElemInfo(const PO_ElemInfo& right);
  		
  		void operator = (const PO_ElemInfo& right);
  		
  		friend int operator == (const PO_ElemInfo &left,
  		                        const PO_ElemInfo& right);
  		
  		friend int operator < (const PO_ElemInfo &left,
  		                       const PO_ElemInfo& right);
  		
  		friend ostream& operator << (ostream& os,const PO_ElemInfo& po);
  		
  		ElemInfo m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

