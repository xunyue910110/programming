#ifndef PO_EDGEMSCCELLID_H
#define PO_EDGEMSCCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_EdgeMscCellId
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_EdgeMscCellId(){}

    PO_EdgeMscCellId(const PO_EdgeMscCellId& right);

    void operator=(const PO_EdgeMscCellId& right);

    friend int operator==(const PO_EdgeMscCellId &left,
    		const PO_EdgeMscCellId& right);

    friend int operator<(const PO_EdgeMscCellId &left,
    		const PO_EdgeMscCellId& right);

	friend ostream& operator<<(ostream& os,const PO_EdgeMscCellId& po);

    struct
    {
    	T_CELL_ID cellId;
    	T_MSC msc;
    	T_AREA_CODE feeCode;
    	T_DATE_TIME beginTime;
    	T_DATE_TIME endTime;
    	T_EDGE_FLAG edgeFlag;
    	T_AREA_CODE visit_code;
    }recordStruct;

  private:
	static PO_Property m_property;

};


#endif /* PO_EDGEMSCCELLID_H */
