#ifndef _TableSpaceMgr_H_INCLUDE_20080425
#define _TableSpaceMgr_H_INCLUDE_20080425
#include "SpaceMgrBase.h"
#include "PageMemMgr.h"

class TableSpaceMgr: public SpaceMgrBase
{
  public:
    TableSpaceMgr(SpaceInfo &r_spaceInfo);
    virtual ~TableSpaceMgr();
  public:
    //根据表空间定义信息,创建表空间
    bool createTbSpace(const int &r_flag);
    bool deleteTbSpace();
    //创建式初始化表空间
    bool initTbSpace(const int &r_flag);
    //attach方式初始化表空间
    bool attach_init();

    //申请页面
    bool allocatePage(PageInfo* &r_pPage,ShmPosition &r_pagePos);
    //回收本Space内的page
    bool freePage(const ShmPosition &r_pagePos);
    //通过Slot位置求取对应页面位置
    bool getPagePosBySlotPos(const size_t &r_slotPos,size_t &r_pagePos);
  private:
    //第一次初始化表空间
    bool f_initTbSpace();
      
  private:
    PageMemMgr    m_pageMemMgr;
  public:
    //调试接口
    virtual bool dumpSpaceInfo(ostream &r_os);
    // 内存使用率 add by chenm
    // 默认返回所有表空间使用率
    virtual float getTableSpaceUsedPercent();
};


#endif //_TableSpaceMgr_H_INCLUDE_20080425
