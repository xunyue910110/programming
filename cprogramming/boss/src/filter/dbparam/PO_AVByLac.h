#ifndef PO_AVBYLAC_H
#define PO_AVBYLAC_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByLac
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByLac(){}

    PO_AVByLac(const PO_AVByLac& right);

    void operator=(const PO_AVByLac& right);

    friend int operator==(const PO_AVByLac &left,
    		const PO_AVByLac& right);

    friend int operator<(const PO_AVByLac &left,
    		const PO_AVByLac& right);

	friend ostream& operator<<(ostream& os,const PO_AVByLac& po);

	struct
	{
		T_LAC lac;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYLAC_H */
