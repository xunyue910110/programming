#include "PO_Price.h"


bool PO_Price::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_Price::convert(const StringVector &argname)
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
	m_record.m_carryTag = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_sumToIntType, tmpstr.c_str(),MIN_STR_LENGTH );
	m_record.m_sumToIntType[MIN_STR_LENGTH] = '\0';	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_sumToIntFee, tmpstr.c_str(),MIN_STR_LENGTH );
	m_record.m_sumToIntFee[MIN_STR_LENGTH] = '\0';	
	
	m_record.m_priceComp = 0;
	m_record.m_priceExtend = 0;
			
	return true;
}


PO_Price::PO_Price(const PO_Price& right)
{
	*this = right;
}


void PO_Price::operator = (const PO_Price& right)
{
	m_record = right.m_record;
}


int operator == (const PO_Price &left,const PO_Price& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_Price &left,const PO_Price& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_Price& po)
{
	os<< po.m_record.m_id		 <<" , "<<
		 po.m_record.m_carryTag  <<" , "<<
		 po.m_record.m_priceComp; 

	return os;
}


PO_Property PO_Price::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_Price","","","",
    "SELECT PRICE_ID,CARRY_TAG,SUMTOINT_TYPE,SUMTOINT_FEE FROM TD_B_PRICE",
    4,"SELECT count(*) FROM TD_B_PRICE"};

