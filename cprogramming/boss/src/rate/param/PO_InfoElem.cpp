#include "PO_InfoElem.h"


bool PO_ElemInfo::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ElemInfo::convert(const StringVector &argname)
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
	m_record.m_infoElemKey = atoi(tmpstr.c_str());			
	return true;
}


PO_ElemInfo::PO_ElemInfo(const PO_ElemInfo& right)
{
	*this = right;
}


void PO_ElemInfo::operator = (const PO_ElemInfo& right)
{
	m_record = right.m_record;
}


int operator == (const PO_ElemInfo &left,const PO_ElemInfo& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ElemInfo &left,const PO_ElemInfo& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ElemInfo& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ElemInfo::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ElemInfo","","","",
    "SELECT INFO_ELEM_ID,INFO_ELEM_KEY FROM TD_B_INFOELEM",2,
    "SELECT count(*) FROM TD_B_INFOELEM"};

