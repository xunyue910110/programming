#ifndef ACCTDISCNTPROC_H_HEADER_INCLUDED_B72F4815
#define ACCTDISCNTPROC_H_HEADER_INCLUDED_B72F4815
#include "AcctDiscnt.h"
#include "Procedure.h"
#include "RateGlobalInfo.h"

// 账户优惠过程类：实现优惠对象为账户的优惠
//##ModelId=48ACC5B10012
class AcctDiscntProc : public Procedure
{
  public:
    // 账户优惠主流程
    //##ModelId=48ACC5B10064
    int execute();

  private:
     
    bool init();
    
    bool pricing();
    
    bool updateBill(); 
    
    //##ModelId=48B361B700AE
    AcctDiscnt m_acctDiscnt;
    
    ComBill  *m_pComBill;

};



#endif /* ACCTDISCNTPROC_H_HEADER_INCLUDED_B72F4815 */
