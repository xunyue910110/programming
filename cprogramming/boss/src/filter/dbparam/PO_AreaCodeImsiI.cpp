#include "PO_AreaCodeImsiI.h"

bool PO_AreaCodeImsiI::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AreaCodeImsiI::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_imsi=tmpstr;
	
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
	recordStruct.m_feeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_userType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_aspCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupBeginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupEndTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_aspGroup=tmpstr;

	return true;
}

PO_AreaCodeImsiI::PO_AreaCodeImsiI(const PO_AreaCodeImsiI& right)
{
	*this=right;
}

void PO_AreaCodeImsiI::operator=(const PO_AreaCodeImsiI& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AreaCodeImsiI &left,const PO_AreaCodeImsiI &right)
{
	return left.recordStruct.m_imsi==right.recordStruct.m_imsi;
}

int operator<(const PO_AreaCodeImsiI &left,const PO_AreaCodeImsiI& right)
{
	return (left.recordStruct.m_imsi<right.recordStruct.m_imsi);
}

ostream& operator<<(ostream& os,const PO_AreaCodeImsiI& po)
{
	os<<po.recordStruct.m_imsi<<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime<<" , "<<
			po.recordStruct.m_feeCode<<" , "<<
			po.recordStruct.m_groupId<<" , "<<
			po.recordStruct.m_userType<<" , "<<
			po.recordStruct.m_aspCode<<" , " <<
			po.recordStruct.m_groupBeginTime<<" , " <<
			po.recordStruct.m_groupEndTime<<" , " <<
			po.recordStruct.m_aspGroup;
	return os;
}


PO_Property PO_AreaCodeImsiI::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_AreaCodeImsiI",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select IMSI, BEGIN_TIME, END_TIME, NATION_AREA_CODE, GROUP_ID, asp||serv_type||home_type, ASP_CODE, GROUP_BEGINTIME, GROUP_ENDTIME, ASP_GROUP from TD_IMSII",
		10,"select count(*) from TD_IMSII"};


