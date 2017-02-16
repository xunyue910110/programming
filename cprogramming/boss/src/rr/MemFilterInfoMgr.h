#ifndef MEMFILTERINFOMGR_H_HEADER_INCLUDED_B7642486
#define MEMFILTERINFOMGR_H_HEADER_INCLUDED_B7642486
#include "HisFilterInfoMgr.h"
#include "HashList.h"
#include <map>

class BusiFidInfoMgr;
class MemInnerInfo
{
  public:
    BusiFidInfoMgr *m_busiFidMgr;
    long            m_firstTime;
    long            m_nodeNumber;
    long            m_delTimePos;
    long            m_remainTimePos;
    long            m_delNodeNum;
};
//单业务过滤信息管理类
class BusiFidInfoMgr : public HisFilterInfoMgr
{
  public:
    BusiFidInfoMgr();
    virtual ~BusiFidInfoMgr();
  public:
    bool initialization(const PString &r_fileType,
                        const PString &r_filePath,
                        const int     &r_channelNo,
                        const int     &r_onefileTime);
    //根据时间判断是否在内存时间段内
    bool isInMemList(const long& r_beginTime) const;
    //判断是否重单
    bool isRepeatFid(const int &r_rrType,const FilterInfo& r_fidInfo);
    //插入过滤信息
    bool insertFidInfo(const vector<FilterInfo*> &r_filterList);
    //取内存使用信息
    void getMemInfo(MemInnerInfo &r_memInfo) ;
    //按照规则删除
    size_t deleteNode(const long &r_delTime,const long &r_remainTime,const long &r_delNodeNum); 
  private:
    PString           m_fileType;    //文件类型
    HashList          m_fidInfoList; //该业务对应的过滤信息列表
    long              m_firstTime;   //当前内存中的最早时间
};



class MemFilterInfoMgr
{
  public:
    MemFilterInfoMgr();
    virtual ~MemFilterInfoMgr();
  public:
    bool initialization(const RrConfig &r_config);
    
    bool setFileType(const PString &r_fileType);
    bool isInMemList(const int &r_rrType,const FilterInfo& r_fidInfo,const long &r_timeDuration);
    bool isRepeatFid(const int &r_rrType,const FilterInfo& r_fidInfo);
    bool insertFidInfo(const vector<FilterInfo*> &r_filterList);
    bool addFidInfoMgr(const PString &r_fileType);
    void deleteOverDue();
  protected:
    //根据各个内存信息，结合允许的最大量和时间跨度
    //计算删除时间点
    void computeDelTimePos(vector<MemInnerInfo> &r_memInfoList);
  private:
    PString m_filePath;        //对应过滤信息文件存放在何处
    size_t  m_stdTimeDuration; //存放标准时长
    map<PString,BusiFidInfoMgr*> m_fidInfoList;    //各业务过滤信息管理对象
    BusiFidInfoMgr*              m_busiFidInfoMgr; //当前业务过滤信息管理对象
    int                          m_channelNo;      //排重通道号
    size_t                       m_maxFidNumber;   //过滤信息允许的最大数量
    size_t                       m_onefileTime;
};



#endif /* MEMFILTERINFOMGR_H_HEADER_INCLUDED_B7642486 */
