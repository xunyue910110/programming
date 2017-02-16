#include "PO_VisitAreaCodeLac.h"


bool PO_VisitAreaCodeLac::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_VisitAreaCodeLac::convert(const StringVector &argname)
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

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.officeCode=tmpstr;

	return true;
}

PO_VisitAreaCodeLac::PO_VisitAreaCodeLac(const PO_VisitAreaCodeLac& right)
{
	*this=right;
}

void PO_VisitAreaCodeLac::operator=(const PO_VisitAreaCodeLac& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_VisitAreaCodeLac &left,const PO_VisitAreaCodeLac& right)
{
	return (left.recordStruct.lac==right.recordStruct.lac);
}

int operator<(const PO_VisitAreaCodeLac &left,const PO_VisitAreaCodeLac& right)
{
	return (left.recordStruct.lac<right.recordStruct.lac);
}

ostream& operator<<(ostream& os,const PO_VisitAreaCodeLac& po)
{
	os<<po.recordStruct.lac<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode<<" , "<<
			po.recordStruct.officeCode;
	return os;
}


PO_Property PO_VisitAreaCodeLac::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_VisitAreaCodeLac",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select LAC, BEGIN_TIME, END_TIME, AREA_CODE, OFFICE_CODE from TD_LAC",
		5,"select count(*) from TD_LAC"};

