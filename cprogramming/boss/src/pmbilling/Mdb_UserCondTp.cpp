#include "Mdb_UserCondTp.h"

bool Mdb_UserCondTp::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserCondTp::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];
	
 	session->getColumnValue("FEEPOLICY_INS_ID" ,&t_longValue,t_length); m_record.m_tpObjId = t_longValue;
 	session->getColumnValue("ID_TYPE"          ,&t_strTmp   ,t_length); 
 	                      t_strTmp[64]=0;m_record.m_idType  = t_strTmp[0];
 	session->getColumnValue("ID"               ,&t_longValue,t_length); m_record.m_userId  = t_longValue;
 	session->getColumnValue("FEEPOLICY_ID"     ,&t_intValue ,t_length); m_record.m_tpId    = t_intValue;
 	session->getColumnValue("RELA_USER_ID"     ,&t_longValue,t_length); m_record.m_relatin_vpnId = t_longValue;
 	session->getColumnValue("START_DATE"       ,&t_longValue,t_length); m_record.m_beginTime     = t_longValue;
 	session->getColumnValue("END_DATE"         ,&t_longValue,t_length); m_record.m_endTime       = t_longValue;
 	return true;
}

void Mdb_UserCondTp::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	
	char t_strTmp[20+1];
	t_strTmp[0]=m_record.m_idType;t_strTmp[1]=0;
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)(t_strTmp));	
}

void Mdb_UserCondTp::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_UserCondTp::Mdb_UserCondTp(const Mdb_UserCondTp& right)
{
	*this=right;
}

void Mdb_UserCondTp::operator=(const Mdb_UserCondTp& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserCondTp &po)
{
	os << po.m_record.m_tpObjId      << " , "
	   << po.m_record.m_tpId         << " , "
	   << po.m_record.m_beginTime    << " , "
	   << po.m_record.m_endTime      << " , "
	   << po.m_record.m_idType       << " , "
	   << po.m_record.m_userId       << " , "
	   << po.m_record.m_relatin_vpnId;
	return os;
}

Mdb_Property Mdb_UserCondTp::m_property={"TF_B_COND_FEEPOLICY","IDX_TF_B_COND_FEEPOLICY_ID",2,"",0};
