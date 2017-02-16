#ifndef _MDB_VPNINFOBYPHONE_H_20090214_
#define _MDB_VPNINFOBYPHONE_H_20090214_

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_VpnInfobyPhone
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_VpnInfobyPhone(){}

    Mdb_VpnInfobyPhone(const Mdb_VpnInfobyPhone& right);

    void operator=(const Mdb_VpnInfobyPhone& right);

    friend int operator<(const Mdb_VpnInfobyPhone &left,const Mdb_VpnInfobyPhone& right);

    friend ostream& operator<<(ostream& os,const Mdb_VpnInfobyPhone& po);

    struct
    {
      long            m_userId;         // 三户标识
      T_MEMBER_TYPE   m_memberType;     // 成员角色
      T_MEMBER_TYPE   m_memberIdType;   // 成员角色标识类型
      long            m_memberId;       // 成员标识
      T_OTHER_PARTY   m_memberNumber;   // 成员号码
    	T_DATE_TIME     m_beginTime;      // 起始时间
    	T_DATE_TIME     m_endTime;        // 终止时间
    	T_MEMBER_TYPE   m_lastMemberType; // 最底层角色类型

    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_VPNINFOBYPHONE_H_20090214_