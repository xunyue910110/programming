#include "PO_FeePolicyRemidInfo.h"

bool PO_FeePolicyRemidInfo::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_FeePolicyRemidInfo::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_feePolicyId=tmpstr.c_str();

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_billId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_remindLevel=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_valueType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_value=atol(tmpstr.c_str());

	return true;
}

PO_FeePolicyRemidInfo::PO_FeePolicyRemidInfo(const PO_FeePolicyRemidInfo& right)
{
	*this=right;
}

void PO_FeePolicyRemidInfo::operator=(const PO_FeePolicyRemidInfo& right)
{
	m_record=right.m_record;
}

int operator==(const PO_FeePolicyRemidInfo &left,const PO_FeePolicyRemidInfo& right)
{
	return (left.m_record.m_feePolicyId == right.m_record.m_feePolicyId &&
	        left.m_record.m_billId == right.m_record.m_billId);
}

int operator<(const PO_FeePolicyRemidInfo &left,const PO_FeePolicyRemidInfo& right)
{
	if(left.m_record.m_feePolicyId < right.m_record.m_feePolicyId) return true;
	if(left.m_record.m_feePolicyId > right.m_record.m_feePolicyId) return false;
	return (left.m_record.m_billId < right.m_record.m_billId);
}

ostream& operator<<(ostream& os,const PO_FeePolicyRemidInfo& po)
{
	os<<po.m_record.m_feePolicyId   <<" , "<<
	    po.m_record.m_billId<<" , "<<
		po.m_record.m_remindLevel<<" , "<<
		po.m_record.m_valueType<<" , "<<
		po.m_record.m_value;
			
	return os;
}

PO_Property PO_FeePolicyRemidInfo::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FeePolicyRemidInfo",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT FEEPOLICY_ID,ADDUP_ID,REMIND_LEVEL,VALUE_TYPE,VALUE FROM TD_B_FEEPOLICY_REMIND",
		5,"SELECT COUNT(*) FROM TD_B_FEEPOLICY_REMIND"};
