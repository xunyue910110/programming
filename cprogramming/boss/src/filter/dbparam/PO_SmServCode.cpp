#include "PO_SmServCode.h"

bool PO_SmServCode::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
bool PO_SmServCode::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.number=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.match_type=tmpstr[0];
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serv_type=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serv_code_type=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.home_type=tmpstr[0];
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.asp=tmpstr[0];
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.call_type=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.begin_time=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.end_time=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.sm_typestat=tmpstr;
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.priority=atoi(tmpstr.c_str());
	return true;
}

PO_SmServCode::PO_SmServCode(const PO_SmServCode& right)
{
	*this=right;
}

void PO_SmServCode::operator=(const PO_SmServCode& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SmServCode &left,const PO_SmServCode& right)
{
	return strncmp(left.recordStruct.number.c_str(),right.recordStruct.number.c_str(),
	               left.recordStruct.number.length())==0;
}

int operator<(const PO_SmServCode &left,const PO_SmServCode& right)
{
    //if(strncmp(left.recordStruct.number.c_str(),right.recordStruct.number.c_str(),
    //           left.recordStruct.number.length())<=0) return false;
    //return true;
    if (left.recordStruct.priority==right.recordStruct.priority)
        return strcmp(left.recordStruct.number.c_str(),right.recordStruct.number.c_str())>0;
    else 
        return left.recordStruct.priority>right.recordStruct.priority;
    
}

ostream& operator<<(ostream& os,const PO_SmServCode& po)
{
	os<<po.recordStruct.number   <<","<<
	    po.recordStruct.match_type    <<","<<
	    po.recordStruct.serv_type     <<","<<
	    po.recordStruct.serv_code_type<<","<<
	    po.recordStruct.home_type     <<","<<
	    po.recordStruct.asp           <<","<<
	    po.recordStruct.call_type     <<","<<
	    po.recordStruct.begin_time    <<","<<
	    po.recordStruct.end_time      <<","<<
	    po.recordStruct.sm_typestat   <<","<<
	    po.recordStruct.priority   ;
	return os;
}


PO_Property PO_SmServCode::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SmServCode",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select dial_number,match_type,serv_type,serv_code_type,home_type,asp,call_type,begin_time,end_time,sm_typestat,priority from TD_SM_SERV_CODE",
		11,"select count(*) from TD_SM_SERV_CODE"};

