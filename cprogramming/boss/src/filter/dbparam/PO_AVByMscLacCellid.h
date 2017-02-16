#ifndef PO_AVBYMSCLACCELLID_H
#define PO_AVBYMSCLACCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByMscLacCellid
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByMscLacCellid(){}

    PO_AVByMscLacCellid(const PO_AVByMscLacCellid& right);

    void operator=(const PO_AVByMscLacCellid& right);

    friend int operator==(const PO_AVByMscLacCellid &left,
    		const PO_AVByMscLacCellid& right);

    friend int operator<(const PO_AVByMscLacCellid &left,
    		const PO_AVByMscLacCellid& right);

	friend ostream& operator<<(ostream& os,const PO_AVByMscLacCellid& po);

	struct
	{
		T_MSC msc;
		T_LAC lac;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_CELL_ID cellId;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYMSCLACCELLID_H */
