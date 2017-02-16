#ifndef PO_HOMEAREACODE_H
#define PO_HOMEAREACODE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCode
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_HomeAreaCode(){}

    PO_HomeAreaCode(const PO_HomeAreaCode& right);

    void operator=(const PO_HomeAreaCode& right);

    friend int operator==(const PO_HomeAreaCode &left,
    		const PO_HomeAreaCode& right);

    friend int operator<(const PO_HomeAreaCode &left,
    		const PO_HomeAreaCode& right);

	friend ostream& operator<<(ostream& os,const PO_HomeAreaCode& po);

    struct
    {
    	T_M123 m123;
    	T_DATE_TIME   beginTime;
    	T_DATE_TIME   endTime;
    	T_AREA_CODE feeCode;
    	T_USER_TYPE userType;
    	T_GROUP_ID groupId;
    }recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_HOMEAREACODE_H */
