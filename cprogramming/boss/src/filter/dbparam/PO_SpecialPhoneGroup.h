#ifndef PO_SPECIALPHONEGROUP_H
#define PO_SPECIALPHONEGROUP_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_SpecialPhoneGroup
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpecialPhoneGroup(){}

    PO_SpecialPhoneGroup(const PO_SpecialPhoneGroup& right);

    void operator=(const PO_SpecialPhoneGroup& right);

    friend int operator==(const PO_SpecialPhoneGroup &left,
    		const PO_SpecialPhoneGroup& right);

    friend int operator<(const PO_SpecialPhoneGroup &left,
    		const PO_SpecialPhoneGroup& right);

    friend ostream& operator<<(ostream& os,const PO_SpecialPhoneGroup& po);

    struct
      {
      	T_FLAG        m_matchType;
      	T_PREFIX_TYPE m_prefixType;
      	T_MSISDN      m_phone;
      	T_AREA_CODE   m_areaCode;
      	int           m_groupId;
      	T_DATE_TIME   m_beginTime;
      	T_DATE_TIME   m_endTime;
      }recordStruct;
  private:
	static PO_Property m_property;
};

class PO_SpecialPhoneGroup_CNC:public PO_SpecialPhoneGroup
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};

#endif /* PO_SPECIALPHONEGROUP_H */
