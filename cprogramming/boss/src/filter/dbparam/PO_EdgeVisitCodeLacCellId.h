#ifndef PO_EDGEVISITCODELACCELLID_H
#define PO_EDGEVISITCODELACCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_EdgeVisitCodeLacCellId
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_EdgeVisitCodeLacCellId(){}

    PO_EdgeVisitCodeLacCellId(const PO_EdgeVisitCodeLacCellId& right);

    void operator=(const PO_EdgeVisitCodeLacCellId& right);

    friend int operator==(const PO_EdgeVisitCodeLacCellId &left,
    		const PO_EdgeVisitCodeLacCellId& right);

    friend int operator<(const PO_EdgeVisitCodeLacCellId &left,
    		const PO_EdgeVisitCodeLacCellId& right);

	friend ostream& operator<<(ostream& os,const PO_EdgeVisitCodeLacCellId& po);

    struct
    {
    	T_CELL_ID   m_cellId;
    	T_AREA_CODE m_visitAreaCode;
    	T_LAC       m_lac;
    	T_AREA_CODE m_feeCode;
    	T_DATE_TIME m_beginTime;
    	T_DATE_TIME m_endTime;
    	T_EDGE_FLAG m_edgeFlag;
    }recordStruct;

  private:
	static PO_Property m_property;

};


#endif /* PO_EdgeVisitCodeLacCellId_H */
