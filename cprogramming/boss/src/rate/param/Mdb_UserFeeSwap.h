#ifndef MDB_USERFEESWAP_H_INCLUDED_20081014_
#define MDB_USERFEESWAP_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

//定义用户间费用互转信息. 
class UserFeeSwap
{
  public:
    UserFeeSwap(){}
    virtual ~UserFeeSwap(){}

    friend int operator < (const UserFeeSwap &r_left,const UserFeeSwap &r_right)
    {
    	 /*if (r_left.m_priority == r_right.m_priority)
    	 {
    	 	 if (r_left.m_limitId == r_right.m_limitId)
    	 	 {
    	 	   return (r_left.m_orderNo < r_right.m_orderNo);
    	 	 }
    	 	 else
    	 	 	 return (r_left.m_limitId < r_right.m_limitId);
    	 }
       else*/
        return (r_left.m_priority > r_right.m_priority);
      
    } 
   
    T_SERVER_ID  m_srcUserId;     //费用转移源帐户	   
    T_SERVER_ID  m_objUserId;     //费用转移目标帐户	
    int          m_swapItemCode;  //费用转移帐目	    
    int          m_priority;      //费用转移优先级	  
    char         m_swapType;      //费用转移限定标识 
    int          m_startCycId;    //生效帐期
    int          m_endCycId;      //失效帐期 
    //int          m_limitId;       //限定标识
    //int          m_orderNo;       //序列号 
    //long         m_minValue;      //最小金额
    //long         m_maxValue;      //最大金额
    char         m_limitType;     //限定方式
    long         m_limitValue;    //限定额度
    //long         m_limitRatio;    //限定比例
    //char         m_fillTag;       //补足标识

};


class Mdb_UserFeeSwap
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserFeeSwap(){}

    Mdb_UserFeeSwap(const Mdb_UserFeeSwap& right);

    void operator=(const Mdb_UserFeeSwap& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserFeeSwap& po);
    UserFeeSwap m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_USERFEESWAP_H_INCLUDED_20081014_
