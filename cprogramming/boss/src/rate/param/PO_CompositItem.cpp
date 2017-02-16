#include "PO_CompositItem.h"


bool PO_CompositItem::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_CompositItem::convert(const StringVector &argname)
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
	m_record.m_subItemId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_orderNo = atoi(tmpstr.c_str());
	
	m_record.m_itemType = '1';
		
	return true;
}


PO_CompositItem::PO_CompositItem(const PO_CompositItem &right)
{
	*this = right;
}


void PO_CompositItem::operator = (const PO_CompositItem &right)
{
	m_record = right.m_record;
}


int operator == (const PO_CompositItem &left,const PO_CompositItem& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_CompositItem &left,const PO_CompositItem& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_CompositItem& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_CompositItem::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_CompositItem","","","",
    "SELECT ITEM_ID,SUB_ITEM_ID,SUB_ITEM_NO FROM TD_B_COMPITEM",3,
    "SELECT count(*) FROM TD_B_COMPITEM"};
