#ifndef _PROVMONAUDITUNPROC_019_H_20081209_
#define _PROVMONAUDITUNPROC_019_H_20081209_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"


class ProvMonAuditUNProc_019:public CDRProc
{
  public:
    virtual int execute();

  protected:
  	//费用稽核
  	bool auditFee();

  	//准备用户状态稽核
  	bool preauditStatus();
  	bool auditSmStatus();
  	bool auditSpStatus();
  	bool auditppsSpStatus();

  	//用户订购关系稽核
  	bool auditsp();
  	
  	bool smsaudit();     //联通在信稽核
  	bool spaudit();      //普通代收费稽核
  	bool ppsspaudit();   //智能网代收费稽核
  	bool monthfeeaudit();//包月费用户状态稽核

  protected:
  	T_ATTRI_VAL m_userstate;
  	T_ATTRI_VAL m_stategroup;
  	T_ATTRI_VAL r_lastmonthstop;
  	char isVerifyState; //是否要稽核用户状态
  	char isVerifyOrder; //是否要稽核用户订购关系

};

#endif //_PROVMONAUDITUNPROC_019_H_20081209_
