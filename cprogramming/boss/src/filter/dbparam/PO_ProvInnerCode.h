#ifndef PO_PROVINNERCODE_H
#define PO_PROVINNERCODE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_ProvInnerCode
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_ProvInnerCode(){}

    PO_ProvInnerCode(const PO_ProvInnerCode& right);

    void operator=(const PO_ProvInnerCode& right);

    friend int operator==(const PO_ProvInnerCode &left,
    		const PO_ProvInnerCode& right);

    friend int operator<(const PO_ProvInnerCode &left,
    		const PO_ProvInnerCode& right);

	friend ostream& operator<<(ostream& os,const PO_ProvInnerCode& po);

    struct
    {
    	T_AREA_CODE m_areaCode;
	    int         m_channelNo;
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_PROVINNERCODE_H */
