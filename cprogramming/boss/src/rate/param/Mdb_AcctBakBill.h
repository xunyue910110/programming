#ifndef Mdb_AcctBakBill_H_INCLUDED_20081014_
#define Mdb_AcctBakBill_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

//定义付费关系信息. 
class AcctBakBill
{
  public:
    T_SERVER_ID  m_acctId;        //帐户ID
    T_SERVER_ID  m_userId;        //用户ID
    int          m_cycId;         //帐期
    int          m_itemCode;      //帐目
    long         m_fee;           //实际费用
    long         m_bDiscnt;       //帐务优惠费用
    long         m_aDiscnt;       //帐户优惠费用
    long         m_bAdjust;       //调帐优惠费用
};


class Mdb_AcctBakBill
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_AcctBakBill(){}

    Mdb_AcctBakBill(const Mdb_AcctBakBill& right);

    void operator=(const Mdb_AcctBakBill& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_AcctBakBill& po);
    AcctBakBill m_record;

  private:
	static Mdb_Property m_property;
};

#endif //Mdb_AcctBakBill_H_INCLUDED_20081014_
