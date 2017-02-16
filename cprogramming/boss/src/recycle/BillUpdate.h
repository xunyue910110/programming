
#ifndef __BillUpdate_H__
#define __BillUpdate_H__

#include "config-all.h"

#include "RateCdr.h"
//#include "FieldsXX.h"
#include "BillStruct.h"
#include <map>

class BillManager;

class BillUpdate
{
  public:
    void initialization(BillManager* r_billMgr,int& r_channelNo);
    bool rollbackOldBill();
    bool update(const int& r_billCycle,T_SERVER_ID& r_userId, char* r_billItem,
                vector<RollBackBill>& r_rollBackBillVec);
    bool clearRollBackBill();
    bool commit();
  private:
  	bool splitBillItem(const int& r_billCycle,T_SERVER_ID& r_userId,char* r_billItem,vector<RollBackBill>& r_rollBackBill);
  	void getUserBill();
  	template<class T>
		void genIncreBill(map<T_SERVER_ID,vector<T> > &r_orgBill, map<T_SERVER_ID,vector<T> > &r_newBill, const bool &flag, vector<T> r_increBill[2]);
	template<class T>
		void updateToLocalBillMap(const T &r_userBill,map<T_SERVER_ID,vector<T> > &r_localBill);

  private:
    int  m_channelNo;
    BillManager     *m_billMgr;
    
    //Added By Sunam 2011/3/22 9:48:11
    //用户话单中需要回退的账单汇总
    map<T_SERVER_ID,vector<UserBill>       > m_tUserBill[12];
	map<T_SERVER_ID,vector<UserSumBill1>   > m_tUserSumBill1[12];
	map<T_SERVER_ID,vector<UserSumBill2>   > m_tUserSumBill2;
	
	//用户回退前的原始账单
	map<T_SERVER_ID,vector<UserBill>       > m_tUserBill_org[12];
	map<T_SERVER_ID,vector<UserSumBill1>   > m_tUserSumBill1_org[12];
	map<T_SERVER_ID,vector<UserSumBill2>   > m_tUserSumBill2_org;
	
	//用户账单回退后，费用发生变动的账单
	vector<UserBill>     m_increUserBill[12][2];
	vector<UserSumBill1> m_increUserSumBill1[12][2];
	vector<UserSumBill2> m_increUserSumBill2[2];
};

#endif      
