#ifndef PO_HOMEAREACODEBYSSP_H
#define PO_HOMEAREACODEBYSSP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeBySSP
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeBySSP(){}

    PO_HomeAreaCodeBySSP(const PO_HomeAreaCodeBySSP& right);

    void operator=(const PO_HomeAreaCodeBySSP& right);

    friend int operator==(const PO_HomeAreaCodeBySSP &left,
    		const PO_HomeAreaCodeBySSP& right);

    friend int operator<(const PO_HomeAreaCodeBySSP &left,
    		const PO_HomeAreaCodeBySSP& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeBySSP& po);

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
    	//Added by Sunam 2005-12-14
    	T_CALLED_TYPE m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_HOMEAREACODEBYSSP_H */
