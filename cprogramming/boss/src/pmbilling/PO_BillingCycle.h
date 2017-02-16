#ifndef PO_BILLINGCYCLE_H_INCLUDE_20080923_
#define PO_BILLINGCYCLE_H_INCLUDE_20080923_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class PO_BillingCycle
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_BillingCycle(){}

    PO_BillingCycle(const PO_BillingCycle &right);

    void operator=(const PO_BillingCycle &right);

    friend int operator==(const PO_BillingCycle &left,const PO_BillingCycle &right);

    friend int operator<(const PO_BillingCycle &left,const PO_BillingCycle &right);

	friend ostream& operator<<(ostream& os,const PO_BillingCycle& po);

    struct
    {
      int        m_dbNo;     //库号 add by gaojf 2009-2-13 18:40
      T_EPARCHY_CODE	m_eparchyCode;	//地市编码 add by limeng 2009-9-21
    	T_DATE     m_cycleId;  //帐期YYYYMM
  		T_DATE     m_beginDate;//起始日期YYYYMMDD
  		T_DATE     m_endDate;  //结束日期YYYYMMDD
    }m_record;

  private:
	static PO_Property m_property;

}; 
#endif //PO_BILLINGCYCLE_H_INCLUDE_20080923_
