#include "PO_EdgeLacCellId.h"

bool PO_EdgeLacCellId::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_EdgeLacCellId::convert(const StringVector &argname)
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
	recordStruct.cellId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.feeCode=tmpstr;

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
	recordStruct.edgeFlag=tmpstr[0];

	//add by yangyi  on 2006,12,02
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.visit_code=tmpstr;
	return true;
}

PO_EdgeLacCellId::PO_EdgeLacCellId(const PO_EdgeLacCellId& right)
{
	*this=right;
}

void PO_EdgeLacCellId::operator=(const PO_EdgeLacCellId& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_EdgeLacCellId &left,const PO_EdgeLacCellId& right)
{
	//将homeAreaCode的判断拿出去
	return (left.recordStruct.lac==right.recordStruct.lac)&&
			(left.recordStruct.cellId==right.recordStruct.cellId);
	/*
	return (left.recordStruct.cellId==right.recordStruct.cellId)&&
		(left.recordStruct.lac==right.recordStruct.lac)&&
		(left.recordStruct.feeCode==right.recordStruct.feeCode);
	*/
}

int operator<(const PO_EdgeLacCellId &left,const PO_EdgeLacCellId& right)
{
	if (left.recordStruct.lac<right.recordStruct.lac)
       	return true;
    if (left.recordStruct.lac>right.recordStruct.lac)
     	return false;
	if (left.recordStruct.cellId<right.recordStruct.cellId)
        return true;
    if (left.recordStruct.cellId>right.recordStruct.cellId)
        return false;
    //将homeAreaCode的判断拿出去
	/*
	if (left.recordStruct.feeCode<right.recordStruct.feeCode)
		return true;
	*/

    return false;

}

ostream& operator<<(ostream& os,const PO_EdgeLacCellId& po)
{
	os<<po.recordStruct.lac<<" , "<<
		po.recordStruct.cellId<<" , "<<
		po.recordStruct.feeCode<<" , "<<
		po.recordStruct.beginTime<<" , "<<
		po.recordStruct.endTime<<" , "<<
		po.recordStruct.visit_code<<" , "<<
		po.recordStruct.edgeFlag;
		
	return os;
}


PO_Property PO_EdgeLacCellId::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_EdgeLacCellId",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select LAC, CELL_ID, HOME_AREA_CODE,BEGIN_TIME,END_TIME,EDGE_FLAG,VISIT_AREA_CODE from TD_EDGE_ROAM ",
		7,"select count(*) from TD_EDGE_ROAM"};


