#include "Mdb_UserInfo.h"
//2008-10-24 9:29:30
bool Mdb_UserInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserInfo::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[50+1];
        memset(t_strTmp,0,51);
	
	//m_record->m_idAttr[IdInfo::ID_TYPE_CODE] = USER_ID_TYPE;
 	session->getColumnValue("USER_ID"           ,&t_longValue ,t_length); m_record->m_idAttr[IdInfo::ID_ATTR_ID]  = t_longValue;
 	session->getColumnValue("DUMMY_TAG"      , t_strTmp    ,t_length); 
 	        t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_TYPE_CODE]= (t_strTmp[0]=='0'?(char)USER_ID_TYPE:(char)GROUP_ID_TYPE);
 	session->getColumnValue("EPARCHY_CODE"      , t_strTmp    ,t_length); 
 	        t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_EPARCHY_CODE]= t_strTmp;
 	session->getColumnValue("CITY_CODE"         , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_CITY_CODE] = t_strTmp;
 	session->getColumnValue("CUST_ID"           ,&t_longValue ,t_length); m_record->m_idAttr[IdInfo::ID_CUST_ID]  = t_longValue;
 	session->getColumnValue("USER_TYPE_CODE"     , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_USER_TYPE_CODE] = t_strTmp[0];
    session->getColumnValue("BRAND_CODE"         , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_BRAND_CODE] = t_strTmp;	
 	session->getColumnValue("PRODUCT_ID"         , &t_intValue    ,t_length); m_record->m_idAttr[IdInfo::ID_PRODUCT_ID] = t_intValue;	
 	session->getColumnValue("SERVICE_STATE_CODE"         , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_USER_STATE] = t_strTmp;
 	session->getColumnValue("SCORE_VALUE"       ,&t_longValue,t_length); 
 		m_record->m_idAttr[IdInfo::ID_SCORE_VALUE]  = t_longValue;
 	session->getColumnValue("CREDIT_VALUE"      ,&t_longValue,t_length); 
 	    m_record->m_idAttr[IdInfo::ID_CREDIT_VALUE] = t_longValue;
 	session->getColumnValue("NET_TYPE_CODE" ,t_strTmp,t_length);
 	    t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_NETTYPE_CODE] = t_strTmp;
 	session->getColumnValue("REMOVE_TAG"        , t_strTmp ,t_length);
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_REMOVE_TAG] = t_strTmp[0];
 	session->getColumnValue("OPEN_DATE"         ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_OPEN_DATE] = t_longValue;
 	session->getColumnValue("DESTROY_DATE"       ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_REMOVE_DATE]  = t_longValue;
  session->getColumnValue("ACCT_TAG"        , t_strTmp ,t_length);
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_ACCOUNT_TAG] = t_strTmp[0];
 	session->getColumnValue("FIRST_CALL_DATE"   ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_FIRSTCALL_TIME] = t_longValue;
 	session->getColumnValue("PRE_DESTROY_DATE"   ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_PREDESTROY_TIME] = t_longValue;
  session->getColumnValue("LAST_STOP_DATE"   ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_LASTSTOP_TIME] = t_longValue;	
 	session->getColumnValue("SERIAL_NUMBER"         , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_MSISDN] = t_strTmp;
 	session->getColumnValue("UPDATE_TIME"   ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_UPDATE_TIME] = t_longValue;	
 	session->getColumnValue("PREPAY_TAG"      , t_strTmp    ,t_length); 
 	        t_strTmp[t_length]=0;m_record->m_idAttr[IdInfo::ID_PREPAY_TAG]= t_strTmp[0];
	session->getColumnValue("IN_DATE"         ,&t_longValue,t_length); m_record->m_idAttr[IdInfo::ID_IN_DATE] = t_longValue;	 //add by xuf 2010-1-3 21:37:58        	
 	return true;
}

void Mdb_UserInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record->m_idAttr[IdInfo::ID_ATTR_ID].value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_UserInfo::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserInfo::Mdb_UserInfo(const Mdb_UserInfo& right)
{
	*this=right;
}

void Mdb_UserInfo::operator=(const Mdb_UserInfo& right)
{
	(*m_record)=(*right.m_record);
}

ostream& operator<<(ostream& os,const Mdb_UserInfo &po)
{
	os << *po.m_record; 
	return os;
}

Mdb_Property Mdb_UserInfo::m_property={"TF_F_USER","idx_tf_f_user_userid",1,"",0};
