#include "PO_CellDef.h"

bool PO_CellDef::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_CellDef::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_areaCode=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_partitionId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_lac=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cellId=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime=tmpstr;
	
	return true;
}

PO_CellDef::PO_CellDef(const PO_CellDef& right)
{
	*this=right;
}

void PO_CellDef::operator=(const PO_CellDef& right)
{
	m_record=right.m_record;
}

int operator==(const PO_CellDef &left,const PO_CellDef& right)
{
	return (left.m_record.m_cellId   == right.m_record.m_cellId &&
	        left.m_record.m_areaCode == right.m_record.m_areaCode);
}

int operator<(const PO_CellDef &left,const PO_CellDef& right)
{
  if(left.m_record.m_areaCode < right.m_record.m_areaCode)
    return true;
	if(left.m_record.m_areaCode > right.m_record.m_areaCode)
    return false;
  
	if(left.m_record.m_cellId < right.m_record.m_cellId)
    return true;
	if(left.m_record.m_cellId > right.m_record.m_cellId)
    return false;

  return false;
}

ostream& operator<<(ostream& os,const PO_CellDef& po)
{
	os<<po.m_record.m_areaCode<<" , "<<
	    po.m_record.m_partitionId<<" , "<<
			po.m_record.m_msc<<" , "<<
			po.m_record.m_lac<<" , "<<
			po.m_record.m_cellId<<" , "<<
			po.m_record.m_beginTime<<" , "<<
			po.m_record.m_endTime;
			
	return os;
}

PO_Property PO_CellDef::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_CellDef",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select eparchy_code,spec_area_id,msc,lac,cell_id,to_char(start_date,'YYYYMMDDHH24MISS'), \
		to_char(end_date,'YYYYMMDDHH24MISS') from TD_B_SPECAREA_COMP",
		7,"select count(*) from TD_B_SPECAREA_COMP"};

