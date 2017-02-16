#include "PO_FixTrunk.h"

bool PO_FixTrunk::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
bool PO_FixTrunk::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.area_code=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_bill_type=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.bill_number=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.serv_type=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.trunk_direction=tmpstr;

  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.trunk_group=tmpstr;

  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_begin_phone_a=tmpstr;

  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_end_phone_a=tmpstr;

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

PO_FixTrunk::PO_FixTrunk(const PO_FixTrunk& right)
{
	*this=right;
}

void PO_FixTrunk::operator=(const PO_FixTrunk& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_FixTrunk &left,const PO_FixTrunk& right)
{
	return ((left.recordStruct.area_code==right.recordStruct.area_code) &&
            (left.recordStruct.trunk_group==right.recordStruct.trunk_group));
}

int operator<(const PO_FixTrunk &left,const PO_FixTrunk& right)
{
	if(left.recordStruct.area_code<right.recordStruct.area_code) return true;
	if(left.recordStruct.area_code>right.recordStruct.area_code) return false;
  if(left.recordStruct.trunk_group<right.recordStruct.trunk_group) return true;
  return false;
}

ostream& operator<<(ostream& os,const PO_FixTrunk& po)
{
	os<<po.recordStruct.area_code   <<" , "<<
	    po.recordStruct.msc         <<" , "<<
	    po.recordStruct.m_bill_type <<" , "<<
	    po.recordStruct.bill_number <<" , "<<
	    po.recordStruct.serv_type   <<" , "<<
	    po.recordStruct.trunk_direction <<" , "<<
	    po.recordStruct.m_begin_phone_a <<" , "<<
	    po.recordStruct.m_end_phone_a   <<" , "<<
	    po.recordStruct.trunk_group;

	return os;
}


PO_Property PO_FixTrunk::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FixTrunk",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select area_code,msc,bill_type,bill_number,serv_type,trunk_direction,trunk_group,begin_phone_a,end_phone_a,begin_time,end_time from TD_FIX_TRUNK",
		11,"select count(*) from TD_FIX_TRUNK"};

