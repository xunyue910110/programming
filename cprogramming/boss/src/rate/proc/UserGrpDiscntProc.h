#ifndef USERGRPDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B
#define USERGRPDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B
#include "UserGroupDiscnt.h"
#include "Procedure.h"
#include "AcctBillMgr.h"
//##ModelId=484F338F0148
class UserGrpDiscntProc : public Procedure 
{
  public:
    //##ModelId=4850B4980232
    void pricing();
    //##ModelId=48D73C0F0119
    int execute();

  private:
    //##ModelId=48D740E70035
    bool init();
 
    //##ModelId=48B3618601B2
    UserGroupDiscnt m_userGroupDiscnt;
    //##ModelId=48D73C43038A
    ComBill *m_pComBill;
    
    AcctBillMgr m_acctBillMgr;
    
};



#endif /* USERGRPDISCNTPROC_H_HEADER_INCLUDED_B72F5F0B */
