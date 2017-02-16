#ifndef COMBILL_H_HEADER_INCLUDED_B728A5B2
#define COMBILL_H_HEADER_INCLUDED_B728A5B2

#include "mgr/DetailBillMgr.h"
#include "BillStruct.h"
#include "BillManager.h"
#include "AccountCommon.h"
#include "PMManager.h"
#include "PolicyInfo.h"


typedef PMManager PM_Account;
typedef IdInfo    T_ID_INFO;
typedef map<char,int> stateMap;

// 帐务处理属性类，如用户信息，账户信息，用户状态，账单等。
//##ModelId=48D45F0200B8
class ComBill
{
	public:
    // 构造函数
    //##ModelId=48D6F61703E4
		ComBill();
   
    //析构函数
    //##ModelId=48D6F62E036F
    virtual ~ComBill();

  	/**
		* initialize 初始化
		* @param  r_channleNo         通道号
		* @param  r_provinceCode      省份代码
		* @param  r_pBillMgr          账单管理对象指针
		* @param  r_pAccountParam     资料管理对象指针
		* @param  r_pAddBillVector    附加账单容器指针
		* @param  r_pDiscntBillVector 优惠轨迹容器指针
		* @param  r_ppCdr             指向话单指针的指针
		* @paran  r_pErrMsg           错误信息指针
		* @paran  r_pUserServStateVector   用户服务状态容器指针
		* @paran  r_pUserServVector        用户服务容器指针
		* @return true：成功 false：失败 
		*/
		//##ModelId=48DB3EEE01B8        
    bool initialize(const int &r_channleNo,
                    const string &r_provinceCode,
                    BillManager *r_pBillMgr,
                    PM_Account *r_pAccountParam,
                    vector<AdditionalBill> *r_pAddBillVector,
                    vector<DiscntBill>  *r_pDiscntBillVector,
	                  const PCDR           &r_pCdr,
	                  PString          *r_pErrMsg,
	                  vector<UserServState>  *r_pUserServStateVector,
	                  vector<UserServ>    *r_pUserServVector
                    );
                
    /**
		* initUser 初始化用户
		* @param  r_userId         用户标识
		* @param  r_cycParam       帐期信息
		* @return true 成功,false 失败 
		*/ 
    //##ModelId=48D6F97F021B
    bool initUser(const T_USER_ID &r_userId,const CycParam &r_cycParam);
   
    /**
		* initUser 初始化当前账户
		* @param  r_acctId         账户标识
		* @return 无
		*/ 
    //##ModelId=48D6F9930062
    void initAcct(const T_ACCT_ID &r_acctId);

    /**
		* initUser 初始化当前用户群
		* @param  r_acctId         群标识
		* @return void 
		*/ 
    //##ModelId=48D6F9A70060
    void initUserGroup(const T_USER_ID &r_id);
                   
