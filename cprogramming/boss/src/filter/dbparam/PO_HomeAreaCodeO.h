#ifndef PO_HOMEAREACODEO_H
#define PO_HOMEAREACODEO_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeO
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeO(){}

    PO_HomeAreaCodeO(const PO_HomeAreaCodeO& right);

    void operator=(const PO_HomeAreaCodeO& right);

    friend int operator==(const PO_HomeAreaCodeO &left,
    		const PO_HomeAreaCodeO& right);

    friend int operator<(const PO_HomeAreaCodeO &left,
    		const PO_HomeAreaCodeO& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeO& po);

    struct
    {
    	T_MSISDN      m_beginMsisdn;
    	T_MSISDN      m_endMsisdn;
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    	T_AREA_CODE   m_feeCode;
    	T_USER_TYPE   m_userType;
    	T_GROUP_ID    m_groupId;
    	T_CALLED_TYPE m_calledType;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_HOMEAREACODEO_H */
