#include "MemFilterInfoMgr.h"
#include <iostream>
#include "RrException.h"

BusiFidInfoMgr::BusiFidInfoMgr()
{
}
BusiFidInfoMgr::~BusiFidInfoMgr()
{
  m_fidInfoList.free_map();
}  
//r_memSize为该业务允许的最大内存
bool BusiFidInfoMgr::initialization(const PString &r_fileType,
                                    const PString &r_filePath,
                                    const int     &r_channelNo,
                                    const int     &r_onefileTime)
{
  //初始化Hash列表
  m_fidInfoList.construct_table(RR_HASH_SIZE);
  m_fileType   = r_fileType;
  m_filePath   = r_filePath;
  m_channelNo  = r_channelNo;
  m_onefileTime= r_onefileTime;
  /////////////////////////////////////////////////
  size_t  *t_indexBuff=NULL;
  size_t   t_buffSize = 0;
  //从文件中导入过滤信息(初始化只导半个小时的过滤信息)
  vector<PString> t_fileList;
  char t_fileName[PATH_MAX];
  HisFidInfo t_hisFidInfo;
  size_t t_indexNum;
  FILE *t_pfile;
  long  t_lastTime=0,t_lastTime1;
  vector<PString>::iterator t_itr;
  getLastFileList(m_fileType,t_fileList,m_firstTime);
  for(t_itr=t_fileList.begin();t_itr!=t_fileList.end();++t_itr)
  {
    sprintf(t_fileName,"%s/%s",m_filePath.c_str(),t_itr->c_str());
    t_pfile=fopen(t_fileName,"rb+");
    if(!t_pfile)
    {
      if(t_indexBuff!=NULL)   {delete[] t_indexBuff;t_indexBuff=NULL;}
      return false;
    }
    t_lastTime1 = getLastNodeTime(t_pfile);
    if(t_lastTime<t_lastTime1) t_lastTime = t_lastTime1;
    fclose(t_pfile);
  }
  if(t_lastTime>0) m_firstTime = getTime(t_lastTime,RR_MEMFID_MINTIME);
  else m_firstTime=0;
  for(t_itr=t_fileList.begin();t_itr!=t_fileList.end();++t_itr)
  { //导入该文件的数据
    sprintf(t_fileName,"%s/%s",m_filePath.c_str(),t_itr->c_str());
    t_pfile=fopen(t_fileName,"rb+");
    if(!t_pfile)
    {
      if(t_indexBuff!=NULL)   {delete[] t_indexBuff;t_indexBuff=NULL;}
      return false;
    }
    getAllIdxList(t_pfile,t_indexNum,t_indexBuff,t_buffSize);
    for(size_t i=0;i<t_indexNum;++i)
    {
      getNodeValue(t_pfile,t_indexBuff[i],t_hisFidInfo);
      if(t_lastTime>0 && t_hisFidInfo.m_beginTime>=m_firstTime)
      { //如果该节点在内存时间范围内
        if(m_fidInfoList.insert(t_hisFidInfo)==false)
        {
          fclose(t_pfile);
          #ifdef _DEBUG_
            cout<<"m_fidInfoList.insert(t_hisFidInfo) false!"<<__FILE__<<__LINE__<<endl;
          #endif
          if(t_indexBuff!=NULL)   {delete[] t_indexBuff;t_indexBuff=NULL;}
          return false;
        }
      }
    }
    fclose(t_pfile);
  }
  #ifdef _DEBUG_
  //m_fidInfoList.print_map();
  #endif
  if(t_fileList.empty())  m_firstTime=0;  
  if(t_indexBuff!=NULL)   {delete[] t_indexBuff;t_indexBuff=NULL;}
  return true;
}

