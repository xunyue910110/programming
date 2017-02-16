#include "RateGlobalInfo.h"
#include "RateConstDef.h"
#include "RateLog.h"
#include "PMManager.h"
#include "BillManager.h"
#include "FeePolicyComp.h"
#include "PriceParamImpl.h"
#include "RateError.h"
#include "base/StringUtil.h"
#include "appfrm/ErrorHandle.h"
#include "EventFeePolicy.h"
#include "ObjectBase.h"
#include "base/exceptions.h"

bool RateGlobalInfo::initialization(RateConfig *r_pConfig,RateLog *r_rateLog,
                                    PMManager *r_pParamMgr,BillManager *r_billMgr,
                                    CycParam  *r_pSysCycParam,int *r_pSysCycle,
                                    ErrorMessages *r_pErrMsgs)
{
  m_pErrorMsgs = r_pErrMsgs;
  m_pConfig  = r_pConfig;
  m_rateLog  = r_rateLog;
  m_paramMgr = r_pParamMgr;
  m_pCdr     = &m_cdr;
  m_billMgr  = r_billMgr;
  m_channelNo= r_pConfig->m_channelNo;
  //added by hany 2011/3/22 9:58:27
  m_compItemMgr = m_paramMgr->getCompositItemMgr();
  m_sumBillMgr.initialize(this,m_compItemMgr,&(m_rateFeeInfo.m_sumRuleList));
  m_detailBillMgr.initialize(m_compItemMgr,&m_discountBills,&m_addBillVector);
  
  if(m_comBill.initialize(m_channelNo,m_pConfig->m_provinceCode,m_billMgr,
                          m_paramMgr,&m_addBillVector,&m_discountBills,
                          m_pCdr,&m_errMsg,
                          &m_vecUserServState,&m_vecUserServ
                          )==false)
  {
    #ifdef _DEBUG_
      cout<<"m_comBill.initialize error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_pSysCycParam = r_pSysCycParam;
  m_pSysCycle    = r_pSysCycle;
  memset(m_dealTime,0,sizeof(m_dealTime));
  m_fatalECode = 0;
  m_fatalEMsg  = "";
  
  outputCdrVector.clear();
  outputBillVector.clear();
  //初始化3G参数模板
  m_paramIdMapKey.clear();
  m_paramIdMapKey[1]="fee";
  m_paramIdMapKey[2]="feetype";
  m_paramIdMapKey[3]="billingunit";
  m_paramIdMapKey[4]="maxfee";
  m_paramIdMapKey[5]="minfee";
  m_paramIdMapKey[6]="monthtype";
  m_paramIdMapKey[7]="chargemode";
  m_paramIdMapKey[8]="ifautoorder";
  
  m_jsInBuf.newField("V0",dtDouble,10);
  m_jsInBuf.newField("V1",dtDouble,10);
  m_jsInBuf.newField("V2",dtDouble,10);
  m_jsInBuf.newField("V3",dtDouble,10);
  m_jsInBuf.newField("V4",dtDouble,10);
  m_jsInBuf.newField("V5",dtDouble,10);
  m_jsInBuf.newField("V6",dtDouble,10);
  m_jsInBuf.newField("V7",dtDouble,10);
  m_jsInBuf.newField("V8",dtDouble,10);
  m_jsInBuf.newField("V9",dtDouble,10);
  m_jsOutBuf.newField("result",dtDouble,10);
  return true;
}

//Added By Sunam 2009/8/10 15:53:01 处理文件前的初始化，或者文件中途提交后的初始化
bool RateGlobalInfo::fileReset()
{
	int i;
	//清空本地账单缓存及已处理标志
	
	for (i = 0; i < 2; ++i)
	{
		//m_tUserBill[i].clear();
		m_tAccountBill[i].clear();
		//m_tRateDiscnts[i].clear();
		//m_tUserBill_org[i].clear();
		m_tAccountBill_org[i].clear();
		//m_tRateDiscnts_org[i].clear();
		//m_billDealUserSet[i].clear();
		m_accountDealUserSet[i].clear();
		//m_rateDiscntDealUserSet[i].clear();
	}
	for (i = 0; i < 12; ++i)
	{
		m_tUserBill[i].clear();
		m_tUserMonfeeBill[i].clear();
		m_tUserSumBill1[i].clear();
		//m_tAccountBill[i].clear();
		m_tRateDiscnts[i].clear();
		m_tUserBill_org[i].clear();
		m_tUserMonfeeBill_org[i].clear();
		m_tUserSumBill1_org[i].clear();
		//m_tAccountBill_org[i].clear();
		m_tRateDiscnts_org[i].clear();
		m_billDealUserSet[i].clear();
		m_MonfeebillDealUserSet[i].clear();
		m_sum1DealUserSet[i].clear();
		//m_accountDealUserSet[i].clear();
		m_rateDiscntDealUserSet[i].clear();
	}
	for (i = 0; i < 12; ++i)
	{
		m_tUserSumBill1[i].clear();
		m_tUserSumBill1_org[i].clear();
		m_sum1DealUserSet[i].clear();
	}
	m_tUserSumBill2.clear();
	m_tUserSumBill2_org.clear();
	m_sum2DealUserSet.clear();
	
  m_tUserSumBill3.clear();
	m_tUserSumBill3_org.clear();
	m_sum3DealUserSet.clear();
	//清空待加锁用户列表
	m_lockUserList.clear();
	//清空已加锁用户列表
	m_lockInfoList.clear();
	//锁状态正常
	m_lockStatus = true;
	
	//清空待加锁账户列表 add by xuf 
	m_lockAcctList.clear();
	//清空回滚账单信息
	m_rollBackBillsAll.clear();
	
	m_monfeeOutputCdrList.clear();
	//add by xuf 2009-11-24 18:33:37
  	m_comBill.m_vUserFileMap.clear();
	
	return true;
}

bool RateGlobalInfo::cdrReset()
{
	//add for rent new version begin add by malong 2009.9.11
	m_mtmapRentTpList.clear();
	m_rentFeeLogInfoVector.clear();//防止前一次有U文件调用过RentProc之后，容器没有清空，后面过来的文件，仍然输出这个容器内容。
	//add for rent new version end add by malong 2009.9.11
	
  if(m_cdr.reset()==false)
  {
    return false;
  }
  //Added By Sunam 2009/10/27 18:40:19 m_pCdr[ATTRI_FILE_NAME]在mcdr.reset()中被清空了，重置一下
  m_pCdr[ATTRI_FILE_NAME] = m_fileName.c_str();
  m_userBills.clear();
  m_userSumBill1s.clear();
  m_userSumBill2s.clear();
  m_accountBills.clear();
  m_rateDiscnts.clear();
  m_userMonfeeBills.clear();
  m_userBills_new.clear();
  m_userMonfeeBills_new.clear();
  m_userSumBill1s_new.clear();
  m_userSumBill2s_new.clear();
  m_accountBills_new.clear();
  m_rateDiscnts_new.clear();
  m_rollBackBills.clear();
  m_addBillVector.clear();
  m_balAcctIdList.clear();
  m_discountBills.clear();
  
  m_userTpList.clear();
  m_tpInfoList.clear();
  m_pRateTpList.clear();
  m_pAcctTpList.clear();
  m_pRentTpList.clear(); //add by duyu 2009.9.10
  m_pDataTpList.clear();
  
  //Modified By Sunam 2009/8/10 19:39:46 放到fileReset()中去
  //m_lockInfoList.clear();
  m_gBillUserList.clear();
  m_groupUserList.clear();
  m_rollBackBillsGroupUser.clear();
  //added by zhaoly for 3G at 2009-5-16 01:17上午
  m_partyPriceInfo.clear();
  m_parsedPartyPriceInfo.clear();
  for(int i=0;i<FEEFIELDS_MAX;++i)
  {
    m_hasFeeFlag[i]=false;
    m_cdr[ATTRI_ORIGINFLAG_FEE1+i-1]=0;
  }
  m_cdr[ATTRI_FEEUNIT_1] = DEFAULT_CFEE_UNIT;
  m_cdr[ATTRI_FEEUNIT_2] = DEFAULT_CFEE_UNIT;
  m_cdr[ATTRI_FEEUNIT_3] = DEFAULT_CFEE_UNIT;
  m_cdr[ATTRI_FEEUNIT_4] = DEFAULT_LFEE_UNIT;
  m_cdr[ATTRI_FEEUNIT_5] = DEFAULT_LFEE_UNIT;
  m_cdr[ATTRI_VPN_FLAG]  = FLAG_NO;
  m_cdr[ATTRI_CELL_FLAG] = FLAG_NO;
  m_cdr[ATTRI_NEED_OUTPUTCDR] = FLAG_YES; //默认是需要输出话单
  m_rateFeeInfo.clear();  
  m_vecUserServ.clear();
  m_vecUserServState.clear();
  
  //add by gaojf 2010-1-11 13:58:16 begin
  m_vpnInfoList_a.clear();
  m_vpnInfoList_b.clear();
  //end 
  
  //Added By Sunam 2009/8/8 18:16:24
  m_remindInfo.clear();
  m_adjustBeforeBills.clear();//add by xuf 2009-9-30 19:46:05
  m_isExistServ 		= false ; //add by xuf 2009-11-17 4:44:30
  m_isExistServState 	= false ; //add by xuf 2009-11-17 4:44:50
  m_comBill.m_userNeedAcctDealTag = true; //add by xuf 2009-11-29 15:11:00
  m_ExclusivedEventFeepolicy.clear();//清空被互斥事件列表
  return true;
}

//读取话单后初始化
bool RateGlobalInfo::canonicalize()
{
  char t_errMsg[256];
  if(m_cdr.canonicalize()==false)
  {
    return false;
  }
  m_cdr[ATTRI_DEAL_TIME] = m_dealTime+4;
  if(m_cdr[ATTRI_HOME_CODE_A] == m_cdr[ATTRI_HOME_CODE_B])
  {
    m_cdr[ATTRI_ABSAMEHOME_FLAG] = "1";
  }else
  {
    m_cdr[ATTRI_ABSAMEHOME_FLAG] = "0";
  } 
  //如果没有结束时间,则设置结束时间
  if(strlen(m_cdr[ATTRI_END_DATE].c_str())==0)
  {
    DateTime t_beginDateTime;
    DateTime t_endDateTime;
    t_beginDateTime.set(m_cdr[ATTRI_BEGIN_DATE].c_str(),m_cdr[ATTRI_BEGIN_TIME].c_str());
    t_endDateTime = t_beginDateTime+m_cdr[ATTRI_DURATION].value();
    m_cdr[ATTRI_END_DATE] = t_endDateTime.getDate();
    m_cdr[ATTRI_END_TIME] = t_endDateTime.getTime();
  }
  
  m_userId = m_cdr[ATTRI_USER_ID];
  m_bizType= m_cdr[ATTRI_BIZ_TYPE];
  //设置m_userId，对于无主等..需要根据msisdn或imsi设置。
  
  //if(m_userId == INVALID_SERVER_ID || m_userId.length() == 0)
  if(m_userId == INVALID_SERVER_ID || m_userId.length() == 0 || m_userId == "0")
  {
    //
    //if(m_cdr[ATTRI_ROAM_TYPE]   == ROAM_TYPE_NATIONAL_IN ||
    //   m_cdr[ATTRI_ROAM_TYPE]   == ROAM_TYPE_INTERNATIONAL_IN ||
    //   m_pConfig->m_channelType == CHANNEL_INTERPROV)
    if (m_pConfig->m_channelType == CHANNEL_INTERPROV)
    {
      m_billUserId = INVALID_SERVER_ID;
    }else
    {      
        m_billUserId = m_cdr[ATTRI_MSISDN_A];
        //Added By Sunam 2009/10/8 10:45:48 
        //无主话单中有错误编码，无法正常批价，因此如果是无主话单需要批价时先重置错误编码
        m_pCdr[ATTRI_ERROR_CODE] = E_DEFAULT;
        m_pCdr->m_errorCode      = E_DEFAULT;
    }
  }else
  {
    m_billUserId = m_userId;
  }
  
  int t_cdrDate = atoi(m_cdr[ATTRI_BEGIN_DATE].c_str());
//Add by yangb For sx REQ21 At 2010/2/24 14:18:45
#ifdef __PROV019__
//如果是宽带则根据话单结束时间判断账期
  if (m_bizType==BIZ_TYPE_NET||m_bizType==BIZ_TYPE_FIXVOICE)
  {
    t_cdrDate = atoi(m_cdr[ATTRI_END_DATE].c_str());
  }
#endif
  int t_cdrCycle;
  if(m_paramMgr->getBillCycle(t_cdrDate,t_cdrCycle)==false)
  {
    #ifdef _DEBUG_
      cout<<"getBillCycle("<<t_cdrDate<<",t_cdrCycle) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    sprintf(t_errMsg,"求取该帐期信息 失败! 用户=%s, 话单时间=%d ,帐期=%d",m_billUserId.c_str(),t_cdrDate,t_cdrCycle);
    setErrInfo(E_BILL_CYCLE,t_errMsg);
    writeErrMsg(__FILE__,__LINE__,t_errMsg);
    return false;
  }
  m_cdr[ATTRI_CDR_CYCLE_ORG] = t_cdrCycle;
  
  if(isNomalCdr(t_cdrCycle)==false) return false;
  m_cdr[ATTRI_CYCLE_DATE] = t_cdrCycle;
  m_beginDateTime = m_cdr[ATTRI_BEGIN_DATE]+m_cdr[ATTRI_BEGIN_TIME];
  m_endDateTime   = m_cdr[ATTRI_END_DATE]+m_cdr[ATTRI_END_TIME];
  
  //add by gaojf 为了适应集团资料的特殊处理 
  if(m_paramMgr->isMobilePhone(m_cdr[ATTRI_MSISDN_B].c_str())||
      strncmp(m_cdr[ATTRI_MSISDN_B].c_str(),"0",1)==0 ||
      strncmp(m_cdr[ATTRI_MSISDN_B].c_str(),"1",1)==0 ||
      strlen(m_cdr[ATTRI_MSISDN_B].c_str())==0 ||
      m_pCdr[ATTRI_DEALMSISDN_FLAG].value() == 0) 
  {//手机号或0开头或不需加区号处理的
    m_otherParty = m_cdr[ATTRI_MSISDN_B];
  }else
  { 
    m_otherParty = m_cdr[ATTRI_HOME_CODE_B]+m_cdr[ATTRI_MSISDN_B];
  }
  
  //求取该帐期的第一秒和最后一秒
  if(m_paramMgr->getCycleTime(t_cdrCycle,m_cycParam)==false)
  {
    #ifdef _DEBUG_
      cout<<"求取该帐期信息 失败! 帐期："<<t_cdrCycle<<endl;
    #endif
    sprintf(t_errMsg,"求取该帐期信息 失败! 帐期=%d",t_cdrCycle);
    setErrInfo(E_BILL_CYCLE,t_errMsg);
    writeErrMsg(__FILE__,__LINE__,t_errMsg);
    return false;
  }
  
  //Added By Sunam 2009/11/9 21:20:43 用户当月最早开机时间
  vector<UserServState> r_userServStates;
  #ifdef _DEBUG_
    cout << " m_cycParam.m_beginTime : " << m_cycParam.m_beginTime << endl;
    cout << " m_cycParam.m_endTime   : " << m_cycParam.m_endTime << endl;
  #endif
//  if (m_paramMgr->getUserServState(m_userId,r_userServStates) > 0)
//  {
//  	  bool t_flag = false;
//  	  for (int m=0; m < r_userServStates.size(); ++m)
//  	  {
//  	  	  //主体服务
//  	  	  if (r_userServStates[m].m_mainTag == '1')
//	      {
//	      	  #ifdef _DEBUG_
//                cout << " r_userServStates[m].m_startDate : " << r_userServStates[m].m_startDate << endl;
//                cout << " r_userServStates[m].m_endDate   : " << r_userServStates[m].m_endDate << endl;
//              #endif
//	      	  if (r_userServStates[m].m_startDate > m_cycParam.m_endTime)
//	      	  {
//	      	  	  break;
//	      	  }
//  	      	  if (((r_userServStates[m].m_startDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_startDate <= m_cycParam.m_endTime)) ||
//  	      	  	  ((r_userServStates[m].m_endDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate <= m_cycParam.m_endTime)) ||
//  	      	  	  ((r_userServStates[m].m_startDate <= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate >= m_cycParam.m_endTime)))
//		  	  {
//		  	  	  //如果是开机状态
//		  	  	  if (r_userServStates[m].m_servStateCode == '0')
//		  	  	  {
//		  	  	      m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime > r_userServStates[m].m_startDate ? m_cycParam.m_beginTime : r_userServStates[m].m_startDate;
//		  	  	      t_flag = true;
//		  	  	      break;
//		  	  	  }
//		  	  }
//		  }
//  	  }
//  	  //如果未找到，则认为当月无开机状态
//  	  if (!t_flag)
//  	  {
//  	      m_cdr[ATTRI_EARLIEST_OPEN_DATE] = "0";
//  	  }
//  }
//  else
//  {
//  	  m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime;
//  }


// del by songyx 20100706 begin 
/*
    if (m_paramMgr->getUserServState(m_userId,r_userServStates) > 0)
    {
        bool t_flag = false;
        bool t_fApplyStop=false;
        ::sort(r_userServStates.begin(),r_userServStates.end());
        for (int m=0; m < r_userServStates.size(); ++m)
        {
            //主体服务
            if (r_userServStates[m].m_mainTag == '1')
            {
                if (r_userServStates[m].m_servStateCode=='1')//申请停机
                {
                    if (r_userServStates[m].m_startDate<m_cycParam.m_beginTime)
                    {
                        t_fApplyStop=true;//在非本账期时间段内有申请停机
                    }
                }
                if (t_fApplyStop&&(r_userServStates[m].m_servStateCode == '0'))//有停机并且有开机状态
                {
                    if (((r_userServStates[m].m_startDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_startDate <= m_cycParam.m_endTime)) ||
                        ((r_userServStates[m].m_endDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate <= m_cycParam.m_endTime)) ||
                        ((r_userServStates[m].m_startDate <= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate >= m_cycParam.m_endTime)))
                    {
                        t_flag = true;
                        m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime > r_userServStates[m].m_startDate ? m_cycParam.m_beginTime : r_userServStates[m].m_startDate;
                        break;
                    }
                }
            }
          
    
        }
        //如果未找到
        if (!t_flag)
        {
            m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime;
        }
    }*/
// del by songyx 20100706 end     
    
// add by songyx 20100706 累积量折算 begin
    if (m_paramMgr->getUserServState(m_userId,r_userServStates) > 0)
    {
        bool t_flag = false;
        bool t_fApplyStop=false;
        bool t_fTardyStop=false;
        bool t_fTardyStopT=false;
        bool t_fNormal=false;
        bool t_fNormalT=false;
        
        int  t_size=0;
        ::sort(r_userServStates.begin(),r_userServStates.end());
        #ifdef _DEBUG_
        cout <<"用户主体服务状态:r_userServStates.size()="<<r_userServStates.size()<<endl;
        cout <<"r_userServStates[m].m_servStateCode="<<r_userServStates[0].m_servStateCode<<endl;
        #endif
        t_size=r_userServStates.size();
        for (int m=0; m < r_userServStates.size(); ++m)
        {   
        	   
            //主体服务
            if (r_userServStates[m].m_mainTag == '1')
            {
            	  #ifdef _DEBUG_
                cout <<"用户主体服务状态:r_userServStates[m].m_servStateCode="<<r_userServStates[m].m_servStateCode<<","<<m<<endl;
                #endif
                //Modified by yangyb 20100609
                if (r_userServStates[m].m_servStateCode=='1')//申请停机
                {
                    if (r_userServStates[m].m_startDate<m_cycParam.m_beginTime)
                    {
                        t_fApplyStop=true;//在非本账期时间段内有申请停机
                    }
                    else
                    	  t_fApplyStop=false;//add by yangyb 20100728

                    if(m+1 < r_userServStates.size())
                    {                	   
                    	   for (int x=m+1; x < r_userServStates.size(); ++x)
                	  	   {
                	  	   	    if((r_userServStates[m+1].m_servStateCode!='b') &&(r_userServStates[m+1].m_servStateCode!='B')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='5') && (r_userServStates[m+1].m_servStateCode!='7')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='A') && (r_userServStates[m+1].m_servStateCode!='P')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='Q'))
                    	   	         break;
                	  	   	    #ifdef _DEBUG_
                              cout <<"用户主体服务状态:r_userServStates[x].m_servStateCode="<<r_userServStates[x].m_servStateCode<<","<<x<<endl;
                              #endif
                	  	   	    if (r_userServStates[x].m_servStateCode=='b'||r_userServStates[x].m_servStateCode=='B'
                	  	   	    	||r_userServStates[x].m_servStateCode=='5'||r_userServStates[x].m_servStateCode=='7'
                	  	   	    	||r_userServStates[x].m_servStateCode=='A'||r_userServStates[x].m_servStateCode=='P'
                	  	   	    	||r_userServStates[x].m_servStateCode=='Q')//欠费停机
                	  	   	    {
                	  	   	    	   t_fTardyStopT=true;
                	  	   	    }
                	  	   	    #ifdef _DEBUG_
                              cout <<"用户主体服务状态:t_fTardyStop="<<t_fTardyStop<<endl;
                              #endif
                	  	   	    if (t_fTardyStopT&&(r_userServStates[x].m_servStateCode == '0')&&((x+1)==t_size))//欠停又开机
                	  	   	    {
                	  	   	    	   if (((r_userServStates[x].m_startDate >= m_cycParam.m_beginTime) && (r_userServStates[x].m_startDate <= m_cycParam.m_endTime)) ||
                                  ((r_userServStates[x].m_endDate >= m_cycParam.m_beginTime) && (r_userServStates[x].m_endDate <= m_cycParam.m_endTime)) ||
                                  ((r_userServStates[x].m_startDate <= m_cycParam.m_beginTime) && (r_userServStates[x].m_endDate >= m_cycParam.m_endTime)))
                                  {
                                        t_flag = true;
                                        t_fNormalT=true;
                                        #ifdef _DEBUG_
                                        cout <<"用户状态变化：停保->欠停->开机 ,按天算"<<endl;
                                        #endif
                                        m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime > r_userServStates[x].m_startDate ? m_cycParam.m_beginTime : r_userServStates[x].m_startDate;
                                        break;
                                   }
                	  	   	    }
                                  // add by yangyb 20100727
                                   else
                                      break;
                                 // add by yangyb 20100727 end                	  	   	    
                	  	   }
                	  	  if (t_fNormalT)
                	  	    break;
                    }
                }
                
                if (t_fApplyStop&&(r_userServStates[m].m_servStateCode == '0'))//有停机并且有开机状态
                {
                	  if(m+1 < r_userServStates.size())
                	  {
                	  	   for (int n=m+1; n < r_userServStates.size(); ++n)
                	  	   {
                	  	   	    if((r_userServStates[m+1].m_servStateCode!='b') &&(r_userServStates[m+1].m_servStateCode!='B')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='5') && (r_userServStates[m+1].m_servStateCode!='7')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='A') && (r_userServStates[m+1].m_servStateCode!='P')&&
                	  	   	    	(r_userServStates[m+1].m_servStateCode!='Q'))
                    	   	         break;
                	  	   	    #ifdef _DEBUG_
                              cout <<"用户主体服务状态:r_userServStates[n].m_servStateCode="<<r_userServStates[n].m_servStateCode<<","<<n<<endl;
                              #endif
                	  	   	    if (r_userServStates[n].m_servStateCode=='b'||r_userServStates[n].m_servStateCode=='B'
                	  	   	    	 ||r_userServStates[n].m_servStateCode=='5'||r_userServStates[n].m_servStateCode=='7'
                	  	   	    	 ||r_userServStates[n].m_servStateCode=='A'||r_userServStates[n].m_servStateCode=='P'
                	  	   	    	 ||r_userServStates[n].m_servStateCode=='Q')//欠费停机
                	  	   	    {
                	  	   	    	   t_fTardyStop=true;
                	  	   	    }
                	  	   	    #ifdef _DEBUG_
                              cout <<"用户主体服务状态:t_fTardyStop="<<t_fTardyStop<<"n="<<n<<",t_size="<<t_size<<endl;
                              #endif
                	  	   	    if (t_fTardyStop&&(r_userServStates[n].m_servStateCode == '0')&&((n+1)==t_size))//欠停又开机
                	  	   	    {
                	  	   	    	   t_fNormal=true;
                	  	   	    	   #ifdef _DEBUG_
                	  	   	    	   cout <<"用户状态变化：停保->复机->欠停->开机 ,按月算"<<endl;
                	  	   	    	   #endif
                	  	   	    	   break;
                	  	   	    }
                	  	   }
                	  	  if (t_fNormal)
                	  	    break;
                	  }
                    else
                    {
                    	  if (((r_userServStates[m].m_startDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_startDate <= m_cycParam.m_endTime)) ||
                        ((r_userServStates[m].m_endDate >= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate <= m_cycParam.m_endTime)) ||
                        ((r_userServStates[m].m_startDate <= m_cycParam.m_beginTime) && (r_userServStates[m].m_endDate >= m_cycParam.m_endTime)))
                        {
                            t_flag = true;
                            #ifdef _DEBUG_
                            cout <<"用户状态变化：停保->开机 ,按天算"<<endl;
                            #endif
                            m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime > r_userServStates[m].m_startDate ? m_cycParam.m_beginTime : r_userServStates[m].m_startDate;
                            break;
                        }
                    }
                }
                //if (t_fNormal)
                	  	 //break;
                //Modified by yangyb 20100609 end
            }
          
    
        }
        //如果未找到
        if (!t_flag)
        {
            m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime;
            #ifdef _DEBUG_
            cout <<"用户状态变化：找到用户状态 ,按月算"<<endl;
            #endif
        }
    }

