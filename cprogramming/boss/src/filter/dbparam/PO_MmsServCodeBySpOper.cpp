#include "PO_MmsServCodeBySpOper.h"

bool PO_MmsServCodeBySpOper::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_MmsServCodeBySpOper::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.prov_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.sp_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.oper_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.service_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.call_type=tmpstr;

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
	recordStruct.sp_type=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.asp=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.is_verifystae=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.is_verifyorder=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.sp_serv_area=tmpstr[0];

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
	recordStruct.mms_typestat=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.priority=atoi(tmpstr.c_str());

	return true;
}

PO_MmsServCodeBySpOper::PO_MmsServCodeBySpOper(const PO_MmsServCodeBySpOper& right)
{
	*this=right;
}

void PO_MmsServCodeBySpOper::operator=(const PO_MmsServCodeBySpOper& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_MmsServCodeBySpOper &left,const PO_MmsServCodeBySpOper& right)
{
	if( (strncmp(left.recordStruct.sp_code.c_str(),right.recordStruct.sp_code.c_str(),
	               right.recordStruct.sp_code.length())==0) &&
	    (strcmp(left.recordStruct.oper_code.c_str(),right.recordStruct.oper_code.c_str())==0 ||
	     right.recordStruct.oper_code=="*" ) &&
	    (strcmp(left.recordStruct.call_type.c_str(),right.recordStruct.call_type.c_str())==0 ||
	     right.recordStruct.call_type=="*" ) )
        return true;
    else
        return false;        
}

int operator<(const PO_MmsServCodeBySpOper &left,const PO_MmsServCodeBySpOper& right)
{
    if (left.recordStruct.priority==right.recordStruct.priority)
    {
        if( strcmp(left.recordStruct.sp_code.c_str(),right.recordStruct.sp_code.c_str())==0)
        {
            if (strcmp(left.recordStruct.oper_code.c_str(),right.recordStruct.oper_code.c_str())==0)
                return strcmp(left.recordStruct.call_type.c_str(),right.recordStruct.call_type.c_str())>0;
            else
                return strcmp(left.recordStruct.call_type.c_str(),right.recordStruct.call_type.c_str())>0;
        }
        else
            return strcmp(left.recordStruct.sp_code.c_str(),right.recordStruct.sp_code.c_str())>0;
    }
    else 
        return left.recordStruct.priority>right.recordStruct.priority;
}

ostream& operator<<(ostream& os,const PO_MmsServCodeBySpOper& po)
{
	os<<po.recordStruct.prov_code     <<","<<
	    po.recordStruct.sp_code       <<","<<
	    po.recordStruct.service_code  <<","<<
	    po.recordStruct.serv_type     <<","<<
	    po.recordStruct.serv_code_type<<","<<
	    po.recordStruct.home_type     <<","<<
	    po.recordStruct.asp           <<","<<
	    po.recordStruct.sp_serv_area  <<","<<
	    po.recordStruct.begin_time    <<","<<
	    po.recordStruct.end_time      <<","<<
	    po.recordStruct.priority      <<","<<
	    po.recordStruct.mms_typestat;

	return os;
}

PO_Property PO_MmsServCodeBySpOper::m_property={FROM_DATABASE,BY_FLAG,

		"","PO_MmsServCodeBySpOper",USERNAME_1,PASSWORD_1,SERVNAME_1,

		"select PROV_CODE,SP_CODE,OPER_CODE,SERVICE_CODE,CALL_TYPE,SERV_TYPE,SERV_CODE_TYPE,HOME_TYPE,SP_TYPE,ASP,IS_VERIFYSTAE,IS_VERIFYORDER,SP_SERV_AREA,BEGIN_TIME,END_TIME,MMS_TYPESTAT,PRIORITY from TD_MMS_SERV_CODE",
		17,"select count(*) from TD_MMS_SERV_CODE"};



