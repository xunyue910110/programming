#include "IndexMemMgr.h"
#include "TableDescript.h"

IndexMemMgr::IndexMemMgr()
{
}
IndexMemMgr::~IndexMemMgr()
{
}

bool IndexMemMgr::initialize(SpaceInfo &r_spaceInfo)
{
  SpaceMgrBase::initialize(r_spaceInfo);
  return true;
}
//根据表空间定义信息,创建表空间
bool IndexMemMgr::createIdxSpace(SpaceInfo &r_spaceInfo,const int &r_flag)
{
  if(createSpace(m_spaceHeader,r_flag)==false)
  {
    #ifdef _DEBUG_
      cout<<"createSpace false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  r_spaceInfo=*(getSpaceInfo());
      
  return true;
}
bool IndexMemMgr::deleteIdxSpace()
{
  return deleteSpace(m_spaceHeader);
} 
//初始化表空间
bool IndexMemMgr::initSpaceInfo(const int &r_flag)
{
  if(r_flag==0)
  {
    return f_init();
  }
  //否则从文件中读取
  if(loadDataFromFile()==false)
  {
    #ifdef _DEBUG
      cout<<"loadDataFromFile() false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  size_t  t_offSet=0;
  //1. 跳过块头信息m_spaceHeader
  t_offSet+=sizeof(SpaceInfo);
  
  //2.attach初始化页列表信息
  m_pIdxSegInfo = (IndexSegInfo*)(m_spaceHeader.m_shmAddr+t_offSet);
  t_offSet+=sizeof(IndexSegInfo);
  return true;
}

bool IndexMemMgr::f_init()
{
  if(m_spaceHeader.m_size<sizeof(SpaceInfo)+sizeof(IndexSegInfo)+sizeof(HashIndexSeg))
  {
    #ifdef _DEBUG_
      cout<<"索引表空间数值太小:"<<m_spaceHeader.m_size
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  
  //初始化为一个大段
  size_t  t_offSet=0;
  //1. 初始化块头信息m_spaceHeader
  memcpy(m_pSpHeader,&m_spaceHeader,sizeof(SpaceInfo));t_offSet+=sizeof(SpaceInfo);
  //2.设置段列表信息
  m_pIdxSegInfo = (IndexSegInfo*)(m_spaceHeader.m_shmAddr+t_offSet);
  t_offSet+=sizeof(IndexSegInfo);
  m_pIdxSegInfo->m_lIdleSeg=m_pIdxSegInfo->m_fIdleSeg=t_offSet;
  m_pIdxSegInfo->m_fUsedSeg=m_pIdxSegInfo->m_lUsedSeg=-1;
  //3.初始化段信息
  HashIndexSeg * t_pSegInfo;
  t_pSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_offSet);
  t_pSegInfo->m_sOffSet = t_offSet;
  t_pSegInfo->m_segSize = m_spaceHeader.m_size-t_offSet;
  t_pSegInfo->m_prev=t_pSegInfo->m_next=-1;
  return true;
}
//attach方式init
bool IndexMemMgr::attach_init(SpaceInfo &r_spaceInfo)
{
  initialize(r_spaceInfo);
  if(SpaceMgrBase::attach()==false)
  {
    return false;
  }
  size_t  t_offSet=0;
  //1. 跳过块头信息m_spaceHeader
  t_offSet+=sizeof(SpaceInfo);
  
  //2.attach初始化页列表信息
  m_pIdxSegInfo = (IndexSegInfo*)(m_spaceHeader.m_shmAddr+t_offSet);
  t_offSet+=sizeof(IndexSegInfo);
  r_spaceInfo=m_spaceHeader;
  return true;
}

//r_shmPos是指向Hash中ShmPositioin[]的首地址
bool IndexMemMgr::createHashIndex(IndexDesc * r_idxDesc,ShmPosition &r_shmPos)
{
  bool t_bRet=true;
  try
  {
      if(m_pdbLock->getWriteLock()==false)
      {
        #ifdef _DEBUG_
          cout<<"m_pdbLock->getWriteLock() false!"<<endl;
        #endif
        t_bRet=false;return t_bRet;
      }
    bool t_flag=false; //是否有足够的空间
    if(m_pIdxSegInfo->m_fIdleSeg==-1)
    {
      #ifdef _DEBUG_
        cout<<"索引空间空闲队列为空!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet=false;
      throw Mdb_Exception(__FILE__, __LINE__, "索引空间空闲队列为空!");
    }
    size_t         t_needSize;
    HashIndexSeg * t_pSegInfo;
    HashIndexSeg * t_prevSegInfo;
    HashIndexSeg * t_nextSegInfo;
    //0.计算分配段的最小空间t_needSize
    t_needSize=sizeof(HashIndexSeg)+r_idxDesc->m_indexDef.m_hashSize*sizeof(ShmPosition);
    //1.找到满足分配条件的空闲段t_pSegInfo
    t_pSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_fIdleSeg);
    while(1)
    {
      if(t_pSegInfo->m_segSize >= t_needSize)
      {
        t_flag=true;
        break;
      }
      if(t_pSegInfo->m_next==-1) break;
      t_pSegInfo =(HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_next); 
    };
    if(t_flag==false)
    {
      #ifdef _DEBUG_
        cout<<"索引空间没有足够的连续空闲空间!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet=false;
      throw Mdb_Exception(__FILE__, __LINE__, "索引空间没有足够的连续空闲空间!");
    }
    if(t_pSegInfo->m_segSize <(t_needSize+2*sizeof(HashIndexSeg)))
    { //2.如果空间刚好(浪费空间<2*sizeof(HashIndexSeg)，则从空闲队列中移出
      if(t_pSegInfo->m_prev==-1)
      { //第一段
        m_pIdxSegInfo->m_fIdleSeg = t_pSegInfo->m_next;
      }else
      {
        t_prevSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_prev);
        t_prevSegInfo->m_next=t_pSegInfo->m_next;
      }
      if(t_pSegInfo->m_next==-1)
      { //最后一段
        m_pIdxSegInfo->m_lIdleSeg = t_pSegInfo->m_prev;
      }else
      {
        t_nextSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_next);
        t_nextSegInfo->m_prev = t_pSegInfo->m_prev;
      }
    }else
    {
      //3.否则，分割该段
      t_prevSegInfo=t_pSegInfo;
      t_pSegInfo   =(HashIndexSeg*)(((char*)t_prevSegInfo)+(t_prevSegInfo->m_segSize - t_needSize));
      t_prevSegInfo->m_segSize=t_prevSegInfo->m_segSize - t_needSize;
      t_pSegInfo->m_sOffSet=t_prevSegInfo->m_sOffSet+t_prevSegInfo->m_segSize;
      t_pSegInfo->m_segSize=t_needSize;
    }
    //设置m_pIdxSegInfo信息
    t_pSegInfo->m_hashSize = r_idxDesc->m_indexDef.m_hashSize;
    memcpy(t_pSegInfo->m_idxName,r_idxDesc->m_indexDef.m_indexName,sizeof(T_NAMEDEF));
    //4.将分配的段插入已使用段尾部
    if(m_pIdxSegInfo->m_lUsedSeg==-1)
    {//第一段
       m_pIdxSegInfo->m_lUsedSeg=m_pIdxSegInfo->m_fUsedSeg=t_pSegInfo->m_sOffSet;
       t_pSegInfo->m_prev=t_pSegInfo->m_next=-1;
    }else
    {
      t_pSegInfo->m_prev=m_pIdxSegInfo->m_lUsedSeg;
      t_pSegInfo->m_next=-1;
      t_prevSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_lUsedSeg);
      m_pIdxSegInfo->m_lUsedSeg=t_prevSegInfo->m_next=t_pSegInfo->m_sOffSet;
    }
    //5.设置新段的ShmPosition地址
    if(r_shmPos.setSpaceCode(m_spaceHeader.m_spaceCode)==false)
    {
      t_bRet=false;
      throw Mdb_Exception(__FILE__, __LINE__, "r_shmPos.setSpaceCode false!");
    }
    if(r_shmPos.setOffSet(t_pSegInfo->m_sOffSet+sizeof(HashIndexSeg))==false)
    {
      t_bRet=false;
      throw Mdb_Exception(__FILE__, __LINE__, "r_shmPos.setOffSet false!");
    }
    t_pSegInfo->initHash((char*)t_pSegInfo);
    m_pdbLock->releaseWriteLock();
    return t_bRet;
  }catch(Mdb_Exception &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    m_pdbLock->releaseWriteLock();
    return false;
  }
}
//r_shmPos是指向Hash中ShmPositioin[]的首地址
void IndexMemMgr::initHashSeg(const ShmPosition &r_shmPos)
{
  HashIndexSeg * t_pSegInfo;
  t_pSegInfo=(HashIndexSeg*)(m_spaceHeader.m_shmAddr+r_shmPos.getOffSet()-sizeof(HashIndexSeg));
  t_pSegInfo->initHash((char*)t_pSegInfo);
}

