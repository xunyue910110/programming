#include "LocalizeParamManager_019.h"

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
INSTALL_LPARAMCONTAINER_INSTANCE(PO_TrunkUser);

bool PO_TrunkUser::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
bool PO_TrunkUser::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.begin_phone=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.end_phone=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.usertype=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.area_code=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginDate=tmpstr;

  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endDate=tmpstr;

	return true;
}

PO_TrunkUser::PO_TrunkUser(const PO_TrunkUser& right)
{
	*this=right;
}

void PO_TrunkUser::operator=(const PO_TrunkUser& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_TrunkUser &left,const PO_TrunkUser& right)
{
	return ((left.recordStruct.begin_phone>=right.recordStruct.begin_phone)&&
			(left.recordStruct.end_phone<=right.recordStruct.end_phone))||
			((left.recordStruct.begin_phone<=right.recordStruct.begin_phone)&&
			(left.recordStruct.end_phone>=right.recordStruct.end_phone));
}

int operator<(const PO_TrunkUser &left,const PO_TrunkUser& right)
{
	return (left.recordStruct.begin_phone<right.recordStruct.begin_phone)&&
			(left.recordStruct.end_phone<right.recordStruct.end_phone);
}

ostream& operator<<(ostream& os,const PO_TrunkUser& po)
{
	os<<po.recordStruct.begin_phone <<","<<
	    po.recordStruct.end_phone  <<","<<
	    po.recordStruct.usertype<<","<<
	    po.recordStruct.area_code<<","<<
	    po.recordStruct.m_beginDate <<","<<
	    po.recordStruct.m_endDate;
	    
	return os;
}


PO_Property PO_TrunkUser::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_TrunkUser",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select begin_phone, end_phone, user_type, area_code, begin_time, end_time from TD_TRUNK_USER",
		6,"select count(*) from TD_TRUNK_USER"};

//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_BpsPstnNumseg);

bool PO_BpsPstnNumseg::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_BpsPstnNumseg::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areacode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_numberhead=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_businessareacode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_operatorid=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_nettype=tmpstr;

	return true;
}

PO_BpsPstnNumseg::PO_BpsPstnNumseg(const PO_BpsPstnNumseg& right)
{
	*this=right;
}

void PO_BpsPstnNumseg::operator=(const PO_BpsPstnNumseg& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_BpsPstnNumseg &left,const PO_BpsPstnNumseg& right)
{
	return (left.recordStruct.m_numberhead==right.recordStruct.m_numberhead) &&
	       (left.recordStruct.m_areacode==right.recordStruct.m_areacode);
}

int operator<(const PO_BpsPstnNumseg &left,const PO_BpsPstnNumseg& right)
{
	if(left.recordStruct.m_areacode == right.recordStruct.m_areacode)
	{
		return (left.recordStruct.m_numberhead < right.recordStruct.m_numberhead);
	}
	else
	{
		return (left.recordStruct.m_areacode < right.recordStruct.m_areacode);
	}
}

ostream& operator<<(ostream& os,const PO_BpsPstnNumseg& po)
{
	os<<po.recordStruct.m_areacode<<" , "<<
			po.recordStruct.m_numberhead<<" , "<<
			po.recordStruct.m_businessareacode<<" , "<<
			po.recordStruct.m_operatorid<<" , "<<
			po.recordStruct.m_nettype;
	return os;
}

PO_Property PO_BpsPstnNumseg::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_BpsPstnNumseg",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_CODE,NUMBER_HEAD,BUSINESS_AREA_CODE,OPERATOR_ID,NET_TYPE from TD_BPS_PSTN_NUMSEG",
		5,"select count(*) from TD_BPS_PSTN_NUMSEG"};


//////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_BpsRelation);

bool PO_BpsRelation::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_BpsRelation::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areacode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_businessareacode1=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_businessareacode2=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_connecttype=tmpstr;


	return true;
}

PO_BpsRelation::PO_BpsRelation(const PO_BpsRelation& right)
{
	*this=right;
}

void PO_BpsRelation::operator=(const PO_BpsRelation& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_BpsRelation &left,const PO_BpsRelation& right)
{
	return (left.recordStruct.m_areacode==right.recordStruct.m_areacode)&&
			(left.recordStruct.m_businessareacode1==right.recordStruct.m_businessareacode1)&&
			(left.recordStruct.m_businessareacode2==right.recordStruct.m_businessareacode2);
}

