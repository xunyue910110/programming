#ifndef PO_HOMEAREACODEIMSIN_H
#define PO_HOMEAREACODEIMSIN_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeImsiN
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeImsiN(){}

    PO_HomeAreaCodeImsiN(const PO_HomeAreaCodeImsiN& right);

    void operator=(const PO_HomeAreaCodeImsiN& right);

    friend int operator==(const PO_HomeAreaCodeImsiN &left,
    		const PO_HomeAreaCodeImsiN &right);

    friend int operator<(const PO_HomeAreaCodeImsiN &left,
    		const PO_HomeAreaCodeImsiN& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeImsiN& po);

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



#endif /* PO_HOMEAREACODEIMSIN_H */

