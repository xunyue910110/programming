#ifndef PO_BASECELLTP_H_INCLUDE_20080926_
#define PO_BASECELLTP_H_INCLUDE_20080926_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_BaseCellTp
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_BaseCellTp(){}

    PO_BaseCellTp(const PO_BaseCellTp& right);

    void operator=(const PO_BaseCellTp& right);

    friend int operator==(const PO_BaseCellTp &left,const PO_BaseCellTp& right);

    friend int operator<(const PO_BaseCellTp &left,const PO_BaseCellTp& right);

	friend ostream& operator<<(ostream& os,const PO_BaseCellTp& po);

    struct
    {
  		T_CALL_TYPE    m_callType;
  		T_ROAM_TYPE    m_roamType;
  		T_SERV_TYPE    m_servTypea;
  		T_AREA_CODE    m_homeAreaCode;
  		T_PARTITION_ID m_partitionId;
    	T_TP_ID        m_tpId;
  		T_DATE_TIME    m_beginTime;
  		T_DATE_TIME    m_endTime;
  		int            m_priority;
    }m_record;

  private:
	static PO_Property m_property;

};



#endif //PO_BASECELLTP_H_INCLUDE_20080926_