    /**
		* getUserInfo 根据用户标识获取用户详细信息
		* @param  r_id         用户标识
		* @param  r_idInfo     用户信息结构
		* @return true 成功,false 失败 
		*/ 
    //##ModelId=48D6F68C0099
    bool getUserInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo);

   
    /**
		* getUserGroupInfo 根据群标识获取群详细信息
		* @param  r_id         群标识
		* @param  r_idInfo     群信息结构
		* @return true 成功,false 失败 
		*/
    //##ModelId=48D6F7B801DB
    bool getUserGroupInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo);

    /**
		* getAcctInfo 根据账户标识获取账户详细信息
		* @param  r_id         账户标识
		* @param  r_idInfo     账户信息结构
		* @return true 成功,false 失败 
		*/
    //##ModelId=48D6F893006D
    bool getAcctInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo);

                        
    /**
    * getGroupInfoByUserId: 获取用户群信息结构
    * @param  无.
    * @return true 成功, false 失败.      
    */
    bool getGroupInfoByUserId();
  
    /**
    * GetUserInfoByGrpId: 根据群ID获取群成员信息结构
    * @param  r_groupId 群ID.
    * @return true 成功, false 失败.      
    */
    bool GetUserInfoByGrpId(const T_SERVER_ID &r_groupId);
    /**
    * getDetailItems:获取所有明细帐目数组
    * @param  无.
    * @return true 成功, false 失败.      
    */
    bool getDetailItems( );
   
     /**
    * carry: 进位.
    * @param  帐目.
    * @param  值.
    * @return 进位后的值.
    */   
   long carry(const int &r_itemCode, const long &r_value);
   
    /**
    * ifItemExists:判断明细帐目是否存在
    * @param  r_detailItem 明细帐目.
    * @return true:成功 false：失败      
    */
    bool ifItemExists(const int &r_detailItem);
          
    /**
    * getLastStopTime:获取用户首次停机时间
    * @param  r_userInfo 用户信息结构.
    * @return true:成功 false：失败      
    */
    bool getLastStopTime(IdInfo &r_userInfo);
   
  	/**
		* judgeNotOpen判断在时间段内用户的主服务是否开通过
		* @param  startDate  开始时间
		* @param  endDate    结束时间
		* @param  endDate    用户当前的服务状态集
		* @return true：没有开通 false：有开通 
		*/      
		bool judgeNotOpen(const string& r_startDate,
                      const string& r_endDate,
                      const vector<UserServState>& r_userServStateVector);
//add by yuxk begin 2009-9-27 20:48:40
    /**
    * getLastITime:获取用户最后一次停保时间
    * @param  r_attrValue 最后一次停保时间.
    * @param  r_flag:  true 取最后一次停保时间，后面开机也算 ,false 取最后一次停保时间，后面开机不算， 默认为false .
    * @return 无      
    */
    void getLastITime(T_OBJECT_ATTR &r_attrValue,bool r_flag=false);
    /**
    * getLastITimenew:获取用户最后一次停保时间
    * @param  r_attrValue 最后一次停保时间.
    * @param  r_flag:  true 取最后一次停保时间，后面开机也算 ,false 取最后一次停保时间，后面开机不算，拆机算 默认为false .
    * @return 无      
    */
    
    void getLastITimenew(T_OBJECT_ATTR &r_attrValue,bool r_flag=false);
   
  	/**
		* judgeOnlyI判断在时间段内用户的主服务是否有停机保号以外的服务状态
		* @param  startDate  开始时间
		* @param  endDate    结束时间
    * @param  r_flag:  true 可以紧跟拆机 ,false 不可以紧跟拆机， 默认为false .
		* @param  userServStateVector    用户当前的服务状态集
    * @return true：没有其他状态 false：有其他状态
		*/      
		bool judgeOnlyI(const string& r_startDate,
                      const string& r_endDate,
                      const vector<UserServState>& r_userServStateVector,bool r_flag=false);
    /**
    * getLastStopOpenTime:获取用户最后一次停保复活时间
    * @param  r_userInfo 用户信息结构.
    * @param  r_isObjUse 是否为对象使用，默认为非对象使用 add by xuf 2010-1-13 23:29:06
    * @return r_attrValue     
    */
    void getLastStopOpenTime(T_OBJECT_ATTR &r_attrValue,bool r_isObjUse=false);
    /**
    * getFirstStopOpenTime:获取用户某个时间段内首次停保复话时间，无则返回"20501231000000"
    * @param  r_beginDate 开始时间
    * @param  r_endDate 结束时间
		* @param  userServStateVector    用户当前的服务状态集
    * @return r_attrValue     
    */
    void getFirstStopOpenTime(T_OBJECT_ATTR &r_attrValue,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate);
   
    bool judegeEarlieastStopState(const string& startDate,
                           const vector<UserServState>& userServStateVector);
    void getLastServStopTime(T_OBJECT_ATTR &r_attrValue);
    
    bool judgeServHalfMonth(const int &r_attrValue,const int &r_timeRangeFlag); //add by yuxk 2010-1-27 16:31:25
    