//r_shmPos是指向Hash中ShmPositioin[]的首地址
bool IndexMemMgr::dropHashIdex(const ShmPosition &r_shmPos)
{
  bool t_bRet=true;
  try
  {
      if(m_pdbLock->getWriteLock()==false)
      {
        #ifdef _DEBUG_
          cout<<"m_pdbLock->getWriteLock() false!"<<endl;
        #endif
        t_bRet=false;return t_bRet;
      }
    bool t_flag;
    //1. 计算HashIndexSeg的起始位置
    size_t t_segOffSet;
    t_segOffSet  = r_shmPos.getOffSet();
    t_segOffSet -= sizeof(HashIndexSeg);
    //2.从已使用的段列表中找到对应的段
    if(m_pIdxSegInfo->m_fUsedSeg == -1)
    {
      #ifdef _DEBUG_
        cout<<"索引空间已使用队列为空!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "索引空间已使用队列为空!");
    }
    HashIndexSeg * t_pSegInfo;
    t_pSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_fUsedSeg);
    t_flag=false;
    while(1)
    {
      if(t_pSegInfo->m_sOffSet==t_segOffSet)
      {
        t_flag=true;
        break;
      }
      if(t_pSegInfo->m_next==-1) break;
      t_pSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_next);
    };
    if(t_flag==false)
    {
      #ifdef _DEBUG_
        cout<<"索引空间已使用队列未找到相应的段!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "索引空间已使用队列未找到相应的段!");
    }
    HashIndexSeg * t_pPrevSegInfo,*t_pNextSegInfo;
    //3.将该段从已使用记录中移出
    if(t_pSegInfo->m_sOffSet==m_pIdxSegInfo->m_fUsedSeg)
    { //第一段
      m_pIdxSegInfo->m_fUsedSeg=t_pSegInfo->m_next;
    }else
    {
      t_pPrevSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_prev);
      t_pPrevSegInfo->m_next=t_pSegInfo->m_next;
    }
    if(t_pSegInfo->m_sOffSet == m_pIdxSegInfo->m_lUsedSeg)
    { //最后一段
      m_pIdxSegInfo->m_lUsedSeg=t_pSegInfo->m_prev;
    }else
    {
      t_pNextSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pSegInfo->m_next);
      t_pNextSegInfo->m_prev=t_pSegInfo->m_prev;
    }
    //4.将该段加入空闲队列
    if(m_pIdxSegInfo->m_fIdleSeg==-1)
    { //第一段
      m_pIdxSegInfo->m_lIdleSeg=m_pIdxSegInfo->m_fIdleSeg=t_pSegInfo->m_sOffSet;
      t_pSegInfo->m_prev=t_pSegInfo->m_next=-1;
    }else
    {
      //4.1 找到偏移量比t_pSegInfo大的第一个段
      t_flag=false;
      t_pNextSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_fIdleSeg);
      while(1)
      {
        if(t_pSegInfo->m_sOffSet<t_pNextSegInfo->m_sOffSet)
        {
          t_flag=true;
          break;
        }
        //if(t_pSegInfo->m_next==-1) break; modified by chenm 2008-11-6 This is a bug!
        if(t_pNextSegInfo->m_next==-1) break;
        t_pNextSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pNextSegInfo->m_next);
      };    
      if(t_flag==false)
      {
        //看是否能和最后一段合并
        t_pPrevSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_lIdleSeg);
        if(t_pPrevSegInfo->m_sOffSet+t_pPrevSegInfo->m_segSize==t_pSegInfo->m_sOffSet)
        {
          //t_pPrevSegInfo和t_pSegInfo合并，并用t_pSegInfo来计合并后的信息
          t_pPrevSegInfo->m_segSize=t_pPrevSegInfo->m_segSize+t_pSegInfo->m_segSize;
          t_pPrevSegInfo->m_next   =t_pSegInfo->m_next;
          t_pSegInfo=t_pPrevSegInfo;
        }else
        {//追到到最后一段
          t_pSegInfo->m_prev=m_pIdxSegInfo->m_lIdleSeg;
          t_pSegInfo->m_next=-1;
          t_pPrevSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_lIdleSeg);
          m_pIdxSegInfo->m_lIdleSeg=t_pPrevSegInfo->m_next=t_pSegInfo->m_sOffSet;
        }
      }else
      {//加在t_pNextSegInfo前
        t_pPrevSegInfo = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+t_pNextSegInfo->m_prev);
        t_pPrevSegInfo->m_next=t_pSegInfo->m_sOffSet;
        t_pSegInfo->m_prev=t_pPrevSegInfo->m_sOffSet;
        t_pSegInfo->m_next=t_pNextSegInfo->m_sOffSet;
        t_pNextSegInfo->m_prev=t_pSegInfo->m_sOffSet;
        //5.合并空闲队列
        //5.1 看t_pPrevSegInfo能否和t_pSegInfo合并
        if(t_pPrevSegInfo->m_sOffSet+t_pPrevSegInfo->m_segSize==t_pSegInfo->m_sOffSet)
        {
          //t_pPrevSegInfo和t_pSegInfo合并，并用t_pSegInfo来计合并后的信息
          t_pPrevSegInfo->m_segSize=t_pPrevSegInfo->m_segSize+t_pSegInfo->m_segSize;
          t_pPrevSegInfo->m_next   =t_pSegInfo->m_next;
          t_pSegInfo=t_pPrevSegInfo;
        }
        //5.2 看t_pSegInfo能否和t_pNextSegInfo合并
        if(t_pSegInfo->m_sOffSet+t_pSegInfo->m_segSize==t_pNextSegInfo->m_sOffSet)
        {
          //t_pPrevSegInfo和t_pSegInfo合并，并用t_pSegInfo来计合并后的信息
          t_pSegInfo->m_segSize=t_pSegInfo->m_segSize+t_pNextSegInfo->m_segSize;
          t_pSegInfo->m_next   =t_pNextSegInfo->m_next;
          if(t_pSegInfo->m_next==-1)
          { //如果合并后是最后一段，则调整m_lIdleSeg值
            m_pIdxSegInfo->m_lIdleSeg = t_pSegInfo->m_sOffSet;
          }
        }
      }
    }
    m_pdbLock->releaseWriteLock();
    return t_bRet;
  }catch (Mdb_Exception e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    m_pdbLock->releaseWriteLock();
    return false;
  }
}
bool IndexMemMgr::dumpSpaceInfo(ostream &r_os)
{
  r_os<<"---------索引表空间:"<<m_pSpHeader->m_spaceName<<"内容 起始!---------"<<endl;
  SpaceMgrBase::dumpSpaceInfo(r_os);
  r_os<<"---------索引段信息----------------------"<<endl;
  r_os<<"m_fIdleSeg = "<<m_pIdxSegInfo->m_fIdleSeg<<endl;
  r_os<<"m_lIdleSeg = "<<m_pIdxSegInfo->m_lIdleSeg<<endl;
  r_os<<"m_fUsedSeg = "<<m_pIdxSegInfo->m_fUsedSeg<<endl;
  r_os<<"m_lUsedSeg = "<<m_pIdxSegInfo->m_lUsedSeg<<endl;
  r_os<<"------已使用索引段信息----------------"<<endl;
  HashIndexSeg *t_phashIndex ;
  
  if(m_pIdxSegInfo->m_fUsedSeg!=-1)
  {
    t_phashIndex = (HashIndexSeg*)(getSpaceAddr()+m_pIdxSegInfo->m_fUsedSeg);
    while(1)
    {
      r_os<<"--------索引："<<t_phashIndex->m_idxName<<"----"<<endl;
      r_os<<"m_sOffSet ="<<t_phashIndex->m_sOffSet<<endl;
      r_os<<"m_segSize ="<<t_phashIndex->m_segSize<<endl;
      r_os<<"m_hashSize="<<t_phashIndex->m_hashSize<<endl;
      r_os<<"m_prev    ="<<t_phashIndex->m_prev<<endl;
      r_os<<"m_next    ="<<t_phashIndex->m_next<<endl;
      if(t_phashIndex->m_next==-1) break;
      t_phashIndex = (HashIndexSeg*)(getSpaceAddr()+t_phashIndex->m_next);
    };
  }
  r_os<<"------空闲索引段信息--－－-------------"<<endl;
  if(m_pIdxSegInfo->m_fIdleSeg!=-1)
  {
    t_phashIndex = (HashIndexSeg*)(getSpaceAddr()+m_pIdxSegInfo->m_fIdleSeg);
    while(1)
    {
      r_os<<"--------索引："<<t_phashIndex->m_idxName<<"----"<<endl;
      r_os<<"m_sOffSet ="<<t_phashIndex->m_sOffSet<<endl;
      r_os<<"m_segSize ="<<t_phashIndex->m_segSize<<endl;
      r_os<<"m_hashSize="<<t_phashIndex->m_hashSize<<endl;
      r_os<<"m_prev    ="<<t_phashIndex->m_prev<<endl;
      r_os<<"m_next    ="<<t_phashIndex->m_next<<endl;
      if(t_phashIndex->m_next==-1) break;
      t_phashIndex = (HashIndexSeg*)(getSpaceAddr()+t_phashIndex->m_next);
    };
  }
  r_os<<"---------索引表空间:"<<m_pSpHeader->m_spaceName<<"内容 终止!---------"<<endl;
  return true;
}

void IndexMemMgr::getTableSpaceUsedPercent(map<string,float> &vUserdPercent,const char * cTableSpaceName)
{

	HashIndexSeg *pHashIndexSeg;
	size_t totalFreeSegSize = 0;
	float fPercent;
	
	if(m_pIdxSegInfo->m_fIdleSeg==-1)
	{
		fPercent = 1;
	}
	else
	{
		pHashIndexSeg = (HashIndexSeg*)(m_spaceHeader.m_shmAddr+m_pIdxSegInfo->m_fIdleSeg);
	    while(1)
	    {
	    	totalFreeSegSize += pHashIndexSeg->m_segSize ;
	    	
	    	if(pHashIndexSeg->m_next==-1) 
				break;
	    	pHashIndexSeg =(HashIndexSeg*)(m_spaceHeader.m_shmAddr+pHashIndexSeg->m_next); 
	    }
	    fPercent = 1 - (float)totalFreeSegSize/m_spaceHeader.m_size;
	}
	
	vUserdPercent.insert(map<string,float>::value_type(m_spaceHeader.m_spaceName,fPercent));
	return;	
}
