#ifndef PO_MSISDNSECTIONFIX_H_20090814
#define PO_MSISDNSECTIONFIX_H_20090814

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_MsisdnSectionFix
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_MsisdnSectionFix(){}

    PO_MsisdnSectionFix(const PO_MsisdnSectionFix& right);

    void operator=(const PO_MsisdnSectionFix& right);

    friend int operator==(const PO_MsisdnSectionFix&left,
    		const PO_MsisdnSectionFix& right);

    friend int operator<(const PO_MsisdnSectionFix &left,
    		const PO_MsisdnSectionFix& right);

	friend ostream& operator<<(ostream& os,const PO_MsisdnSectionFix& po);

    struct
    {
      T_ATTRI_VAL   m_areaCode;     //手机区号
    	T_MSISDN      m_msisdnSeg;    //手机号头
    	T_ATTRI_VAL   m_sectionCode;  //营业区
    	T_DATE_TIME   m_beginTime;
    	T_DATE_TIME   m_endTime;
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_MSISDNSECTIONFIX_H_20090814 */
