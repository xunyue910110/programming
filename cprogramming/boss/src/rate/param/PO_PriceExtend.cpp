#include "PO_PriceExtend.h"


bool PO_PriceExtend::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_PriceExtend::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_extendId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_extendType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_extendValue1 = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_extendValue2 = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_extendValue3 = tmpstr;
	
	m_record.m_next = 0;		
	return true;
}


PO_PriceExtend::PO_PriceExtend(const PO_PriceExtend& right)
{
	*this = right;
}


void PO_PriceExtend::operator = (const PO_PriceExtend& right)
{
	m_record = right.m_record;
}


int operator == (const PO_PriceExtend &left,const PO_PriceExtend& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_PriceExtend &left,const PO_PriceExtend& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_PriceExtend& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_PriceExtend::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_PriceExtend","","","",
    "SELECT EXTEND_ID,PRICE_ID,EXTEND_TYPE,EXTEND_VALUE1,"
    "EXTEND_VALUE2,EXTEND_VALUE3 FROM TD_B_PRICE_EXTEND",6,
    "SELECT count(*) FROM TD_B_PRICE_EXTEND"};

