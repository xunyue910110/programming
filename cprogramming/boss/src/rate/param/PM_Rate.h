#ifndef _PM_RATE_H_INCLUDE_20080702_
#define _PM_RATE_H_INCLUDE_20080702_
#include "dbparam/ParamManager.h"
#include "FieldsXX.h"
#include "RateCdr.h"
#include "TpInfo.h"
#include "PM_Billing.h"
#include "BillingCommon.h"
#include "CompositItem.h"  //DetailItem
#include "FactorParamMgr.h"
#include "AccountCommon.h"
#include "Mdb_UserOtherInfo.h"
#include "policy/CondRule.h"
#include "PO_PartyPrice.h"//added by hany for 3g at 2009-5-16 01:31上午
//Added By Sunam 2009/8/14 10:28:52
#include "ObjectParam.h"
#include "ObjectValue.h"
#include "PO_FeePolicyAttr.h" //add by gaojf 2009-5-7 10:00
#include "PO_VpnRelFeePolicy.h"
#include "ObjectUserParam.h" //add by xuf 2009-10-2 13:23:38
#include "PO_SectionRelFix.h"//add by fug 2009-10-10 15:14
#include "PO_DiscntStatecode.h" 

//Added By Sunam 2009/8/8 13:46:46
class PO_FeePolicyRemidInfo;
class PO_VpnRelFeePolicy;
//Added By Sunam 2009/8/11 13:31:42
class PO_Holiday;
class Holiday;

class CompositItemMgr;

class PO_RedoUserInfo;
class PO_InterRoamInFeeBaseTp;
class PO_BaseTp;
class PO_BaseCellTp;
class PO_CellDef;
class FeebaseTP;
class PO_FeebaseTP;
class PO_SpecInfo_org;
class DetailBillItem;
class PO_DetailBillItem;
class PO_AdjustReason;
class PO_FeePolicy_Rent;

class PO_DetailItem;
class PO_GprsTimeDiscount;
class PO_GprsBaseTp;

//资费参数类
class ObjectBase;
class ObjectBill;
class CondBase;
class FeePolicyComp;
class EventFeePolicy;
class Price;
class PriceComp;
class PriceExtend;
class PriceParamImpl;
class EffectDiscnt;
class CondRuleRela;

class PO_FeePolicyComp;
class PO_EventFeePolicy;
class PO_Price;
class PO_PriceComp;
class PO_PriceExtend;
class PO_PriceParamImpl;
class PO_CondRuleRela;
class PO_CondRuleScope;
class PO_CondRuleScopeL;
class PO_CondRuleScopeR;
class PO_CondRuleScopeLR;
class PO_CondRuleIn;
class PO_CondRuleExists;
class PO_Condition;
class PO_EffectDiscnt;
class PO_EffectAddup;
class PO_EffectFeeCount;
class PO_ObjectBill;
class PO_ObjectCdr;  
class PO_ObjectAddup; 
class PO_ObjectUser; 
class PO_Object;   
class PO_Item;   
class PO_CompositItem;
class PO_AddupItem;
class PO_AddupCycleRule;
class PO_DefaultFeePolicy;
class PO_AttrFactors;
class PO_ElemEvent;
class PO_ElemInfo;
class PO_PartyPrice;//added by hany for 3G at 2009-5-16 01:32上午
class PO_FeepolicyMutexRela;//add by cuiyw at 2009-12-13 20:55:00
class PO_RateEventExclusiveRela;

//包月费参数
class PO_SpDiscount;        
class PO_SpTariff;          
//class PO_AdcECuserNum;      
//class PO_AdcECAbnormalState;
//class PO_SpCdrInfo;         
class PO_SpPunish;          
class PO_SpRelaDiscntDetail;
#include "PO_UserTpParam_def.h"
#include "PO_SpecialInfo_comp.h"
#include "Mdb_VpnInfo_UserId.h"
#include "Mdb_VpnInfo_Number.h"
#include "IdInfo.h"
#include "RateConstDef.h"
#include "Mdb_AdjustBefore.h"
#include "Mdb_PayRelation.h"
#include "Mdb_AcctBakBill.h"
#include "PO_DetailItem.h"
#include "Mdb_UserFeeSwap.h"


class PM_Rate:public PM_Billing
{
  public:
  	PM_Rate();
    virtual bool addParamObject();
    virtual ~PM_Rate();
    virtual bool getData();
    bool   afterGetData(); //取参数后,初始化一些内部成员
    bool 	 end();
    
    
  public:
  	
