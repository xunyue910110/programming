#include "SpaceInfo.h"

///<首次创建时初始化
void MDbGlobalInfo::init()
{
  memset(this,0,sizeof(MDbGlobalInfo));
  updateDbTime();
  updateSpTime();
  updateTbTime();
  updateIdxTime();
  m_nextSpCode=2;
  m_curSessionId=0;
}
void MDbGlobalInfo::updateDbTime()
{
  time(&m_dbTime);
}
void MDbGlobalInfo::updateSpTime()
{
  time(&m_spUpTime);
}
void MDbGlobalInfo::updateTbTime()
{
  time(&m_tbUpTime);
}
void MDbGlobalInfo::updateIdxTime()
{
  time(&m_idxUpTime);
}
unsigned int MDbGlobalInfo::getNextSpCode()
{
  m_nextSpCode++;
  return m_nextSpCode-1;
}
void MDbGlobalInfo::setReserve(const size_t &r_offSet)
{
  m_reserve = r_offSet;
}
int MDbGlobalInfo::getSessionId()
{
  int t_sId=-1;
  for(t_sId=m_curSessionId;t_sId<MAX_SESSION_NUM;t_sId++)
  {
    if(m_sidState[t_sId]==false)
    {
      m_sidState[t_sId]= true;
      m_curSessionId=t_sId;
      return t_sId;
    }
  }
  for(t_sId=0;t_sId<m_curSessionId;t_sId++)
  {
    if(m_sidState[t_sId]==false)
    {
      m_sidState[t_sId]= true;
      m_curSessionId=t_sId;
      return t_sId;
    }
  }
  t_sId = -1;
  return t_sId;
}
//释放SessionId
void MDbGlobalInfo::releadSid(const int &r_sid)
{
  if(r_sid>=0&&r_sid<MAX_SESSION_NUM)
  {
    m_sidState[r_sid]= false;
  }
  return;
}

void SpaceInfo::dumpInfo(ostream &r_os)
{
  r_os<<"m_dbName   ="<<m_dbName<<endl;
  r_os<<"m_fileName ="<<m_fileName<<endl;
  r_os<<"m_size     ="<<m_size<<endl;
  r_os<<"m_pageSize ="<<m_pageSize<<endl;
  r_os<<"m_type     ="<<m_type<<endl;
  r_os<<"m_spaceCode="<<m_spaceCode<<endl;
  r_os<<"m_shmKey   ="<<m_shmKey<<endl;
}
void GlobalParam::dumpInfo(ostream &r_os)
{
  r_os<<"m_paramName="<<m_paramName<<endl;
  r_os<<"m_value    ="<<m_value<<endl;
}
void SessionInfo::dumpInfo(ostream &r_os)
{
  r_os<<"m_pId      ="<<m_pId<<endl;
  r_os<<"m_sessionId="<<m_sessionId<<endl;
  r_os<<"m_time     ="<<m_time<<endl;
  r_os<<"m_ipAddr   ="<<m_ipAddr<<endl;
}

