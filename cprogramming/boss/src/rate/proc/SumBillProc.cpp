#include "SumBillProc.h"
#include "RateGlobalInfo.h"
#include "base/StringUtil.h"

INSTALL_PROCEDURE_INSTANCE(SumBillProc);
INSTALL_PROCEDURE_INSTANCE(ErrorProc);
INSTALL_PROCEDURE_INSTANCE(SumBillProc_Total);
INSTALL_PROCEDURE_INSTANCE(NullProc);
int NullProc::execute()
{
  long      t_value=0,t_totalFee=0;
  if((*m_pcdr).m_errorCode != E_DEFAULT) return 0;
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_totalFee;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_totalFee;
  for(int t_feeField=0;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    m_pcdr[t_feeField]  = m_pcdr[t_feeField+10];
    m_pcdr[t_feeField+5]= m_pcdr[t_feeField]; 
    t_value=m_pcdr[t_feeField+10].value();
    t_totalFee      += t_value;
  }
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_totalFee;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_totalFee;
  return 0;
}

int SumBillProc::execute()
{ //对于智能网话单只需合帐务账单
  long      t_value=0,t_totalFee=0;
  long      t_signFlag = 1;
  
  T_BILL_ID t_billId;
  UserBill  t_userBill;
  if((*m_pcdr).m_errorCode != E_DEFAULT) return 0;
  t_userBill.m_userId = m_pRateInfo->m_billUserId;
  //ATTRI_SUM_SIGNFLAG为1表示减免、优惠
  if(m_pcdr[ATTRI_SUM_SIGNFLAG].value() == 1) t_signFlag=-1;
  for(int t_feeField=0;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    m_pcdr[t_feeField]  = m_pcdr[t_feeField+10];
    m_pcdr[t_feeField+5]= m_pcdr[t_feeField]; 
    t_value=m_pcdr[t_feeField+10].value();
    t_totalFee      += t_value*t_signFlag;
    if(t_value!=0)
    {
      if(m_pmManager->getAcctDetailBill(t_feeField,
                                        m_pRateInfo->m_bizType,
                                        m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
                                        m_pcdr,
                                        m_pRateInfo->m_cycParam.m_cycleId,
                                        0,0,
                                        t_billId) ==false)
     {
        sprintf(m_errmsg,"求取帐务明细账单项失败,业务:%s,费用域:%d!",
                m_pRateInfo->m_bizType.c_str(),t_feeField);
        m_pRateInfo->setErrInfo(E_GET_ACCTDBILL,m_errmsg);
        return 1;
      }
      t_userBill.m_billId      = t_billId;
      t_userBill.m_value       = t_value; 
      t_userBill.m_discntValue = t_value;
      m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,t_userBill);
    }
  }
  
    if (strncmp(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),"B",1)==0)
    {
        vector<string> t_scpBillInfoList;
        string t_scpBillInfo;
        UserBill  t_userBill1;
        
        t_scpBillInfo = m_pcdr->getSpLongValue(ATTRI_SCP_BILLINFO);
        t_scpBillInfoList.clear();
        //118:110;127:110;149:110;
        StringUtil::split(t_scpBillInfo,";",t_scpBillInfoList,true);
        if (t_scpBillInfoList.size()>0)
        {
            for (int i = 0; i < t_scpBillInfoList.size(); ++i)
            {
            	char *pTmp;
            	t_userBill1.clear();
                t_userBill1.m_userId      = m_pRateInfo->m_billUserId;
                pTmp=strtok(const_cast<char *>(t_scpBillInfoList.at(i).c_str()),":");
                t_userBill1.m_billId      = atoi(pTmp);
                pTmp=strtok(NULL,":");
                t_userBill1.m_value       = atol(pTmp); 
                t_userBill1.m_discntValue = t_userBill1.m_value;
                m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,t_userBill1);
            }
        }
    }	

  setBillInfos();
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_totalFee;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_totalFee;
  return 0;
}

