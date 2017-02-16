#include "CondRuleExists.h"
#include "PolicyInfo.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h" 
                                                                     
FuncJudgeExists CondRuleExists::m_funcJudgeExists[] =
{  
	CondRuleExists::judgeExistsTp            ,    //存在tpID            
	CondRuleExists::judgeExistsState         ,    //停机状态持续了n个月 
	CondRuleExists::judgeExistsStateCurMonth    ,  //当月状态   
	CondRuleExists::judgeExistsDealedRateTp  , // 是否已经作用过批价政策
	CondRuleExists::judgeExistsServCurMonth	,			//是否享受过某服务
	CondRuleExists::judgeExistsAreaType,         //数据业务区内区间属性
	CondRuleExists::judgeExistsOpenDateTpStartDate,  //资费起始时间等于用户开户时间，内蒙G网同步 	
 //add by yuxk for shanxi begin
  CondRuleExists::judgeExistsServStartFirstHalf,   //服务开始时间是当前帐期上半月
  CondRuleExists::judgeExistsServStartLastHalf,   //服务开始时间是当前帐期下半月 
  CondRuleExists::judgeExistsServEndFirstHalf,   //服务结束时间是当前帐期上半月  
  CondRuleExists::judgeExistsServEndLastHalf,    //服务结束时间是当前帐期下半月 
  CondRuleExists::judgeServHalfMonth   //服务时间和停保停保复话时间组合判断是哪个半月
 //add by yuxk for shanxi end
};

//操作符号   exists
bool CondRuleExists::execute()
{
	int key = atoi(m_minValue);
	if (key<0 || key>=TAG_EXISTS_TYPE_MAX)
	{
		THROW_C_P2(BusinessException, -1, "CondRuleExists错误,非法的下标值!"
    	"cond_id=%d,minValue=%s", (long)m_id, m_minValue);		
	}
	if(m_yesOrNo==TAG_YESORNO_YES)
	return m_funcJudgeExists[key](*this);	
	  
	return !m_funcJudgeExists[key](*this);	
}	

//对优惠进行判断
bool CondRuleExists::judgeExistsTp(CondRuleExists &condRuleExists)
{
	const char *pEnumValue = condRuleExists.getEnumValue();
	vector<T_STR_MIN>::iterator iterVector;
	vector<RateTpInfo>::iterator	iterTp;
	map<T_STR_MIN,T_STR_MIN>::iterator iterMap;
	//add by xuf 2009-11-30 14:54:29
	T_STR_MIN  t_tpId;
	bool t_bRet = false;
		
	if(pEnumValue!=NULL)
	{
		condRuleExists.splitEnumValue(pEnumValue, 
		condRuleExists.m_enumVector, condRuleExists.m_enumMap);
	}
	
	//如果是群总量优惠，判断虚拟用户上的资费ID add by xuf 2009-11-30 15:39:41
  if(m_globalInfo->m_cdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GRPBILL)
  {
  	vector<Discount>::iterator	iterDist;  	
  	for (iterDist=m_globalInfo->m_comBill.m_userGrpDiscountVector.begin(); 
       iterDist!=m_globalInfo->m_comBill.m_userGrpDiscountVector.end(); ++iterDist)
	{    
		//判资费实例表中的资费ID add by xuf 2009-11-30 14:54:38 
		t_tpId = (long)iterDist->m_rateTpInfo.m_pUserTp->m_tpId;
		t_bRet=binary_search(condRuleExists.m_enumVector.begin(),
		              condRuleExists.m_enumVector.end(),t_tpId);
		if(t_bRet) 
		{
			return true;
		}
				
		for(iterMap=condRuleExists.m_enumMap.begin();
		    iterMap!=condRuleExists.m_enumMap.end();++iterMap)
		{
		  	if( t_tpId.value()>= iterMap->first.value() &&
		  	    t_tpId.value()<= iterMap->second.value() )
			   return true;
		}
	
	}	
	
	return false;
  }//if(m_globalInfo->m_cdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GRPBILL)	
			
	for (iterTp=m_globalInfo->m_tpInfoList.begin(); 
       iterTp!=m_globalInfo->m_tpInfoList.end(); ++iterTp)
	{
		if(iterTp->m_pUserTp->m_tpObjId==0) //不判系统级资费 add by xuf 2009-11-12 19:01:47
			continue;

//继承型资费也需要判断 add by xuf 2009-11-30 15:31:47			
//	  if(iterTp->isValid(m_globalInfo->m_cycParam.m_endTime,
//	                     m_globalInfo->m_cycParam.m_endTime) &&
//       iterTp->m_pUserTp->m_userId == m_globalInfo->m_userId)
  	if(iterTp->isValid(m_globalInfo->m_cycParam.m_endTime,
	                     m_globalInfo->m_cycParam.m_endTime))     
	{
		//判实例表中的资费ID add by xuf 2009-11-30 14:54:38
		t_tpId = (long)iterTp->m_pUserTp->m_tpId;
		t_bRet=binary_search(condRuleExists.m_enumVector.begin(),
		              condRuleExists.m_enumVector.end(),t_tpId);
		if(t_bRet) 
		{
			return true;
		}
		
//		for(iterVector=condRuleExists.m_enumVector.begin();
//		    iterVector!=condRuleExists.m_enumVector.end();
//		    ++iterVector)
//		{
//		  	if ( (long) iterTp->m_rateTpId == (*iterVector).value())
//			   return true;
//		}
		
		for(iterMap=condRuleExists.m_enumMap.begin();
		    iterMap!=condRuleExists.m_enumMap.end();++iterMap)
		{
		  	if( t_tpId.value()>= iterMap->first.value() &&
		  	    t_tpId.value()<= iterMap->second.value() )
			   return true;
		}
	}
	}
	
	return false;
}

