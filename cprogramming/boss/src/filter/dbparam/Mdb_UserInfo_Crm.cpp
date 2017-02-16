#include "Mdb_UserInfo_Crm.h"

bool Mdb_UserInfo_Crm::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserInfo_Crm::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];

 	session->getColumnValue("net_type_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_net_type_code = t_strTmp;

	session->getColumnValue("eparchy_code",t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_eparchy_code = t_strTmp;

 	session->getColumnValue("city_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_office_code = t_strTmp;

	session->getColumnValue("cust_id"     ,&t_longValue,t_length);m_record.m_custId = t_longValue;

  session->getColumnValue("brand_code"  ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_brand_code = t_strTmp;

  session->getColumnValue("open_mode"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_open_mode = t_strTmp;

  session->getColumnValue("acct_tag"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_acct_tag = t_strTmp;

 	session->getColumnValue("product_id"  ,&t_intValue,t_length);m_record.m_productId = t_intValue;

  session->getColumnValue("prepay_tag"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_prepay_tag = t_strTmp;

  session->getColumnValue("user_type_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_user_type_code = t_strTmp;

  session->getColumnValue("in_date"     ,&t_longValue,t_length);m_record.m_inTime = t_longValue;

 	session->getColumnValue("open_date"   ,&t_longValue,t_length);m_record.m_openTime = t_longValue;

 	return true;
}

void Mdb_UserInfo_Crm::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&m_record.m_userId);
}

void Mdb_UserInfo_Crm::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserInfo_Crm::Mdb_UserInfo_Crm(const Mdb_UserInfo_Crm& right)
{
	*this=right;
}

void Mdb_UserInfo_Crm::operator=(const Mdb_UserInfo_Crm& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserInfo_Crm &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_net_type_code << " , "
	   << po.m_record.m_eparchy_code  << " , "
	   << po.m_record.m_office_code   << " , "
	   << po.m_record.m_custId        << " , "
	   << po.m_record.m_brand_code    << " , "
	   << po.m_record.m_open_mode     << " , "
	   << po.m_record.m_acct_tag      << " , "
	   << po.m_record.m_productId     << " , "
	   << po.m_record.m_prepay_tag    << " , "
	   << po.m_record.m_user_type_code  << " , "
	   << po.m_record.m_inTime        << " , "
	   << po.m_record.m_openTime;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserInfo_Crm::m_property={"tf_f_user","idx_tf_f_user_userid",1,"",0};
