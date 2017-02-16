#ifndef PO_HOMEAREACODEC_H
#define PO_HOMEAREACODEC_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeC
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeC(){}

    PO_HomeAreaCodeC(const PO_HomeAreaCodeC& right);

    void operator=(const PO_HomeAreaCodeC& right);

    friend int operator==(const PO_HomeAreaCodeC&left,
    		const PO_HomeAreaCodeC& right);

    friend int operator<(const PO_HomeAreaCodeC &left,
    		const PO_HomeAreaCodeC& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeC& po);

    struct
    {
    	T_AREA_CODE feeCode;
    	T_DATE_TIME   beginTime;
    	T_DATE_TIME   endTime;
    	T_USER_TYPE userType;
    	T_GROUP_ID groupId;
    	T_CITY_CODE cityCode;
    	T_PROVINCE_CODE provCode;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_HOMEAREACODEC_H */
