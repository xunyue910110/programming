#ifndef USERDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B
#define USERDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B
#include "UserDiscnt.h"
#include "UserGroupDiscnt.h"
#include "Procedure.h"
#include "AcctBillMgr.h"
//##ModelId=484F338F0148
class UserAfterAcctDiscntProc : public Procedure 
{
  public:
    //##ModelId=4850B4980232
    void pricing();
    //##ModelId=48D73C0F0119
    int execute();

  private:
    //##ModelId=48D740E70035
    bool init();
    // 更新输出账单和内存账单
    //##ModelId=48AF623A0392
    void updateBill();
 
    //##ModelId=48B3614E03C4
    UserDiscnt m_userDiscnt;

    //##ModelId=48B3618601B2
    //UserGroupDiscnt m_userGroupDiscnt;
    //##ModelId=48D73C43038A
    ComBill *m_pComBill;
    
    AcctBillMgr m_acctBillMgr;
    
};



#endif /* USERDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B */
