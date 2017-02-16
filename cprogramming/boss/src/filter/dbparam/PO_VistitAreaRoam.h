#ifndef PO_VISTITAREAROAM_H
#define PO_VISTITAREAROAM_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_VistitAreaRoam
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_VistitAreaRoam(){}

    PO_VistitAreaRoam(const PO_VistitAreaRoam& right);

    void operator=(const PO_VistitAreaRoam& right);

    friend int operator==(const PO_VistitAreaRoam&left,
    		const PO_VistitAreaRoam& right);

    friend int operator<(const PO_VistitAreaRoam &left,
    		const PO_VistitAreaRoam& right);

	friend ostream& operator<<(ostream& os,const PO_VistitAreaRoam& po);

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

#endif /* PO_VISTITAREAROAM_H */
