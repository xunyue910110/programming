#include "PO_Holiday.h"

bool PO_Holiday::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_Holiday::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_holidayType=tmpstr.c_str();

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_holidayBeginDate=tmpstr.c_str();

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_holidayEndDate=tmpstr.c_str();

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_dateType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cycleType=tmpstr[0];
	
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

PO_Holiday::PO_Holiday(const PO_Holiday& right)
{
	*this=right;
}

void PO_Holiday::operator=(const PO_Holiday& right)
{
	m_record=right.m_record;
}

int operator==(const PO_Holiday &left,const PO_Holiday& right)
{
	return left.m_record.m_holidayBeginDate == right.m_record.m_holidayBeginDate;
}

int operator<(const PO_Holiday &left,const PO_Holiday& right)
{
	return left.m_record.m_holidayBeginDate < right.m_record.m_holidayBeginDate;
}

ostream& operator<<(ostream& os,const PO_Holiday& po)
{
	os<<po.m_record.m_holidayType   <<" , "<<
	    po.m_record.m_holidayBeginDate<<" , "<<
		po.m_record.m_holidayEndDate<<" , "<<
		po.m_record.m_dateType<<" , "<<
		po.m_record.m_cycleType<<" , "<<
		po.m_record.m_beginTime<<" , "<<
		po.m_record.m_endTime;
	return os;
}

PO_Property PO_Holiday::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_Holiday",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT HOLIDAY_TYPE,BEGIN_DATE,END_DATE,DATE_TYPE,CYCLE_TYPE,BEGIN_TIME,END_TIME FROM TD_B_HOLIDAY",
		7,"SELECT COUNT(*) FROM TD_B_HOLIDAY"};

void Holiday::operator=(const Holiday &right)
{
	m_holidayType = right.m_holidayType;
	m_holidayDate = right.m_holidayDate;
	m_beginTime  = right.m_beginTime;
	m_endTime     = right.m_endTime;
}

void Holiday::operator=(const PO_Holiday& right)
{
	m_holidayType = right.m_record.m_holidayType;
	m_holidayDate = right.m_record.m_holidayBeginDate;
	m_beginTime  = right.m_record.m_beginTime;
	m_endTime     = right.m_record.m_endTime;
}

int operator==(const Holiday &left,const Holiday &right)
{
	return left.m_holidayDate == right.m_holidayDate;
}
int operator<(const Holiday &left,const Holiday &right)
{
	return left.m_holidayDate < right.m_holidayDate;
}
ostream& operator<<(ostream& os,const Holiday& po)
{
	os<<po.m_holidayType   <<" , "<<
	    po.m_holidayDate<<" , "<<
		po.m_beginTime<<" , "<<
		po.m_endTime;
	return os;
}
