#include "Mdb_FeeChgLimit.h"

 	 ostream& operator<<(ostream& os,const FeeChgLimit &po)
	{
		os <<"m_limitId: "<<po.m_limitId<<" , "       
	   	<< "m_orderNo: "<<po.m_orderNo<<" , "       
	   	<< "m_minValue: "<<po.m_minValue<<" , "      
	   	<< "m_maxValue: "<<po.m_maxValue<<" , "      
	   	<< "m_limitType: "<<po.m_limitType<<" , "     
	   	<< "m_limitValue: "<<po.m_limitValue<<" , "    
	   	<< "m_limitRatio: "<<po.m_limitRatio<<" , "    
	   	<< "m_fillTag: "<<po.m_fillTag;
	   
		return os;        
	}  
	
	
	bool operator < (const FeeChgLimit &r_left,const FeeChgLimit &r_right)
  {
      if (r_left.m_limitId == r_right.m_limitId)
      {
        return (r_left.m_orderNo < r_right.m_orderNo);
      }
      else
      {
        return (r_left.m_limitId < r_right.m_limitId);
      }        
  } 

bool Mdb_FeeChgLimit::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_FeeChgLimit::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
 	session->getColumnValue("LIMIT_ID"     ,&m_record.m_limitId ,t_length); 
 	session->getColumnValue("ORDER_NO"     ,&m_record.m_orderNo ,t_length);
 	session->getColumnValue("MIN_VALUE"    ,&m_record.m_minValue ,t_length); 
 	session->getColumnValue("MAX_VALUE"    ,&m_record.m_maxValue  ,t_length);
 	session->getColumnValue("LIMIT_TYPE"   ,t_strTmp     ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_limitType  = t_strTmp[0];
 	session->getColumnValue("LIMIT_VALUE"  ,&m_record.m_limitValue ,t_length);
 	session->getColumnValue("LIMIT_RATIO"  ,&m_record.m_limitRatio ,t_length); 
 	session->getColumnValue("FILL_TAG"     ,t_strTmp  ,t_length); 
 	                        t_strTmp[t_length]=0;m_record.m_fillTag = t_strTmp[0];
 	return true;
}

void Mdb_FeeChgLimit::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_INT,(void *)&m_record.m_limitId);
}

void Mdb_FeeChgLimit::setOtherParams(InputParams *pInputParamsseOther) const
{

  
}

Mdb_FeeChgLimit::Mdb_FeeChgLimit(const Mdb_FeeChgLimit& right)
{
	*this=right;
}

void Mdb_FeeChgLimit::operator=(const Mdb_FeeChgLimit& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_FeeChgLimit &po)
{
	os << po.m_record.m_limitId<<" , "       
	   << po.m_record.m_orderNo<<" , "       
	   << po.m_record.m_minValue<<" , "      
	   << po.m_record.m_maxValue<<" , "      
	   << po.m_record.m_limitType<<" , "     
	   << po.m_record.m_limitValue<<" , "    
	   << po.m_record.m_limitRatio<<" , "    
	   << po.m_record.m_fillTag;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_FeeChgLimit::m_property={"TF_F_FEECHANGE_LIMIT","IDX_TF_F_FEECHANGE_LIMIT",1,
    "",0};
