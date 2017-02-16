#include "Mdb_PayRelation_All.h"

bool Mdb_PayRelation_All::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_PayRelation_All::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
 	session->getColumnValue("USER_ID" ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("ACCT_ID" ,&t_longValue ,t_length); m_record.m_acctId = t_longValue;
  
 	return true;
}

void Mdb_PayRelation_All::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_PayRelation_All::setOtherParams(InputParams *pInputParamsseOther) const
{
  
}

Mdb_PayRelation_All::Mdb_PayRelation_All(const Mdb_PayRelation_All& right)
{
	*this=right;
}

void Mdb_PayRelation_All::operator=(const Mdb_PayRelation_All& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_PayRelation_All &po)
{
	os << po.m_record.m_userId<<" , "
	   << po.m_record.m_acctId;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_PayRelation_All::m_property={"TF_F_PayRelation","IDX_TF_F_PayRelation",1,
    "",0};
