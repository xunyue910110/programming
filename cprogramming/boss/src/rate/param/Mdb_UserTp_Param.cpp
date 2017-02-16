#include "Mdb_UserTp_Param.h"

bool Mdb_UserTp_Param::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserTp_Param::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[128+1];
	
 	session->getColumnValue("FEEPOLICY_INS_ID"     ,&t_longValue,t_length);m_record.m_tpObjId=t_longValue;
 	session->getColumnValue("FEEPOLICY_PARAM_ID"   ,&t_intValue,t_length); m_record.m_tpParamId=t_intValue;
 	session->getColumnValue("FEEPOLICY_PARAM_VALUE",&t_strValue,t_length); 
 	           t_strValue[t_length]=0;m_record.m_tpValue=t_strValue;
 	session->getColumnValue("START_DATE"           ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;
 	session->getColumnValue("END_DATE"             ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_UserTp_Param::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_longValue;
  t_longValue = long(m_record.m_tpObjId);
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);
}

void Mdb_UserTp_Param::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_UserTp_Param::Mdb_UserTp_Param(const Mdb_UserTp_Param& right)
{
	*this=right;
}

void Mdb_UserTp_Param::operator=(const Mdb_UserTp_Param& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserTp_Param &po)
{
	os << po.m_record.m_tpObjId    << " , "
	   << po.m_record.m_tpParamId  << " , "
	   << po.m_record.m_tpValue    << " , "
	   << po.m_record.m_beginTime  << " , "
	   << po.m_record.m_endTime;
	return os;
}

Mdb_Property Mdb_UserTp_Param::m_property={"TF_F_FEEPOLICY_PARAM",
                              "IDX_TF_F_FEEPOLICY_PARAM_TPOBJID",1,"",0};
