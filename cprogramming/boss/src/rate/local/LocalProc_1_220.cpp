#include "LocalProc_1_220.h"

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCEDURE_INSTANCE(SetOriginFeeFlag);
INSTALL_PROCEDURE_INSTANCE(LocalizeGsmFreeFlagProc);
INSTALL_PROCEDURE_INSTANCE(LocalizeGsmFreeFlagProc);
int IsNoNeedTPSpecialProc::execute()
{
	//IP直通车保留原始费用，不取用户政策
	//Modified by Sunam 2006-3-29 17:18 海南IP直通车要求用户政策
	//if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IPCALL)
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IPCALL) && 
	    (m_pRateConfig->m_provinceCode != "898"))
	{
		return true;
	}  
  
  
  return 0;
}

int LocalizeGsmFreeFlagProc::execute()
{
  
    // wap：17266，17201,163,169前60s免 
    if (((m_pcdr[ATTRI_USER_SERVICE_TYPE_B] == SERVICE_TYPE_WAP_17266)
        || (m_pcdr[ATTRI_USER_SERVICE_TYPE_B] == SERVICE_TYPE_WAP_17201)
        || (m_pcdr[ATTRI_USER_SERVICE_TYPE_B] == SERVICE_TYPE_163_169))
        && (m_pcdr[ATTRI_DURATION] < 60)
        && (m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_INTERNATIONAL_IN)
        && (m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_INTERNATIONAL_OUT)//国际漫游来访不做60秒wap检查 
        && (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC))  //163/169前60秒免费仅限于主叫 
    {
        m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
        return ;
    }
  
  
  return 0;
}

int SetOriginFeeFlag::execute()
{
  if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
  {
   //天津智能网话单认为没有原始费用，由BOSS重批
   if((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_MSC) ||
      (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_SCP) ||
      (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_PBX) )
    {
      //无原始费用
    }else
    {
      //省际边界漫游，需要重批，认为没有原始费用
      if (m_pcdr[ATTRI_EDGE_FLAG] > EDGE_ROAM_PORVINNER)
      {
        //无原始费用
      }else
      {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD5,1);
      }
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SMS)
  {
    //短信中心和互联网关话单无原始费用
    //行业应用短信，从梦网上行话单里分出，无原始费用
    if((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_SMSC) ||
      (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_ISMG) ||
      (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_HYYY))
    {
      //无原始费用
    }//梦网网关上行无原始费用，下行有原始费用
    else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_MSMG)
    {
      if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMO)||
          (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMOF) )
      {
        //无原始费用
      }
      else
      {
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
        m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
      }
    }else
    {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
    }
  }else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GPRS)
  {
    if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_ROIG)
    {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
    }
    else
    {
      //无原始费用
    }
  }else
  {
      //本地彩信，并且call_type in(00,01)无原始费用
    if((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_MISC_LMMS) && 
       ((m_pcdr[ATTRI_CALL_TYPE] == "00") || 
        (m_pcdr[ATTRI_CALL_TYPE] == "01"))) 
    {
       //无原始费用
    }
    else
    {
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD1,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD2,1);
      m_pRateInfo->setOrgFeeFlag(ATTRI_DISCFEE_FIELD3,1);
    }
  }
  return 0;
}

