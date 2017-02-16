#ifndef PO_HOMEAREACODEBYSPECIALMSISDN_H
#define PO_HOMEAREACODEBYSPECIALMSISDN_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeBySpecialMsisdn
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeBySpecialMsisdn(){}

    PO_HomeAreaCodeBySpecialMsisdn(const PO_HomeAreaCodeBySpecialMsisdn& right);

    void operator=(const PO_HomeAreaCodeBySpecialMsisdn& right);

    friend int operator==(const PO_HomeAreaCodeBySpecialMsisdn &left,
    		const PO_HomeAreaCodeBySpecialMsisdn& right);

    friend int operator<(const PO_HomeAreaCodeBySpecialMsisdn &left,
    		const PO_HomeAreaCodeBySpecialMsisdn& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeBySpecialMsisdn& po);

    struct
    {
    	T_MSISDN        m_beginMsisdn;
    	T_MSISDN        m_endMsisdn;
    	T_DATE_TIME     m_beginTime;
    	T_DATE_TIME     m_endTime;
    	T_AREA_CODE     m_feeCode;
    	T_PROVINCE_CODE m_provCode;
    	T_USER_TYPE     m_userType;
    	T_GROUP_ID      m_groupId;
    	T_CALLED_TYPE   m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_HOMEAREACODEBYSPECIALMSISDN_H */