//add by yuxk end 2009-9-27 20:48:44                     
//add by yuxk for shanxi begin
  /**
	* judgeServStartFirstHalf      服务开始时间上半月
	* @param  r_attrValue   服务编码
	* @return true 存在 ,false 不存在
	*/    
//  bool judgeServStartFirstHalf(const int &r_attrValue);
//  /**
//	* judgeServStartLastHalf:      服务开始时间下半月
//	* @param  r_attrValue   服务编码
//	* @return true 存在 ,false 不存在
//	*/    
//  bool judgeServStartLastHalf(const int &r_attrValue);
//  /**
//	* judgeServEndFirstHalf:      服务结束时间上半月
//	* @param  r_attrValue   服务编码
//	* @return true 存在 ,false 不存在
//	*/    
//  bool judgeServEndFirstHalf(const int &r_attrValue);
//  /**
//	* judgeServEndLastHalf:      服务结束时间下半月
//	* @param  r_attrValue   服务编码
//	* @return true 存在 ,false 不存在
//	*/    
//  bool judgeServEndLastHalf(const int &r_attrValue);
//add by yuxk for shanxi end
   
//add by xuf begin 2010-1-21 22:15:49
		//judgeServStart判断用户服务开始时间是否在本月上下半月
		bool judgeServStart(const int &r_servId,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate);

    //lastTimesNotExistServer判断前一秒是否存在相同服务
		bool lastTimesNotExistServer(const int &r_servId,const T_DATE_TIME &r_beginDate);

    //judgeServEnd判断用户服务结束时间是否在本月上下半月
		bool judgeServEnd(const int &r_servId,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate);

    //nextTimesNotExistServer判断后一秒是否存在相同服务
		bool  nextTimesNotExistServer(const int &r_servId,const T_DATE_TIME &r_beginDate);