//对状态进行判断
bool CondRuleExists::judgeExistsState(CondRuleExists &condRuleExists)
{
	long durAcycNum;
	T_OBJECT_ATTR stateSet;
	stateSet = condRuleExists.getEnumValue();
	if (stateSet.empty())
		stateSet = condRuleExists.m_enumValue;
		
	condRuleExists.getMaxValue(durAcycNum);
	
	return m_globalInfo->m_comBill.judgeStateEffect(
	       m_globalInfo->m_cycParam, stateSet, durAcycNum);
}


//资费起始时间等于用户开户时间	
bool CondRuleExists::judgeExistsOpenDateTpStartDate(CondRuleExists &condRuleExists)
{
	T_OBJECT_ATTR openDate,tpStartDate;
	openDate=m_globalInfo->m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE];
	
	tpStartDate=NodeBase::m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
	
	if(openDate.substr(0,8)==tpStartDate.substr(0,8))
		{
			return true;
		}
	else
		{
			return false;
		}
		return false;			
}

//对当月状态进行判断
bool CondRuleExists::judgeExistsStateCurMonth(CondRuleExists &condRuleExists)
{
	//当月存在过某个状态
	const char *pStateSet=condRuleExists.getEnumValue();
	const char *pChar;
	int i;
	char servState;
	T_OBJECT_ATTR stateSet;
	
	//取时间范围标志 0：全月，1：上半月，2：下半月 ,3: 1日  add by xuf 2010-1-14 23:04:42
	long t_timeRangeFlag=0;
	condRuleExists.getMaxValue(t_timeRangeFlag);
	
	m_globalInfo->m_comBill.getCurStateSet(stateSet,t_timeRangeFlag);	
	if(pStateSet==NULL)
	{
		pStateSet = condRuleExists.m_enumValue;
	}
	
	for(i=0;i<stateSet.length();++i)
	{
		pChar = pStateSet;		
		while( (servState=(*pChar)) != '\0') 
		{
			if(servState==stateSet[i])
				return true;
			++pChar;
		}
	}
	
	return  false;
}

// add by chenm for 山西  VPN 强制组间通话资费
 bool CondRuleExists::judgeExistsDealedRateTp(CondRuleExists &condRuleExists)
 {
	vector<T_STR_MIN>::iterator iterVector;
	vector<RateTpInfo>::iterator	iterTp;

	for (iterTp=m_globalInfo->m_tpInfoList.begin(); 
       	iterTp!=m_globalInfo->m_tpInfoList.end(); 
		++iterTp)
	{
		for(iterVector=condRuleExists.m_enumVector.begin();
			      iterVector!=condRuleExists.m_enumVector.end();
			      ++iterVector)
		{
			// 如果某个批价政策已经成功匹配，并作用过了，返回真
			 if ( (long) iterTp->m_rateTpId == (*iterVector).value()
			 	&&  iterTp->m_isDealed == true)  
			  	   return true;
		}
       }
	return  false;
 }

bool CondRuleExists::judgeExistsServCurMonth(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	//取时间范围标志 0：全月，1：上半月，2：下半月 ,3: 1日 add by xuf 2010-1-14 23:04:42
	long t_timeRangeFlag=0;
	condRuleExists.getMaxValue(t_timeRangeFlag);
	
	int iServId = atoi(servId.c_str());
	
	return m_globalInfo->m_comBill.judgeServExists(iServId,t_timeRangeFlag);
}

bool CondRuleExists::judgeExistsAreaType(CondRuleExists &condRuleExists)
{  
  T_USER_ID r_userIdO;
  T_USER_ID r_userIdN=m_globalInfo->m_comBill.m_userId;

  if(m_globalInfo->m_comBill.m_pAccountParam->getOppositeUserIdByNativeUserId(r_userIdN,"0",
                                  m_globalInfo->m_comBill.m_cycParam,r_userIdO)!=-1)
  {                                
     #ifdef _DEBUG_
     cout<<__FILE__<<":"<<__LINE__<<"获取用户:"<<r_userIdN<<"的对端用户错误！"<<endl;
     #endif 
  }
  IdInfo userInfoN,userInfoO;
  m_globalInfo->m_comBill.m_pAccountParam->getUserInfo(r_userIdN,userInfoN);
  if(m_globalInfo->m_comBill.m_pAccountParam->getUserInfo(r_userIdO,userInfoO)==0)//modify by fug 20091225
   {
   	userInfoO.m_idAttr[IdInfo::ID_CITY_CODE]="0040";
   }
  if(userInfoN.m_idAttr[IdInfo::ID_CITY_CODE] == userInfoO.m_idAttr[IdInfo::ID_CITY_CODE])
  	return true;
  if(m_globalInfo->m_comBill.m_pAccountParam->searchMultiSectionRelFix(userInfoN,userInfoO) == 0)
  	return true;
	return false;
}
 //add by yuxk for shanxi begin
