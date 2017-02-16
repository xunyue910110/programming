#include "ProvMonAuditUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"
#include "base/TimeUtil.h"

INSTALL_PROCESSBASE_INSTANCE(ProvMonAuditUNProc);

bool ProvMonAuditUNProc::auditFee()
{
	int rd;
	int infofee;
  rd = m_globalManager->getInfoFeeBySpOper(m_pcdr[ATTRI_SP_CODE],
				    m_pcdr[ATTRI_OPER_CODE],
				    m_pcdr[ATTRI_SERVICE_CODE],
				    m_pcdr[ATTRI_CHARGE_TYPE],
				    m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME],
				    infofee);

    if(rd==-2) //费用类型不匹配（江苏联通不存在这种错误）
    {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_CHARGE_TYPE;
        m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
        return false;
    }
    else if(rd==-1) //SP测试话单
    {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPTEST;
        m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_SPTEST;
        return false;
    }
    else if(rd==1)
    {
    	if(m_pcdr[ATTRI_ORGFEE_FIELD1].value()+m_pcdr[ATTRI_ORGFEE_FIELD2].value()+m_pcdr[ATTRI_ORGFEE_FIELD3].value()>infofee)
    	{
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_ALLFEE;
        m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
        return false;
    	}
    }
  return true;
}

