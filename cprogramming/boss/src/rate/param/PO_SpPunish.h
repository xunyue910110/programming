#ifndef PO_SpPunish_H
#define PO_SpPunish_H


#include "dbparam/PO_Common.h"
#include "AccountCommon.h"

class PO_SpPunish
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_SpPunish(){}
  		
  		PO_SpPunish(const PO_SpPunish& right);
  		
  		void operator=(const PO_SpPunish& right);
  		
  		friend int operator==(const PO_SpPunish &left,
  		                      const PO_SpPunish& right);
  		
  		friend int operator<(const PO_SpPunish &left,
  		                     const PO_SpPunish& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_SpPunish& po);
  		
  		SpPunish m_record;
  	
  	private:
  	  	static PO_Property m_property;
};


#endif
