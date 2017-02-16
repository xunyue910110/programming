#ifndef _PM_BILLING_H_INCLUDE_20081008_
#define _PM_BILLING_H_INCLUDE_20081008_
#include "dbparam/ParamManager.h"
#include "dbparam/ParamContainer.h"
#include "BillingCommon.h"
#include "FieldsXX.h"
#include "MdbInfo.h"
#include "IdInfo.h"
//#include "TpInfo.h"
//#include "PO_CycleTime.h"
//#include "PO_Discount.h"
//#include "Mdb_UserServState.h"

//class UserServState;
class PO_CycleEparchy;
class PO_BillingCycle;
class PO_SysParam;
class PO_CycleFeeRule;
class PO_SubCycle;
class PO_Discount;
class PO_EventType;
class PO_UserChannel;
class PO_ServStatePriority;

class MdbParamManager;

//#include "Mdb_VpnInfo_UserId.h"
//#include "Mdb_VpnInfo_Number.h"

class PM_Billing:public ParamManager
{
  public:
    virtual bool addParamObject();
    PM_Billing();
    ~PM_Billing();
    //针对oracle的
    virtual bool getData();
    bool   afterGetData(); //取参数后,初始化一些内部成员    
    bool needUpdate();
    bool update();
    
    //同时针对oracle和mdb的
    bool initialization();
    bool 	 end();
    
    //获取内存数据库出错信息
    void getMdbErrMsg(int& r_errno,string& r_errMsg);
    
  protected:
//    ParamContainer<PO_CycleTime>          *m_PO_CycleTimeContainer;
    ParamContainer<PO_CycleEparchy>          *m_PO_CycleEparchyContainer;	//limeng add 20090814
	ParamContainer<PO_BillingCycle>       *m_PO_BillingCycleContainer;
	ParamContainer<PO_SysParam>           *m_PO_SysParamContainer;
	ParamContainer<PO_CycleFeeRule>       *m_PO_CycleFeeRuleContainer;
	ParamContainer<PO_SubCycle>           *m_PO_SubCycleContainer;
	ParamContainer<PO_Discount>           *m_PO_DiscountContainer;
	ParamContainer<PO_EventType>          *m_PO_EventTypeContainer;
	ParamContainer<PO_UserChannel>        *m_PO_UserChannelContainer;
  	ParamContainer<PO_ServStatePriority>  *m_PO_ServStatePriorityContainer;
    
    //const CondRuleMgr * m_pCondRuleMgr;
  
  public:
    //设置Mdb接口信息
    bool setMdbInfo(const MdbInfo &r_mdbInfo);
    
    /**
     * getUserInfo:根据用户(群)标识获取用户(群)信息
     * @param  userId         用户(群)标识,也可以是客户标识.
     * @param  userInfo          用户(群)信息.
     * @return   -1表示Mdb异常，0表示没有查找出结果，1 表示查找出结果
     */
    int getUserInfo(const T_SERVER_ID &r_userId,
                         IdInfo      &r_userInfo);
	
    /**
     * getBillCycle       根据日期取对应的帐期.
     * @param  r_Billday : 日期YYYYMMDD
     * @param  r_billCycle:帐期YYYYMM
     * @return true: 是  false:否
     */	
	bool getBillCycle(const int &r_Billday,int &r_billCycle) const;

	bool getCycleTime(const int &r_cycle,CycParam &r_cycleParam) const;

	bool getCycleTime(const int &r_cycle,  
                         T_DATE_TIME &r_beginTime,
                         T_DATE_TIME &r_halfTime,
                         T_DATE_TIME &r_endTime) const;
                         
    /*
	 * 根据进程名和通道号取系统参数
	 * @param r_sysParamCode     进程名
	 * @param r_channelNo        通道号
	 * @param r_sysParam         参数信息
	 * @return true 找到, false 未找到
	 */           
	bool getSysParam(const T_SYSPARAMCODE &r_sysParamCode,
                          const int            &r_channelNo,
                                ProcSysParam   &r_sysParam);
    
    /*
	 * 根据系统参数取系统参数信息
	 * @param r_sysParamCode     系统参数
	 * @param r_sysParamInfoVector         系统参数信息
	 * @return true 找到, false 未找到
	 */
    bool getSysParam (const T_SYSPARAMCODE &r_sysParamCode,
                     vector<ProcSysParam> &r_sysParamInfoVector);

