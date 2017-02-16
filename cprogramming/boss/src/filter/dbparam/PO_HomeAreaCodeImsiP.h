#ifndef PO_HOMEAREACODEIMSIP_H
#define PO_HOMEAREACODEIMSIP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeImsiP
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeImsiP(){}

    PO_HomeAreaCodeImsiP(const PO_HomeAreaCodeImsiP& right);

    void operator=(const PO_HomeAreaCodeImsiP& right);

    friend int operator==(const PO_HomeAreaCodeImsiP &left,
    		const PO_HomeAreaCodeImsiP& right);

    friend int operator<(const PO_HomeAreaCodeImsiP &left,
    		const PO_HomeAreaCodeImsiP& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeImsiP& po);

    struct
    {
    	T_IMSI        beginImsi;
    	T_IMSI        endImsi;
    	T_DATE_TIME   beginTime;
    	T_DATE_TIME   endTime;
    	T_AREA_CODE   feeCode;
    	T_OFFICE_CODE officeCode;
    	T_USER_TYPE   userType;
    	T_GROUP_ID    groupId;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_HOMEAREACODEIMSIP_H */
