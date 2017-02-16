#ifndef PO_HOMEAREACODEN_H
#define PO_HOMEAREACODEN_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeN
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeN(){}

    PO_HomeAreaCodeN(const PO_HomeAreaCodeN& right);

    void operator=(const PO_HomeAreaCodeN& right);

    friend int operator==(const PO_HomeAreaCodeN&left,
    		const PO_HomeAreaCodeN& right);

    friend int operator<(const PO_HomeAreaCodeN &left,
    		const PO_HomeAreaCodeN& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeN& po);

    struct
    {
    	T_AREA_CODE   feeCode;
    	T_DATE_TIME   beginTime;
    	T_DATE_TIME   endTime;
    	T_USER_TYPE   userType;
    	T_GROUP_ID    groupId;
    	T_CITY_CODE     cityCode; //add by
    	T_PROVINCE_CODE provCode; //add by
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_HOMEAREACODEN_H */