  	/**
     * getAttrFactors     根据业务类型和因素类型取对应扩展参数.
     * @param  r_fType : 1 基本政策因素，2 明细帐单因素
     * @param  r_bizType:业务类型
     * @param  r_factorList:因素列表
     * @return true: 是  false:否
     */
    bool getAttrFactors(const int &r_fType,const T_BIZ_TYPE &r_bizType,vector<PO_AttrFactors*> &r_factorList);
    /**
     * getBillCycle       根据日期取对应的帐期.
     * @param  r_Billday : 日期YYYYMMDD
     * @param  r_billCycle:帐期YYYYMM
     * @return true: 是  false:否
     */
    //已放入PM_Billing中bool getBillCycle(const int &r_Billday,int &r_billCycle) const;
    //根据帐期取对应的开始时间和结束时间
//    bool getCycleTime(const int &r_cycle,T_DATE_TIME &r_beginTime,
//                                         T_DATE_TIME &r_halfTime,
//                                         T_DATE_TIME &r_endTime) const;
    //bool getCycleTime(const int &r_cycle,CycParam &r_cycleParam) const;
                                         
    
    /**
     * isRedoUser       根据用户标识判断是否回退用户.
     * @param  r_userId : 用户标识
     * @return true: 是  false:否
     */
    bool isRedoUser(const T_SERVER_ID &r_userId) const ;
    
    /*
     * 基本政策求取
     * @param r_feeField    费用域
     * @param r_bizType     业务类型
     * @param r_baseTpFacts 因素列表（ID）
     * @param r_pcdr        话单
     * @param r_startTime   通话时间
     * @param r_tp_id     政策
     * @return true 找到, false 未找到
     */           
	bool getFeeBaseTp(const int         &r_feeField,
                      const T_BIZ_TYPE  &r_bizType,
                      const vector<int> &r_baseTpFacts,
                      const PCDR        &r_pcdr,
                      const T_DATE_TIME &r_startTime,
                            T_TP_ID     &r_tpId);
	
    /*
     * 国际来访业务基本政策求取
     * @param r_feeField  费用域
     * @param r_beginTime 通话时间
     * @param r_aspGroup  运行商分组
     * @param r_bizType   业务类型
     * @param r_sameHomeAb 计费方和对方是否统一归属
     * @param r_homeCodeB  对方归属
     * @param r_callType   呼叫类型
     * @param r_bAsp       对方运行商
     * @param r_bServType  对方业务类型
     * @param r_carrierType 承载类型
     * @param r_longType1   长途类型1
     * @param r_longGroup1  长途分组1
     * @param r_longType2   长途类型2
     * @param r_longGroup2  长途分组2
     * @param r_tp_id     政策
     * @return true 找到, false 未找到
     */           
  	bool getInterRoamInBaseTp(const T_FEE_FIELD  r_feeField,
                          		const T_DATE_TIME &r_beginTime,
                          		const T_ATTRI_VAL &r_aspGroup,
                          		const T_ATTRI_VAL &r_bizType,
                          		const char        &r_sameHomeAb,
                          		const T_ATTRI_VAL &r_homeCodeB,
                          		const T_ATTRI_VAL &r_callType,
                          		const T_ATTRI_VAL &r_bAsp,
                          		const T_ATTRI_VAL &r_bServType,
                          		const T_ATTRI_VAL &r_carrierType,
                          		const T_ATTRI_VAL &r_longType1,
                          		const T_ATTRI_VAL &r_longGroup1,
                          		const T_ATTRI_VAL &r_longType2,
                          		const T_ATTRI_VAL &r_longGroup2,
                                    T_TP_ID     &r_tp_id) const;
                                    
    /*
     * 获取系统级基本优惠政策
     * @param r_bizFlag        是否需要取计费事件的政策
     * @param r_bizType        事件类型
     * @param r_beginTime      通话日期时间
     * @param r_homeAreaCodeA  计费方归属地
     * @param r_callType       呼叫类型
     * @param r_roamType       漫游类型
     * @param r_servType       对方业务类型
     * @param r_carrierType    承载类型
     * @param r_baseTp         政策
     * @return true 找到, false 未找到
     * 注：帐务事件均需取
     */           
    bool getBaseTp(const bool        &r_bizFlag,
                   const T_BIZ_TYPE  &r_bizType,
                   const T_DATE_TIME &r_beginTime,
      						 const T_ATTRI_VAL &r_homeAreaCodeA, 
      						 const T_ATTRI_VAL &r_callType, 
      						 const T_ATTRI_VAL &r_roamType, 
      						 const T_ATTRI_VAL &r_servType, 
      						 const T_ATTRI_VAL &r_carrierType,
      						 const T_DATE_TIME &r_monthEndTime, 
      						 const PCDR        &r_pcdr,
      						    vector<T_TP_ID>&r_baseTp) const;
    