//add by songyx 20100706 累积量折算 end
    else
    {
        m_cdr[ATTRI_EARLIEST_OPEN_DATE] = m_cycParam.m_beginTime;
    }
    
    T_DATE_TIME t_openDate;
    t_openDate=m_cdr[ATTRI_OPEN_DATE].value();
    if(t_openDate >= m_cycParam.m_beginTime )
    {
        m_cdr[ATTRI_EARLIEST_OPEN_DATE]=m_cdr[ATTRI_OPEN_DATE];
    }
  
  #ifdef _DEBUG_
    cout << " m_cdr[ATTRI_EARLIEST_OPEN_DATE] : " << m_cdr[ATTRI_EARLIEST_OPEN_DATE] << endl;
  #endif
  
  //add by gaojf 2009-2-18 14:37
  m_cycDays = getMonthDays(t_cdrCycle);
  //设置第一次开打时间
  setFirstCallDate(); 
  
  //设置ATTRI_BEGIN_DATETIME
  m_cdr[ATTRI_BEGIN_DATETIME] = m_cycParam.m_beginTime;
  //设置ATTRI_BEGIN_WEEK
  DateTime  t_beginDate;
  t_beginDate.set(m_cdr[ATTRI_BEGIN_DATE].c_str(),m_cdr[ATTRI_BEGIN_TIME].c_str());
  m_cdr[ATTRI_BEGIN_WEEK] = t_beginDate.getWeekDay();
  
  
  if(m_bizType == BIZ_TYPE_ONETIMES ||
     m_bizType == BIZ_TYPE_BILL||
     m_bizType == BIZ_TYPE_GRPBILL||
     m_bizType == BIZ_TYPE_ACCTBILL
     )
  { //出帐事件或一次性费用事件
    m_acctDay = atoi(m_cdr[ATTRI_ACCT_DATE].c_str());
  }else
  { //普通清单
    m_acctDay = atoi(m_cdr[ATTRI_BEGIN_DATE].c_str());
  }
  //added by zhaoly for 3G at 2009-5-16 01:19上午
  vector<PartyPrice> t_partyPriceInfo;
    if (m_cdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_UNICOMSP&&
        m_cdr[ATTRI_SOURCE_TYPE]=="4D"&&
        m_cdr[ATTRI_SP_SRVTYPE]!="70")//VAC话单 70为炫铃
    {
        if (m_paramMgr->getProductInfo(m_cdr[ATTRI_SP_SRVID],m_beginDateTime,t_partyPriceInfo)==false)
        {
            setErrInfo(990,"产品ID未定义");
            return false;
        }
        else
        {
            if (t_partyPriceInfo.size()>2)
            {
                setErrInfo(991,"一个产品在TD_B_PARTY_PRICE中记录数超过2");
                return false;
            }
            //设置M/T的初始值
            m_cdr[ATTRI_M_TIMES]=0;
            m_cdr[ATTRI_T_TIMES]=0;
            //解析paramvalue;
            for (vector<PartyPrice>::iterator priceItr=t_partyPriceInfo.begin();
                 priceItr!=t_partyPriceInfo.end();++priceItr)
            {
                ParsedPartyPrice t_parsedPartyPrice;
                t_parsedPartyPrice.m_productCode=priceItr->m_productCode;
                t_parsedPartyPrice.m_billingModeCode=priceItr->m_billingModeCode;
                t_parsedPartyPrice.m_paramCode=priceItr->m_paramCode;
                vector<string> t_splitParamValue;
                string t_paramValue=priceItr->m_paramValue;
                t_splitParamValue=StringUtil::split(t_paramValue,";");
                for (vector<string>::iterator paramitr=t_splitParamValue.begin();
                     paramitr!=t_splitParamValue.end();++paramitr)
                {
                    //Fee=1M+100;FeeType=2;BillingUnit=1;MaxFee=0;MinFee=0
                    int pos1=(*paramitr).find_first_of("=");
                    string t_key=(*paramitr).substr(0,pos1);
                    string t_value=(*paramitr).substr(pos1+1);
                    if (t_value.find_first_of("MT")!=string::npos)
                    {
                        if (t_value.find_first_of("+")!=string::npos)
                        {
                            char *pTmp;
                            string sTmp;
                            long lvalue;
                            pTmp=strtok(const_cast<char *>(t_value.c_str()),"+");
                            if (t_value.find_first_of("M")!=string::npos)
                            {
                                t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type("M",pTmp));
                                //设定M/T值
                                m_cdr[ATTRI_M_TIMES]=atol(pTmp);
                            }
                            else
                            {
                                t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type("T",pTmp));
                                //设定M/T值
                                m_cdr[ATTRI_T_TIMES]=atol(pTmp);
                            }
                            pTmp=strtok(NULL,"+");
                            t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type(StringUtil::toLowerCase(t_key),pTmp));
                            #ifdef _DEBUG_
                            cout << "key : " << StringUtil::toLowerCase(t_key) << "    value : " << pTmp <<endl;
                            #endif
                        }
                        else
                        {
                            if (t_value.find_first_of("M")!=string::npos)
                            {
                                t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type("M",t_value));
                                //设定M/T值
                                m_cdr[ATTRI_M_TIMES]=atol(t_value.c_str());
                            }
                            else
                            {
                                t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type("T",t_value));
                                //设定M/T值
                                m_cdr[ATTRI_T_TIMES]=atol(t_value.c_str());
                            }
                        }
                    }
                    else//Key值统一转成小写
                    {
                        t_parsedPartyPrice.m_prodPriceElement.insert(map<string,string>::value_type(StringUtil::toLowerCase(t_key),t_value));
                        #ifdef _DEBUG_
                        cout << "key : " << StringUtil::toLowerCase(t_key) << "    value : " << t_value <<endl;
                        #endif
                    }
                    #ifdef _DEBUG_
                    cout << "t_key   : " << StringUtil::toLowerCase(t_key) << endl;
                    cout << "t_value : " << t_value <<endl;
                    #endif
                }
                m_parsedPartyPriceInfo.push_back(t_parsedPartyPrice);
            }
            if ((m_cdr[ATTRI_T_TIMES].value()+m_cdr[ATTRI_M_TIMES].value())!=0)
            {
                //一个产品只能按M或者T计费
                m_cdr[ATTRI_DURATION]=m_cdr[ATTRI_T_TIMES].value()==0?m_cdr[ATTRI_DURATION].value():m_cdr[ATTRI_T_TIMES].value();
                m_cdr[ATTRI_DURATION]=m_cdr[ATTRI_M_TIMES].value()==0?m_cdr[ATTRI_DURATION].value():m_cdr[ATTRI_M_TIMES].value();
            }
        }
    }
    
  m_comBill.m_ifExistUserInfo = false;
  
  //add by xuf 2009-12-27 21:27:00
  if(!m_comBill.initUser(m_billUserId,m_cycParam))
  {
  	DEBUG_PRINT("[%s:%d] Initialize comBill error!\n",__FILE__,__LINE__);
    return false;	
  }

  
  return true;
}

