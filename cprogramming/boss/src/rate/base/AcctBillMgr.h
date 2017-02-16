#ifndef ACCTBILLMGR_H_HEADER_INCLUDED_B599470D
#define ACCTBILLMGR_H_HEADER_INCLUDED_B599470D


#include "mgr/DetailBillMgr.h"
#include "BillStruct.h"
#include "AccountCommon.h"
#include "RateGlobalInfo.h"

typedef IdInfo    T_ID_INFO;

// 用户账单，明细账单和账户账单之间的转换功能
//##ModelId=4A66BF54001A
class AcctBillMgr
{
  public:
  	//初始化
  	bool  initialize(RateGlobalInfo *r_pRateInfo);
  	
    /**
    * updateUserBill:       更新用户账单.
    * @param  r_detailBill      明细账单.
    * @param  r_accountBills    用户账单.
    * @return true:成功 false：失败       
    */ 
    bool updateUserBill(const DetailBill &r_detailBill, vector<UserBill> &r_userBills); 

    /**
    * getAccountBill:          取账户账单.
    * @param  r_acctIdSet   		账户集合.
    * @param  r_userIdSet      用户集合.
    * @param  r_accountBills   账户账单.
    * @return 无      
    */ 
    void getAccountBill(const set<T_SERVER_ID> &r_acctIdSet,  
                        const set<T_USER_ID>  &r_userIdSet,
                        vector<AccountBill> &r_accountBills);  

    /**
    * updateAccountBill:更新账户账单
    * @param  r_detailBills     明细账单集合.
    * @param  r_accountBills    账户账单.
		* @param  r_idInfo          用户信息.
    * @return true:成功 false：失败       
    */
    bool updateAccountBill(const vector<DetailBill> &r_detailBills,
                           vector<AccountBill> &r_accountBills,
                           const IdInfo &r_idInfo);  
    /**
    * updateAccountBill:更新账户账单
    * @param  r_detailBill      明细账单.
    * @param  r_accountBills    账户账单.
    * @param  r_idInfo          用户信息.
    * @return true:成功 false：失败       
    */                    
		bool updateAccountBill(DetailBill r_detailBill,
		                       vector<AccountBill>  &r_accountBills,
		                       const IdInfo &r_idInfo);

  /**
	* getRateDiscntBill:        转换用户账户账单成计费优惠账单
	* @param  r_userId                 用户标识
	* @param  r_userAccountBills       用户账户账单
	* @param  r_rateDiscntBills        计费优惠账单
	* @return 无
	*/                    
	void getRateDiscntBill(const T_SERVER_ID &r_userId,
	                       		vector<UserAccountBill> r_userAccountBills,
	                     		vector<RateDiscntBill> &r_rateDiscntBills);  

 //账单管理
/**
* getDetailBill: 获取用户的明细账单（从账户账单容器获取，添加满足条件的用户到明细账单）
* @param  r_userId：账户ID
* @param  r_accountBills. 账户账单容器
* @param  r_detailBill. 明细账单
* @param  r_idInfo. 用户信息
* @return true 成功, false 失败.
* @remark  条件仅仅判断用户的属性idinfo
*/ 
 bool getDetailBill(const T_SERVER_ID &r_userId,IdInfo &r_idInfo, 
                    const vector<AccountBill> &r_accountBills, 
                    DetailBill &r_detailBill);
    /**
		* getDetailBill 根据用户标识和用户账单获取明细账单
		* @param  r_id         用户标识
		* @param  r_idtype     用户类型
		* @param  r_userId     用户标识
		* @param  r_userBills  用户账单
		* @param  r_detailBill 明细账单
		* @return true 成功,false 失败  
		*/ 
    //##ModelId=48E02D740149
    bool getDetailBill(const T_SERVER_ID &r_id, 
                       const ID_TYPE &r_idtype,
                       const T_USER_ID &r_userId,
                       const vector<UserBill> &r_userBills,
                       DetailBill &r_detailBill);
		                 
	  /**
		* getDetailBill 获取用户群中除出帐用户外的账单
		* @param  r_userGroupId     群标识
		* @param  r_userId         	用户标识
		* @param  r_userIdSet     	群成员集合
		* @param  r_detailBill      明细账单
		* @return true 成功,false 失败  
		*/ 
    bool getDetailBill(const T_SERVER_ID &r_userGroupId,
                       const T_USER_ID &r_userId,
	                     set<T_USER_ID> &r_userIdSet,
	                     DetailBill &r_detailBill
	                     );
   
