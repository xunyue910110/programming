#include "PO_TimeMethodBind.h"

bool PO_TimeMethodBind::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_TimeMethodBind::convert(const StringVector &argname)
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
	recordStruct.m_method=tmpstr;

	//add by dph,增加客户资料前后偏移量
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_preoffset=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_postoffset=atoi(tmpstr.c_str());
	//end of add by dph,增加客户资料前后偏移量

	return true;
}

PO_TimeMethodBind::PO_TimeMethodBind(const PO_TimeMethodBind& right)
{
	*this=right;
}

void PO_TimeMethodBind::operator=(const PO_TimeMethodBind& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_TimeMethodBind &left,const PO_TimeMethodBind& right)
{
	return (left.recordStruct.m_sourceType == right.recordStruct.m_sourceType);
}

int operator<(const PO_TimeMethodBind &left,const PO_TimeMethodBind& right)
{
	return (left.recordStruct.m_sourceType < right.recordStruct.m_sourceType);
}

ostream& operator<<(ostream& os,const PO_TimeMethodBind& po)
{
	/*os<<po.recordStruct.m_sourceType<<" , "<<
		po.recordStruct.m_method;*/
	//modify by dph
	os<<po.recordStruct.m_sourceType<<" , "<<
		po.recordStruct.m_method<<" , "<<po.recordStruct.m_preoffset<<" , "<<po.recordStruct.m_postoffset;
	//end of modify by dph
	return os;
}

/*PO_Property PO_TimeMethodBind::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_TimeMethodBind",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT SOURCE_TYPE,METHOD FROM TD_TIME_BINDMETHOD",
		2,"select count(*) from TD_TIME_BINDMETHOD"};*/

//modify by dph,增加客户资料前后偏移量
PO_Property PO_TimeMethodBind::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_TimeMethodBind",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT SOURCE_TYPE,METHOD,PRE_OFFSET,POST_OFFSET FROM TD_TIME_BINDMETHOD",
		4,"select count(*) from TD_TIME_BINDMETHOD"};
