#ifndef PO_EDGECELLID_H
#define PO_EDGECELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_EdgeCellId
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_EdgeCellId(){}

    PO_EdgeCellId(const PO_EdgeCellId& right);

    void operator=(const PO_EdgeCellId& right);

    friend int operator==(const PO_EdgeCellId &left,
    		const PO_EdgeCellId& right);

    friend int operator<(const PO_EdgeCellId &left,
    		const PO_EdgeCellId& right);

	friend ostream& operator<<(ostream& os,const PO_EdgeCellId& po);

    struct
    {
    	T_CELL_ID cellId;
    	T_AREA_CODE feeCode;
    	T_DATE_TIME beginTime;
    	T_DATE_TIME endTime;
    	T_EDGE_FLAG edgeFlag;
    	T_AREA_CODE visit_code;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_EDGROAMCELLID_H */