//话单批价后处理
bool RateGlobalInfo::finalize()
{
  bool t_bRet=true;
  if(m_cdr.finalize()==false)
  {
    t_bRet = false;
  }
  /* 在重做模块中+1，批价模块不改变
  if(m_cdr.m_errorCode == E_DEFAULT) 
  {
    m_cdr[ATTRI_RATE_TIMES]=m_cdr[ATTRI_RATE_TIMES].value()+1;
  }
  */
  for(int i=1;i<FEEFIELDS_MAX;++i)
  {
    if(strlen(m_cdr[ATTRI_DISCFEE_FIELD1+i-1].c_str())<=0)
    {
      m_cdr[ATTRI_DISCFEE_FIELD1+i-1]=0;
    }
    if(strlen(m_cdr[ATTRI_DISCFEE_FIELD1+i-1+5].c_str())<=0)
    {
      m_cdr[ATTRI_DISCFEE_FIELD1+i-1+5]=0;
    }
    if(strlen(m_cdr[ATTRI_DISCFEE_FIELD1+i-1+10].c_str())<=0)
    {
      m_cdr[ATTRI_DISCFEE_FIELD1+i-1+10]=0;
    }
  }
  
  return t_bRet;
}

//判断是否该判为延迟话单
bool RateGlobalInfo::isDelayedCdr()
{
  if( m_pConfig->m_channelType != "P")
  {
    return false;
  }
  // 回退用户延迟处理
  if(m_paramMgr->isRedoUser(m_userId)==true)
  {
    return true;
  }
  //对于单帐期运行模式,跨月期间,下月的话单作为延迟话单处理
  if(m_pConfig->m_doubleCycMode == CYCLEMODE_ONE &&
     m_cycParam.m_cycleId > m_pSysCycParam->m_cycleId )
  {
    return true;
  }
  return false;
}