//根据时间判断是否在内存时间段内
bool BusiFidInfoMgr::isInMemList(const long& r_beginTime) const
{
  if(m_firstTime < r_beginTime )
  {
    return true;
  }else
  {
    return false;
  }
}
void BusiFidInfoMgr::getMemInfo(MemInnerInfo &r_memInfo) 
{
  r_memInfo.m_busiFidMgr = this;
  r_memInfo.m_firstTime=m_firstTime;
  r_memInfo.m_nodeNumber=m_fidInfoList.getNodeNumber();
  r_memInfo.m_delTimePos=m_firstTime;
}
//判断是否重单
bool BusiFidInfoMgr::isRepeatFid(const int &r_rrType,const FilterInfo& r_fidInfo)
{
  vector<HashList::Bucket*> t_result;
  HisFidInfo  t_tmpFidInfo;
  t_tmpFidInfo = r_fidInfo;
  
  if(m_fidInfoList.lookup(t_tmpFidInfo,t_result)==0)
  {
    return false;
  }
  if(r_rrType==E_RR_RRTYPE_EQUAL || r_rrType==E_RR_RRTYPE_FILE)//changed by hanyu 08-11-28,added a condition : r_rrType == E_RR_REPFLAG_FILE
  {
    return true;
  }else
  { //E_RR_RRTYPE_CONTAIN
    for(vector<HashList::Bucket*>::const_iterator t_itr=t_result.begin();
        t_itr!=t_result.end();++t_itr)
    {
      //判断时间是否交叉
      if((*t_itr)->m_data.m_beginTime >= t_tmpFidInfo.m_beginTime &&
         (*t_itr)->m_data.m_beginTime <  t_tmpFidInfo.m_endTime ||
         (*t_itr)->m_data.m_beginTime <= t_tmpFidInfo.m_beginTime &&
         (*t_itr)->m_data.m_endTime   >  t_tmpFidInfo.m_beginTime )
      {
        return true;
      }
    }
    return false;
  }
}

//插入过滤信息
bool BusiFidInfoMgr::insertFidInfo(const vector<FilterInfo*> &r_filterList)
{
  HisFidInfo  t_tmpFidInfo;
  for(vector<FilterInfo*>::const_iterator t_itr=r_filterList.begin();
      t_itr!=r_filterList.end();++t_itr)
  {
    if((*t_itr)->m_uniqFlag != E_RR_REPFLAG_UNIQ)
    {
      continue;
    }
    t_tmpFidInfo = **t_itr;
    if(m_fidInfoList.insert(t_tmpFidInfo)<0)
    {
      #ifdef _DEBUG_
        cout<<"m_fidInfoList.insert(t_tmpFidInfo) false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      RR_THROW_C(E_RR_MEM_INSERTFID,"内存插入过滤信息错误");
      return false;
    }
  }
  return true;
}
//按照规则删除
size_t BusiFidInfoMgr::deleteNode(const long &r_delTime,
                                  const long &r_remainTime,
                                  const long &r_delNodeNum)
{
  long  t_nodeNum;
  long  t_remainTime;
  t_nodeNum=m_fidInfoList.erase_AI(r_delTime,r_remainTime,r_delNodeNum,t_remainTime);
  if(t_remainTime!=0)
  {
    m_firstTime = t_remainTime;
  }
  return t_nodeNum;
}


MemFilterInfoMgr::MemFilterInfoMgr()
{
}
MemFilterInfoMgr::~MemFilterInfoMgr()
{
  map<PString,BusiFidInfoMgr*>::iterator t_itr;
  for(t_itr  = m_fidInfoList.begin();
      t_itr != m_fidInfoList.end();++t_itr)
  {
    delete t_itr->second;
    t_itr->second=NULL;
  }
  m_fidInfoList.clear();
}

bool MemFilterInfoMgr::setFileType(const PString &r_fileType)
{
  map<PString,BusiFidInfoMgr*>::iterator t_itr;
  
  t_itr=m_fidInfoList.find(r_fileType);
  if(t_itr==m_fidInfoList.end())
  {
    return addFidInfoMgr(r_fileType);
  }else
  {
    m_busiFidInfoMgr=m_fidInfoList[r_fileType];
  }
  return true;
}
bool MemFilterInfoMgr::isInMemList(const int  &r_rrType,const FilterInfo& r_fidInfo,
                                   const long &r_timeDuration)
{
  //如果是包容排重，则必须保证最早跨度的时间也在改范围内
  if(r_rrType==E_RR_RRTYPE_EQUAL )
  { //完全排重
    return m_busiFidInfoMgr->isInMemList(r_fidInfo.m_beginTime);
  }else
  { //文件排重或包容排重
    //取可能最早的时间点
    long t_firstTime=m_busiFidInfoMgr->getTime(r_fidInfo.m_beginTime,-r_timeDuration);
    return m_busiFidInfoMgr->isInMemList(t_firstTime);
  }
}
bool MemFilterInfoMgr::isRepeatFid(const int &r_rrType,const FilterInfo& r_fidInfo)
{
  return m_busiFidInfoMgr->isRepeatFid(r_rrType,r_fidInfo);
}

