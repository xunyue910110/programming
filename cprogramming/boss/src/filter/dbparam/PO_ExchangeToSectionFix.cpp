#include "PO_ExchangeToSectionFix.h"

bool PO_ExchangeToSectionFix::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_ExchangeToSectionFix::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_exchangeCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sectionCode=tmpstr;
	
	return true;
}

PO_ExchangeToSectionFix::PO_ExchangeToSectionFix(const PO_ExchangeToSectionFix& right)
{
	*this=right;
}

void PO_ExchangeToSectionFix::operator=(const PO_ExchangeToSectionFix& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_ExchangeToSectionFix &left,const PO_ExchangeToSectionFix& right)
{
	return left.recordStruct.m_exchangeCode==right.recordStruct.m_exchangeCode;
}

int operator<(const PO_ExchangeToSectionFix &left,const PO_ExchangeToSectionFix& right)
{
	return (left.recordStruct.m_exchangeCode<right.recordStruct.m_exchangeCode);
}

ostream& operator<<(ostream& os,const PO_ExchangeToSectionFix& po)
{
	os<<po.recordStruct.m_exchangeCode<<" , "<<
			po.recordStruct.m_sectionCode;
	return os;
}


PO_Property PO_ExchangeToSectionFix::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_ExchangeToSectionFix",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_CODE||EXCHANGE_CODE, SECTION_CODE from TD_EXCHANGE_SECTION_REL",
		2,"select count(*) from TD_EXCHANGE_SECTION_REL"};
