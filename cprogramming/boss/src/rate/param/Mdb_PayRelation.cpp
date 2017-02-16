#include "Mdb_PayRelation.h"

bool Mdb_PayRelation::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_PayRelation::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
	session->getColumnValue("PAYRELATION_ID"  ,&t_longValue ,t_length); m_record.m_payRelationId = t_longValue;
 	session->getColumnValue("USER_ID"         ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("ITEM_CODE"       ,&m_record.m_itemCode ,t_length);
 	session->getColumnValue("ACCT_ID"         ,&t_longValue ,t_length); m_record.m_acctId = t_longValue;
 	//session->getColumnValue("LIMIT_ID"        ,&m_record.m_limitId  ,t_length);
 	session->getColumnValue("BIND_TYPE"       ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0; m_record.m_bindType  = t_strTmp[0];
 	session->getColumnValue("ADDUP_METHOD"       ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0; m_record.m_addupMethod  = t_strTmp[0];
 	session->getColumnValue("ADDUP_MONTHS"         ,&t_longValue ,t_length); m_record.m_addupMonths = t_longValue;
 	session->getColumnValue("LIMIT_TYPE"       ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0; m_record.m_limitType  = t_strTmp[0];
 	session->getColumnValue("LIMIT_VALUE"         ,&t_longValue ,t_length); m_record.m_limitValue = t_longValue;
 	session->getColumnValue("PAY_PRIORITY"    ,&m_record.m_payPriority ,t_length);
 	session->getColumnValue("DISCNT_PRIORITY" ,&m_record.m_discntPriority ,t_length); 
 	session->getColumnValue("DEFAULT_TAG"     ,t_strTmp  ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_defaultTag = t_strTmp[0];
 	session->getColumnValue("ACT_TAG"         ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_actTag  = t_strTmp[0];
 	session->getColumnValue("FILL_TAG"     ,t_strTmp  ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_fillTag = t_strTmp[0];
 	session->getColumnValue("START_CYC_ID"    ,&m_record.m_startCycId ,t_length); 
 	session->getColumnValue("END_CYC_ID"      ,&m_record.m_endCycId ,t_length);    
 	return true;
}

void Mdb_PayRelation::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_PayRelation::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_PayRelation::Mdb_PayRelation(const Mdb_PayRelation& right)
{
	*this=right;
}

void Mdb_PayRelation::operator=(const Mdb_PayRelation& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_PayRelation &po)
{
	os << po.m_record.m_payRelationId<<" , "
     << po.m_record.m_userId<<" , "
	   << po.m_record.m_itemCode<<" , "
	   << po.m_record.m_acctId<<" , "        
	   << po.m_record.m_bindType<<" , "      
	   << po.m_record.m_addupMethod<<" , "      
	   << po.m_record.m_addupMonths<<" , "      
	   << po.m_record.m_payPriority<<" , "   
	   << po.m_record.m_discntPriority<<" , "
	   << po.m_record.m_defaultTag<<" , "    
//	   << po.m_record.m_limitId<<" , "       
//	   << po.m_record.m_orderNo<<" , "       
//	   << po.m_record.m_minValue<<" , "      
//	   << po.m_record.m_maxValue<<" , "      
	   << po.m_record.m_limitType<<" , "     
	   << po.m_record.m_limitValue<<" , "    
//	   << po.m_record.m_limitRatio<<" , "    
	   << po.m_record.m_fillTag<<" , "       
//	   << po.m_record.m_valueType<<" , "     
//	   << po.m_record.m_value<<" , "         
	   << po.m_record.m_actTag<<" , "        
	   << po.m_record.m_startCycId<<" , "    
	   << po.m_record.m_endCycId;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_PayRelation::m_property={"TF_F_PAYRELATION","IDX_TF_F_PAYRELATION",1,
    "",1};
