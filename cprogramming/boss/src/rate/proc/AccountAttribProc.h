#ifndef ACCOUNTATTRIBPROC_H_HEADER_INCLUDED_B59A0520
#define ACCOUNTATTRIBPROC_H_HEADER_INCLUDED_B59A0520

#include "Procedure.h"
#include "ComBill.h"
#include "BillStruct.h"
typedef IdInfo    T_ID_INFO;

// 帐务属性求取过程
//##ModelId=4A5E9C2C0082
class AccountAttribProc : public Procedure
{
  public:
    // initAcct
    // getAcctInfo
    int execute();

  private:
	ComBill *m_pComBill;
	//资料管理对象指针
	PM_Account *m_pAccountParam;

};



#endif /* ACCOUNTATTRIBPROC_H_HEADER_INCLUDED_B59A0520 */
