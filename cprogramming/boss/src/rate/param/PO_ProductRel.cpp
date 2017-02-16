#include "PO_ProductRel.h"

bool PO_ProductRel::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_ProductRel::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_productId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_status=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_parentProductId=tmpstr;
	
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

PO_ProductRel::PO_ProductRel(const PO_ProductRel& right)
{
	*this=right;
}

void PO_ProductRel::operator=(const PO_ProductRel& right)
{
	m_record=right.m_record;
}

int operator==(const PO_ProductRel &left,const PO_ProductRel& right)
{
	return (left.m_record.m_productId == right.m_record.m_productId);
}

int operator<(const PO_ProductRel &left,const PO_ProductRel& right)
{
	return (left.m_record.m_productId < right.m_record.m_productId);
}

ostream& operator<<(ostream& os,const PO_ProductRel& po)
{
	os<<po.m_record.m_productId<<" , "<<
		po.m_record.m_status<<" , "<<
		po.m_record.m_parentProductId<<" , "<<
		po.m_record.m_beginTime<<" , "<<
		po.m_record.m_endTime;
	return os;
}

PO_Property PO_ProductRel::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_ProductRel",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select PRODUCT_ID,STATUS,PARENT_PRODUCT_ID,BEGIN_TIME,END_TIME from TD_B_PRODUCT_REL",
		5,"select count(*) from TD_B_PRODUCT_REL"};
