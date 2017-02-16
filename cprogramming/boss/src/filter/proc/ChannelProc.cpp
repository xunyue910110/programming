#include "ChannelProc.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "base/TimeUtil.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(ChannelProc);

ChannelProc::ChannelProc()
{
  m_channelRule = NULL;
}

ChannelProc::~ChannelProc()
{
  m_channelRule = NULL;
}

bool ChannelProc::initialize()
{
  return true;
}

int ChannelProc::reset()
{
  return 0;
}

int ChannelProc::execute()
{
  if(!InitCdrProperty())
  {
    #ifdef _DEBUG_
      cout<<m_FilterInfo->m_fullPathName<<" get channel param fail,biz_type is not defined!"
          <<__FILE__<<__LINE__<<endl;
  	theErrorMessages->insert(E_GET_CHANNEL_PARAM,m_FilterInfo->m_fullPathName + " get channel param fail,biz_type is not defined!");
    #endif
  	m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;
  	return 0;
  }

  getChanByFilterTable();

  //根据JS正则重求通道
  getChanByRule();

    #ifdef OUT_DEBUG_
      cout<<"分拣通道:                   = "<<m_pcdr[ATTRI_CDR_CHANNEL]<<endl;
    #endif

  return 0;
}

bool ChannelProc::InitCdrProperty()
{
	m_channelRule = m_Config->m_channelRuleMgr.getChannelRule(m_pcdr[ATTRI_BIZ_TYPE].c_str());
	if(m_channelRule == NULL)
	{
    #ifdef _DEBUG_
      cout<<"m_Config->m_channelRuleMgr.getChannelRule false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
	}

  if(m_channelRule->m_channelparam.size()<=0)
  {
  	return false;
  }

	//add by shenglj 2009-05-14 :19:39:01
	//有些话单获取不到结束时间，重新计算
  if(m_pcdr[ATTRI_END_DATE].length()<8 || m_pcdr[ATTRI_END_TIME].length()<6)
  {
    char finish_time[20];
    char start_time[20];
    char t_time[20];
    char t_finish_date[10];
    char t_finish_time[10];

    sprintf(start_time,"%s%s",m_cdr[ATTRI_BEGIN_DATE].c_str(),m_cdr[ATTRI_BEGIN_TIME].c_str());
    TimeUtil::addSeconds(t_time,start_time,m_pcdr[ATTRI_DURATION].value());

    strncpy(t_finish_date,t_time,8);
    t_finish_date[8]=0;
    strncpy(t_finish_time,t_time+8,6);
    t_finish_time[6]=0;
    m_cdr[ATTRI_END_DATE] = t_finish_date;
    m_cdr[ATTRI_END_TIME] = t_finish_time;
  }

  return true;
}

bool ChannelProc::getChanByFilterTable()
{
  if(m_pcdr[ATTRI_ERROR_CODE]!=DEFAULT_ERROR)
  {
  	 m_pcdr[ATTRI_ERROR_FLAG]=ATTRI_ERROR_Y;
  }

  char t_outStr[120+1];
  int i=0;
	DetailChannelItem r_record;

	//设置通道因素
  for(vector<ChannelRule::ChannelParam>::iterator r_Itr=m_channelRule->m_channelparam.begin();
			r_Itr!=m_channelRule->m_channelparam.end();r_Itr++)
  {
 	   r_record.m_factors[i] = m_pcdr[r_Itr->m_id];
 	   i++;
    #ifdef OUT_DEBUG_
      sprintf(t_outStr,"%03d %-30s = %s",i,r_Itr->m_name.c_str(),m_pcdr[r_Itr->m_id].c_str());
      cout<<t_outStr<<endl;
    #endif

  } 
	r_record.m_beginTime = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
	r_record.m_endTime   = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];

  if(m_globalManager->getDetailChannelItem(r_record,m_channelRule->m_channelparam.size(),m_channel))
 	{
 	  if(strncasecmp(m_channel.c_str(),CHANNEL_BACKUP.c_str(),CHANNEL_BACKUP.length())==0)
 	  {
 	  	m_pcdr[ATTRI_BACK_CHANNEL] = m_channel;
 	  }
 	  else if(strncasecmp(m_channel.c_str(),CHANNEL_MUX.c_str(),CHANNEL_MUX.length())==0)
 	  {
 	  	m_pcdr[ATTRI_MUX_CHANNEL] = m_channel;
 	  }
 	  else
 	  {
      m_pcdr[ATTRI_CDR_CHANNEL] = m_channel;
 	  }
 	}
 	else
 	{
 		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;
 	}

  return true;
}
