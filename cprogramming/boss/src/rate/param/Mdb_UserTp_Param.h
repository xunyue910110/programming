#ifndef MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829
#define MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_UserTp_Param
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserTp_Param(){}

    Mdb_UserTp_Param(const Mdb_UserTp_Param& right);

    void operator=(const Mdb_UserTp_Param& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserTp_Param& po);
    struct
    {
      T_TPOBJ_ID        m_tpObjId;        // 资费实例标识
      T_TP_ID           m_tpParamId;      // 资费参数标识
      T_TPPARAM_VALUE   m_tpValue;        // 参数值
    	T_TIME            m_beginTime;      // 起始时间
    	T_TIME            m_endTime;        // 终止时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829
