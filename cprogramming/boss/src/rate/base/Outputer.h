#ifndef _OUTPUTER_H_INCLUDE_20080627_
#define _OUTPUTER_H_INCLUDE_20080627_
#include "Inoutputer.h"
#include "InoutputRule.h"
#include "RateGlobalInfo.h"

const char FILE_TYPE_MONFEE_M = 'M';   // 包月费用明细帐单文件(对应老包月费的D文件)
const char FILE_TYPE_MONFEE_P = 'P';   // SP费用清单文件      
const char FILE_TYPE_MONFEE_I = 'I';   // ADC费用清单文件     
const char FILE_TYPE_MONFEE_B = 'B';   // 包月费用帐单文件    
//const char FILE_TYPE_MONFEE_H = 'H';   // 稽核文件(对应老包月费的C文件）    
//const char FILE_TYPE_MONFEE_N = 'N';   // 无主文件 (融入批价E文件中)

const char FILE_TYPE_NORMALCDR = 'C';
const char FILE_TYPE_ERRORCDR  = 'E';
const char FILE_TYPE_USERBILL  = 'U';
const char FILE_TYPE_ACCTBILL  = 'A';
const char FILE_TYPE_BALFILE   = 'R';
const char FILE_TYPE_ACCTTRACK = 'D';
const char FILE_TYPE_FIXTRACK  = 'F';
const char FILE_TYPE_OTHER     = 'O';
//Added by Sunam 2009/8/8 19:02:35 免费资源提醒文件
const char FILE_TYPE_REMIND    = 'M';
//add by cuiyw 20090915
const char FILE_TYPE_WIRELESSBILL = 'W';
//end by cuiyw 20090915
//Added By Sunam 2009/9/28 19:15:38 累积量文件 
const char FILE_TYPE_USERSUM   = 'S';
//帐前调账轨迹--Added by dingr
const char FILE_TYPE_ADJUSTBEFORE  = 'B';

//批价输出管理类，负责批价所有以文件形式输出信息的输出
class Outputer
{
  public:
    Outputer();
    virtual ~Outputer();
  public:
    //系统级初始化
    bool  initialize(RateGlobalInfo *r_gRateInfo,
                     OutputRuleMgr  *r_outputRule,
                     const RateRunInfo &r_runState);
    //文件级初始化
    // r_dealDate为处理日期:YYMMDD
    bool  fileInit(const int &r_dealDate);
    //输出各种文件信息
    // r_billCycle为话单所在帐期
    bool  output(const int &r_billCycle); 
    //文件级提交
    bool  fileCommit(long &r_nextFileNo);
    //帐期切换
    void  billCycleSwitch(const int &r_newCycle);
    //将所有文件关闭
    void  closeTmpFile();
  protected:
    bool  daySwitch(const int &r_dealDate);
    bool  outputCdr(const int &r_billCycle,const int &r_errCode,const char *r_cdrStr);
    bool  outputUserBill(const int &r_billCycle,const RollBackBill &r_userBill);
    bool  outputAcctBill(const int &r_billCycle,const RollBackBill &r_userBill);
    //add by cuiyw 20090914
    bool outputWirelessBill(const int &r_billCycle,const RollBackBill &r_wirelessBill);
    //add by cuiyw 20090914
    
    //包月费账单输出
    bool outputMonfeeBBills(const int &r_billCycle,const vector<OutBill> &r_outBills);
    
    bool outputMonfeeIBills(const int &r_billCycle,const vector<CdrBill> &r_cdrbills);
    bool outputMonfeePBills(const int &r_billCycle,const vector<CdrBill> &r_cdrbills);    
        
    bool outputMonfeeMBills(const int &r_billCycle,UserSubInfo &r_userSubInfo);

