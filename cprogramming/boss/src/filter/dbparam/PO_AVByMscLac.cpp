#include "PO_AVByMscLac.h"


bool PO_AVByMscLac::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByMscLac::convert(const StringVector &argname)
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

PO_AVByMscLac::PO_AVByMscLac(const PO_AVByMscLac& right)
{
	*this=right;
}

void PO_AVByMscLac::operator=(const PO_AVByMscLac& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByMscLac &left,const PO_AVByMscLac& right)
{
	return (left.recordStruct.msc==right.recordStruct.msc)&&
			(left.recordStruct.lac==right.recordStruct.lac);
}

int operator<(const PO_AVByMscLac &left,const PO_AVByMscLac& right)
{
	if(left.recordStruct.msc<right.recordStruct.msc)return true;
	if(left.recordStruct.msc>right.recordStruct.msc)return false;
	if(left.recordStruct.lac<right.recordStruct.lac)return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_AVByMscLac& po)
{
	os<<po.recordStruct.msc<<" , "<<
			po.recordStruct.lac<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByMscLac::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByMscLac",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, LAC, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		5,"select count(*) from TD_LAC"};