bool MemFilterInfoMgr::insertFidInfo(const vector<FilterInfo*> &r_filterList)
{
  //插入一个节点的过滤信息
  if(m_busiFidInfoMgr->insertFidInfo(r_filterList)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_busiFidInfoMgr->insertFidInfo(r_filterList) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  //按照一定的规则，删除过期的数据
  deleteOverDue();  
  return true;
}


bool MemFilterInfoMgr::initialization(const RrConfig &r_config)
{
  m_channelNo       = r_config.m_channelNo;
  m_filePath        = r_config.m_fidPath;
  m_stdTimeDuration = r_config.m_fidMemTime;
  m_maxFidNumber    = r_config.m_fidMaxNum;
  m_onefileTime     = r_config.m_onefileTIme;
  m_busiFidInfoMgr  = NULL;
  return true;
}

bool MemFilterInfoMgr::addFidInfoMgr(const PString &r_fileType)
{
  size_t t_maxMemSize=0;
  deleteOverDue();
  //new 一个 BusiFidInfoMgr
  BusiFidInfoMgr *t_busiFidInfoMgr=new BusiFidInfoMgr();
  t_busiFidInfoMgr->initialization(r_fileType,m_filePath,m_channelNo,m_onefileTime);
  m_fidInfoList[r_fileType]=t_busiFidInfoMgr;
  m_busiFidInfoMgr = m_fidInfoList[r_fileType];
  return false;
}

void MemFilterInfoMgr::deleteOverDue()
{
  ////////////淘汰机制///////////////////////////////////
  ///1. 取当前内存中的信息：最早时间、最晚时间、记录数///
  ///2. 找该淘汰的时间点：                            ///
  ///3. 淘汰                                          ///
  ///////////////////////////////////////////////////////
  size_t               t_totalNumber=0;
  vector<MemInnerInfo> t_memInfoList;
  MemInnerInfo         t_memInfo;
  ///1. 取当前内存中的信息：最早时间、记录数///
  for(map<PString,BusiFidInfoMgr*>::iterator 
      t_itr =m_fidInfoList.begin();
      t_itr!=m_fidInfoList.end();++t_itr)
  {
    t_itr->second->getMemInfo(t_memInfo);
    t_totalNumber+=t_memInfo.m_nodeNumber;
    t_memInfoList.push_back(t_memInfo);
  }
  //如果节点数未达到限制值，则不需清理
  if(t_totalNumber<m_maxFidNumber )
  {
    return;
  }
  ///2. 根据信息和记录数找到各业务的淘汰时间点
  computeDelTimePos(t_memInfoList);
  ///3. 进行淘汰处理
  for(vector<MemInnerInfo>::iterator t_memInfoItr=t_memInfoList.begin();
      t_memInfoItr!=t_memInfoList.end();++t_memInfoItr)
  {
    
    t_memInfoItr->m_busiFidMgr->deleteNode(t_memInfoItr->m_delTimePos,
                                           t_memInfoItr->m_remainTimePos,
                                           t_memInfoItr->m_delNodeNum);
    t_memInfoItr->m_firstTime=t_memInfoItr->m_firstTime;
  }
}

//根据各个内存信息，结合允许的最大量和时间跨度
//计算删除时间点
void MemFilterInfoMgr::computeDelTimePos(vector<MemInnerInfo> &r_memInfoList)
{
  ////////////////////////////////////////////////////////////////
  /////1. 计算清理条数////////////////////////////////////////////
  //1. 将 m_stdTimeDuration前的过滤信息清除
  //2. 如果在RR_MEMFID_MINTIME内的不清除
  //3. 如果在[m_stdTimeDuration,RR_MEMFID_MINTIME]，
  //   若记录数未减少到 m_maxFidNumber*80% 则清除到80%
  ////////////////////////////////////////////////////////////////
  long  t_nowTime,t_firstTime,t_lastTime;
  t_nowTime = HisFilterInfoMgr::getCurTime();
  t_firstTime= HisFilterInfoMgr::getTime(t_nowTime,-m_stdTimeDuration);
  t_lastTime = HisFilterInfoMgr::getTime(t_nowTime,-RR_MEMFID_MINTIME);
  for(vector<MemInnerInfo>::iterator t_itr=r_memInfoList.begin();
      t_itr!=r_memInfoList.end();++t_itr)
  {
    t_itr->m_delTimePos    = t_firstTime;
    t_itr->m_remainTimePos = t_lastTime;
    //删除20%
    t_itr->m_delNodeNum = (t_itr->m_nodeNumber*20/100);
  }
}