//add by xuf end 2010-1-21 22:15:57   
   
   
		/**
		* getUserPayInfo:    取用户付费关系
		* @param  r_userId   用户标识        
		* @return： true 成功, false 失败
		*/ 
		bool getUserPayInfo(const T_SERVER_ID &r_userId);
		
		/**
		* Added By Sunam 2009/8/27 10:21:34 取批量用户的付费关系
		* getUserPayInfo:    取用户付费关系
		* @param  r_userId   用户标识        
		* @return： true 成功, false 失败
		*/ 
		bool getUserPayInfo(const vector<T_SERVER_ID> &r_userId);
   	
   	/**
		* Added By xuf 2009/8/27 10:21:34 取批量用户的付费关系
		* getUserPayInfo:    取用户付费关系
		* @param  r_userId   用户标识        
		* @return： true 成功, false 失败
		*/ 
		bool getUserPayInfo(const set<T_SERVER_ID> &r_userIds);

  	/**
		* getTotalDiscntGroupUsers:   取总量优惠用户群用户
		* @return true:成功 false:失败
		*/ 
		bool  getTotalDiscntGroupUsers();
	
  /**
	* setErrorCode:                设置错误代码
	* @param  r_id                 用户标识
	* @param  r_errCode            计费优惠账单
	* @param  r_operTypeTag        操作类型标志：0未处理，1已处理
  * @param  r_idType             标识类型
  * @param  r_fValue             辅助字段1
	* @param  r_sValue             辅助字段2
	* @return 无
	*/  
		void setErrorCode(const T_SERVER_ID &r_id,
	                  const int &r_errCode,
	                  const T_FLAG &r_operTypeTag="0",
	                  ID_TYPE r_idType=USER_ID_TYPE,
	                  const long &r_fValue=0,
	                  const long &r_sValue=0);
	                        
	
	//获取用户存在某些状态的天数
	void getDaysByState(const char*				  r_stateSet,
                  	const T_DATE_TIME   &r_beginTime,
                  	const T_DATE_TIME   &r_endTime,
                    unsigned int &r_dayNum);
   
   //获取本帐期有的服务开始时间 added by liugj for NX 20090429
   bool getServStartEndDate(const int svc, T_DATE_TIME& startDate, T_DATE_TIME& endDate);
	                    	
  
  /**
	* updateBalanceAcctId:      更新结余账户ID
	* @param  r_balAcctIds      结余账户容器
	* @param  r_accountBills    账户账单容器
	* @return 无
	*/ 
	//void updateBalanceAcctId(vector<T_SERVER_ID>  &r_balAcctIds, vector<AccountBill> &r_accountBills);
	//modify by luxy 2011-02-13
   void updateBalanceAcctId(vector<T_SERVER_ID>  &r_balAcctIds,const RollBackBill  & r_rollBackBill,const int &r_billType); 
  
 
  /**
	* getCurStateSet:       获取出帐用户当前状态集合
	* @param  r_attrValue   状态集合
	* @param  r_timeRangeFlag   时间范围限定标志，1  限定为上半月，2：限定为下半月  ，其他： 限定为全月 add by xuf 2010-1-14 22:58:10
	* @return 无
	*/ 
  void getCurStateSet(T_OBJECT_ATTR &r_attrValue,const int &r_timeRangeFlag=0);
 
  /**
	* getLastState:       获取出帐用户当月最后时刻的状态
	* @param  r_attrValue   最后状态
	* @return 无
	*/ 
  void getLastState(T_OBJECT_ATTR &r_attrValue); //zhaogc add
 
  /**
	* judgeStateEffect:     判断指定帐期内用户是否存在某些状态
	* @param  r_cycParam    当前帐期信息
	* @param  r_stateSet    状态集合
	* @param  durCycNum     偏移帐期数
	* @return true 存在 ,false 不存在
	*/ 
  bool judgeStateEffect(const CycParam &r_cycParam, const T_OBJECT_ATTR &r_stateSet, const long &durCycNum);
 
  /**
	* judgeServExists:      判断当前帐期内用户是否存在某种服务
	* @param  r_attrValue   服务编码
	* @param  r_timeRangeflag    时间范围限定标志，1  限定为上半月，2：限定为下半月 ，其他： 限定为全月 add by xuf 2010-1-14 22:58:10
	* @return true 存在 ,false 不存在
	*/   
  bool judgeServExists(const int &r_attrValue,const int &r_timeRangeflag=0);
   
  /**
	* setEffectTrack:       设置作用轨迹
	* @param  r_id    			三户标识
	* @param  r_idType    	三户类型
	* @param  r_subId     	成员标识
	* @param  r_detailItem  明细帐目
	* @param  r_effectFee   作用费用
	* @param  r_discntType  优惠类型
	* @param  r_effectId    作用标识
	* @param  r_orderNo     作用次序
	* @param  r_adjustInsId 调帐实例号
	* @return 无
	*/
  void  setEffectTrack(T_SERVER_ID   r_id,
											  ID_TYPE  			r_idType,          
											  T_SERVER_ID   r_subId,	    
												int           r_detailItem, 
												long          r_effectFee,
												char          r_discntType, 
												int           r_effectId,   
												int           r_orderNo,    
												long          r_adjustInsId=0
												); 
 /**
	* isMonthEnd: 判断是否出月末账.
  * @param  无.
	* @return true 月末，false 实时.
	*/ 											
	bool	isMonthEnd();	
	
	/**
	* ifNeedAccountDeal: 判断是否需要做帐务处理.
  * @param  无.
	* @return true 需要 ,false 不.
	*/ 	
	bool ifNeedAccountDeal(); 
	
	
	/**
	* getSvcStateTime: 获取用户状态到目前为止的时间生效情况.
  * @param  r_statecodeMap：有效服务状态的编码.
  * @param  curDateTime：指截止到哪一天为止.
	* @return int数字,表示本月的服务状态有效分布情况.
	*/ 	
	int getSvcStateTime(const stateMap& r_stateMap ,T_DATE_TIME curDateTime );
	
  // add by geyf

  //返回数字n对应的二进制数中1的个数

	int getCount(unsigned int n);

	/**
	* getDetailBillBak:   获取备份明细账单
	* @param  r_userId                用户标识        
	* @param  r_detailItemFeeVector   明细费用集合
	* @param  r_userAccountBillVector 用户账户账单集合            
	* @return： true 成功, false 失败
	*/ 
	bool getDetailBillBak(const T_SERVER_ID &r_userId,
                        vector<DetailItemFee>   &r_detailItemFeeVector,
                        vector<UserAccountBill> &r_userAccountBillVector);
 
 	/**
	* getUserDiscntOrderInAcct:      取用户在账户下的优惠优先级.
	* @param  r_acctId               账户标识.
	* @param  r_userOrderMap         用户优先级.
	* @return  优先级
	*/  
 int  getUserDiscntOrderInAcct(const T_ACCT_ID  &r_acctId,
	                             map<T_SERVER_ID,int> &r_userOrderMap);
	                            
	 //判断角色是否存在或相等	
	bool judgeRoleCodeIn(vector<T_MEMBER_TYPE > &r_roleCodeVec, map<T_MEMBER_TYPE ,T_MEMBER_TYPE > &r_roleCodeMap,T_MEMBER_TYPE &r_roleCode);
	
	
	 //根据参数ID取对应的值，找不到返回 -1
   const string getServParamValue(const int  &r_servId,const int &r_paramId) const;
                               				               
		/*add by xuf 2010-1-13 20:48:31
		* getVaildMonthDays: 
		* 取租费有效天数，山西联通特有
		* 开机，半停，停机收租费，其他如停保，销户不收租费
		* 停保之后半停停机，不收租费，
		* 一天内存在开机和其他状态，以开机为准
		* 一天内存在停保和半停停机状态，以开始时间在前的状态为准
		* @param  r_beginTime：指从此日开始.
		* @param  r_endTime：指截止到哪一天为止.
		* @return int数字,表示本月的主服务状态有效天数.
		*/ 
   int getVaildMonthDays(const T_DATE_TIME   &r_beginTime,const T_DATE_TIME   &r_endTime);      
                         				               
   //是否为账户下USERID最小的，且非往月停保用户 add by xuf 2010-1-30 10:48:31
   bool ifSpecialUserId(const  T_SERVER_ID &r_acctId);  
     //add by dingr for shanxin 按规则查找账户下userid最小固话用户
   bool ifSpecialFixUserId(T_SERVER_ID &r_userId);
   //add by dingr for shanxin 按规则查找账户下userid最小手机用户
  bool ifSpecialMobileUserId( T_SERVER_ID &r_userId); 
  
  //add by wanglu 2011-2-15 begin
  int getValidRecvDays(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime);
  void getUserPayInfo(T_SERVER_ID &r_acctId,const T_SERVER_ID &r_userId);
  //end 2011-2-15                          				               
  
  //add for monfee new S
  void getSecond(const String<14>& endDate, const String<14>& startDate,unsigned int& secondNum);
  
  void getDuration(const String<14>& endDate, const String<14>& startDate,int& dayNum);
  
  bool getUserMonfeeBill(const T_SERVER_ID &r_userId,const String<18> &r_spCode,
                         const String<18> &r_spBizCode,vector<UserMonfeeBill> &r_monFeebill,
                         vector<UserMonfeeBill> &l_monFeebill);
  
  bool updateMonfeeBill(vector<UserMonfeeBill> &l_monFeebill,vector<UserMonfeeBill> &r_monFeebill);
                        
  int getTariffBySubInfo(const char &dataTypeCode,
                         const UserSubInfo& userSubInfo,                                                                                  
                         vector<MonfeeTariff>& monfeeTariffVector);
  
  void getSubInfoByDiscnt(const MonfeeDiscount& monfeeDiscount,
                                 vector<UserSubInfo>& userAllSubInfoVector,
                                 vector<UserSubInfoIter>& userDiscountVector);
                                 
  void getUserNum(const T_SERVER_ID& ecUserId, const String<18>& spBizCode,int &usernum);
  
  bool judgeECAbnormalState(const T_SERVER_ID& ecUserId, const String<18>& spBizCode);
  
  bool judgeSilenceUser(const int& cycId, const T_MSISDN& serialNumber, const MonfeeTariff& monfeeTariff);
  
  /**
  * getSpPunishRule:获取sp处罚规则
  * @return true:成功 false:失败
  */
  bool getSpPunishRule(const String<18>& spCode,
                       const String<18>& spBizCode,
                       SpStateVector& spStateVector);   
  
  /*
  * getAllRelaDiscount:获取所有SP关联优惠信息
  * @param  spRelaDiscntMap   SP关联优惠信息集.
  * @return true:成功 false:失败
  */
  bool getAllRelaDiscount(SpRelaDiscntMap &spRelaDiscntMap);                                                                                               
  
  bool getInfoFromDiscountId(const char& dataType,                                
                             const IdDiscount& idDiscnt,
                             vector<MonfeeDiscount> & dVector);
  
  int getUserSpDiscnt(const char& dataType,
                       const IdInfo& idInfo,
                       const ID_TYPE &r_idType,
                       const String<18> &r_spCode,
                       const String<18> &r_spBizCode,                               
                       const vector<sysTariff>& sysDiscountVector,
                       vector<MonfeeDiscount>& discountVector);
                       
  //add for monfee new E                     				               
