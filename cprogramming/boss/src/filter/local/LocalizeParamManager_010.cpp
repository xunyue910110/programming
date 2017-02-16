#include "LocalizeParamManager_010.h"

//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_SpServCodeSpname);

bool PO_SpServCodeSpname::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_SpServCodeSpname::convert(const StringVector &argname)
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
	recordStruct.sm_typestat=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.priority=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.sp_name=tmpstr;
	
	return true;
	
}


PO_SpServCodeSpname::PO_SpServCodeSpname(const PO_SpServCodeSpname& right)
{
	*this=right;
}


void PO_SpServCodeSpname::operator=(const PO_SpServCodeSpname& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SpServCodeSpname &left,const PO_SpServCodeSpname& right)
{
	return(strcmp(left.recordStruct.sp_code.c_str(),right.recordStruct.sp_code.c_str())==0);
}

int operator<(const PO_SpServCodeSpname &left,const PO_SpServCodeSpname& right)
{
	return left.recordStruct.sp_code < right.recordStruct.sp_code;
}

ostream& operator<<(ostream& os,const PO_SpServCodeSpname& po)
{
	os<<po.recordStruct.prov_code     <<","<<
		po.recordStruct.sp_code       <<","<<
		po.recordStruct.oper_code     <<","<<
		po.recordStruct.service_code  <<","<<
		po.recordStruct.call_type     <<","<<
		po.recordStruct.serv_type     <<","<<
		po.recordStruct.serv_code_type<<","<<
		po.recordStruct.home_type     <<","<<
		po.recordStruct.sp_type       <<","<<
		po.recordStruct.asp           <<","<<
		po.recordStruct.sp_serv_area  <<","<<
		po.recordStruct.begin_time    <<","<<
		po.recordStruct.end_time      <<","<<
		po.recordStruct.priority      <<","<<
		po.recordStruct.sp_name       <<","<<
		po.recordStruct.sm_typestat;
	
	return os;
	
}

PO_Property PO_SpServCodeSpname::m_property={FROM_DATABASE,BY_FLAG,
"","PO_SpServCodeSpname",USERNAME_1,PASSWORD_1,SERVNAME_1,
"select PROV_CODE,SP_CODE,OPER_CODE,SERVICE_CODE,CALL_TYPE,SERV_TYPE,SERV_CODE_TYPE,HOME_TYPE,SP_TYPE,ASP,IS_VERIFYSTAE,IS_VERIFYORDER,SP_SERV_AREA,BEGIN_TIME,END_TIME,SM_TYPESTAT,PRIORITY,SP_NAME from TD_SP_SERV_CODE",
18,"select count(*) from TD_SP_SERV_CODE"};


//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_ServiceCodeSpname);

bool PO_ServiceCodeSpname::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_ServiceCodeSpname::convert(const StringVector &argname)
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
	recordStruct.sm_typestat=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.priority=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.sp_name=tmpstr;
	
	return true;
	
}


PO_ServiceCodeSpname::PO_ServiceCodeSpname(const PO_ServiceCodeSpname& right)
{
	*this=right;
}


void PO_ServiceCodeSpname::operator=(const PO_ServiceCodeSpname& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_ServiceCodeSpname &left,const PO_ServiceCodeSpname& right)
{
	return(strcmp(left.recordStruct.service_code.c_str(),right.recordStruct.service_code.c_str())==0);
}

int operator<(const PO_ServiceCodeSpname &left,const PO_ServiceCodeSpname& right)
{
	
	return left.recordStruct.service_code < right.recordStruct.service_code;
}


ostream& operator<<(ostream& os,const PO_ServiceCodeSpname& po)
{
	os<<po.recordStruct.prov_code     <<","<<
		po.recordStruct.sp_code       <<","<<
		po.recordStruct.oper_code     <<","<<
		po.recordStruct.service_code  <<","<<
		po.recordStruct.call_type     <<","<<
		po.recordStruct.serv_type     <<","<<
		po.recordStruct.serv_code_type<<","<<
		po.recordStruct.home_type     <<","<<
		po.recordStruct.sp_type       <<","<<
		po.recordStruct.asp           <<","<<
		po.recordStruct.sp_serv_area  <<","<<
		po.recordStruct.begin_time    <<","<<
		po.recordStruct.end_time      <<","<<
		po.recordStruct.priority      <<","<<
		po.recordStruct.sp_name       <<","<<
		po.recordStruct.sm_typestat;
	
	return os;
	
}

PO_Property PO_ServiceCodeSpname::m_property={FROM_DATABASE,BY_FLAG,
"","PO_ServiceCodeSpname",USERNAME_1,PASSWORD_1,SERVNAME_1,
"select PROV_CODE,SP_CODE,OPER_CODE,SERVICE_CODE,CALL_TYPE,SERV_TYPE,SERV_CODE_TYPE,HOME_TYPE,SP_TYPE,ASP,IS_VERIFYSTAE,IS_VERIFYORDER,SP_SERV_AREA,BEGIN_TIME,END_TIME,SM_TYPESTAT,PRIORITY,SP_NAME from TD_SP_SERV_CODE",
18,"select count(*) from TD_SP_SERV_CODE"};


