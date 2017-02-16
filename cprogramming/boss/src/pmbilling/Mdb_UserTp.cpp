#include "Mdb_UserTp.h"

bool Mdb_UserTp::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserTp::convert(Session* session)
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
 	session->getColumnValue("PRODUCT_ID"       ,&m_record.m_productId ,t_length); 
 	session->getColumnValue("SERV_BUND_ID"     ,&m_record.m_servBund_Id  ,t_length); 
 	session->getColumnValue("SERV_ID"          ,&m_record.m_servId ,t_length);
 	session->getColumnValue("FEEPOLICY_BUND_ID",&m_record.m_feeBund_Id ,t_length);
 	session->getColumnValue("START_DATE"       ,&t_longValue,t_length); m_record.m_beginTime     = t_longValue;
 	session->getColumnValue("END_DATE"         ,&t_longValue,t_length); m_record.m_endTime       = t_longValue;
 	return true;
}

void Mdb_UserTp::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	
	char t_strTmp[20+1];
	t_strTmp[0]=m_record.m_idType;t_strTmp[1]=0;
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)(t_strTmp));	
}

void Mdb_UserTp::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_UserTp::Mdb_UserTp(const Mdb_UserTp& right)
{
	*this=right;
}

void Mdb_UserTp::operator=(const Mdb_UserTp& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserTp &po)
{
	os << po.m_record.m_tpObjId      << " , "
	   << po.m_record.m_tpId         << " , "
	   << po.m_record.m_beginTime    << " , "
	   << po.m_record.m_endTime      << " , "
	   << po.m_record.m_idType       << " , "
	   << po.m_record.m_userId       << " , "
	   << po.m_record.m_productId    << " , "
	   << po.m_record.m_servBund_Id  << " , "
	   << po.m_record.m_servId       << " , "
	   << po.m_record.m_feeBund_Id;
	return os;
}

Mdb_Property Mdb_UserTp::m_property={"TF_F_FEEPOLICY","idx_TF_F_FEEPOLICY_ID",2,"",0};
