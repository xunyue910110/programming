#ifndef MDB_ACCT_UNIPAY_H_INCLUDED_20081014_
#define MDB_ACCT_UNIPAY_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

//定义付费关系信息. 
class AcctUnipay
{
  public:
    T_SERVER_ID  m_acctId;        //帐户ID
    T_MSISDN     m_msisdn;        //支付号码
    char         m_areaType;      //跨区域类型 0:全省跨地市统一付费 1:全国跨省份统一付费
    char         m_homeTag;       //统一付费地标志 0: 本地 1: 外地
};


class Mdb_Acct_Unipay
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_Acct_Unipay(){}

    Mdb_Acct_Unipay(const Mdb_Acct_Unipay& right);

    void operator=(const Mdb_Acct_Unipay& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_Acct_Unipay& po);
    AcctUnipay m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_ACCT_UNIPAY_H_INCLUDED_20081014_
