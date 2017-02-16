#ifndef _RateLog_H_INCLUDE_20080626_
#define _RateLog_H_INCLUDE_20080626_
#include "config-all.h"
#include "base/PString.h"

class RateLog
{
  public:
    PString   m_fileName;     //处理文件
    PString   m_fullFileName; //处理的全路径文件名
    long      m_inputCdrs;    //总记录数
    long      m_normalCdrs;   //正确记录数
    long      m_errorCdrs;    //错误记录数
    long      m_delayedCdrs;  //延迟记录数
    time_t    m_beginDealTime;//开始处理时间
    time_t    m_endDealTime;  //结束处理时间
    //下面是指两个帐期的记录数（双帐期并存时有用）
    //专门用于记录输出记录信息
    long      m_monfeeBBill[2];//两个账期的包月费账单文件记录数
    long      m_monfeeIBill[2];//两个账期的ADC费用清单文件记录数
    long      m_monfeePBill[2];//两个账期的SP费用清单文件记录数
    long      m_monfeeMBill[2];//两个账期的包月费用明细帐单文件记录数
    
    long      m_normalCdr[2]; //两个帐期的清单记录数
    long      m_userBills[2]; //两个帐期的用户帐单记录数
    long      m_acctBills[2]; //两个帐期的帐户帐单记录数
    long      m_balRcds;      //结余记录数
    long      m_fixRcds[2];   //固定费轨迹记录数
    long      m_discntRcds[2];//优惠明细记录数
    long      m_otherRcds[2]; //其它信息记录数
    long      m_adjustBfeoreBill[2]; //两个帐期的调账记录数
    //Added By Sunam 2009/8/8 19:14:58 免费资源提醒记录数
    long      m_remindCdrs;
    //add by cuiyw 20090915
    long      m_wirelessBills[2];//两个账期的用户无线上网卡账单记录数
    //end by cuiyw 20090915
    //Added By Sunam 2009/9/29 10:01:34 免费资源提醒记录数
    long      m_userSumBills;
    //记录清单记录统计信息
    PString   m_outCdrFileName[3];//输出的清单文件名  
    long      m_totalData[2];     //流量
    long      m_baseFee[2];       //一次批价费用
    long      m_totalFee[2];      //二次批价费用
    long      m_totalDuration[2]; //总时长
  public:
    PString   m_delayFileName; //延迟话单文件
    void      setDelayFileName(const char *r_delayFullFileName);
  public:
    void reset();
    void addTotalCdr();
    void addErrCdr();
    
    void addMonfeeBBills(const int &r_cycleIdx);
    void addMonfeeIBills(const int &r_cycleIdx);
    void addMonfeePBills(const int &r_cycleIdx);    
    void addMonfeeMBills(const int &r_cycleIdx);
    
    void addUserBills(const int &r_cycleIdx);
    void addAcctBills(const int &r_cycleIdx);
    void addBalRcds(const int &r_cycleIdx);
    void addDiscntRcds(const int &r_cycleIdx);
    void addFixRcds(const int &r_cycleIdx);
    void addOtherRcds(const int &r_cycleIdx);
    //add by cuiyw 20090915
    void addWirelessBills(const int &r_cycleIdx);
    //end by cuiyw 20090915
    void addNormalCdr(const int  &r_cycleIdx,const long &r_baseFee,
                      const long &r_totalFee,const long &r_totalDuration,
                      const long &r_totalData);
    void addDelayCdr();
    //Added By Sunam 2009/8/8 19:29:08
    void addRemindRcds(const int &r_cycleIdx);
	void addAdjustBeforeBills(const int &r_cycleIdx);
    //Added By Sunam 2009/9/29 10:00:33
    void adduserSumBills();
    	
    void setFileName(const char * r_fileName);
    void setOCdrFileName(const int &r_cycleIdx,const int &r_errCode,const char *r_oFileName);
    void setOCdrFileName(const int &r_cycleIdx,const int &r_errCode,
                         const char* r_path,const char * r_fileName);
    void writeLog();
    void logBegin();
    void logEnd();
};
#endif //_RateLog_H_INCLUDE_20080626_
