#include "PageMemMgr.h"

PageMemMgr::PageMemMgr()
{
  m_pHeader = NULL;
}
PageMemMgr::~PageMemMgr()
{
  m_pHeader = NULL;
}

//r_size包括pageListinfo和下面的列表
bool PageMemMgr::f_init(unsigned int &r_spaceCode,
                               char * r_spAddr,
                        const size_t &r_offSet,
                        const size_t &r_size,
                        const size_t &r_pageSize)
{
  if(r_pageSize<=sizeof(PageInfo))
  {
    #ifdef _DEBUG_
      cout<<"r_pageSize="<<r_pageSize<<" 值过小!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  attach_init(r_spaceCode,r_spAddr,r_offSet);
  size_t pageNum=(r_size-sizeof(PageListInfo))/r_pageSize;
  //1.初始化页列表信息
  m_pHeader->m_pageSize = r_pageSize;
  m_pHeader->m_idleNum  = m_pHeader->m_pageNum=pageNum;
  if(pageNum<=0)
  {
    m_pHeader->m_lIdlePage=m_pHeader->m_fIdlePage=-1;
  }else
  {
    m_pHeader->m_fIdlePage= r_offSet+sizeof(PageListInfo);
    m_pHeader->m_lIdlePage= m_pHeader->m_fIdlePage+(pageNum-1)*r_pageSize;
  }
  //2.初始化各个页面信息
  PageInfo *t_pPage;
  size_t    t_pagePos;
  t_pagePos=m_pHeader->m_fIdlePage;
  for(size_t i=0;i<m_pHeader->m_pageNum;i++)
  {
    t_pPage = (PageInfo *)(r_spAddr+t_pagePos);
    t_pPage->m_spaceCode=r_spaceCode;
    t_pPage->m_pos=t_pagePos;
    t_pPage->m_pageSize=r_pageSize;
    t_pPage->m_nextNotNullPage=NULL_SHMPOS;
    if(i==m_pHeader->m_pageNum-1)
    {
      t_pPage->m_next=NULL_SHMPOS;
    }else
    {
      if(t_pPage->m_next.setValue(r_spaceCode,t_pagePos+r_pageSize)==false)
      {
        #ifdef _DEBUG_
          cout<<"t_pPage->m_next.setValue false!"<<__FILE__<<__LINE__<<endl;
        #endif
        return false;
      }
    }
    t_pagePos += r_pageSize;
  }
  //dumpSpaceInfo(cout);
  return true;
}
void PageMemMgr::attach_init(unsigned int &r_spaceCode,char * r_spAddr,
                             const size_t &r_offSet)
{
  m_spaceCode=r_spaceCode;
  m_spAddr  = r_spAddr;
  m_sOffSet = r_offSet;
  m_pHeader = (PageListInfo*)(m_spAddr+m_sOffSet);
  return;
}

size_t  PageMemMgr::getIdlePageNum()
{
  if(m_pHeader==NULL) return 0;
  return m_pHeader->m_idleNum;
}
bool  PageMemMgr::freePage(const ShmPosition &r_pagePos)
{
  //1.初始化页面指针
  PageInfo *t_pPageInfo;
  PageInfo *t_pPrevPageInfo;
  size_t t_pageOffSet=r_pagePos.getOffSet();
  t_pPageInfo = (PageInfo*)(m_spAddr+t_pageOffSet);
  if(t_pPageInfo->m_pos != t_pageOffSet)
  { //校验：是否该页面
    #ifdef _DEBUG_
      cout<<"t_pPageInfo->m_pos="<<t_pPageInfo->m_pos
          <<" t_pageOffSet="<<t_pageOffSet
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  //2.将页面插入空闲队列
  if(m_pHeader->m_lIdlePage==-1)
  { //第一个空闲页面
    m_pHeader->m_fIdlePage=m_pHeader->m_lIdlePage=t_pageOffSet;
    t_pPageInfo->m_next=NULL_SHMPOS;
  }else
  { //追加在空闲段最后
    t_pPrevPageInfo=(PageInfo*)(m_spAddr+m_pHeader->m_lIdlePage);
    m_pHeader->m_lIdlePage=t_pageOffSet;
    t_pPrevPageInfo->m_next=r_pagePos;
    t_pPageInfo->m_next=NULL_SHMPOS;
  }
  //3.空闲页面数+1;
  m_pHeader->m_idleNum=m_pHeader->m_idleNum+1;
  return true;  
}

//申请页面
bool PageMemMgr::allocatePage(PageInfo* &r_pPage,ShmPosition &r_pagePos)
{
  if(m_pHeader->m_idleNum<=0)
  {
    return false;
  }
  //从头部取一个页面
  r_pPage  = (PageInfo*)(m_spAddr+m_pHeader->m_fIdlePage);
  r_pagePos.setValue(m_spaceCode,m_pHeader->m_fIdlePage);
  
  if(r_pPage->m_next == NULL_SHMPOS)
  { //最后一段
    m_pHeader->m_fIdlePage=m_pHeader->m_lIdlePage=-1;
  }else
  {
    m_pHeader->m_fIdlePage = r_pPage->m_next.getOffSet();
  }
  m_pHeader->m_idleNum--;
  return true;
}
//通过Slot位置求取对应页面位置
bool PageMemMgr::getPagePosBySlotPos(const size_t &r_slotPos,size_t &r_pagePos)
{
  //计算相对首个PAGE的偏移量
  size_t t_offSet = r_slotPos-m_sOffSet-sizeof(PageListInfo);
  //计算该Slot属于第几个page(0.1.2....)
  size_t t_num = t_offSet/(m_pHeader->m_pageSize);
  r_pagePos = m_sOffSet+sizeof(PageListInfo)+t_num*m_pHeader->m_pageSize;
  return true;
}
bool PageMemMgr::dumpSpaceInfo(ostream &r_os)
{
  r_os<<"-------页列表信息开始---------------"<<endl;
  r_os<<"m_pageSize ="<<m_pHeader->m_pageSize<<endl;
  r_os<<"m_pageNum  ="<<m_pHeader->m_pageNum<<endl;
  r_os<<"m_idleNum  ="<<m_pHeader->m_idleNum<<endl;
  r_os<<"m_fIdlePage="<<m_pHeader->m_fIdlePage<<endl;
  r_os<<"m_lIdlePage="<<m_pHeader->m_lIdlePage<<endl;
  r_os<<"-------页列表信息终止---------------"<<endl;
  PageInfo  *t_pPage;

  r_os<<"-------所有页面信息开始-------------"<<endl;
  char * t_pPage0 = ((char *)m_pHeader)+sizeof(PageListInfo);
  for(size_t i=0;i<m_pHeader->m_pageNum;i++)
  {
    t_pPage = (PageInfo *)(t_pPage0+i*m_pHeader->m_pageSize);
    r_os<<"m_spaceCode      ="<<t_pPage->m_spaceCode<<endl;
    r_os<<"m_pos            ="<<t_pPage->m_pos<<endl;
    r_os<<"m_next           ="<<t_pPage->m_next<<endl;
    r_os<<"m_nextNotNullPage="<<t_pPage->m_nextNotNullPage<<endl;
    r_os<<"m_pageSize       ="<<t_pPage->m_pageSize<<endl;
  }
  r_os<<"-------所有页面结束开始-------------"<<endl;
  r_os<<"-------空闲页面信息-----------------"<<endl;
  size_t    t_idlePage=0;
  if(m_pHeader->m_fIdlePage!=-1)
  {
    t_pPage=(PageInfo*)(m_spAddr+m_pHeader->m_fIdlePage);
    while(1)
    {
      t_idlePage++;
      r_os<<"m_spaceCode      ="<<t_pPage->m_spaceCode<<endl;
      r_os<<"m_pos            ="<<t_pPage->m_pos<<endl;
      r_os<<"m_next           ="<<t_pPage->m_next<<endl;
      r_os<<"m_nextNotNullPage="<<t_pPage->m_nextNotNullPage<<endl;
      r_os<<"m_pageSize       ="<<t_pPage->m_pageSize<<endl;
      if(t_pPage->m_next == NULL_SHMPOS) break;
      t_pPage=(PageInfo*)(m_spAddr+t_pPage->m_next.getOffSet());
    };
    if(m_pHeader->m_idleNum!=t_idlePage)
    {
      cout<<"空闲页面数和空闲页面列表不符!"<<endl;
      cout<<"m_pHeader->m_idleNum = "<<m_pHeader->m_idleNum<<endl;
      cout<<"t_idlePage = "<<t_idlePage<<endl;
    }
  }
  return true;
}

float PageMemMgr::getTableSpaceUsedPercent()
{
	PageInfo *pPageInfo;
	
	pPageInfo = (PageInfo*)(m_spAddr+m_pHeader->m_fIdlePage);	
	if(m_pHeader->m_fIdlePage==-1)
	{
		return 1;	
	}
	
	int iFreePagesCount=0;
	while(pPageInfo->m_next != NULL_SHMPOS)
	{ 
		++iFreePagesCount;
		pPageInfo = (PageInfo*)(m_spAddr+pPageInfo->m_next.getOffSet());
	}
	
	float fPercent=1.0-(float)iFreePagesCount/(m_pHeader->m_pageNum);
	
	return fPercent;
}
