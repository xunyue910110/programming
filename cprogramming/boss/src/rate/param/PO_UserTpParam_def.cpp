#include "PO_UserTpParam_def.h"

bool PO_UserTpParam_def::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_UserTpParam_def::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpParamId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_relatinType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	{
	  m_record.m_valueType=tmpstr.c_str()[0];
	}else
	{
	  m_record.m_valueType='0';
	}
	return true;
}

PO_UserTpParam_def::PO_UserTpParam_def(const PO_UserTpParam_def& right)
{
	*this=right;
}

void PO_UserTpParam_def::operator=(const PO_UserTpParam_def& right)
{
	m_record=right.m_record;
}

int operator==(const PO_UserTpParam_def &left,const PO_UserTpParam_def& right)
{
	return (left.m_record.m_tpId == right.m_record.m_tpId);
}

int operator<(const PO_UserTpParam_def &left,const PO_UserTpParam_def& right)
{
	if (left.m_record.m_tpId < right.m_record.m_tpId)
	{
		return true;
	}
	if (left.m_record.m_tpId > right.m_record.m_tpId)
	{
		return false;
	}
	return false;
}

ostream& operator<<(ostream& os,const PO_UserTpParam_def& po)
{
	os<<po.m_record.m_tpParamId  <<" , "<<
			po.m_record.m_tpId       <<" , "<<
			po.m_record.m_relatinType<<" , "<<
			po.m_record.m_valueType;
			
	return os;
}

PO_Property PO_UserTpParam_def::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_UserTpParam_def",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select PARAM_ID,FEEPOLICY_ID,PARAM_RELA_TYPE,PARAM_VALUE_TYPE \
		 from TD_B_FEEPOLICY_PARAM",
		4,"select count(*) from TD_B_FEEPOLICY_PARAM"};

