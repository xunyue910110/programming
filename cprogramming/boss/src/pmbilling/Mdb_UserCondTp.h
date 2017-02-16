#ifndef MDB_USERCONDTP_H_HEADER_INCLUDED_20080829
#define MDB_USERCONDTP_H_HEADER_INCLUDED_20080829

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserCondTp
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserCondTp(){}

    Mdb_UserCondTp(const Mdb_UserCondTp& right);

    void operator=(const Mdb_UserCondTp& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserCondTp& po);
    struct
    {
      T_TPOBJ_ID      m_tpObjId;        // 资费实例标识
    	T_TP_ID         m_tpId;           // 资费政策标识
    	T_DATE_TIME     m_beginTime;      // 起始时间
    	T_DATE_TIME     m_endTime;        // 终止时间
      char            m_idType;         // 三户类型
      T_SERVER_ID     m_userId;         // 三户标识
      T_SERVER_ID     m_relatin_vpnId;  // 关联群标识
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //Mdb_UserCondTp_H_HEADER_INCLUDED_20080829
