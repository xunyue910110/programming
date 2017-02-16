#ifndef _RATEGLOBALINFO_H_INCLUDE_20080614_
#define _RATEGLOBALINFO_H_INCLUDE_20080614_
#include "RateCdr.h"
#include "RateConfig.h"
#include "BillStruct.h"
#include "TpInfo.h"
#include "FeeInfo.h"
#include "RateConstDef.h"
#include "CompositItemMgr.h"
#include "DetailBillMgr.h"
#include "IdInfo.h"
#include "SumBillMgr.h"
#include "ComBill.h"
#include "PartyPrice.h"
//Added By Sunam 2009/8/8 18:14:43
#include "RemindInfo.h"
#include "JScriptParser.h"
#include "JsExpression.h"
#include <set>
#include <algorithm>

class  PMManager;
class  RateLog;
class  BillManager;
class  IdErrorInfo;
class  ErrorMessages;

class  RateGlobalInfo
{
  public:
    //致命错误信息,防止错误情况下继续处理
    int              m_fatalECode;      //致命错误代码
    PString          m_fatalEMsg;       //致命错误信息
    ////////////////////////////////////////////
    RateConfig      *m_pConfig;         //配置文件信息
    RateCdr          m_cdr;             //话单属性
    PCDR             m_pCdr;            //话单指针：便于操作
    IdInfo           m_idInfo;          //用户资料信息
    PMManager       *m_paramMgr;        //资料参数管理
    RateLog         *m_rateLog;         //处理日志
    PString          m_fileName;        //处理文件
    ProcessRule     *m_processRule;     //处理规则
    BillManager     *m_billMgr;         //帐单管理
    DetailBillMgr    m_detailBillMgr;   //内部账单管理的类
    SumBillMgr       m_sumBillMgr;      //批价过程中的总量管理器
    //added by hany 2011/3/22 9:57:37
    CompositItemMgr *m_compItemMgr;     //组合账单参数管理器
    ComBill          m_comBill;         //帐务处理资料账单共享对象 xuf
    CycParam        *m_pSysCycParam;    //系统帐期信息
    int             *m_pSysCycle;       //当前系统帐期
    char             m_dealTime[14+1];  //处理时间YYYYMMDDHHMISS
    ErrorMessages   *m_pErrorMsgs;      //告警信息句柄
    IdInfo           m_grpIdInfo;       //虚拟用户资料信息
    map<string,string>  m_PriceElement;
		long loop_feeRatio;
  public:
    int              m_channelNo;     //通道号
    T_SERVER_ID      m_userId;        //用户标识
    T_BIZ_TYPE       m_bizType;       //业务类型
    T_SERVER_ID      m_billUserId;    //帐单UserId,无主有的根据IMSI或MSISDN来合帐
    T_DATE_TIME      m_beginDateTime; //通话起始时间
    T_DATE_TIME      m_endDateTime;   //通话结束时间
    T_OTHER_PARTY    m_otherParty;    //对方号码
    T_DATE_TIME      m_firstCallTime; //第一次开打时间
    CycParam         m_cycParam;      //事件帐期信息
    int              m_cycDays;       //该事件帐期天数add by gaojf2009-2-18 14:36
    PString          m_errMsg;        //错误信息
  public: //账单信息
    //说明： 新总账单 - 原始帐单 = 回滚账单 (计费优惠除外)
    
    //约定1：该部分帐单成员只是存放读取后的帐单信息
    //       处理过程中不更新（原始值） ---原始帐单 保持已排序
    vector<UserBill>         m_userBills;           //话单归属帐期的普通帐单
    vector<UserMonfeeBill>   m_userMonfeeBills;     //话单归属帐期的包月费帐单
    vector<UserSumBill1>     m_userSumBill1s;       //话单归属帐期的累计帐单1
    vector<UserSumBill2>     m_userSumBill2s;       //累计量2
    vector<UserSumBill3>     m_userSumBill3s;       //累计量3，包月费累计
    vector<AccountBill>      m_accountBills;        //话单归属帐期的账户帐单
    vector<RateDiscntBill>   m_rateDiscnts;         //计费优惠
    vector<UserRentPriceLog> m_rentFeeLogInfoVector;//固定费批价日志信息集
    
