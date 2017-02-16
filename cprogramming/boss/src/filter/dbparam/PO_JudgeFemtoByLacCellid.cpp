#include "PO_JudgeFemtoByLacCellid.h"


bool PO_JudgeFemtoByLacCellid::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_JudgeFemtoByLacCellid::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	//-1 lac
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.lac=tmpstr;

	//2-cell_id
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.cellId=tmpstr;

	//3-生效时间
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	//4-失效时间
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;

	//5-FEMTO归属地
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.visitAreaCode=tmpstr;
	
	//6-服务代码 03：FEMTO
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serviceType=tmpstr;

	return true;
}

PO_JudgeFemtoByLacCellid::PO_JudgeFemtoByLacCellid(const PO_JudgeFemtoByLacCellid& right)
{
	*this=right;
}

void PO_JudgeFemtoByLacCellid::operator=(const PO_JudgeFemtoByLacCellid& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_JudgeFemtoByLacCellid &left,const PO_JudgeFemtoByLacCellid& right)
{
	//right为传入的参数
	return (left.recordStruct.lac==right.recordStruct.lac || left.recordStruct.lac=="*" || right.recordStruct.lac=="*");
}

int operator<(const PO_JudgeFemtoByLacCellid &left,const PO_JudgeFemtoByLacCellid& right)
{
	if(left.recordStruct.lac<right.recordStruct.lac || left.recordStruct.lac =="*" || right.recordStruct.lac =="*")
		return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_JudgeFemtoByLacCellid& po)
{
	os<<po.recordStruct.lac<<" , "<<
			po.recordStruct.cellId<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.visitAreaCode<<" , "<<
			po.recordStruct.serviceType;
	return os;
}


PO_Property PO_JudgeFemtoByLacCellid::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_JudgeFemtoByLacCellid",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT LAC, CELL_ID, BEGIN_TIME, END_TIME, AREA_CODE, SERVICE_TYPE FROM TD_LAC_FEMTO",
		6,"select count(*) from TD_LAC_FEMTO"};
