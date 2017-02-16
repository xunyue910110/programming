#ifndef PO_CELLDEF_H_INCLUDE_20080926_
#define PO_CELLDEF_H_INCLUDE_20080926_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_CellDef
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_CellDef(){}

    PO_CellDef(const PO_CellDef& right);

    void operator=(const PO_CellDef& right);

    friend int operator==(const PO_CellDef &left,const PO_CellDef& right);

    friend int operator<(const PO_CellDef &left,const PO_CellDef& right);

	friend ostream& operator<<(ostream& os,const PO_CellDef& po);

    struct
    {
      T_ATTRI_VAL    m_areaCode;
  		T_PARTITION_ID m_partitionId;
      T_MSC          m_msc;
      T_LAC          m_lac;
      T_CELL_ID      m_cellId;
  		T_DATE_TIME    m_beginTime;
  		T_DATE_TIME    m_endTime;
    }m_record;

  private:
	static PO_Property m_property;

};



#endif //PO_CELLDEF_H_INCLUDE_20080926_

