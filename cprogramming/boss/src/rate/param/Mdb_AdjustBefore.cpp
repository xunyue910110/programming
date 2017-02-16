#include "Mdb_AdjustBefore.h"

bool Mdb_AdjustBefore::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_AdjustBefore::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
 	session->getColumnValue("adjust_ins_id"    ,&m_record.m_adjustInsId ,t_length); 
 	session->getColumnValue("id"               ,&t_longValue ,t_length);m_record.m_id=t_longValue;
 	session->getColumnValue("id_type"          ,t_strTmp     ,t_length); m_record.m_idType   = t_strTmp[0];
 	session->getColumnValue("EFFECT_OBJECT_ID" ,&m_record.m_effectObjectId  ,t_length);
 	session->getColumnValue("ACT_MODE"         ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_actMode  = t_strTmp[0];
 	session->getColumnValue("EFFECT_MODE"      ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_effectMode = t_strTmp[0];
 	session->getColumnValue("ADDUP_MODE"       ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_addupMode  = t_strTmp[0];
 	session->getColumnValue("ADDUP_OBJECT_ID"  ,&m_record.m_addupObjectId  ,t_length); 
 	session->getColumnValue("EFFECT_VALUE_TYPE",t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_effectValueType  = t_strTmp[0];
 	session->getColumnValue("EFFECT_VALUE"     ,&m_record.m_effectValue ,t_length); 
 	session->getColumnValue("REFER_OBJECT_ID"  ,&m_record.m_referObjId ,t_length);
 	session->getColumnValue("REFER_FEE"        ,&m_record.m_referFee ,t_length);    
 	session->getColumnValue("START_CYC_ID"     ,&m_record.m_startCycId  ,t_length);
 	session->getColumnValue("END_CYC_ID"       ,&m_record.m_endCycId  ,t_length);
 	session->getColumnValue("LIMIT_FEE"        ,&m_record.m_limitFee ,t_length); 
 	session->getColumnValue("RECV_TAG"         ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_recvTag  = t_strTmp[0];
 	session->getColumnValue("ADJUST_REASON_ID" ,&m_record.m_adjustReasonId  ,t_length);
 	return true;
}

void Mdb_AdjustBefore::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_id = m_record.m_id.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_id);
}

void Mdb_AdjustBefore::setOtherParams(InputParams *pInputParamsseOther) const
{
  char t_strTmp[20+1];
  t_strTmp[0]=m_record.m_idType;t_strTmp[1]=0;
  pInputParamsseOther->setValue(VAR_TYPE_VSTR,(void *)(t_strTmp));	
  pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));	
  pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));	
}

Mdb_AdjustBefore::Mdb_AdjustBefore(const Mdb_AdjustBefore& right)
{
	*this=right;
}

void Mdb_AdjustBefore::operator=(const Mdb_AdjustBefore& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_AdjustBefore &po)
{
	os << po.m_record.m_adjustInsId        << " , "
	   << po.m_record.m_id     << " , "
	   << po.m_record.m_idType       << " , "
	   << po.m_record.m_effectObjectId << " , "
	   << po.m_record.m_actMode     << " , "
	   << po.m_record.m_effectMode   << " , "
	   << po.m_record.m_addupMode   << " , "
	   << po.m_record.m_addupObjectId   << " , "
	   << po.m_record.m_effectValueType   << " , "
	   << po.m_record.m_effectValue   << " , "
	   << po.m_record.m_referFee   << " , "
	   << po.m_record.m_startCycId   << " , "
	   << po.m_record.m_endCycId   << " , "
	   << po.m_record.m_limitFee   << " , "
	   << po.m_record.m_recvTag   << " , "
	   << po.m_record.m_adjustReasonId;
	return os;
}

Mdb_Property Mdb_AdjustBefore::m_property={"TS_B_ADJUSTBEFORE","IDX_TS_B_ADJUSTBEFORE",1,
    "((id_type=:0) and (START_CYC_ID<=:1)) and (END_CYC_ID>=:2)",3};
