#include "FilterGlobalInfo.h"
#include "base/TimeUtil.h"

bool FilterGlobalInfo::initialization(FilterConfig *r_pConfig,
                       SortLog *r_sortLog,
                       PMGlobalManager  *r_globalManager)
{
  m_Config        = r_pConfig;
  m_sortLog       = r_sortLog;
  m_globalManager = r_globalManager;
  m_pCdr          = &m_cdr;

  return true;
}

void FilterGlobalInfo::setFileNameChannelName(const char *r_fileName,
	   const char *r_fullPathName,
	   const char *r_subChannelName,
	   const int &r_bindmethod)
{
  m_fileName       = r_fileName;
  m_fullPathName   = r_fullPathName;
  m_subChannelName = r_subChannelName;
  m_bindMethod     = r_bindmethod;
  m_ferrfullPathName  = m_Config->m_errorPath + m_fileName;
}

void FilterGlobalInfo::setFileOutChanInfo(vector<OutChanInfo> *r_outChanInfo)
{
  m_outChanInfo = r_outChanInfo;
}

void FilterGlobalInfo::reset()
{
  m_cdr.reset();
  m_unique_flag = FLAG_YES;
  m_smsMethod = 0;
}

void FilterGlobalInfo::backupcdr()
{
  memcpy(&m_originalcdr,&m_cdr,sizeof(m_cdr));
}

void FilterGlobalInfo::canonicalize()
{
  m_cdr.canonicalize();
  addDealingNum();
  //调整开始时间
  for(vector<string>::iterator itr=m_Config->m_resetTimeSourceVector.begin();
                               itr!=m_Config->m_resetTimeSourceVector.end();++itr)
  {
    if(m_cdr[ATTRI_SOURCE_TYPE] == *itr)
     	 adjustCdrBeginTime();
  }
}

void FilterGlobalInfo::finalize()
{
  m_cdr.finalize();
}

void FilterGlobalInfo::addDealingNum()
{
  m_totalCdrNum++;
}

void FilterGlobalInfo::clearDealingNum()
{
  m_totalCdrNum = 0;
}

void FilterGlobalInfo::adjustCdrBeginTime()
{
  int t_ret;
  char    finish_time[20];
  char    start_time[20];
  char    current_time[20];
  char    t_time[20];
  char    t_begin_date[10];
  char    t_begin_time[10];
  int t_resetDelayTime = 0;

	if(m_Config->m_resetDelayTimeMap.count(m_cdr[ATTRI_SOURCE_TYPE].c_str()))
	{
		 t_resetDelayTime = (m_Config->m_resetDelayTimeMap)[m_cdr[ATTRI_SOURCE_TYPE].c_str()];
	}
  sprintf(finish_time,"%s%s",m_cdr[ATTRI_END_DATE].c_str(),m_cdr[ATTRI_END_TIME].c_str());
  sprintf(start_time,"%s%s",m_cdr[ATTRI_BEGIN_DATE].c_str(),m_cdr[ATTRI_BEGIN_TIME].c_str());
  TimeUtil::now(current_time);
  current_time[14]=0;

  //根据配置，用户状态稽核选择按话单时间（finish_time）、当前时间、或者话单时间加一定的延时 2006-5-24 19:16
  //增加当前时间加一定的延时 2006-7-4 15:00
  switch(m_Config->m_resetTimeMethod)
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
          TimeUtil::addSeconds(t_time,start_time,t_resetDelayTime);
          t_time[14]=0;
          break;
      case DELAY_CURRENT_TIME:
          TimeUtil::addSeconds(t_time,current_time,t_resetDelayTime);
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

  strncpy(t_begin_date,t_time,8);
  t_begin_date[8]=0;
  strncpy(t_begin_time,t_time+8,6);
  t_begin_time[6]=0;
  m_cdr[ATTRI_BEGIN_DATE] = t_begin_date;
  m_cdr[ATTRI_BEGIN_TIME] = t_begin_time;
}

