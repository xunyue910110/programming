#include "PO_AttrFactors.h"
#include "base/StringUtil.h"
#include <algorithm>

bool PO_AttrFactors::getProperty(PO_Property &property)
{
  property=m_property;
  return true;
}


bool PO_AttrFactors::convert(const StringVector &argname)
{
  int i=0;
  string tmpstr;
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_biztype=tmpstr;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_fType=atoi(tmpstr.c_str());
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_factn=atoi(tmpstr.c_str())-1;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_fvalues.clear();
	StringUtil::split(tmpstr,";",m_record.m_fvalues,true);
	::sort(m_record.m_fvalues.begin(),m_record.m_fvalues.end());
  
  return true;
}

PO_AttrFactors::PO_AttrFactors(const PO_AttrFactors& right)
{
  *this=right;
}

void PO_AttrFactors::operator=(const PO_AttrFactors& right)
{
  m_record=right.m_record;
}

int operator==(const PO_AttrFactors &left,const PO_AttrFactors& right)
{
  return (left.m_record.m_biztype == right.m_record.m_biztype &&
          left.m_record.m_fType   == right.m_record.m_fType) ;
}

int operator<(const PO_AttrFactors &left,const PO_AttrFactors& right)
{
  if (left.m_record.m_biztype < right.m_record.m_biztype)    return true;
  if (left.m_record.m_biztype > right.m_record.m_biztype)    return false;
  
  if (left.m_record.m_fType < right.m_record.m_fType)    return true;
  if (left.m_record.m_fType > right.m_record.m_fType)    return false;
    
  return false;
}

ostream& operator<<(ostream& os,const PO_AttrFactors& po)
{
	os<<po.m_record.m_biztype<<" , "<<
			po.m_record.m_factn<<" , ";
	
	for(vector<string>::const_iterator itr=po.m_record.m_fvalues.begin();
		  itr!=po.m_record.m_fvalues.end();itr++)
	{
		if(itr==po.m_record.m_fvalues.begin())
			os<<*itr;
		else
			os<<";"<<*itr;
	}
	os<<","<<po.m_record.m_fType;
	return os;
}

PO_Property PO_AttrFactors::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_AttrFactors",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select biz_type,ftype,factn,fvalues from td_attr_factors",
    4,"select count(*) from td_attr_factors"};

