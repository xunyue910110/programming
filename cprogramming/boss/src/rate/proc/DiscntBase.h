#ifndef DISCNTBASE_H_HEADER_INCLUDED_B72F56EC
#define DISCNTBASE_H_HEADER_INCLUDED_B72F56EC
#include "PMManager.h"
#include "RateGlobalInfo.h"
#include "AdjustBeforeCalc.h"
#include "FeePolicyComp.h" 
#include "EventFeePolicy.h"
#include "AcctBillMgr.h"
          
// 功能：帐务优惠基类，帐务级优惠均由此派生：处理单个用户，账户，虚拟用户的优惠
// 日期：2008年8月
//##ModelId=48B21A0E0104
class DiscntBase
{
  public:
    //##ModelId=48B21A0F00A2
    DiscntBase();

    //##ModelId=48B21A0F00A3
    ~DiscntBase();

    // 初始化：文件级->三户级（用户，账户，用户群组）
    //##ModelId=48B21A0F00A4
   virtual bool initialize(const T_USER_ID &r_id,
                            ID_TYPE r_idTypeCode,
                            RateGlobalInfo *r_pRateInfo,
                            PM_Account *r_pAccountParam ,
                            ComBill *r_pComBill);
    
   virtual bool getDiscount(const vector<Discount>     &r_discountVector);
           
   virtual bool soluteDiscount( ); 
    
//   virtual bool getPolicyCompByTp(){return true;}
   
   //取群上帐务资费
//   virtual bool getAcctGrpTp() {return true;}
   
//   virtual bool getAcctTpInfo() {return true;}
   /**getEffDiscntByAdjustBefore:根据帐前调整信息获取有效的优惠信息
   *@param  r_adjustBefore    帐前调整信息
   *@param  r_discnt         优惠信息
   *@return true:获取到有效的优惠信息  false:未获取到
   */
    bool getEffDiscntByAdjustBefore(const AdjustBefore& r_adjustBefore,Discount& r_discnt);
    bool isFeePolicyValid(const T_DATE_TIME &r_beginTime,
  	                       const T_DATE_TIME &r_endTime,
  	                       const CycParam  &r_cycParam,
						   const T_BIZ_TYPE &r_eventType=BIZ_TYPE_USERDIS); 
						   
	 void getRateTpInfo(vector<UserTp> &t_userTpVector,const ID_TYPE &r_idType);					   
    
  public:
       
    
   /**getEffectDiscntByTariffPolicy:根据资费政策信息获取有效的优惠信息
   *@param  r_id               三户标识
   *@param  r_roleCode         角色代码
   *@param  r_tpObjId          资费实例标识
   *@param  r_beginTime        起始时间
   *@param  r_endTime          结束时间
   *@param  r_priority         优先级
   *@param  r_pRateTpInfo      资费信息
   *@param  r_discountInfo     td_b_discnt信息
   *@return true:获取到有效的优惠信息  false:未获取到
   */
    bool getEffectDiscntByTariffPolicy(const T_SERVER_ID  &r_id,
                                		 const T_MEMBER_TYPE &r_roleCode,
                                		 const long  &r_tpObjId, 
                               			 const T_DATE_TIME &r_beginTime, 
                              			 const T_DATE_TIME &r_endTime, 
                               			 const int  &r_priority,
                               			 RateTpInfo  r_pRateTpInfo,
                               			 DiscountInfo *r_discountInfo=NULL
                               			 );
                               			

   void setTpParamInfo(vector<UserTp> &r_userTpVector);
  protected:
     //##ModelId=48B21A0F011C
    virtual void specialDeal();
     
    // 最小实时帐期
    //##ModelId=48B21A0F0029
    int m_minRealCycId;

    // 本次处理的帐期信息
    //##ModelId=48B21A0F002A
    CycParam m_cycParam;

    //##ModelId=48B21A0F0034
    IdInfo m_idInfo;//三户信息
    
    T_SERVER_ID m_id; //三户标识
    
    ID_TYPE m_idTypeCode;//0:用户标识 1:帐户标识 2:用户群标识 3:帐户群标识  
     
    // 资料内存管理对象
    //##ModelId=48B21A0F005B
    PM_Account   *m_pAccountParam;

    ComBill     *m_pComBill;
    
    RateGlobalInfo *m_pRateInfo;
    
    //帐前调帐信息(用户\账户)
   	vector<AdjustBefore>  m_adjustBeforeVector;
   	
    AdjustBeforeCalc  m_adjustBeforeCalc;
    
//    vector<UserTp> m_userTpVector;
        
    vector<UserFeeSwap> m_userFeeSwaps;
    
 // 	vector<RateTpInfo>   m_tpInfoList;    //批价政策（批价实现政策信息）
 // 	vector<int>          m_pAcctTpList;   //帐务阶段批价政策

//public:
    //资费构成，帐期调帐，特殊优惠(用户\账户)
    vector<Discount>     m_discountVector; 
	
	AcctBillMgr m_acctBillMgr;
    
};



#endif /* DISCNTBASE_H_HEADER_INCLUDED_B72F56EC */
