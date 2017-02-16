#ifndef _FEEINFO_H_INCLUDE_20080910_
#define _FEEINFO_H_INCLUDE_20080910_

#include "DateTime.h"
#include "FieldsXX.h"
#include <vector>
#include "RateConstDef.h"

class RateTpInfo;
//话单天分段结构
class CdrSeg
{
  public:
    DateTime m_startDateTime;  //起始时间
    DateTime m_endDateTime;    //结束时间
    long     m_duration;       //时长
    long     m_startSecond;    //起始秒数
    long     m_endSecond;      //终止秒数
    long     m_data;           //流量:字节数
    long     m_startData;      //起始字节
    long     m_endData;        //终止字节
  public:
    CdrSeg();
    void clear();
    //设置并获取时长
    long getDuration()
    {
        m_duration = m_endDateTime-m_startDateTime;
        return(m_duration);
    }
    //根据时长切割话单,false 表示没有满足切割区间的段
    bool split(const long &r_startSecond,const long &r_endSecond,
               CdrSeg &r_splitSeg) const;
    //根据时长切割话单,false 表示没有满足切割区间的段
    bool split_data(const long &r_startData,const long &r_endData,
                       CdrSeg  &r_splitSeg) const;
    //根据时间切割话单
    bool split(const DateTime &r_startDateTime, 
               const DateTime &r_endDateTime, 
               CdrSeg &r_splitSeg) const;
  public:
    friend ostream& operator<<(ostream& os,const CdrSeg &r_days)
    {
      os<<"----------------------------"<<endl;
      os<<" m_startTime   = "<<r_days.m_startDateTime <<endl;
      os<<" m_endTime     = "<<r_days.m_endDateTime   <<endl;
      os<<" m_duration    = "<<r_days.m_duration      <<endl;
      os<<" m_startSecond = "<<r_days.m_startSecond   <<endl;
      os<<" m_endSecond   = "<<r_days.m_endSecond     <<endl;
      os<<" m_data        = "<<r_days.m_data     <<endl;
      os<<" m_startData   = "<<r_days.m_startData     <<endl;
      os<<" m_endData     = "<<r_days.m_endData     <<endl;
      return os;
    }
};

//段类型
enum SEG_TYPE
{
  FEE_SEG     = 0, //费用段
  SEND_MIN_SEG   , //赠送分钟数
  SEND_FEE_SEG   , //赠送费用段
  SEND_DATA_SEG  , //赠送流量
  PATCH_SEG      , //补偿段
  TMP_SEG        , //临时段
    //Add by yangb For gd 1/1M At 2009-2-24 11:03:12 上午
  COM_ADD_SEG      //累计流量计算量

};
enum BILLOBJ_TYPE
{
  BILLOBJ_DURATION  = 0, //时长
  BILLOBJ_DATA      = 1, //流量
  BILLOBJ_FEE       = 2, //费用
  BILLOBJ_TIMEFEE   = 3, //时长方式费用
  BILLOBJ_DATAFEE   = 4, //流量方式费用
  BILLOBJ_FEELIMIT_T= 5, //封顶费用(时长)
  BILLOBJ_FEELIMIT_D= 6  //封顶费用(流量)
};
enum ACT_METHOD
{
  ACT_METHOD_COVER = 0, //覆盖式
  ACT_METHOD_MULTI = 1, //叠加式
  RATE_METHOD_TIME = 0, //时长计费
  RATE_METHOD_DATA = 1  //流量计费
};
class RateBillId
{
  public:
    T_SERVER_ID m_userId;       //累计用户ID(或集团ID)
    long        m_tpObjId;      //资费实例
    int         m_billId;       //累计ID
    long        m_sumBeginTime; //累计起始时间
    long        m_sumEndTime;   //累计结束时间
    int         m_billType;     //账单类型:(区分大类型)
                                // 用户普通账单
                                // 个人累计账单1
                                // 个人累计账单2/集团账单2
    int         m_type;         // 账单类型
                                // 对于用户普通账单：0 组合，1明细
                                // 对于个人累计量：  0 用户，1集团
    T_SERVER_ID m_groupId;      //集团Id
    //Added By Sunam 2009/8/6 16:32:25
    T_TP_ID m_feePolicyId;
  public:
    //判断两个累计ID主键
    friend int operator==(const RateBillId &r_left,const RateBillId &r_right);
    friend int operator<(const RateBillId &r_left,const RateBillId &r_right);
    friend ostream& operator<<(ostream& r_os,const RateBillId &r_billId);
};

