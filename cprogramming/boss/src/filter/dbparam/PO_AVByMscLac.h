#ifndef PO_AVBYMSCLAC_H
#define PO_AVBYMSCLAC_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByMscLac
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByMscLac(){}

    PO_AVByMscLac(const PO_AVByMscLac& right);

    void operator=(const PO_AVByMscLac& right);

    friend int operator==(const PO_AVByMscLac &left,
    		const PO_AVByMscLac& right);

    friend int operator<(const PO_AVByMscLac &left,
    		const PO_AVByMscLac& right);

	friend ostream& operator<<(ostream& os,const PO_AVByMscLac& po);

	struct
	{
		T_MSC msc;
		T_LAC lac;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYMSCLAC_H */