private:
	/**
	* addMonth:      月份+月数
	* @param  r_month         月份
	* @param  r_addMonth      月数
	* @return 月份
	*/
  long  addMonth(long r_month,const int &r_addMonth) const;
   
   int m_errCode;                            
                            
public:
	  //出帐用户
    T_USER_ID    m_userId;
       
    //总量优惠用户群ID
    T_SERVER_ID  m_userGrpId;
   
    //当前处理账户ID
    T_SERVER_ID   m_acctId;
        
	  //用户状态集合
    vector<UserServState>     *m_pUserServStateVector;

		//用户服务集合
		vector<UserServ> *m_pUserServVector;
		
    //是否存在用户资料
    bool m_ifExistUserInfo;
   
    //是否存在群资料
    bool m_ifExistGrpInfo;
   
    //是否存在群总量优惠
    bool m_isExistGrpTotalDiscnt;
   
    //是否存在账户优惠
    bool m_isExistAcctDiscnt;
      
    //明细账单：内部处理使用
    DetailBill          m_detailBill;
    //明细账单集合：内部使用
    //##ModelId=48DB06DD02D4
    vector<DetailBill>          m_detailBillVector;

    //优惠轨迹
    vector<DiscntBill>            *m_pDiscntBillVector;
   
    //附加账单：异常，集合，赠送等账单
		vector <AdditionalBill>      *m_pAddBillVector;
   
    //存放用户/群优惠需要参考的明细账单
    DetailBill m_otherDetailBill;
   
    bool m_acctDiscntForAllUsers;  
    bool m_grpDiscntForAllUsers;
	int  m_commonDealFlag;
   
    //用户群用户账单
    vector <UserBill> m_userGrpBills_new;
