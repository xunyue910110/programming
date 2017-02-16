#ifndef Mdb_PayRelation_H_INCLUDED_20081014_
#define Mdb_PayRelation_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"
//#include "Mdb_FeeChgLimit.h"

//定义付费关系信息. 
class PayRelation
{
  public:
    PayRelation(){/*m_feeChgLimitVec.clear();*/}
    virtual ~PayRelation(){/*m_feeChgLimitVec.clear();*/}

    friend int operator < (const PayRelation &r_left,const PayRelation &r_right)
    {
      if (r_left.m_defaultTag == r_right.m_defaultTag)
      {
        if (r_left.m_payPriority == r_right.m_payPriority)
          return (r_left.m_acctId > r_right.m_acctId);//zhaogc add 20091205
        else
          return (r_left.m_payPriority > r_right.m_payPriority);
      }
      else
      {
        return (r_left.m_defaultTag < r_right.m_defaultTag);
      }        
    } 
    T_TPOBJ_ID   m_payRelationId; //资费实例标识
    T_SERVER_ID  m_userId;        //用户ID
    int          m_itemCode;      //帐目
    T_SERVER_ID  m_acctId;        //帐户ID
    char         m_bindType;      //费用帮定支付帐户方式
    int          m_payPriority;   //费用支付优先级
    int          m_discntPriority;//用户优惠优先级
    char         m_defaultTag;    //默认标志
    char         m_addupMethod;   //累计方式
    long         m_addupMonths;   //累计月份
    //int          m_limitId;       //限定标识
    //char         m_valueType;     //费用类型(参数查询不填)
    //int          m_value;         //费用(参数查询不填)
    char         m_actTag;        //作用标记
    int          m_startCycId;    //生效帐期
    int          m_endCycId;      //失效帐期
    //vector <FeeChgLimit> m_feeChgLimitVec; //费用支付限定
//    int          m_orderNo;       //序列号 
//    long         m_minValue;      //最小金额
//    long         m_maxValue;      //最大金额
    char         m_limitType;     //限定方式
    long         m_limitValue;    //限定额度
//    long         m_limitRatio;    //限定比例
    char         m_fillTag;       //补足标识
    

};


class Mdb_PayRelation
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_PayRelation(){}

    Mdb_PayRelation(const Mdb_PayRelation& right);

    void operator=(const Mdb_PayRelation& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_PayRelation& po);
    PayRelation m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //Mdb_PayRelation_H_INCLUDED_20081014_