    /*
     * 获取系统级基站通优惠政策
     * @param r_beginTime      通话日期时间
     * @param r_callType       呼叫类型
     * @param r_servType       对方业务类型
     * @param r_roamType       漫游类型
     * @param r_homeAreaCodeA  计费方归属地
     * @param r_msc            交换机
     * @param r_lac            LAC
     * @param r_cellId         CELLID
     * @param r_baseTp         政策
     * @return true 找到, false 未找到
     */           
    bool getBaseCellTp(const T_DATE_TIME &r_beginTime,
                       const T_ATTRI_VAL &r_callType,
                       const T_ATTRI_VAL &r_servType,
                       const T_ATTRI_VAL &r_roamType,
                       const T_ATTRI_VAL &r_homeAreaCodeA,
                       const T_ATTRI_VAL &r_msc,
                       const T_ATTRI_VAL &r_lac,
                       const T_ATTRI_VAL &r_cellId,
                             T_TP_ID     &r_baseTp) const;

    //根据对方号码设置特殊号码属性
    bool  getUserTpSpecialAttr(const T_TPPARAM_VALUE &r_specialGroup,
                               const T_OTHER_PARTY   &r_otherPary,
                               const T_ATTRI_VAL     &r_areaCodeb,
                               const T_DATE_TIME     &r_beginTime,
                                     T_LEVEL_TYPE    &r_levelType,
                                     T_MEMBER_TYPE   &r_memType) const;
    //根据位置信息设置小区属性
    bool  getUserTpCellInfo(const T_ATTRI_VAL     &r_areaCodeb,
                            const T_TPPARAM_VALUE &r_areaId,
                            const T_ATTRI_VAL     &r_msc,
                            const T_ATTRI_VAL     &r_lac,
                            const T_ATTRI_VAL     &r_cellId,
                            const T_DATE_TIME     &r_beginTime,
                                  T_LEVEL_TYPE    &r_levelType,
                                  T_MEMBER_TYPE   &r_memType) const;
    //根据对方号码设置特殊V网属性
    bool  getUserTpVpnInfo(const T_TPPARAM_VALUE &r_specialVpnId,
                           const T_OTHER_PARTY   &r_otherPary,
                           const T_ATTRI_VAL     &r_areaCodeb,
                           const T_DATE_TIME     &r_beginTime,
                                 T_LEVEL_TYPE    &r_levelType,
                                 T_MEMBER_TYPE   &r_memType) ;

     
    /*
     * 根据计费合帐要素，求取明细账单项
     * @param r_feeField     费用域
     * @param r_bizType      业务类型
     * @param r_rateDtlFacts 因素列表（ID）
     * @param r_pcdr         话单
     * @param r_BillId       明细账单项
     * @return true 找到, false 未找到
     */           
    bool getRateDetailBill(const int         &r_feeField,
                           const T_BIZ_TYPE  &r_bizType,
                           const vector<int> &r_rateDtlFacts,
                           const PCDR        &r_pcdr,
                           const T_DATE      &r_billCycle,
                                 T_BILL_ID   &r_BillId);
    
    /*
     * 根据帐务合帐要素，求取明细账单项
     * @param r_feeField     费用域
     * @param r_bizType      业务类型
     * @param r_acctDtlFacts 因素列表（ID）
     * @param r_pcdr         话单
     * @param r_BillId       明细账单项
     * @return true 找到, false 未找到
     */
    //Modified By Sunam 2009/8/15 9:44:11
	/*
    bool getAcctDetailBill(const int         &r_feeField,
                           const T_BIZ_TYPE  &r_bizType,
                           const vector<int> &r_acctDtlFacts,
                           const PCDR        &r_pcdr,
                           const T_DATE      &r_billCycle,
                                 T_BILL_ID   &r_BillId);
	*/
	bool getAcctDetailBill(const int         &r_feeField,
                                const T_BIZ_TYPE  &r_bizType,
                                const vector<int> &r_acctDtlFacts,
                                const PCDR        &r_pcdr,
                                const T_DATE      &r_billCycle,
                                const T_TP_ID     &r_feePolicyId,
                            	const T_TIME      &r_beginTime,
                                      T_BILL_ID   &r_BillId);
    
