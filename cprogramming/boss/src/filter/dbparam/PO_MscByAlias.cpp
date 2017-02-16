#include "PO_MscByAlias.h"


bool PO_MscByAlias::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_MscByAlias::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.msc=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.msc_alias=tmpstr;

	return true;
}

PO_MscByAlias::PO_MscByAlias(const PO_MscByAlias& right)
{
	*this=right;
}

void PO_MscByAlias::operator=(const PO_MscByAlias& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_MscByAlias &left,const PO_MscByAlias& right)
{
	return (left.recordStruct.msc_alias==right.recordStruct.msc_alias);
}

int operator<(const PO_MscByAlias &left,const PO_MscByAlias& right)
{
	return (left.recordStruct.msc_alias<right.recordStruct.msc_alias);
}

ostream& operator<<(ostream& os,const PO_MscByAlias& po)
{
	os<<po.recordStruct.msc<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.msc_alias;
	return os;
}


PO_Property PO_MscByAlias::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_MscByAlias",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, BEGIN_TIME, END_TIME, MSC_ALIAS from TD_PROV_MSC",
		4,"select count(*) from TD_PROV_MSC"};