int operator<(const PO_BpsRelation &left,const PO_BpsRelation& right)
{

	if(left.recordStruct.m_areacode<right.recordStruct.m_areacode)return true;
	if(left.recordStruct.m_areacode>right.recordStruct.m_areacode)return false;
	if(left.recordStruct.m_businessareacode1<right.recordStruct.m_businessareacode1)return true;
	if(left.recordStruct.m_businessareacode1>right.recordStruct.m_businessareacode1)return false;
	if(left.recordStruct.m_businessareacode2<right.recordStruct.m_businessareacode2)return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_BpsRelation& po)
{
	os<<po.recordStruct.m_areacode<<" , "<<
			po.recordStruct.m_businessareacode1<<" , "<<
			po.recordStruct.m_businessareacode2<<" , "<<
			po.recordStruct.m_connecttype;
	return os;
}

PO_Property PO_BpsRelation::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_BpsRelation",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_CODE,BUSINESS_AREA_CODE1,BUSINESS_AREA_CODE2,CONNECT_TYPE from TD_BPS_BUSINESS_AREA_REL",
		4,"select count(*) from TD_BPS_BUSINESS_AREA_REL"};

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
INSTALL_LPARAMCONTAINER_INSTANCE(PO_LocalSpOperFee);

bool PO_LocalSpOperFee::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}
  
bool PO_LocalSpOperFee::convert(const StringVector &argname)
{
    int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.spCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serviceCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.operCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.feeType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.infoFee=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.infoCheckType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.giftFee=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.giftCheckType=tmpstr[0];

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
	recordStruct.sp_serv_area=tmpstr[0];
	return true;
}

PO_LocalSpOperFee::PO_LocalSpOperFee(const PO_LocalSpOperFee& right)
{
    *this = right;
}

void PO_LocalSpOperFee::operator=(const PO_LocalSpOperFee& right)
{
    recordStruct = right.recordStruct;
}

int operator==(const PO_LocalSpOperFee &left,const PO_LocalSpOperFee& right)
{
   return (left.recordStruct.spCode==right.recordStruct.spCode);
}

int operator<(const PO_LocalSpOperFee &left,const PO_LocalSpOperFee& right)
{
	return (left.recordStruct.spCode<right.recordStruct.spCode);
		
}

ostream& operator<<(ostream& os,const PO_LocalSpOperFee& po)
{
    os<<po.recordStruct.spCode     <<" , "
      <<po.recordStruct.serviceCode<<" , "
      <<po.recordStruct.operCode   <<" , "
      <<po.recordStruct.feeType   <<" , "
      <<po.recordStruct.infoFee   <<" , "
      <<po.recordStruct.infoCheckType   <<" , "
      <<po.recordStruct.giftFee   <<" , "
      <<po.recordStruct.giftCheckType   <<" , "
      <<po.recordStruct.sp_serv_area    <<" , "
      <<po.recordStruct.m_beginTime	<< " , "
      <<po.recordStruct.m_endTime << endl;
    return os;
}

PO_Property PO_LocalSpOperFee::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_LocalSpOperFee",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select SP_CODE,SERVICE_CODE,OPER_CODE,FEE_TYPE,INFO_FEE,INFO_CHECK_TYPE,GIFT_FEE,GIFT_CHECK_TYPE,BEGIN_TIME,END_TIME,SP_TYPE from TD_SP_MON_OPER",
		11,"select count(*) from TD_SP_MON_OPER"};
////////////////////////////////////////////////////////////////////////


bool Mdb_UserMemberIvpn::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserMemberIvpn::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[64+1];
 	session->getColumnValue("user_id"           ,&t_longValue,t_length);m_record.m_userId=t_longValue;
 	session->getColumnValue("member_role_code"  ,&t_intValue ,t_length);m_record.m_memberType=t_intValue;
 	session->getColumnValue("member_role_type"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberIdType=t_strValue;
 	session->getColumnValue("member_role_id"    ,&t_longValue,t_length);m_record.m_memberId=t_longValue;
 	session->getColumnValue("member_role_number",&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberNumber=t_strValue;
 	session->getColumnValue("start_date"        ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;
 	session->getColumnValue("end_date"          ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	
 	return true;
}

void Mdb_UserMemberIvpn::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)&m_record.m_memberNumber);
}

