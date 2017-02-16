#ifndef PO_SPECIALPHONE_H
#define PO_SPECIALPHONE_H

#include "dbparam/PO_Common.h"
#include "UserProperty.h"

class PO_SpecialPhone
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpecialPhone(){}

    PO_SpecialPhone(const PO_SpecialPhone& right);

    void operator=(const PO_SpecialPhone& right);

    friend int operator==(const PO_SpecialPhone &left,
    		const PO_SpecialPhone& right);

    friend int operator<(const PO_SpecialPhone &left,
    		const PO_SpecialPhone& right);

	  friend ostream& operator<<(ostream& os,const PO_SpecialPhone& po);

/*
    struct
    {
		  T_FLAG        m_matchType;
		  T_PREFIX_TYPE m_preFixType;
		  int           m_condition;
		  T_MSISDN      m_msisdn;
		  T_AREA_CODE   m_areacode;
		  T_PRI         m_priority;
		  T_SERV_TYPE   m_servType;
		  T_ASP         m_asp;
		  T_HOME_TYPE   m_homeType;
		  int           m_groupId;
		  int           m_length;
		  T_CALLED_TYPE m_calledType;
		  T_FLAG        m_isNoNeedTp;
		  T_ATTRI_VAL   m_telitem;    //山东联通特殊标识
		  T_DATE_TIME   m_beginTime;
		  T_DATE_TIME   m_endTime;
    }recordStruct;
*/
    SpecialPhoneInfo recordStruct;

  private:
	static PO_Property m_property;
};

class PO_SpecialPhone_CNC:public PO_SpecialPhone
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};


#endif /* PO_SPECIALPHONE_H */