  	/**
	 * getChannelNoByUserId:根据用户，地州获取所在通道（根据TD_USER_CHANNEL表）
	 * @param  r_userId              用户标示
	 * @param  r_eparchyCode            地州标示.
	 * @param  r_channelNo            通道.
	 * @return  false 表示 不成功 ，true 表示 成功
	 */
	bool getChannelNoByUserId(const T_USER_ID &r_userId,
	                                  const T_EPARCHY_CODE &r_eparchyCode,
	                                  T_CHANNEL  &r_channelNo);    
    /**
	 * getcycPara: 根据传入时间获取账期参数
	 * @param  r_scomputeDate    传入时间(YYYYMMDDHHMMSS). 
	 * @param  r_cycPara    账期参数. 
	 * @return  true 表示查找正常，false 表示查找错误
	 */
	bool getcycPara(const T_DATE_TIME &r_scomputeDate, CycParam &r_cycPara);
                     
    /*
     * 根据用户ID，取对应的服务信息列表
     * @param r_userId     帐户Id
     * @param r_userServStates       服务列表
     * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
     */           
    int getUserServState(const T_SERVER_ID& r_userId, vector<UserServState>& r_userServStates);
    
    //add by xuf 2009-8-19 10:46:45
        /**
	* getMinMaxSubIdFromItem:根据事件，账期获取最小最大子账期标示
	* @param  r_eventTypeId           事件编码
	* @param  r_cycId                 账期
	* @param  r_minSubId              最小子账期
	* @param  r_maxSubId              最大子账期
	* @return false 表示 不成功 ，true 表示 成功
	*/
	bool getMinMaxSubIdFromItem(const T_BIZ_TYPE& r_eventTypeId,const int& r_cycId,
                                int& r_minSubId,int& r_maxSubId);

   /**
	* getUserServ:获取用户所有服务
	* @param  r_userId              用户标示
	* @param  r_userServVector            用户所有服务.
	* @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
	*/
    int getUserServ(const T_USER_ID &r_userId,vector<UserServ> &r_userServVector);    

    /**
	* getSubCycIdFromDay:从某个日期、帐目，获取该帐目项子帐期
	* @param  r_date                 日期YYYYMMDD
	* @param  r_itemCode              账目
	* @return 子账期标识, 0 表示 没获取
	*/
	int getSubCycIdFromDay(const int r_date,const int r_itemCode);
	//add for rent new version begin
  /**
	* getSubCycTypeByEventId:根据事件获取子帐期类型
	* @param  r_EventTypeId           事件类型
	* @param  t_subCycType            子账期类型
	* @return false 表示 不成功 ，true 表示 成功
	*/
  bool getSubCycTypeByEventId(const T_BIZ_TYPE r_EventTypeId,T_SUBCYC_TYPE & t_subCycType);  
  
  /**
	* getSubCycBySubId:获取当前子帐期开始时间，结束时间
	* @param  r_subCycType           子账期类型
	* @param  r_subCycId             当前子账期
	* @param  r_cycId                当前账期
	* @param  t_startDateTime        子帐期开始时间	
	* @param  t_endDateTime          子帐期结束时间	
	* @return false 表示 不成功 ，true 表示 成功
	*/
  bool getSubCycBySubId(const char r_subCycType,const int r_subCycId,
                        const int r_cycId,T_DATE_TIME& t_startDateTime,
                        T_DATE_TIME& t_endDateTime);
                        
	//add for rent new version end
	/**
	* getDiscountInfo:根据资费，获取该资费具体信息
	* @param  r_discountId                 资费标识
	* @param  r_discountInfo              资费信息
	* @return false 表示 不成功 ，true 表示 成功
	*/
	//Modified By Sunam 2009/9/27 12:38:17
	//bool getDiscountInfo(const T_TP_ID &r_discountId,DiscountInfo &r_discountInfo);
	bool getDiscountInfo(const T_TP_ID &r_discountId,vector<DiscountInfo> &r_discountInfo);
		/**
	* getDiscountInfo:根据资费，获取该资费具体信息(加时间判断)
	* @param  r_discountId                资费标识
	* @param  r_beginTime                 开始时间
	* @param  r_discountInfo              资费信息
	* @return -1 无对应的参数配置,0 参数失效 ，1 成功
	*/
	//Modified By Sunam 2009/9/27 12:37:48
	/*
	int  getDiscountInfo(const T_TP_ID     &r_discountId,
	                     const T_DATE_TIME &r_beginTime,
	                         DiscountInfo  &r_discountInfo);	
    */
    int  getDiscountInfo(const T_TP_ID     &r_discountId,
                         const T_DATE_TIME &r_beginTime,
                         vector<DiscountInfo>  &r_discountInfo);
    /**
     * getUserImportInfo:根据用户(群)标识获取用户(群)异动信息
     * @param  userId         用户(群)标识,也可以是客户标识.
     * @param  userInfo          用户(群)信息.
     * @param  r_cycPram          帐期信息.
     * @return   -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
     */
     