void Mdb_UserMemberIvpn::setOtherParams(InputParams *pInputParamsseOther) const
{
  pInputParamsseOther->setValue(VAR_TYPE_VSTR,(void *)&m_record.m_memberType);	
}

Mdb_UserMemberIvpn::Mdb_UserMemberIvpn(const Mdb_UserMemberIvpn& right)
{
	*this=right;
}

void Mdb_UserMemberIvpn::operator=(const Mdb_UserMemberIvpn& right)
{
	m_record=right.m_record;
}

int operator<(const Mdb_UserMemberIvpn &left,const Mdb_UserMemberIvpn& right)
{
	return (left.m_record.m_beginTime>right.m_record.m_beginTime);
}

ostream& operator<<(ostream& os,const Mdb_UserMemberIvpn &po)
{
	os << po.m_record.m_userId      << " , "
	   << po.m_record.m_memberType  << " , "
	   << po.m_record.m_memberIdType<< " , "
	   << po.m_record.m_memberId    << " , "
	   << po.m_record.m_memberNumber<< " , "
	   << po.m_record.m_beginTime   << " , "
	   << po.m_record.m_endTime     << " , ";
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserMemberIvpn::m_property={"tf_f_user_member","IDX_TF_F_USER_MEMBER_NUMBER",1,"",0};


////////////////////////////////////////////////////////////////

bool Mdb_UserInfoSN::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserInfoSN::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];

 	session->getColumnValue("net_type_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_net_type_code = t_strTmp;
 	         
 	//wujg add 20090531
	session->getColumnValue("user_type_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_user_type_code = t_strTmp;	         

	session->getColumnValue("eparchy_code",t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_eparchy_code = t_strTmp;

 	session->getColumnValue("city_code"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_office_code = t_strTmp;

	session->getColumnValue("cust_id"     ,&t_longValue,t_length);m_record.m_custId = t_longValue;

  session->getColumnValue("brand_code"  ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_brand_code = t_strTmp;

  session->getColumnValue("open_mode"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_open_mode = t_strTmp;

  session->getColumnValue("acct_tag"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_acct_tag = t_strTmp;

 	session->getColumnValue("product_id"  ,&t_intValue,t_length);m_record.m_productId = t_intValue;

  session->getColumnValue("in_date"     ,&t_longValue,t_length);m_record.m_inTime = t_longValue;

 	session->getColumnValue("open_date"   ,&t_longValue,t_length);m_record.m_openTime = t_longValue;
 	return true;
}

void Mdb_UserInfoSN::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)m_record.m_sn.c_str());
}

void Mdb_UserInfoSN::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserInfoSN::Mdb_UserInfoSN(const Mdb_UserInfoSN& right)
{
	*this=right;
}

void Mdb_UserInfoSN::operator=(const Mdb_UserInfoSN& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserInfoSN &po)
{
	os << po.m_record.m_sn            << " , "
	   << po.m_record.m_net_type_code << " , "
	   << po.m_record.m_user_type_code << " , " //wujg add 20090531
	   << po.m_record.m_eparchy_code  << " , "
	   << po.m_record.m_office_code   << " , "
	   << po.m_record.m_custId        << " , "
	   << po.m_record.m_brand_code    << " , "
	   << po.m_record.m_open_mode     << " , "
	   << po.m_record.m_acct_tag      << " , "
	   << po.m_record.m_productId     << " , "
	   << po.m_record.m_inTime        << " , "
	   << po.m_record.m_openTime;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserInfoSN::m_property={"tf_f_user","IDX_TF_F_USER_MSISDN",1,"",0};

////////////////////////////////////////////////////////////////////////
INSTALL_LPARAMCONTAINER_INSTANCE(PO_LocalCdrType);

bool PO_LocalCdrType::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}

bool PO_LocalCdrType::convert(const StringVector &argname)
{
  int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.nRecord_type=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_priority=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_bizType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sourceType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_trunkGroupIn=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_callType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_userAspB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_UserServiceTypeB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_paType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_