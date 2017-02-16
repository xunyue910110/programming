#include "TableSpaceMgr.h"

TableSpaceMgr::TableSpaceMgr(SpaceInfo &r_spaceInfo)
{
  SpaceMgrBase::initialize(r_spaceInfo);
}
TableSpaceMgr::~TableSpaceMgr()
{
}

/**
 *createTbSpace 创建表空间.
 *@return  true 创建成功,false 失败
 */
bool TableSpaceMgr::createTbSpace(const int &r_flag)
{
  if(createSpace(m_spaceHeader,r_flag)==false)
  {
    #ifdef _DEBUG_
      cout<<"createSpace false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}
bool TableSpaceMgr::deleteTbSpace()
{
  if(deleteSpace(m_spaceHeader)==false)
  {
    #ifdef _DEBUG_
      cout<<"createSpace false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}


//初始化表空间
bool TableSpaceMgr::initTbSpace(const int &r_flag)
{
  if(r_flag==0)
  {
    return f_initTbSpace();
  }else
  {
    if(loadDataFromFile()==false)
    {
      #ifdef _DEBUG_
        cout<<"loadDataFromFile() false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    size_t  t_offSet=0;
    //1. 跳过块头信息m_spaceHeader
    t_offSet+=sizeof(SpaceInfo);
    //2.初始化页列表信息
    m_pageMemMgr.attach_init(m_spaceHeader.m_spaceCode,
                             m_spaceHeader.m_shmAddr,t_offSet);
    return true;
  }
}
//第一次初始化表空间
bool TableSpaceMgr::f_initTbSpace()
{
  size_t  t_offSet=0;
  //1. 初始化块头信息m_spaceHeader
  memcpy(m_pSpHeader,&m_spaceHeader,sizeof(SpaceInfo));t_offSet+=sizeof(SpaceInfo);
  //2.初始化页列表信息
  if(m_pageMemMgr.f_init(m_spaceHeader.m_spaceCode,
                         m_spaceHeader.m_shmAddr,t_offSet,
                         m_spaceHeader.m_size-t_offSet,
                         m_spaceHeader.m_pageSize)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_pageMemMgr.f_init false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

//attach方式初始化表空间
bool TableSpaceMgr::attach_init()
{
  if(SpaceMgrBase::attach()==false)
  {
    return false;
  }
  size_t  t_offSet=0;
  //1. 跳过块头信息m_spaceHeader
  t_offSet+=sizeof(SpaceInfo);
  //2.初始化页列表信息
  m_pageMemMgr.attach_init(m_spaceHeader.m_spaceCode,
                           m_spaceHeader.m_shmAddr,t_offSet);
  return true;
}

//申请页面
bool TableSpaceMgr::allocatePage(PageInfo* &r_pPage,ShmPosition &r_pagePos)
{
  bool t_bret=true;
  
  try
  {
    if(m_pdbLock->getWriteLock()==false)
    {
      #ifdef _DEBUG_
        cout<<"m_pdbLock->getWriteLock() false!"<<endl;
      #endif
      t_bret=false;return t_bret;
    }
  }catch(Mdb_Exception &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    return false;
  }
  if(m_pageMemMgr.allocatePage(r_pPage,r_pagePos)==false)
  {
    t_bret=false;
  }
  m_pdbLock->releaseWriteLock();
  return t_bret;
}

//回收本Space内的page
bool TableSpaceMgr::freePage(const ShmPosition &r_pagePos)
{
  bool t_bret=true;
  
  try
  {
    if(m_pdbLock->getWriteLock()==false)
    {
      #ifdef _DEBUG_
        cout<<"m_pdbLock->getWriteLock() false!"<<endl;
      #endif
      t_bret=false;return t_bret;
    }
  }catch(Mdb_Exception &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    return false;
  }
  if(m_pageMemMgr.freePage(r_pagePos)==false)
  {
    t_bret=false;
  }
  m_pdbLock->releaseWriteLock();
  return t_bret;
}
//通过Slot位置求取对应页面位置
bool TableSpaceMgr::getPagePosBySlotPos(const size_t &r_slotPos,size_t &r_pagePos)
{
  if(r_slotPos>m_spaceHeader.m_size)
  {
    return false;
  }
  return m_pageMemMgr.getPagePosBySlotPos(r_slotPos,r_pagePos);
}

//调试接口
bool TableSpaceMgr::dumpSpaceInfo(ostream &r_os)
{
  r_os<<"---------数据表空间:"<<m_pSpHeader->m_spaceName<<"内容 起始!---------"<<endl;
  SpaceMgrBase::dumpSpaceInfo(r_os);
  m_pageMemMgr.dumpSpaceInfo(r_os);
  r_os<<"---------数据表空间:"<<m_pSpHeader->m_spaceName<<"内容 终止!---------"<<endl;
  return true;
}

float TableSpaceMgr::getTableSpaceUsedPercent()
{
	return m_pageMemMgr.getTableSpaceUsedPercent();	
}
