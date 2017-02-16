#include "PO_DiscntStatecode.h"

bool PO_DiscntStatecode::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_DiscntStatecode::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_feediscntid=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	//m_record.m_statecode=tmpstr[0];
	strncpy(m_record.m_stateCodes,tmpstr.c_str(),STR_ENUMLEN);
	m_record.m_stateCodes[STR_ENUMLEN] = '\0';
	
	m_record.splitEnumValue((const char*)m_record.m_stateCodes, 
	                         m_record.m_stateCodeVector);
	
	return true;
}

PO_DiscntStatecode::PO_DiscntStatecode(const PO_DiscntStatecode& right)
{
	*this=right;
}

void PO_DiscntStatecode::operator=(const PO_DiscntStatecode& right)
{
	m_record=right.m_record;
}

int operator==(const PO_DiscntStatecode &left,const PO_DiscntStatecode& right)
{
	return (left.m_record.m_feediscntid == right.m_record.m_feediscntid);
}

int operator<(const PO_DiscntStatecode &left,const PO_DiscntStatecode& right)
{
	return (left.m_record.m_feediscntid < right.m_record.m_feediscntid);
}

ostream& operator<<(ostream& os,const PO_DiscntStatecode& po)
{
	os<<po.m_record.m_feediscntid<<" , "<<
			po.m_record.m_stateCodes;

	return os;
}

PO_Property PO_DiscntStatecode::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_DiscntStatecode",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select feediscnt_id,state_code from TD_B_FEEDISCNT_STATECODE",
		2,"select count(*) from TD_B_FEEDISCNT_STATECODE"};

