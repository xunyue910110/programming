#ifndef PO_VISITAREACODELAC_H
#define PO_VISITAREACODELAC_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_VisitAreaCodeLac
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_VisitAreaCodeLac(){}

    PO_VisitAreaCodeLac(const PO_VisitAreaCodeLac& right);

    void operator=(const PO_VisitAreaCodeLac& right);

    friend int operator==(const PO_VisitAreaCodeLac &left,
    		const PO_VisitAreaCodeLac& right);

    friend int operator<(const PO_VisitAreaCodeLac &left,
    		const PO_VisitAreaCodeLac& right);

	friend ostream& operator<<(ostream& os,const PO_VisitAreaCodeLac& po);

	struct
	{
		T_LAC lac;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_AREA_CODE visitAreaCode;
		T_OFFICE_CODE officeCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_VISITAREACODELAC_H */
