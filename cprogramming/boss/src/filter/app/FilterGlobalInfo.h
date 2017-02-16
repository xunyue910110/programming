#ifndef _FILTERGLOBALINFO_H_
#define _FILTERGLOBALINFO_H_
#include "FilterCdr.h"
#include "FilterError.h"
#include "BillingConstDef.h"

#include "FilterConfig.h"
#include "OutChanInfo.h"
#include "PMGlobalManager.h"

class ProcessorRule;
class FilterConfig;
class SortLog;
class PMGlobalManager;
class OutChanInfo;
class  FilterGlobalInfo
{
  public:
    FilterConfig *m_Config;
    FilterCdr     m_cdr;
    FilterCdr     m_originalcdr;
    PCDR          m_pCdr;
    PString       m_fileName;       //处理文件
    PString       m_fullPathName;   //输入路径全名
    PString       m_subChannelName; //子通道名
    PString       m_ferrfullPathName;   //异常话单文件全名 add by shenglj 2010-10-25 16:10:01
    long          m_totalCdrNum;    //被处理话单记录行数
    vector<OutChanInfo> *m_outChanInfo;
    int           m_bindMethod;     //获取资料方式 BIND_BY_MSISDN = 1,BIND_BY_IMSI,BIND_BY_MSISDN_IMSI,BIND_BY_TRUNK
    int           m_smsMethod;      //短信处理方式 SM_PROCESS_METHOD = 1,SP_PROCESS_METHOD,USSD_PROCESS_METHOD,HY_PROCESS_METHOD
    T_FLAG        m_unique_flag;    //Y 需要排重 N 不需要排重 缺省需要排重
    T_ATTRI_VAL   m_acctTag;        //出帐标志：0-正常处理，1-定时激活，2-待激活用户，Z-不出帐(2 标识预开卡用户)

    //RateInfo     m_rateInfo;
    PMGlobalManager  *m_globalManager;
    SortLog          *m_sortLog;
    ProcessorRule    *m_processorRule; //处理规则

  public:
   bool initialization(FilterConfig *r_pConfig,
                       SortLog *r_sortLog,
                       PMGlobalManager  *r_globalManager);

/*   bool cdrReset();     //读取话单前初始化
   bool canonicalize(); //读取话单后初始化
   bool isDelayedCdr(); //判断是否该判为延迟话单
*/
   //获得文件 子通道名 获取资料方式
   void setFileNameChannelName(const char *r_fileName,
   const char *r_fullPathName,
   const char *r_subChannelName,
   const int &r_bindmethod);
   //获得子进程输出配置信息 通道号.文件前缀.路径.表达式
   void setFileOutChanInfo(vector<OutChanInfo> *r_outChanInfo);

   //初始化话单
   void reset();
   //话单记录备份
   void backupcdr();
   //读取话单后规整
   void canonicalize();
   //话单输出前规整
   void finalize();
   //话单被处理行数
   void addDealingNum();
   //话单被处理行数清空
   void clearDealingNum();

 private:
   void adjustCdrBeginTime(); //调整话单开始时间

};

#endif //_FILTERGLOBALINFO_H_

