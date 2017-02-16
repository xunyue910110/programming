#ifndef PO_FixSpecialPhoneSection_H
#define PO_FixSpecialPhoneSection_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_FixSpecialPhoneSection
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_FixSpecialPhoneSection(){}

    PO_FixSpecialPhoneSection(const PO_FixSpecialPhoneSection& right);

    void operator=(const PO_FixSpecialPhoneSection& right);

    friend int operator==(const PO_FixSpecialPhoneSection &left,
    		const PO_FixSpecialPhoneSection& right);

    friend int operator<(const PO_FixSpecialPhoneSection &left,
    		const PO_FixSpecialPhoneSection& right);

    friend ostream& operator<<(ostream& os,const PO_FixSpecialPhoneSection& po);

    struct
      {
      	T_FLAG        m_matchType;
      	T_PREFIX_TYPE m_prefixType;
      	T_ATTRI_VAL   m_phone;
      	T_AREA_CODE   m_areaCode;
    	  T_ATTRI_VAL   m_sectionCode; //营业区
      	T_DATE_TIME   m_beginTime;
      	T_DATE_TIME   m_endTime;
      }recordStruct;
  private:
	static PO_Property m_property;
};

#endif /* PO_FixSpecialPhoneSection_H */
