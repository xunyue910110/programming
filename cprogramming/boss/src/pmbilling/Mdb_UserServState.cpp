#include "Mdb_UserServState.h"

bool Mdb_UserServState::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserServState::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[10+1];
	
 	session->getColumnValue("USER_ID"        ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("SERV_ID"        ,&m_record.m_serviceId ,t_length);
 	session->getColumnValue("MAIN_TAG"       , t_strTmp   ,t_length); 
 	                        t_strTmp[10]=0;m_record.m_mainTag= t_strTmp[0];
 	session->getColumnValue("SERV_STATE_CODE", t_strTmp   ,t_length); 
 	                        t_strTmp[10]=0;m_record.m_servStateCode= t_strTmp[0];
 	session->getColumnValue("START_DATE"     ,&t_longValue,t_length); m_record.m_startDate  = t_longValue;
 	session->getColumnValue("END_DATE"       ,&t_longValue,t_length); m_record.m_endDate    = t_longValue;
 	return true;
}

void Mdb_UserServState::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_UserServState::setOtherParams(InputParams *pInputParamsseOther) const
{
}

Mdb_UserServState::Mdb_UserServState(const Mdb_UserServState& right)
{
	*this=right;
}

void Mdb_UserServState::operator=(const Mdb_UserServState& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserServState &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_serviceId     << " , "
	   << po.m_record.m_mainTag       << " , "
	   << po.m_record.m_servStateCode << " , "
	   << po.m_record.m_startDate     << " , "
	   << po.m_record.m_endDate;
	return os;
}

Mdb_Property Mdb_UserServState::m_property={"TF_F_USER_SERVSTATE","IDX_TF_F_USER_SERVSTATE",1,"",0};