    int getUserImportInfo(const T_SERVER_ID &r_userId,
                    IdInfo      	&r_userInfo,
                    const CycParam &r_cycPram);
	
	/**
	 * getUserInfo:根据用户(群)标识获取用户(群)信息
	 * @param  userId         用户(群)标识,也可以是客户标识.
	 * @param  userInfo          用户(群)信息.
	 * @param  r_cycPram          帐期信息.
	 * @return   表示Mdb异常，1 表示查找出结果
	 */
	int getUserInfo(const T_SERVER_ID &r_userId,
                			IdInfo      &r_userInfo,
                			const CycParam &r_cycPram);
	
	  /**
       * getMinRealCycId:取最小实时帐期信息
       * @param  minRealCycleId  最小实时帐期      .
       * @return  false 表示异常，true 表示正常
      */
      bool getMinRealCycId(int &minRealCycleId);
      
      /**
       * getAllUserReAccount:取用户主表所有用户信息
       * @param  reAccountEventVector  所有用户信息
       * @return  -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
      */
      int getAllUserReAccount(vector<ReAccountEventInfo>& reAccountEventVector);
      
    /**
	 * getServStatePrior:获取服务状态所在服务的优先级
	 * @param  r_iServ              服务
	 * @param  r_servState            服务状态.
	 * @param  r_prior            优先级.
	 * @return  false 表示 不成功 ，true 表示 成功
	 */	  	
  	bool getServStatePrior(const int r_iServ, const char r_servState, int& r_prior);
    
    /**
	 * getUserServState:获取用户在相应时间内的相应服务的服务状态
	 * @param  r_userId              用户标示
	 * @param  r_subCycType              子账期类型
	 * @param  r_cycId              账期标示
	 * @param  r_subCycId              子账期标示
	 * @param  r_iServ              用户服务
	 * @param  r_svcStateCode            用户服务状态.
	 * @return true 表示 正常，false 表示 不正常
	 */
	bool getUserServState(const T_USER_ID& r_userId, 
                          const char r_subCycType, const int r_cycId, 
                          const int r_subCycId,  const int r_iSvc,
                          char& r_servStateCode,int& r_oServ);

	/**
	 * getEventFromItem:根据账目获取事件类型，服务，子账期类型信息
	 * @param  r_itemCode                    账目
	 * @param  r_eventAndSubCycType          EventServAndSubCycType结构
	 * @return true 表示 正常，false 表示 不正常
	 */
	bool getEventFromItem(const int r_itemCode, 
	                      EventServAndSubCycType& r_eventAndSubCycType);
	 
		/** add by zhouq 
   * getEventFromItem:根据账目获取事件类型，服务，子账期类型信息
   * @param  r_itemCode                    账目
   * @param  vector<r_eventAndSubCycType>    EventServAndSubCycType结构
   * @return true 表示 正常，false 表示 不正常
   */
  bool getEventFromItem(const int r_itemCode,
	                      vector<EventServAndSubCycType>& v_eventAndSubCycType);                      
	/**
	 * Added By Sunam 2009/8/8 13:48:12
	 * getFeePolicyRemidInfo:根据资费编码及累计量编码查找免费资源提醒信息
	 * @param  r_feePolicyId                        资费编码
	 * @param  r_addupId                            累计量编码
	 * @param  r_result                             查找结果
	 * @return true 表示查找成功，false 表示未查找到指定记录
	 */
	bool getFeePolicyRemidInfo(const T_TP_ID &r_feePolicyId, const int &r_addupId, vector<FeePolicyRemidInfo> &r_result);
	
	/**
	 * limeng added 20090812
	 * getUserChannel:根据库号查找批价通道信息
	 * @param  r_dbNo                        库号
	 * @param  r_result                      查找结果
	 * @return true 表示查找成功，false 表示未查找到指定记录
	 */
	bool getUserChannel(const int &r_dbNo,vector<UserChannel> &r_result);
	bool getHolidayType(const char *r_beginDate, const T_DATE_TIME &r_beginDateTime, vector<T_OBJECT_ATTR> &r_holidayType);
	
