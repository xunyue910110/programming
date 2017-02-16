#ifndef PO_EDGEMSCLACCELLID_H
#define PO_EDGEMSCLACCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_EdgeMscLacCellId
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_EdgeMscLacCellId(){}

    PO_EdgeMscLacCellId(const PO_EdgeMscLacCellId& right);

    void operator=(const PO_EdgeMscLacCellId& right);

    friend int operator==(const PO_EdgeMscLacCellId &left,
    		const PO_EdgeMscLacCellId& right);

    friend int operator<(const PO_EdgeMscLacCellId &left,
    		const PO_EdgeMscLacCellId& right);

	friend ostream& operator<<(ostream& os,const PO_EdgeMscLacCellId& po);

    struct
    {
    	T_CELL_ID cellId;
    	T_MSC msc;
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


#endif /* PO_EDGEMSCLACCELLID_H */
