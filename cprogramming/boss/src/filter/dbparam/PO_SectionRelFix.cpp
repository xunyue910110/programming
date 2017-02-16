#include "PO_SectionRelFix.h"

bool PO_SectionRelFix::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_SectionRelFix::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sectionCodeA=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phoneSegA=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sectionCodeB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phoneSegB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaType=tmpstr;

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
	recordStruct.m_priority=atoi(tmpstr.c_str());

//cout<<"recordStruct.m_priority:"<<recordStruct.m_priority<<endl;
//cout<<"recordStruct.m_sectionCodeA:"<<recordStruct.m_sectionCodeA<<endl;
//cout<<"recordStruct.m_phoneSegA:"<<recordStruct.m_phoneSegA<<endl;
//cout<<"recordStruct.m_sectionCodeB:"<<recordStruct.m_sectionCodeB<<endl;
//cout<<"recordStruct.m_phoneSegB:"<<recordStruct.m_phoneSegB<<endl;
//cout<<"recordStruct.m_areaType:"<<recordStruct.m_areaType<<endl;
//cout<<"recordStruct.m_beginTime:"<<recordStruct.m_beginTime<<endl;
//cout<<"recordStruct.m_endTime:"<<recordStruct.m_endTime<<endl;

	return true;
}

PO_SectionRelFix::PO_SectionRelFix(const PO_SectionRelFix& right)
{
	*this=right;
}

void PO_SectionRelFix::operator=(const PO_SectionRelFix& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SectionRelFix &left,const PO_SectionRelFix& right)
{
	return (left.recordStruct.m_sectionCodeA==right.recordStruct.m_sectionCodeA ||
	        left.recordStruct.m_sectionCodeA=="*" || right.recordStruct.m_sectionCodeA=="*");
}

int operator<(const PO_SectionRelFix &left,const PO_SectionRelFix& right)
{
		return (left.recordStruct.m_sectionCodeA < right.recordStruct.m_sectionCodeA);
}

ostream& operator<<(ostream& os,const PO_SectionRelFix& po)
{
	os<<po.recordStruct.m_priority     <<" , "<<
	    po.recordStruct.m_phoneSegA    <<" , "<<
      po.recordStruct.m_sectionCodeA <<" , "<<
      po.recordStruct.m_phoneSegB    <<" , "<<
      po.recordStruct.m_sectionCodeB <<" , "<<
      po.recordStruct.m_areaType     <<" , "<<
      po.recordStruct.m_beginTime    <<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_SectionRelFix::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SectionRelFix",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select SECTION_CODE_A, AREA_CODE_A||PHONE_SEG_A, SECTION_CODE_B, AREA_CODE_B||PHONE_SEG_B, AREA_TYPE, BEGIN_TIME, END_TIME, PRIORITY from TD_SECTION_REL",
		8,"select count(*) from TD_SECTION_REL"};
