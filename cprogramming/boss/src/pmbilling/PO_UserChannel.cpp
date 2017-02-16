#include "PO_UserChannel.h"

bool PO_UserChannel::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_UserChannel::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginPartition=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endPartition=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_eparchyCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_channelNo=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_dbNo = atoi(tmpstr.c_str());	
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_ratePath = tmpstr;	
	
	return true;
}

PO_UserChannel::PO_UserChannel(const PO_UserChannel& right)
{
	*this=right;
}

void PO_UserChannel::operator=(const PO_UserChannel& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_UserChannel &left,const PO_UserChannel& right)
{
	return (left.recordStruct.m_eparchyCode == right.recordStruct.m_eparchyCode)&&
	       (((left.recordStruct.m_beginPartition >= right.recordStruct.m_beginPartition)&&
			    (left.recordStruct.m_endPartition <= right.recordStruct.m_endPartition))||
			    ((left.recordStruct.m_beginPartition <= right.recordStruct.m_beginPartition)&&
			    (left.recordStruct.m_endPartition >= right.recordStruct.m_endPartition)));
}

int operator<(const PO_UserChannel &left,const PO_UserChannel& right)
{
	if(left.recordStruct.m_eparchyCode == right.recordStruct.m_eparchyCode)
	{
		return (left.recordStruct.m_beginPartition<right.recordStruct.m_beginPartition)&&
				(left.recordStruct.m_endPartition<right.recordStruct.m_endPartition);
	}
	else
	{
		return (left.recordStruct.m_eparchyCode < right.recordStruct.m_eparchyCode);
	}
}

ostream& operator<<(ostream& os,const PO_UserChannel& po)
{
	os<<po.recordStruct.m_beginPartition<<" , "<<
			po.recordStruct.m_endPartition<<" , "<<
			po.recordStruct.m_eparchyCode<<" , "<<
			po.recordStruct.m_channelNo<<" , "<<
			po.recordStruct.m_dbNo<<" , "<<
			po.recordStruct.m_ratePath;

	return os;
}

PO_Property PO_UserChannel::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_UserChannel",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT BEGIN_PARTITION_ID,END_PARTITION_ID,EPARCHY_CODE,CHANNEL_NO,DB_NO,RATE_PATH FROM TD_USER_CHANNEL",
		6,"select count(*) from TD_USER_CHANNEL"};

