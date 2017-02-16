#include "PO_VirtualArea.h"


bool PO_VirtualArea::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_VirtualArea::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areagroup=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areacode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_hometype=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_asp=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;

	return true;
}

PO_VirtualArea::PO_VirtualArea(const PO_VirtualArea& right)
{
	*this=right;
}

void PO_VirtualArea::operator=(const PO_VirtualArea& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_VirtualArea &left,const PO_VirtualArea& right)
{
	return (left.recordStruct.m_areacode==right.recordStruct.m_areacode);

}

int operator<(const PO_VirtualArea &left,const PO_VirtualArea& right)
{
	return (left.recordStruct.m_areacode<right.recordStruct.m_areacode);
}

ostream& operator<<(ostream& os,const PO_VirtualArea& po)
{
	os<<po.recordStruct.m_areagroup<<" , "<<
			po.recordStruct.m_areacode<<" , "<<
			po.recordStruct.m_hometype<<" , "<<
			po.recordStruct.m_asp<<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_VirtualArea::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_VirtualArea",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_GROUP, AREA_CODE, HOME_TYPE, ASP_LIMIT, BEGIN_TIME, END_TIME from TD_VIRTUAL_AREA",
		6,"select count(*) from TD_VIRTUAL_AREA"};

