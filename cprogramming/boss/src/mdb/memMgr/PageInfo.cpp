#include "PageInfo.h"


//申请r_num个元素，r_slotAddrList初始化在外部，该处只push_back
//在外部已经判断有足够的元素提供分配。
void PageInfo::allocateNode(char* t_pageAddr,const int &r_num,
                            vector<MdbAddress>& r_slotAddrList,
                            int &r_newNum)
{
  r_newNum=0;
  if(m_fIdleSlot==-1) return;
  
  // add by chenm 2009-3-1 3:03:51
  if(m_fIdleSlot>m_pageSize+m_pos)
  {
  	m_idleNum = 0;
  	m_fIdleSlot=-1;
  	return;	
  }
  // over 2009-3-1 3:03:55	
  
  static size_t  t_nullPos = -1;
  MdbAddress  t_addr;
  IdleSlot   *t_pSlot;
  t_addr.m_pos.setSpaceCode(m_spaceCode);
  while(m_fIdleSlot!=-1 && r_num>r_newNum)
  {
    t_addr.m_pos.setOffSet(m_fIdleSlot);
    t_addr.m_addr=t_pageAddr+(m_fIdleSlot-m_pos);
    t_pSlot = (IdleSlot*)(t_addr.m_addr);
    //m_fIdleSlot = t_pSlot->m_next;
    memcpy(&m_fIdleSlot,&(t_pSlot->m_next),sizeof(size_t));
    ++r_newNum;--m_idleNum;
    r_slotAddrList.push_back(t_addr);
  };
  if(m_idleNum==0)
  {
    //m_fIdleSlot=m_lIdleSlot=-1;
    memcpy(&m_fIdleSlot,&t_nullPos,sizeof(size_t));
    memcpy(&m_lIdleSlot,&t_nullPos,sizeof(size_t));
  }
  return;
}

//初始化
void PageInfo::initSlotList(char* t_pageAddr,const int &r_slotSize)
{
  m_slotSize = r_slotSize;
  m_slotNum  = (m_pageSize-sizeof(PageInfo))/m_slotSize;
  m_idleNum  = m_slotNum;
  m_fIdleSlot= m_pos+sizeof(PageInfo);
  m_lIdleSlot= m_fIdleSlot+(m_slotNum-1)*r_slotSize;
  IdleSlot *t_pSlot =(IdleSlot*)(t_pageAddr+sizeof(PageInfo));
  size_t    t_offSet=m_fIdleSlot;
  size_t    t_offSet2=0;
  for(size_t i=0;i<m_slotNum;i++)
  {
    if(i==m_slotNum-1)
    {
      //t_pSlot->m_next=size_t(-1);
      t_offSet2 = size_t(-1);
      memcpy(&(t_pSlot->m_next),&t_offSet2,sizeof(size_t));
      break;
    }else
    {
      //t_pSlot->m_next = t_offSet+r_slotSize;
      //t_offSet += r_slotSize;
      t_offSet2 = t_offSet+r_slotSize;
      memcpy(&(t_pSlot->m_next),&t_offSet2,sizeof(size_t));
      t_offSet = t_offSet2;
      t_pSlot = (IdleSlot*)(((char*)t_pSlot)+r_slotSize);
    }
  }
}
//释放Slot
void PageInfo::freeNode(char* t_pageAddr,const size_t &r_slotPos)
{
  IdleSlot *t_pSlot,*t_prevSlot;
  t_pSlot = (IdleSlot*)(t_pageAddr+r_slotPos-m_pos);
  static size_t  t_nullPos = -1;
  memcpy(&(t_pSlot->m_next),&t_nullPos,sizeof(size_t));
  if(m_lIdleSlot==-1)
  {//第一节点
    //m_fIdleSlot=m_lIdleSlot=r_slotPos;
    memcpy(&m_fIdleSlot,&r_slotPos,sizeof(size_t));
    memcpy(&m_lIdleSlot,&r_slotPos,sizeof(size_t));
  }else
  {
    t_prevSlot = (IdleSlot*)(t_pageAddr+m_lIdleSlot-m_pos);
    //t_prevSlot->m_next = r_slotPos;
    memcpy(&(t_prevSlot->m_next),&r_slotPos,sizeof(size_t));
    m_lIdleSlot=r_slotPos;
  }
  ++m_idleNum;
}


