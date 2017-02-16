/**
*   批价参数查询接口实现：私有内存管理部分.
*/
#include "base/StringUtil.h" //add by yangyi

#include "PM_Rate.h"
#include "RateConstDef.h"
#include "MdbParamManager.h"
#include "PO_AttrFactors.h"
#include "PO_RedoUserInfo.h"
#include "PO_InterRoamInFeeBaseTp.h"
#include "PO_BaseTp.h"
#include "PO_BaseCellTp.h"
#include "PO_FeebaseTP.h"
#include "PO_DetailBillItem.h"
#include "PO_SpecInfo_org.h"
#include "PO_CellDef.h"
#include "PO_AdjustReason.h"
#include "PO_FeePolicy_Rent.h"
#include "PO_DetailItem.h"

//资费参数管理接口
#include "PO_FeePolicyComp.h" 
#include "PO_EventFeePolicy.h" 
#include "PO_Price.h"		 
#include "PO_PriceComp.h"	 
#include "PO_PriceParamImpl.h"  
#include "PO_PriceExtend.h"
#include "PO_CondRuleRela.h"  
#include "PO_CondRuleScope.h" 
#include "PO_CondRuleScopeL.h" 
#include "PO_CondRuleScopeR.h" 
#include "PO_CondRuleScopeLR.h" 
#include "PO_CondRuleIn.h" 
#include "PO_CondRuleExists.h" 
#include "PO_Condition.h" 
#include "PO_EffectDiscnt.h"  
#include "PO_EffectAddup.h"  
#include "PO_EffectFeeCount.h"  
#include "PO_ObjectBill.h"  
#include "PO_ObjectCdr.h"  
#include "PO_ObjectAddup.h"  
#include "PO_ObjectUser.h"  
#include "PO_Object.h" 
#include "PO_Item.h"  
#include "PO_CompositItem.h"  
#include "PO_AddupItem.h"  
#include "PO_AddupCycleRule.h"
#include "PO_EventElem.h"    
#include "PO_InfoElem.h"   
#include "PO_DefaultFeePolicy.h"    
#include "PO_PartyPrice.h"
//Added By Sunam 2009/8/8 13:52:21
#include "PO_FeePolicyRemidInfo.h"
#include "PO_Holiday.h"
#include "base/TimeUtil.h"
#include "Calendar.h"

//add for rent new version begin 
#include "PO_EventType.h"
#include "PO_CycleFeeRule.h"
//add for rent new version end

//#ifdef __PROV019__  //如果是山西    
//add by cuiyw 2009-12-13 20:48:30
#include "PO_FeepolicyMutexRela.h"
#include "PO_RateEventExclusiveRela.h"
//#endif

//包月费参数类
#include "PO_SpDiscount.h"
#include "PO_SpTariff.h" 
#include "PO_SpPunish.h"
#include "PO_SpRelaDiscntDetail.h"