public:     
    //用户集合(出帐用户和参与总量优惠用户)   
    set<T_USER_ID>   m_userIdSet;
       
    //用户群集合
    set<T_USER_ID>   m_userGroupIdSet;
   
    //所有用户群-成员Map
    map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> > m_groupUserIdMap;
       
    //当前处理用户的所有账户
    vector<T_ACCT_ID>   m_acctIdVector;
   
    //当前用户的付费关系
    vector<PayRelation>   m_payRelationVector;
   
    //用户付费关系MAP
    map<T_SERVER_ID, vector<PayRelation> > m_userPaylationMap;
    
    //所有出帐账户集合
    set<T_ACCT_ID>  m_acctIdSet;  
   
    //所有参与账户级优惠的账户ID add by xuf 2009-8-17 16:53:17
    set<T_ACCT_ID> m_acctIds;
    
public:
	  //话单帐期参数信息
    //##ModelId=48D70E0C03CF
    CycParam m_cycParam;
   
    //最小实时帐期(usetag=0 的最小帐期)
    //##ModelId=48D7662C032E
    int m_minRealCycId;
   
    // 帐期
    //##ModelId=48D888B0037B
    int m_cycleId;
   
     // 省份代码
    //##ModelId=48D766570054
    string m_provinceCode;
   
    //通道号
    //##ModelId=48DB3F4A0192
    int m_channelNo;
   
    //账单管理指针
    //##ModelId=48DB3F5D0239
    BillManager *m_pBillMgr;
   
    //资料管理对象指针
    //##ModelId=48DB3FDD032E
    PM_Account *m_pAccountParam;
   
    //对应明细帐目是否存在标识        
    unsigned char     m_existFlag[MAX_ITEM];
   
    //明细帐目数组
    DetailItemInfo        m_detailItem[MAX_ITEM];
   
    //对应明细帐目是否存在标识        
    unsigned char     m_carryModes[MAX_ITEM];   
      
   //统一付费类型 0:不存在统一付费 1:集团归属本地 2:集团归属外地
    int      m_unionPayType ;   
    //统一付费号码   
    T_MSISDN      m_unipayPhone;
   
    //是否出月末账标识
    bool  m_isMonthEnd;
   
    //话单指针
		PCDR  m_pCdr;
		
		//错误信息
		PString *m_pErrMsg;
		
