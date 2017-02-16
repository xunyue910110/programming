#ifndef MDB_USERTP_H_HEADER_INCLUDED_20080829
#define MDB_USERTP_H_HEADER_INCLUDED_20080829

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserTp
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserTp(){}

    Mdb_UserTp(const Mdb_UserTp& right);

    void operator=(const Mdb_UserTp& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserTp& po);
    struct
    {
      T_TPOBJ_ID      m_tpObjId;        // 资费实例标识
    	T_TP_ID         m_tpId;           // 资费政策标识
    	T_DATE_TIME     m_beginTime;      // 起始时间
    	T_DATE_TIME     m_endTime;        // 终止时间
      char            m_idType;         // 三户类型
      T_SERVER_ID     m_userId;         // 三户标识
      int             m_productId;      // 产品标识
      int             m_servBund_Id;    // 服务包标识
      int             m_servId;         // 服务标识
      int             m_feeBund_Id;     // 资费包标识 
      T_SERVER_ID     m_relatin_vpnId;  // 关联群标识
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_USERTP_H_HEADER_INCLUDED_20080829
