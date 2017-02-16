#include "PO_AVByMscLacCellid.h"


bool PO_AVByMscLacCellid::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByMscLacCellid::convert(const StringVector &argname)
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

PO_AVByMscLacCellid::PO_AVByMscLacCellid(const PO_AVByMscLacCellid& right)
{
	*this=right;
}

void PO_AVByMscLacCellid::operator=(const PO_AVByMscLacCellid& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByMscLacCellid &left,const PO_AVByMscLacCellid& right)
{
	return (left.recordStruct.msc==right.recordStruct.msc)&&
			(left.recordStruct.lac==right.recordStruct.lac)&&
			(left.recordStruct.cellId==right.recordStruct.cellId);
}

int operator<(const PO_AVByMscLacCellid &left,const PO_AVByMscLacCellid& right)
{
	if(left.recordStruct.msc<right.recordStruct.msc)return true;
	if(left.recordStruct.msc>right.recordStruct.msc)return false;
	if(left.recordStruct.lac<right.recordStruct.lac)return true;
	if(left.recordStruct.lac>right.recordStruct.lac)return false;
	if(left.recordStruct.cellId<right.recordStruct.cellId)return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_AVByMscLacCellid& po)
{
	os<<po.recordStruct.msc<<" , "<<
			po.recordStruct.lac<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.cellId<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByMscLacCellid::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByMscLacCellid",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, LAC, CELL_ID, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		6,"select count(*) from TD_LAC"};