bool CondRuleExists::judgeExistsServStartFirstHalf(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	int iServId = atoi(servId.c_str());
	//return m_globalInfo->m_comBill.judgeServStartFirstHalf(iServId);
	return m_globalInfo->m_comBill.judgeServStart(iServId,m_globalInfo->m_cycParam.m_beginTime,m_globalInfo->m_cycParam.m_halfTime);
	
  	}

  bool CondRuleExists::judgeExistsServStartLastHalf(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	int iServId = atoi(servId.c_str());
	//return m_globalInfo->m_comBill.judgeServStartLastHalf(iServId);
	return m_globalInfo->m_comBill.judgeServStart(iServId,m_globalInfo->m_cycParam.m_halfTime,m_globalInfo->m_cycParam.m_endTime);
  	}

  bool CondRuleExists::judgeExistsServEndFirstHalf(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	int iServId = atoi(servId.c_str());
	//return m_globalInfo->m_comBill.judgeServEndFirstHalf(iServId);
	return m_globalInfo->m_comBill.judgeServEnd(iServId,m_globalInfo->m_cycParam.m_beginTime,m_globalInfo->m_cycParam.m_halfTime);
  	}

  bool CondRuleExists::judgeExistsServEndLastHalf(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	int iServId = atoi(servId.c_str());
	//return m_globalInfo->m_comBill.judgeServEndLastHalf(iServId);
	return m_globalInfo->m_comBill.judgeServEnd(iServId,m_globalInfo->m_cycParam.m_halfTime,m_globalInfo->m_cycParam.m_endTime);
  	}

bool CondRuleExists::judgeServHalfMonth(CondRuleExists &condRuleExists)
{
	string servId = condRuleExists.getEnumValue();
	if (servId.empty())
		servId = condRuleExists.m_enumValue;
	
	//取时间范围标志0：服务开始在当月，1：服务开始在上半月，2：服务开始在下半月，3：服务结束在上半月，4: 服务结束在下半月，5: 服务结束在当月，
	long t_timeRangeFlag=0;
	condRuleExists.getMaxValue(t_timeRangeFlag);
	
	int iServId = atoi(servId.c_str());
	
	return m_globalInfo->m_comBill.judgeServHalfMonth(iServId,t_timeRangeFlag);
}


 //add by yuxk for shanxi end


const char * CondRuleExists::getEnumValue()
{
	if(m_enumValue[0] == '?') //需要替换参数
	{	  
		T_TP_ID r_paramId;
		const char *paramValue;
    	
		r_paramId = &m_enumValue[1];
		paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
		if (paramValue == NULL)
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "cond_id=%d,m_minValue=%s",
			                                   (long)m_id, m_minValue);
		}
		return paramValue;
	}
	else if (strlen(m_enumValue)>3&&strncmp(m_enumValue, "EXP",3)==0) //表达式
	{
		char *expr;
		expr = &m_enumValue[3];
		long  value=0;
		if (!m_globalInfo->explanExpr(value,expr,m_policyInfo.m_rateTpInfo))
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
			                                  "cond_id=%d,m_enumValue=%s",(long)m_id, m_enumValue);
		}
    
		char t_str[MIN_STR_LENGTH];
		sprintf(t_str,"%ld",value);
		const char *paramValue = t_str;
		
		return paramValue;
	}
	else if (strlen(m_enumValue)>1&&m_enumValue[0] == '#') //用户属性USER_ITEM
	{
		T_TP_ID r_paramId;
		const char *paramValue;
		r_paramId = &m_enumValue[1];		
		m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,
		                          m_globalInfo->m_cycParam.m_endTime,
		                          m_policyInfo.m_rateTpInfo->m_eventType);
		paramValue = m_globalInfo->m_idInfo.getParamValue(r_paramId);
		if (paramValue == NULL)
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "cond_id=%d,m_enumValue=%s",
			                                   (long)m_id, m_enumValue);
		}
		return 	paramValue;
	}
	else
		return NULL;
}

void CondRuleExists::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	
	strcat(myPrefix,prefix);	
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- CondRuleExists : " <<m_id<<" ----" <<endl;	
	CondRule::dump(myPrefix);
	cout <<myPrefix<<"m_enumValue   = "<< m_enumValue  <<endl;
	cout <<myPrefix<<"}" <<endl;
}