		/**
	* getDiscountInfo:根据资费，获取该资费具体信息
	* @param  r_discountId                 资费标识
	* @param  r_discountInfo              资费信息
	* @return false 表示 不成功 ，true 表示 成功
	*/
	bool getDiscountInfo(const T_TP_ID &r_discountId,DiscountInfo &r_discountInfo);

	/**
	* getDiscountInfo:根据资费，获取该资费具体信息(加时间判断)
	* @param  r_discountId                资费标识
	* @param  r_beginTime                 开始时间
	* @param  r_discountInfo              资费信息
	* @return -1 无对应的参数配置,0 参数失效 ，1 成功
	*/
	int  getDiscountInfo(const T_TP_ID     &r_discountId,
	                     const T_DATE_TIME &r_beginTime,
	                         DiscountInfo  &r_discountInfo);
	
	
	
/**
* getMinMaxSubIdFromItem:根据账目，账期获取最小最大子账期标示
* @param  r_itemCode              账目
* @param  r_cycId                 账期
* @param  r_minSubId              最小子账期
* @param  r_maxSubId              最大子账期
* @return false 表示 不成功 ，true 表示 成功
*/
bool getMinMaxSubIdFromItem(const int& r_itemCode,const int& r_cycId,
                                        int& r_minSubId,int& r_maxSubId);
	protected:
		/**
		 * getSubCycInfo:根据账期，子账期获取子账期
		 * @param  r_cycId              账期标识
		 * @param  r_subCycId            子账期标识
		 * @param  r_subCycVector            子账期参数集
		 * @return  0 表示查找结果为空 ，>0 表示结果记录数
		 */
		int getSubCycInfo(const int &r_cycId,const char &r_subCycId,
	    	               vector<SubCycPara> &r_subCycVector);
    
    private:
    	
    	void getSubCycStartDay(CycParam& r_cyclePara,int** r_subCycStartDay);
    	void getSubCycEndDay(CycParam& r_cyclePara,int** r_subCycEndDay);
    	void getcycParaArray(CycParam r_cycPara[]);
    	
    	int getNextMonth(int d_yyyymm);
    	bool validDate(int d_YYYYMMDD);
    	bool isLeapYear(long y_YYYY);
    	int getMonthDays(const int d_YYYYMMDD);
    	int getweek(const int date);
    	void getDealDate();
    	bool initArray();
    	bool initItemEventSubCyc();
    	int addCycle(int r_cycleId,int num);
  		bool initstatePriority();
  		
  public:
	map<int,int**> m_subCycStartDayMap;
  	map<int,int**> m_subCycEndDayMap;
    	//map<int,EventServAndSubCycType> m_itemToEventMap;
  	multimap<int,EventServAndSubCycType> m_itemToEventMap; //add by xuf 2009-8-19 10:54:01
  	map<int,map<char,int> > m_statePriority;
  	int m_callProcMode;  //固定费进程是否调用updateBill的Proc以及帐务优惠的Proc 标志
  	//add for new version begin
    vector<T_SUBCYC_TYPE> m_subCycTypeVec; //add by xuf 2009-8-19 10:54:39
    //add for new version end
  protected:
    //内存数据库管理接口类
	MdbParamManager   *m_pMdbManager;

  private:
  	bool          m_needUpdated;
  	MdbInfo       m_mdbInfo;    	
  	T_DATE_TIME   m_dealDate;
  	
  public://add by gaojf 2009-2-13 18:45
    void setDbNo(const int &r_dbNo){m_dbNo = r_dbNo;}
     //limeng add 20090814
    inline void setEparchyCode(const string &r_eparchyCode){m_eparchyCode = r_eparchyCode;}
 private:
    int           m_dbNo; 
    T_EPARCHY_CODE	m_eparchyCode;	//limeng add 20090814 
    
  public:
  	 	//取用户属性
	 	int   getUserParam(const T_SERVER_ID     &r_userId,
                           vector<UserParam>     &r_userParamList);  	 
                           
  	 	//取服务属性
	 	int   getUserServParam(const long     &servInsId,
                           vector<UserServParam>     &r_userServParamList);
  		/*
     	* 根据帐户ID，取对应的帐户信息 
     	* @param r_acctId     帐户Id
     	* @param r_acctInfo   帐户信息
     	* @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
     	*/           
    	int getAcctInfo(const T_SERVER_ID &r_acctId,
                        IdInfo      &r_acctInfo);			     
						   	                           
};

#endif //_PM_BILLING_H_INCLUDE_20081008_



