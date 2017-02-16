#include "PO_FeePolicyAttr.h"

bool PO_FeePolicyAttr::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_FeePolicyAttr::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_ruleId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_attrid=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_value=tmpstr;
	
	m_record.m_condRule=NULL;
	
	return true;
}

PO_FeePolicyAttr::PO_FeePolicyAttr(const PO_FeePolicyAttr& right)
{
	*this=right;
}

void PO_FeePolicyAttr::operator=(const PO_FeePolicyAttr& right)
{
	m_record=right.m_record;
}

int operator==(const PO_FeePolicyAttr &left,const PO_FeePolicyAttr& right)
{
	return (left.m_record.m_tpId == right.m_record.m_tpId);
}

int operator<(const PO_FeePolicyAttr &left,const PO_FeePolicyAttr& right)
{
	if (left.m_record.m_tpId < right.m_record.m_tpId) return true;
	if (left.m_record.m_tpId > right.m_record.m_tpId) return false;
	
	return false;
}

ostream& operator<<(ostream& os,const PO_FeePolicyAttr& po)
{
	os<<po.m_record.m_tpId  <<" , "<<
			po.m_record.m_ruleId<<" , "<<
			po.m_record.m_attrid<<" , "<<
			po.m_record.m_value;
			
	return os;
}

PO_Property PO_FeePolicyAttr::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FeePolicyAttr",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT FEEPOLICY_ID,COND_ID,ATTR_ID,VALUE FROM TD_B_FEEPOLICY_ATTR",
		4,"SELECT COUNT(*) FROM TD_B_FEEPOLICY_ATTR"};

