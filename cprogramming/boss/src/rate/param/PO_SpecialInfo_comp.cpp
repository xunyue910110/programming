#include "PO_SpecialInfo_comp.h"

bool PO_SpecialInfo_comp::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_SpecialInfo_comp::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_spInfoId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_memberId=tmpstr;

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
	m_record.m_matchType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_preFixType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_servType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_memberType=tmpstr;

	return true;
}

PO_SpecialInfo_comp::PO_SpecialInfo_comp(const PO_SpecialInfo_comp& right)
{
	*this=right;
}

void PO_SpecialInfo_comp::operator=(const PO_SpecialInfo_comp& right)
{
	m_record=right.m_record;
}

int operator==(const PO_SpecialInfo_comp &left,const PO_SpecialInfo_comp& right)
{
	if(!(left.m_record.m_memberType == right.m_record.m_memberType))
  {
    return false;
  }
  
	if(!(left.m_record.m_memberId == right.m_record.m_memberId))
  {
    return false;
  }

	if(!(left.m_record.m_preFixType == right.m_record.m_preFixType))
  {
    return false;
  }

  if(left.m_record.m_beginTime >= right.m_record.m_beginTime &&
     left.m_record.m_beginTime <= right.m_record.m_endTime ||
     left.m_record.m_beginTime <= right.m_record.m_beginTime &&
     left.m_record.m_endTime   >= right.m_record.m_beginTime )
  {
    return true;
  }

	return false;
}

int operator<(const PO_SpecialInfo_comp &left,const PO_SpecialInfo_comp& right)
{
	if(left.m_record.m_memberType < right.m_record.m_memberType)
  {
    return true;
  }
	if(left.m_record.m_memberType > right.m_record.m_memberType)
  {
    return false;
  }
    
	if(left.m_record.m_memberId < right.m_record.m_memberId)
  {
    return true;
  }

	if(left.m_record.m_memberId > right.m_record.m_memberId)
  {
    return false;
  }
	if(left.m_record.m_preFixType < right.m_record.m_preFixType)
  {
    return true;
  }
	if(left.m_record.m_preFixType > right.m_record.m_preFixType)
  {
    return false;
  }
  
  if(left.m_record.m_beginTime < right.m_record.m_beginTime &&
     left.m_record.m_endTime < right.m_record.m_endTime )
  {
    return true;
  }
  return false;
}

ostream& operator<<(ostream& os,const PO_SpecialInfo_comp& po)
{
	os<<po.m_record.m_spInfoId  <<" , "<<
			po.m_record.m_memberId  <<" , "<<
			po.m_record.m_beginTime <<" , "<<
			po.m_record.m_endTime   <<" , "<<
			po.m_record.m_matchType <<" , "<<
			po.m_record.m_preFixType<<" , "<<
			po.m_record.m_servType  <<" , "<<
			po.m_record.m_memberType;
			
	return os;
}

PO_Property PO_SpecialInfo_comp::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpecialInfo_comp",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select SPEC_PHONE_ID,MEMBER_ID,to_char(START_DATE,'yyyymmddhh24miss'),\
		 to_char(END_DATE,'yyyymmddhh24miss'),\
		 match_type,prefix_type,service_type,MEMBER_TYPE from TD_B_SPECPHONE_COMP",
		8,"select count(*) from TD_B_SPECPHONE_COMP"};