    /*
     * 根据资费ID、角色、事件类型取实扣月租信息
     * @param r_tpId         资费ID
     * @param r_roleCode     成员角色
     * @param r_eventType    事件类型
     * @param r_monthRent    全月月租
     * @param r_halfRent     半月月租
     * @return true 找到, false 未找到
     */           
    bool getFeePolicyRent(const T_TP_ID       &r_tpId,
                          const T_MEMBER_TYPE &r_roleCode,
                          const T_BIZ_TYPE    &r_eventType,
                                long          &r_monthRent,
                                //long          &r_halfRent,
                                char          &r_compMethod,
                                int           &r_sumBillId,
                                int           &r_rentBillId);
    
    //根据组合帐目取对应的明细账单列表
    //bool getDetailBillItem();

    /*
     * 根据调帐原因标识取对应的原因
     * @param r_adjustReasonId     原因标识
     * @param r_adjustModeType     调整类型
     * @param r_priority           优先级
     * @param r_ifShowNegative     优惠负值显示标志
     * @param r_effectTimeTag      作用时间标志
     * @param r_condId             条件ID
     * @return true 找到, false 未找到
     */           
	bool getAdjustReason(const int     &r_adjustReasonId, 
                                    char    &r_adjustModeType,
                                    int     &r_priority      , 
                                    char    &r_ifShowNegative,
                                    char    &r_effectTimeTag,
                                    T_RULE_ID  &r_condId) ;

	  /**
	  * getUserInfoByVpnId:根据群标识,取用户记录信息
	  * @param  r_userGrpId           用户群标识
	  * @param  r_cycParam            帐期信息.
	  * @param  r_memberRoleCode           用户角色编码
	  * @param  r_userIdMap        用户角色集合.
	  * @return  -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
	 */
    int getUserInfoByVpnId(const T_SERVER_ID     &r_userGrpId,
                                const CycParam        &r_cycParam,
                                 const T_MEMBER_TYPE &r_memberRoleCode, 
                                 map<T_SERVER_ID,T_MEMBERINFO_TYPE>  &r_userIdMap);

      
      /*
	   * 获取系统级基本优惠政策
	   * @param r_bizType        事件类型
	   * @param r_beginTime      帐期开始时间
	   * @param r_beginTime      帐期结束时间
	   * @param r_baseTpVec      帐务系统资费
	   * @return true 找到, false 未找到
	   */ 
	  bool getBaseTp(const T_BIZ_TYPE  &r_bizType,
	                          const T_DATE_TIME &r_beginTime,
	                          const T_DATE_TIME &r_endTime,
	              			  vector<sysTariff> &r_baseTpVec) const;      
      
    bool getGprsBaseTp(const char             &recordType,
                       const T_GPRS_APNGROUP  &apnniGroup,
                       const T_ATTRI_VAL      &aspGroup,
                       const T_GPRS_SERVCODE  &servCode,
                       const T_AREA_CODE      &areaCode,
                       const T_ROAM_TYPE      &roamType,
                       const T_DATE           &startDate,
                       const T_TIME           &startTime,
                             int              &m_rateObject,
                             int              &m_rateMethod,
                             T_GPRS_SERVTYPE  &servType,
                             T_TP_ID          &m_tpId);
    //取GPRS流量时段优惠信息
    bool getDataTimeDiscount(const char             &recordType,
                             const T_ROAM_TYPE      &roamType,
                             const T_GPRS_SERVCODE  &servCode,
                             const int              &feild,
                             const T_DATE           &startDate,
                                   int              &discount);
                                   
	/**
	  * getFeePolicyAttrList:根据资费获取一些属性信息
	  * @param  r_tpId              资费ID
	  * @param  r_attrList          资费属性信息
	  * @return true:OK，false 未找到
	 */
   bool getFeePolicyAttrList(const T_TP_ID &r_tpId, vector<PO_FeePolicyAttr*> &r_attrList);
	
	/**
	 * Added By Sunam 2009/8/8 13:48:12
	 * getFeePolicyRemidInfo:根据资费编码及累计量编码查找免费资源提醒信息
	 * @param  r_feePolicyId                        资费编码
	 * @param  r_addupId                            累计量编码
	 * @param  r_result                             查找结果
	 * @return true 表示查找成功，false 表示未查找到指定记录
	 */
	bool getFeePolicyRemidInfo(const T_TP_ID &r_feePolicyId, const int &r_addupId, vector<FeePolicyRemidInfo> &r_result);
	bool getVpnRelFeePolicy(const T_SERVER_ID &r_vpnIdA, const T_DATE_TIME &r_beginTime, vector<T_SERVER_ID> &r_bVpnIdList, vector<T_TP_ID> &r_feePolicyIdList);
	bool getHolidayType(const char *r_beginDate, const T_DATE_TIME &r_beginDateTime, vector<T_OBJECT_ATTR> &r_holidayType);