bool ProvMonAuditUNProc::preauditStatus()
{
  int t_ret;
  char    finish_time[20];
  char    start_time[20];
  char    current_time[20];
  char    t_time[20];
  int     t_error;

  sprintf(finish_time,"%s%s",m_pcdr[ATTRI_END_DATE].c_str(),m_pcdr[ATTRI_END_TIME].c_str());
  sprintf(start_time,"%s%s",m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
  TimeUtil::now(current_time);
  current_time[14]=0;

  //根据配置，用户状态稽核选择按话单时间（finish_time）、当前时间、或者话单时间加一定的延时 2006-5-24 19:16
  //增加当前时间加一定的延时 2006-7-4 15:00
  switch(m_Config->m_statusAuditMethod)
  {
      case FINISH_TIME:
          sprintf(t_time,"%s",finish_time);
          t_time[14]=0;
          break;
      case CURRENT_TIME:
          sprintf(t_time,"%s",current_time);
          t_time[14]=0;
          break;
      case DELAY_TIME:
          TimeUtil::addSeconds(t_time,finish_time,m_Config->m_delayTime);
          t_time[14]=0;
          break;
      case DELAY_CURRENT_TIME:
          TimeUtil::addSeconds(t_time,current_time,m_Config->m_delayTime);
          t_time[14]=0;
          break;
      case START_TIME:
          sprintf(t_time,"%s",start_time);
          t_time[14]=0;
          break;
      default:
          sprintf(t_time,"%s",finish_time);
          t_time[14]=0;
          break;
  }

  t_ret = m_globalManager->getUserStateByUserId(m_pcdr[ATTRI_USER_ID].c_str(),t_time,
             m_userstate,m_stategroup,r_lastmonthstop,t_error);
  if(t_error>0)
  {
  	m_pcdr[ATTRI_ERROR_CODE] = t_error;
  	return false;
  }
  if(t_ret>0)
  {
  	m_pcdr[ATTRI_USER_STATE] = m_userstate;
  	m_pcdr[ATTRI_USER_STATE_GROUP] = m_stategroup;
  }

  return true;
}

bool ProvMonAuditUNProc::auditSmStatus()
{
  //包月话单稽核流程
  //对于停机用户，如果不是本月停机（上月或更早前停机即从上月或更早前到现在为止一直停机），信息费包月话单作错单处理
  if(m_pcdr[ATTRI_CHARGE_TYPE] == CHARGE_TYPE_03)
  {
    if(m_stategroup == USER_UNDEFINED) //用户状态找不到
    {
      m_pcdr[ATTRI_ERROR_CODE]=ERROR_NOSTATUS;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
    else if(m_stategroup == USER_DESTROY)   //欠费预销号 欠费销号
    {
      m_pcdr[ATTRI_ERROR_CODE] =ERROR_DESTROYSTATUS;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
    else if(m_stategroup == USER_STOP && r_lastmonthstop == "1") //用户上月停机
    {
      m_pcdr[ATTRI_ERROR_CODE]=ERROR_STOPSTATUS_C;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
  }
  //按条话单稽核流程
  //停机用户的发送话单及下行按条有费用(老系统批价)的话单作正常话单处理
  else
 {
    if(m_stategroup == USER_UNDEFINED) //用户状态找不到
    {
      m_pcdr[ATTRI_ERROR_CODE]=ERROR_NOSTATUS;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
    else if(m_stategroup == USER_DESTROY)   //欠费预销号 欠费销号
    {
      m_pcdr[ATTRI_ERROR_CODE] =ERROR_DESTROYSTATUS;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
    else if(m_stategroup == USER_STOP && m_pcdr[ATTRI_CFEE_TYPE]!=CHARGE_TYPE_02)//用户停机并且基本费类别为02
    {
      m_pcdr[ATTRI_ERROR_CODE]=ERROR_STOPSTATUS;
      m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
      return false;
    }
 }

  return true;
}

bool ProvMonAuditUNProc::auditSpStatus()
{
  if(m_stategroup == USER_UNDEFINED) //用户状态找不到
  {
    m_pcdr[ATTRI_ERROR_CODE]=ERROR_NOSTATUS;
    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
    return false;
  }
  else if(m_stategroup == USER_DESTROY)   //欠费预销号 欠费销号
  {
    m_pcdr[ATTRI_ERROR_CODE] =ERROR_DESTROYSTATUS;
    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
    return false;
  }
  else if(m_stategroup == USER_STOP) //用户停机
  {
		//彩信发送成功 正常返回
		if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS && m_pcdr[ATTRI_SEND_STATUS] == "0")
		{
      return true;
		}
		else if(r_lastmonthstop == "1") //用户上月停机
		{
	    m_pcdr[ATTRI_ERROR_CODE]=ERROR_STOPSTATUS;
	    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
	    return false;
		}
  }

  return true;
}

bool ProvMonAuditUNProc::auditppsSpStatus()
{
  if(m_stategroup == USER_UNDEFINED) //用户状态找不到
  {
    m_pcdr[ATTRI_ERROR_CODE]=ERROR_NOSTATUS;
    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
    return false;
  }
  else if(m_stategroup == USER_DESTROY)   //欠费预销号 欠费销号
  {
    m_pcdr[ATTRI_ERROR_CODE] =ERROR_DESTROYSTATUS;
    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
    return false;
  }

  return true;
}

bool ProvMonAuditUNProc::auditsp()
{
  return true;
}

bool ProvMonAuditUNProc::smsaudit()
{
  isVerifyState = 'Y'; //默认稽核用户状态
  isVerifyOrder = 'Y'; //默认稽核订购关系

  m_globalManager->getSpOperVerify(m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_OPER_CODE],
         m_pcdr[ATTRI_SERVICE_CODE],m_pcdr[ATTRI_CALL_TYPE],
         m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME],
         isVerifyState,isVerifyOrder);
	//1. 费用稽核
  if(m_Config->m_isVerifyFee == 'Y')
  {
    //如果费用稽核为错单，不再做其他稽核
	  if(!auditFee())
    return false;
  }

	//2. 用户状态稽核
	if(isVerifyState == 'Y' &&
		(m_pcdr[ATTRI_CHARGE_TYPE] == CHARGE_TYPE_02 ||
		 m_pcdr[ATTRI_CHARGE_TYPE] == CHARGE_TYPE_03))
	{
	  if(!preauditStatus())
      return false;

    if(!auditSmStatus())
      return false;
	}
	else if(isVerifyState == 'U')
  {
    m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPOPER;
    m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
    return false;
  }

	//3. 订购关系稽核 暂不需要
	if(isVerifyOrder == 'Y')
	{
	}

  return true;
}

bool ProvMonAuditUNProc::spaudit()
{
  if(!preauditStatus())
    return false;

  if(!auditSpStatus())
    return false;

  return true;
}

bool ProvMonAuditUNProc::ppsspaudit()
{
  if(!preauditStatus())
    return false;

  if(!auditppsSpStatus())
    return false;

  return true;
}

bool ProvMonAuditUNProc::monthfeeaudit()
{
  if(!preauditStatus())
    return false;

  return true;
}

int ProvMonAuditUNProc::execute()
{
  if(m_pcdr[ATTRI_CDR_CHANNEL]==CHANNEL_ERROR ||
  	 m_pcdr[ATTRI_CDR_CHANNEL]==CHANNEL_TRASH ||
  	 m_pcdr[ATTRI_CDR_CHANNEL]==CHANNEL_NOINFO)
  {
  	return 0;
  }

  //短信稽核
  if ((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PSMG ||
       m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_NSMG) &&
       (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT ||
        m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMTF||
        m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMC ||
        m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMCF))
  {
     if(!smsaudit())
     	return 0;
  }

  //SP稽核
  if (m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_UNICOMSP)
  {
     if(!spaudit())
     	return 0;
  }

//智能网销户用户不用判用户状态，销户用户时间段空，走无主流程，不需走状态稽核
/*
  //统计类稽核
  if (m_pcdr[ATTRI_SOURCE_TYPE]>=SOURCE_TYPE_SP_PPS)
  {
     if(!auditppsSpStatus())
     	return 0;
  }
*/

  //包月费稽核
  if (m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_SPMONTH)
  {
    if(!monthfeeaudit())
    {
    	m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
     	return 0;
    }
  }

  return 0;
}
