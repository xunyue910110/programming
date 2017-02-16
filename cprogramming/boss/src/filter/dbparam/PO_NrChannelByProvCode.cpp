#include "PO_NrChannelByProvCode.h"

bool PO_NrChannelByProvCode::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_NrChannelByProvCode::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_provCode = tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_channelNo = atoi(tmpstr.c_str());

	return true;
}

PO_NrChannelByProvCode::PO_NrChannelByProvCode(const PO_NrChannelByProvCode& right)
{
	*this=right;
}

void PO_NrChannelByProvCode::operator=(const PO_NrChannelByProvCode& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_NrChannelByProvCode &left,const PO_NrChannelByProvCode& right)
{
	return left.recordStruct.m_provCode == right.recordStruct.m_provCode;
}

int operator<(const PO_NrChannelByProvCode &left,const PO_NrChannelByProvCode& right)
{
	return left.recordStruct.m_provCode < right.recordStruct.m_provCode;
}

ostream& operator<<(ostream& os,const PO_NrChannelByProvCode& po)
{
	os<<po.recordStruct.m_provCode << " , "
		<<po.recordStruct.m_channelNo;
	return os;
}

PO_Property PO_NrChannelByProvCode::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_NrChannelByProvCode",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select prov_code,channelNo from td_prov_code",
		2,"select count(*) from td_prov_code"};
