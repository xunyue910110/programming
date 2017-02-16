#include "PO_AVByMsc.h"


bool PO_AVByMsc::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByMsc::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msc = tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_mscType = atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime = tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime = tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_visitAreaCode = tmpstr;

	return true;
}

PO_AVByMsc::PO_AVByMsc(const PO_AVByMsc& right)
{
	*this=right;
}

void PO_AVByMsc::operator=(const PO_AVByMsc& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByMsc &left,const PO_AVByMsc& right)
{
	return (left.recordStruct.m_msc == right.recordStruct.m_msc);
}

int operator<(const PO_AVByMsc &left,const PO_AVByMsc& right)
{
	return (left.recordStruct.m_msc < right.recordStruct.m_msc);
}

ostream& operator<<(ostream& os,const PO_AVByMsc& po)
{
	os<<po.recordStruct.m_msc << " , " <<
			po.recordStruct.m_mscType <<" , "<<
			po.recordStruct.m_beginTime <<" , "<<
			po.recordStruct.m_endTime <<" , "<<
			po.recordStruct.m_visitAreaCode;
	return os;
}


PO_Property PO_AVByMsc::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByMsc",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, MSC_TYPE, BEGIN_TIME, END_TIME, AREA_CODE from TD_PROV_MSC",
		5,"select count(*) from TD_PROV_MSC"};

