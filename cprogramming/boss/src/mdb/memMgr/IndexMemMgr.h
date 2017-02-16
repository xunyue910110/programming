#ifndef _INDEXMEMMGR_H_INCLUDE_20080425_
#define _INDEXMEMMGR_H_INCLUDE_20080425_
#include "SpaceMgrBase.h"
class IndexDesc;
class IndexSegInfo
{
  public:
    size_t m_fIdleSeg;
    size_t m_lIdleSeg;
    size_t m_fUsedSeg;
    size_t m_lUsedSeg;
};

class HashIndexSeg
{
  public:
    size_t    m_sOffSet;
    size_t    m_segSize;
    size_t    m_hashSize;
    T_NAMEDEF m_idxName;
    size_t    m_prev;
    size_t    m_next;
  public:
    //t_addr为指向HashIndexSeg的地址
    void initHash(char* t_addr)
    {
      memset(t_addr+sizeof(HashIndexSeg),0,m_hashSize*sizeof(ShmPosition));
    } 
};
class IndexMemMgr:public SpaceMgrBase
{
  public:
    IndexMemMgr();
    virtual ~IndexMemMgr();
  public:
    bool initialize(SpaceInfo &r_spaceInfo);
    //根据表空间定义信息,创建表空间
    bool createIdxSpace(SpaceInfo &r_spaceInfo,const int &r_flag);
    bool deleteIdxSpace();
    //初始化表空间
    bool initSpaceInfo(const int &r_flag);
    //attach方式init
    bool attach_init(SpaceInfo &r_spaceInfo);
  public:
    //r_shmPos是指向Hash中ShmPositioin[]的首地址
    bool createHashIndex(IndexDesc * r_idxDesc,ShmPosition &r_shmPos);
    //r_shmPos是指向Hash中ShmPositioin[]的首地址
    bool dropHashIdex(const ShmPosition &r_shmPos);
    //r_shmPos是指向Hash中ShmPositioin[]的首地址
    void initHashSeg(const ShmPosition &r_shmPos);
  protected:
    bool f_init();
  private:
    IndexSegInfo  *m_pIdxSegInfo;
  public:
    //调试接口
    virtual bool dumpSpaceInfo(ostream &r_os);
    virtual void getTableSpaceUsedPercent(map<string,float> &vUserdPercent,const char * cTableSpaceName);
};

#endif //_INDEXMEMMGR_H_INCLUDE_20080425_
