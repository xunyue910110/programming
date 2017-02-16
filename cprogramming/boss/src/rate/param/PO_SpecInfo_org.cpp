#include "PO_SpecInfo_org.h"

bool PO_SpecInfo_org::getProperty(PO_Property &property)
{
  property=m_property;
  return true;
}


bool PO_SpecInfo_org::convert(const StringVector &argname)
{
  int i=0;
  string tmpstr;
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_spInfoType=tmpstr;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_spInfoId=tmpstr;

  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_level=tmpstr;
  
  tmpstr=*(argname.begin()+i++);
  tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
  m_record.m_superId=tmpstr;
  
  return true;
}

PO_SpecInfo_org::PO_SpecInfo_org(const PO_SpecInfo_org& right)
{
  *this=right;
}

void PO_SpecInfo_org::operator=(const PO_SpecInfo_org& right)
{
  m_record=right.m_record;
}

int operator==(const PO_SpecInfo_org &left,const PO_SpecInfo_org& right)
{
  return (left.m_record.m_spInfoType == right.m_record.m_spInfoType &&
          left.m_record.m_spInfoId   == right.m_record.m_spInfoId) ;
}

int operator<(const PO_SpecInfo_org &left,const PO_SpecInfo_org& right)
{
  if (left.m_record.m_spInfoType < right.m_record.m_spInfoType)
  {
    return true;
  }
  if (left.m_record.m_spInfoType > right.m_record.m_spInfoType)
  {
    return false;
  }
  if (left.m_record.m_spInfoId < right.m_record.m_spInfoId)
  {
    return true;
  }
  if (left.m_record.m_spInfoId > right.m_record.m_spInfoId)
  {
    return false;
  }
  
  return false;
}

ostream& operator<<(ostream& os,const PO_SpecInfo_org& po)
{
  os<<po.m_record.m_spInfoType<<" , "<<
      po.m_record.m_spInfoId<<" , "<<
      po.m_record.m_level<<" , "<<
      po.m_record.m_superId;
      
  return os;
}

PO_Property PO_SpecInfo_org::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SpecInfo_org",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select specinfo_type,specinfo_id,layer_class,\
     super_specinfo_id from TD_B_SPECINFO_ORG",
    4,"select count(*) from TD_B_SPECINFO_ORG"};

