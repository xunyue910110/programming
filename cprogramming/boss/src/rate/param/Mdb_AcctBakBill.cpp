#include "Mdb_AcctBakBill.h"

bool Mdb_AcctBakBill::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_AcctBakBill::convert(Session* session)
{
	int  t_length;
	long t_longValue;
	
 	session->getColumnValue("ACCT_ID"       ,&t_longValue ,t_length); m_record.m_acctId = t_longValue;
 	session->getColumnValue("USER_ID"       ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("CYC_ID"        ,&m_record.m_cycId  ,t_length);
 	session->getColumnValue("DETAIL_ITEM_CODE",&m_record.m_itemCode ,t_length); 
 	session->getColumnValue("FEE"      ,&m_record.m_fee ,t_length);
 	session->getColumnValue("B_DISCNT" ,&m_record.m_bDiscnt ,t_length); 
 	session->getColumnValue("A_DISCNT" ,&m_record.m_aDiscnt ,t_length); 
 	session->getColumnValue("B_ADJUST" ,&m_record.m_bAdjust ,t_length); 
 	return true;
}

void Mdb_AcctBakBill::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_AcctBakBill::setOtherParams(InputParams *pInputParamsseOther) const
{
  pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_cycId));	
  
}

Mdb_AcctBakBill::Mdb_AcctBakBill(const Mdb_AcctBakBill& right)
{
	*this=right;
}

void Mdb_AcctBakBill::operator=(const Mdb_AcctBakBill& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_AcctBakBill &po)
{
	os << po.m_record.m_acctId<<" , "
	   << po.m_record.m_userId<<" , "
	   << po.m_record.m_cycId<<" , "        
	   << po.m_record.m_itemCode<<" , "      
	   << po.m_record.m_fee<<" , "   
	   << po.m_record.m_bDiscnt<<" , "
	   << po.m_record.m_aDiscnt<<" , "    
	   << po.m_record.m_bAdjust;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_AcctBakBill::m_property={"TS_B_DETAILBILL_BAK","IDX_TS_B_DETAILBILL",1,
    "CYC_ID=:0",1};
