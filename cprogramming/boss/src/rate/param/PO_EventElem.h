#ifndef PO_ElemEvent_H
#define PO_ElemEvent_H


#include "dbparam/PO_Common.h"
#include "ElemEvent.h"

class PO_ElemEvent
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ElemEvent(){}
  		
  		PO_ElemEvent(const PO_ElemEvent& right);
  		
  		void operator = (const PO_ElemEvent& right);
  		
  		friend int operator == (const PO_ElemEvent &left,
  		                        const PO_ElemEvent& right);
  		
  		friend int operator < (const PO_ElemEvent &left,
  		                       const PO_ElemEvent& right);
  		
  		friend ostream& operator << (ostream& os,const PO_ElemEvent& po);
  		
  		ElemEvent m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