//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_SpBillRule);

bool PO_SpBillRule::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_SpBillRule::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_source=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_spcode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sp_srvtype=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_billflag=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;
	
	return true;
}

PO_SpBillRule::PO_SpBillRule(const PO_SpBillRule& right)
{
	*this=right;
}

void PO_SpBillRule::operator=(const PO_SpBillRule& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SpBillRule &left,const PO_SpBillRule& right)
{
	return (left.recordStruct.m_source==right.recordStruct.m_source)&&
		(left.recordStruct.m_spcode==right.recordStruct.m_spcode);
}

int operator<(const PO_SpBillRule &left,const PO_SpBillRule& right)
{
	if(left.recordStruct.m_source == right.recordStruct.m_source)
	{
		return (left.recordStruct.m_spcode < right.recordStruct.m_spcode);
	}
	else
	{
		return (left.recordStruct.m_source < right.recordStruct.m_source);
	}
}

ostream& operator<<(ostream& os,const PO_SpBillRule& po)
{
	os<<po.recordStruct.m_source     <<" , "<<
		po.recordStruct.m_spcode     <<" , "<<
		po.recordStruct.m_sp_srvtype <<" , "<<
		po.recordStruct.m_billflag   <<" , "<<
		po.recordStruct.beginTime    <<" , "<<
		po.recordStruct.endTime;
	return os;
}

PO_Property PO_SpBillRule::m_property={FROM_DATABASE,BY_FLAG,
"","PO_SpBillRule",USERNAME_1,PASSWORD_1,SERVNAME_1,
"select SOURCE_TYPE,SP_CODE,SP_SRVTYPE,BILL_FLAG,BEGIN_TIME,END_TIME from TD_SP_BILLRULE",
6,"select count(*) from TD_SP_BILLRULE"};


//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_RegularFixCalled);

bool PO_RegularFixCalled::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_RegularFixCalled::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msc=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_calledareacode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_calledprefix=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;
	
	return true;
}

PO_RegularFixCalled::PO_RegularFixCalled(const PO_RegularFixCalled& right)
{
	*this=right;
}

void PO_RegularFixCalled::operator=(const PO_RegularFixCalled& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_RegularFixCalled &left,const PO_RegularFixCalled& right)
{
	return 1;
}

int operator<(const PO_RegularFixCalled &left,const PO_RegularFixCalled& right)
{
	return (left.recordStruct.m_calledprefix > right.recordStruct.m_calledprefix);
}

ostream& operator<<(ostream& os,const PO_RegularFixCalled& po)
{
	os<<po.recordStruct.m_msc          <<" , "<<
		po.recordStruct.m_calledareacode <<" , "<<
		po.recordStruct.m_calledprefix <<" , "<<
		po.recordStruct.beginTime      <<" , "<<
		po.recordStruct.endTime;
	return os;
}

PO_Property PO_RegularFixCalled::m_property={FROM_DATABASE,BY_FLAG,
"","PO_RegularFixCalled",USERNAME_1,PASSWORD_1,SERVNAME_1,
"select MSC, CALLED_AREA_CODE, CALLED_PHONE_PRE, BEGIN_TIME, END_TIME from TD_REGULAR_FIXCALLED",
5,"select count(*) from TD_REGULAR_FIXCALLED"};

//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_NmFixTrashPtp);

bool PO_NmFixTrashPtp::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_NmFixTrashPtp::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msc=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phone=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;
	
	return true;
}

PO_NmFixTrashPtp::PO_NmFixTrashPtp(const PO_NmFixTrashPtp& right)
{
	*this=right;
}

void PO_NmFixTrashPtp::operator=(const PO_NmFixTrashPtp& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_NmFixTrashPtp &left,const PO_NmFixTrashPtp& right)
{
	return (left.recordStruct.m_phone == right.recordStruct.m_phone);
}

int operator<(const PO_NmFixTrashPtp &left,const PO_NmFixTrashPtp& right)
{
	return (left.recordStruct.m_phone < right.recordStruct.m_phone);
}

ostream& operator<<(ostream& os,const PO_NmFixTrashPtp& po)
{
	os<<po.recordStruct.m_msc       <<" , "<<
		po.recordStruct.m_phone     <<" , "<<
		po.recordStruct.m_groupId   <<" , "<<
		po.recordStruct.beginTime   <<" , "<<
		po.recordStruct.endTime;
	return os;
}

PO_Property PO_NmFixTrashPtp::m_property={FROM_DATABASE,BY_FLAG,
"","PO_NmFixTrashPtp",USERNAME_1,PASSWORD_1,SERVNAME_1,
"select MSC, AREA_CODE||PHONE, GROUP_ID, BEGIN_TIME, END_TIME from TD_NM_FIXTRASH_PTP",
5,"select count(*) from TD_NM_FIXTRASH_PTP"};


