#ifndef PO_SpecInfo_org_H
#define PO_SpecInfo_org_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_SpecInfo_org
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpecInfo_org(){}

    PO_SpecInfo_org(const PO_SpecInfo_org& right);

    void operator=(const PO_SpecInfo_org& right);

    friend int operator==(const PO_SpecInfo_org &left,const PO_SpecInfo_org& right);

    friend int operator<(const PO_SpecInfo_org &left,const PO_SpecInfo_org& right);

	friend ostream& operator<<(ostream& os,const PO_SpecInfo_org& po);

    struct{
      T_MEMBER_TYPE       m_spInfoType;  // 特殊信息类型：小区、号码
      T_SPECIALINFO_ID    m_spInfoId;    // 特殊信息ID
      T_LEVEL_TYPE        m_level;       // 层次
      T_SPECIALINFO_ID    m_superId;     // 上级ID
    }m_record;
  private:
	  static PO_Property m_property;

};



#endif /* PO_SpecInfo_org_H */
