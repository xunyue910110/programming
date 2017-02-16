#ifndef _PAGEMEMMGR_H_INCLUDE_20080425_
#define _PAGEMEMMGR_H_INCLUDE_20080425_
#include "PageInfo.h"

class PageMemMgr
{
  public:
    PageMemMgr();
    virtual ~PageMemMgr();
  public:
    //r_size包括pageListinfo和下面的列表
    bool f_init(unsigned int &r_spaceCode,
                       char * r_spAddr,
                const size_t &r_offSet,
                const size_t &r_size,
                const size_t &r_pageSize);
    //attach方式init
    void attach_init(unsigned int &r_spaceCode,char * r_spAddr,
                      const size_t &r_offSet);
                    
    size_t  getIdlePageNum();
    //申请页面
    bool allocatePage(PageInfo* &r_pPage,ShmPosition &r_pagePos);
    //释放页面
    bool freePage(const ShmPosition &r_pagePos);
    //通过Slot位置求取对应页面位置
    bool getPagePosBySlotPos(const size_t &r_slotPos,size_t &r_pagePos);
  private:
    PageListInfo  *m_pHeader;   //支持pageListinfo
    char          *m_spAddr;    //表空间首地址
    size_t         m_sOffSet;   //起始偏移量
  unsigned int     m_spaceCode; //表空间代码
  public:
    //调试接口
    virtual bool dumpSpaceInfo(ostream &r_os);
	virtual float getTableSpaceUsedPercent();
};
#endif //_PAGEMEMMGR_H_INCLUDE_20080425_
