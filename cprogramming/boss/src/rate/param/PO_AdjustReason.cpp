#include "PO_AdjustReason.h"

bool PO_AdjustReason::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_AdjustReason::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_adjustReasonId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	  m_record.m_adjustModeType=tmpstr.c_str()[0];
  else
    m_record.m_adjustModeType=0;
    
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_priority=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	  m_record.m_ifShowNegative=tmpstr.c_str()[0];
  else
    m_record.m_ifShowNegative=0;

  	tmpstr=*(argname.begin()+i++);

	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	  m_record.m_effectTimeTag=tmpstr.c_str()[0];
  else
    m_record.m_effectTimeTag=0;
    
  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_condId=atoi(tmpstr.c_str()); 

	return true;
}

PO_AdjustReason::PO_AdjustReason(const PO_AdjustReason& right)
{
	*this=right;
}

void PO_AdjustReason::operator=(const PO_AdjustReason& right)
{
	m_record=right.m_record;
}

int operator==(const PO_AdjustReason &left,const PO_AdjustReason& right)
{
	return (left.m_record.m_adjustReasonId    == right.m_record.m_adjustReasonId );
}

int operator<(const PO_AdjustReason &left,const PO_AdjustReason& right)
{
	return (left.m_record.m_adjustReasonId < right.m_record.m_adjustReasonId);
}

ostream& operator<<(ostream& os,const PO_AdjustReason& po)
{
	os<<po.m_record.m_adjustReasonId<<" , "<<
			po.m_record.m_adjustModeType<<" , "<<
			po.m_record.m_priority<<" , "<<
			po.m_record.m_ifShowNegative<<" , "<<
			po.m_record.m_effectTimeTag<<","<<
			po.m_record.m_condId;
			
	return os;
}

PO_Property PO_AdjustReason::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AdjustReason",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select adjust_reason_id,adjust_mode_type,priority, \
		 negative_show_tag ,effect_time_tag,nvl(cond_id,0) from TD_B_ADJUSTREASON",
		6,"select count(*) from TD_B_ADJUSTREASON"};

