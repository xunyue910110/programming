#ifndef PO_HOMEAREACODEFIX_H
#define PO_HOMEAREACODEFIX_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeFix
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeFix(){}

    PO_HomeAreaCodeFix(const PO_HomeAreaCodeFix& right);

    void operator=(const PO_HomeAreaCodeFix& right);

    friend int operator==(const PO_HomeAreaCodeFix &left,
    		const PO_HomeAreaCodeFix& right);

    friend int operator<(const PO_HomeAreaCodeFix &left,
    		const PO_HomeAreaCodeFix& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeFix& po);

    struct
    {
    	T_MSISDN      m_phoneSeg;    //号头
    	T_ATTRI_VAL   m_exchangeCode; //局向
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    	T_AREA_CODE   m_areaCode;
    	T_OFFICE_CODE m_officeCode;
    	T_USER_TYPE   m_userType;
    	T_GROUP_ID    m_groupId;
    	T_CALLED_TYPE m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_HOMEAREACODEFIX_H */
