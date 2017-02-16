#include "Mdb_UserCondTp_Param.h"

bool Mdb_UserCondTp_Param::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserCondTp_Param::convert(Session* session)
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

void Mdb_UserCondTp_Param::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_longValue;
  t_longValue = long(m_record.m_tpObjId);
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);
}

void Mdb_UserCondTp_Param::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_UserCondTp_Param::Mdb_UserCondTp_Param(const Mdb_UserCondTp_Param& right)
{
	*this=right;
}

void Mdb_UserCondTp_Param::operator=(const Mdb_UserCondTp_Param& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserCondTp_Param &po)
{
	os << po.m_record.m_tpObjId    << " , "
	   << po.m_record.m_tpParamId  << " , "
	   << po.m_record.m_tpValue    << " , "
	   << po.m_record.m_beginTime  << " , "
	   << po.m_record.m_endTime;
	return os;
}

Mdb_Property Mdb_UserCondTp_Param::m_property={"TF_B_COND_FEEPOLICY_PARAM",
                              "IDX_TF_B_COND_FEEPOLICY_PARAM",1,"",0};
