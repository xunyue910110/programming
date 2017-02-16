#include "PO_B_Eventelem.h"

bool PO_B_Eventelem::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_B_Eventelem::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_elementId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_elementKey=atoi(tmpstr.c_str());
	
	
	return true;
}

PO_B_Eventelem::PO_B_Eventelem(const PO_B_Eventelem& right)
{
	*this=right;
}

void PO_B_Eventelem::operator=(const PO_B_Eventelem& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_B_Eventelem &left, const PO_B_Eventelem& right)
{
	return left.recordStruct.m_elementId==right.recordStruct.m_elementId;
}

int operator<(const PO_B_Eventelem &left,const PO_B_Eventelem& right)
{
	return (left.recordStruct.m_elementId < right.recordStruct.m_elementId);
}

ostream& operator<<(ostream& os,const PO_B_Eventelem& po)
{
	os<<po.recordStruct.m_elementId<<" , "<<
			po.recordStruct.m_elementKey;
	return os;
}


PO_Property PO_B_Eventelem::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_B_Eventelem",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select EVENT_ELEM_ID, EVENT_ELEM_KEY from TD_B_EVENTELEM",
		2,"select count(*) from TD_B_EVENTELEM"};

