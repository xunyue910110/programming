#include "FilterInfoMgr.h"
#include "FileRrRule.h"
#include "RrConstDef.h"
#include <algorithm>
#include <iostream>
#include "base/supdebug.h" 

bool FilterInfoMgr::setRepeatFlag(vector<FilterInfo>& r_fidInfoList, int r_fidCount,
                                  const FileRrRule *r_fileRrRule)
{
  m_rrType  = r_fileRrRule->m_rrType;
  m_fileType= r_fileRrRule->m_fileType;
  m_fileRrRule=r_fileRrRule;
  m_fidCount= r_fidCount;
  m_memFilterInfoMgr.setFileType(m_fileType);
  m_fileFilterInfoMgr.setFileType(m_fileType);
  if(m_rrType==E_RR_RRTYPE_NONE) //不需排重
  { //不需排重，则直接返回
    return true;
  }
  m_pFidInfoList.clear();
  for(size_t i=0;i<r_fidCount;++i)
  {    
    if(r_fidInfoList[i].m_uniqFlag == E_RR_REPFLAG_ERROR)
    { //错单不再排重
      continue;
    }else
    {
      m_pFidInfoList.push_back(&(r_fidInfoList[i]));
    }
  }
  ::stable_sort(m_pFidInfoList.begin(),m_pFidInfoList.end(),CmpPFilterInfo());
  if(m_rrType == E_RR_RRTYPE_EQUAL)
  { //完全排重
    setRepeatFlag_equal();
  }else if(m_rrType == E_RR_RRTYPE_CONTAIN)//changed by hanyu 08-11-27
  { //包容排重
    setRepeatFlag_contain();
  }else
  {
  	setRepeatFlag_file();//added by hanyu 08-11-27
  }
  return true;
}
bool FilterInfoMgr::setRepeatFlag_equal()
{
  //文件内排重
  size_t t_prePos=0;
  for(size_t i=0; i<m_fidCount; ++i)
  {
    //文件内排重
    if(i==0)
    {
      t_prePos = i;
    }else if(m_pFidInfoList[i]->m_fid == m_pFidInfoList[t_prePos]->m_fid)
    {
      //如果后面一个是不需排重的，则应该将前面的判为重单
      if(m_pFidInfoList[i]->m_uniqFlag == E_RR_REPFLAG_NONE)
      {
        if(m_pFidInfoList[t_prePos]->m_uniqFlag==E_RR_REPFLAG_UNIQ)
        {
          m_pFidInfoList[t_prePos]->m_uniqFlag==E_RR_REPFLAG_INFILE;
        }
      }else
      {
        m_pFidInfoList[i]->m_uniqFlag = E_RR_REPFLAG_INFILE;
      }
      continue;
    }else
    {
      t_prePos = i;
    }
    if(setHisRepeatFlag(*m_pFidInfoList[i])==false)
    {
      #ifdef _DEBUG_
        cout<<"setHisRepeatFlag false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  return true;
}
bool FilterInfoMgr::setRepeatFlag_contain()
{
  //文件内排重
  size_t t_prePos=0;
  size_t t_prePos1=0; //第一个Fid相同的
  for(size_t i=1; i<m_fidCount; ++i)
  {
    //文件内排重
    if(m_pFidInfoList[i]->m_fid == m_pFidInfoList[t_prePos]->m_fid)
    {
      for(size_t j=t_prePos;j<i;++j)
      {
        if(m_pFidInfoList[j]->m_uniqFlag == E_RR_REPFLAG_NONE ||
           m_pFidInfoList[j]->m_uniqFlag == E_RR_REPFLAG_UNIQ)
        {   
          if(m_pFidInfoList[j]->m_beginTime>= m_pFidInfoList[i]->m_beginTime &&
             m_pFidInfoList[j]->m_beginTime <  m_pFidInfoList[i]->m_endTime ||
             m_pFidInfoList[j]->m_beginTime <= m_pFidInfoList[i]->m_beginTime &&
             m_pFidInfoList[j]->m_endTime   >  m_pFidInfoList[i]->m_beginTime )
          {
            if(m_pFidInfoList[i]->m_uniqFlag == E_RR_REPFLAG_NONE)
            {
              m_pFidInfoList[j]->m_uniqFlag = E_RR_REPFLAG_INFILE;
            }else
            {
              m_pFidInfoList[i]->m_uniqFlag = E_RR_REPFLAG_INFILE;
              break;
            }
          }
        }else continue;
      }
      continue;
    }else
    {
      t_prePos = i;
    }
  }
  for(size_t k=0;k<m_fidCount; ++k)
  {
    if(setHisRepeatFlag(*m_pFidInfoList[k])==false)
    {
      #ifdef _DEBUG_
        cout<<"setHisRepeatFlag false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  return true;
}

bool FilterInfoMgr::setRepeatFlag_file() //added by hanyu 08-11-27
{
  if(setHisRepeatFlag(*m_pFidInfoList[0])==false)
  {
    #ifdef _DEBUG_
      cout<<"setHisRepeatFlag false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

bool FilterInfoMgr::setHisRepeatFlag(FilterInfo& r_fidInfo)
{
  if(r_fidInfo.m_uniqFlag!=E_RR_REPFLAG_UNIQ)
  {
    return true;
  }
  #ifdef FP_DEBUG
    FP_BEGIN(setHisRepeatFlag)
  #endif
  //1. 根据时间等信息，判断是否属于内存范围内排重
  if(m_memFilterInfoMgr.isInMemList(m_rrType,r_fidInfo,m_fileRrRule->m_timeDuration)==true)
  {
  #ifdef FP_DEBUG
    FP_BEGIN(mem_isRepeatFid)
  #endif
    if(m_memFilterInfoMgr.isRepeatFid(m_rrType,r_fidInfo)==true)
    {
      r_fidInfo.m_uniqFlag = E_RR_REPFLAG_INMEM;
    }else
    {
      m_memFidInfo.push_back(&r_fidInfo);
    }
  #ifdef FP_DEBUG
    FP_END(mem_isRepeatFid)
  #endif
  }else
  {
  #ifdef FP_DEBUG
    FP_BEGIN(FILE_isRepeatFid)
  #endif
    if(m_fileFilterInfoMgr.isRepeatFid(m_rrType,r_fidInfo,
                                       m_fileRrRule->m_timeDuration)==true)
    {
      r_fidInfo.m_uniqFlag = E_RR_REPFLAG_INHIS;
    }
  #ifdef FP_DEBUG
    FP_END(FILE_isRepeatFid)
  #endif
  }
  #ifdef FP_DEBUG
    FP_END(setHisRepeatFlag)
  #endif
  return true;
}

bool FilterInfoMgr::commitFilterInfo()
{
  if(m_rrType==E_RR_RRTYPE_NONE) //不需排重
  { //不需排重，则直接返回
    return true;
  }
  #ifdef FP_DEBUG
    FP_BEGIN(MemInsertFidInfo)
  #endif
  m_memFilterInfoMgr.insertFidInfo(m_memFidInfo);
  #ifdef FP_DEBUG
    FP_END(MemInsertFidInfo)
  #endif
  #ifdef FP_DEBUG
    FP_BEGIN(FileInsertFidInfo)
  #endif
  m_fileFilterInfoMgr.insertFidInfo(m_rrType,m_pFidInfoList);
  #ifdef FP_DEBUG
    FP_END(FileInsertFidInfo)
  #endif
  //插入文件失败，该如何处理。。。。
  //因为有可能是一部份成功，一部分失败了
  //暂定:已经插入的不管,但对应的输入记录,不输出
  m_pFidInfoList.clear();
  m_memFidInfo.clear();
  return true;
}

bool FilterInfoMgr::initialization(const RrConfig &r_config)
{
  if(m_memFilterInfoMgr.initialization(r_config)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_memFilterInfoMgr.initialization false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(m_fileFilterInfoMgr.initialization(r_config)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_fileFilterInfoMgr.initialization false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

bool FilterInfoMgr::end()
{
  m_pFidInfoList.clear();
  m_memFidInfo.clear();
  return true;
}