bool PM_Rate::getData()
{
	if(ParamManager::getData()==false) 
	{
	  return false;
	}
	//add for new version begin
		vector<ProcSysParam> t_sysParamInfoVector;
		vector<ProcSysParam>::iterator t_iter;
		m_rentfeePolicySortMap.clear();
    if(getSysParam ("RENTFEE_POLICY_SORTTAG",t_sysParamInfoVector))
    {
    	for(t_iter = t_sysParamInfoVector.begin(); t_iter != t_sysParamInfoVector.end(); ++t_iter)
    	{
    	  int t_BizType = t_iter->m_valueN;
    		m_rentfeePolicySortMap.insert(map<int,int>::value_type(t_BizType,t_iter->m_preValueN1));
    	}
    }
    
		m_svcStateSortMap.clear();
		t_sysParamInfoVector.clear();
    if(getSysParam ("RENTFEE_SVCSTATE_SORTTAG",t_sysParamInfoVector))
    {
    	for(t_iter = t_sysParamInfoVector.begin(); t_iter != t_sysParamInfoVector.end(); ++t_iter)
    	{
    	  int t_BizType = t_iter->m_valueN;
    		m_svcStateSortMap.insert(map<int,int>::value_type(t_BizType,t_iter->m_preValueN1));
    	}
    }  

    //Added by Malone 2009-10-11 20:48 start
		m_rentfeeRecvStateMap.clear(); //需要收费的状态
		m_ohterRentfeeRecvStateMap.clear();//停保状态
		m_expRentfeeRecvStateMap.clear(); //停保状态后续不收费状态 add by xuf  2010-1-13 10:49:12			  		
		t_sysParamInfoVector.clear();
		int t_ServicId=0;
    if(getSysParam ("RENTFEE_SVCSTATE_RECVTAG",t_sysParamInfoVector))
    {
    	for(t_iter = t_sysParamInfoVector.begin(); t_iter != t_sysParamInfoVector.end(); ++t_iter)
    	{
    		 t_ServicId = t_iter->m_valueN;
    		m_rentfeeRecvStateMap.insert(map<int,T_SYSVALUE_STR>::value_type(t_ServicId,t_iter->m_preValueS1));
              //addde by dingr 
		m_ohterRentfeeRecvStateMap.insert(map<int,T_SYSVALUE_STR>::value_type(t_ServicId,t_iter->m_preValueS2));
			//add by xuf 2010-1-13 10:58:28	
    		m_expRentfeeRecvStateMap.insert(map<int,T_SYSVALUE_STR>::value_type(t_ServicId,t_iter->m_preValueS3));
    	}
    }
    //Added by Malone 2009-10-11 20:48 end  

//#ifdef __PROV019__    
    //add by xuf begin 2010-1-13 11:10:52
    //获取山西特殊租费收取的状态集合
    //1.收取租费状态集合
     m_stateTagMap.clear();
     m_stateTagMap.insert(map<char,int>::value_type('0',1));
     m_stateTagMap.insert(map<char,int>::value_type('N',1));
    if(!m_rentfeeRecvStateMap.empty())  
    {   
    	if(!getRentfeeRecvStateTag(0,m_stateTagMap,m_rentfeeRecvStateMap))
    	{
    		m_stateTagMap.clear();
    		m_stateTagMap.insert(map<char,int>::value_type('0',1));
    		m_stateTagMap.insert(map<char,int>::value_type('N',1));
    	}
   	}
   	
     //2.停保状态集合
     m_otherStateTagMap.clear();
     m_otherStateTagMap.insert(map<char,int>::value_type('1',2));
    if(!m_ohterRentfeeRecvStateMap.empty())  
    {   
    	if(!getRentfeeRecvStateTag(0,m_otherStateTagMap,m_ohterRentfeeRecvStateMap))
    	{
    		m_otherStateTagMap.clear();
    		m_otherStateTagMap.insert(map<char,int>::value_type('1',2));
    	}
   	}
   	
   	 //3.停保后不收取租费集合
     m_expStateTagMap.clear();
     m_expStateTagMap.insert(map<char,int>::value_type('5',4));
     m_expStateTagMap.insert(map<char,int>::value_type('7',4));
     m_expStateTagMap.insert(map<char,int>::value_type('A',4));
     m_expStateTagMap.insert(map<char,int>::value_type('B',4));		
    if(!m_expRentfeeRecvStateMap.empty())  
    {   
    	if(!getRentfeeRecvStateTag(0,m_expStateTagMap,m_expRentfeeRecvStateMap))
    	{
    		m_expStateTagMap.clear();
     		m_expStateTagMap.insert(map<char,int>::value_type('5',4));
     		m_expStateTagMap.insert(map<char,int>::value_type('7',4));
     		m_expStateTagMap.insert(map<char,int>::value_type('A',4));
     		m_expStateTagMap.insert(map<char,int>::value_type('B',4));	
    	}
   	}    
//#endif //add by xuf end 2010-1-13 11:10:58 
     
    m_stopOffsetTimes  = 0; 
    m_stopCalcRentDate = "20501231000000";
    if(getSysParam ("RENTFEE_STOP_DATE",t_sysParamInfoVector))
    {
    	m_stopCalcRentDate = t_sysParamInfoVector.begin()->m_valueD;
    	m_stopOffsetTimes  = t_sysParamInfoVector.begin()->m_valueN; //日租取状态偏移秒数add by xuf 2010-1-31 11:50:22
    }   
    
    m_isCallRentProc = true;
    if(getSysParam ("RENTFEE_UFILE_MODE",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN==1)
    	{
    	    m_isCallRentProc = false;
    	}
    }  
    
		 m_needAcctDealTag = true;	
		//参数配置是否需要帐务处理
		if(getSysParam("ACCOUNT_DEAL_TAG",t_sysParamInfoVector))
		{
			if(t_sysParamInfoVector.begin()->m_valueN==0)
    	{
    	  m_needAcctDealTag = false;
    	}
		}
   
     m_divisor          = 10;             //默认1->10 modify by xuf 2010-1-10 20:27:46        
   //取进位方式和精度
		if(getSysParam("CARRY_MODE",t_sysParamInfoVector))
   	{
   		if (t_sysParamInfoVector.begin()->m_preValueN1 >0)
   		{
   		 	m_divisor  = t_sysParamInfoVector.begin()->m_preValueN1;
   		}
   	}
    
     //add by xuf 2010-3-2 14:55:21
     m_bOnlyUFileFlag = false;
     //ad by xuf 2010-1-26 9:42:57
		//优惠轨迹是否输出原始费用
			m_outSourceFeeFlag = true;
   //取是否实时输出优惠轨迹标志
		m_outAllRealDiscountTag = '0';
		if (getSysParam("AS_ASP_OutAllRealDiscountTag",t_sysParamInfoVector))
		{
			m_outAllRealDiscountTag = t_sysParamInfoVector.begin()->m_valueS[0];

			if (t_sysParamInfoVector.begin()->m_preValueN1==2)
				m_outSourceFeeFlag = false;
				
			if (t_sysParamInfoVector.begin()->m_preValueN2==2)
				m_bOnlyUFileFlag = true;	
		}	
		
    //add by xuf 2009-8-20 21:50:25
        
     //开机状态集合, 各省CRM定义的开机状态不一定相同 add by xuf 2009-11-9 22:31:53
    m_openStateSet.clear();
    if(getSysParam("OPEN_STATE_SET",t_sysParamInfoVector))
    {
 			getOpenStateSet(t_sysParamInfoVector.begin()->m_valueS.c_str(),m_openStateSet);
    }  
    
     //add by xuf 2009-11-24 18:52:57
    //取是否实时输出优惠轨迹标志
		m_isNeedRealGrpDiscntTag = false;
		m_userGrpLockWaitTimes = 60*30; //默认为半个小时最大值
		if (getSysParam("REAL_GRPDISCNT_TAG",t_sysParamInfoVector))
		{
			if (t_sysParamInfoVector.begin()->m_valueN==1)
					m_isNeedRealGrpDiscntTag = true;
			
			if(t_sysParamInfoVector.begin()->m_preValueN1>0)		
				m_userGrpLockWaitTimes = t_sysParamInfoVector.begin()->m_preValueN1;	
		}	

  /*
  //Added by liuyufeng 2009-9-14 13:20 start 
  //将取值放在条件指针关联到之后，放到afterGetData()函数中处理       
	//获取所有的事件
  PO_EventType po;
	vector<PO_EventType> t_eventList ;
	vector<PO_EventType>::iterator riter;
	t_eventList = m_PO_EventTypeContainer->getParamVector();   
	m_vecRentEvent.clear(); 
  for(riter=t_eventList.begin();riter!=t_eventList.end();++riter)
  {
      RentEvent rentEvent;
      rentEvent.m_eventCode  = riter->m_record.m_eventTypeId;
      rentEvent.m_subCycType = riter->m_record.m_subCycType;
      rentEvent.m_serviceId  = riter->m_record.m_servId;
      rentEvent.m_condId     = riter->m_record.m_condId;
      rentEvent.m_condRule   = riter->m_record.m_condRule;
      m_vecRentEvent.push_back(rentEvent);
  }
  ::partial_sort (m_vecRentEvent.begin(), m_vecRentEvent.end(), m_vecRentEvent.end());
    //Added by liuyufeng 2009-9-14 13:20 end 
  */
  //add for new version end
  
	//added by liugj 20090505 同步广东代码到北方联通CVS
	m_mapStateRecvTag.clear();
	m_mapStateRecvTag['0']=1;
	m_mapStateRecvTag['N']=1;
	
	//added for tianjin Unicom
	//除了1、2、4、I、6、F状态之外，其他一般都收
	m_mapStateRecvTag['A']=1;
	m_mapStateRecvTag['B']=1;
	m_mapStateRecvTag['7']=1;
	m_mapStateRecvTag['8']=1;
	m_mapStateRecvTag['5']=1;//天津联通对于状态5(欠费停机)状态需要收取月租
		
	//modify by xuf 2010-1-8 14:15:03	
	if(!afterGetData())
	{
		return false;	
	}
	
	//add by xuf 2010-1-8 14:13:56
    //是否计算月租条件ID
    m_pCondBaseCalcRent = NULL;//add by xuf 2009-12-29 19:18:24
    m_isNeedCalcRentCondId = CONDID_ISNEED_CALC_RENT;
    if(getSysParam ("RENTFEE_CALC_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{
    	   m_isNeedCalcRentCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   m_pCondBaseCalcRent = getCondition(m_isNeedCalcRentCondId);  //add by xuf 2009-12-29 19:14:17
    	}
    }  
    //优惠是否有效条件ID
     m_pCondBaseDiscntInValid = NULL; //add by xuf 2009-12-29 19:14:22
     m_discntIsValidCondId = CONDID_STATE_DISCNT_INVALID;
     m_exTpVec.clear();
    if(getSysParam ("DISCNT_INVALID_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{
    	   m_discntIsValidCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   m_pCondBaseDiscntInValid = getCondition(m_discntIsValidCondId); //add by xuf 2009-12-29 19:14:17
    	  
    	  //add by xuf 2010-1-7 21:06:05
    	   string t_strTps=t_sysParamInfoVector.begin()->m_valueS;
    	    
    	   //m_valueS 长度不够时，使用预留字段 add by xuf 2010-1-17 17:31:10 
    	   if (t_sysParamInfoVector.begin()->m_preValueS1!="")
    	   {   	   	  
    	    t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS1;
    	   }  	  
    	   	if (t_sysParamInfoVector.begin()->m_preValueS2!="")
    	   {   	   	  
    	    t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS2;
    	   }
 	       if (t_sysParamInfoVector.begin()->m_preValueS3!="")
    	   {   	   	  
    	    t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS3;
    	   }
    	   
    	   m_exTpVec = StringUtil::split(t_strTps,",",true);		     	   	
    	   ::sort(m_exTpVec.begin(),m_exTpVec.end()); 
    	   
    #ifdef _DEBUG_	   
    	   for (vector<string>::iterator t_it = m_exTpVec.begin();
    	   	 t_it != m_exTpVec.end();++t_it )
    	   	{
    	   	  cout<<"exclude feepolicy:"<<*t_it<<endl;	
    	   	}	
   #endif 	   	
    	   		  
    	}
    } 
		
		//next if statement add by daizq@20100714  DISCNT_INVALID_COND 系统参数满足 仍要执行的事件资费ID的集合
    m_exEventTpVec.clear();
    if(getSysParam ("DISCNT_VALID_EVENT_FEEPOLICY",t_sysParamInfoVector))
    {


    	
    	string t_strTps=t_sysParamInfoVector.begin()->m_valueS;
    	 
    	//m_valueS 长度不够时，使用预留字段 add by daizq@20100714
    	if (t_sysParamInfoVector.begin()->m_preValueS1!="")
    	{   	   	  
    	 t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS1;
    	}  	  
    		if (t_sysParamInfoVector.begin()->m_preValueS2!="")
    	{   	   	  
    	 t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS2;
    	}
 	  	if (t_sysParamInfoVector.begin()->m_preValueS3!="")
    	{   	   	  
    	 t_strTps+=","+t_sysParamInfoVector.begin()->m_preValueS3;
    	}
    	
    	m_exEventTpVec = StringUtil::split(t_strTps,",",true);		     	   	
    	::sort(m_exEventTpVec.begin(),m_exEventTpVec.end()); 
    	   
    	#ifdef _DEBUG_	
    		cout<<"DISCNT_INVALID_COND 系统参数满足 仍要执行的事件资费ID:"<<endl;   
    		for (vector<string>::iterator t_it = m_exEventTpVec.begin();
    	   	 t_it != m_exEventTpVec.end();++t_it )
    		{
    	   	  cout<<"event_feepolicy_id : "<<*t_it<<endl;	
    		}	
   		#endif 	   	
    	 
    }      
    //是否计算结余条件ID
     m_pCondBaseCalcBal = NULL;//add by xuf 2009-12-29 19:18:24
     m_isNeedCalcBalCondId = CONDID_ISNEED_CALC_BAL;
    if(getSysParam ("BAL_CALC_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{
    	   m_isNeedCalcBalCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   m_pCondBaseCalcBal = getCondition(m_isNeedCalcBalCondId);  //add by xuf 2009-12-29 19:14:17
    	}
    }  
     //是否做账户级优惠条件ID
     m_pCondBaseAcctDiscntInValid = NULL;//add by xuf 2009-12-29 19:18:24
     m_isNeedAcctDiscntCondId = CONDID_ISNEED_ACCT_DISCNT;
    if(getSysParam ("ACCT_DISCNT_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{
    	   m_isNeedAcctDiscntCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   m_pCondBaseAcctDiscntInValid = getCondition(m_isNeedAcctDiscntCondId);  //add by xuf 2009-12-29 19:14:17
    	}
    }  
   
     //是否出帐条件ID
     m_isNeedAcctDealCondId = CONDID_NOTNEED_ACCTDEAL;
     m_pCondBaseNoAcct=NULL;//add by xuf 2009-12-27 22:11:26
    if(getSysParam ("NOT_ACCOUNT_DEAL_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{
    	   m_isNeedAcctDiscntCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   
    	   //add by xuf 2009-12-27 22:10:56
    	    m_pCondBaseNoAcct=getCondition(m_isNeedAcctDealCondId);
    	}
    }   

    //是否付费关系绑定条件ID add by xuf 2010-1-30 10:13:44
     m_pCondBasePayRelation=NULL;
    if(getSysParam ("NOT_PAYRELATION_COND",t_sysParamInfoVector))
    {
    	if(t_sysParamInfoVector.begin()->m_valueN>0)
    	{  
    	   int t_isPayRelationCondId = t_sysParamInfoVector.begin()->m_valueN;
    	   m_pCondBasePayRelation    = getCondition(t_isPayRelationCondId);
    	}
    }
    
    //added by wanglu 2011-2-26 begin
    //获取宽带存折编码deposit_code
    m_sysparamDepositCodeVec.clear();
    if(getSysParam("BROADBAND_DEPOSIT_CODE",t_sysParamInfoVector))
    {
    	//宽带存折编码
    	string t_strDepositCodes=t_sysParamInfoVector.begin()->m_valueS;
    	
    	//value_s长度不够,可使用预留字段pre_value_s1,pre_value_s2,pre_value_s3
    	if (t_sysParamInfoVector.begin()->m_preValueS1!="")
    	{   	   	  
    	 t_strDepositCodes+=","+t_sysParamInfoVector.begin()->m_preValueS1;
    	}  	  
    		if (t_sysParamInfoVector.begin()->m_preValueS2!="")
    	{   	   	  
    	 t_strDepositCodes+=","+t_sysParamInfoVector.begin()->m_preValueS2;
    	}
 	    if (t_sysParamInfoVector.begin()->m_preValueS3!="")
    	{   	   	  
    	 t_strDepositCodes+=","+t_sysParamInfoVector.begin()->m_preValueS3;
    	}
    	
    	m_sysparamDepositCodeVec = StringUtil::split(t_strDepositCodes,",",true);		     	   	
    	::sort(m_sysparamDepositCodeVec.begin(),m_sysparamDepositCodeVec.end()); 
    	 
      #ifdef _DEBUG_	   
    	for (vector<string>::iterator t_it = m_sysparamDepositCodeVec.begin();
    	 	   t_it != m_sysparamDepositCodeVec.end();++t_it)
    	{
    	 	  cout<<"BroadBand DepositCode:"<<*t_it<<endl;	
    	}	
      #endif 	
    }   	    
    //end 2011-2-26           

	return true;
	
	
}
//取参数后,初始化一些内部成员
bool PM_Rate::afterGetData()
{
 	//建立内部指针关联
	m_PO_FeePolicyComp->buildNextRelation();
	m_PO_EventFeePolicy->buildNextRelation();	
	m_PO_PriceComp->buildNextRelation();
	m_PO_PriceParamImpl->buildNextRelation();
	m_PO_PriceExtend->buildNextRelation();	
	m_PO_CondRuleRela->buildNextRelation();
	m_PO_EffectDiscnt->buildNextRelation();
	m_PO_EffectAddup->buildNextRelation();
   
   //建立外部指针关联
	brCompositItem();				//建立外部关联-组合账目
	brObjectBill();				  //建立外部关联-账单对象
	brObjectAddup();              //建立外部关联-累计量对象
	brObjectCdr();              //建立外部关联-清单对象
	brObjectUser();              //建立外部关联-资料对象
	brAddupItem();                //建立外部关联-累计帐目
	brObject();					    //建立外部关联-对象索引
	brCondition();				  //建立外部关联-条件索引
	brCondRuleRela();				//建立外部关联-组合条件(依赖brCondition)

	if (isLoopDefineCondition() )	 //组合条件存在循环定义
	{
		return false;
	}
	brFeePolicyComp();			     //建立外部关联-资费政策构成				  
	brEventFeePolicy();			     //建立外部关联-事件资费
	brPrice();					         //建立外部关联-批价政策					  
	brPriceComp();				       //建立外部关联-批价政策构成(依赖brCondition) 
	if (isLoopDefinePriceComp() )//批价政策构成存在循环定义
	{
		return false;
	}   
	brEffectDiscnt();				//建立外部关联-费用优惠
	brEffectAddup();                //建立外部关联-累计处理
	brEffectFeeCount();             //建立外部关联-费用计算 
	
	//Added By Sunam 2009/8/14 10:11:50	
    //建立关联关系前清除已有结果
    for (int i=0; i < m_objectValue.size(); ++i)
    {
        delete m_objectValue[i];
    }
    for (int i=0; i < m_objectParam.size(); ++i)
    {
        delete m_objectParam[i];
    }
    //add by xuf 2009-10-2 13:24:54
     for (int i=0; i < m_objectUserParam.size(); ++i)
    {
        delete m_objectUserParam[i];
    }  
    m_objectValue.clear();
    m_objectParam.clear();
    m_objectUserParam.clear(); //add by xuf 2009-10-2 13:25:56

	//建立外部关联-简单条件()(依赖brObject)
	vector<PO_CondRuleScope> &vScope = m_PO_CondRuleScope->getParamVector();
	brCondRule<PO_CondRuleScope>(vScope);  
		
	//建立外部关联-简单条件[)(依赖brObject)
	vector<PO_CondRuleScopeL> &vScopeL = m_PO_CondRuleScopeL->getParamVector();
	brCondRule<PO_CondRuleScopeL>(vScopeL); 
	
	//建立外部关联-简单条件(](依赖brObject)
	vector<PO_CondRuleScopeR> &vScopeR = m_PO_CondRuleScopeR->getParamVector();	
	brCondRule<PO_CondRuleScopeR>(vScopeR); 
	
	//建立外部关联-简单条件[](依赖brObject)
	vector<PO_CondRuleScopeLR> &vScopeLR=m_PO_CondRuleScopeLR->getParamVector();
	brCondRule<PO_CondRuleScopeLR>(vScopeLR);
	
	//建立外部关联-简单条件in(依赖brObject)   
	vector<PO_CondRuleIn> & vIn = m_PO_CondRuleIn->getParamVector();	
	brCondRule<PO_CondRuleIn>(vIn);	
	
	//初始化帐目管理器
	vector<PO_CompositItem> &vCompositItem=m_PO_CompositItem->getParamVector();
	vector<PO_Item> &vItem = m_PO_Item->getParamVector();
	vector<PO_DetailItem> &vDetailItem = m_PO_DetailItem->getParamVector();
	if (!m_pCompositItemMgr->initialize(vCompositItem, vItem,
	                                    vDetailItem,m_errorMessage) )
	{
		return false;
	}

  //绑定基本政策扩展因素条件实现            
  setBaseTpRule();   
  //绑定明细帐目扩展因素条件实现
  setDtlItemRule();
  //Added By Sunam 2009/8/15 9:37:43
  holidayInfoInit();
  
  //建立资费条件指针 add by gaojf 2009-5-7 10:21
  vector<PO_FeePolicyAttr>   &t_tpAttrList=m_PO_FeePolicyAttr_Container->getParamVector();
  for(vector<PO_FeePolicyAttr>::iterator t_itr=t_tpAttrList.begin();
      t_itr!=t_tpAttrList.end();++t_itr)
  {
    if(t_itr->m_record.m_ruleId == DEFAULT_RULE_ID)
    {
      t_itr->m_record.m_condRule = NULL;
    }else
    {
      t_itr->m_record.m_condRule=getCondition(t_itr->m_record.m_ruleId);
    }
  }
  
  return true;
} 

