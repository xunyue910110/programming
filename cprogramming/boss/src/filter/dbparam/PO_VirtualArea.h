#ifndef PO_VIRTUALAREA_H
#define PO_VIRTUALAREA_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_VirtualArea
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_VirtualArea(){}

    PO_VirtualArea(const PO_VirtualArea& right);

    void operator=(const PO_VirtualArea& right);

    friend int operator==(const PO_VirtualArea &left,
    		const PO_VirtualArea& right);

    friend int operator<(const PO_VirtualArea &left,
    		const PO_VirtualArea& right);

	friend ostream& operator<<(ostream& os,const PO_VirtualArea& po);

  struct
  {
  	 T_GROUP      m_areagroup;
		 T_AREA_CODE  m_areacode;
		 T_HOME_TYPE  m_hometype; //用户A拨打B地区；A漫游到B，拨打B地区都算本地通话 用于区分省内国内用户 add by
		 T_ASP        m_asp;
		 T_DATE_TIME  m_beginTime;
		 T_DATE_TIME  m_endTime;
  }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_VIRTUALAREA_H */