    //
    bool  outputBalInfo(const int &r_billCycle,const vector<T_SERVER_ID> &r_acctIdList);
    bool  outputFixTrack(const int &r_billCycle,const vector<UserRentPriceLog> &r_fixTracks);
    bool  outputAcctTrack(const int &r_billCycle,const vector<DiscntBill> &r_acctTrackList);
    bool  outputOther(const int &r_billCycle,const vector<AdditionalBill> &r_otherBills);
 //输出帐前调账轨迹 add by dingr 2009/09/17
    bool  outputAdjustBeforTrack(const int &r_billCycle,const vector<AdditionalBill> r_adjustBeforeBill);
    
    //Added By Sunam 2009/8/8 19:25:11
    bool  outputRemindCdr();
    //Added By Sunam 2009/8/8 19:25:11
    bool  outputUserSumBill();
    
    //true 表示插入成功, false 表示重复
    bool  addBalAcctId(const T_SERVER_ID &r_acctId);
  public:
    bool  ouputRentCdr(const int &r_billCycle,const vector<PString> &r_cdrList);
    OutputRule* getOutputRule();
    bool  outputBills(const int &r_billCycle);
  protected:
    RateGlobalInfo       * m_gRateInfo;
    PATTERNTYPE            m_patternType;   //匹配方式：1文件名 2SourceType 
    OutputRuleMgr        * m_outputRuleMgr; //输入规则管理类
    OutputRule           * m_OutputRule   ; //当前输入规则
                                            //普通清单输出规则
    PString                m_sFileName;     //输入文件名
    PString                m_workPath;      //工作目录
    
    int                    m_billCycle;    //当前帐期YYYYMM
    int                    m_dealDate;     //处理日期YYMMDD     
    long                   m_fileNo;       //下一个文件序号
    bool                   m_openFlag;     //是否有文件打开
    //Added By Sunam 2009/10/8 10:43:56通道号
    int                    m_channelNo;
    //下面1标识当前帐期，2表示下个帐期
    //下面为各个信息的输出文件对象
    Inoutputer            m_errorFile;       //错单文件
    long                  m_cdrFileNos[2];   //清单文件打开标记
    Inoutputer            m_cdrFile[2];      //双帐期清单文件
    Inoutputer            m_userBillFile[2]; //用户帐单文件
    Inoutputer            m_acctBillFile[2]; //帐户帐单文件
    Inoutputer            m_balFile;         //结余文件
    Inoutputer            m_fixTrackFile[2]; //固定费明细文件
    Inoutputer            m_acctTrackFile[2];//优惠明细文件
    Inoutputer            m_otherFile[2];    //存折等其它信息文件
    //Added by Sunam 2009/8/8 18:57:16 免费资源提醒文件
    Inoutputer            m_adjustBeforeFile[2];    //帐前调账优惠轨迹--added by dingr
    Inoutputer            m_remindFile;
    //Added By Sunam 2009/9/28 20:24:49
    Inoutputer            m_userSumBillFile;
    //add by cuiyw 20090914 
    Inoutputer            m_wirelessBillFile[2]; //无线上网卡账单文件
    //end by cuiyw 20090914 
    
    Inoutputer            m_monfeeBBill[2]; //两个账期的包月费账单文件      
    Inoutputer            m_monfeeIBill[2]; //两个账期的ADC费用清单文件
    Inoutputer            m_monfeePBill[2]; //两个账期的SP费用清单文件     
    Inoutputer            m_monfeeMBill[2]; //两个账期的包月费用明细帐单文件
                          
    vector<T_SERVER_ID>   m_balAcctIdList;      //结余帐户ID列表
  private:
    //XXXNN_标准文件名.C.YYMM.YYMMDDXXXXXX
    //C的位置为文件类型：清单、帐单、结余等
    char                  m_stdFileName[1024+1];//标准文件名
    int                   m_cycleIdex;          //0. 当前帐期、1下个帐期
    char                  m_rcdStr[1024*2+1];   //记录字符串
    long                  m_nextCdrFileNo;      //当前下一个清单文件序号
};

#endif //_OUTPUTER_H_INCLUDE_20080627_