bool PM_Rate::end()
{
  bool t_bRet = true;
  //一些成员清理
    //Added By Sunam 2009/8/25 15:39:27
    for (int i=0; i < m_objectValue.size(); ++i)
    {
        delete m_objectValue[i];
    }
    for (int i=0; i < m_objectParam.size(); ++i)
    {
        delete m_objectParam[i];
    }    
    for (int i=0; i < m_objectUserParam.size(); ++i)
    {
        delete m_objectUserParam[i];
    }
    
/*  if(ParamManager::end()==false)
  {
    t_bRet = false;
  }
  if(m_pMdbManager->end()==false)
  {
    t_bRet = false;
  }*/
  if(PM_Billing::end()==false)
  {
    t_bRet = false;
  }
  
  
  return t_bRet;
}

PM_Rate::PM_Rate()
{
  m_pCompositItemMgr = new CompositItemMgr();
}

PM_Rate::~PM_Rate()
{
	delete m_pCompositItemMgr;
  for(m_paramContainerListItr=m_paramContainerList.begin();
      m_paramContainerListItr!=m_paramContainerList.end();
      m_paramContainerListItr++)
  {
      delete (*m_paramContainerListItr);
  }
    
  m_paramContainerList.clear();    
}


#define ADDPARAMOBJECT(TYPE,OBJ) \
          OBJ = new ParamContainer<TYPE>(); \
          if(OBJ == NULL) \
              return false; \
          else \
              m_paramContainerList.push_back(OBJ); 
              