	/**
  * getUserOtherInfo:根据用户标识,取用户其他信息记录集
  * @param  r_userId              用户标识
  * @param  r_cycParam            帐期信息.
  * @param  r_userOtherInfoVec    其他信息集合
  * @return  -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数

	int getUserOtherInfo(const T_SERVER_ID     &r_userId,
                       const CycParam        &r_cycParam,
                       vector<UserOtherInfo> &r_userOtherInfoVec);    	*/ 
  //protected:
  public:
    //根据资费政策ID，取对应参数定义信息                             
    bool getUserTpParamDefInfo(const T_TP_ID                     &r_tpId,
                                     vector<PO_UserTpParam_def*> &r_userTpParamDefs) const;
  protected:
    //根据成员取对应组列表       
    bool getSpecialInfoId(const T_MEMBER_TYPE       &r_memType,
                          const T_MEMBER_ID         &r_memberId,
                          const T_ATTRI_VAL         &r_areaCodeb,
                          const T_DATE_TIME         &r_beginTime,
                       vector<PO_SpecialInfo_comp*> &r_spInfoList) const;
    //根据位置信息判断是否在小区内
    bool isInCell(const T_ATTRI_VAL    &r_areaCodeb,
                  const T_ATTRI_VAL    &r_msc,
                  const T_ATTRI_VAL    &r_lac,
                  const T_ATTRI_VAL    &r_cellId,
                  const T_DATE_TIME    &r_beginTime,
                  const T_PARTITION_ID &r_partitionId) const;
    //根据位置找对应的小区列表
    bool getPartitionList(const T_ATTRI_VAL    &r_areaCodeb,
                          const T_ATTRI_VAL    &r_msc,
                          const T_ATTRI_VAL    &r_lac,
                          const T_ATTRI_VAL    &r_cellId,
                          const T_DATE_TIME    &r_beginTime,
                        vector<T_PARTITION_ID> &r_partIdList) const;      

    //根据VPNID或小区ID是否在特殊信息组织层次中
    bool getSpecialInfoById(const T_MEMBER_TYPE            &r_spInfoType ,
                            const T_SPECIALINFO_ID         &r_stdSpInfoId,  
                            const T_SPECIALINFO_ID         &r_spInfoId,
                                           T_LEVEL_TYPE    &r_levelType,
                                           T_MEMBER_TYPE   &r_memType) const;
    
    /*
     * 根据合帐要素，求取明细账单项
     * @param r_billtype     账单类型：计费、帐务
     * @param r_feeField     费用域
     * @param r_bizType      业务类型
     * @param r_acctDtlFacts 因素列表（ID）
     * @param r_pcdr         话单
     * @param r_BillId       明细账单项
     * @return true 找到, false 未找到
     */
    //Modified By Sunam 2009/8/15 9:43:44
	/*
    bool getDetailBill(const int         &r_billtype,
                       const int         &r_feeField,
                       const T_BIZ_TYPE  &r_bizType,
                       const vector<int> &r_dtlFacts,
                       const PCDR        &r_pcdr,
                       const T_DATE      &r_billCycle,
                             T_BILL_ID   &r_BillId);
	*/
	bool getDetailBill(const int         &r_billtype,
                            const int         &r_feeField,
                            const T_BIZ_TYPE  &r_bizType,
                            const vector<int> &r_dtlFacts,
                            const PCDR        &r_pcdr,
                            const T_DATE      &r_billCycle,
                            const T_TP_ID     &r_feePolicyId,
                            const T_TIME      &r_beginTime,
                                  T_BILL_ID   &r_BillId);
    //绑定基本政策扩展因素条件实现            
    void setBaseTpRule();
    //绑定明细帐目扩展因素条件实现
    void setDtlItemRule();
  protected:
  	vector<FactorParamMgr<FeebaseTP>*>       m_baseTpMgrList;//存放基本政策参数展开结果
    vector<FactorParamMgr<DetailBillItem>*>  m_detailBillMgrList;//存放明细帐单参数展开结果
    ParamContainer<PO_AttrFactors>          *m_PO_AttrFactorsContainer;
    ParamContainer<PO_RedoUserInfo>         *m_PO_RedoUserInfoContainer;
    ParamContainer<PO_InterRoamInFeeBaseTp> *m_PO_InterRoamInFeeBaseTpContainer;
    ParamContainer<PO_BaseTp>               *m_PO_cBaseTpContainer;
    ParamContainer<PO_BaseCellTp>           *m_PO_BaseCellTpContainer;
    ParamContainer<PO_CellDef>              *m_PO_CellDefContainer;
    ParamContainer<PO_FeebaseTP>            *m_PO_FeebaseTPContainer;
    ParamContainer<PO_DetailBillItem>       *m_PO_DetailBillItemContainer;
    ParamContainer<PO_FeePolicy_Rent>       *m_PO_FeePolicy_RentContainer;
    
