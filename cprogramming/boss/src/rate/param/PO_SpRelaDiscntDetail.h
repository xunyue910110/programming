#ifndef PO_SpRelaDiscntDetail_H
#define PO_SpRelaDiscntDetail_H

#include "dbparam/PO_Common.h"
#include "AccountCommon.h"

class PO_SpRelaDiscntDetail
{
  	public:
  		  bool getProperty(PO_Property &property);

  		  bool convert(const StringVector &argname);

  		  PO_SpRelaDiscntDetail(){}

  		  PO_SpRelaDiscntDetail(const PO_SpRelaDiscntDetail& right);

  		  void operator=(const PO_SpRelaDiscntDetail& right);

  		  friend int operator==(const PO_SpRelaDiscntDetail &left,
  		                        const PO_SpRelaDiscntDetail& right);

  		  friend int operator<(const PO_SpRelaDiscntDetail &left,
  		                       const PO_SpRelaDiscntDetail& right);

  		  friend ostream& operator<<(ostream& os,const PO_SpRelaDiscntDetail& po);

  		  struct
  		  {
  		  	  int    m_discntId;
  		  	  int    m_priority;
  		  	  int    m_itemCode;
            string m_spCode;
            string m_spBizCode;
		    } m_record;

  	private:
  	  	static PO_Property m_property;
};
#endif
