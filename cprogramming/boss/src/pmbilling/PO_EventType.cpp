#include "PO_EventType.h"


bool PO_EventType::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_EventType::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eventTypeId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_pEventTypeId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eventRank=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_servId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_subCycType=tmpstr[0];
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_condId=atoi(tmpstr.c_str());
	
	m_record.m_condRule=NULL;

	return true;
}

PO_EventType::PO_EventType(const PO_EventType& right)
{
	*this=right;
}

void PO_EventType::operator=(const PO_EventType& right)
{
	m_record=right.m_record;
}

int operator==(const PO_EventType &left,const PO_EventType& right)
{
    return (left.m_record.m_eventTypeId == right.m_record.m_eventTypeId);
}

int operator<(const PO_EventType &left,const PO_EventType& right)
{
    return (left.m_record.m_eventTypeId < right.m_record.m_eventTypeId);
}

ostream& operator<<(ostream& os,const PO_EventType& po)
{
	os<<po.m_record.m_eventTypeId	<<" , "<<
		po.m_record.m_pEventTypeId  <<" , "<<
		po.m_record.m_eventRank		<<" , "<<
		po.m_record.m_servId	    <<" , "<<
		po.m_record.m_subCycType    <<" , "<<
		po.m_record.m_condId; 

	return os;
}


PO_Property PO_EventType::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_EventType","","","","SELECT EVENT_TYPE_ID,P_EVENT_TYPE_ID, \
    EVENT_RANK,SERV_ID,CYCLE_TYPE_ID, COND_ID FROM TD_B_EVENTTYPE \
    WHERE CYCLE_TYPE_ID IS NOT NULL AND CYCLE_TYPE_ID != 0",
    6,"SELECT count(*) FROM TD_B_EVENTTYPE WHERE CYCLE_TYPE_ID IS NOT NULL AND CYCLE_TYPE_ID != 0"};