    ParamContainer<PO_AdjustReason>         *m_PO_AdjustReasonContainer;
    //用户套餐参数定义信息
    ParamContainer<PO_UserTpParam_def>      *m_PO_UserTpParam_defContainer;
    ParamContainer<PO_SpecialInfo_comp>     *m_PO_SpecialInfo_compContainer;
    ParamContainer<PO_SpecInfo_org>         *m_PO_SpecInfo_org_compContainer;
    
    ParamContainer<PO_DetailItem>         *m_PO_DetailItem;
    CompositItemMgr                       *m_pCompositItemMgr;//帐目管理器
    ParamContainer<PO_PartyPrice>     *m_PO_PartyPriceContainer;//added by hany for 3G at 2009-5-16 01:35上午
    
    //GPRS计费增加的一些参数 2009-3-19 gaojf
    ParamContainer<PO_GprsTimeDiscount>   *m_PO_GprsTimeDiscount_Container;
    ParamContainer<PO_GprsBaseTp>         *m_PO_GprsBaseTp_Container;
    ParamContainer<PO_FeePolicyAttr>      *m_PO_FeePolicyAttr_Container;
    
    //Added By Sunam 2009/8/14 10:10:38 存放ObjectValue,ObjectParam对象
    vector<ObjectValue *> m_objectValue;
    vector<ObjectParam *> m_objectParam;
    vector<ObjectUserParam *> m_objectUserParam; //add by xuf 2009-10-2 13:24:03

    ParamContainer<PO_SectionRelFix>      *m_PO_SectionRelFix_Container;//add by fug for tianjin data 2009-10-10 15:18
    ParamContainer<PO_DiscntStatecode>    *m_PO_DiscntStatecode;//优惠状态表
  
  public:  //以下为资费树参数管理部分
    //根据条件编码返回条件指针
    //参数：condId 条件编码
	  //返回值: NULL：没有找到 非空：条件指针		
    CondBase * getCondition(const T_RULE_ID &condId);
    
  	//根据资费政策编码返回资费政策指针
  	//参数：feePolicyId 资费政策ID
  	//返回值: NULL：没有找到 非空：资费政策头指针	
  	FeePolicyComp * getFeePolicy(const T_TP_ID &r_tpId);

  	//根据对象编码返回对象指针
    //参数：objId 对象编码
	  //返回值: NULL：没有找到 非空：对象指针		
    ObjectBase * getObjectByObjId(const T_RULE_ID &objId);
    
    //获取帐目管理器指针指针    
    CompositItemMgr * getCompositItemMgr();

  private:
    void brCompositItem();   //建立外部关联-组合账目
    void brObjectBill();     //建立外部关联-账单对象
    void brObjectAddup();    //建立外部关联-累计量对象
    void brObjectUser();     //建立外部关联-资料对象
    void brObjectCdr();      //建立外部关联-清单对象
    void brObject();         //建立外部关联-对象索引
    void brAddupItem();      //建立外部关联-累计帐目
    void brFeePolicyComp();  //建立外部关联-资费政策构成
    void brEventFeePolicy(); //建立外部关联-事件资费
    void brPrice();          //建立外部关联-批价政策    
    void brPriceComp();      //建立外部关联-批价政策构成
    void brCondition();      //建立外部关联-条件索引
    void brCondRuleRela();   //建立外部关联-组合条件(依赖brCondition)    
    void brEffectDiscnt();   //建立外部关联-资费政策构成(依赖brObject) 
    void brEffectAddup();    //建立外部关联-累计处理
    void brEffectFeeCount(); //建立外部关联-费用计算 
    