    //约定2: 该部分账单成员存放批价后的最新的总量帐单
    // 计费优惠除外，计费优惠只存放变更的账单 --新总帐单 保持已排序
    vector<UserBill>        m_userBills_new;      //话单归属帐期的普通帐单
    vector<UserMonfeeBill>  m_userMonfeeBills_new;//话单归属帐期的包月费帐单
    vector<UserSumBill1>    m_userSumBill1s_new;  //话单归属帐期的累计帐单1
    vector<UserSumBill2>    m_userSumBill2s_new;  //累计量2
    vector<UserSumBill3>    m_userSumBill3s_new;  //累计量3,包月费累计
    vector<AccountBill>     m_accountBills_new;   //话单归属帐期的账户帐单
    vector<RateDiscntBill>  m_rateDiscnts_new;    //计费优惠
    vector<AdditionalBill>  m_addBillVector;      //赠送/稽核/异常信息账单
    vector<T_SERVER_ID>     m_balAcctIdList;      //结余帐户ID列表
    vector<DiscntBill>      m_discountBills;      //优惠明细账单(轨迹)
    //帐前调账轨迹信息
     vector<AdditionalBill>    m_adjustBeforeBills; 
    //added by zhaoly for 3G at 2009-5-16 01:14上午
    vector<PartyPrice>      m_partyPriceInfo;     //存放product相关信息
    vector<ParsedPartyPrice> m_parsedPartyPriceInfo;     //存放product相关信息
    map<int,string>          m_paramIdMapKey;           //参数ID和字符串对应关系    
    //单条话单的回滚账单信息
    vector<RollBackBill>    m_rollBackBills;     //回滚帐单信息
    //当前文件中所有话单的回滚账单信息
    vector<RollBackBill>    m_rollBackBillsAll;
    vector<RollBackBill>    m_rollBackBillsGroupUser;
    
    //Added By Sunam 2009/8/9 22:08:08 用于在本地缓存用户账单
    map<T_SERVER_ID,vector<UserBill>       > m_tUserBill[12];
    map<T_SERVER_ID,vector<UserMonfeeBill> > m_tUserMonfeeBill[2];
	map<T_SERVER_ID,vector<UserSumBill1>   > m_tUserSumBill1[12];
	map<T_SERVER_ID,vector<UserSumBill2>   > m_tUserSumBill2;
	map<T_SERVER_ID,vector<UserSumBill3>   > m_tUserSumBill3;
	map<T_SERVER_ID,vector<AccountBill>    > m_tAccountBill[2];
	map<T_SERVER_ID,vector<RateDiscntBill> > m_tRateDiscnts[12];
	  
	map<T_SERVER_ID,vector<UserBill>       > m_tUserBill_org[12];
    map<T_SERVER_ID,vector<UserMonfeeBill> > m_tUserMonfeeBill_org[2];
	map<T_SERVER_ID,vector<UserSumBill1>   > m_tUserSumBill1_org[12];
	map<T_SERVER_ID,vector<UserSumBill2>   > m_tUserSumBill2_org;
	map<T_SERVER_ID,vector<UserSumBill3>   > m_tUserSumBill3_org;
    map<T_SERVER_ID,vector<AccountBill>    > m_tAccountBill_org[2];
	map<T_SERVER_ID,vector<RateDiscntBill> > m_tRateDiscnts_org[12];
	
	//Added By Sunam 2009/9/28 19:44:35 存放当前文件中用户累计量增量的全量
	map<int,vector<UserSumBill1> > m_userSumBill1Update[2];
	vector<UserSumBill2> m_userSumBill2Update[2];
	
	//Added By Sunam 2009/10/6 10:48:23 存放群用户列表，对于群用户按单条方式处理
	vector<T_SERVER_ID> m_groupUserList;
	
	//UserBill已处理用户集合(包括UserBill)
	set<T_SERVER_ID> m_billDealUserSet[12];
  //UserMonfeeBill已处理用户集合(包括UserMonfeeBill)
	set<T_SERVER_ID> m_MonfeebillDealUserSet[12];
	//UserBill已处理用户集合(包括AccountBill)
	set<T_SERVER_ID> m_accountDealUserSet[2];
	//UserBill已处理用户集合(包括RateDiscnt)
	set<T_SERVER_ID> m_rateDiscntDealUserSet[12];
	//UserSum1已处理用户集合
	set<T_SERVER_ID> m_sum1DealUserSet[12];
	//UserSum2已处理用户集合
	set<T_SERVER_ID> m_sum2DealUserSet;
  //UserSum3已处理用户集合
	set<T_SERVER_ID> m_sum3DealUserSet;
	//累计量1的账期 YYYYMM
	int m_sum1BillCycle;
	
