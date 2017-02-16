#ifndef _LOCALPROCACCT019_H_INCLUDE_20090318_
#define _LOCALPROCACCT019_H_INCLUDE_20090318_

#include "Procedure.h"
#include "config-all.h"
#include "RateGlobalInfo.h"
#include "AcctGiftDeal.h"

//赠款处理
class AcctGiftDealProc:public Procedure
{
  public:
    virtual int execute();
    
    void getBill();
		void updateBill(); 
		  
  private:
  	AcctGiftDeal m_acctGiftDeal;  
		vector<UserOtherInfo> m_userOtherInfoVec;
		vector<AccountBill>   m_accountBillVec;
};


#endif //_LOCALPROCACCT019_H_INCLUDE_20090318_

