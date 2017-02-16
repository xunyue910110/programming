#ifndef Mdb_VpnInfo_Number_H_HEADER_INCLUDED_20080901
#define Mdb_VpnInfo_Number_H_HEADER_INCLUDED_20080901

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_VpnInfo_Number
{
  public:
    friend class Mdb_VpnInfo_UserId;
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_VpnInfo_Number(){}

    Mdb_VpnInfo_Number(const Mdb_VpnInfo_Number& right);

    void operator=(const Mdb_VpnInfo_Number& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_VpnInfo_Number& po);
    struct
    {
      T_SERVER_ID     m_userId;         // 三户标识
      T_MEMBER_TYPE   m_memberType;     // 成员角色
      T_MEMBER_TYPE   m_memberIdType;   // 成员角色标识类型
      T_SERVER_ID     m_memberId;       // 成员标识
      T_OTHER_PARTY   m_memberNumber;   // 成员号码
    	T_DATE_TIME     m_beginTime;      // 起始时间
    	T_DATE_TIME     m_endTime;        // 终止时间
    	T_MEMBER_TYPE   m_lastMemberType; // 最底层角色类型
    	//为了支持子母集团，小集团内和集团间不同优惠而增加 gaojf 2009-3-21 11:15
    	T_SERVER_ID     m_lastVpnId;      // 最底层的集团ID
    	T_MEMBER_RELATION_TYPE m_relation_type;//组类型

    }m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_VpnInfo_Number_H_HEADER_INCLUDED_20080901