	  /**
		* getDetailBill 获取账户集明细账单，并获取相应账户账单备份
		* @param  r_acctIdSet         账户集合
		* @param  r_userIds           用户集合
		* @param  r_detailBills     	明细账单
		* @param  r_accountBills      账户账单
		* @return true 成功,false 失败  
		*/  
	bool getDetailBill(set<T_ACCT_ID> &r_acctIdSet, 
	                     set<T_USER_ID> &r_userIds,  
	                     vector<DetailBill> &r_detailBills,
	                     vector<AccountBill> &r_accountBills
	                     );
    
    /**
		* getDetailBill 把账户的账户账单转换成明细账单
		* @param  r_acctId            账户标识
		* @param  r_userIds           用户集合
		* @param  r_accountBills      账户账单
		* @param  r_detailBill     	  明细账单
		* @return true 成功,false 失败  
		*/  
    bool getDetailBill(const T_ACCT_ID &r_acctId,
	                     const set<T_USER_ID> &r_userIds,
	                     vector<AccountBill> &r_accountBills,
	                     DetailBill &r_detailBill);
     
		 /**
		* getDetailBill 把用户的用户账单转换成明细账单
		* @param  r_userId                用户标识
		* @param  r_userAccountBills      用户账户账单
		* @param  r_detailBills     	    明细账单
		* @return true 成功,false 失败  
		*/ 
		bool getDetailBill(const T_USER_ID &r_userId, 
		                   const vector<UserAccountBill>  &r_userAccountBills,
		                   vector<DetailBill> &r_detailBills);

     /**
    * getDetailBill:         取用户/用户群明细账.
    * @param  r_id           三户标识.
    * @param  r_idType       三户类型.
    * @param  r_cycleId      帐期标识.
    * @param  r_detailBill   明细账单.
    * @return true:成功 false：失败       
    */ 
    bool getDetailBill(const T_USER_ID &r_id,
                       const ID_TYPE &r_idType,  
                       DetailBill &r_detailBill);
                       
                       
    /**
		* getDetailBill 根据把群下指定角色用户转换成明细账单
		* @param  r_userGrpId  虚拟用户标识
		* @param	r_userIdSet	 用户群下成员
		* @param  r_userBills  用户账单
		* @param  r_detailBill 明细账单
		* @param  r_roleCode   角色
		* @param  m_roleCodeVector   枚举角色：离散
		* @param  m_roleCodeMap   枚举角色：连续
		* @return true 成功,false 失败  
		*/ 
    bool getDetailBill(const T_SERVER_ID &r_userGrpId, 
                       set<T_USER_ID> &r_userIdSet,
                       const vector<UserBill> &r_userBills,
                       DetailBill &r_detailBill,
                       const T_MEMBER_TYPE &r_roleCode,
											 vector<T_MEMBER_TYPE> 	&r_roleCodeVector,  
											 map<T_MEMBER_TYPE, T_MEMBER_TYPE> &r_roleCodeMap
                       );  
		                       
		 /**
		* getDetailBill:转换用户账单成明细账单
		* @param	r_id	虚拟用户标识
		* @param	r_userId	用户标识
		* @param	r_userBills	用户账单
		* @param  r_detailBill 明细账单	
		* @return	true false
		*/
		bool getDetailBill(const T_SERVER_ID &r_id, 
	                     const T_USER_ID &r_userId,
	                     const vector<UserBill> &r_userBills,
	                     DetailBill &r_detailBill
	                     );   
	                     
/**
* getDetailBill:转换多个用户的用户账单成明细账单
* @param	r_id	虚拟用户标识
* @param	r_userIds	用户标识结合
* @param	r_userBills	用户账单
* @param  r_detailBill 明细账单	
* @return	true false
*/
	bool getDetailBill(const T_ACCT_ID &r_userGrpId,
										 const set<T_USER_ID> &r_userIds,
	                   const vector<UserBill> &r_userBills,
	                    DetailBill &r_detailBill
	                    );                                                          
  
/**
	* getDetailItemFee:        转换计费优惠账单成内部明细账单
	* @param  r_userId                用户标识
	* @param  r_rateDiscntBills       计费优惠账单
	* @param  r_detailItemFees        明细费用
	* @return 无
	*/   
	void getDetailItemFee(const T_SERVER_ID &r_userId,
	                   		vector<RateDiscntBill> r_rateDiscntBills,
	                   		vector<DetailItemFee> &r_detailItemFees);
  /**
	* userBillToGrpBill: 是否出月末.
  * @param  r_detailBill 用户群明细账单.
	* @return 无.
	*/ 	
	void userBillToGrpBill(DetailBill &r_detailBill);	
	
