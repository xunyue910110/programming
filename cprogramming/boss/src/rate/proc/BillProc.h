
#ifndef __BILLPROC_H__
#define __BILLPROC_H__

#include "config-all.h"

#include "Procedure.h"

#include "RateCdr.h"
#include "FieldsXX.h"
#include "BillStruct.h"


class GetBillProc:public Procedure 
{
  public:
    virtual int execute();
};          


class UpdateBillProc:public Procedure 
{
  public:
    virtual int execute();
    //Added By Sunam 2009/8/10 16:20:37
    //提交本地账单至内存数据库,同时需要插入回滚账单
    int commitToMdb1();
    //提交本地账单至内存数据库,不需要插入回滚账单
    int commitToMdb2();
    //提交用户账单至本地缓存
    int commitToLocal();
    //Added By Sunam 2009/8/20 10:11:18
    int commit();
    int rollBack();
  private:
    //以下存放：0 存放更新的帐单,1 存放新增的帐单
    //Modified By Sunam 2009/8/11 20:14:21
    //vector<UserBill>       m_userBills[2];
    //vector<UserSumBill1>   m_userSumBill1s[2];
    //vector<UserSumBill2>   m_userSumBill2s[2];
    //vector<AccountBill>    m_accountBills[2];
    //vector<RateDiscntBill> m_rateDiscnts[2];
    map<T_SERVER_ID,vector<UserBill>       > m_userBills[2];
    map<T_SERVER_ID,vector<UserMonfeeBill> > m_userMonfeeBills[2];
    map<T_SERVER_ID,vector<UserSumBill1>   > m_userSumBill1s[2];
    map<T_SERVER_ID,vector<UserSumBill2>   > m_userSumBill2s[2];
    map<T_SERVER_ID,vector<UserSumBill3>   > m_userSumBill3s[2];
    map<T_SERVER_ID,vector<AccountBill>    > m_accountBills[2];
    map<T_SERVER_ID,vector<RateDiscntBill> > m_rateDiscnts[2];
    char                   m_errMsg[1024];
  private:
    //template<class T>
    //void setBill(const vector<T> &r_newBillList,vector<T> r_billList[2],const RollBackBill &r_rollBill);
    template<class T>
    void setBill(const vector<T> &r_newBillList,map<T_SERVER_ID,vector<T> > r_billList[2],const RollBackBill &r_rollBill,const T_SERVER_ID &r_userId);
    //Added By Sunam 2009/8/11 20:11:47 更新r_userBill到r_localBill
    template<class T>
	void updateToBillMap(map<T_SERVER_ID,vector<T> > &r_userBill,map<T_SERVER_ID,vector<T> > &r_localBill);
	//Added By Sunam 2009/8/11 20:11:47 插入r_userBill到r_localBill
	template<class T>
	void insertToBillMap(map<T_SERVER_ID,vector<T> > &r_userBill,map<T_SERVER_ID,vector<T> > &r_localBill);
	
	template<class T>
	void genIncreBill(map<T_SERVER_ID,vector<T> > &r_orgBill, map<T_SERVER_ID,vector<T> > &r_newBill, vector<T> r_increBill[2]);
	
  private:
    void addRollBackBill();
    void printInterBill();
};          

class RollBackBillProc:public Procedure
{
  public:
    virtual int execute();
    static bool cmpRollBackBill(const RollBackBill &r_left,const RollBackBill &r_right);
  private:
    bool rollbackUserBill(const T_SERVER_ID &r_userId,const vector<RollBackBill> &r_userRollBill);
    bool rollbackUserBill(const T_SERVER_ID &r_userId,
                          const int &r_billCycle,
                          const int &r_billType,
                          const vector<RollBackBill> &r_userRollBill);
    //回退用户类账单模板函数
    template<class T>
    bool rollbackUserBill(const int &r_billCycle,const vector<RollBackBill> &r_rollBillList);
    //帐户账单回退方法
    bool rollbackAcctBill(const int &r_billCycle,const vector<RollBackBill> &r_rollBillList);
  private:
    int  m_channelNo;
};

#endif      

            
            