	//待加锁用户列表，在Inputer中准备
	set<T_SERVER_ID> m_lockUserList;
	//锁状态
	bool m_lockStatus;
	
	//待加锁账户列表，在Inputer中准备
	set<T_SERVER_ID> m_lockAcctList;
    
    //Added By Sunam 2009/8/8 18:15:13 提醒信息
    vector<RemindInfo>      m_remindInfo;
    
    DataBuffer m_jsInBuf,m_jsOutBuf;
    map<string,JsExpression> m_expressionInfo;
    
    set<int> m_ExclusivedEventFeepolicy;//被互斥的事件
    
  public:
    //下面信息由TpProc完成求取
    vector<UserTp>       m_userTpList;    //用户优惠政策（CRM或BILLING源）
    vector<RateTpInfo>   m_tpInfoList;    //批价政策（批价实现政策信息）
    //存放m_tpInfoList的下标
    vector<int>          m_pRateTpList;   //计费阶段批价政策
    vector<int>          m_pAcctTpList;   //帐务阶段批价政策
    vector<int>          m_pRentTpList;   //固定费批价政策 add by duyu 2009.9.10
    vector<int>          m_pDataTpList;   //帐务流量批价政策
    
    //为了提高批价性能而增加 add by gaojf 2010-1-11 13:57:24 begin
    list<Mdb_VpnInfo_UserId> m_vpnInfoList_a,m_vpnInfoList_b; 
    //end gaojf 2010-1-11 13:57:24 begin
    
  public:
    bool                 m_hasFeeFlag[FEEFIELDS_MAX]; //是否有费用标记
    RateInfo             m_rateFeeInfo;               //批价费用标记
  public:
    //以下为支持固定费模式而增加
    vector<PString>      m_rentInputCdrList;  //一个用户的输入话单列表
    vector<PString>      m_rentOutputCdrList; //一个用户的输出话单列表
    InputRule          * m_inputRule   ;      //当前输入规则
    OutputRule         * m_OutputRule  ;      //当前输出规则   
    
    //包月费输出话单列表
    vector<CdrBill> outputCdrVector;
    UserSubInfo outputUserSubInfoBill;
    vector<OutBill> outputBillVector; 
    vector<PString>   m_monfeeOutputCdrList; //包月费输出话单列表
    //为提高效率，从RentProc中提出到RateApp::dealRentFeeFile中初始化
    int**                m_subCycStartDay;            //获取子账期开始时间的二维数组
    int**                m_subCycEndDay;              //获取子账期结束时间的二维数组
    
    //帐户信息时间:用于江苏联通首长工程(按通话时间天计算)
    int                   m_acctDay; //通话日期YYYYMMDD
    vector<UserServ>      m_vecUserServ;
    vector<UserServState> m_vecUserServState;
    //add for rent new version begin
    multimap<T_BIZ_TYPE, int> m_mtmapRentTpList;
    //add for rent new version end
    //add by xuf 2009-11-17 4:43:28
    bool m_isExistServ;
    bool m_isExistServState;
  public:
    //初始化
    bool initialization(RateConfig  *r_pConfig  ,RateLog     *r_rateLog,
                        PMManager   *r_pParamMgr,BillManager *r_billMgr,
                        CycParam    *r_pSysCycParam,int *r_pSysCycle,
                        ErrorMessages *r_pErrMsgs);
    bool cdrReset();     //读取话单前初始化
    bool canonicalize(); //读取话单后初始化
    bool finalize();     //话单批价后处理
    bool isDelayedCdr(); //判断是否该判为延迟话单
    bool isNomalCdr(int &r_cdrCycle);   //判断是否正常的话单
    
