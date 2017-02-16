#ifndef PO_AVByMscCellid_H
#define PO_AVByMscCellid_H

#include "PO_AVByMscCellid.h"


bool PO_AVByMscCellid::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AVByMscCellid::convert(const StringVector &argname)
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

PO_AVByMscCellid::PO_AVByMscCellid(const PO_AVByMscCellid& right)
{
	*this=right;
}

void PO_AVByMscCellid::operator=(const PO_AVByMscCellid& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AVByMscCellid &left,const PO_AVByMscCellid& right)
{
	return (left.recordStruct.msc==right.recordStruct.msc)&&
			(left.recordStruct.cellId==right.recordStruct.cellId);
}

int operator<(const PO_AVByMscCellid &left,const PO_AVByMscCellid& right)
{
	if(left.recordStruct.msc<right.recordStruct.msc)return true;
	if(left.recordStruct.msc>right.recordStruct.msc)return false;
	if(left.recordStruct.cellId<right.recordStruct.cellId)return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_AVByMscCellid& po)
{
	os<<po.recordStruct.msc<<" , "<<
			po.recordStruct.cellId<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode;
	return os;
}


PO_Property PO_AVByMscCellid::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AVByMscCellid",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, CELL_ID, BEGIN_TIME, END_TIME, AREA_CODE from TD_LAC",
		5,"select count(*) from TD_LAC"};

#endif /* PO_AVByMscCellid_H */