void SumBillProc::setBillInfos()
{
  m_pRateInfo->generateRollBill();
  m_billTrackLen = 0;m_billTrack[0]=0;
  for(vector<RollBackBill>::const_iterator t_itr=
        m_pRateInfo->m_rollBackBills.begin();
      t_itr!=m_pRateInfo->m_rollBackBills.end();
      ++t_itr)
  {
    if(t_itr->m_value == 0) continue;
    if(m_billTrackLen>0){sprintf(m_billTrack+m_billTrackLen,";");++m_billTrackLen;}
    if(t_itr->m_userId == m_pRateInfo->m_userId)
    {
      sprintf(m_billTrack+m_billTrackLen,"0|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }else
    {
      sprintf(m_billTrack+m_billTrackLen,"%s|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_userId.c_str(),
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }
    m_billTrackLen = strlen(m_billTrack);
  }
  m_pcdr->setSpLongValue(ATTRI_SPLONG_BILLLIST,m_billTrack); 
}

int ErrorProc::execute()
{ 
  if((*m_pcdr).m_errorCode != E_DEFAULT) return 0;

  sprintf(m_errmsg,"找不到对应的处理规则!");
  m_pRateInfo->setErrInfo(E_NOTFOUND_PROCRULE,m_errmsg);
  return 0;
}

int SumBillProc_Total::execute()
{ //对于有些业务需要做账单级的覆盖式合张
  long      t_value=0,t_totalFee=0;
  const UserBill *t_pUserBill=NULL;
  T_BILL_ID t_billId;
  UserBill  t_userBill;
  if((*m_pcdr).m_errorCode != E_DEFAULT) return 0;
  t_userBill.m_userId = m_pRateInfo->m_billUserId;
  for(int t_feeField=0;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    //江苏联通mon业务A7 费用1不处理 2009-3-9 15:27 gaojf (于永红、莫凡确认)
    if(m_pRateConfig->m_provinceCode == "034" && 
       m_pcdr[ATTRI_SOURCE_TYPE] == "A7" && t_feeField==1)
    {
      continue;
    }  
    m_pcdr[t_feeField]  = m_pcdr[t_feeField+10];
    m_pcdr[t_feeField+5]= m_pcdr[t_feeField]; 
    t_value=m_pcdr[t_feeField+10].value();
    t_totalFee      += t_value;
    if(m_pmManager->getAcctDetailBill(t_feeField,
                                        m_pRateInfo->m_bizType,
                                        m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
                                        m_pcdr,
                                        m_pRateInfo->m_cycParam.m_cycleId,
                                        0,0,
                                        t_billId) ==false)
    {
      if(t_value == 0)  continue;
      sprintf(m_errmsg,"求取帐务明细账单项失败,业务:%s,费用域:%d!",
              m_pRateInfo->m_bizType.c_str(),t_feeField);
      m_pRateInfo->setErrInfo(E_GET_ACCTDBILL,m_errmsg);
      return 1;
    }
    t_userBill.m_billId      = t_billId;
    
    if(t_value == 0)
    {
      if(m_pRateInfo->getBillItem<UserBill>(m_pRateInfo->m_userBills_new,t_userBill,t_pUserBill)==false)
      { //没有此项原始费用
        continue;
      }
    }
    t_userBill.m_value       = t_value; 
    t_userBill.m_discntValue = t_value;
    m_pRateInfo->updateBillTotal<UserBill>(m_pRateInfo->m_userBills_new,t_userBill);
  }
  setBillInfos();
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_totalFee;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_totalFee;
  return 0;
}

void SumBillProc_Total::setBillInfos()
{
  m_pRateInfo->generateRollBill();
  m_billTrackLen = 0;m_billTrack[0]=0;
  for(vector<RollBackBill>::const_iterator t_itr=
        m_pRateInfo->m_rollBackBills.begin();
      t_itr!=m_pRateInfo->m_rollBackBills.end();
      ++t_itr)
  {
    if(t_itr->m_value == 0) continue;
    if(m_billTrackLen>0){sprintf(m_billTrack+m_billTrackLen,";");++m_billTrackLen;}
    if(t_itr->m_userId == m_pRateInfo->m_userId)
    {
      sprintf(m_billTrack+m_billTrackLen,"0|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }else
    {
      sprintf(m_billTrack+m_billTrackLen,"%s|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_userId.c_str(),
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }
    m_billTrackLen = strlen(m_billTrack);
  }
  m_pcdr->setSpLongValue(ATTRI_SPLONG_BILLLIST,m_billTrack); 
}

int SumBillProc_OT::execute()
{ //对于有些业务需要做账单级的覆盖式合张
  long      t_value=0,t_totalFee=0;
  const UserBill *t_pUserBill=NULL;
  T_BILL_ID t_billId;
  UserBill  t_userBill;
  if((*m_pcdr).m_errorCode != E_DEFAULT) return 0;
  t_userBill.m_userId = m_pRateInfo->m_billUserId;
  t_userBill.m_billId = m_pcdr[ATTRI_ITEM_ID].value();
  
  m_pcdr[ATTRI_DISCFEE_FIELD1]  = m_pcdr[ATTRI_DISCFEE_FIELD1+10];
  m_pcdr[ATTRI_DISCFEE_FIELD1+5]= m_pcdr[ATTRI_DISCFEE_FIELD1]; 
  t_value=m_pcdr[ATTRI_DISCFEE_FIELD1+10].value();
  t_totalFee += t_value;    
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_totalFee;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_totalFee;
    
  if(t_value == 0)
  {
    if(m_pRateInfo->getBillItem<UserBill>(m_pRateInfo->m_userBills_new,t_userBill,t_pUserBill)==false)
    { //没有此项原始费用
      return 0;
    }
  }
  t_userBill.m_value       = t_value; 
  t_userBill.m_discntValue = t_value;
  m_pRateInfo->updateBillTotal<UserBill>(m_pRateInfo->m_userBills_new,t_userBill);

  setBillInfos();
  return 0;
}


