//judge FEMTO cdr by lac and cell_id
#ifndef PO_JUDGEFEMTOBYLACCELLID_H
#define PO_JUDGEFEMTOBYLACCELLID_H
#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_JudgeFemtoByLacCellid
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_JudgeFemtoByLacCellid(){}

    PO_JudgeFemtoByLacCellid(const PO_JudgeFemtoByLacCellid& right);

    void operator=(const PO_JudgeFemtoByLacCellid& right);

    friend int operator==(const PO_JudgeFemtoByLacCellid &left,
    		const PO_JudgeFemtoByLacCellid& right);

    friend int operator<(const PO_JudgeFemtoByLacCellid &left,
    		const PO_JudgeFemtoByLacCellid& right);

		friend ostream& operator<<(ostream& os,const PO_JudgeFemtoByLacCellid& po);

	struct
	{
		T_LAC lac;	               //FEMTO LAC
		T_CELL_ID cellId;          //FEMTO cellid
		T_DATE_TIME beginTime;     //生效时间
		T_DATE_TIME endTime;       //失效时间
		T_AREA_CODE visitAreaCode; //FEMTO归属区号
		T_ATTRI_VAL serviceType;   //服务代码 03:FEMTO
	}recordStruct;

  private:
	static PO_Property m_property;
};

#endif
