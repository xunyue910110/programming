#include "PO_TransNumber.h"

bool PO_TransNumber::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_TransNumber::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_transNumber=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_carrierType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_transType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_numberLength=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_asp=tmpstr[0];
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_dialType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_isNoNeedTp=tmpstr[0];

	return true;
}

PO_TransNumber::PO_TransNumber(const PO_TransNumber& right)
{
	*this=right;
}

void PO_TransNumber::operator=(const PO_TransNumber& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_TransNumber &left,const PO_TransNumber& right)
{
	return 1;
}

//CHG BY XIEL 2003/07/22 倒序排 使得197300,197匹配顺序　
int operator<(const PO_TransNumber &left,const PO_TransNumber& right)
{
	return (left.recordStruct.m_transNumber > right.recordStruct.m_transNumber);
}

ostream& operator<<(ostream& os,const PO_TransNumber& po)
{
	os<<po.recordStruct.m_transNumber<<" , "<<
			po.recordStruct.m_carrierType<<" , "<<
			po.recordStruct.m_transType<<" , "<<
			po.recordStruct.m_numberLength<<" , "<<
			po.recordStruct.m_asp<<" , "<<
			po.recordStruct.m_dialType<<" , "<<
			po.recordStruct.m_isNoNeedTp;
	return os;
}


PO_Property PO_TransNumber::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_TransNumber",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select TRANS_NUMBER, CARRIER_TYPE, TRANS_TYPE, NUMBER_LENGTH, ASP, DIALTYPE, ISNONEEDTP from TD_TRANS_NUMBER WHERE ASP_LIMIT = '0' ",
		7,"select count(*) from TD_TRANS_NUMBER"};

///继承类
bool PO_TransNumber_CNC::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

PO_Property PO_TransNumber_CNC::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_TransNumber_CNC",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select TRANS_NUMBER, CARRIER_TYPE, TRANS_TYPE, NUMBER_LENGTH, ASP, DIALTYPE, ISNONEEDTP from TD_TRANS_NUMBER WHERE ASP_LIMIT <> '0' ",
		7,"select count(*) from TD_TRANS_NUMBER"};

