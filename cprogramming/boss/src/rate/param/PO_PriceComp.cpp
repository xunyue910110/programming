#include "PO_PriceComp.h"


bool PO_PriceComp::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_PriceComp::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_nodeId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_parentNode = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_nodeGroup = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_orderNo = atoi(tmpstr.c_str());	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_nodeType = tmpstr[0];		
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_nodeActId = atoi(tmpstr.c_str());	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	m_record.m_curNode = 0;
	m_record.m_nodeAction = 0;
	m_record.m_next = 0;
	
	return true;
}


PO_PriceComp::PO_PriceComp(const PO_PriceComp &right)
{
	*this = right;
}


void PO_PriceComp::operator = (const PO_PriceComp &right)
{
	m_record = right.m_record;
}


bool operator == (const PO_PriceComp &left,const PO_PriceComp &right)
{
    return (left.m_record == right.m_record);
}


bool operator < (const PO_PriceComp &left,const PO_PriceComp &right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_PriceComp &po)
{
	//os<< po.m_record <<endl;

	return os;
}


PO_Property PO_PriceComp::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_PriceComp","","","",
    "SELECT NODE_ID,P_NODE_ID,NODE_GROUP,NODE_NO,EXEC_TYPE,  \
            EXEC_ID,PRICE_ID FROM TD_B_PRICE_COMP",
    7,"SELECT count(*) FROM TD_B_PRICE_COMP"};

