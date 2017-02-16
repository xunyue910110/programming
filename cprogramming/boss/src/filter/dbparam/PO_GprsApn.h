#ifndef PO_GPRSAPN_H
#define PO_GPRSAPN_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_GprsApn
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_GprsApn(){}

    PO_GprsApn(const PO_GprsApn& right);

    void operator=(const PO_GprsApn& right);

    friend int operator==(const PO_GprsApn&left,
    		const PO_GprsApn& right);

    friend int operator<(const PO_GprsApn &left,
    		const PO_GprsApn& right);

	friend ostream& operator<<(ostream& os,const PO_GprsApn& po);

    struct
    {
    	T_PROVINCE_CODE     provCode;
    	T_ATTRIBUTE_VALUE   apnni;
    	T_DATE_TIME         beginTime;
    	T_DATE_TIME         endTime;
    	T_GPRS_APNGROUP     apnGroup;
    	char                servRange;
    	char                matchType; //江苏联通加匹配模式
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_GPRSAPN_H */
