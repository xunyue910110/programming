#ifndef _BILL_MANAGER_H_20080702_
#define _BILL_MANAGER_H_20080702_
#include <vector>
#include <set>
#include "Session.h"
#include "DataBase.h"
#include "InputParams.h"
#include "FieldsXX.h"
#include "BillStruct.h"
#include "MdbInfo.h"

class BillManager
{
  public:
    BillManager();
  public:
    const string& getErrorMsg(){return m_errMsg;};
    bool initialization(const MdbInfo& r_mdbInfo);
    bool end();
    //取批价运行信息
    bool getRateRunInfo(RateRunInfo &rateRunInfo);
    //更新批价运行信息
    //根据通道号,更新m_state、m_fileName、m_rateDate、m_nextFileNo
    bool updateRateRunInfo(const RateRunInfo &rateRunInfo);
    
    //插入回滚帐单信息:话单级
    bool insertRollBackBill(const vector<RollBackBill> &rollbackBill);
    
    //文件级清理回滚帐单：以通道为单位清理
    bool clearRollBackBill(const int & r_channelNo);
    bool getRollBackBill(const int & r_channelNo,vector<RollBackBill> &rollbackBill);
    
    //用户或者帐户加解锁：type表示ID类型（0 用户 , 1帐户）
    bool lockUserId(const T_SERVER_ID &r_userId,const int &r_type,const int &r_channelNo);
    
    //Added By Sunam 2009/8/9 8:52:19批量加锁
    bool lockUserId(const set<T_SERVER_ID> &r_userId,const set<T_SERVER_ID> &r_acctId,const int &r_channelNo);
    //Added By zb 2009/8/12 批量加锁
    bool lockUserId(const set<T_SERVER_ID> &r_userId,const int &r_type,const int &r_channelNo);
	
    bool unlockUserId(const T_SERVER_ID &r_userId,const int &r_type);
    bool clearUserLock(const int &r_channelNo);
    //增加一个等待超时的lock接口 timeOut为ms
    bool lockUserId_w(const T_SERVER_ID &r_userId,const int &r_type,
                      const int &r_channelNo,const int &timeOut = 100);
    
    //取该用户相关的所有帐单
    //包括：普通用户帐单、用户月累计量、用户跨月累计量
    //      账户帐单
    //注： 传入的容器，在求取时不清空.需在外部做清空处理
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserBill>     &r_bills);
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill1> &r_bills);
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill2> &r_bills);
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill3>  &r_bills);//add for monfee by fug
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_accountId,vector<AccountBill>  &r_bills);
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<RateDiscntBill>  &r_bills);
    bool getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserMonfeeBill>  &r_bills);//add for monfee by fug
    
    //修改的帐单项部分,全量方式覆盖
    bool updateUserBill(const int &r_cycle,const vector<UserBill>      &r_bills);
    bool updateUserBill(const int &r_cycle,const vector<UserSumBill1>  &r_bills);
    bool updateUserBill(const int &r_cycle,const vector<UserSumBill2>  &r_bills);
    bool updateUserBill(const int &r_cycle,const vector<UserSumBill3>  &r_bills);//add for monfee by fug
    bool updateUserBill(const int &r_cycle,const vector<AccountBill>   &r_bills);
    bool updateUserBill(const int &r_cycle,const vector<RateDiscntBill>&r_bills);
    bool updateUserBill(const int &r_cycle,const vector<UserMonfeeBill> &r_bills);//add for monfee by fug

    //增加的部分   
    bool insertUserBill(const int &r_cycle,const vector<UserBill>      &r_bills);
    bool insertUserBill(const int &r_cycle,const vector<UserSumBill1>  &r_bills);
    bool insertUserBill(const int &r_cycle,const vector<UserSumBill2>  &r_bills);
    bool insertUserBill(const int &r_cycle,const vector<UserSumBill3>  &r_bills);//add for monfee by fug
    bool insertUserBill(const int &r_cycle,const vector<AccountBill>   &r_bills);
    bool insertUserBill(const int &r_cycle,const vector<RateDiscntBill>&r_bills);
    bool insertUserBill(const int &r_cycle,const vector<UserMonfeeBill>&r_bills);//add for monfee by fug
    
    //删除帐单
    bool deleteUserBill(const int &r_cycle,const vector<UserBill>      &r_bills);
    bool deleteUserBill(const int &r_cycle,const vector<UserSumBill1>  &r_bills);
    bool deleteUserBill(const int &r_cycle,const vector<UserSumBill2>  &r_bills);
    bool deleteUserBill(const int &r_cycle,const vector<UserSumBill3>  &r_bills);//add for monfee by fug
    bool deleteUserBill(const int &r_cycle,const vector<AccountBill>   &r_bills);
    bool deleteUserBill(const int &r_cycle,const vector<RateDiscntBill>&r_bills);
    bool deleteUserBill(const int &r_cycle,const vector<UserMonfeeBill>&r_bills);//add for monfee by fug
    //Added By Sunam 2009/8/20 9:57:46
    //added by wanglu 2011-2-24
    bool getUserAcctDeposit(const T_SERVER_ID &r_acctId,vector<AcctDeposit> &r_acctDeposit);
    //end 2011-2-24
    bool commit();
    bool rollBack();
  private:
  	bool connect();
  	bool disconnect();
  private: //内存数据库信息
    MdbInfo        m_mdbInfo;
 	  DataBase      *m_db;
	  Session       *m_session;
	  bool           m_connected;
	  int            m_errno;
	  string         m_errMsg;
	  
	  //一些内部数据
	  vector<string> m_LockUserFields;
	  vector<string> m_rateInfoUpFields;
	  
	  vector<string> m_userBillUpFields;
	  vector<string> m_userMonfeeBillUpFields;//add for monfee by fug
	  vector<string> m_userSum1UpFields;
	  vector<string> m_userSum2UpFields;
	  vector<string> m_userSum3UpFields;//add for monfee by fug
	  vector<string> m_accntBillUpFields;
	  vector<string> m_rateDiscntUpFields;
	  vector<string> m_userBillInFields;
	  vector<string> m_userMonfeeBillInFields;//add for monfee by fug
	  vector<string> m_userSum1InFields;
	  vector<string> m_userSum2InFields;
	  vector<string> m_userSum3InFields;//add for monfee by fug
	  vector<string> m_accntBillInFields;
	  vector<string> m_rateDiscntInFields;
	  vector<string> m_rollBillInFields;
  //Added By Sunam 2009/9/1 13:11:48 存放处理错误记录
  public :
    vector<string> m_errVector;
};
#endif //_BILL_MANAGER_H_20080702_
