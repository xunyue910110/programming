#ifndef PO_NETTYPECODEBIND_H_
#define PO_NETTYPECODEBIND_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_NetTypeCodeBind
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_NetTypeCodeBind(){}

    PO_NetTypeCodeBind(const PO_NetTypeCodeBind& right);

    void operator=(const PO_NetTypeCodeBind& right);

    friend int operator==(const PO_NetTypeCodeBind &left,
    		const PO_NetTypeCodeBind& right);

    friend int operator<(const PO_NetTypeCodeBind &left,
    		const PO_NetTypeCodeBind& right);

	friend ostream& operator<<(ostream& os,const PO_NetTypeCodeBind& po);

    struct
    {
    	T_ATTRI_VAL m_sourceType;
    	T_ATTRI_VAL m_netTypeCode;
    	T_FLAG      m_method;
	    T_DATE_TIME m_beginDate;
	    T_DATE_TIME m_endDate;
    }recordStruct;

  private:
	static PO_Property m_property;
};
#endif /* PO_NETTYPECODEBIND_H_ */