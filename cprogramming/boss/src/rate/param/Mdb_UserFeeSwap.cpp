#include "Mdb_UserFeeSwap.h"

bool Mdb_UserFeeSwap::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserFeeSwap::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
	#define GET_COLUMNVALUE(X,Y) session->getColumnValue(X  ,&Y ,t_length); 	
 	#define GET_COLUMNVALUE_LONG(X,Y) session->getColumnValue(X ,&t_longValue ,t_length); Y = t_longValue;
 	#define GET_COLUMNVALUE_CHAR(X,Y) session->getColumnValue(X ,t_strTmp    ,t_length); t_strTmp[t_length]=0; Y = t_strTmp[0];
 	
 	GET_COLUMNVALUE_LONG("SRC_USER_ID",m_record.m_srcUserId);
 	GET_COLUMNVALUE_LONG("OBJ_USER_ID",m_record.m_objUserId);
 	GET_COLUMNVALUE("SWAP_ITEM_CODE",m_record.m_swapItemCode);
 	GET_COLUMNVALUE("PRIORITY",m_record.m_priority); 
 	//GET_COLUMNVALUE("LIMIT_ID" ,m_record.m_limitId);
 	GET_COLUMNVALUE_CHAR("SWAP_TYPE",m_record.m_swapType); 
 	GET_COLUMNVALUE_CHAR("LIMIT_TYPE",m_record.m_limitType); 
 	GET_COLUMNVALUE_LONG("LIMIT_VALUE",m_record.m_limitValue);
 	GET_COLUMNVALUE("START_CYC_ID",m_record.m_startCycId);  
	GET_COLUMNVALUE("END_CYC_ID",m_record.m_endCycId);  
 	  
 	return true;
}

void Mdb_UserFeeSwap::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_srcUserId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_UserFeeSwap::setOtherParams(InputParams *pInputParamsseOther) const
{
  pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));	
  pInputParamsseOther->setValue(VAR_TYPE_INT,(void *)(&m_record.m_startCycId));
}

Mdb_UserFeeSwap::Mdb_UserFeeSwap(const Mdb_UserFeeSwap& right)
{
	*this=right;
}

void Mdb_UserFeeSwap::operator=(const Mdb_UserFeeSwap& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserFeeSwap &po)
{
	os << po.m_record.m_srcUserId<<" , "                         
	   << po.m_record.m_objUserId<<" , "                         
	   << po.m_record.m_swapItemCode<<" , "                      
	   << po.m_record.m_swapType<<" , "                          
	   << po.m_record.m_priority<<" , "                          
	   //<< po.m_record.m_limitId<<" , "                           
	   //<< po.m_record.m_orderNo<<" , "                           
	   //<< po.m_record.m_minValue<<" , "                          
	   //<< po.m_record.m_maxValue<<" , "                          
	   << po.m_record.m_limitType<<" , "                         
	   << po.m_record.m_limitValue<<" , "                        
	   //<< po.m_record.m_limitRatio<<" , "                        
	   //<< po.m_record.m_fillTag<<" , "                           
	   << po.m_record.m_startCycId<<" , "    
	   << po.m_record.m_endCycId;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_UserFeeSwap::m_property={"TF_F_USER_FEESWAP","IDX_TF_F_USER_FEESWAP",1,
    "(START_CYC_ID<=:0) and (END_CYC_ID>=:1)",2};
