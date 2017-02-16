#include "Mdb_SpCdrInfo.h"


bool Mdb_SpCdrInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_SpCdrInfo::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];
 	 	session->getColumnValue("SERIAL_NUMBER"          ,&m_record.m_serialNumber ,t_length);
 	 	session->getColumnValue("SP_CODE"                ,&m_record.m_spCode ,t_length);
 	 	session->getColumnValue("SP_SVC_ID"              ,&m_record.m_spBizCode ,t_length);
    session->getColumnValue("CYC_ID"                 ,&t_longValue,t_length); m_record.m_cycId  = t_longValue;
    session->getColumnValue("CDR_INFO"              ,&t_longValue,t_length); m_record.m_cdrInfo  = t_longValue;

 	return true;
}

void Mdb_SpCdrInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_serialNumber = m_record.m_serialNumber.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_serialNumber);	
}

void Mdb_SpCdrInfo::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_SpCdrInfo::Mdb_SpCdrInfo(const Mdb_SpCdrInfo& right)
{
	*this=right;
}

void Mdb_SpCdrInfo::operator=(const Mdb_SpCdrInfo& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_SpCdrInfo &po)
{
   os <<po.m_record.m_serialNumber<<","
      <<po.m_record.m_spCode        <<","
      <<po.m_record.m_spBizCode     <<","
      <<po.m_record.m_cycId         <<","
      <<po.m_record.m_cdrInfo ;
	return os;
}

Mdb_Property Mdb_SpCdrInfo::m_property={"TI_A_SP_TO_AUDIT_DETAIL","idx_TI_A_SP_TO_AUDIT_DETAIL",1,"",0};
