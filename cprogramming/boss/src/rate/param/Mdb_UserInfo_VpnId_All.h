#ifndef Mdb_UserInfo_VpnId_All_H_HEADER_INCLUDED
#define Mdb_UserInfo_VpnId_All_H_HEADER_INCLUDED

#include "Mdb_Common.h"
#include "Mdb_VpnInfo_Number.h"
#include "FieldsXX.h"

class Mdb_UserInfo_VpnId_All
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserInfo_VpnId_All(){}

    Mdb_UserInfo_VpnId_All(const Mdb_UserInfo_VpnId_All& right);

    void operator=(const Mdb_UserInfo_VpnId_All& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserInfo_VpnId_All& po);
    Mdb_UserInfo_VpnId_All& operator=(const Mdb_VpnInfo_Number& r_right);
    struct
    {
      T_SERVER_ID     m_userId;         // 三户标识
      T_MEMBER_TYPE   m_memberType;     // 成员角色
      T_MEMBER_TYPE   m_memberIdType;   // 成员角色标识类型
      T_SERVER_ID     m_memberId;       // 成员标识
      T_OTHER_PARTY   m_memberNumber;   // 成员号码
      T_PRI           m_orderNo;        // 成员优惠优先级   
    	T_DATE_TIME     m_beginTime;      // 起始时间
    	T_DATE_TIME     m_endTime;        // 终止时间
    	T_MEMBER_TYPE   m_lastMemberType; // 最底层角色类型
    	T_MEMBER_RELATION_TYPE m_relation_type;//组类型
    }m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_UserInfo_VpnId_All_H_HEADER_INCLUDED
