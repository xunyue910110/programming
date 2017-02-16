#include "PO_RedoUserInfo.h"

bool PO_RedoUserInfo::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_RedoUserInfo::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_userId=tmpstr;

	return true;
}

PO_RedoUserInfo::PO_RedoUserInfo(const PO_RedoUserInfo& right)
{
	*this=right;
}

void PO_RedoUserInfo::operator=(const PO_RedoUserInfo& right)
{
	m_record=right.m_record;
}

int operator==(const PO_RedoUserInfo &left,const PO_RedoUserInfo& right)
{
	return left.m_record.m_userId==right.m_record.m_userId;
}

int operator<(const PO_RedoUserInfo &left,const PO_RedoUserInfo &right)
{
	return left.m_record.m_userId<right.m_record.m_userId;
}

ostream& operator<<(ostream& os,const PO_RedoUserInfo& po)
{
	bool isFirst = true;
	os<<po.m_record.m_userId;
	return os;
}


PO_Property PO_RedoUserInfo::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_RedoUserInfo",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select UserId from td_b_redoUserInfo",
		1,"select count(*) from td_b_redoUserInfo"};