    void setFileName(const char *r_fileName);//设置处理的文件名
    void getFeeFields(bool r_feeFields[FEEFIELDS_MAX]) const; //设置费用域
    void setFirstCallDate(); //设置第一次通话时间
    bool getRateTpInfo();//根据UserTp 设置 m_tpInfoList信息
    //设置是否有原始费用标记
    void setOrgFeeFlag(const int &r_feeField,const int &r_value);
    //设置错误代码和错误信息
    void setErrInfo(const int &r_errCode,const char *r_errMsg);
    //取错误信息
    const char* getErrMsg();
    void getExclusivedEFID();//获取被互斥的事件
  public:    
  	    bool explanExpr(long &r_value, const string &r_expr);     
    //r_month: YYYYMM r_addMonth月数
    void addMonth(const long &r_addMonth,long &r_month) const;  
    //Added By Sunam 2009/8/10 15:16:34 在r_month(YYYYMM)上减掉r_minusMonth个月
	void minusMonth(const long &r_minusMonth,long &r_month) const;
	//取多个用户的用户账单
	bool getUserBill(const int &r_cycle,const set<T_SERVER_ID> &r_userIds,vector<UserBill> &r_bills);
	//取多个用户的用户账单，排除某个用户
	bool getUserBill(const int &r_cycle,const set<T_SERVER_ID> &r_userIds,const T_SERVER_ID &r_userId,vector<UserBill> &r_bills);
	//取多个用户的账户账单
	bool getUserBill(const int &r_cycle,const set<T_SERVER_ID> &r_accountIds,vector<AccountBill> &r_bills);
	//取多个用户累计量2
	bool getUserBill(const int &r_cycle,const set<T_SERVER_ID> &r_userIds,vector<UserSumBill2> &r_bills);
	bool getUserBill(const int &r_cycle,const vector<T_SERVER_ID> &r_userIds,vector<UserSumBill2> &r_bills);
	
