#ifndef ACCTTPPROC_H_HEADER_INCLUDED_B59A4C7C
#define ACCTTPPROC_H_HEADER_INCLUDED_B59A4C7C
#include "Procedure.h"
#include "RateGlobalInfo.h"
#include "ComBill.h"
#include "UserDiscnt.h"
#include "AcctDiscnt.h"
// 用户群/账户级资费求取，一般在帐务属性求取后做。
// 资费包括系统级资费，用户/用户群/客户/账户级资费；
// 用户级，账户级帐前调帐（用户群帐前调账暂不支持）。

//取用户级资费（含群和账户基础型）
class UserAcctTpProc : public Procedure
{
  public:
   //##ModelId=4A5E9E930267
   int execute();

   //##ModelId=4A6439C6038E
   bool  getTpInfo();
   
   bool  getPolicyCompByTp();
   
 public:
	   ComBill *m_pComBill;
	   UserDiscnt m_userDiscnt;
	   vector<UserTp> m_userTpVector;

};

//取群资费
class UserGrpTpProc : public Procedure
{
  public:

   int execute();

   bool getTpInfo();
   
 public:
	   ComBill *m_pComBill;
	   UserDiscnt m_userDiscnt;
	  // vector<UserTp> m_userTpVector;
};

//取账户级资费
class AccountTpProc : public Procedure
{
  public:

   int execute();

   bool getTpInfo();
      
 public:
	   ComBill *m_pComBill;
	   AcctDiscnt m_acctDiscnt;
	   vector<UserTp> m_userTpVector;
};

//取账户级优惠后执行的用户级资费add by xuzy 2011.2.7
class UserAfterAcctTpProc : public Procedure
{
  public:
   //##ModelId=4A5E9E930267
   int execute();

   //##ModelId=4A6439C6038E
   bool  getTpInfo();
   
   bool  getPolicyCompByTp();
   
 public:
	   ComBill *m_pComBill;
	   UserDiscnt m_userDiscnt;
	   vector<UserTp> m_userTpVector;

};

#endif /* ACCTTPPROC_H_HEADER_INCLUDED_B59A4C7C */
