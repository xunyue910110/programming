#include "PO_AVByLac.h"


bool PO_AVByLac::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByLac::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.lac=tmpstr;
	
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
	recordStruct.visitAreaCode=tmpstr;

	return true;
}

PO_AVByLac::PO_AVByLac(const PO_AVByLac& right)
{
	*this=right;
}

void PO_AVByLac::operator=(const PO_AVByLac& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByLac &left,const PO_AVByLac& right)
{
	return (left.recordStruct.lac==right.recordStruct.lac);
}

int operator<(const PO_AVByLac &left,const PO_AVByLac& right)
{
	return (left.recordStruct.lac<right.recordStruct.lac);
}

ostream& operator<<(ostream& os,const PO_AVByLac& po)
{
	os<<po.recordStruct.lac<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByLac::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByLac",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select LAC, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		4,"select count(*) from TD_LAC"};

