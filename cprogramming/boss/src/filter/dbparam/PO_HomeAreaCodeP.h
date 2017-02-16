#ifndef PO_HOMEAREACODEP_H
#define PO_HOMEAREACODEP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeP
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeP(){}

    PO_HomeAreaCodeP(const PO_HomeAreaCodeP& right);

    void operator=(const PO_HomeAreaCodeP& right);

    friend int operator==(const PO_HomeAreaCodeP &left,
    		const PO_HomeAreaCodeP& right);

    friend int operator<(const PO_HomeAreaCodeP &left,
    		const PO_HomeAreaCodeP& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeP& po);

    struct
    {
    	T_MSISDN      m_beginMsisdn;
    	T_MSISDN      m_endMsisdn;
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    	T_AREA_CODE   m_feeCode;
    	T_OFFICE_CODE m_officeCode;
    	T_USER_TYPE   m_userType;
    	T_GROUP_ID    m_groupId;
    	//Added By Sunam 2005-12-13
    	T_CALLED_TYPE m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_HOMEAREACODEP_H */
