//add by luoyb
#include "PO_AVByCellid.h"


bool PO_AVByCellid::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByCellid::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.cellId=tmpstr;
	
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

PO_AVByCellid::PO_AVByCellid(const PO_AVByCellid& right)
{
	*this=right;
}

void PO_AVByCellid::operator=(const PO_AVByCellid& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByCellid &left,const PO_AVByCellid& right)
{
	return (left.recordStruct.cellId==right.recordStruct.cellId);
}

int operator<(const PO_AVByCellid &left,const PO_AVByCellid& right)
{
	return (left.recordStruct.cellId<right.recordStruct.cellId);
}

ostream& operator<<(ostream& os,const PO_AVByCellid& po)
{
	os<<po.recordStruct.cellId<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByCellid::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByCellid",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select CELL_ID, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		4,"select count(*) from TD_LAC"};

