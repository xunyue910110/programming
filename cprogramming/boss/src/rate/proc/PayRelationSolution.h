#ifndef PAYRELATIONSOLUTION_H_HEADER_INCLUDED_B72F686C
#define PAYRELATIONSOLUTION_H_HEADER_INCLUDED_B72F686C
#include "RateGlobalInfo.h"
#include "ComBill.h"

// 完成用户付费关系处理
//##ModelId=48BCD4A401D7
class PayRelationSolution
{
  public:
    //##ModelId=48BCD4A402DB
    PayRelationSolution();

    //##ModelId=48BCD4A402DC
    ~PayRelationSolution();

    // * solutePayRelation: 完成用户付费关系记录的处理
    // * @param  r_userId                 用户标识.
    // * @param  r_pComBill               帐务处理全局共享数据对象指针.
    // * @param  r_pDetailItemFeeVector   用户明细帐单信息集.
    // * @param  r_pUserAccountBillVector 用户账户账单
    // * @param  r_pRateInfo 							批价全局共享数据对象指针
    // * @return int
    //          
    //##ModelId=48BCD4A402DD
    int solutePayRelation(const T_USER_ID& r_userId, 
	                        ComBill *r_pComBill,
	                        vector<DetailItemFee> *r_pDetailItemFeeVector,
	                        vector<UserAccountBill> *r_pUserAccountBillVector,
	                        RateGlobalInfo *r_pRateInfo
	                       );
    
  private:
    // * judgeUserPayRelation: 根据用户付费关系判断用户付费关系类型
    // * @param  无
    // * @return -1:错误 0：普通付费关系 1：复杂付费关系
    //##ModelId=48BCD4A4030D
    int judgeUserPayRelation();

    // * dealCommonUserPayRelation: 完成对普通用户进行付费关系处理
    // * @param  无.
    // * @return 无
    //##ModelId=48BCD4A4030E
    void dealCommonUserPayRelation();

    // * dealSpecialUserPayRelation: 完成对复杂用户进行付费关系处理
    // * @param  无.
    // * @return 无
    //##ModelId=48BCD4A40317
    void dealSpecialUserPayRelation();

    // * dealPayRelation: 完成对复杂用户一条付费关系记录的处理
    // * @param  payRelation      付费关系记录.
    //##ModelId=48BCD4A40321
    void dealPayRelation(const PayRelation& payRelation);

    // * computePayFee: 完成对一条付费关系记录计算帐户应该帮用户付的费用
    // * @param  payRelation             付费关系记录.
    // * @param  payItemFee              付费帐目上费用.
    // * @param  effectAcctBillFee       作用在帐户上费用.
    // * @param  effectUserBillFee       作用在用户上费用.
    // * @return true:该记录需要处理 false:该记录不需要处理
    //##ModelId=48BCD4A4032B
    bool computePayFee(const PayRelation& payRelation, int& payItemFee, int& effectAcctBillFee, int& effectUserBillFee);

    // * effectPayFee: 完成根据一条付费关系记录对用户费用进行绑定帐户处理
    // * @param  payRelation             付费关系记录.
    // * @param  payItemFee              付费帐目上费用.
    // * @param  effectAcctBillFee       作用在帐户上费用.
    // * @param  effectUserBillFee       作用在用户上费用.
    // * @return 无
    //##ModelId=48BCD4A40341
    void effectPayFee(const PayRelation& payRelation, const int& payItemFee, const int& effectAcctBillFee, const int& effectUserBillFee);

    // * getPayItemFee: 获取用户付费帐目上费用
    // * @param  payItemCode    用户付费帐目编码.
    // * @param  nfee           用户付费帐目费用.
    // * @return true:用户该付费帐目有费用 false:用户该付费帐目无费用
    //##ModelId=48BCD4A4034D
    bool getPayItemFee(const int& payItemCode, int& nfee);

    // * getDetailItemFromPayItem: 根据付费帐目编码获取对应明细帐目编码集
    // * @param  payItemCode    用户付费帐目编码.
    //##ModelId=48BCD4A40355
    void getDetailItemFromPayItem(const int& payItemCode);

