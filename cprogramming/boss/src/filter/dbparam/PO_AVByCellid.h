#ifndef PO_AVBYCELLID_H
#define PO_AVBYCELLID_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

//add by luoyb
class PO_AVByCellid
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByCellid(){}

    PO_AVByCellid(const PO_AVByCellid& right);

    void operator=(const PO_AVByCellid& right);

    friend int operator==(const PO_AVByCellid &left,
    		const PO_AVByCellid& right);

    friend int operator<(const PO_AVByCellid &left,
    		const PO_AVByCellid& right);

	friend ostream& operator<<(ostream& os,const PO_AVByCellid& po);

	struct
	{
		T_CELL_ID cellId;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_AREA_CODE visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYLAC_H */