// Begin add by xiezq 20101207 for 集团BSSV4.1 一卡双号需求
// 一卡双号MSISDN
INSTALL_LPARAMCONTAINER_INSTANCE(PO_MsisdnHk);

bool PO_MsisdnHk::getProperty(PO_Property& property)
{
	property = m_property;
	return true;
}

bool PO_MsisdnHk::convert(const StringVector& argname)
{
	int i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.begin_phone = tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.end_phone = tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.usertype = tmpstr;
		
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.beginDate = tmpstr;

    tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.endDate = tmpstr;

	return true;
}

PO_MsisdnHk::PO_MsisdnHk(const PO_MsisdnHk& rhs)
{
	*this = rhs;
}

void PO_MsisdnHk::operator=(const PO_MsisdnHk& rhs)
{
	recordStruct = rhs.recordStruct;
}

int operator==(const PO_MsisdnHk& lhs, const PO_MsisdnHk& rhs)
{
	return ((lhs.recordStruct.begin_phone >= rhs.recordStruct.begin_phone) && 
			(lhs.recordStruct.end_phone <= rhs.recordStruct.end_phone)) || 
			((lhs.recordStruct.begin_phone <= rhs.recordStruct.begin_phone) && 
			(lhs.recordStruct.end_phone >= rhs.recordStruct.end_phone));
}

int operator<(const PO_MsisdnHk& lhs, const PO_MsisdnHk& rhs)
{
	return (lhs.recordStruct.begin_phone < rhs.recordStruct.begin_phone) && 
			(lhs.recordStruct.end_phone < rhs.recordStruct.end_phone);
}

ostream& operator<<(ostream& os, const PO_MsisdnHk& po)
{
	os << po.recordStruct.begin_phone << ","
	   << po.recordStruct.end_phone << ","
	   << po.recordStruct.usertype << ","
	   << po.recordStruct.beginDate << ","
	   << po.recordStruct.endDate;
	    
	return os;
}


PO_Property PO_MsisdnHk::m_property={FROM_DATABASE, BY_FLAG,
		"", "PO_MsisdnHk", USERNAME_1, PASSWORD_1, SERVNAME_1,
		"SELECT begin_msisdn, end_msisdn, user_type, begin_time, end_time FROM TD_MSISDN_HK",
		5,"SELECT count(*) FROM TD_MSISDN_HK"};


//////////////////////////////////////////////////////////////////////////
// 一卡双号IMSI
INSTALL_LPARAMCONTAINER_INSTANCE(PO_ImsiHk);

bool PO_ImsiHk::getProperty(PO_Property& property)
{
	property=m_property;
	return true;
}

bool PO_ImsiHk::convert(const StringVector& argname)
{
	int i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.begin_imsi = tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.end_imsi = tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.usertype = tmpstr;
	
	tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.beginDate = tmpstr;
	
    tmpstr = *(argname.begin() + i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r") + 1, tmpstr.npos);
	recordStruct.endDate = tmpstr;
	
	return true;
}         

PO_ImsiHk::PO_ImsiHk(const PO_ImsiHk& rhs)
{
	*this = rhs;
}

void PO_ImsiHk::operator=(const PO_ImsiHk& rhs)
{
	recordStruct = rhs.recordStruct;
}

int operator==(const PO_ImsiHk& lhs, const PO_ImsiHk& rhs)
{
	return((lhs.recordStruct.begin_imsi >= rhs.recordStruct.begin_imsi) && 
			(lhs.recordStruct.end_imsi <= rhs.recordStruct.end_imsi)) || 
			((lhs.recordStruct.begin_imsi <= rhs.recordStruct.begin_imsi) && 
			(lhs.recordStruct.end_imsi >= rhs.recordStruct.end_imsi));
}

int operator<(const PO_ImsiHk& lhs, const PO_ImsiHk& rhs)
{
	return(lhs.recordStruct.begin_imsi < rhs.recordStruct.begin_imsi) && 
			(lhs.recordStruct.end_imsi < rhs.recordStruct.end_imsi);
}

ostream& operator<< (ostream& os, const PO_ImsiHk& po)
{
	os << po.recordStruct.begin_imsi << "," 
	   << po.recordStruct.end_imsi << "," 
	   << po.recordStruct.usertype << "," 
	   << po.recordStruct.beginDate << "," 
	   << po.recordStruct.endDate;
	
	return os;
}

PO_Property PO_ImsiHk::m_property={FROM_DATABASE, BY_FLAG,
		"", "PO_ImsiHk", USERNAME_1, PASSWORD_1, SERVNAME_1,
		"SELECT begin_imsi, end_imsi, user_type, begin_time, end_time FROM TD_IMSI_HK",
		5,"SELECT count(*) FROM TD_IMSI_HK"};

// End add by xiezq 20101207 for 集团BSSV4.1 一卡双号需求
