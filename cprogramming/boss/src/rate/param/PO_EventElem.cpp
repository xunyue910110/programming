#include "PO_EventElem.h"


bool PO_ElemEvent::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ElemEvent::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eventElemKey = atoi(tmpstr.c_str());
	
	return true;
}


PO_ElemEvent::PO_ElemEvent(const PO_ElemEvent& right)
{
	*this = right;
}


void PO_ElemEvent::operator = (const PO_ElemEvent& right)
{
	m_record = right.m_record;
}


int operator == (const PO_ElemEvent &left,const PO_ElemEvent& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ElemEvent &left,const PO_ElemEvent& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ElemEvent& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ElemEvent::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ElemEvent","","","",
    "SELECT EVENT_ELEM_ID,EVENT_ELEM_KEY FROM TD_B_EVENTELEM",2,
    "SELECT count(*) FROM TD_B_EVENTELEM"};