    template <class T>
    void brCondRule(vector<T> &vCondRule)//建立外部关联-简单条件(依赖brObject)
    {
    	PO_Object poObject;	
    	PO_Object *rpoObject;	
     
    	long buff;
    	int length = sizeof(void *);
    	
    	typename vector<T>::iterator iterCond = vCondRule.begin();
    	for ( ; iterCond!=vCondRule.end(); ++iterCond)
    	{		
    		//added by hany at 2011/3/18 17:30:15
    		char computExpr = iterCond->m_record.m_judgeValue[0];
    		if(computExpr == 'E')
    		{
    			continue;
    		}
    		//added by hany at 2011/3/18 17:30:15
    		//判断值对象
    		memcpy((void *)&buff, (void *)&(*iterCond).m_record.m_judgeObj,length);
    		
    		//Added By Sunam 2009/8/14 10:28:34
    		//直接数
			if (iterCond->m_record.m_judgeObjType == '5')
    		{
    			ObjectValue *tmp = new ObjectValue();
    			tmp->setValue(buff);
    			m_objectValue.push_back(tmp);
    			iterCond->m_record.m_judgeObj = tmp;
    		}
    		else if (iterCond->m_record.m_judgeObjType == '6')
    		{
    			ObjectParam *tmp = new ObjectParam();
    			tmp->setParamId(buff);
    			m_objectParam.push_back(tmp);
    			iterCond->m_record.m_judgeObj = tmp;
    		}
    		 else if (iterCond->m_record.m_judgeObjType == '7') //add by xuf 2009-10-2 13:26:20
    		{
    			ObjectUserParam *tmp = new ObjectUserParam();
    			tmp->setParamId(buff);
    			m_objectUserParam.push_back(tmp);
    			iterCond->m_record.m_judgeObj = tmp;
    		}
    		else
    		{
    			poObject.m_record.m_id = buff;
    			//在对象索引中找
       			if ( m_PO_Object->search(poObject, rpoObject)>0)
    			{
       				(*iterCond).m_record.m_judgeObj=(*rpoObject).m_record.m_objectBase;
    			}
    			else
    			{			
    				(*iterCond).m_record.m_judgeObj = NULL;
					#ifdef _DEBUG_
						if( ((*iterCond).m_record.m_diviedTag<TAG_DIVIED_DURATION ||
						   (*iterCond).m_record.m_diviedTag>TAG_DIVIED_DATA )  &&
						     (*iterCond).m_record.m_judgeMode!=TAG_JUDGE_MODE_EXISTS  )
						cout <<"Error:CondRule="<<(*iterCond).m_record.m_id<<"  m_judgeObj="<<buff
						<<"  在m_PO_Object中没有定义"<<endl;
					#endif
    			}
    		}
    		
    		//最小值对象		
    		if ((*iterCond).m_record.m_minValue[0] == '*')//m_minObj 有效
    		{
    			//取对象id
    			poObject.m_record.m_id = atoi(&(*iterCond).m_record.m_minValue[1]);
    		
    			//在对象索引中找
    			if ( m_PO_Object->search(poObject, rpoObject)>0)
    			{
       				(*iterCond).m_record.m_minObj=rpoObject->m_record.m_objectBase;
    			}
    			else
    			{		  
    				(*iterCond).m_record.m_minObj = NULL; 
#ifdef _DEBUG_
cout <<"Error:CondRule="<<(*iterCond).m_record.m_id<<"  m_minObj="<<(*iterCond).m_record.m_minValue
<<"  在m_PO_Object中没有定义"<<endl;
#endif
    			}
    		}
    		else
    			(*iterCond).m_record.m_minObj = NULL;
    		
    		//最大值对象		
    		if ((*iterCond).m_record.m_maxValue[0] == '*')//m_maxObj 有效
    		{
    			//取对象id
    			poObject.m_record.m_id = atoi(&(*iterCond).m_record.m_maxValue[1]);
    		
    			//在对象索引中找
    			if ( m_PO_Object->search(poObject, rpoObject)>0)
    			{
       				(*iterCond).m_record.m_maxObj=rpoObject->m_record.m_objectBase;
    			}
    			else
    			{   
    				(*iterCond).m_record.m_maxObj = NULL;
#ifdef _DEBUG_
cout <<"Error:CondRule="<<(*iterCond).m_record.m_id<<"  m_minObj="<<(*iterCond).m_record.m_maxValue
<<"  在m_PO_Object中没有定义"<<endl;
#endif
    			}
    		}
    		else
    			(*iterCond).m_record.m_maxObj = NULL;
    	} 	
    }
    
    bool isLoopDefineCondition();//判断组合条件是否存在循环定义
    bool isLoopDefinePriceComp();//判断批价政策构成是否存在循环定义
    
    bool	addCondRule(T_RULE_ID &r_ruleId, vector<PO_CondRuleRela> &r_conRuleRelaList);
    bool	addPriceComp(int &priceId, int &parentNodeId, vector<PO_PriceComp> &vPriceComp);
  private:
    //获取参数的指针,供 br 函数调用,必要时可开放为 public
    
    //根据事件资费标识返回条件指针
  	//参数：eventFeePolicyId 事件资费标识
  	//返回值: NULL：没有找到 非空：事件资费头指针	
  	EventFeePolicy * getEventFeePolicy(const int &eventFeePolicyId);

