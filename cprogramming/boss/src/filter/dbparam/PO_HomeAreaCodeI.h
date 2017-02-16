#ifndef PO_HOMEAREACODEI_H
#define PO_HOMEAREACODEI_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeI
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCodeI(){}

    PO_HomeAreaCodeI(const PO_HomeAreaCodeI& right);

    void operator=(const PO_HomeAreaCodeI& right);

    friend int operator==(const PO_HomeAreaCodeI &left,
    		const PO_HomeAreaCodeI& right);

    friend int operator<(const PO_HomeAreaCodeI &left,
    		const PO_HomeAreaCodeI& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCodeI& po);

    struct
    {
    	T_AREA_CODE feeCode;
    	T_DATE_TIME beginTime;
    	T_DATE_TIME endTime;
    	T_USER_TYPE userType;
    	T_GROUP_ID groupId;
    	T_GROUP_ID groupIdEx;
    	//added by yaoh 2007-7-3
    	T_GROUP_ID groupIdEx1;
    	T_GROUP_ID groupIdEx2;
    	//Added By Sunam 2007-12-7 10:43
    	T_GROUP_ID groupIdGprs;
    	T_INTER_ASPCODE nation_code; //国家代码 漫游上发使用 add by
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_HOMEAREACODEI_H */
