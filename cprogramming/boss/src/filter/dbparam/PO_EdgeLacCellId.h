#ifndef PO_EDGELACCELLID_H
#define PO_EDGELACCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_EdgeLacCellId
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_EdgeLacCellId(){}

    PO_EdgeLacCellId(const PO_EdgeLacCellId& right);

    void operator=(const PO_EdgeLacCellId& right);

    friend int operator==(const PO_EdgeLacCellId &left,
    		const PO_EdgeLacCellId& right);

    friend int operator<(const PO_EdgeLacCellId &left,
    		const PO_EdgeLacCellId& right);

	friend ostream& operator<<(ostream& os,const PO_EdgeLacCellId& po);

    struct
    {
    	T_CELL_ID cellId;
    	T_LAC lac;
    	T_AREA_CODE feeCode;
    	T_DATE_TIME beginTime;
    	T_DATE_TIME endTime;
    	T_EDGE_FLAG edgeFlag;
    	T_AREA_CODE visit_code;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_EdgRoamLacCellId_H */
