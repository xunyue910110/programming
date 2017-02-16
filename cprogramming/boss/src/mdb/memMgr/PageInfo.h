#ifndef _PAGEINFO_H_INCLUDE_20080425_
#define _PAGEINFO_H_INCLUDE_20080425_
#include "MdbConstDef.h"
#include "MdbAddress.h"

//#ifdef _USEDSLOT_LIST_
class UsedSlot
{
  public:
    ShmPosition m_prev; ///<指向表内上一条记录
    ShmPosition m_next; ///<指向表内下一条记录
};
//#endif
 
class IdleSlot
{
  public:
    size_t m_next;
};
class PageInfo
{
  public:
    unsigned int m_spaceCode; //表空间代码
    size_t       m_pos; //该page位置信息
    ShmPosition  m_next;
    ShmPosition  m_nextNotNullPage;
    size_t       m_pageSize;
    int          m_slotSize;
    size_t       m_slotNum;
    size_t       m_idleNum;
    size_t       m_fIdleSlot;
    size_t       m_lIdleSlot;
  public:
    //申请r_num个元素，r_slotAddrList初始化在外部，该处只push_back
    void allocateNode(char* t_pageAddr,const int &r_num,
                      vector<MdbAddress>& r_slotAddrList,
                      int &r_newNum);
    //初始化
    void initSlotList(char* t_pageAddr,const int &r_slotSize);
    //释放Slot
    void freeNode(char* t_pageAddr,const size_t &r_slotPos);
};

class PageListInfo
{
  public:
    size_t m_pageSize;
    size_t m_pageNum;
    size_t m_idleNum;
    size_t m_fIdlePage;
    size_t m_lIdlePage;
  public:
    
};

#endif //_PAGEINFO_H_INCLUDE_20080425_
