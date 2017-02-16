#include "PO_BaseCellTp.h"

bool PO_BaseCellTp::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_BaseCellTp::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_callType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_roamType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_servTypea=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_homeAreaCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_partitionId=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpId=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_priority=atoi(tmpstr.c_str());
	
	return true;
}

PO_BaseCellTp::PO_BaseCellTp(const PO_BaseCellTp& right)
{
	*this=right;
}

void PO_BaseCellTp::operator=(const PO_BaseCellTp& right)
{
	m_record=right.m_record;
}

int operator==(const PO_BaseCellTp &left,const PO_BaseCellTp& right)
{
	return true;
}

int operator<(const PO_BaseCellTp &left,const PO_BaseCellTp& right)
{
	return (left.m_record.m_priority > right.m_record.m_priority);
}

ostream& operator<<(ostream& os,const PO_BaseCellTp& po)
{
	os<<po.m_record.m_callType<<" , "<<
			po.m_record.m_roamType<<" , "<<
			po.m_record.m_servTypea<<" , "<<
			po.m_record.m_homeAreaCode<<" , "<<
			po.m_record.m_partitionId<<" , "<<
			po.m_record.m_tpId<<" , "<<
			po.m_record.m_beginTime<<" , "<<
			po.m_record.m_endTime<<" , "<<
			po.m_record.m_priority;
			
	return os;
}

PO_Property PO_BaseCellTp::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_BaseCellTp",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select call_type,roam_type,service_typea,home_area_code, \
		 spec_area_id,feePolicy_id,begin_time, \
		 end_time,priority from td_basecell_tp",
		9,"select count(*) from td_basecell_tp"};

