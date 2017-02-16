#include "PO_CycleEparchy.h"

bool PO_CycleEparchy::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_CycleEparchy::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_dbNo=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eparchyCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cycleId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_discntJudgeTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_monthAcctStatus=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_auxAcctStatus=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_acctTag=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_fileCreateTag=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_openAcctDate=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_halfTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_useTag=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_addTag=tmpstr;

	return true;
}

PO_CycleEparchy::PO_CycleEparchy(const PO_CycleEparchy& right)
{
	*this=right;
}

void PO_CycleEparchy::operator=(const PO_CycleEparchy& right)
{
	m_record=right.m_record;
}

int operator==(const PO_CycleEparchy &left,const PO_CycleEparchy& right)
{
	return (left.m_record.m_cycleId == right.m_record.m_cycleId &&
	        left.m_record.m_dbNo == right.m_record.m_dbNo &&
	        left.m_record.m_eparchyCode == right.m_record.m_eparchyCode);
}

int operator<(const PO_CycleEparchy &left,const PO_CycleEparchy& right)
{
    if (left.m_record.m_cycleId < right.m_record.m_cycleId) return true;
    if (left.m_record.m_cycleId > right.m_record.m_cycleId) return false;
    if(left.m_record.m_dbNo < right.m_record.m_dbNo) return true;
    if(left.m_record.m_dbNo > right.m_record.m_dbNo) return false;
    return (left.m_record.m_eparchyCode < right.m_record.m_eparchyCode);
}

ostream& operator<<(ostream& os,const PO_CycleEparchy& po)
{
	os<<po.m_record.m_dbNo   <<" , "<<
	    po.m_record.m_cycleId<<" , "<<
	    po.m_record.m_eparchyCode<<" , "<<
	    po.m_record.m_acctTag<<" , "<<
	    po.m_record.m_fileCreateTag<<" , "<<
			po.m_record.m_beginTime<<" , "<<
			po.m_record.m_halfTime<<" , "<<
			po.m_record.m_endTime<<" , "<<
			po.m_record.m_useTag<<" , "<<
			po.m_record.m_addTag;
			
	return os;
}

//limeng 20090818
PO_Property PO_CycleEparchy::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_CycleEparchy",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select db_no,eparchy_code,cycle_id,to_char(discnt_judge_time,'YYYYMMDDHH24MISS'),month_acct_status,aux_acct_status,acct_tag,file_create_tag,to_char(open_acct_date,'YYYYMMDDHH24MISS'),to_char(cyc_start_time,'YYYYMMDDHH24MISS'), \
		 to_char(cyc_half_time,'YYYYMMDDHH24MISS'), \
		 to_char(cyc_end_time,'YYYYMMDDHH24MISS'),use_tag,add_tag  from VW_TD_B_CYCLE_EPARCHY",
		14,"select count(*) from VW_TD_B_CYCLE_EPARCHY"};
