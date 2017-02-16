#ifndef PO_NumberConvert_H_20090814
#define PO_NUMBERCONVERT_H_20090814

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_NumberConvert
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_NumberConvert(){}

    PO_NumberConvert(const PO_NumberConvert& right);

    void operator=(const PO_NumberConvert& right);

    friend int operator==(const PO_NumberConvert&left,
    		const PO_NumberConvert& right);

    friend int operator<(const PO_NumberConvert &left,
    		const PO_NumberConvert& right);

	friend ostream& operator<<(ostream& os,const PO_NumberConvert& po);

    struct
    {
    	T_ATTRI_VAL   m_msc;
    	T_MSISDN      m_phone;
    	T_ATTRI_VAL   m_ServType;
    	T_ATTRI_VAL   m_bill_type; //0:维持原号码不变 1:用代表号码替换原计费号码
    	T_MSISDN      m_billNumber;
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_NUMBERCONVERT_H_20090814 */
