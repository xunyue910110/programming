#include "CondRule.h"
#include "ObjectBase.h"
#include "ObjectAddup.h"
#include "ObjectBill.h"
#include "PolicyInfo.h"
#include "AddupItem.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"
#include "PMManager.h"
#include "NodeBase.h"
#include "ObjectValue.h"
#include "ObjectParam.h"
#include "ObjectUserParam.h"


CondRule::CondRule(){}
CondRule::~CondRule(){}

bool CondRule::judge(vector<CdrSeg> *r_pCdrSegList,
	                   vector<CdrSeg> *r_pSepCdrSegList,
	                   bool           &r_splitFlag,
	                   SumTpRule      &r_sumRuleInfo)
{	
	//add by gaojf 2008-11-12 10:06 begin
	r_splitFlag = false;
	r_pSepCdrSegList->clear();
	
	//add by gaojf 2008-11-12 10:06 end
	if (m_diviedTag==TAG_DIVIED_NO) //话单不分段
	{
		return execute();
	}else if(m_diviedTag==TAG_DIVIED_BILL) //账单总量分段
	{
	  SumRule  t_billSumRule;
	  UserBill t_userBill;
	  const UserBill * t_pUserBill;
	  RateGlobalInfo *t_pGlobalInfo = NodeBase::m_globalInfo;
	  ObjectBill *t_pBillObj = dynamic_cast<ObjectBill *>(m_judgeObj);
	  t_billSumRule.m_billId.m_tpObjId = 0;
	  t_billSumRule.m_billId.m_groupId = 0;
    //帐目类型：组合or明细
    t_billSumRule.m_billId.m_userId = t_pGlobalInfo->m_billUserId;
    t_billSumRule.m_billId.m_sumBeginTime=t_pGlobalInfo->m_cycParam.m_beginTime.value();
    t_billSumRule.m_billId.m_sumEndTime=t_pGlobalInfo->m_cycParam.m_endTime.value();
    t_billSumRule.m_billId.m_billType = BILL_TYPE_USER;
	  t_billSumRule.m_billId.m_billId = t_pBillObj->m_attrId;
	  t_billSumRule.m_billId.m_type = t_pBillObj->m_itemType - '0'; 
	  t_userBill.m_userId = t_billSumRule.m_billId.m_userId;
	  //t_userBill.m_billId = t_billSumRule.m_billId.m_billId;
	  //if(t_pGlobalInfo->getBillItem<UserBill>(t_pGlobalInfo->m_userBills_new,
	  //                  t_userBill,t_pUserBill)==true)
	  //{
	  //  t_billSumRule.m_curValue = t_pUserBill->m_value;
	  //}else
	  //{
	  //  t_billSumRule.m_curValue = 0;
	  //}
	  t_billSumRule.m_curValue = 0;
	  IntMap & t_dtlItemMap = t_pGlobalInfo->m_paramMgr->getCompositItemMgr()->
	                          find(t_billSumRule.m_billId.m_billId);
      for(map<int,int>::const_iterator t_mapItr = t_dtlItemMap.begin();
          t_mapItr!=t_dtlItemMap.end();++t_mapItr)
      {
        t_userBill.m_billId = t_mapItr->first;
        if(t_pGlobalInfo->getBillItem<UserBill>(t_pGlobalInfo->m_userBills_new,
	                      t_userBill,t_pUserBill)==true)
	      {
	        t_billSumRule.m_curValue += t_pUserBill->m_value;
	      }
      }
      
	  getMinValue(t_billSumRule.m_minValue);
		getMaxValue(t_billSumRule.m_maxValue);		
		if (t_billSumRule.m_curValue>=t_billSumRule.m_maxValue)
		{
			return false;
		}
		r_sumRuleInfo.m_ruleList.push_back(t_billSumRule);
		return true;
	}
	else if (m_diviedTag==TAG_DIVIED_ADDUP)//累计量分段
	{
		SumRule tempSumRule;
		ObjectAddup *pAddupObj = dynamic_cast<ObjectAddup *>(m_judgeObj);
		set<RateBillId> t_rateBillIdSet;
		pair<set<RateBillId>::iterator, bool> t_result;
	    SumRule lastTempSumRule;
	    bool isFirstFlag = true;
		if (pAddupObj==NULL)
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,累计量分段时，参考对象不是累计量对象或为空：cond_id=%d,divied_tag=%c", (long)m_id,m_diviedTag);	
		}
		else
		{
			T_DATE_TIME r_cdrStartDateTime;
			for (int seg=0; seg < r_pCdrSegList->size(); ++seg)
			{
				r_cdrStartDateTime = (*r_pCdrSegList)[seg].m_startDateTime.getDateTime();
				//如果不是第一个分段，且当前分段的时间在上一个累计量周期内则不再查找累计量信息
				if (!isFirstFlag)
				{
					isFirstFlag = false;
					if (r_cdrStartDateTime >= lastTempSumRule.m_billId.m_sumBeginTime &&
						r_cdrStartDateTime <= lastTempSumRule.m_billId.m_sumEndTime)
					{
						continue;
					}
				}
				if (pAddupObj->m_addupItem != NULL)
				{
					if (! pAddupObj->m_addupItem->getRateBillIdInfo(m_globalInfo,m_policyInfo.m_rateTpInfo,r_cdrStartDateTime,&tempSumRule.m_billId) )
        			{
        			   	THROW_C_P3(BusinessException, -1, "条件配置错误,"
						   		"累计量指针调用getRateBillIdInfo()失败："
						   		"cond_id=%d,obj_id=%d,addupItemId=%d", 
						   		(long)m_id,pAddupObj->m_id,pAddupObj->m_addupItem->m_id);
        			}
        			t_result = t_rateBillIdSet.insert(tempSumRule.m_billId);
        			//已经存在
        			if(!t_result.second)
					{
						continue;
					}
					lastTempSumRule = tempSumRule;
            	
        			if (! pAddupObj->m_addupItem->getAddupValue(m_globalInfo,
        		                           m_policyInfo.m_rateTpInfo, 
        		                           tempSumRule.m_billId.m_sumBeginTime,
        		                           tempSumRule.m_billId.m_sumEndTime, 
        		                           tempSumRule.m_curValue) )
					{
						THROW_C_P2(BusinessException, -1, "条件配置错误,getAddupValue return false"
					   "cond_id=%d,attr_Id=%d", (long)m_id,pAddupObj->m_attrId);	
					}	
				}
					
				getMinValue(tempSumRule.m_minValue);
				getMaxValue(tempSumRule.m_maxValue);		
				//add by yangyb 2010-07-26 start
        vector<CdrSeg>::iterator iterCdrSegtmp;
        char computOper = m_valueParam[0];
        long Sumdata=0,Sumduration=0;
        long dividedValue = atol (&m_valueParam[1]);
        for (iterCdrSegtmp=r_pCdrSegList->begin();
             iterCdrSegtmp!=r_pCdrSegList->end();++iterCdrSegtmp)
        {
                Sumdata+=iterCdrSegtmp->m_data;
        }
        if (computOper == '/')
        {
          if((tempSumRule.m_curValue+Sumdata)>=dividedValue)//倍数累计
          {
                getMinValue(tempSumRule.m_minValue);
                        getMaxValue(tempSumRule.m_maxValue);
                r_sumRuleInfo.m_ruleList.push_back(tempSumRule);
                return true;
           }
           return false;
        }
        //add by yangyb 2010-07-26 end
        		if(m_judgeMode == TAG_JUDGE_MODE_SUMOPENDATE )
        		{
        		  tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_minValue,
        		                           RENT_COMPMETHOD_OPENDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		  tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_maxValue,
        		                           RENT_COMPMETHOD_OPENDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		}
        		//根据套餐日期折算上下限比较  
        		else if(m_judgeMode == TAG_JUDGE_MODE_SUMTPDATE )
        		{
        		  tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_minValue,
        		                           RENT_COMPMETHOD_TPDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		  tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_maxValue,
        		                           RENT_COMPMETHOD_TPDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		}
        		//Added By Sunam 2009/11/10 10:25:08 按当月最早开机时间折算赠送时长
        		else if(m_judgeMode == TAG_JUDGE_MODE_SUMEARLIESTOPENDATE )
        		{
        		  tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_minValue,
        		                           RENT_COMPMETHOD_EARLIESTOPENDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		  tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_maxValue,
        		                           RENT_COMPMETHOD_EARLIESTOPENDATE,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		}
        		else if(m_judgeMode == TAG_JUDGE_MODE_HALFMONTH)
        		{
        			tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_minValue,
        		                           RENT_COMPMETHOD_HALF,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		  tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_maxValue,
        		                           RENT_COMPMETHOD_HALF,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str(),
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
										   m_valueParam);
        		}
        		//Added By Sunam 2009/12/24 13:47:32
                else if(m_judgeMode == TAG_JUDGE_MODE_SUMSPEACIALDATE )
                {
                	T_DATE_TIME t_maxDate;
                	if (m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE] > m_globalInfo->m_firstCallTime)
                	{
                		if (m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE] > m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime)
                		{
                			t_maxDate = m_globalInfo->m_pCdr[ATTRI_EARLIEST_OPEN_DATE];
                		}
                		else
		        		{
		        			t_maxDate = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
		        		}
                	}
                	else
		        	{
		        		if (m_globalInfo->m_firstCallTime > m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime)
                		{
                			t_maxDate = m_globalInfo->m_firstCallTime;
                		}
                		else
		        		{
		        			t_maxDate = m_policyInfo.m_rateTpInfo->m_pUserTp->m_beginTime;
		        		}
		        	}
                		        
                    tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
                                             tempSumRule.m_minValue,
                                             RENT_COMPMETHOD_SPEACIALDATE,
                                             m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
                                             m_globalInfo->m_firstCallTime,
                                             m_globalInfo->m_cycDays,
                                             t_maxDate,
                                             m_globalInfo->m_pSysCycParam->m_cycleId,
										     m_valueParam);
                    tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
                                             tempSumRule.m_maxValue,
                                             RENT_COMPMETHOD_SPEACIALDATE,
                                             m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
                                             m_globalInfo->m_firstCallTime,
                                             m_globalInfo->m_cycDays,
                                             t_maxDate,
                                             m_globalInfo->m_pSysCycParam->m_cycleId,
										     m_valueParam);
                }
        		//Added By Suanm 2009/10/6 20:07:26
        		//非月累计量折算
        		else if(m_judgeMode == TAG_JUDGE_MODE_SUM2 )
        		{
        		  tempSumRule.m_minValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_minValue,
        		                           RENT_COMPMETHOD_SUM2,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_beginDateTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
        		                           m_valueParam);
        		  tempSumRule.m_maxValue = m_policyInfo.m_rateTpInfo->getValue(
        		                           tempSumRule.m_maxValue,
        		                           RENT_COMPMETHOD_SUM2,
        		                           m_globalInfo->m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
        		                           m_globalInfo->m_firstCallTime,
        		                           m_globalInfo->m_beginDateTime,
        		                           m_globalInfo->m_cycDays,
        		                           m_globalInfo->m_pSysCycParam->m_cycleId,
        		                           m_valueParam);
        		}
        		//////////////2009-2-18 11:24////////////////////////////
				
				//如果是累计量1只查找一次，并判断返回
				if (tempSumRule.m_billId.m_billType == BILL_TYPE_SUM1)
				{
					if (tempSumRule.m_curValue>=tempSumRule.m_maxValue)
				    {
				    	return false;
				    }
				    r_sumRuleInfo.m_ruleList.push_back(tempSumRule);
				    return true;
				}
				//如果是累计量2，保存当前结果，继续查找下一个分段，
				/*即使当前值已经超过最大值，仍然要保留，到EffectFeeCount::sumTpProcess()中去处理
				  因为一条话单的两个分段可能属于两个不同的周期，会存在一个周期超出，另外一个周期
				  还未使用完的情况
				*/
				else
				{
					r_sumRuleInfo.m_ruleList.push_back(tempSumRule);
				}
			}
			if (r_sumRuleInfo.m_ruleList.size() > 0)
			{
				return true;
			}
			return false;
		}
	}else//非累计量分段
	{
		r_splitFlag = false;
		r_pSepCdrSegList->clear();
		
		CdrSeg tempSeg;
		vector<CdrSeg>::iterator iterCdrSeg;
		
		if (m_diviedTag==TAG_DIVIED_DURATION)//按时长
		{
			long startSecond, endSecond;
			getMinValue(startSecond);
			getMaxValue(endSecond);
			for (iterCdrSeg=r_pCdrSegList->begin(); 
			     iterCdrSeg!=r_pCdrSegList->end();++iterCdrSeg)
			{
				if (iterCdrSeg->split(startSecond, endSecond, tempSeg))
				{
					r_pSepCdrSegList->push_back(tempSeg);
					r_splitFlag = true;			
				}
			}			
		}else if(m_diviedTag==TAG_DIVIED_DATA)//流量分割
		{
			long t_startData, t_endData;
			getMinValue(t_startData);
			getMaxValue(t_endData);
			for (iterCdrSeg=r_pCdrSegList->begin(); 
			     iterCdrSeg!=r_pCdrSegList->end();++iterCdrSeg)
			{
				if (iterCdrSeg->split_data(t_startData, t_endData, tempSeg))
				{
					r_pSepCdrSegList->push_back(tempSeg);
					r_splitFlag = true;			
				}
			}			
		}else if (m_diviedTag==TAG_DIVIED_BUSY)//忙闲时
		{
			long startTime, endTime;
			DateTime startDateTime, endDateTime;
			Integer curDate;
			getMinValue(startTime);
			getMaxValue(endTime);
			
			if (endTime<startTime) //忙时时段是跨天的,假设忙时时段为 21:00 - 07:00
			{
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			         iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{
					curDate = (*iterCdrSeg).m_startDateTime.getDateInteger();
					//切割 0点-7点的话单
					startDateTime.set(curDate,0);
					endDateTime.set(curDate,endTime);
					
					if ((*iterCdrSeg).split(startDateTime, endDateTime, tempSeg))
					{
						(*r_pSepCdrSegList).push_back(tempSeg);
						r_splitFlag = true;			
					}
					
					//切割 21点-次日0点的话单
					startDateTime.set(curDate,(Integer) startTime);
					endDateTime.nextday();//取第二天的0点
					
					if ((*iterCdrSeg).split(startDateTime, endDateTime, tempSeg))
					{
						(*r_pSepCdrSegList).push_back(tempSeg);
						r_splitFlag = true;			
					}
				}
			}
			else
			{
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			         iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{
					curDate = (*iterCdrSeg).m_startDateTime.getDateInteger();
					startDateTime.set(curDate,(Integer) startTime);
					endDateTime.set(curDate,(Integer) endTime);
					
					if ((*iterCdrSeg).split(startDateTime, endDateTime, tempSeg))
					{
						(*r_pSepCdrSegList).push_back(tempSeg);
						r_splitFlag = true;			
					}
				}
			}				
		}
		//Modified By Sunam 2009/8/13 12:30:05
		//else if (m_diviedTag==TAG_DIVIED_HOLIDAY)//节假日
		else if (m_diviedTag==TAG_DIVIED_DAY)//按天分段
		{
			long holiday;
			if (m_judgeMode == TAG_JUDGE_MODE_IN)//离散值
			{				
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			         iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{
					holiday = (long) (*iterCdrSeg).m_startDateTime.getDateInteger();
					if (judgeCond(holiday))					
					{
						(*r_pSepCdrSegList).push_back((*iterCdrSeg));
						r_splitFlag = true;	
						break;
					}
				}				
			}
			else //连续值
			{				
				long startHoliDay, endHoliDay;
				getMinValue(startHoliDay);
				getMaxValue(endHoliDay);				
					
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			         iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{					
					holiday = (long) (*iterCdrSeg).m_startDateTime.getDateInteger();
					if (holiday>=startHoliDay && holiday<=endHoliDay)
					{
						(*r_pSepCdrSegList).push_back((*iterCdrSeg));
						r_splitFlag = true;	
					}					
				}				
			}
		}
		else if (m_diviedTag==TAG_DIVIED_WEEK)//按周
		{
			long weekDay;
			if (m_judgeMode == TAG_JUDGE_MODE_IN)//离散值
			{
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			       iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{
					weekDay = (*iterCdrSeg).m_startDateTime.getWeekDay();
					if (judgeCond(weekDay))					
					{
						(*r_pSepCdrSegList).push_back((*iterCdrSeg));
						r_splitFlag = true;	
						break;
					}
				}
			}			
			else //连续值
			{				
				long startWeekDay, endWeekDay;
				getMinValue(startWeekDay);
				getMaxValue(endWeekDay);
					
				for (iterCdrSeg=(*r_pCdrSegList).begin(); 
			         iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
				{					
					weekDay = (*iterCdrSeg).m_startDateTime.getWeekDay();
					if (weekDay>=startWeekDay && weekDay<=endWeekDay)
					{
						(*r_pSepCdrSegList).push_back((*iterCdrSeg));
						r_splitFlag = true;	
					}					
				}				
			}
		}
		else if (m_diviedTag==TAG_DIVIED_SUBDTIME)//绝对时间
		{
			long     t_lstartDateTime,t_lendDateTime;
			DateTime t_startDateTime, t_endDateTime;
			getMinValue(t_lstartDateTime);
			getMaxValue(t_lendDateTime);
			
      		for (iterCdrSeg=r_pCdrSegList->begin();iterCdrSeg!=r_pCdrSegList->end();++iterCdrSeg)
			{
				t_startDateTime.set((Integer)(t_lstartDateTime/1000000),
				                    (Integer)(t_lstartDateTime%1000000));
				t_endDateTime  .set((Integer)(t_lendDateTime/1000000),
				                    (Integer)(t_lendDateTime%1000000));
				
				if (iterCdrSeg->split(t_startDateTime, t_endDateTime, tempSeg))
				{
					r_pSepCdrSegList->push_back(tempSeg);
					r_splitFlag = true;			
				}
			}
		}
		//Added By Sunam 2009/8/13 12:30:05
		else if (m_diviedTag==TAG_DIVIED_HOLIDAY)//节假日
		{
		    //YES_OR_NO = 0正常
		    if (m_yesOrNo == TAG_YESORNO_YES)
		    {
			    vector<T_OBJECT_ATTR> t_holiday;
			    PMManager *t_paramMgr = NodeBase::m_globalInfo->m_paramMgr;
			    if (m_judgeMode == TAG_JUDGE_MODE_IN)//离散值
			    {				
			    	for (iterCdrSeg=(*r_pCdrSegList).begin();iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
			    	{
			    		T_DATE_TIME t_beginTime;
			    		t_beginTime = (long) iterCdrSeg->m_startDateTime.getDateInteger();
			    		t_holiday.clear();
			    		if (t_paramMgr->getHolidayType(iterCdrSeg->m_startDateTime.getDate(),t_beginTime,t_holiday))
			    		{
			    		    for (int i=0; i<t_holiday.size(); ++i)
			    		    {
			    		    	if (judgeCond(t_holiday[i]))					
			    		    	{
			    		    		(*r_pSepCdrSegList).push_back((*iterCdrSeg));
			    		    		r_splitFlag = true;	
			    		    		break;
			    		    	}
			    		    }
			    		}
			    	}				
			    }
			    else //连续值
			    {				
			    	T_OBJECT_ATTR startHoliDay, endHoliDay;
			    	getMinValue(startHoliDay);
			    	getMaxValue(endHoliDay);				
			    		
			    	for (iterCdrSeg=(*r_pCdrSegList).begin();iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
			    	{					
			    		T_DATE_TIME t_beginTime;
			    		t_beginTime = (long) iterCdrSeg->m_startDateTime.getDateInteger();
			    		t_holiday.clear();
			    		if (t_paramMgr->getHolidayType(iterCdrSeg->m_startDateTime.getDate(),t_beginTime,t_holiday))
			    		{
			    		    for (int i=0; i<t_holiday.size(); ++i)
			    		    {
			    		    	if ((t_holiday[i] >= startHoliDay) && (t_holiday[i] <= endHoliDay))
			    		    	{
			    		    		(*r_pSepCdrSegList).push_back((*iterCdrSeg));
			    		    		r_splitFlag = true;	
			    		    		break;
			    		    	}
			    		    }
			    		}
			    	}				
			    }
			}
			//YES_OR_NO != 0取反
			else
			{
			    vector<T_OBJECT_ATTR> t_holiday;
			    PMManager *t_paramMgr = NodeBase::m_globalInfo->m_paramMgr;
			    bool matchFlag;
			    if (m_judgeMode == TAG_JUDGE_MODE_IN)//离散值
			    {				
			    	for (iterCdrSeg=(*r_pCdrSegList).begin();iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
			    	{
			    	    matchFlag = true;
			    		T_DATE_TIME t_beginTime;
			    		t_beginTime = (long) iterCdrSeg->m_startDateTime.getDateInteger();
			    		t_holiday.clear();
			    		if (t_paramMgr->getHolidayType(iterCdrSeg->m_startDateTime.getDate(),t_beginTime,t_holiday))
                        {
			    		    for (int i=0; i<t_holiday.size(); ++i)
			    		    {
			    		    	if (judgeCond(t_holiday[i]))					
			    		    	{
			    		    	    matchFlag = false;
			    		    	}
			    		    }
			    		}
			    		//如果是取反，全部不满足enumValue才认为条件成立
			    		if (matchFlag)
                        {
			    		    (*r_pSepCdrSegList).push_back((*iterCdrSeg));
                            r_splitFlag = true;	
                        }
			    	}				
			    }
			    else //连续值
			    {				
			    	T_OBJECT_ATTR startHoliDay, endHoliDay;
			    	getMinValue(startHoliDay);
			    	getMaxValue(endHoliDay);				
			    		
			    	for (iterCdrSeg=(*r_pCdrSegList).begin();iterCdrSeg!=(*r_pCdrSegList).end();++iterCdrSeg)
			    	{				
			    	    matchFlag = true;	
			    		T_DATE_TIME t_beginTime;
			    		t_beginTime = (long) iterCdrSeg->m_startDateTime.getDateInteger();
			    		t_holiday.clear();
			    		if (t_paramMgr->getHolidayType(iterCdrSeg->m_startDateTime.getDate(),t_beginTime,t_holiday))
			    		{
			    		    for (int i=0; i<t_holiday.size(); ++i)
			    		    {
			    		    	if ((t_holiday[i] >= startHoliDay) && (t_holiday[i] <= endHoliDay))
			    		    	{
			    		    	    matchFlag = false;
			    		    	}
			    		    }
			    		}
			    		//如果是取反，全部不满足enumValue才认为条件成立
			    		if (matchFlag)
                        {
			    		    (*r_pSepCdrSegList).push_back((*iterCdrSeg));
                            r_splitFlag = true;	
                        }
			    	}				
			    }
			}
	        return r_splitFlag;
		}
		else
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未定义的分段标志："
                                "cond_id=%d,divied_Tag=%c", (long)m_id, m_diviedTag);
		}
		return r_splitFlag;
	}	
                                
	return false; 
}

bool CondRule::execute() 
{
	bool result = false;
	
	if (m_valueType == TAG_VALUE_TYPE_NUMBER)//值类型   0:数值
	{
		//Modified by hany at 2011/3/31 16:38:55	
		long value=0;
		if(m_judgeObj == NULL)
		{
			char computExpr = m_judgeValue[0];

			if(computExpr == 'H')
				{
					char *expr;
		      expr = &m_judgeValue[1];
		      if (!m_globalInfo->explanSpecialExpr(value,expr,m_policyInfo.m_rateTpInfo))
		      {
		      	THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
		      	                                  "cond_id=%d,m_judgeValue=%s",(long)m_id, m_judgeValue);
		      }
	      }
	    else
	     	THROW_C_P1(BusinessException, -1, "条件配置错误,对象未定义："
			                                  "cond_id=%d",(long)m_id);
		}
		else
		{
		  m_judgeObj->getObjectValue(value);
	  }
	  //Modified by hany at 2011/3/31 16:38:55
	  
		if (m_valueMethed == TAG_VALUE_METHOD_COMMON)//取值方法：普通
		{
			result = judgeCond(value);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_COMPUTE)//数值运算
		{
			char computOper = m_valueParam[0];
			//add by fug 200910082121 for "valueParam" expr s
			char computExpr = m_valueParam[1];
			long dividedValue=0;
			dividedValue = atol (&m_valueParam[1]); 
			if(computExpr == 'E')
				{
					char *expr;
		      expr = &m_valueParam[2];
		      if (!m_globalInfo->explanExpr(dividedValue,expr,m_policyInfo.m_rateTpInfo))
		      {
		      	/*THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
		      	                                  "cond_id=%d,m_minValue=%s",(long)m_id, m_valueParam);*/
		      	 		return false;	  	 
		      	                                  
		      }
	      }
	     else
	     				dividedValue = atol (&m_valueParam[1]); 
			//add by fug 200910082121 for "valueParam" expr e
			
			if (computOper == '%')  //取模
			{
				if (dividedValue != 0)				
					value %= dividedValue;
				else    
					THROW_C_P3(BusinessException, -1, "条件配置错误,值参数作分母时为0："
                                   "cond_id=%d,value_param=%s,value_method=%c", 
                                   (long)m_id, m_valueParam, m_valueMethed);
			}
			else if (computOper == '/')//倍数
			{
				if (dividedValue != 0)
					value /= dividedValue;				
				else
					THROW_C_P3(BusinessException, -1, "条件配置错误,值参数作分母时为0："
                                   "cond_id=%d,value_param=%s,value_method=%c", 
                                   (long)m_id, m_valueParam, m_valueMethed);
			}
			else if (computOper == '*')//乘法
			{				
					value *= dividedValue;				
			}
			else if (computOper == '+')
			{
					value += dividedValue;				
			}
			else if (computOper == '-')
			{
					value -= dividedValue;
			}
			else
			{
				THROW_C_P3(BusinessException, -1, "条件配置错误,未定义的运算方式："
                                   "cond_id=%d,value_param=%s,comput_oper=%c", 
                                   (long)m_id, m_valueParam, m_valueParam[0]);
			}
			result = judgeCond(value);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_STRCAT)//取值函数:STRCAT
		{      
			char computOper = m_valueParam[0];
			
			if (computOper == '*')  //直接连接
			{
				T_OBJECT_ATTR strValue;
				strValue = value;
				strValue.append(strValue.length(),&m_valueParam[1]);
				value = strValue.value();
			}
			else 
			{
				//TXXDDHHNNSS,T:+代表加,-代表减;XX代表相对帐期
				char cycleNum[3];
				long r_addMonth;
				
				cycleNum[0]=m_valueParam[1];
				cycleNum[1]=m_valueParam[2];
				cycleNum[2]='\0';				
				
				if (computOper == '+')
				{
					r_addMonth = atol(cycleNum);					 					
				}
				else if (computOper == '-')
				{
					r_addMonth = - atol(cycleNum);					
				}
				else
				{
					THROW_C_P3(BusinessException, -1, "条件配置错误,未定义的运算方式："
                                   "cond_id=%d,value_param=%s,comput_oper=%c", 
                                   (long)m_id, m_valueParam, m_valueParam[0]);
				}
				
				for (int k=0; k<10; ++k)
				{
					if (value > 999999)
					{
						value = value/10;
					}
					else
					{
						break;
					}
				}
				
				m_globalInfo->addMonth(r_addMonth,value);
				T_OBJECT_ATTR strValue1;
				strValue1 = value;
				strValue1.append(strValue1.length(),&m_valueParam[3]);
				value = strValue1.value();
			}
			
			result = judgeCond(value);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_SUBSTR)//取值函数:SUBSTR
		{      
			THROW_C_P3(BusinessException, -1, "条件配置错误,值类型和取值方法冲突："
                                   "cond_id=%d,value_type=%c,value_method=%c", 
                                   (long)m_id, m_valueType, m_valueMethed);
		}
		else
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未定义的取值方法："
			                                  "cond_id=%d,value_method=%c",
			                                   (long)m_id, m_valueMethed);
		}
			
	}
	else if (m_valueType == TAG_VALUE_TYPE_CHAR)//值类型 1:字符
	{
		//Modified by hany at 2011/3/31 16:38:55
		T_OBJECT_ATTR strValue;
		
		if(m_judgeObj == NULL)
		{
			char computExpr = m_judgeValue[0];

			if(computExpr == 'E')
				{
					char *expr;
		      expr = &m_judgeValue[1];
		      long dividedValue = 0;
		      if (!m_globalInfo->explanSpecialExpr(dividedValue,expr,m_policyInfo.m_rateTpInfo))
		      {
		      	THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
		      	                                  "cond_id=%d,m_judgeValue=%s",(long)m_id, m_judgeValue);
		      }
		      strValue = dividedValue;
	      }
	    else
	     	THROW_C_P1(BusinessException, -1, "条件配置错误,对象未定义："
			                                  "cond_id=%d",(long)m_id);
		}
		else
		{
		  m_judgeObj->getObjectValue(strValue);
	  }
	  //Modified by hany at 2011/3/31 16:38:55

		if (m_valueMethed == TAG_VALUE_METHOD_COMMON)//取值方法：普通
		{
			result = judgeCond(strValue);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_SUBSTR)//取值函数:SUBSTR		
		{ 
			int pos;
			int length;
			
			char buff[MAX_STR_LENGTH+1];
			strncpy(buff, m_valueParam,MAX_STR_LENGTH);
			buff[MAX_STR_LENGTH]='\0';	
			
			char * pValue = strtok(buff, ","); 
			pos = atoi(pValue); 			
			pValue = strtok(NULL, ",");			
			length = atoi(pValue);
			
			strValue = strValue.substrN(pos, length);

			result = judgeCond(strValue);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_STRCAT)//取值函数:STRCAT
		{      
			char computOper = m_valueParam[0];
			
			if (computOper == '*')  //直接连接
			{
				strValue.append(strValue.length(),&m_valueParam[1]);
			}
			else 
			{
				//TXXDDHHNNSS,T:+代表加,-代表减;XX代表相对帐期
				char cycleNum[3];
				long r_addMonth;
				long intValue = strValue.value();				
				
				cycleNum[0]=m_valueParam[1];
				cycleNum[1]=m_valueParam[2];
				cycleNum[2]='\0';				
				
				if (computOper == '+')
				{
					r_addMonth = atol(cycleNum);					 					
				}
				else if (computOper == ength);

			result = judgeCond(strValue);
		}
		else if (m_valueMethed == TAG_VALUE_METHOD_STRCAT)//鍙栧