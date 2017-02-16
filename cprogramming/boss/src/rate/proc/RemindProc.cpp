#include "RemindProc.h"
#include "RateError.h"
#include "BillManager.h"
#include "RateConstDef.h"
#include "RateGlobalInfo.h"
#include "BillingCommon.h"
#include "RemindInfo.h"
#include <algorithm>

INSTALL_PROCEDURE_INSTANCE(RemindProc);

int RemindProc::execute()
{
    vector<UserSumBill1>::iterator itr11;
    vector<UserSumBill1>::iterator itr12;
    vector<UserSumBill2>::iterator itr21;
    vector<UserSumBill2>::iterator itr22;
    vector<FeePolicyRemidInfo> result;
    vector<FeePolicyRemidInfo>::iterator itr3;
    RemindInfo t_remindInfo;
    long t_value;
    
    t_remindInfo.m_userId        = m_pcdr[ATTRI_USER_ID];
    t_remindInfo.m_msisdn        = m_pcdr[ATTRI_MSISDN_A];
    t_remindInfo.m_startDateTime = m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME];
    t_remindInfo.m_endDateTime   = m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME];
    t_remindInfo.m_duration      = m_pcdr[ATTRI_DURATION].value();
    
    for (itr11 = m_pRateInfo->m_userSumBill1s_new.begin(); itr11 != m_pRateInfo->m_userSumBill1s_new.end(); ++itr11)
    {
    	result.clear();
    	#ifdef _DEBUG_
    		cout << " itr11->m_feePolicyId : " << itr11->m_feePolicyId << " itr11->m_billId : " << itr11->m_billId << endl;
    	#endif
    	//根据feePolicyId表TD_B_FEEPOLICY_REMIND中查找提醒配置
    	if (m_pmManager->getFeePolicyRemidInfo(itr11->m_feePolicyId, itr11->m_billId, result))
    	{
    		itr12 = ::lower_bound(m_pRateInfo->m_userSumBill1s.begin(),m_pRateInfo->m_userSumBill1s.end(),*itr11);
    		//原始账单是否有该累计量
    		if ((itr12 != m_pRateInfo->m_userSumBill1s.end()) && (*itr12 == *itr11))
    		{
    			t_value = itr12->m_value;
    		}
    		else
    		{
    			t_value = 0;
    		}
    		for (itr3 = result.begin(); itr3 != result.end(); ++itr3)
    		{
    			//提醒配置的为固定值
    			if (itr3->m_valueType == FIXVALUE)
    			{
    				if ((t_value < itr3->m_value) && (itr11->m_value >= itr3->m_value))
    				{
    					//需要提醒
    					t_remindInfo.m_billType       = BILL_TYPE_SUM1;
    					t_remindInfo.m_cycleId        = m_pRateInfo->m_sum1BillCycle;
    					t_remindInfo.m_cycleBeginTime = "";
    					t_remindInfo.m_cycleEndTime   = "";
    					t_remindInfo.m_feePolicyId    = itr11->m_feePolicyId;
    					t_remindInfo.m_tpObjId        = itr11->m_tpObjId;
    					t_remindInfo.m_addupId        = itr11->m_billId;
    					t_remindInfo.m_remindLevel    = itr3->m_remindLevel;
    					t_remindInfo.m_remindValue    = itr3->m_value;
    					t_remindInfo.m_maxValue       = itr11->m_maxValue;
    					t_remindInfo.m_curValue       = itr11->m_value;
    					m_pRateInfo->m_remindInfo.push_back(t_remindInfo);
    					#ifdef _DEBUG_
    						cout << t_remindInfo << endl;
    					#endif
    				}
    			}
    			//提醒配置的为偏移量
    			else if (itr3->m_valueType == OFFSETVALUE)
    			{
    				if ((t_value < itr11->m_maxValue + itr3->m_value) && (itr11->m_value >= itr11->m_maxValue + itr3->m_value))
    				{
    					//需要提醒
    					t_remindInfo.m_billType       = BILL_TYPE_SUM1;
    					t_remindInfo.m_cycleId        = m_pRateInfo->m_sum1BillCycle;
    					t_remindInfo.m_cycleBeginTime = "";
    					t_remindInfo.m_cycleEndTime   = "";
    					t_remindInfo.m_feePolicyId    = itr11->m_feePolicyId;
    					t_remindInfo.m_tpObjId        = itr11->m_tpObjId;
    					t_remindInfo.m_addupId        = itr11->m_billId;
    					t_remindInfo.m_remindLevel    = itr3->m_remindLevel;
    					t_remindInfo.m_remindValue    = itr11->m_maxValue + itr3->m_value;
    					t_remindInfo.m_maxValue       = itr11->m_maxValue;
    					t_remindInfo.m_curValue       = itr11->m_value;
    					m_pRateInfo->m_remindInfo.push_back(t_remindInfo);
    					#ifdef _DEBUG_
    						cout << t_remindInfo << endl;
    					#endif
    				}
    			}
    		}
    	}
    }
    
    for (itr21 = m_pRateInfo->m_userSumBill2s_new.begin(); itr21 != m_pRateInfo->m_userSumBill2s_new.end(); ++itr21)
    {
    	result.clear();
    	//根据feePolicyId表TD_B_FEEPOLICY_REMIND中查找提醒配置
    	if (m_pmManager->getFeePolicyRemidInfo(itr21->m_feePolicyId, itr21->m_billId, result))
    	{
    		itr22 = ::lower_bound(m_pRateInfo->m_userSumBill2s.begin(),m_pRateInfo->m_userSumBill2s.end(),*itr21);
    		if ((itr22 != m_pRateInfo->m_userSumBill2s.end()) && (*itr22 == *itr21))
    		{
    			for (itr3 = result.begin(); itr3 != result.end(); ++itr3)
    			{
    				//提醒配置的为固定值
    				if (itr3->m_valueType == FIXVALUE)
    				{
    					if ((itr22->m_value < itr3->m_value) && (itr21->m_value >= itr3->m_value))
    					{
    						t_remindInfo.m_billType       = BILL_TYPE_SUM2;
    					    t_remindInfo.m_cycleId        = itr21->m_cycleId;
    					    t_remindInfo.m_cycleBeginTime = itr21->m_beginTime;
    					    t_remindInfo.m_cycleEndTime   = itr21->m_endTime;
    					    t_remindInfo.m_feePolicyId    = itr21->m_feePolicyId;
    					    t_remindInfo.m_tpObjId        = itr21->m_tpObjId;
    					    t_remindInfo.m_addupId        = itr21->m_billId;
    					    t_remindInfo.m_remindLevel    = itr3->m_remindLevel;
    					    t_remindInfo.m_remindValue    = itr3->m_value;
    					    t_remindInfo.m_maxValue       = itr21->m_maxValue;
    					    t_remindInfo.m_curValue       = itr21->m_value;
    					    m_pRateInfo->m_remindInfo.push_back(t_remindInfo);
    					    #ifdef _DEBUG_
    					    	cout << t_remindInfo << endl;
    					    #endif
    					}
    				}
    				//提醒配置的为偏移量
    				else if (itr3->m_valueType == OFFSETVALUE)
    				{
    					if ((itr22->m_value < itr21->m_maxValue + itr3->m_value) && (itr21->m_value >= itr21->m_maxValue + itr3->m_value))
    					{
    						//需要提醒
    						t_remindInfo.m_billType       = BILL_TYPE_SUM2;
    					    t_remindInfo.m_cycleId        = itr21->m_cycleId;
    					    t_remindInfo.m_cycleBeginTime = itr21->m_beginTime;
    					    t_remindInfo.m_cycleEndTime   = itr21->m_endTime;
    					    t_remindInfo.m_feePolicyId    = itr21->m_feePolicyId;
    					    t_remindInfo.m_tpObjId        = itr21->m_tpObjId;
    					    t_remindInfo.m_addupId        = itr21->m_billId;
    					    t_remindInfo.m_remindLevel    = itr3->m_remindLevel;
    					    t_remindInfo.m_remindValue    = itr21->m_maxValue + itr3->m_value;
    					    t_remindInfo.m_maxValue       = itr21->m_maxValue;
    					    t_remindInfo.m_curValue       = itr21->m_value;
    					    #ifdef _DEBUG_
    					    	cout << t_remindInfo << endl;
    					    #endif
    					}
    				}
    			}
    		}
    	}
    }
    return 0;
}