    //根据批价政策编码返回条件指针
  	//参数：priceId 批价政策ID
  	//返回值: NULL：没有找到 非空：批价政策头指针	
  	Price * getPrice(const int &priceId);
  	
  	//根据资费构成标识返回批价参数实现指针
  	//参数：compId 资费构成标识
  	//返回值: NULL：没有找到 非空：批价参数实现头指针	
  	PriceParamImpl * getPriceParamImpl(const int &compId);	
  	
  	//根据批价政策标识返回批价政策构成指针
  	//参数：priceId 批价政策ID
  	//返回值: NULL：没有找到 非空：批价政策构成头指针	
  	PriceComp * getPriceComp(const int &priceId);	
  	
  	//根据批价政策标识返回批价政策扩展指针
  	//参数：priceId 批价政策ID
  	//返回值: NULL：没有找到 非空：批价政策扩展头指针	
  	PriceExtend * getPriceExtend(const int &priceId);

	//根据条件编码返回条件指针
  	//参数：effectId 优惠规则标志
  	//返回值: NULL：没有找到 非空：优惠规则头指针	
  	EffectDiscnt * getEffectDiscnt(const int &effectId);	
  protected:
    ParamContainerNext<PO_FeePolicyComp>  *m_PO_FeePolicyComp;  //资费政策构成 
    ParamContainerNext<PO_EventFeePolicy> *m_PO_EventFeePolicy; //事件资费 
  	ParamContainer<PO_Price>              *m_PO_Price;          //批价政策 
  	ParamContainerChild<PO_PriceComp>     *m_PO_PriceComp;      //批价政策构成 
  	ParamContainerNext<PO_PriceParamImpl> *m_PO_PriceParamImpl; //批价参数实现
  	ParamContainerNext<PO_PriceExtend>    *m_PO_PriceExtend;    //批价政策扩展
  	ParamContainerNext<PO_CondRuleRela>   *m_PO_CondRuleRela;   //组合条件
  	ParamContainer<PO_CondRuleScope>      *m_PO_CondRuleScope;  //简单条件()
  	ParamContainer<PO_CondRuleScopeL>     *m_PO_CondRuleScopeL; //简单条件[)
  	ParamContainer<PO_CondRuleScopeR>     *m_PO_CondRuleScopeR; //简单条件(]
  	ParamContainer<PO_CondRuleScopeLR>    *m_PO_CondRuleScopeLR;//简单条件[]
  	ParamContainer<PO_CondRuleIn>         *m_PO_CondRuleIn;     //简单条件In
  	ParamContainer<PO_CondRuleExists>     *m_PO_CondRuleExists; //简单条件Exists
  	ParamContainer<PO_Condition>          *m_PO_Condition;      //条件索引
  	ParamContainerNext<PO_EffectDiscnt>   *m_PO_EffectDiscnt;   //费用优惠
  	ParamContainer<PO_EffectFeeCount>     *m_PO_EffectFeeCount; //费用计算
  	ParamContainerNext<PO_EffectAddup>    *m_PO_EffectAddup;    //累计
  	ParamContainer<PO_ObjectBill>         *m_PO_ObjectBill;     //账单对象
  	ParamContainer<PO_ObjectCdr>          *m_PO_ObjectCdr;      //清单对象
  	ParamContainer<PO_ObjectUser>         *m_PO_ObjectUser;     //用户属性对象
  	ParamContainer<PO_ObjectAddup>        *m_PO_ObjectAddup;    //累计量对象
  	ParamContainer<PO_Object>             *m_PO_Object;         //对象索引
  	ParamContainer<PO_Item>               *m_PO_Item;           //账目
  	ParamContainer<PO_CompositItem>       *m_PO_CompositItem;   //组合账目
  	ParamContainer<PO_AddupItem>          *m_PO_AddupItem;      //累计账目
  	ParamContainer<PO_AddupCycleRule>     *m_PO_AddupCycleRule; //周期规则表
  	ParamContainer<PO_ElemEve     *m_PO_PriceComp;      //鎵逛环鏀跨瓥鏋勬垚 
  	ParamContainerNext<PO_PriceParamImpl> *m_PO_PriceParamImpl; //鎵逛环鍙傛暟瀹炵幇
  	ParamContainerNext<PO_PriceExtend>    *m_PO_PriceExtend;    //鎵逛环鏀跨瓥鎵╁睍
  	ParamContainerNext<PO_CondRuleRela>   *m_PO_CondRuleRela;   //缁勫悎鏉′欢
  	ParamContainer<PO_CondRuleScope>      *m_PO_CondRuleScope;  //绠