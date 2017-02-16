#include "PO_EdgeVisitCodeLacCellId.h"

bool PO_EdgeVisitCodeLacCellId::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_EdgeVisitCodeLacCellId::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_visitAreaCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_lac=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_cellId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_feeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_edgeFlag=tmpstr[0];
	
	return true;
}

PO_EdgeVisitCodeLacCellId::PO_EdgeVisitCodeLacCellId(const PO_EdgeVisitCodeLacCellId& right)
{
	*this=right;
}

void PO_EdgeVisitCodeLacCellId::operator=(const PO_EdgeVisitCodeLacCellId& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_EdgeVisitCodeLacCellId &left,const PO_EdgeVisitCodeLacCellId& right)
{
	//将homeAreaCode的判断拿出去
	return (left.recordStruct.m_visitAreaCode == right.recordStruct.m_visitAreaCode)&&
			(left.recordStruct.m_lac == right.recordStruct.m_lac)&&
			(left.recordStruct.m_cellId == right.recordStruct.m_cellId);
}

int operator<(const PO_EdgeVisitCodeLacCellId &left,const PO_EdgeVisitCodeLacCellId& right)
{
	if (left.recordStruct.m_visitAreaCode < right.recordStruct.m_visitAreaCode)
       	return true;
    if (left.recordStruct.m_visitAreaCode > right.recordStruct.m_visitAreaCode)
     	return false;
	if (left.recordStruct.m_lac < right.recordStruct.m_lac)
       	return true;
    if (left.recordStruct.m_lac > right.recordStruct.m_lac)
     	return false;
	if (left.recordStruct.m_cellId < right.recordStruct.m_cellId)
        return true;
    if (left.recordStruct.m_cellId > right.recordStruct.m_cellId)
        return false;

    return false;
}

ostream& operator<<(ostream& os,const PO_EdgeVisitCodeLacCellId& po)
{
	os<<po.recordStruct.m_visitAreaCode<<" , "<<
		po.recordStruct.m_lac<<" , "<<
		po.recordStruct.m_cellId<<" , "<<
		po.recordStruct.m_feeCode<<" , "<<
		po.recordStruct.m_beginTime<<" , "<<
		po.recordStruct.m_endTime<<" , "<<
		po.recordStruct.m_edgeFlag;
		
	return os;
}

PO_Property PO_EdgeVisitCodeLacCellId::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_EdgeVisitCodeLacCellId",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select VISIT_AREA_CODE, LAC, CELL_ID, HOME_AREA_CODE,BEGIN_TIME,END_TIME,EDGE_FLAG  from TD_EDGE_ROAM ",
		7,"select count(*) from TD_EDGE_ROAM"};