//added by yinb 2011/2/18 12:12:30
class SegAddupInfo
{
  public:
    RateBillId   m_billId;         //累计ID信息
    DateTime     m_startDateTime;  //起始时间
    DateTime     m_endDateTime;    //结束时间
    long         m_duration;       //时长
    long         m_startSecond;    //起始秒数
    long         m_endSecond;      //终止秒数
    long         m_data;           //流量:字节数
    long         m_startData;      //起始字节
    long         m_endData;        //终止字节
    long         m_unitValue;      //单位量
    long         m_unitCount;      //单元
    long         m_value;          //累计量 
    int          m_objType;        //计费:时长计费/流量计费
  public:
    SegAddupInfo();
    ~SegAddupInfo();
    SegAddupInfo(const SegAddupInfo &r_right);
    SegAddupInfo& operator=(const SegAddupInfo &r_right);
    SegAddupInfo& operator=(const CdrSeg &r_right);
    void clear();
    void compute();
  public:
    friend ostream& operator<<(ostream& os,const SegAddupInfo &r_days)
    {
      os<<"----------------------------"<<endl;
      os<<" m_startTime   = "<<r_days.m_startDateTime <<endl;
      os<<" m_endTime     = "<<r_days.m_endDateTime   <<endl;
      os<<" m_duration    = "<<r_days.m_duration      <<endl;
      os<<" m_startSecond = "<<r_days.m_startSecond   <<endl;
      os<<" m_endSecond   = "<<r_days.m_endSecond     <<endl;
      os<<" m_data        = "<<r_days.m_data     <<endl;
      os<<" m_startData   = "<<r_days.m_startData     <<endl;
      os<<" m_endData     = "<<r_days.m_endData     <<endl;
      os<<" m_unitValue   = "<<r_days.m_unitValue     <<endl;
      os<<" m_unitCount   = "<<r_days.m_unitCount     <<endl;
      os<<" m_value       = "<<r_days.m_value     <<endl;
      os<<" m_objType     = "<<r_days.m_objType     <<endl;
      return os;
    }
};

class RateBillInfo
{
  public:
    RateBillId  m_billId;           //累计ID信息
    int         m_objType;          //累计时长、费用、流量...
    long        m_unitValue;        //单位量
    long        m_unitFee;          //费率
    long        m_billValue;        //累计值(时长或费用)
    //Added By Sunam 2009/8/6 16:32:25
    long        m_maxValue;
  public:
    //判断两个累计ID主键
    friend int operator==(const RateBillInfo &r_left,const RateBillInfo &r_right);
    //值相加
    RateBillInfo& operator+=(const RateBillInfo &r_right);
    //added by yinb 2011/2/18 17:07:05
    RateBillInfo& operator=(const SegAddupInfo &r_right);
  public:
    void clear();
    friend ostream& operator<<(ostream& r_os,const RateBillInfo &r_billInfo);
};

class AddupInfo
{
  public:
    int                   m_seg;
    int                   m_segCapacity;
    vector<SegAddupInfo>  m_segInfoList;
  public:
    AddupInfo();
    ~AddupInfo();
    void compute();
    void addSegAddupInfo(const SegAddupInfo &r_segAddupInfo);
    void ajustAddupInfoList();
    void clear();
  private:
    int getNextNodeByLackData(const long &r_lackDatas,const int &r_sIdx,long &r_lackLast) const;
    int getNextNodeByLackTime(const long &r_lackTimes,const int &r_sIdx,long &r_lackLast) const;
};

