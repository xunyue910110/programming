#include "PO_ProvInnerCode.h"

bool PO_ProvInnerCode::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_ProvInnerCode::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_channelNo = atoi(tmpstr.c_str());

	return true;
}

PO_ProvInnerCode::PO_ProvInnerCode(const PO_ProvInnerCode& right)
{
	*this=right;
}

void PO_ProvInnerCode::operator=(const PO_ProvInnerCode& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_ProvInnerCode &left,const PO_ProvInnerCode& right)
{
	return (left.recordStruct.m_areaCode==right.recordStruct.m_areaCode);
}

int operator<(const PO_ProvInnerCode &left,const PO_ProvInnerCode& right)
{
	return (left.recordStruct.m_areaCode<right.recordStruct.m_areaCode);
}

ostream& operator<<(ostream& os,const PO_ProvInnerCode& po)
{
	os<<po.recordStruct.m_areaCode << " , "
		<<po.recordStruct.m_channelNo;
	return os;
}

PO_Property PO_ProvInnerCode::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_ProvInnerCode",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select area_code,channelno from TD_PROV_INNER_AREA",
		2,"select count(*) from TD_PROV_INNER_AREA"};

