#include "PO_Trunk.h"

bool PO_Trunk::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}
bool PO_Trunk::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.trunkid=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.trunktype=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.busitype=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.usertype=tmpstr;

  tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.filterflag=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.trunkdirection=tmpstr;

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

PO_Trunk::PO_Trunk(const PO_Trunk& right)
{
	*this=right;
}

void PO_Trunk::operator=(const PO_Trunk& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_Trunk &left,const PO_Trunk& right)
{
	return ((left.recordStruct.msc==right.recordStruct.msc) &&
            (left.recordStruct.trunkid==right.recordStruct.trunkid));
}

int operator<(const PO_Trunk &left,const PO_Trunk& right)
{
	if(left.recordStruct.msc<right.recordStruct.msc) return true;
	if(left.recordStruct.msc>right.recordStruct.msc) return false;
  if(left.recordStruct.trunkid<right.recordStruct.trunkid) return true;
  return false;
}

ostream& operator<<(ostream& os,const PO_Trunk& po)
{
	os<<po.recordStruct.msc      <<" , "<<
	    po.recordStruct.trunkid  <<" , "<<
	    po.recordStruct.trunkdirection<<" , "<<
	    po.recordStruct.trunktype<<" , "<<
	    po.recordStruct.busitype <<" , "<<
	    po.recordStruct.usertype <<" , "<<
	    po.recordStruct.filterflag;
	    
	return os;
}


PO_Property PO_Trunk::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_Trunk",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select substr(msc,1,8),trunkgroup,trunk_type,busi_type,user_type,filter_flag,trunk_direction,begin_time,end_time from TD_TRUNK",
		9,"select count(*) from TD_TRUNK"};