public:   	
   	int  m_insNo;
   	
   	bool m_needAcctDealTag;
   	
   	//是否实时输出优惠轨迹
		char	m_outAllRealDiscountTag;
		
		//是否输出帐务优惠
		bool ifNeedOutDiscount();
		  	
		//以时间为种子获取序列号
		void getSeqNo(long &r_seqNo);
		
		int m_divisor;
   	
public:
			
  /**
	* ifNotProvGroup:     是否不是全省集团账户
	* @param  r_acctId    账户标识
	* @return  true :不是全省集团账户，false ：是全省集团账户
	*/
		bool ifNotProvGroup(const T_SERVER_ID &r_acctId);
	
		//资费构成，帐期调帐，特殊优惠(用户\账户)
   	vector<Discount>     m_discountVector;
		//帐前调帐信息(用户\账户)
		vector<AdjustBefore>  m_adjustBeforeVector;
      
   	//资费构成，帐期调帐，特殊优惠(群总量)
    vector<Discount>     m_userGrpDiscountVector;

		vector<Discount>	 m_acctDiscountVector;

		//存放事件类型为27的资费add by xuzy 2011.2.7
		vector<Discount>	 m_userAfterAcctDiscntVec;		
		
		map<T_ACCT_ID,vector<Discount> >	 m_acctsDiscountMap;

		//用户网别
	  String<2> m_netTypeCode;
	 
	  //虚拟用户资费
	  vector<UserTp> m_userTpVector;
	 
	  //账户资费
	  map<T_ACCT_ID,vector<UserTp> > m_acctTpMap;
	 
	/**
	* updateBill:               更新账单
	* @param  r_billList        账单容器
	* @param  r_bill            单条账单
	* @return 无
	*/                       
	template<class T> void updateBill(vector<T> &r_billList,const T &r_bill)
  {
      vector<T>::iterator t_itr = ::lower_bound(r_billList.begin(),r_billList.end(),r_bill);
      if(t_itr == r_billList.end() || (!(*t_itr==r_bill)))
      {
        r_billList.insert(t_itr,r_bill);
      }else
      {
        *t_itr = r_bill;
      }
  }

  public:
  	Discount m_discntInfo;
	bool m_userNeedAcctDealTag;
		
		//add by xuf 2009-11-24 17:51:19
		//虚拟用户列表
   map <T_SERVER_ID,T_VFILE> m_vUserFileMap;
   
   //是否过滤资费 add by xuf 2010-1-7 23:20:50
   bool m_needGetPartTp;
   
};



#endif /* COMBILL_H_HEADER_INCLUDED_B728A5B2 */
