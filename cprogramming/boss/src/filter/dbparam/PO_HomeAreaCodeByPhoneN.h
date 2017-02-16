#ifndef PO_HOMEAREACODEBYPHONEN_H
#define PO_HOMEAREACODEBYPHONEN_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeByPhoneN
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeByPhoneN(){}

    PO_HomeAreaCodeByPhoneN(const PO_HomeAreaCodeByPhoneN& right);

    void operator=(const PO_HomeAreaCodeByPhoneN& right);

    friend int operator==(const PO_HomeAreaCodeByPhoneN &left,
    		const PO_HomeAreaCodeByPhoneN& right);

    friend int operator<(const PO_HomeAreaCodeByPhoneN &left,
    		const PO_HomeAreaCodeByPhoneN& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeByPhoneN& po);

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
    	//Added by Sunam 2005-12-14
    	T_CALLED_TYPE   m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_HOMEAREACODEBYPHONEN_H */

