#include "PM_Billing.h"

#include "PO_CycleEparchy.h"
#include "PO_SubCycle.h"
#include "PO_BillingCycle.h"
#include "PO_EventType.h"
#include "PO_SysParam.h"
#include "PO_CycleFeeRule.h"
#include "PO_Discount.h"
#include "PO_UserChannel.h"
#include "PO_ServStatePriority.h"
//#include "Mdb_UserServState.h"
#include "Mdb_UserInfo.h"
#include "Mdb_UserTp.h"
#include "Mdb_UserServState.h"
#include "Mdb_UserImportInfo.h"
#include "Mdb_ReUserInfo_All.h"
#include "Mdb_UserServ.h"
//#include "RentfeeCommon.h"
#include "MdbParamManager.h"
#include "Mdb_UserParam.h"
#include "Mdb_UserServParam.h"
#include "Mdb_AccntInfo.h"

//#include "RateConstDef.h"
//#include "Mdb_UserTp.h"
//#include "Mdb_Partition_Info.h"
//#include "Mdb_SpInfo_Org.h"
//#include "Mdb_UserTp_Param.h"

bool PM_Billing::getData()
{
	if(ParamManager::getData()==false) 
	{
	  return false;
	}
	return afterGetData();
}
//取参数后,初始化一些内部成员
bool PM_Billing::afterGetData()
{
	//一些成员初始化
	return true;
} 

bool PM_Billing::end()
{
  bool t_bRet = true;
  //一些成员清理

  if(ParamManager::end()==false)
  {
    t_bRet = false;
  }
  if(m_pMdbManager->end()==false)
  {
    t_bRet = false;
  }

  //结束数组
  	map<int,int** >::iterator iterMap;
	for(iterMap=m_subCycStartDayMap.begin();iterMap!=m_subCycStartDayMap.end();++iterMap)
	{
		for(int i=0;i<32;i++)
     	{delete   [](iterMap->second)[i];}   
  		delete   [](iterMap->second); 
	}
	
	for(iterMap=m_subCycEndDayMap.begin();iterMap!=m_subCycEndDayMap.end();++iterMap)
	{
		for(int i=0;i<32;i++)
     	{delete   [](iterMap->second)[i];}   
  		delete   [](iterMap->second); 
	}
  return t_bRet;
}

PM_Billing::PM_Billing():m_eparchyCode("")
{    
    m_needUpdated = true;
    m_pMdbManager = new MdbParamManager();
    m_dbNo = 1;

    //初始化子账期类型2009-8-18 18:58:55
    m_subCycTypeVec.clear();
    char array[] = {'4','7','8'};
    m_subCycTypeVec.insert(m_subCycTypeVec.begin(),array,array+3);
}

PM_Billing::~PM_Billing()
{
  delete m_pMdbManager;
  for(m_paramContainerListItr=m_paramContainerList.begin();
      m_paramContainerListItr!=m_paramContainerList.end();
      m_paramContainerListItr++)
  {
      delete (*m_paramContainerListItr);
  }
    
  m_paramContainerList.clear();    
}

bool PM_Billing::needUpdate()
{	
	//自身更新判断
	m_needUpdated = ParamManager::needUpdate();

	return m_needUpdated;
}


bool PM_Billing::update()
{	
	//自身更新操作
	if ( m_needUpdated )
	{
		if ( !ParamManager::update() )
		{
			return false;
		}
	}
	
	return true;
}

//获取内存数据库出错信息
void PM_Billing::getMdbErrMsg(int& r_errno,string& r_errMsg)
{
	m_pMdbManager->getMdbErrMsg(r_errno,r_errMsg);
}

#define ADDPARAMOBJECT(TYPE,OBJ) \
          OBJ = new ParamContainer<TYPE>(); \
          if(OBJ == NULL) \
              return false; \
          else \
              m_paramContainerList.push_back(OBJ); 

bool PM_Billing::addParamObject() 
{
#ifdef _DEBUG_
cout<<"PM_Billing::addParamObject"<<endl;
#endif
	
//	ADDPARAMOBJECT(PO_CycleTime,m_PO_CycleTimeContainer);
	ADDPARAMOBJECT(PO_CycleEparchy,m_PO_CycleEparchyContainer);
	ADDPARAMOBJECT(PO_BillingCycle,m_PO_BillingCycleContainer);
	ADDPARAMOBJECT(PO_SysParam,m_PO_SysParamContainer);
	ADDPARAMOBJECT(PO_CycleFeeRule,m_PO_CycleFeeRuleContainer);
	ADDPARAMOBJECT(PO_SubCycle,m_PO_SubCycleContainer);
	ADDPARAMOBJECT(PO_EventType,m_PO_EventTypeContainer);
	ADDPARAMOBJECT(PO_Discount,m_PO_DiscountContainer);
	ADDPARAMOBJECT(PO_UserChannel,m_PO_UserChannelContainer);
	ADDPARAMOBJECT(PO_ServStatePriority,m_PO_ServStatePriorityContainer);
	
#ifdef _DEBUG_
cout<<"PM_Billing::addParamObject end"<<endl;
#endif
	return true;
}

