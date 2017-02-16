#ifndef PO_FIXTRUNK_H_20090326
#define PO_FIXTRUNK_H_20090326

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_FixTrunk
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_FixTrunk(){}

    PO_FixTrunk(const PO_FixTrunk& right);

    void operator=(const PO_FixTrunk& right);

    friend int operator==(const PO_FixTrunk &left,
    		const PO_FixTrunk& right);

    friend int operator<(const PO_FixTrunk &left,
    		const PO_FixTrunk& right);

	friend ostream& operator<<(ostream& os,const PO_FixTrunk& po);
    struct
    {
    	T_ATTRI_VAL area_code;
    	T_ATTRI_VAL msc;
    	T_ATTRI_VAL m_bill_type; //0:维持原号码不变 1:用代表号码替换原计费号码
    	T_ATTRI_VAL bill_number;
    	T_ATTRI_VAL serv_type;
    	T_ATTRI_VAL trunk_direction;
    	T_ATTRI_VAL trunk_group;
    	T_ATTRI_VAL m_begin_phone_a; //计费开始号码段
    	T_ATTRI_VAL m_end_phone_a;   //计费结束号码段
	    T_DATE_TIME m_beginDate;
	    T_DATE_TIME m_endDate;
    }recordStruct;
  private:
	static PO_Property m_property;
};

#endif /* PO_FIXTRUNK_H_20090326 */
