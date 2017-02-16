#include "Mdb_UserOtherInfo.h"

bool Mdb_UserOtherInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserOtherInfo::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[100+1];
	
	#define GET_COLUMNVALUE(X,Y) 			session->getColumnValue(X ,&Y ,t_length); 	
	#define GET_COLUMNVALUE_LONG(X,Y) session->getColumnValue(X ,&t_longValue ,t_length); Y = t_longValue;
 	#define GET_COLUMNVALUE_CHAR(X,Y) session->getColumnValue(X ,t_strTmp    ,t_length); t_strTmp[t_length]=0; Y = t_strTmp[0];
 	
 	GET_COLUMNVALUE_LONG("USER_ID",						  m_record.m_userId);
 	GET_COLUMNVALUE("OTHERINFO_TYPE",			m_record.m_otherInfoType);
 	GET_COLUMNVALUE("ATTR_VALUE1",				  m_record.m_attrValue1);
 	GET_COLUMNVALUE("ATTR_VALUE2",				  m_record.m_attrValue2); 
 	GET_COLUMNVALUE("ATTR_VALUE3",				  m_record.m_attrValue3); 
 	GET_COLUMNVALUE("ATTR_VALUE4",				  m_record.m_attrValue4); 
 	GET_COLUMNVALUE("ATTR_VALUE5",				  m_record.m_attrValue5);
 	GET_COLUMNVALUE("ATTR_VALUE6",				  m_record.m_attrValue6);  
	GET_COLUMNVALUE("ATTR_VALUE7",				  m_record.m_attrValue7);  
	GET_COLUMNVALUE("ATTR_VALUE8",				  m_record.m_attrValue8); 
 	GET_COLUMNVALUE_LONG("START_DATE",					m_record.m_startDate);  
	GET_COLUMNVALUE_LONG("END_DATE",						m_record.m_endDate); 
 	  
 	return true;
}

void Mdb_UserOtherInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_userId = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);
}

void Mdb_UserOtherInfo::setOtherParams(InputParams *pInputParamsseOther) const
{
}

Mdb_UserOtherInfo::Mdb_UserOtherInfo(const Mdb_UserOtherInfo& right)
{
	*this=right;
}

void Mdb_UserOtherInfo::operator=(const Mdb_UserOtherInfo& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserOtherInfo &po)
{	   
	 os << po.m_record.m_userId<<" , "         //用户标识	          
    	<< po.m_record.m_otherInfoType<<" , "  //其他信息类型	
   	 	<< po.m_record.m_attrValue1<<" , "    //属性值1  
     	<< po.m_record.m_attrValue2<<" , "     //属性值2	  
     	<< po.m_record.m_attrValue3<<" , "     //属性值3  
     	<< po.m_record.m_attrValue4<<" , "     //属性值4  
     	<< po.m_record.m_attrValue5<<" , "     //属性值5	  
    	<< po.m_record.m_attrValue6<<" , "      //属性值6	  
     	<< po.m_record.m_attrValue7<<" , "      //属性值7	  
     	<< po.m_record.m_attrValue8<<" , "      //属性值8
     	<< po.m_record.m_startDate<<" , " 			//生效时间
     	<< po.m_record.m_endDate;				        //终止时间
	   
	return os;        
}                   
                    
Mdb_Property Mdb_UserOtherInfo::m_property={"TF_F_USER_OTHERINFO","IDX_TF_F_USER_OTHERINFO_USER_ID",1,
    "",0};