/*bool PM_Billing::setMdbInfo(T_DATABASETYPE& dbType,string& dbServer,
                    string& hostName,int& port)
{
	m_mdbInfo.m_dbType = dbType;
	m_mdbInfo.m_dbServer = dbServer;
	m_mdbInfo.m_hostName = hostName;
	m_mdbInfo.m_port = port;
	
	return true;
}*/

bool PM_Billing::initialization()
{
	//内存数据库初始化
	//if(!setMdbInfo(m_mdbInfo))
		//return false;
	//oracle初始化
    m_needUpdated = true;    
    vector<ProcSysParam> r_sysParamInfoVector;
    vector<ProcSysParam>::iterator iter;
    bool ret = ParamManager::initialization();    
    if(!ret)
    {
    	#ifdef _DEBUG_
    	cout <<m_errorMessage << endl;
    	#endif
    	
    	return ret;
    }
    
    m_callProcMode = 0;
    
    //获取 固定费进程是否调用updateBill的Proc以及帐务优惠的Proc 的标志
    if(getSysParam ("RENTFEE_CallProc_Mode",r_sysParamInfoVector))
    {
    	m_callProcMode = (r_sysParamInfoVector.begin())->m_valueN;
    }
    	
    //初始化子账期数组
	if(initArray()==false)
		return false;
		
    return true;
}

bool PM_Billing::setMdbInfo(const MdbInfo &r_mdbInfo)
{
  if(m_pMdbManager->initialization(r_mdbInfo)==false)
  {
    return false;
  }
  return true;
}

/**
 * getUserInfo:根据用户(群)标识获取用户(群)信息
 * @param  userId         用户(群)标识,也可以是客户标识.
 * @param  userInfo          用户(群)信息.
 * @return   -1表示Mdb异常，0表示没有查找出结果，1 表示查找出结果
 */
int PM_Billing::getUserInfo(const T_SERVER_ID &r_userId,
                         IdInfo      &r_userInfo)
{
    int t_iRet=0;
 
	Mdb_UserInfo t_po(&r_userInfo);
	
	t_po.m_record->m_idAttr[IdInfo::ID_ATTR_ID] = r_userId;
	t_iRet=m_pMdbManager->search<Mdb_UserInfo>(t_po,t_po);
	if(t_iRet<=0) return t_iRet;
		
	//r_userInfo = t_rpo.m_record;
	//获取用户属性列表
	if(getUserParam(r_userId, r_userInfo.m_userParamList)<0)
		return -1;
  	
	return t_iRet;

}     

/**
 * getBillCycle       根据日期取对应的帐期.
 * @param  r_Billday : 日期YYYYMMDD
 * @param  r_billCycle:帐期YYYYMM
 * @return true: 是  false:否
 */
bool PM_Billing::getBillCycle(const int &r_Billday,int &r_billCycle) const
{
  PO_BillingCycle t_po,t_rpo;
  
  t_po.m_record.m_dbNo = m_dbNo;
  t_po.m_record.m_beginDate = r_Billday;
  t_po.m_record.m_endDate   = r_Billday;
  //limeng add 2009-09-21
  t_po.m_record.m_eparchyCode = m_eparchyCode;

  if(m_PO_BillingCycleContainer->search(t_po,t_rpo))
  {
    r_billCycle = (int)(t_rpo.m_record.m_cycleId);
    return true;
  }
  else
  {
    return false;
  }
}

//limeng modified 20090814 改为地市帐期表
bool PM_Billing::getCycleTime(const int &r_cycle,CycParam &r_cycleParam) const
{
  PO_CycleEparchy t_po,t_rpo;

  t_po.m_record.m_dbNo    = m_dbNo;
  t_po.m_record.m_cycleId = r_cycle;
  t_po.m_record.m_eparchyCode = m_eparchyCode;
  if(m_PO_CycleEparchyContainer->search(t_po,t_rpo))
  {
    r_cycleParam = t_rpo.m_record;
    return true;
  }
  return false;
}

bool PM_Billing::getCycleTime(const int &r_cycle,  
                         T_DATE_TIME &r_beginTime,
                         T_DATE_TIME &r_halfTime,
                         T_DATE_TIME &r_endTime) const
{
  CycParam t_cycleParam;
  if(getCycleTime(r_cycle,t_cycleParam)==false)
  {
    return false;
  }    
  r_beginTime = t_cycleParam.m_beginTime;
  r_halfTime  = t_cycleParam.m_halfTime; 
  r_endTime   = t_cycleParam.m_endTime;
  return true;
}

/**
 * getcycPara: 根据传入时间获取账期参数
 * @param  r_scomputeDate    传入时间(YYYYMMDDHHMMSS). 
 * @param  r_cycPara    账期参数. 
 * @return  true 表示查找正常，false 表示查找错误
 */
bool PM_Billing::getcycPara(const T_DATE_TIME &r_scomputeDate,
	                        CycParam &r_cycPara)
{
	int t_billCycle;
	int t_date;
	t_date = r_scomputeDate.value()/1000000;

	if(!getBillCycle(t_date,t_billCycle))
		return false;
	if(!getCycleTime(t_billCycle,r_cycPara))
		return false;
	
	return true;
}

