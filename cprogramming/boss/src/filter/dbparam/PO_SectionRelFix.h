#ifndef PO_SECTIONRELFIX_H_20090814
#define PO_SECTIONRELFIX_H_20090814

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_SectionRelFix
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SectionRelFix(){}

    PO_SectionRelFix(const PO_SectionRelFix& right);

    void operator=(const PO_SectionRelFix& right);

    friend int operator==(const PO_SectionRelFix&left,
    		const PO_SectionRelFix& right);

    friend int operator<(const PO_SectionRelFix &left,
    		const PO_SectionRelFix& right);

	friend ostream& operator<<(ostream& os,const PO_SectionRelFix& po);

    struct
    {
    	int           m_priority;     //优先级
    	T_MSISDN      m_phoneSegA;    //计费号码头
    	T_ATTRI_VAL   m_sectionCodeA; //计费营业区
    	T_MSISDN      m_phoneSegB;    //对端号码头
    	T_ATTRI_VAL   m_sectionCodeB; //对端营业区
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    	T_ATTRI_VAL   m_areaType;     //区内区间标志 0:区内1:区间2:其他
    }recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_SECTIONRELFIX_H_20090814 */
