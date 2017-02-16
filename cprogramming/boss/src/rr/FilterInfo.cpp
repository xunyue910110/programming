#include "FilterInfo.h"

bool CmpPFilterInfo::operator() (const FilterInfo *r_left, 
                                 const FilterInfo *r_right) const
{
  if(r_left->m_fid<r_right->m_fid)
  {
    return true;
  }
  if(r_left->m_fid>r_right->m_fid)
  {
    return false;
  }
  if(r_left->m_beginTime < r_right->m_beginTime)
  {
    return true;
  }
  if(r_left->m_beginTime > r_right->m_beginTime)
  {
    return false;
  }
  return false;
}

bool CmpPHisFidInfo_Time::operator() (const HisFidInfo *r_left, const HisFidInfo *r_right) const
{
  return (r_left->m_beginTime < r_right->m_beginTime);
};

FilterInfo::FilterInfo()
{
}
FilterInfo::FilterInfo(const FilterInfo &r_right)
{
  m_fid = r_right.m_fid;
  m_beginTime=r_right.m_beginTime;
  m_endTime  =r_right.m_endTime;
  m_filePos  =r_right.m_filePos;
  #ifdef _DEBUG_
    m_fileName=r_right.m_fileName;
    m_line=r_right.m_line;   
  #endif 
  m_rrFlag  =r_right.m_rrFlag;
  m_uniqFlag=r_right.m_uniqFlag;
}

FilterInfo::~FilterInfo()
{
  
}
FilterInfo& FilterInfo::operator=(const FilterInfo &r_right)
{
  m_fid = r_right.m_fid;
  m_beginTime=r_right.m_beginTime;
  m_endTime  =r_right.m_endTime;
  m_filePos  =r_right.m_filePos;
  #ifdef _DEBUG_
    m_fileName=r_right.m_fileName;
    m_line=r_right.m_line;   
  #endif 
  m_rrFlag  =r_right.m_rrFlag;
  m_uniqFlag=r_right.m_uniqFlag;
  return *this;
}

HisFidInfo::HisFidInfo()
{
}
HisFidInfo::HisFidInfo(const HisFidInfo &r_right)
{
  m_fid = r_right.m_fid;
  m_beginTime=r_right.m_beginTime;
  m_endTime  =r_right.m_endTime;
}
HisFidInfo::~HisFidInfo()
{
}
HisFidInfo& HisFidInfo::operator=(const HisFidInfo &r_right)
{
  m_fid = r_right.m_fid;
  m_beginTime=r_right.m_beginTime;
  m_endTime  =r_right.m_endTime;
  return *this;
}

HisFidInfo& HisFidInfo::operator=(const FilterInfo &r_right)
{
  m_fid = r_right.m_fid;
  m_beginTime=r_right.m_beginTime;
  m_endTime  =r_right.m_endTime;
  return *this;
}

