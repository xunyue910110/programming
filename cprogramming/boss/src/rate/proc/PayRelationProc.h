#ifndef PAYRELATIONPROC_H_HEADER_INCLUDED_B72F69DF
#define PAYRELATIONPROC_H_HEADER_INCLUDED_B72F69DF
#include "PayRelationSolution.h"
#include "Procedure.h"
#include "RateGlobalInfo.h"
#include "AcctBillMgr.h"
// 付费关系处理过程
//##ModelId=484F379303D8
class PayRelationProc : public Procedure
{
  public:
    //##ModelId=484F386701D4
   int execute();


  private:

    // 更新内存账单
    //##ModelId=48D73AD50117
    void updateBill();
    //绑定帐务关系
    //##ModelId=48D9DC190331
    void dealPayRelation(PayRelationSolution& payRelationSolution);
    
     
    // 该付费关系是否出错
    //##ModelId=48D1BB3803A4
    int m_ifErrorPayrealation;

    // 帐务处理属性对象，如资料等
    //##ModelId=48D707FC00E8
    ComBill *m_pComBill;
    
    //付费关系绑定对象
    PayRelationSolution m_payRelationSolution;
    
    //用户账户账单
    vector<UserAccountBill>     m_userAccountBillVector;
    
    AcctBillMgr m_acctBillMgr;
};



#endif /* PAYRELATIONPROC_H_HEADER_INCLUDED_B72F69DF */
