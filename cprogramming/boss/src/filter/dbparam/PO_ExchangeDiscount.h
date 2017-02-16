#ifndef PO_EXCHANGEDISCOUNT_H_20090814
#define PO_EXCHANGEDISCOUNT_H_20090814

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_ExchangeDiscount
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_ExchangeDiscount(){}

    PO_ExchangeDiscount(const PO_ExchangeDiscount& right);

    void operator=(const PO_ExchangeDiscount& right);

    friend int operator==(const PO_ExchangeDiscount&left,
    		const PO_ExchangeDiscount& right);

    friend int operator<(const PO_ExchangeDiscount &left,
    		const PO_ExchangeDiscount& right);

	friend ostream& operator<<(ostream& os,const PO_ExchangeDiscount& po);

    struct
    {
    	T_ATTRI_VAL   m_msc;
    	T_ATTRI_VAL   m_beginPhone_a;
    	T_ATTRI_VAL   m_endPhone_a;
    	T_ATTRI_VAL   m_ServType;
    	T_ATTRI_VAL   m_beginPhone_b;
    	T_ATTRI_VAL   m_endPhone_b;
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_EXCHANGEDISCOUNT_H_20090814 */