#define ADDPARAMOBJECTNEXT(TYPE,OBJ) \
          OBJ = new ParamContainerNext<TYPE>(); \
          if(OBJ == NULL) \
              return false; \
          else \
              m_paramContainerList.push_back(OBJ); 
              
#define ADDPARAMOBJECTCHILD(TYPE,OBJ) \
          OBJ = new ParamContainerChild<TYPE>(); \
          if(OBJ == NULL) \
              return false; \
          else \
              m_paramContainerList.push_back(OBJ);

bool PM_Rate::addParamObject() 
{
	if(PM_Billing::addParamObject()==false)
	{
		return false;
	}
	
	ADDPARAMOBJECT(PO_AttrFactors         ,m_PO_AttrFactorsContainer);
	//ADDPARAMOBJECT(PO_BillingCycle        ,m_PO_BillingCycleContainer);
	//ADDPARAMOBJECT(PO_CycleTime           ,m_PO_CycleTimeContainer);
	ADDPARAMOBJECT(PO_RedoUserInfo        ,m_PO_RedoUserInfoContainer);
  ADDPARAMOBJECT(PO_InterRoamInFeeBaseTp,m_PO_InterRoamInFeeBaseTpContainer);
  ADDPARAMOBJECT(PO_BaseTp              ,m_PO_cBaseTpContainer);
  ADDPARAMOBJECT(PO_BaseCellTp          ,m_PO_BaseCellTpContainer);
  ADDPARAMOBJECT(PO_CellDef             ,m_PO_CellDefContainer); 
  ADDPARAMOBJECT(PO_FeebaseTP           ,m_PO_FeebaseTPContainer);
  ADDPARAMOBJECT(PO_DetailBillItem      ,m_PO_DetailBillItemContainer);
  ADDPARAMOBJECT(PO_UserTpParam_def     ,m_PO_UserTpParam_defContainer);
  ADDPARAMOBJECT(PO_SpecialInfo_comp    ,m_PO_SpecialInfo_compContainer);
  ADDPARAMOBJECT(PO_SpecInfo_org        ,m_PO_SpecInfo_org_compContainer);
  //ADDPARAMOBJECT(PO_SysParam            ,m_PO_SysParamContainer);
  ADDPARAMOBJECT(PO_AdjustReason        ,m_PO_AdjustReasonContainer);
  ADDPARAMOBJECT(PO_FeePolicy_Rent      ,m_PO_FeePolicy_RentContainer);

  //资费参数部分
	ADDPARAMOBJECTNEXT (PO_FeePolicyComp,  m_PO_FeePolicyComp);
	ADDPARAMOBJECTNEXT (PO_EventFeePolicy,  m_PO_EventFeePolicy);
	ADDPARAMOBJECT	   (PO_Price,		   m_PO_Price);		  
	ADDPARAMOBJECTCHILD(PO_PriceComp,	   m_PO_PriceComp);	  
	ADDPARAMOBJECTNEXT (PO_PriceParamImpl, m_PO_PriceParamImpl);
	ADDPARAMOBJECTNEXT (PO_PriceExtend,    m_PO_PriceExtend);	 
	ADDPARAMOBJECTNEXT (PO_CondRuleRela,   m_PO_CondRuleRela);
	ADDPARAMOBJECT	   (PO_CondRuleScope,  m_PO_CondRuleScope);
	ADDPARAMOBJECT	   (PO_CondRuleScopeL, m_PO_CondRuleScopeL);
	ADDPARAMOBJECT	   (PO_CondRuleScopeR, m_PO_CondRuleScopeR);
	ADDPARAMOBJECT	   (PO_CondRuleScopeLR,m_PO_CondRuleScopeLR);
	ADDPARAMOBJECT	   (PO_CondRuleIn,	   m_PO_CondRuleIn);
	ADDPARAMOBJECT	   (PO_CondRuleExists, m_PO_CondRuleExists);	
	ADDPARAMOBJECT	   (PO_Condition,	   m_PO_Condition);
	ADDPARAMOBJECTNEXT (PO_EffectDiscnt,   m_PO_EffectDiscnt);
	ADDPARAMOBJECTNEXT (PO_EffectAddup,    m_PO_EffectAddup);
	ADDPARAMOBJECT     (PO_EffectFeeCount, m_PO_EffectFeeCount);
	ADDPARAMOBJECT	   (PO_ObjectBill,	   m_PO_ObjectBill);
	ADDPARAMOBJECT	   (PO_ObjectAddup,	   m_PO_ObjectAddup);
	ADDPARAMOBJECT	   (PO_ObjectCdr,	   m_PO_ObjectCdr);
	ADDPARAMOBJECT	   (PO_ObjectUser,	   m_PO_ObjectUser);
	ADDPARAMOBJECT	   (PO_Object,		   m_PO_Object);
	ADDPARAMOBJECT	   (PO_Item,		   m_PO_Item);
	ADDPARAMOBJECT	   (PO_CompositItem,   m_PO_CompositItem);
	ADDPARAMOBJECT	   (PO_AddupItem,      m_PO_AddupItem);
	ADDPARAMOBJECT	   (PO_AddupCycleRule, m_PO_AddupCycleRule);
	ADDPARAMOBJECT	   (PO_ElemEvent,      m_PO_ElemEvent);
	ADDPARAMOBJECT	   (PO_ElemInfo ,      m_PO_ElemInfo);
	
	ADDPARAMOBJECT	   (PO_DetailItem, m_PO_DetailItem);

//#ifdef __PROV019__  //如果是山西		add by xuf 2009-12-13 17:01:08
	ADDPARAMOBJECT	   (PO_DefaultFeePolicy, m_PO_DefaultFeePolicy);
//#endif
	//added by hany for 3G at 2009-5-16 01:52上午
  ADDPARAMOBJECT(PO_PartyPrice              ,m_PO_PartyPriceContainer);
	//Added By Sunam 2009/8/8 13:52:21
	ADDPARAMOBJECT(PO_FeePolicyRemidInfo,m_PO_FeePolicyRemidInfoContainer);
	ADDPARAMOBJECT(PO_VpnRelFeePolicy,m_PO_VpnRelFeePolicyContainer);
	//Added By Sunam 2009/8/11 13:32:40
		//add by fug 2009-10-10 s
	ADDPARAMOBJECT     (PO_SectionRelFix,m_PO_SectionRelFix_Container);
	//add by fug 2009-10-10 e
	ADDPARAMOBJECT(PO_Holiday,m_PO_HolidayContainer);
	ADDPARAMOBJECT(PO_FeePolicyAttr       ,m_PO_FeePolicyAttr_Container);
  
  //add for rent new version begin
	ADDPARAMOBJECT     (PO_EventType,m_PO_EventTypeContainer);
	ADDPARAMOBJECT     (PO_CycleFeeRule,m_PO_CycleFeeRuleContainer);
	//add for rent new version end
	ADDPARAMOBJECT		 (PO_DiscntStatecode, m_PO_DiscntStatecode);

//#ifdef __PROV019__  //如果是山西		
	//add by cuiyw 2009-12-13 20:50:00
  ADDPARAMOBJECT	   (PO_FeepolicyMutexRela, m_PO_FeepolicyMutexRela);
  ADDPARAMOBJECT	   (PO_RateEventExclusiveRela, m_PO_RateEventExclusiveRela);

//#endif
      //包月费参数对象添加
  ADDPARAMOBJECT(PO_SpDiscount,m_PO_SpDiscount_Container);
  ADDPARAMOBJECT(PO_SpTariff,m_PO_SpTariff_Container);
  //ADDPARAMOBJECT(PO_AdcECuserNum,m_PO_AdcECuserNum_Container);
  //ADDPARAMOBJECT(PO_AdcECAbnormalState,m_PO_AdcECAbnormalState_Container);
  //ADDPARAMOBJECT(PO_SpCdrInfo,m_PO_SpCdrInfo_Container);
  ADDPARAMOBJECT(PO_SpPunish,m_PO_SpPunish_Container);
  ADDPARAMOBJECT(PO_SpRelaDiscntDetail,m_PO_SpRelaDiscntDetail_Container);

	return true;
}

