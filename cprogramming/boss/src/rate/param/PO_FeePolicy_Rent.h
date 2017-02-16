#ifndef _PO_FEEPOLICY_RENT_H_INCLUDE_20081023_
#define _PO_FEEPOLICY_RENT_H_INCLUDE_20081023_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_FeePolicy_Rent
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_FeePolicy_Rent(){}

    PO_FeePolicy_Rent(const PO_FeePolicy_Rent& right);

    void operator=(const PO_FeePolicy_Rent& right);

    friend int operator==(const PO_FeePolicy_Rent &left,const PO_FeePolicy_Rent& right);

    friend int operator<(const PO_FeePolicy_Rent &left,const PO_FeePolicy_Rent& right);

	friend ostream& operator<<(ostream& os,const PO_FeePolicy_Rent& po);

    struct
    {
    	T_TP_ID        m_tpId;
  		T_MEMBER_TYPE  m_roleCode;
  		T_BIZ_TYPE     m_eventType;
  		long           m_monthRent;
  		//long           m_halfRent;
  		char           m_compMethod;
  		int            m_sumBillId;
  		int            m_billId;
    }m_record;

  private:
	  static PO_Property m_property;

};



#endif //_PO_FEEPOLICY_RENT_H_INCLUDE_20081023_