    // * addAccountDetailItemFee: 完成对帐户下明细帐目的付费关系作用
    // * @param  r_acctId                   帐户标识.
    // * @param  r_userPriority             用户优先级.
    // * @param  r_defaultTag               默认帐户标志.
    // * @param  r_detailItemFee            明细帐目费用信息.
    // * @return 无
    //##ModelId=48BCD4A40357
    void addAccountDetailItemFee(const T_ACCT_ID& r_acctId, const int& r_userPriority, const char& r_defaultTag, DetailItemFee r_detailItemFee, int flag = 0);

    // * fillPayFee: 完成对帐户费用的补足处理
    // * @param  payRelation              付费关系.
    // * @param  effectUserBillFee        作用在用户帐单上的费用.
    // * @param  effectAcctBillFee        作用在帐户帐单上的费用.
    //##ModelId=48BCD4A4036A
    void fillPayFee(const PayRelation& payRelation, const int& effectUserBillFee, const int& effectAcctBillFee);

    // 取用户过户前的账户账单，并做相应处理
    //##ModelId=48C1DC5202B4
    bool dealUserBeforeBill();
    
    // * getPayLimit: 获取付费限额
    // * @param  payItemFee         付费帐目费用
    // * @param  feeChgLimitVec     付费限额容器.
    // * @param  feeChgLimit        付费限额.
//    bool getPayLimit(const int &payItemFee,vector<FeeChgLimit> feeChgLimitVec, FeeChgLimit &feeChgLimit);
		// * updatePresentMoney:     更新赠款
    // * @param  r_acctId              账户标识.
    // * @param  r_presentObjId        赠款对象实例标识.
    // * @param  r_presentMoney        赠送费用.
    // * @param  r_startCycleId        起始帐期.
    // * @param  r_endCycleId          结束帐期.
		void updatePresentMoney(const int &r_acctId,
	                         const T_FLAG &r_presentType,
                           const int &r_presentObjId, 
                           const int &r_presentMoney,
                           const int &r_startCycleId,
                           const int &r_endCycleId);

    // * getAddupInfo:      获取累计值
    // * @param  r_payRelationId    	付费关系实例.
    // * @param  r_beginTime        	起始时间.
    // * @param  r_addupItem        	累计帐目.
    // * @param  r_addupValue        	累计值.
    bool getAddupInfo(const long &r_payRelationId, 
    	                const long &r_beginTime, 
    	                const int &r_addupItem, 
    	                long &r_addupValue);   
   
    // * updateAddupInfo:      更新累计值
    // * @param  r_payRelationId    	付费关系实例.
    // * @param  r_addupItem        	累计帐目.
    // * @param  r_beginTime        	起始时间.
    // * @param  r_endTime        		结束时间.
    // * @param  r_effectFee        	作用值.    	                
		void updateAddupInfo(const long &r_payRelationId,
												const int &r_addupItem, 
												const long &r_beginTime,
												const long &r_endTime,
												const long &r_effectFee);    	                                    

	private:	
    //##ModelId=48BCD4A40263
    int m_cycleId;

    //##ModelId=48BCD4A40277
    vector<DetailItem> m_detailItemVector;

    //##ModelId=48BCD4A40281
    unsigned char m_detailItem[MAX_ITEM];

    //##ModelId=48BCD4A4028B
    int m_existDetailItem[MAX_ITEM];

    //##ModelId=48BCD4A40295
    vector<PayRelation>       *m_payRelationVector;

    //##ModelId=48BCD4A4029F
    vector<T_ACCT_ID>          *m_acctIdVector;

    //##ModelId=48BCD4A402A9
    vector<DetailItemFee>      *m_detailItemFeeVector;

    //##ModelId=48BCD4A402B3
    vector<UserAccountBill>   *m_userAccountBillVector;

    //##ModelId=48BCD4A402D1
    vector<UserAccountBill>   m_addUserAccountBillVector;

    //##ModelId=48BCD4A40209
    T_USER_ID 	m_userId;

    //##ModelId=48BCD4A4026E
    PM_Account *m_accountParam;
    
    ComBill *m_pComBill;
    
    //支付剩余额转入用户对应默认账户容器指针
    vector<AdditionalBill> *m_pAddBillVec;

    //批价全局信息
    RateGlobalInfo *m_pRateInfo;
    
    //用于多条付费关系共享总限额，优先级高付费关系剩余值供优先级低剩余值使用
    int m_leftPayFee;

};



#endif /* PAYRELATIONSOLUTION_H_HEADER_INCLUDED_B72F686C */