/*
 * 根据进程名和通道号取系统参数
 * @param r_sysParamCode     进程名
 * @param r_channelNo        通道号
 * @param r_sysParam         参数信息
 * @return true 找到, false 未找到
 */           
bool PM_Billing::getSysParam(const T_SYSPARAMCODE &r_sysParamCode,
                          const int            &r_channelNo,
                                ProcSysParam   &r_sysParam)
{
	
  vector<ProcSysParam> sysParamInfoVector;
  vector<ProcSysParam>::iterator iter;
  
  if(getSysParam (r_sysParamCode,sysParamInfoVector))
  {
    for(iter=sysParamInfoVector.begin();
        iter!=sysParamInfoVector.end();++iter)
    {
      if((*iter).m_channelNo == r_channelNo)
      {
        r_sysParam = (*iter);  
        return true;
      } 
    }
  }
  
  return false;
}

/*
 * 根据系统参数取系统参数信息
 * @param r_sysParamCode     系统参数
 * @param r_sysParamInfoVector         系统参数信息
 * @return true 找到, false 未找到
 */
bool PM_Billing::getSysParam (const T_SYSPARAMCODE &r_sysParamCode,
                     vector<ProcSysParam> &r_sysParamInfoVector)
{
    PO_SysParam                    po;
    vector<PO_SysParam>            rposVector;
    vector<PO_SysParam>::iterator  iter;

    r_sysParamInfoVector.clear();

    po.m_record.m_sysParamCode=r_sysParamCode;
    //po.m_record.m_channelNo=0;
    m_PO_SysParamContainer->searchMulti(po,rposVector);
    for ( iter = rposVector.begin(); iter != rposVector.end(); ++iter )
    {
    	if(iter->m_record.m_useTag == "1")
    	{
        	r_sysParamInfoVector.push_back(iter->m_record);
        }
    }

    if ( r_sysParamInfoVector.size() == 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * getChannelNoByUserId:根据用户，地州获取所在通道（根据TD_USER_CHANNEL表）
 * @param  r_userId              用户标示
 * @param  r_eparchyCode            地州标示.
 * @param  r_channelNo            通道.
 * @return  false 表示 不成功 ，true 表示 成功
 */
bool PM_Billing::getChannelNoByUserId(const T_USER_ID &r_userId,
	                                  const T_EPARCHY_CODE &r_eparchyCode,
	                                  T_CHANNEL  &r_channelNo)
{
     PO_UserChannel po;
	 PO_UserChannel* rpo;

 	 po.recordStruct.m_beginPartition = atoi(r_userId.substr(r_userId.length()-4,4).c_str());
 	 po.recordStruct.m_endPartition   = atoi(r_userId.substr(r_userId.length()-4,4).c_str());
     po.recordStruct.m_eparchyCode    = r_eparchyCode;
	 if ( m_PO_UserChannelContainer->search(po, rpo))
	 {
	 	r_channelNo = (*rpo).recordStruct.m_channelNo;
	 	return true;
	 }

   return false;
}

/*
 * 根据用户ID，取对应的服务信息列表
 * @param r_acctId     帐户Id
 * @param r_time       时间
 * @param r_acctInfo   帐户信息
 * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */           
int PM_Billing::getUserServState(const T_SERVER_ID& r_userId, 
                              vector<UserServState>& r_userServStates)
{
  int t_iRet = 0;
  Mdb_UserServState t_po;
  vector<Mdb_UserServState> t_rpos;
  
  r_userServStates.clear();
  
  t_po.m_record.m_userId = r_userId;
  t_iRet=m_pMdbManager->searchMulti<Mdb_UserServState>(t_po,t_rpos);
  if(t_iRet<=0) return t_iRet;
  for(vector<Mdb_UserServState>::iterator t_itr = t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
    r_userServStates.push_back(t_itr->m_record);
  }
  //add  by xuf 2009-8-19 10:34:56
  ::partial_sort(r_userServStates.begin(),r_userServStates.end(),
                 r_userServStates.end());
  return t_iRet;
}

/**
 * getSubCycInfo:根据账期，子账期获取子账期
 * @param  r_cycId              账期标识
 * @param  r_subCycId            子账期标识
 * @param  r_subCycVector            子账期参数集
 * @return  0 表示查找结果为空 ，>0 表示结果记录数
 */
int PM_Billing::getSubCycInfo(const int &r_cycId,const char &r_subCycId,
	                   vector<SubCycPara> &r_subCycVector)
{
	r_subCycVector.clear();
	PO_SubCycle                     po;
    vector<PO_SubCycle*>             rposVector;
    vector<PO_SubCycle*>::iterator   iter;
	po.m_record.m_cycId = r_cycId;
    po.m_record.m_cycTypeCode = r_subCycId;
    m_PO_SubCycleContainer->searchMulti(po,rposVector);
    
    for(iter=rposVector.begin();iter!=rposVector.end();++iter)
    {
    	r_subCycVector.push_back((*iter)->m_record);
    }
    
    return r_subCycVector.size();
}

//add by xuf 2009-8-19 10:37:01
/**
* getMinMaxSubIdFromItem:根据事件，账期最小最大子账期标示
* @param  r_eventTypeId           事件id
* @param  r_cycId                 账期
* @param  r_minSubId              最小子账期
* @param  r_maxSubId              最大子账期
* @return false 表示 不成功 ，true 表示 成功
*/
bool PM_Billing::getMinMaxSubIdFromItem(const T_BIZ_TYPE& r_eventTypeId,const int& r_cycId,
                                int& r_minSubId,int& r_maxSubId)
{   
    vector<SubCycPara> t_subCycVector;
	  vector<SubCycPara>::iterator iter;
	  EventServAndSubCycType   t_eventServAndSubCycType;
	  int t_minSubId,t_maxSubId;
	  char t_subCycType;
	  getSubCycTypeByEventId(r_eventTypeId,t_subCycType);
	  
    if(getSubCycInfo(r_cycId,t_subCycType,t_subCycVector)>0)
    {
    	iter=t_subCycVector.begin();
    	t_minSubId = iter->m_subCycId;
    	t_maxSubId = iter->m_subCycId;
    	
    	for(;iter!=t_subCycVector.end();++iter)
    	{
    		if(iter->m_subCycId>t_maxSubId)
    			t_maxSubId = iter->m_subCycId;
    		if(iter->m_subCycId<t_minSubId)
    			t_minSubId = iter->m_subCycId;
    	}
    	r_minSubId = t_minSubId;
    	r_maxSubId = t_maxSubId;
    	return true;
    }
    
    return false;
}

/**
 * getUserServ:获取用户所有服务
 * @param  r_userId              用户标示
 * @param  r_userServVector            用户服务集.
 * @return -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */
int PM_Billing::getUserServ(const T_USER_ID &r_userId,vector<UserServ> &r_userServVector)
{
	int iRet;
	Mdb_UserServ po;
	vector<Mdb_UserServ> rpoVec;
	vector<Mdb_UserServ>::iterator iter;
	r_userServVector.clear();
	po.m_record.m_userId = r_userId;

	UserServ t_userServ;
	iRet = m_pMdbManager->searchMulti<Mdb_UserServ>(po,rpoVec);

	if(iRet<0)
		return iRet;
	
	for(iter = rpoVec.begin(); iter != rpoVec.end(); ++iter)
	{
		t_userServ.m_servInsId = (*iter).m_record.m_servInsId;
		t_userServ.m_userId = (*iter).m_record.m_userId;
		t_userServ.m_servId = (*iter).m_record.m_servId;
		t_userServ.m_mainTag = (*iter).m_record.m_mainTag;
		t_userServ.m_startDate = (*iter).m_record.m_startDate;
		t_userServ.m_endDate = (*iter).m_record.m_endDate;
		
		t_userServ.m_userServParamList.clear();
		getUserServParam(t_userServ.m_servInsId,t_userServ.m_userServParamList);
		
		r_userServVector.push_back(t_userServ);
	}

	return iRet;
	
}

/**
* getSubCycIdFromDay:从某个日期、帐目，获取该帐目项子帐期
* @param  r_date                 日期YYYYMMDD
* @param  r_itemCode              账目
* @return 子账期标识, 0 表示 没获取
*/
int PM_Billing::getSubCycIdFromDay(const int r_date,const int r_itemCode)
{
	int t_cycId;
	char t_subCycType;//add by xuf 2009-8-19 10:38:05
	map<int,EventServAndSubCycType>::iterator iterE;
	EventServAndSubCycType   t_eventServAndSubCycType;
	vector<SubCycPara> t_subCycVector;
	vector<SubCycPara>::iterator iter;
		
	//获取子账期类型,事件标识
	if(!getEventFromItem(r_itemCode,t_eventServAndSubCycType))
		return 0;
		
	t_subCycType = t_eventServAndSubCycType.m_subCycType;
	
	if(!getBillCycle(r_date,t_cycId))
	{
		return 0;
	}
		
	if(getSubCycInfo(t_cycId,t_subCycType,t_subCycVector)>0)
    {
    	for(iter=t_subCycVector.begin();iter!=t_subCycVector.end();++iter)
    	{
    		if(r_date >= iter->m_startTime && r_date < iter->m_endTime)
    			return iter->m_subCycId;
    	}
    }
	return 0;
}

/**
* getDiscountInfo:根据资费，获取该资费具体信息
* @param  r_discountId                 资费标识
* @param  r_discountInfo              资费信息
* @return false 表示 不成功 ，true 表示 成功
*/
//Modified By Sunam 2009/9/27 12:30:51
//bool PM_Billing::getDiscountInfo(const T_TP_ID &r_discountId,DiscountInfo &r_discountInfo)
bool PM_Billing::getDiscountInfo(const T_TP_ID &r_discountId,vector<DiscountInfo> &r_discountInfo)
{
	PO_Discount t_po;
	//PO_Discount t_rpo;
	vector<PO_Discount> t_rpo;
	
	t_po.m_record.m_discountId = r_discountId;
	//if(m_PO_DiscountContainer->search(t_po,t_rpo))
	if(m_PO_DiscountContainer->searchMulti(t_po,t_rpo))
	{
	    for (int i=0; i<t_rpo.size(); ++i)
	    {
	        r_discountInfo.push_back(t_rpo[i].m_record);
	    }
		return true;
	}
	else
		return false;
	
}

/**
* getDiscountInfo:根据资费，获取该资费具体信息(加时间判断)
* @param  r_discountId                资费标识
* @param  r_beginTime                 开始时间
* @param  r_discountInfo              资费信息
* @return false 表示 不成功 ，true 表示 成功
*/
int  PM_Billing::getDiscountInfo(const T_TP_ID     &r_discountId,
	                               const T_DATE_TIME &r_beginTime,
	                                   DiscountInfo  &r_discountInfo)
{
	PO_Discount t_po;
	vector<PO_Discount *> t_rpo;
	t_po.m_record.m_discountId = r_discountId;
	if(m_PO_DiscountContainer->searchMulti(t_po,t_rpo))
	{
	  for(vector<PO_Discount *>::iterator t_itr = t_rpo.begin();
	      t_itr != t_rpo.end();++t_itr)
	  {
	    if ((*t_itr)->m_record.m_startDate <= r_beginTime &&
	        (*t_itr)->m_record.m_endDate   >= r_beginTime)
      {
        r_discountInfo = (*t_itr)->m_record;
        return 1;
      }
	  }
		return 0;
	}
	else
		return -1;
}

/**
* getDiscountInfo:根据资费，获取该资费具体信息(加时间判断)
* @param  r_discountId                资费标识
* @param  r_beginTime                 开始时间
* @param  r_discountInfo              资费信息
* @return false 表示 不成功 ，true 表示 成功
*/
//Modified By Sunam 2009/9/27 12:34:14
//int  PM_Billing::getDiscountInfo(const T_TP_ID     &r_discountId,const T_DATE_TIME &r_beginTime,DiscountInfo  &r_discountInfo)
int  PM_Billing::getDiscountInfo(const T_TP_ID     &r_discountId,const T_DATE_TIME &r_beginTime,vector<DiscountInfo>  &r_discountInfo)
{
	PO_Discount t_po;
	vector<PO_Discount *> t_rpo;
	t_po.m_record.m_discountId = r_discountId;
	if(m_PO_DiscountContainer->searchMulti(t_po,t_rpo))
	{
	  for(vector<PO_Discount *>::iterator t_itr = t_rpo.begin();
	      t_itr != t_rpo.end();++t_itr)
	  {
	    if ((*t_itr)->m_record.m_startDate <= r_beginTime &&
	        (*t_itr)->m_record.m_endDate   >= r_beginTime)
        {
            //Modified By Sunam 2009/9/27 12:35:09
            /*
              r_discountInfo = (*t_itr)->m_record;
              return 1;
            */
            r_discountInfo.push_back((*t_itr)->m_record);
        }
	  }
	  return r_discountInfo.size();
	}
	else
		return -1;
}

/**
 * getUserImportInfo:根据用户(群)标识获取用户(群)异动信息
 * @param  r_userId         用户(群)标识,也可以是客户标识.
 * @param  r_userInfo          用户(群)信息.
 * @param  r_cycPram          帐期信息.
 * @return   -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
 */
int PM_Billing::getUserImportInfo(const T_SERVER_ID &r_userId,
                      IdInfo      	&r_userInfo,
                      const CycParam &r_cycPram)
{
	//取帐期最后一刻落在表中start_date 和end_date间的数据。
	//只取
	//r_userInfo.m_idAttr[ID_BRAND_CODE]       //品牌  
	//r_userInfo.m_idAttr[ID_PRODUCT_ID]       //产品   
	//r_userInfo.m_idAttr[ID_MSISDN]         //电话号码
	Mdb_UserImportInfo     t_po;
	vector<Mdb_UserImportInfo> t_rpos;
	vector<Mdb_UserImportInfo>::iterator iter;
	int t_iRet=0;
	
	t_po.m_record.m_userId     = r_userId;
	t_iRet=m_pMdbManager->searchMulti<Mdb_UserImportInfo>(t_po,t_rpos);
	
	if(t_iRet<0) return t_iRet;
	
	//add by xuf 2009-10-3 14:41:00
	::sort(t_rpos.begin(),t_rpos.end(), greater<Mdb_UserImportInfo>());	
   	
   	for(iter=t_rpos.begin();iter!=t_rpos.end();++iter)
   	{
   		//Modified by jlxu@neimeng 2009-12-29 15:38:15
   		//#ifdef __PROV010__
   		//if(	iter->m_record.m_startDate < iter->m_record.m_endDate)
   		//{
   		//	r_userInfo.m_idAttr[IdInfo::ID_BRAND_CODE] = iter->m_record.m_brandCode;
   		//	r_userInfo.m_idAttr[IdInfo::ID_PRODUCT_ID] = iter->m_record.m_productId;
   		//	r_userInfo.m_idAttr[IdInfo::ID_MSISDN] = iter->m_record.m_msisdn;
   		//	
   		//	return 1;
   		//}
   		//#else
   		 //Modified by xuf 2009-10-3 14:41:09
   		if(r_cycPram.m_endTime>=iter->m_record.m_startDate && 
   			r_cycPram.m_beginTime<=iter->m_record.m_endDate && 
   			iter->m_record.m_startDate < iter->m_record.m_endDate)
   		{
   			r_userInfo.m_idAttr[IdInfo::ID_BRAND_CODE] = iter->m_record.m_brandCode;
   			r_userInfo.m_idAttr[IdInfo::ID_PRODUCT_ID] = iter->m_record.m_productId;
   			r_userInfo.m_idAttr[IdInfo::ID_MSISDN] = iter->m_record.m_msisdn;
   			
   			return 1;
   		}
   		//#endif
   		//end of modification by jlxu@neimeng 2009-12-29 15:38:34
   	}
   	
   	return 0;
} 

/**
 * getUserInfo:根据用户(群)标识获取用户(群)信息
 * @param  userId         用户(群)标识,也可以是客户标识.
 * @param  userInfo          用户(群)信息.
 * @param  r_cycPram          帐期信息.
 * @return   表示Mdb异常，1 表示查找出结果
 */
int PM_Billing::getUserInfo(const T_SERVER_ID &r_userId,
                			IdInfo      &r_userInfo,
                			const CycParam &r_cycPram)
{
	int iRet;
	iRet = getUserInfo(r_userId,r_userInfo);
	if(iRet > 0)
	{
		//如果资料更新时间>帐期结束时间
		if ( r_userInfo.m_idAttr[IdInfo::ID_UPDATE_TIME] > r_cycPram.m_endTime )
		{
			return getUserImportInfo(r_userId,r_userInfo,r_cycPram);
		}
//		else
//		{
//			return 0;
//		}
	}
	
	return iRet;
}

/**
 * getMinRealCycId:取最小实时帐期信息
 * @param  minRealCycleId  最小实时帐期      .
 * @return  false 表示异常，true 表示正常
*/
//limeng modified 20090814 改为地市帐期表
	bool PM_Billing::getMinRealCycId(int &minRealCycleId)
	{
	  int acycId = 999999;
	
	  vector<PO_CycleEparchy>				vPO_CycleEparchy;
	  vector<PO_CycleEparchy>::iterator	iter;
	
	  m_PO_CycleEparchyContainer->getParamVector(vPO_CycleEparchy);
	
	  for (iter=vPO_CycleEparchy.begin(); iter!=vPO_CycleEparchy.end(); ++iter)
	  {
		  // 实时帐期的 useTag 为 '0'
		  if ((iter->m_record.m_dbNo == m_dbNo) &&
			  (iter->m_record.m_useTag == "0") &&
			  (iter->m_record.m_eparchyCode == m_eparchyCode) &&
			  (iter->m_record.m_cycleId < acycId))
		  {
			  acycId = iter->m_record.m_cycleId;
		  }
	  }
	  minRealCycleId = acycId;
	  if (acycId == 999999)
	  {
		  return false;
	  }
	  else
	  {
		  return true;
	  }
	}

/**
 * getAllUserReAccount:取用户主表所有用户信息
 * @param  reAccountEventVector  所有用户信息
 * @return  -1 表示Mdb异常，0 表示查找结果为空 ，>0 表示结果记录数
*/
int PM_Billing::getAllUserReAccount(vector<ReAccountEventInfo>& reAccountEventVector)
{
	int t_defaultCycId = 0;
    getMinRealCycId(t_defaultCycId);
    CycParam t_cycleParam;
    char t_monthEndTag;
    getCycleTime(t_defaultCycId, t_cycleParam);
    t_monthEndTag = '0';
    if(t_cycleParam.m_addTag>=1)
    {
        t_monthEndTag = '1';
    }
	
	int t_iRet = 0;
	Mdb_ReUserInfo_All t_po;
	vector<Mdb_ReUserInfo_All>::iterator iter;
	vector<Mdb_ReUserInfo_All> t_rpos;
	
	//取所有用户
	t_iRet=m_pMdbManager->searchMulti<Mdb_ReUserInfo_All>(t_po,t_rpos);
	if(t_iRet<0) return -1;

	for(iter=t_rpos.begin();iter!=t_rpos.end();++iter)
	{
		ReAccountEventInfo    reAccountEventInfo;
        reAccountEventInfo.m_userId      = iter->m_record.m_userId;
        reAccountEventInfo.m_msisdn      = iter->m_record.m_msisdn;
        reAccountEventInfo.m_eparchyCode = iter->m_record.m_eparchyCode;
        reAccountEventInfo.m_openDate    = iter->m_record.m_openDate;
        reAccountEventInfo.m_cycId       = t_defaultCycId;
        reAccountEventInfo.m_monthEndTag = t_monthEndTag;
        
        reAccountEventVector.push_back(reAccountEventInfo);
	}
	
	return reAccountEventVector.size();
}
/**
 * getServStatePrior:获取服务状态所在服务的优先级
 * @param  r_iServ              服务
 * @param  r_servState            服务状态.
 * @param  r_prior            优先级.
 * @return  false 表示 不成功 ，true 表示 成功
 */	  	
bool PM_Billing::getServStatePrior(const int r_iServ, const char r_servState, int& r_prior)
{
	map<char,int > t_cimap;
	map<char,int >::iterator iter_ci;
	map<int,map<char,int> >::iterator iter_ici;
	
	iter_ici = m_statePriority.find(r_iServ);
	if(iter_ici == m_statePriority.end())
		return false;
	
	iter_ci = iter_ici->second.find(r_servState);
	if(iter_ci == iter_ici->second.end())
		return false;
	else
	{
		r_prior = iter_ci->second;
		return true;
	}
}

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
bool PM_Billing::getUserServState(const T_USER_ID& r_userId, 
                         const char r_subCycType, const int r_cycId, 
                         const int r_subCycId,  const int r_iServ, 
                         char& r_servStateCode,int& r_oServ )
{
	vector<UserServState> t_userServStateVector;
	vector<UserServState>::iterator iter;
	long t_startDay,t_endDay;
	long t_servEndDay,t_servStartDay;
	int** t_subCycDay;
	map<int,int** >::iterator iterMap;
	vector<UserServState> t_mainservStateVec;
	UserServState t_userServState;
	vector<UserServState>::iterator iter_c;
	int t_mainServ;
	int t_priority = -1;
	int t_priority1;
		
	if(getUserServState(r_userId,t_userServStateVector)<0)
		return false;
	
	//获取子账期开始结束时间
	iterMap = m_subCycStartDayMap.find(r_cycId);
	if(iterMap!=m_subCycStartDayMap.end())
    {	
    	t_subCycDay = iterMap->second;
    	t_startDay = (long)t_subCycDay[r_subCycId][r_subCycType-CYCTYPE_CODE_MIN]*1000000;
    }
    
    iterMap = m_subCycEndDayMap.find(r_cycId);
	if(iterMap!=m_subCycEndDayMap.end())
    {	
    	t_subCycDay = iterMap->second;
    	t_endDay = (long)t_subCycDay[r_subCycId][r_subCycType-CYCTYPE_CODE_MIN]*1000000;
    }


	for(iter=t_userServStateVector.begin();iter!=t_userServStateVector.end();++iter)
	{
		t_servStartDay = (*iter).m_startDate.value();
		t_servEndDay = (*iter).m_endDate.value();
		
		if(t_servStartDay < t_endDay && 
			t_servEndDay > t_startDay)
		{
			if((*iter).m_serviceId == r_iServ)
			{
				if(getServStatePrior(r_iServ,(*iter).m_servStateCode,t_priority1))
				{
					if(t_priority < t_priority1)
					{
						r_servStateCode = (*iter).m_servStateCode;
						r_oServ = (*iter).m_serviceId;
						t_priority = t_priority1;
					}
				}
			}
			if((*iter).m_mainTag == '1')
			{//记录主体服务的状态
				t_mainServ = (*iter).m_serviceId;
				t_userServState.m_serviceId = (*iter).m_serviceId;
				t_userServState.m_servStateCode = (*iter).m_servStateCode;
				t_mainservStateVec.push_back(t_userServState);
			}
		}
	}
	
	if(t_priority != -1)
		return true;
		
	//若没找到时间区间内的服务，则取主体服务状态
	if(!t_mainservStateVec.empty())
	{
		for(iter_c=t_mainservStateVec.begin();iter_c!=t_mainservStateVec.end();++iter_c)
		{
			if(getServStatePrior(iter_c->m_serviceId,iter_c->m_servStateCode,t_priority1))
			{
				if(t_priority < t_priority1)
				{
					r_servStateCode = iter_c->m_servStateCode;
					r_oServ = iter_c->m_serviceId;
					t_priority = t_priority1;
				}
			}
		}
		if(t_priority != -1)
			return true;
	}	
	return false;
}

/**
 * getEventFromItem:根据账目获取事件类型，服务，子账期类型信息
 * @param  r_itemCode                    账目
 * @param  r_eventAndSubCycType          EventServAndSubCycType结构
 * @return true 表示 正常，false 表示 不正常
 */
bool PM_Billing::getEventFromItem(const int r_itemCode, 
	                      EventServAndSubCycType& r_eventAndSubCycType)
{
	//map<int,EventServAndSubCycType>::iterator iterE;
	multimap<int,EventServAndSubCycType>::iterator iterE;//add by xuf 2009-8-19 10:39:29
	char t_subCycType;
	
	//获取子账期类型,事件标识
	iterE = m_itemToEventMap.find(r_itemCode);
	if(iterE==m_itemToEventMap.end())
		return false;
	r_eventAndSubCycType.m_eventId = iterE->second.m_eventId;
	r_eventAndSubCycType.m_servId = iterE->second.m_servId;
	r_eventAndSubCycType.m_subCycType = iterE->second.m_subCycType;
	
	return true;
}

//add by xuf 2009-8-19 10:40:45
/** add by zhouq 
 * getEventFromItem:根据账目获取事件类型，服务，子账期类型信息
 * @param  r_itemCode                    账目
 * @param  vector<r_eventAndSubCycType>    EventServAndSubCycType结构
 * @return true 表示 正常，false 表示 不正常
 */
bool PM_Billing::getEventFromItem(const int r_itemCode, 
	                      vector<EventServAndSubCycType>& v_eventAndSubCycType)
{
	multimap<int,EventServAndSubCycType>::iterator iterE;
	EventServAndSubCycType r_eventAndSubCycType;
	char t_subCycType;
	v_eventAndSubCycType.clear();
	//获取子账期类型,事件标识
	iterE = m_itemToEventMap.lower_bound(r_itemCode);
	while (iterE != m_itemToEventMap.upper_bound(r_itemCode))
  {
     r_eventAndSubCycType.m_eventId = iterE->second.m_eventId;
	   r_eventAndSubCycType.m_servId = iterE->second.m_servId;
     r_eventAndSubCycType.m_subCycType = iterE->second.m_subCycType;
     v_eventAndSubCycType.push_back(r_eventAndSubCycType);
     ++iterE;
  }
  if(v_eventAndSubCycType.empty())
  {
  	return false;
  }
	return true;
}

void PM_Billing::getSubCycStartDay(CycParam& r_cyclePara,int** t_subCycStartDay)
{
	int halfDays,inDays;
	int intMonth = r_cyclePara.m_cycleId;
	int intMonth100=intMonth*100;  
	halfDays=atoi(((r_cyclePara.m_halfTime).substr(0,8)).c_str());
	
	
	//日收取周期类型
	
	for(int i=1;i<=31;i++)
	{ 
		inDays=intMonth100+i;
		if(i<=28)
		{
			t_subCycStartDay[i][DAY_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=inDays;
		}
		else
		{
			if(validDate(inDays))  
			{
				t_subCycStartDay[i][DAY_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=inDays;
			}
		}
	}           
	
	//周收取周期类型,星期一为1
	int j=1;
	for(int i=1;i<=31;i++)
	{		
		inDays=intMonth100+i;
		if(getweek(inDays)==1)  
		{
			t_subCycStartDay[j][WEEK_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=inDays;
			j++;
		}             
	}       
	
	//旬收取周期类型
	t_subCycStartDay[1][TENDAY_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=intMonth100+1;       
	t_subCycStartDay[2][TENDAY_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=intMonth100+11;  
	t_subCycStartDay[3][TENDAY_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=intMonth100+21;         
	
	//半月收取周期类型
	t_subCycStartDay[1][HALFMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=intMonth100+1;
	t_subCycStartDay[2][HALFMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=halfDays;  
	
	//  月收取周期类型
	t_subCycStartDay[1][MONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=intMonth100+1;
	
	
	//对于大于一个月子帐期,delayDays就是偏移月数
	//双月收取周期类型
	t_subCycStartDay[1][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;
	t_subCycStartDay[2][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+3)*100+1;
	t_subCycStartDay[3][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+5)*100+1;
	t_subCycStartDay[4][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+7)*100+1;
	t_subCycStartDay[5][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+9)*100+1;
	t_subCycStartDay[6][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+11)*100+1;
	
	//季度收取周期类型
	t_subCycStartDay[1][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;       
	t_subCycStartDay[2][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+4)*100+1;
	t_subCycStartDay[3][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+7)*100+1;
	t_subCycStartDay[4][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+10)*100+1;
	
	
	//半年收取周期类型
	t_subCycStartDay[1][HALFYEAR_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;       
	t_subCycStartDay[2][HALFYEAR_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+7)*100+1;
	
	//年收取周期类型
	t_subCycStartDay[1][YEAR_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;       
	
	//调用数据库验证方法，获取是否到计算时间....
	/*
	m_rentFeeParam->getNatureSubCycStartDay();
	*/                         
	

}

void PM_Billing:: getSubCycEndDay(CycParam& r_cyclePara,int** r_subCycEndDay)
{
	int halfDays,inDays;
	int intMonth = r_cyclePara.ycStartDay[5][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+9)*100+1;
	t_subCycStartDay[6][TWOMONTH_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+11)*100+1;
	
	//瀛ｅ害鏀跺彇鍛ㄦ湡绫诲瀷
	t_subCycStartDay[1][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;       
	t_subCycStartDay[2][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+4)*100+1;
	t_subCycStartDay[3][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+7)*100+1;
	t_subCycStartDay[4][QUARTER_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+10)*100+1;
	
	
	//鍗婂勾鏀跺彇鍛ㄦ湡绫诲瀷
	t_subCycStartDay[1][HALFYEAR_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+1)*100+1;       
	t_subCycStartDay[2][HALFYEAR_CYCTYPE_CODE-CYCTYPE_CODE_MIN]=((intMonth/100)*100+7)*100+1;
	
	//骞存敹鍙栧懆鏈熺被鍨