//定义一个段费用信息
class SegFeeInfo
{
  public:
    const RateTpInfo  *m_pRateTpInfo;     //政策指针
    T_TPOBJ_ID   m_tpObjId;         //资费实例
    T_TP_ID      m_feePolicyId;     //资费政策ID
    T_TP_ID      m_tpId;            //批价政策ID
    DateTime     m_startDateTime;   //起始时间
    DateTime     m_endDateTime;     //终止时间
    long         m_duration;        //时长
    long         m_startSecond;     //起始秒数
    long         m_endSecond;       //终止秒数
    long         m_unitTime;        //单位时长
    long         m_unitFee;         //单位费用：费率
    char         m_intTag;          //跳次规整标记
    long         m_unitCount;       //跳次数
    long         m_value;           //费用
    int          m_segType;         //段类别：
                                    // 0 普通费用段
                                    // 1--赠送时长 
                                    // 2--赠送费用
    //Added By Sunam 2009/10/12 10:41:00 computeMethod=11时使用
    bool         m_totalFlag;
    long         m_totalUnitFee;
    //add for GPRS 0.X厘费率 2009,0904 费用精度
    int m_pricision ;
    vector<RateBillInfo> m_billInfoList; //累计信息列表
    RateBillInfo m_billInfo;        //当前政策处理的该段累计量信息
    int          m_notFinishFlag;   //用于计算过程中(是否处理完毕)
                                    //0 完毕，1 需要做总量调整
    ACT_METHOD   m_actMethod ;      //作用方式:覆盖式、叠加式
    //以下为支持流量计费而增加
    long         m_data;        //流量
    long         m_startData;   //起始字节
    long         m_endData;     //终止字节
    long         m_unitData;    //单位字节数
    ACT_METHOD   m_rateMethod;  //计费:时长计费/流量计费
  public:
    SegFeeInfo();
    virtual ~SegFeeInfo();
    SegFeeInfo(const SegFeeInfo &r_right);
    SegFeeInfo& operator=(const SegFeeInfo &r_right);
    SegFeeInfo& operator=(const CdrSeg &r_right);
    void clear();
    void compute();
    long getTimes() const;
    long getTimes(const long &r_duration,const long &r_unitTime,const char &r_timeIntTag) const;
    long getDataTimes(const long &r_data,const long &r_unitData,const char &r_intTag) const;
    bool cheaper(const SegFeeInfo &r_right) const; //add by gaojf
    //r_exeFlag为执行方式 0 普通 1强制 2 部分强制 3整条比较
    bool needUpdate(const SegFeeInfo &r_right,const int &r_exeFlag);//add by gaojf
    //设置TpId信息
    void setTpIdInfo(const T_TPOBJ_ID &r_objId,
                     const T_TP_ID    &r_feePolicyId,
                     const T_TP_ID    &r_priceId,
                     const RateTpInfo *r_rateTpInfo);
    friend ostream& operator<<(ostream& r_os,const SegFeeInfo &r_segInfo);
};

//定义一个政策费用信息
class FeeInfo
{
  public:
    RateTpInfo         *m_pRateTpInfo;//资费政策：记录最后作用的政策
    long                m_unitCount;  //跳次数
    long                m_feeSum;     //费用
    long                m_freeTime;   //赠送的时长
    long                m_freeData;   //赠送的流量
    long                m_freeFee;    //赠送的费用
    long                m_limitFee;   //封顶费用
    int                 m_seg;        //费用段数
    int                 m_segCapacity; //段费用容器大小
    vector<SegFeeInfo>  m_segInfoList;//段费用列表
    char                m_exeMethod;  //强制、非强制
    char                m_rateTime;   //一次批价、二次批价
    bool                m_coverFlag;  //是否覆盖整条话单标记
    //add by xiangyang 2009/10/26 22:16:08
    long         				m_lackvalue;  //补收费用    
    
  public:
    FeeInfo();
    virtual ~FeeInfo();
    FeeInfo(const FeeInfo &r_right);
    FeeInfo& operator=(const FeeInfo &r_right);
    void clear();
    //重新计算费用和赠送量
    //进位标志r_carryTag  0:不处理（保持现状）1:舍去  2:进位 3:四舍五入 
    void computeFee(const char &r_carryTag=CARRY_TAG_NODEAL);
    void ajustFeeInfoList(const DateTime &r_endDateTime,const long &r_endData);
    friend ostream& operator<<(ostream& r_os,const FeeInfo &r_feeInfo);
  public:
    //更新费用段信息r_segFeeInfo,
    ////r_exeFlag为执行方式: true 为强制执行,false 为普通
    //r_exeFlag为执行方式 0 普通 1强制 2 部分强制
    void updateSegFeeInfo(const SegFeeInfo &r_segFeeInfo,
                          //const bool       &r_exeFlag);
                          const int        &r_exeFlag);
    
