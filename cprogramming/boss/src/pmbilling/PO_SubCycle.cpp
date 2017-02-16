#include "PO_SubCycle.h"


bool PO_SubCycle::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_SubCycle::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cycId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_subCycId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cycTypeCode=tmpstr[0];
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_relativeCyc=atoi(tmpstr.c_str());
	/*
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_sequenceNo=atoi(tmpstr.c_str());*/

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_dayNum=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_startTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endTime=tmpstr;
					
	return true;
}

PO_SubCycle::PO_SubCycle(const PO_SubCycle& right)
{
	*this=right;
}

void PO_SubCycle::operator=(const PO_SubCycle& right)
{
	m_record=right.m_record;
}

int operator==(const PO_SubCycle &left,const PO_SubCycle& right)
{
    return ( (left.m_record.m_cycId == right.m_record.m_cycId) &&
    			(left.m_record.m_cycTypeCode == right.m_record.m_cycTypeCode) );
}

int operator<(const PO_SubCycle &left,const PO_SubCycle& right)
{
    if (left.m_record.m_cycId == right.m_record.m_cycId)
    {
    	return (left.m_record.m_cycTypeCode < right.m_record.m_cycTypeCode);
	}
	else
	{	
    	return (left.m_record.m_cycId < right.m_record.m_cycId);
    }	
}

ostream& operator<<(ostream& os,const PO_SubCycle& po)
{
	os<<po.m_record.m_cycId		<<" , "<<
		po.m_record.m_subCycId		<<" , "<<
		po.m_record.m_cycTypeCode	<<" , "<<
		po.m_record.m_relativeCyc	<<" , "<<
		//po.m_record.m_sequenceNo	<<" , "<<
        po.m_record.m_dayNum    	<<" , "<<
		po.m_record.m_startTime		<<" , "<<
		po.m_record.m_endTime; 

	return os;
}


PO_Property PO_SubCycle::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SubCycle","","","","SELECT CYC_ID,SUBCYC_ID,CYC_TYPE_CODE,	\
	RELATIVE_CYC, DAY_NUM,to_char(START_TIME,'YYYYMMDD'),			\
	to_char(END_TIME,'YYYYMMDD') FROM TD_B_SUBCYCLE",
    7,"SELECT count(*) FROM TD_B_SUBCYCLE"};
