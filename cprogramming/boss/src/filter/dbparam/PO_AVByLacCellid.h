#ifndef PO_AVBYLACCELLID_H
#define PO_AVBYLACCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByLacCellid
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByLacCellid(){}

    PO_AVByLacCellid(const PO_AVByLacCellid& right);

    void operator=(const PO_AVByLacCellid& right);

    friend int operator==(const PO_AVByLacCellid &left,
    		const PO_AVByLacCellid& right);

    friend int operator<(const PO_AVByLacCellid &left,
    		const PO_AVByLacCellid& right);

	friend ostream& operator<<(ostream& os,const PO_AVByLacCellid& po);

	struct
	{
		T_MSC msc;
		T_LAC lac;
		T_CELL_ID cellId;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYLACCELLID_H */

