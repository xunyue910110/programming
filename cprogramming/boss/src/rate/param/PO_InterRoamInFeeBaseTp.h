#ifndef PO_INTERROAMINFEEBASETP_H
#define PO_INTERROAMINFEEBASETP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_InterRoamInFeeBaseTp
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_InterRoamInFeeBaseTp(){}

    PO_InterRoamInFeeBaseTp(const PO_InterRoamInFeeBaseTp& right);

    void operator=(const PO_InterRoamInFeeBaseTp& right);

    friend int operator==(const PO_InterRoamInFeeBaseTp &left,
    		const PO_InterRoamInFeeBaseTp& right);

    friend int operator<(const PO_InterRoamInFeeBaseTp &left,
    		const PO_InterRoamInFeeBaseTp& right);

	friend ostream& operator<<(ostream& os,const PO_InterRoamInFeeBaseTp& po);

    struct
    {
    	T_FEE_ID         m_tpId;
  		T_PRI            m_priority;
  		T_FEE_FIELD      m_feeField;
  		T_DATE_TIME      m_beginTime;
  		T_DATE_TIME      m_endTime;
  		T_INTER_ASPGROUP m_aspGroup;
  		T_BIZ_TYPE       m_bizType;
  		char             m_sameHomeAb;
  		T_AREA_CODE      m_homeCodeB;
  		T_CALL_TYPE      m_callType;
  		T_ASP            m_bAsp;
  		T_SERV_TYPE      m_bServType;
  		T_CARRIER_TYPE   m_carrierType;
  		T_LONG_TYPE      m_longType1;
  		T_LONG_GROUP     m_longGroup1;
  		T_LONG_TYPE      m_longType2;
  		T_LONG_GROUP     m_longGroup2;
    }m_record;

  private:
	static PO_Property m_property;
};



#endif /* PO_AREACODEIMSII_H */