	 /**
		* getRoleBill:   取群下对应角色的账单
		* @param  r_groupId              群用户标识
		* @param  r_roleCode             角色编码 （-1表示所有用户）               
		* @param  r_idType               USER_ID_TYPE  取群账单， ACCT_ID_TYPE 取账户账单              
		* @return 明细账单指针
		*/  
  DetailBill *getRoleBill(const T_SERVER_ID  &r_groupId,
                          T_MEMBER_TYPE   &r_roleCode,
                          ID_TYPE r_idType=USER_ID_TYPE);
                          	
                        	 	
	//add for acct Discnt  begin 2009-9-10 14:56:42
  /**
	* getAccountBill: 获取账户账单
	*（从账户账单表获取账户下的所有账单到账户账单备份容器r_accountBills，
	*  添加非当前出帐用户到账户账单容器r_accountBills_new）
  * @param  r_acctIds：账户ID集合
  * @param  r_userIds：出帐用户ID集合
  * @param  r_accountBills_new. 账户账单容器
  * @param  r_accountBills. 账户账单容器备份
	* @return true 成功, false 失败.
	*/ 
  bool getAccountBill(const set<T_SERVER_ID> &r_acctIds,
             		 			const set<T_SERVER_ID> &r_userIds,
	  	           			vector	<AccountBill>  &r_accountBills_new,  
	  	           			vector	<AccountBill>  &r_accountBills);

  /**
	* getDetailBill: 获取用户的明细账单（从账户账单容器获取，添加满足条件的非当前出帐用户到明细账单）
  * @param  r_acctId：账户ID
  * @param  r_accountBills. 账户账单容器
  * @param  r_detailBill. 明细账单
  * @param  r_idInfo. 用户属性
  * @param  r_discount. 资费信息
	* @return true 成功, false 失败.
	* @remark  条件仅仅判断用户的属性idinfo
	*/ 
	bool getDetailBill(const T_SERVER_ID &r_acctId, 
	                  const vector<AccountBill> &r_accountBills, 
	                  DetailBill &r_detailBill,
	                  IdInfo &r_idInfo, 
	                  Discount *r_discount=NULL);
	
	
	/**
  * updateAccountBill:更新账户账单(用户账户账单r_userAccountBillVector->账户账单r_accountBills)
  * @param  r_userId                  	用户标识.
  * @param  r_userAccountBillVector     用户账户账单集合.
  * @param  r_accountBills    账户账单.
  * @param  r_idInfo          用户信息.
  * @return true:成功 false：失败       
  */
	bool updateAccountBill(const T_SERVER_ID &r_userId, 
	                       const vector<UserAccountBill> &r_userAccountBillVector, 
	                       vector<AccountBill> &r_accountBills,const IdInfo &r_idInfo);
	
	/**
* updateAccountBill: 更新账户账单(帐务优惠a_discnt=用户+群级优惠+账户级）
* @param    accountBills         账户账单
* @return    true false
*/
	bool updateAccountBill(vector<AccountBill>  &r_accountBills);
	
	//add for acct Discnt  end 2009-9-10 14:56:42	
	
	public:
	     /**
    * getSpecialSumBill:取特殊累计量和累计量备份
    * @param  r_id             三户标识.
    * @param  r_sumBillList    累计量.
    * @param  r_sumBillListBak 累计量备份.
    * @return true:成功 false：失败       
    */
    bool getSpecialSumBill(const T_SERVER_ID  &r_id, 
                           vector<UserSumBill2> &r_sumBillList,
                           vector<UserSumBill2> &r_sumBillListBak);
    
    /**
    * getSpecialSumBill:取特殊累计量和累计量备份
    * @param  r_idSet          用户集合.
    * @param  r_sumBillList    累计量.
    * @param  r_sumBillListBak 累计量备份.
    * @return true:成功 false：失败       
    */
    bool getSpecialSumBill(set<T_SERVER_ID> r_idSet, 
                           vector<UserSumBill2> &r_sumBillList,
                           vector<UserSumBill2> &r_sumBillListBak);
   //更新用户账单部分字段                     
   void updatePartBill(vector<UserBill> &r_billList,const UserBill &r_bill);                        

	public:
	int m_divisor;
	
	ComBill *m_pComBill;
  RateGlobalInfo  *m_pRateInfo; 
};





#endif /* ACCTBILLMGR_H_HEADER_INCLUDED_B599470D */