    //根据r_feeInfo更新费用信息(对r_tpObjId和r_tpId符合的更新)
    //r_flag true 是否需要调整(调用ajustFeeInfoList)
    void updateFeeInfo(const FeeInfo  &r_feeInfo,
                       const bool     &r_exeFlag,
                       const T_TPOBJ_ID &r_tpObjId,
                       const T_TP_ID    &r_tpId);
    bool cheaper(FeeInfo& feeinfo) const;
    void addSegFeeInfo(const SegFeeInfo &r_segFeeInfo);
    void resizeSegList(const int &r_size);
    
    //特殊更新方式:赠送项更新，其他的缺失才更新
    void spUpdateFeeInfo(const FeeInfo  &r_feeInfo);
  private:
    int getNextNodeByLackData(const long &r_lackDatas,const int &r_sIdx,long &r_lackLast) const;
    int getNextNodeByLackTime(const long &r_lackTimes,const int &r_sIdx,long &r_lackLast) const;
};

//定义一条清单费用信息
class CdrFeeInfo
{
  public:
    long            m_feeSum;                    //总费用
    long            m_freeTime;                  //赠送的时长
    long            m_freeData;                  //赠送的流量
    long            m_freeFee;                   //赠送的费用
    long            m_limitFee;                  //上限累计额度
    FeeInfo         m_feeInfo[FEEFIELDS_MAX];    //各个费用项费用
    bool            m_noRateFlag[FEEFIELDS_MAX]; //未批价标记
    char            m_cdrCompFlag;               //整条比较标记
  public:
    CdrFeeInfo();
    CdrFeeInfo(const CdrFeeInfo &r_right);
    void clear();
    void ComputeFeeInfo();
    void setFeeInfo(const int &r_feeField,const FeeInfo &r_feeInfo);
    bool cheaper(CdrFeeInfo& r_feeInfo) const;
    CdrFeeInfo& operator=(const CdrFeeInfo &r_right);
};

//总量条件中间结构
class SumRule
{
  public: 
    RateBillId  m_billId;   //总量ID信息 
    long        m_curValue; //总量基准值(前清单作用前)
    long        m_minValue; //条件最小值
    long        m_maxValue; //条件最大值
  public:
    
};
class SumTpRule
{
  public:
    vector<SumRule>  m_ruleList; 
  public:
    SumTpRule();
    virtual ~SumTpRule();
    SumTpRule(const SumTpRule &r_sumTpRule);
    void clear();
};

class RateTpInfo;
class EventFeePolicy;
class Price;
class RateInfo
{
  public:
    CdrSeg               m_cdrSegment;                //话单段
    vector<CdrSeg>       m_cdrSegList;                //话单天分段列表
    CdrFeeInfo           m_cdrFeeInfo;                //每一个阶段的基准结果T0
    CdrFeeInfo           m_cdrFeeInfo2;               //未提交的基准结果T1
    //Add by yangb For gd REQ33 At 2009-4-3 16:35:16 下午
    CdrFeeInfo           m_cdrFeeInfoRate1;           //一次批价结果
    vector<CdrFeeInfo>   m_cdrFeeInfos;               //待选择的政策结果
    int                  m_curFeeIdx;                 //当前政策结果下标
    int                  m_billIdList[FEEFIELDS_MAX]; //计费明细账单项
    
    RateTpInfo          *m_pCurRateTpInfo;            //当前批价的政策
    EventFeePolicy      *m_pCurEventPolicy;           //当前批价的事件政策
    int                  m_curFeeField;               //当前批价的费用项
    Price               *m_pCurPrice;                 //当前处理的批价政策
  public:
    ///////////////////////////////////////////////////////////////////
    FeeInfo                 m_curTpFeeInfo;      //当前批价政策最终结果
    vector<SumTpRule>       m_sumRuleList;       //当前节点的总量条件列表
    RateBillInfo            m_sendSumInfo;       //当前作用赠送累计量信息
    //added by yinb 2011/2/18 10:52:51
    AddupInfo               m_curAddupInfo;      //当前累计信息
    vector<RateBillInfo>    m_addupInfos;
  public:
    void clear();
    void resizeCdrFeeInfo(const int &r_size);
};

#endif //_FEEINFO_H_INCLUDE_20080910_




