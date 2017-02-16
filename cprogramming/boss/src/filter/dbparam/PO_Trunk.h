#ifndef PO_TRUNK_H
#define PO_TRUNK_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_Trunk
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_Trunk(){}

    PO_Trunk(const PO_Trunk& right);

    void operator=(const PO_Trunk& right);

    friend int operator==(const PO_Trunk &left,
    		const PO_Trunk& right);

    friend int operator<(const PO_Trunk &left,
    		const PO_Trunk& right);

	friend ostream& operator<<(ostream& os,const PO_Trunk& po);
    struct
    {
    	T_ATTRI_VAL msc;
    	T_ATTRI_VAL trunkid;
    	T_ATTRI_VAL trunkdirection;
    	T_ATTRI_VAL trunktype;
    	T_ATTRI_VAL busitype; //=1专线
    	T_ATTRI_VAL usertype;
    	T_ATTRI_VAL filterflag;
	    T_DATE_TIME m_beginDate;
	    T_DATE_TIME m_endDate;
    }recordStruct;
  private:
	static PO_Property m_property;
};

#endif /* PO_TRUNK_H */
