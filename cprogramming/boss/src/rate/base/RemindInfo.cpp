#include "RemindInfo.h"

ostream& operator<<(ostream& os,const RemindInfo& po)
{
	os<< po.m_userId         << ','     
  	  << po.m_msisdn         << ','     
      << po.m_startDateTime  << ',' 
      << po.m_endDateTime    << ',' 
      << po.m_duration       << ',' 
      << po.m_billType       << ',' 
      << po.m_cycleId        << ',' 
      << po.m_cycleBeginTime << ',' 
      << po.m_cycleEndTime   << ',' 
      << po.m_feePolicyId    << ',' 
      << po.m_tpObjId        << ',' 
      << po.m_remindLevel    << ',' 
      << po.m_remindValue    << ',' 
      << po.m_maxValue       << ',' 
      << po.m_curValue;
    return os;
}

void RemindInfo::getOuputStr(char *r_str)
{
	sprintf(r_str,"%s,%s,%s,%s,%ld,%d,%d,%s,%s,%ld,%ld,%d,%ld,%ld,%ld\n",
				m_userId.c_str(),
				m_msisdn.c_str(),
				m_startDateTime.c_str(),
				m_endDateTime.c_str(),
				m_duration,
				m_billType,
				m_cycleId,
				m_cycleBeginTime.c_str(),
				m_cycleEndTime.c_str(),
				(long)m_feePolicyId,
				m_tpObjId,
				m_remindLevel,
				m_remindValue,
				m_maxValue,
				m_curValue);
}
