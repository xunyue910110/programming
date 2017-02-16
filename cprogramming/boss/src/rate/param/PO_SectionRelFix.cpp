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
	m_record.m_sectionCodeA=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_phoneSegA=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_sectionCodeB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_phoneSegB=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_areaType=tmpstr;

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
	m_record.m_priority=atoi(tmpstr.c_str());

//cout<<"m_record.m_priority:"<<m_record.m_priority<<endl;
//cout<<"m_record.m_sectionCodeA:"<<m_record.m_sectionCodeA<<endl;
//cout<<"m_record.m_phoneSegA:"<<m_record.m_phoneSegA<<endl;
//cout<<"m_record.m_sectionCodeB:"<<m_record.m_sectionCodeB<<endl;
//cout<<"m_record.m_phoneSegB:"<<m_record.m_phoneSegB<<endl;
//cout<<"m_record.m_areaType:"<<m_record.m_areaType<<endl;
//cout<<"m_record.m_beginTime:"<<m_record.m_beginTime<<endl;
//cout<<"m_record.m_endTime:"<<m_record.m_endTime<<endl;

	return true;
}

PO_SectionRelFix::PO_SectionRelFix(const PO_SectionRelFix& right)
{
	*this=right;
}

void PO_SectionRelFix::operator=(const PO_SectionRelFix& right)
{
	m_record=right.m_record;
}

int operator==(const PO_SectionRelFix &left,const PO_SectionRelFix& right)
{
	return ((left.m_record.m_sectionCodeA==right.m_record.m_sectionCodeA ||
	        left.m_record.m_sectionCodeA=="*" || right.m_record.m_sectionCodeA=="*")&&
	        (left.m_record.m_sectionCodeB==right.m_record.m_sectionCodeB ||
	        left.m_record.m_sectionCodeB=="*" || right.m_record.m_sectionCodeB=="*"));
}

int operator<(const PO_SectionRelFix &left,const PO_SectionRelFix& right)
{
		return (left.m_record.m_sectionCodeA < right.m_record.m_sectionCodeA);
}

ostream& operator<<(ostream& os,const PO_SectionRelFix& po)
{
	os<<po.m_record.m_priority     <<" , "<<
	    po.m_record.m_phoneSegA    <<" , "<<
      po.m_record.m_sectionCodeA <<" , "<<
      po.m_record.m_phoneSegB    <<" , "<<
      po.m_record.m_sectionCodeB <<" , "<<
      po.m_record.m_areaType     <<" , "<<
      po.m_record.m_beginTime    <<" , "<<
			po.m_record.m_endTime;
	return os;
}


PO_Property PO_SectionRelFix::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SectionRelFix",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select SECTION_CODE_A, AREA_CODE_A, SECTION_CODE_B, AREA_CODE_B, AREA_TYPE, BEGIN_TIME, END_TIME, PRIORITY from TD_B_SECTION_REL",
		8,"select count(*) from TD_B_SECTION_REL"};
