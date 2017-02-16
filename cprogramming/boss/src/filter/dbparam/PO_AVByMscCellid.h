#ifndef PO_AVBYMSCCELLID_H
#define PO_AVBYMSCCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByMscCellid
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByMscCellid(){}

    PO_AVByMscCellid(const PO_AVByMscCellid& right);

    void operator=(const PO_AVByMscCellid& right);

    friend int operator==(const PO_AVByMscCellid &left,
    		const PO_AVByMscCellid& right);

    friend int operator<(const PO_AVByMscCellid &left,
    		const PO_AVByMscCellid& right);

    friend ostream& operator<<(ostream& os,const PO_AVByMscCellid& po);

	struct
	{
		T_MSC msc;
		T_CELL_ID cellId;
		T_DATE_TIME   beginTime;
		T_DATE_TIME   endTime;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVByMscCellid_H */
