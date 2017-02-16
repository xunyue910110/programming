#include "PO_B_CompCond.h"

bool PO_B_CompCond::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_B_CompCond::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_condId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_orderNo=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_andOrTag=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_subCondId=atoi(tmpstr.c_str());

	recordStruct.m_condRule = 0;
	recordStruct.m_next     = 0;
	return true;
}

PO_B_CompCond::PO_B_CompCond(const PO_B_CompCond& right)
{
	*this=right;
}

void PO_B_CompCond::operator=(const PO_B_CompCond& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_B_CompCond &left,const PO_B_CompCond& right)
{
	return left.recordStruct.m_condId==right.recordStruct.m_condId;
}

int operator<(const PO_B_CompCond &left,const PO_B_CompCond& right)
{
	if(left.recordStruct.m_condId<right.recordStruct.m_condId) return true;
	if(left.recordStruct.m_condId>right.recordStruct.m_condId) return false;
	if(left.recordStruct.m_orderNo<right.recordStruct.m_orderNo) return true;
	//return (left.recordStruct.m_condId<right.recordStruct.m_condId);
	return false;
}

ostream& operator<<(ostream& os,const PO_B_CompCond& po)
{
	os<<po.recordStruct.m_condId<<" , "<<
			po.recordStruct.m_orderNo<<" , "<<
			po.recordStruct.m_andOrTag<<" , "<<
			po.recordStruct.m_subCondId;
	return os;
}


PO_Property PO_B_CompCond::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_B_CompCond",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select COND_ID, ORDER_NO, AND_OR_TAG, SUB_COND_ID from TD_B_COMPCOND",
		4,"select count(*) from TD_B_COMPCOND"};

