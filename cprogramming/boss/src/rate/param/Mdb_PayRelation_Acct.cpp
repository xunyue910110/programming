#include "Mdb_PayRelation_Acct.h"

bool Mdb_PayRelation_Acct::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_PayRelation_Acct::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
 	session->getColumnValue("USER_ID" ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("ACCT_ID" ,&t_longValue ,t_length); m_record.m_acctId = t_longValue;
 	session->getColumnValue("DISCNT_PRIORITY" ,&m_record.m_discntPriority ,t_length); 
 	session->getColumnValue("ACT_TAG"         ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_actTag  = t_strTmp[0];
 	session->getColumnValue("START_CYC_ID"    ,&m_record.m_startCycId ,t_length); 
 	session->getColumnValue("END_CYC_ID"      ,&m_record.m_endCycId ,t_length);
  
 	return true;
}

void Mdb_PayRelation_Acct::setKeyParams(InputParams *pInputParamsseIndex) const
{
    long t_acctId = m_record.m_acctId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_acctId);
}

void Mdb_PayRelation_Acct::setOtherParams(InputParams *pInputParamsseOther) const
{
    pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));	
    pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));
}

Mdb_PayRelation_Acct::Mdb_PayRelation_Acct(const Mdb_PayRelation_Acct& right)
{
	*this=right;
}

void Mdb_PayRelation_Acct::operator=(const Mdb_PayRelation_Acct& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_PayRelation_Acct &po)
{
	os << po.m_record.m_userId<<" , "
	   << po.m_record.m_acctId<<" , "
	   << po.m_record.m_discntPriority<<" , "
	   << po.m_record.m_actTag<<" , "
	   << po.m_record.m_startCycId<<" , "
	   << po.m_record.m_endCycId;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_PayRelation_Acct::m_property={"TF_F_PayRelation","IDX_TF_F_PayRelation_ACCT",1,
    "(START_CYC_ID<=:0) and (END_CYC_ID>=:1)",2};
