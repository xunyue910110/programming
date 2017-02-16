#include "PO_NetTypeCodeBind.h"

bool PO_NetTypeCodeBind::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_NetTypeCodeBind::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sourceType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_netTypeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_method=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginDate=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endDate=tmpstr;

	return true;
}

PO_NetTypeCodeBind::PO_NetTypeCodeBind(const PO_NetTypeCodeBind& right)
{
	*this=right;
}

void PO_NetTypeCodeBind::operator=(const PO_NetTypeCodeBind& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_NetTypeCodeBind &left,const PO_NetTypeCodeBind& right)
{
	return (left.recordStruct.m_sourceType == right.recordStruct.m_sourceType);
}

int operator<(const PO_NetTypeCodeBind &left,const PO_NetTypeCodeBind& right)
{
	return (left.recordStruct.m_sourceType < right.recordStruct.m_sourceType);
}

ostream& operator<<(ostream& os,const PO_NetTypeCodeBind& po)
{
	os<<po.recordStruct.m_sourceType<<" , "<<
      po.recordStruct.m_netTypeCode<<" , "<<
      po.recordStruct.m_method<<" , "<<
      po.recordStruct.m_beginDate<<" , "<<
			po.recordStruct.m_endDate;
	return os;
}

PO_Property PO_NetTypeCodeBind::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_NetTypeCodeBind",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT SOURCE_TYPE,NET_TYPE_CODE,METHOD,BEGIN_TIME,END_TIME FROM TD_NETTYPE_BINDMETHOD",
		5,"select count(*) from TD_NETTYPE_BINDMETHOD"};
