#include "PO_AVByLacCellid.h"


bool PO_AVByLacCellid::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByLacCellid::convert(const StringVector &argname)
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

PO_AVByLacCellid::PO_AVByLacCellid(const PO_AVByLacCellid& right)
{
	*this=right;
}

void PO_AVByLacCellid::operator=(const PO_AVByLacCellid& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByLacCellid &left,const PO_AVByLacCellid& right)
{
	return (left.recordStruct.lac==right.recordStruct.lac)&&
			(left.recordStruct.cellId==right.recordStruct.cellId);
}

int operator<(const PO_AVByLacCellid &left,const PO_AVByLacCellid& right)
{
	if(left.recordStruct.lac<right.recordStruct.lac)return true;
	if(left.recordStruct.lac>right.recordStruct.lac)return false;
	if(left.recordStruct.cellId<right.recordStruct.cellId)return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_AVByLacCellid& po)
{
	os<<po.recordStruct.msc<<" , "<<
			po.recordStruct.lac<<" , "<<
			po.recordStruct.cellId<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByLacCellid::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByLacCellid",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, LAC, CELL_ID, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		6,"select count(*) from TD_LAC"};