	//1.取单个用户的用户账单
	bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserBill> &r_bills);
	//2.取单个用户的账户账单
	bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_accountId,vector<AccountBill> &r_bills);
	//3.取单个用户的优惠账单
	bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<RateDiscntBill> &r_bills);
	//4.取单个用户的累计量1
	bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill1> &r_bills);
	//5.取单个用户累计量2
	//Modified By Sunam 2009/10/6 11:00:31
	//bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill2> &r_bills);
	bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill2> &r_bills,bool isGroupUser = false);
  //6.取单个用户累积量3（包月费累计量）
  bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill3> &r_bills);  
  //7.取单个用户包月费账单
  bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserMonfeeBill> &r_bills);  

	//Added By Sunam 2009/8/10 15:53:01 处理文件前的初始化，或者文件中途提交后的初始化
	bool fileReset();
	//Added By Sunam 2009/8/10 18:49:54 批量用户加锁
	bool lockUserId(const set<T_SERVER_ID> &r_userId, const set<T_ACCT_ID> &r_acctId);
	//表达式解析
	bool explanExpr(long &r_value, const string &r_expr, const RateTpInfo *r_policyInfo);
    bool explanSpecialExpr(long &r_value, const string &r_expr, const RateTpInfo *r_policyInfo);
	//错误信息输出
	bool writeErrMsg(const string &fileName,const int& line,const char *errMsg);
	
    //r_day:YYYYMMDD r_dayNums为天数
    void addDay(const long &r_dayNums,long &r_day) const;   
    //计算两个日期之间的相差天数r_day1-r_day2,前提 r_day1>r_day2
    long  diffDays(const long &r_day1,const long &r_day2) const;      
  //add by xuf 2009-11-24 19:56:49 用户群系统锁  
  bool lockAcctUserGrp(const T_SERVER_ID &r_vUserId);  
  protected:
    //Modified By Sunam 2009/9/1 11:08:43 vector改成set
    //vector<LockInfo>     m_lockInfoList; //内部锁资源
    set<LockInfo>        m_lockInfoList; //内部锁资源
    vector<T_SERVER_ID>  m_gBillUserList;//群账单用户列表
  public:
    //Modified By Sunam 2009/9/1 11:18:11
    //bool lockUserId(const T_SERVER_ID &r_userId); //用户加锁
    bool lockUserId(const T_SERVER_ID &r_userId,const long &interval = 100,const long &repeatTimes = 2000000);
    bool lockAccountId(const T_SERVER_ID &r_accountId);//帐户加锁
    bool unlockId();//用户帐户解锁
    bool unlockGroupUser();
    //Modified By Sunam 2009/9/1 12:27:05
    //void generateRollBill(); //根据新的账单和原始账单获取回滚信息
    void generateRollBill(const bool &rollBackFlag = false);
    bool UserGrouplocks(vector<T_SERVER_ID> &r_groupUserList); //对所有用户群加锁
    //取群用户账单信息
    bool getGroupUserBill(const vector<T_SERVER_ID> &r_groupUserList);
    //从MDb中根据用户ID取非自身或群的SUM2账单
    bool getRoleUserBill2(const T_SERVER_ID &r_userId);
    
    //取累计2上个帐期的累计量
    bool getLastSumBill2(const vector<UserSumBill2> &r_billList,
                         const UserSumBill2         &r_bill,
                         const UserSumBill2*        (&r_resultBill));
    
    //取累计量3上个帐期的累计量
    bool getLastSumBill3(const vector<UserSumBill3> &r_billList,
                         const UserSumBill3         &r_bill,
                         const UserSumBill3*        (&r_resultBill));
    
    //取相等的一条账单
    template<class T> bool getBillItem(const vector<T> &r_billList,const T &r_bill,const T* (&r_orgBill))
    {
      vector<T>::const_iterator t_itr = ::lower_bound(r_billList.begin(),r_billList.end(),r_bill);
      if(t_itr == r_billList.end()) return false;
      if(*t_itr == r_bill)
      {
        r_orgBill = &(*t_itr);
        return true;
      }
      else return false;
    }
    //以一条账单全量方式更新或新增
    template<class T> void updateBillTotal(vector<T> &r_billList,const T &r_bill)
    {
      vector<T>::iterator t_itr = ::lower_bound(r_billList.begin(),r_billList.end(),r_bill);
      if(t_itr == r_billList.end() || (!(*t_itr==r_bill)))
      {
        if(r_bill.isZero()) return;
        r_billList.insert(t_itr,r_bill);
      }else
      {
        *t_itr = r_bill;
      }
    }    
    //以一条账单增量方式更新或新增
    template<class T> void updateBillIncre(vector<T> &r_billList,const T &r_bill)
    {
      if(r_bill.isZero()) return;
      vector<T>::iterator t_itr = ::lower_bound(r_billList.begin(),r_billList.end(),r_bill);
      if(t_itr == r_billList.end() || (!(*t_itr==r_bill)))
      {
        r_billList.insert(t_itr,r_bill);
      }else
      {
        *t_itr += r_bill;
      }
    }
  public:
    class CmpRateTpIndex
    {
      private:
        vector<RateTpInfo> * m_pRateTpList;
      public:
        CmpRateTpIndex(vector<RateTpInfo> &r_rateTpList)
        {
          m_pRateTpList = &r_rateTpList;
        }
      public:
        virtual bool operator()(const int &r_left,const int &r_right) const
        {
          return ((*m_pRateTpList)[r_left]<(*m_pRateTpList)[r_right]);
        }
    };
    CmpRateTpIndex  *m_cmpRateTpIndex;  
    
  public:
    RateGlobalInfo()
    {
      m_cmpRateTpIndex = new CmpRateTpIndex(m_tpInfoList); 
    }
    virtual ~RateGlobalInfo()
    {
      delete m_cmpRateTpIndex;
    }
  public:
    //以下为测试而增加
    void dumpBillInfo();
    void dumpRateTpInfo();
    template<class T> void dumpBillInfoList(const vector<T> &r_billList) 
    {
      for(vector<T>::const_iterator t_itr=r_billList.begin();
          t_itr!=r_billList.end();++t_itr)
      {
        cout<<*t_itr<<endl;
      }
      cout<<"------------------------------------"<<endl;
    }
  private:
    //根据批价政策，将需要加锁的用户群ID加入r_groupUserList
    void setLockGroupUser(const RateTpInfo    &r_rateTpInfo,
                          const T_MEMBER_TYPE &r_roleCode,
                          vector<T_SERVER_ID> &r_groupUserList);
	//Commented By Sunam 2009/8/11 18:56:23
    //bool getGroupUserBill(const T_SERVER_ID &r_groupUserId);
    //计算r_month月的天数
    long getMonthDays(const long &r_month) const;
  public:
   	bool m_needIdInfo;//add for new version rentfee
   	bool m_needServState;//add for new version rentfee  
};
#endif //_RATEGLOBALINFO_H_INCLUDE_20080614_



