#include "PO_StateGroup.h"

bool PO_StateGroup::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_StateGroup::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_state=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_stategroup=tmpstr;

	return true;
}

PO_StateGroup::PO_StateGroup(const PO_StateGroup& right)
{
	*this=right;
}

void PO_StateGroup::operator=(const PO_StateGroup& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_StateGroup &left,const PO_StateGroup& right)
{
	return (left.recordStruct.m_state == right.recordStruct.m_state);
}

int operator<(const PO_StateGroup &left,const PO_StateGroup& right)
{
	return (left.recordStruct.m_state < right.recordStruct.m_state);
}

ostream& operator<<(ostream& os,const PO_StateGroup& po)
{
	os<<po.recordStruct.m_state<<" , "<<
			po.recordStruct.m_stategroup;
	return os;
}

PO_Property PO_StateGroup::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_StateGroup",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT USER_STATE,USER_GROUP FROM TD_STATE_GROUP",
		2,"select count(*) from TD_STATE_GROUP"};