bool PM_Rate::getAttrFactors(const int &r_fType,const T_BIZ_TYPE &r_bizType,vector<PO_AttrFactors*> &r_factorList)
{
  PO_AttrFactors r_po;
  r_factorList.clear();
  r_po.m_record.m_fType  =r_fType;
  r_po.m_record.m_biztype=r_bizType;
  if(m_PO_AttrFactorsContainer->searchMulti(r_po,r_factorList)==0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
 * getBillCycle       根据日期取对应的帐期.
 * @param  r_Billday : 日期YYYYMMDD
 * @param  r_billCycle:帐期YYYYMM
 * @return true: 是  false:否
 */
/*已放入PM_Billing中
bool PM_Rate::getBillCycle(const int &r_Billday,int &r_billCycle) const
{
  PO_BillingCycle t_po,t_rpo;
  
  t_po.m_record.m_beginDate = r_Billday;
  t_po.m_record.m_endDate   = r_Billday;
  if(m_PO_BillingCycleContainer->search(t_po,t_rpo))
  {
    r_billCycle = (int)(t_rpo.m_record.m_cycleId);
    return true;
  }else
  {
    return false;
  }
}*/

/*已放入PM_Billing中bool PM_Rate::getCycleTime(const int &r_cycle,  
                         T_DATE_TIME &r_beginTime,
                         T_DATE_TIME &r_halfTime,
                         T_DATE_TIME &r_endTime) const
{
  CycParam t_cycleParam;
  if(PM_Billing::getCycleTime(r_cycle,t_cycleParam)==false)
  {
    return false;
  }    
  r_beginTime = t_cycleParam.m_beginTime;
  r_halfTime  = t_cycleParam.m_halfTime; 
  r_endTime   = t_cycleParam.m_endTime;
  return true;
}*/

/*已放入PM_Billing中bool PM_Rate::getCycleTime(const int &r_cycle,CycParam &r_cycleParam) const
{
    return PM_Billing::getCycleTime(r_cycle,r_cycleParam);
}*/

/**
 * isRedoUser       根据用户标识判断是否回退用户.
 * @param  r_userId : 用户标识
 * @return true: 是  false:否
 */
bool PM_Rate::isRedoUser(const T_SERVER_ID &r_userId) const
{
  PO_RedoUserInfo  t_po;
  PO_RedoUserInfo* t_prpo;
  t_po.m_record.m_userId = r_userId;
  
  if(m_PO_RedoUserInfoContainer->search(t_po,t_prpo))
  {
    return true;
  }
  return false;
}

//Added by Sunam 2006-5-26 17:33 
bool PM_Rate::getInterRoamInBaseTp(const T_FEE_FIELD r_feeField,
                              		 const T_DATE_TIME &r_beginTime,
                              		 const T_ATTRI_VAL &r_aspGroup,
                              		 const T_ATTRI_VAL &r_bizType,
                              		 const char &r_sameHomeAb,
                              		 const T_ATTRI_VAL &r_homeCodeB,
                              		 const T_ATTRI_VAL &r_callType,
                              		 const T_ATTRI_VAL &r_bAsp,
                              		 const T_ATTRI_VAL &r_bServType,
                              		 const T_ATTRI_VAL &r_carrierType,
                              		 const T_ATTRI_VAL &r_longType1,
                              		 const T_ATTRI_VAL &r_longGroup1,
                              		 const T_ATTRI_VAL &r_longType2,
                              		 const T_ATTRI_VAL &r_longGroup2,
                                          T_TP_ID    &r_tpId) const
{
  PO_InterRoamInFeeBaseTp t_po;
  vector<PO_InterRoamInFeeBaseTp*> t_prpo;
  

  // 根据 feeField  查找适当的费率政策
  t_po.m_record.m_feeField   = r_feeField;
  t_po.m_record.m_aspGroup   = r_aspGroup;
  t_po.m_record.m_bizType    = r_bizType;
  t_po.m_record.m_sameHomeAb = r_sameHomeAb;
  t_po.m_record.m_homeCodeB  = r_homeCodeB;
  t_po.m_record.m_callType   = r_callType;
  t_po.m_record.m_bAsp       = r_bAsp;
  t_po.m_record.m_bServType  = r_bServType;
  t_po.m_record.m_carrierType= r_carrierType;
  t_po.m_record.m_longType1  = r_longType1;
  t_po.m_record.m_longGroup1 = r_longGroup1;
  t_po.m_record.m_longType2  = r_longType2;
  t_po.m_record.m_longGroup2 = r_longGroup2;
  
  if(m_PO_InterRoamInFeeBaseTpContainer->scanSearchMulti(t_po,t_prpo)==0)
  {
	  return false;
  }
  for (vector<PO_InterRoamInFeeBaseTp*>::iterator t_itr = t_prpo.begin(); 
       t_itr != t_prpo.end(); ++t_itr)
  {
	  if (((*t_itr)->m_record.m_beginTime <= r_beginTime) && 
	      ((*t_itr)->m_record.m_endTime   >= r_beginTime))
	  {
		  r_tpId = (*t_itr)->m_record.m_tpId;
		  return true;
	  }
  }
  return false;
}


/*
 * 基本政策求取
 * @param r_feeField  费用域
 * @param r_baseTpFacts 因素列表（ID）
 * @param r_pcdr        话单
 * @param r_tp_id     政策
 * @return true 找到, false 未找到
 */        
bool PM_Rate::getFeeBaseTp(const int         &r_feeField,
                           const T_BIZ_TYPE  &r_bizType,
                           const vector<int> &r_baseTpFacts,
                           const PCDR        &r_pcdr,
                           const T_DATE_TIME &r_startTime,
                                 T_TP_ID     &r_tpId)
{
  int i,t_fSize;
  FeebaseTP  t_feeBaseTp;
  t_feeBaseTp.m_bizType  = r_bizType;
  t_feeBaseTp.m_feeField = r_feeField;
  t_fSize = r_baseTpFacts.size();
  
  for(i=0;i<t_fSize;++i)
  {
    t_feeBaseTp.m_factors[i] = r_pcdr[r_baseTpFacts[i]];
  }
	for(vector<FactorParamMgr<FeebaseTP>*>::iterator 
	    t_tpMgrItr=m_baseTpMgrList.begin();
			t_tpMgrItr!=m_baseTpMgrList.end();++t_tpMgrItr)
	{ //如果在展开后的范围内，则在展开后采用高效率的算法求取
		if(r_bizType==(*t_tpMgrItr)->m_bizType)
		{
		  FeebaseTP* t_result=NULL;
			if((*t_tpMgrItr)->isMathFactors(t_feeBaseTp)==true)
			{
			  if((*t_tpMgrItr)->getElement(t_feeBaseTp,t_fSize,r_pcdr,t_result)==true)
			  {
			    r_tpId = t_result->m_tpId;
			    return true;
			  }else return false;
			}
			else 
			{
			  break;
			}
		}
	}
  
  bool t_flag=false;
  vector<PO_FeebaseTP> &t_PoList = m_PO_FeebaseTPContainer->getParamVector();
  for(vector<PO_FeebaseTP>::iterator t_itr=t_PoList.begin();
      t_itr!=t_PoList.end();++t_itr)
  {
    if(!(t_itr->m_record == t_feeBaseTp ))
    {
      continue;
    }
    t_flag = true;
    for(i=0;i<t_fSize;++i)
    {
      //added by yangyi 2009,1116 如果表里配置@开头，表示将话单属性按;分割出来，只要有一个匹配就满足
      bool isPassFlag = false;
      if (strncmp((t_itr->勮寖鍥村唴锛屽垯鍦ㄥ睍寮