#include "PO_SysParam.h"

bool PO_SysParam::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_SysParam::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_sysParamCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_channelNo=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_valueN=atol(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_valueS=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_valueD=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueN1=atol(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueN2=atol(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueN3=atol(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueS1=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueS2=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueS3=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueD1=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueD2=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preValueD3=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_useTag=tmpstr[0];

	return true;
}

PO_SysParam::PO_SysParam(const PO_SysParam& right)
{
	*this=right;
}

void PO_SysParam::operator=(const PO_SysParam& right)
{
	m_record=right.m_record;
}

int operator==(const PO_SysParam &left,const PO_SysParam& right)
{
	return (strcasecmp(left.m_record.m_sysParamCode.c_str(),
	                    right.m_record.m_sysParamCode.c_str())==0);
}

int operator<(const PO_SysParam &left,const PO_SysParam& right)
{
  int t_iRet = strcasecmp(left.m_record.m_sysParamCode.c_str(),
	                        right.m_record.m_sysParamCode.c_str());
	return (t_iRet<0);
//  if(t_iRet!=0) return (t_iRet<0);
	//return (left.m_record.m_channelNo < right.m_record.m_channelNo);
}

ostream& operator<<(ostream& os,const PO_SysParam& po)
{
	os<<po.m_record.m_sysParamCode<<" , "<<
			po.m_record.m_channelNo<<" , "<<
			po.m_record.m_valueN<<" , "<<
			po.m_record.m_valueS<<" , "<<
			po.m_record.m_valueD<<" , "<<
			po.m_record.m_preValueN1<<" , "<<
			po.m_record.m_preValueN2<<" , "<<
			po.m_record.m_preValueN3<<" , "<<
			po.m_record.m_preValueS1<<" , "<<
			po.m_record.m_preValueS2<<" , "<<
			po.m_record.m_preValueS3<<" , "<<
			po.m_record.m_preValueD1<<" , "<<
			po.m_record.m_preValueD2<<" , "<<
			po.m_record.m_preValueD3<<" , "<<
			po.m_record.m_useTag;
			
	return os;
}

PO_Property PO_SysParam::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SysParam",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT ltrim(rtrim(SYS_PARAM_CODE)),CHANNEL_NO,NVL(VALUE_N,0),	\
	NVL(ltrim(rtrim(VALUE_S)),''),NVL(to_char(VALUE_D,'yyyymmddhh24miss'),'20501231000000'),	\
	NVL(PRE_VALUE_N1,0),NVL(PRE_VALUE_N2,0),NVL(PRE_VALUE_N3,0),	\
	NVL(ltrim(rtrim(PRE_VALUE_S1)),''),NVL(ltrim(rtrim(PRE_VALUE_S2)),''),	\
	NVL(ltrim(rtrim(PRE_VALUE_S3)),''),NVL(to_char(PRE_VALUE_D1,'yyyymmddhh24miss'),'20501231000000'),	\
	NVL(to_char(PRE_VALUE_D2,'yyyymmddhh24miss'),'20501231000000'),	\
	NVL(to_char(PRE_VALUE_D3,'yyyymmddhh24miss'),'20501231000000'),	\
	USE_TAG FROM TD_B_SYSPARAM",
		15,"select count(*) from TD_B_SYSPARAM"};

