#include "Mdb_AdcECAbnormalState.h"

bool Mdb_AdcECAbnormalState::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_AdcECAbnormalState::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];
	
session->getColumnValue("BIZ_STATE_CODE"          ,&t_strTmp   ,t_length); t_strTmp[64]=0;m_record.m_userState  = t_strTmp[0];
 	session->getColumnValue("EA_USER_ID"               ,&t_longValue,t_length); m_record.m_ecUserId  = t_longValue;
 	session->getColumnValue("BIZ_CODE"          ,&m_record.m_bizCode ,t_length);
 	return true;
}

void Mdb_AdcECAbnormalState::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_userId = m_record.m_ecUserId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);	
}

void Mdb_AdcECAbnormalState::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_AdcECAbnormalState::Mdb_AdcECAbnormalState(const Mdb_AdcECAbnormalState& right)
{
	*this=right;
}

void Mdb_AdcECAbnormalState::operator=(const Mdb_AdcECAbnormalState& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_AdcECAbnormalState &po)
{
	os << po.m_record.m_ecUserId      << " , "
	   << po.m_record.m_userState     << " , "
	   << po.m_record.m_bizCode;
	return os;
}

Mdb_Property Mdb_AdcECAbnormalState::m_property={"TF_F_USER_GRPMBMP_SUB","idx_TF_F_USER_GRPMBMP_SUB",1,"",0};