void RateGlobalInfo::setFileName(const char *r_fileName)
{
  m_fileName = r_fileName;
  m_rateLog->setFileName(r_fileName);
  m_pCdr[ATTRI_FILE_NAME] = r_fileName;
}

void RateGlobalInfo::getFeeFields(bool r_feeFields[FEEFIELDS_MAX]) const
{
  int t_field = 0;
  //初始化，所有的均不需要求取
  memset(r_feeFields,0,sizeof(bool)*FEEFIELDS_MAX);

  //1. GSM业务       1
  //   长途类型1 !=0 3
  //   长途类型2 !=0 5
  //   特殊情况：音信互动业务 5
  //             陕西西咸一体化 2
  //2. SM业务
  //    点对点或梦网上行 1
  //    SP安条           1/2
  //    SP包月话单       3         
  //3. MISC  (是否可以细化)
  //    通讯费 1
  //    信息费 2
  //    包月费 3
  //4. GPRS    1
  //5. 193/17911  同GSM
  //6. SP
  //   1 
  if (m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM.c_str() ||
      m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE.c_str() )
  {
  	r_feeFields[1]=true;
  	r_feeFields[5]=true;
  	if(m_pCdr[ATTRI_LONG_TYPE1] != LONG_TYPE_NONE)
  	{ //长途类型
  	  r_feeFields[3]=true;
  	}
  	if(m_pConfig->m_provinceCode == "290" &&
  	   m_pConfig->m_operatorType == OPERATOR_TYPE_CMCC)
  	{ //陕西移动： 西咸一体化,收取额外费用，放在附加费上
  	  if(((m_pCdr[ATTRI_HOME_CODE_A] == "029"  && m_pCdr[ATTRI_VISIT_CODE_A] == "0910")|| 
  	      (m_pCdr[ATTRI_HOME_CODE_A] == "0910" && m_pCdr[ATTRI_VISIT_CODE_A] == "029"))&&
  	      (m_pCdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC ||
  	       m_pCdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC ))
  	  {
  	    r_feeFields[2]=true;
  	  }
  	}
  }
  else if (m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SMS.c_str()||
           m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXSMS.c_str())
  {
    if (m_pCdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_IINTER)
    { //国际短信 的费用
      r_feeFields[1]=true;
      r_feeFields[2]=true;     
 NONE)
  	{ //闀块