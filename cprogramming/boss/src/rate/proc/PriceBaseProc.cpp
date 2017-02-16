#include "PriceBaseProc.h"
#include "RateError.h"
#include "PMManager.h"
#include "EventFeePolicy.h"
#include "Price.h"
#include "NodeBase.h"
#include "base/exceptions.h"
#include "base/StringUtil.h"
#include "appfrm/ErrorHandle.h"

bool PriceBaseProc::initialize()
{
  if(Procedure::initialize()==false)
    return false;
  m_tpRentMgr.initialize(m_pRateInfo);
  return true;
}

int PriceBaseProc::execute()
{
   #ifdef FP_DEBUG
    FP_BEGIN(PriceBaseProc)
  #endif
	//错单,不进行处理
	if (m_pcdr[ATTRI_ERROR_CODE] != E_DEFAULT)
	{ return 1;}
	//话单级初始化
	if(!cdr_init()) {return 1;}
  
	//设置话单是否有原始费用信息标志
	ProcedureHelper::ExecuteProcedure("SetOriginFeeFlag");
	  
  if(getCdrSegments()==false) 
  {
    m_pRateInfo->m_pErrorMsgs->insert(E_CDR_PRICE,"批价有 " + StringUtil::toString(m_errCode) + " 错单产生:" + StringUtil::toString(__LINE__));
    return 1;
  }
	//设置原始费用信息
	setOrgFeeInfo();
	/*
	#ifdef _DEBUG_
	  cout<<"设置原始费用后 setOrgFeeInfo "<<__FILE__<<__LINE__<<endl;
	  cout<<"------------------------------------------"<<endl;
	  for(int t_ii=1;t_ii<FEEFIELDS_MAX;++t_ii)
	  {
	    if(m_pRateFeeInfo->m_cdrFeeInfo.m_noRateFlag[t_ii]==false)
	    {
        cout<<m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[t_ii]<<endl;
      }
    }
	  cout<<"------------------------------------------"<<endl;
	#endif
	*/
	
	//取计费明细账单项
	T_BILL_ID t_billId;
	//Added By Sunam 2009/10/8 20:07:46
	m_rateBillSet.clear();
	for(int i=1;i<FEEFIELDS_MAX;++i)
  {
    if(m_pRateInfo->m_hasFeeFlag[i]==true)
    {
      if(m_pmManager->getRateDetailBill(i,m_pRateInfo->m_bizType,
                           m_pRateInfo->m_processRule->m_rateFeeIdFactorIds,
                           m_pcdr,
                           m_pRateInfo->m_cycParam.m_cycleId,
                           t_billId)==false)
      {
        setErrorCode(E_GET_RATE_BILLITEM);
        return 1;
      }
      m_pRateFeeInfo->m_billIdList[i] = t_billId;
      //Added By Sunam 2009/10/8 20:07:46
      m_rateBillSet.insert(t_billId);
    }
  }
   #ifdef FP_DEBUG
    FP_END(PriceBaseProc)
  #endif

   #ifdef FP_DEBUG
    FP_BEGIN(pricing)
  #endif
	//批价
  if(!pricing())  {return 1;}
   #ifdef FP_DEBUG
    FP_END(pricing)
  #endif
  
  return 0;  
} 

//话单级初始化
bool PriceBaseProc::cdr_init()
{
  m_pTpList     = &(m_pRateInfo->m_tpInfoList);
  m_pRateTpList = &(m_pRateInfo->m_pRateTpList);
  m_errCode     = E_DEFAULT;
  m_tpRentMgr.cdr_int();
  m_pRateFeeInfo= &(m_pRateInfo->m_rateFeeInfo);
  m_billTrack[0]=0;
  m_billTrackLen=0;
  return true;
}
//设置一些资费参数信息(成员角色、上下半月、首月标记等)
void PriceBaseProc::setTpInfoToCdr(const RateTpInfo *r_tpInfo)
{
  const UserTp *t_pUserTp = r_tpInfo->m_pUserTp;
  vector<UserTpParam>::const_iterator t_paramItr;
  m_pcdr[ATTRI_HALF_INNET_FLAG] = t_pUserTp->m_halfInNetFlag;
  m_pcdr[ATTRI_CURRENT_INNET_FLAG] = t_pUserTp->m_firstMonthFlag;
  m_pcdr[ATTRI_TPVPN_FLAG]        = t_pUserTp->m_vpnFlag;
  m_pcdr[ATTRI_TPVPN_FLAGA] = t_pUserTp->m_memberType_A;
  m_pcdr[ATTRI_TPVPN_FLAGB] = t_pUserTp->m_memberType_B;
  m_pcdr[ATTRI_TPCELL_FLAG]       = t_pUserTp->m_cellFlag;
  m_pcdr[ATTRI_TPRELA_LEVELTYPE] = LEVELTYPE_NULL;

  // add by chenm 2009-4-15 8:58:10
  m_pcdr[ATTRI_TPVPN_LAST_PARENT_ID_A]       = t_pUserTp->m_lastVpnParentIDA;
  m_pcdr[ATTRI_TPVPN_LAST_PARENT_ID_B]       = t_pUserTp->m_lastVpnParentIDB;
  // over 2009-4-15 8:58:10
	
  if(t_pUserTp->m_cellFlag == FLAG_YES)
  {
    m_pcdr[ATTRI_CELL_FLAG]=FLAG_YES;
  }
  if(t_pUserTp->m_subTpType == TP_TYPE_CELL)
  {
    for(t_paramItr = t_pUserTp->m_paramList.begin();
        t_paramItr!= t_pUserTp->m_paramList.end();++t_paramItr)
    {
      if(t_paramItr->m_relatinType == T_RELATYPE_SPECIALCELL)
      {
      m_pcdr[ATTRI_TPRELA_LEVELTYPE] = t_paramItr->m_levelType;
      break;
    }
    }
  }else //t_pUserTp->m_subTpType == TP_TYPE_NORMAL
  {
    if(!(t_pUserTp->m_relationVpnId == NULL_VPN_ID))//关联群
    {
      for(t_paramItr = t_pUserTp->m_paramList.begin();
          t_paramItr!= t_pUserTp->m_paramList.end();++t_paramItr)
      {
        if(t_paramItr->m_relatinType == T_RELATYPE_SPECIALPHONE)
        {
        m_pcdr[ATTRI_TPRELA_LEVELTYPE] = t_paramItr->m_levelType;
        m_pcdr[ATTRI_TPVPN_FLAGB]      = t_paramItr->m_memberType; 
        break;
      }
    }
  }
  }
  if(  t_pUserTp->m_subTpType == TP_TYPE_VPN ||
     !(t_pUserTp->m_relationVpnId == NULL_VPN_ID))
  {
    //江苏联通特殊从处理
    //对方号码为*8开头,判为集团内 2009-2-20 20:30 gaojf
    if(m_pRateInfo->m_pConfig->m_provinceCode=="034" &&
       strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"*8",2)==0)
    {
      m_pcdr[ATTRI_VPN_FLAG]   = FLAG_YES;
      m_pcdr[ATTRI_TPVPN_FLAGB]= VPN_MEMTYPE_VPNINNET;
    }
  }
}
void PriceBaseProc::setBillInfos()
{
  m_pRateInfo->generateRollBill();
  m_billTrackLen = 0;m_billTrack[0]=0;
  //Added By Sunam 2009/9/11 10:31:08
  vector<int> detailBillItemCode;
  vector<long> detailBillItemFee;
  for(vector<RollBackBill>::const_iterator t_itr=
        m_pRateInfo->m_rollBackBills.begin();
      t_itr!=m_pRateInfo->m_rollBackBills.end();
      ++t_itr)
  {
    if(t_itr->m_value == 0) continue;
    if(m_billTrackLen>0){sprintf(m_billTrack+m_billTrackLen,";");++m_billTrackLen;}
    if(t_itr->m_userId == m_pRateInfo->m_userId)
    {
      sprintf(m_billTrack+m_billTrackLen,"0|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }else
    {
      sprintf(m_billTrack+m_billTrackLen,"%s|%d|%d|%ld|%ld|%d|%ld",
                t_itr->m_userId.c_str(),
                t_itr->m_billType,
                t_itr->m_billCycle,
                t_itr->m_tpObjId,
                t_itr->m_beginTime,
                t_itr->m_billId,
                t_itr->m_value);
    }
   m_billTrackLen = strlen(m_billTrack);
   //Added By Sunam 2009/9/11 10:31:08
   if ((t_itr->m_billType == BILL_TYPE_USER) && (m_rateBillSet.find(t_itr->m_billId) == m_rateBillSet.end()))
   {
     detailBillItemCode.push_back(t_itr->m_billId);
     detailBillItemFee.push_back(t_itr->m_value);
   }
  }
  //Added By Sunam 2009/9/11 10:31:08 Begin
  char detailBillTrack[100];
  int length = 0;
  memset(detailBillTrack,0,100);
  if (detailBillItemCode.size() > 0)
  {
  for (int j=0; j < detailBillItemCode.size(); ++j)
  {
    if (length > 0)
    {
        sprintf(detailBillTrack+length,";");
        ++length;
    }
    sprintf(detailBillTrack+length,"%d|%ld",detailBillItemCode[j],detailBillItemFee[j]);
    length = strlen(detailBillTrack);
  }
  }
  //Added By Sunam 2009/11/30 21:49:21 如果二批费用全为0
  else
  {
    T_TIME t_begintTime;
    T_BILL_ID t_billId;
    //费用域1标准资费大于0，优惠费用为0
    if ((m_pRateInfo->m_cdr[ATTRI_DISCFEE_FIELD1].value() == 0) &&
    	(m_pRateInfo->m_cdr[ATTRI_FEE_FIELD1].value() > 0))
	{    	
		FeeInfo &t_feeInfo=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD1];
		for (int seg = 0; seg < t_feeInfo.m_seg; ++seg)
		{
		    SegFeeInfo &t_segFeeInfo = t_feeInfo.m_segInfoList[seg];
		    t_begintTime = t_segFeeInfo.m_startDateTime.getTimeInteger();
		    if(m_pmManager->getAcctDetailBill(ATTRI_DISCFEE_FIELD1,
		                                m_pRateInfo->m_bizType,
		                                m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
		                                m_pcdr,
		                                m_pRateInfo->m_cycParam.m_cycleId,
		                                t_segFeeInfo.m_feePolicyId,
		                                t_begintTime,
		                                t_billId))
		    {
		        sprintf(detailBillTrack+length,"%d|0",(long)t_billId);
		        length = strlen(detailBillTrack);
		        break;
		    }
		}
	}
	//费用域2标准资费大于0，优惠费用为0
    if ((m_pRateInfo->m_cdr[ATTRI_DISCFEE_FIELD2].value() == 0) &&
    	(m_pRateInfo->m_cdr[ATTRI_FEE_FIELD2].value() > 0))
	{    	
		FeeInfo &t_feeInfo=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD2];
		for (int seg = 0; seg < t_feeInfo.m_seg; ++seg)
		{
		    SegFeeInfo &t_segFeeInfo = t_feeInfo.m_segInfoList[seg];
		    t_begintTime = t_segFeeInfo.m_startDateTime.getTimeInteger();
		    if(m_pmManager->getAcctDetailBill(ATTRI_DISCFEE_FIELD2,
		                                m_pRateInfo->m_bizType,
		                                m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
		                                m_pcdr,
		                                m_pRateInfo->m_cycParam.m_cycleId,
		                                t_segFeeInfo.m_feePolicyId,
		                                t_begintTime,
		                                t_billId))
		    {
		    	if (length>0)
				{
		        	sprintf(detailBillTrack+length,";%d|0",(long)t_billId);
		        }
		        else
				{
		        	sprintf(detailBillTrack+length,"%d|0",(long)t_billId);
		        }
		        length = strlen(detailBillTrack);
		        break;
		    }
		}
	}
	//费用域3标准资费大于0，优惠费用为0
    if ((m_pRateInfo->m_cdr[ATTRI_DISCFEE_FIELD3].value() == 0) &&
    	(m_pRateInfo->m_cdr[ATTRI_FEE_FIELD3].value() > 0))
	{    	
		FeeInfo &t_feeInfo=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD3];
		for (int seg = 0; seg < t_feeInfo.m_seg; ++seg)
		{
		    SegFeeInfo &t_segFeeInfo = t_feeInfo.m_segInfoList[seg];
		    t_begintTime = t_segFeeInfo.m_startDateTime.getTimeInteger();
		    if(m_pmManager->getAcctDetailBill(ATTRI_DISCFEE_FIELD3,
		                                m_pRateInfo->m_bizType,
		                                m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
		                                m_pcdr,
		                                m_pRateInfo->m_cycParam.m_cycleId,
		                                t_segFeeInfo.m_feePolicyId,
		                                t_begintTime,
		                                t_billId))
		    {
		    	if (length>0)
				{
		        	sprintf(detailBillTrack+length,";%d|0",(long)t_billId);
		        }
		        else
				{
		        	sprintf(detailBillTrack+length,"%d|0",(long)t_billId);
		        }
		        length = strlen(detailBillTrack);
		        break;
		    }
		}
	}
  }
  if (strlen(detailBillTrack) > 0)
  {
    sprintf(m_billTrack+m_billTrackLen,":%s",detailBillTrack);
  }
  //Added By Sunam 2009/9/11 10:31:08 End
  m_pcdr->setSpLongValue(ATTRI_SPLONG_BILLLIST,m_billTrack); 
}

void PriceBaseProc::commitFldSendMin()
{
  bool t_flag = false;
  vector<RateBillInfo>::iterator t_itr;
  for(vector<RateBillInfo>::const_iterator t_itr1=m_fldSendMinList.begin();
      t_itr1!=m_fldSendMinList.end();++t_itr1)
  {
    for(t_itr=m_tpSendMinList.begin();t_itr!=m_tpSendMinList.end();++t_itr)
    {
      if(*t_itr == *t_itr1)
      {
        if(t_itr->m_billValue < t_itr1->m_billValue)
        { //取最大的作为最终结果
          t_itr->m_billValue = t_itr1->m_billValue;
        }
        t_flag = true;
        break;
      }
    }
    if(t_flag == false)
    {
      m_tpSendMinList.push_back(*t_itr1);
    }
  }
}
//提交赠送优惠量信息
void PriceBaseProc::commitSendBillInfo(const RateBillInfo &r_billInfo)
{
  if(r_billInfo.m_billId.m_billType == BILL_TYPE_SUM1)
  {
    UserSumBill1  t_sumBill1;
    t_sumBill1.m_userId = r_billInfo.m_billId.m_userId;
    t_sumBill1.m_tpObjId= r_billInfo.m_billId.m_tpObjId;
    t_sumBill1.m_billId = r_billInfo.m_billId.m_billId;
    t_sumBill1.m_value  = r_billInfo.m_billValue;
    //Added By Sunam 2009/8/8 10:16:48
    t_sumBill1.m_maxValue    = r_billInfo.m_maxValue;
    t_sumBill1.m_feePolicyId = r_billInfo.m_billId.m_feePolicyId;
    m_pRateInfo->updateBillIncre<UserSumBill1>(m_pRateInfo->m_userSumBill1s_new,t_sumBill1);
  }else if(r_billInfo.m_billId.m_billType == BILL_TYPE_SUM2)
  {
    UserSumBill2  t_sumBill2;
    t_sumBill2.m_userId   =  r_billInfo.m_billId.m_userId;
    t_sumBill2.m_tpObjId  =  r_billInfo.m_billId.m_tpObjId;
    t_sumBill2.m_beginTime=  r_billInfo.m_billId.m_sumBeginTime;
    t_sumBill2.m_endTime  =  r_billInfo.m_billId.m_sumEndTime;
    t_sumBill2.m_cycleId  =  0;
    t_sumBill2.m_billId   =  r_billInfo.m_billId.m_billId;
    t_sumBill2.m_value    =  r_billInfo.m_billValue;
    t_sumBill2.m_type     =  r_billInfo.m_billId.m_type;
    t_sumBill2.m_groupUserId=r_billInfo.m_billId.m_groupId;
    //Added By Sunam 2009/8/8 10:16:48
    t_sumBill2.m_maxValue    = r_billInfo.m_maxValue;
    t_sumBill2.m_feePolicyId = r_billInfo.m_billId.m_feePolicyId;
    m_pRateInfo->updateBillIncre<UserSumBill2>(m_pRateInfo->m_userSumBill2s_new,t_sumBill2);
//1月1日上线，26日功能割接的时候不上线

    if(t_sumBill2.m_type == 1) //集团
    { //集团账单个人也生成一份
      //add by yangyi 改为更新sumBill1 2009/9/8 10:48:48
      UserSumBill1  t_sumBill1;
      t_sumBill1.m_userId = m_pRateInfo->m_billUserId;     
      t_sumBill1.m_tpObjId= r_billInfo.m_billId.m_tpObjId;
      t_sumBill1.m_billId = r_billInfo.m_billId.m_billId;
      t_sumBill1.m_value  = r_billInfo.m_billValue;
      //Added by Sunam 2009/10/27 22:01:56
      t_sumBill1.m_maxValue   = r_billInfo.m_maxValue;
      //t_sumBill2.m_userId   =  m_pRateInfo->m_billUserId;
      //m_pRateInfo->updateBillIncre<UserSumBill2>(m_pRateInfo->m_userSumBill2s_new,t_sumBill2);
      m_pRateInfo->updateBillIncre<UserSumBill1>(m_pRateInfo->m_userSumBill1s_new,t_sumBill1);
      m_pRateInfo->m_cdr[ATTRI_FREE_TIME_ALL] =  r_billInfo.m_billValue;     
    }
  }
}

//批价
bool PriceBaseProc::pricing()
{
  vector<int>::iterator t_IndexItr1,t_IndexItr2;
  int t_feeField;  //当前处理的费用域
  int t_selectIdx; //选取的费用结果
  long t_discFee_All=0;
  long t_baseFee_All=0;
  
  m_pRateFeeInfo->m_curFeeIdx = 0;
  m_pRateFeeInfo->m_cdrFeeInfo2 = m_pRateFeeInfo->m_cdrFeeInfo;
  for(t_feeField=1;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    m_continueProcessFlag[t_feeField] = true;
  }
  
  for(t_IndexItr1 = m_pRateTpList->begin();
      t_IndexItr1!= m_pRateTpList->end();
      t_IndexItr1 = t_IndexItr2)
  {
    m_pRateFeeInfo->resizeCdrFeeInfo(m_pRateFeeInfo->m_curFeeIdx+1);
    t_IndexItr2 = t_IndexItr1+1;
    RateTpInfo &t_rateTp = (*m_pTpList)[*t_IndexItr1];
    setTpInfoToCdr(&t_rateTp);
    //对t_rateTp进行批价
    if(eventPrice(t_rateTp)==false)
    {
      if(m_errCode != E_DEFAULT)
      { //批价过程中出错了
        m_pRateInfo->m_pErrorMsgs->insert(E_CDR_PRICE,"批价有 " + StringUtil::toString(m_errCode) + " 错单产生:" + StringUtil::toString(__LINE__));
        return false;
      }else
      {
        #ifdef _DEBUG_
          cout<<"t_rateTp.m_pEventTp->m_id = "<<t_rateTp.m_pEventTp->m_id
              <<" 无满足条件的资费 "<<__FILE__<<__LINE__<<endl;
        #endif
      }
    }else
    { //该政策批价通过
      m_pRateFeeInfo->m_curFeeIdx++;
		t_rateTp.m_isDealed = true;  
    }
    //优先级变化,或者是最后一个TP,对作用结果进行选择
    if(t_IndexItr2   == m_pRateTpList->end()||
       (*m_pTpList)[*t_IndexItr1].m_priority != 
       (*m_pTpList)[*t_IndexItr2].m_priority )
    {
      //Added By Sunam 2009/9/2 11:08:08
     // m_continueProcessFlag = true;
      selectTpResult();
      m_pRateFeeInfo->m_curFeeIdx = 0; //当前待选择的费用结果归零
      m_pRateFeeInfo->m_cdrFeeInfo2 = m_pRateFeeInfo->m_cdrFeeInfo;
      //Added By Sunam 2009/9/2 11:08:45 不再继续处理
//      if (!m_continueProcessFlag)
//      {
//        break;
//      }
    } //end if
    
  }// end for
  //至此m_pRateInfo->m_feeInfo为批价结果
  
  //一些费用特殊处理
  specialProcess();
  //设置费用项,跳次项,及政策代码
  setFeeResult();
  //计算套餐月租费实收费用m_tpRentFeeList
  setRateTrack();
 
  //合明细账单(来访用户不合帐)
  if ((m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_NATIONAL_IN) && 
      (m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_INTERNATIONAL_IN)&&
      m_pRateConfig->m_channelType == CHANNEL_PROV)
  {
    if(setDetailBill()==false)
    {
      m_pRateInfo->m_pErrorMsgs->insert(E_CDR_PRICE,"批价有 " + StringUtil::toString(m_errCode) + " 错单产生:" + StringUtil::toString(__LINE__));
      return false;
    }
  }else
  {
    t_discFee_All = t_baseFee_All = 0;
    for(t_feeField=1;t_feeField<FEEFIELDS_MAX;++t_feeField)
    {
      t_discFee_All += m_pRateInfo->m_cdr[t_feeField].value();
      t_baseFee_All += m_pRateInfo->m_cdr[t_feeField+5].value();
    }
    m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_baseFee_All;
    m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_discFee_All;
  }
  //合特殊的账单,并提交
  ProcedureHelper::ExecuteProcedure("SpecialBillProc");
  
  //设置明细账单轨迹,便于回退
  setBillInfos();
  
  return true;
}
//一个事件类型政策对一条话单的批价  
bool PriceBaseProc::eventPrice(RateTpInfo &r_rateTp)
{
  /*
  #ifdef _DEBUG_
    cout<<"政策定义如下："<<__FILE__<<__LINE__<<endl;
    char t_tmpChar[10];sprintf(t_tmpChar,"  ");
    r_rateTp.m_pEventTp->dump(t_tmpChar);
    cout<<"-----------------------------------"<<endl;
  #endif  
  */
    if (!m_pRateInfo->m_ExclusivedEventFeepolicy.empty()&&
        m_pRateInfo->m_ExclusivedEventFeepolicy.find(r_rateTp.m_pEventTp->m_id)!=m_pRateInfo->m_ExclusivedEventFeepolicy.end())
    {
        #ifdef _DEBUG_
        cout << __FILE__ <<" : "<<__LINE__<<" : 被互斥掉的事件ID : "<<r_rateTp.m_pEventTp->m_id<<endl;
        #endif  
        return false;
    }
  
  
  NodeBase::m_policyInfo.m_rateTpInfo = &r_rateTp;
  CdrFeeInfo &t_cdrFeeInfo= m_pRateFeeInfo->m_cdrFeeInfos[m_pRateFeeInfo->m_curFeeIdx];
  bool t_cdrRateFlag = false; //该事件政策对话单是否有效(或部分有效)
  m_pRateFeeInfo->m_pCurRateTpInfo = &r_rateTp;
  t_cdrFeeInfo.clear();
  t_cdrFeeInfo.m_cdrCompFlag = r_rateTp.m_compMethod;
  for(int t_feeField=1;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    //跳过无需处理的费用项
    if(m_pRateInfo->m_hasFeeFlag[t_feeField]==false) continue;
        
    if (m_continueProcessFlag[t_feeField] == false) continue;
    
    EventFeePolicy * t_eventTp = r_rateTp.m_pEventTp;
    m_pRateFeeInfo->m_curFeeField = t_feeField;
    for(;t_eventTp!=NULL;t_eventTp=t_eventTp->m_next)
    {
      if(t_eventTp->m_feeId == t_feeField)
      {
        m_pRateFeeInfo->m_pCurEventPolicy=t_eventTp;
        //根据费用域设置话单类型
        //Add by yangb For sx REQ12 At 2009/12/31 15:48:33
        if (t_feeField == 1)//市话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_BRECORD_TYPE];
        if (t_feeField == 3)//长话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];
        if (t_feeField == 5)//信息费(和长话共用一个话单类型字段，并且不重复)
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];
        
        if(rateTpPrice(t_eventTp->m_price)==false)
        {
          if(m_errCode!= E_DEFAULT)
          { //批价过程中出错了
            return false;
          }
        }else
        {
          t_cdrRateFlag = true;
          t_cdrFeeInfo.m_feeInfo[t_feeField].m_pRateTpInfo = &r_rateTp;
        }
        break;
      }else continue;
    }
  }
  return t_cdrRateFlag;
}


//一个事件类型政策对一个费用域的批价  
bool PriceBaseProc::rateTpPrice(Price *r_price)
{
  bool       t_effectFlag = false; //该政策对该作用域是否有效(或部分有效)
  int        t_feeField   = m_pRateFeeInfo->m_curFeeField;
  CdrFeeInfo &t_cdrFeeInfo= m_pRateFeeInfo->m_cdrFeeInfos[m_pRateFeeInfo->m_curFeeIdx];
  FeeInfo    &t_feeInfo   = t_cdrFeeInfo.m_feeInfo[t_feeField];
  CdrFeeInfo *t_pStdCdrFeeInfo=NULL;
  m_pRateFeeInfo->m_pCurPrice = r_price;
  
  t_pStdCdrFeeInfo = &(m_pRateFeeInfo->m_cdrFeeInfo2);
  if(t_pStdCdrFeeInfo->m_noRateFlag[t_feeField] == false)
  { //设置该费用项初始批价结果
    t_cdrFeeInfo.setFeeInfo(t_feeField,t_pStdCdrFeeInfo->m_feeInfo[t_feeField]);
  }
   
  try
  {
    r_price->rateExecute(m_errCode);
    //检查该政策对该费用域是否有生效过
    for(int i=0;i<t_feeInfo.m_seg;++i)
    {
      SegFeeInfo &t_segInfo = t_feeInfo.m_segInfoList[i];
      //资费实例标识、资费标识和批价政策相等
      if(t_segInfo.m_tpObjId     == m_pRateFeeInfo->m_pCurRateTpInfo->m_pUserTp->m_tpObjId &&
         t_segInfo.m_feePolicyId == m_pRateFeeInfo->m_pCurRateTpInfo->m_rateTpId &&
         t_segInfo.m_tpId        ==  r_price->m_id           )
      {
        t_effectFlag = true;
        t_cdrFeeInfo.m_noRateFlag[t_feeField]=false;
        break;
      }
    }
    /*
    #ifdef _DEBUG_
      cout<<"----------当前批价政策结果信息!-----------"<<endl;
      cout<<t_feeInfo<<endl;
      //cout<<"------------------------------------------"<<endl;
      //cout<<"----------CUR T0批价政策结果信息!-----------"<<endl;
      //cout<<m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[t_feeField]<<endl;
      //cout<<"----------CUR T1批价政策结果信息!-----------"<<endl;
      //cout<<m_pRateFeeInfo->m_cdrFeeInfo2.m_feeInfo[t_feeField]<<endl;
      //cout<<"------------------------------------------"<<endl;
    #endif
    */
  }catch (const BusinessException &e)
  {
    m_errCode = getErrorCode();
    return false;
  }
  if(m_errCode == E_DEFAULT)
    return t_effectFlag;
  else
    return false;
}

void PriceBaseProc::setErrorCode(const int &r_errCode)
{
  m_errCode = r_errCode;
  (*m_pcdr).setErrorCode(r_errCode);
}

int PriceBaseProc::getErrorCode()
{
  return (*m_pcdr).m_errorCode;
}

bool PriceBaseProc::setFeeResult()
{
  int t_feeField;
  //设置各项费用
  for(int i=1;i<FEEFIELDS_MAX;++i)
  { //设置实收费用
    t_feeField = ATTRI_DISCFEE_FIELD1+i-1;
    setItemFee(t_feeField);
  }
	setTimes();     //置times1,times2
  return true;
}
//设置实收费用
void PriceBaseProc::setItemFee(const int &r_feeField)
{
  m_pRateInfo->m_cdr[r_feeField]  = 
      m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[r_feeField].m_feeSum;
  return;
}
//设置一次批价费用
void PriceBaseProc::setBaseFee(const int &r_feeField)
{
  m_pRateInfo->m_cdr[r_feeField+5]= 
      m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[r_feeField].m_feeSum;
  return;
}
void PriceBaseProc::setTimes()
{
  long     t_times=0;
  FeeInfo &t_feeInfo1=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD1];
  FeeInfo &t_feeInfo3=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD3];
  FeeInfo &t_feeInfo5=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD5];
  //置times1
  t_times = t_feeInfo1.m_unitCount;
  m_pRateInfo->m_cdr[ATTRI_CFEE_TIMES]= t_times;
    
  //置times2
  if(t_feeInfo3.m_unitCount!=0)
  {
    t_times = t_feeInfo3.m_unitCount;
  }else if(t_feeInfo3.m_unitCount!=0)
  {
    t_times = t_feeInfo5.m_unitCount;
  }else
  {
    t_times = 0;
  }
  m_pRateInfo->m_cdr[ATTRI_LFEE_TIMES]=t_times;
}

bool PriceBaseProc::getCdrSegments()
{
  return getCdrSegments(m_pRateFeeInfo->m_cdrSegment,m_pRateFeeInfo->m_cdrSegList);
}
//按天分割话单，形成话单天分割段列表
bool PriceBaseProc::getCdrSegments(CdrSeg &r_cdrTotalSeg,vector<CdrSeg> &r_cdrSegList)
{
  //get all CDR time segment info
  r_cdrTotalSeg.m_startDateTime.set(m_pcdr[ATTRI_BEGIN_DATE].c_str(),m_pcdr[ATTRI_BEGIN_TIME].c_str());
  r_cdrTotalSeg.m_endDateTime.set(m_pcdr[ATTRI_END_DATE].c_str(),m_pcdr[ATTRI_END_TIME].c_str());
  r_cdrTotalSeg.m_startSecond=0;
  r_cdrTotalSeg.m_duration =m_pcdr[ATTRI_DURATION].value();
  r_cdrTotalSeg.m_endSecond=r_cdrTotalSeg.m_duration;
  r_cdrTotalSeg.m_startData=r_cdrTotalSeg.m_endData=r_cdrTotalSeg.m_data=0;
  
  //校验结束时间 - 开始时间 ?= 时长
  if(r_cdrTotalSeg.m_duration != r_cdrTotalSeg.m_endDateTime-r_cdrTotalSeg.m_startDateTime)
  {
    setErrorCode(E_DATETIME_LONG);
    #ifdef _DEBUG_
      cout<<"r_cdrTotalSeg.m_startDateTime  = "<<r_cdrTotalSeg.m_startDateTime <<endl;
      cout<<"r_cdrTotalSeg.m_duration       = "<<r_cdrTotalSeg.m_duration <<endl;
      cout<<"r_cdrTotalSeg.m_endDateTime    = "<<r_cdrTotalSeg.m_endDateTime <<endl;
    #endif
    return false;
  }

  DateTime tmp1_tp,tmp2_tp;
  CdrSeg   tmp_fs;
  int      i = 0;
  int      firstSegFlag = 1;
  //set start time
  tmp1_tp = r_cdrTotalSeg.m_startDateTime;
  tmp_fs.m_startSecond   = 0;
        
  tmp_fs.m_startDateTime = tmp1_tp;
  tmp_fs.m_endDateTime   = r_cdrTotalSeg.m_endDateTime;
	while (strcmp(tmp1_tp.getDate(),m_pcdr[ATTRI_END_DATE].c_str())<0)
  {
      tmp2_tp                = tmp1_tp;
      tmp2_tp                = tmp2_tp.nextday();
      tmp_fs.m_endDateTime   = tmp2_tp.overnight();
      tmp_fs.m_duration      = tmp_fs.getDuration();
      tmp_fs.m_endSecond     = tmp_fs.m_startSecond+tmp_fs.m_duration;
      r_cdrSegList.push_back(tmp_fs);
      tmp1_tp                = tmp1_tp.nextday();
      tmp_fs.m_startDateTime = tmp1_tp;
      tmp_fs.m_startSecond   = tmp_fs.m_endSecond;
      //特超长话单，判为错单
      //Modified By Sunam 2009/10/7 17:11:16 宽带话单存在超长话单，因此对宽带话单最大允许30天
      //if (i >= DAYSEGS_MAX)
      //if ((i >= DAYSEGS_MAX) && (m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_NET))
      if ((m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_FIXVOICE) &&
          (m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_FIXTIMES) &&
          (m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_NET))
      {
        if (i >= DAYSEGS_MAX)
        {
          setErrorCode(E_DATETIME_LONG);
          return false;
        }
      }
      ++i;
      firstSegFlag = 0;
  }

  tmp_fs.m_endDateTime = r_cdrTotalSeg.m_endDateTime;     //得到结束时间
  tmp_fs.m_duration    = tmp_fs.getDuration();
  tmp_fs.m_endSecond   = tmp_fs.m_startSecond + tmp_fs.m_duration;
  if (tmp_fs.m_duration > 0)
  {    
    r_cdrSegList.push_back(tmp_fs);;
    ++i; 
  }else if (firstSegFlag == 1)
  {//对于0时长话单特殊处理
    tmp_fs.m_startSecond   = 0;
    r_cdrSegList.push_back(tmp_fs);;
    ++i;     //得到天数
  }
  /*
  #ifdef _DEBUG_
    cout << "＝＝＝ DAY SEGMENT INFO ＝＝＝" << endl;
    for (i=0; i<r_cdrSegList.size(); i++)
    {
        cout << "    第 -- "<<i+1<< " -- 天 "<<endl;
        cout << r_cdrSegList[i] <<endl;
    }
  #endif //_DEBUG_
  */
  return true;
}

//合明细账单
bool PriceBaseProc::setDetailBill()
{
  UserBill  t_userBill;
  T_BILL_ID t_billId;
  RateBillInfo         t_billInfo;
  vector<RateBillInfo>::iterator t_itr;
  bool      t_flag;
  long      t_discFee_All = 0;
  long      t_baseFee_All = 0;
  
  m_tpSendMinList.clear();
  for(int t_feeField=1;t_feeField<FEEFIELDS_MAX;++t_feeField)
  {
    //跳过无需处理的费用项
    if(m_pRateInfo->m_hasFeeFlag[t_feeField]==false) continue;
    t_discFee_All += m_pRateInfo->m_cdr[t_feeField].value();
    t_baseFee_All += m_pRateInfo->m_cdr[t_feeField+5].value();
    m_fldSendMinList.clear();
    FeeInfo &t_feeInfo=m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[t_feeField];
    if(t_feeInfo.m_feeSum != 0 )
    {
      //合该费用项费用
      //1.计费账单合帐
      t_userBill.clear();
      t_userBill.m_userId      = m_pRateInfo->m_billUserId;
      t_userBill.m_billId      = m_pRateFeeInfo->m_billIdList[t_feeField];
      t_userBill.m_value       = t_feeInfo.m_feeSum; 
      t_userBill.m_discntValue = t_feeInfo.m_feeSum;
      m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,t_userBill);

        //Add by yangb For sx REQ12 At 2009/12/31 17:34:59
        if (t_feeField == 1)//市话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_BRECORD_TYPE];
        if (t_feeField == 3)//长话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];
        if (t_feeField == 5)//信息费(和长话共用一个话单类型字段，并且不重复)
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];
            
        //2.帐务账单合帐
		T_TIME t_begintTime;
		for (int seg = 0; seg < t_feeInfo.m_seg; ++seg)
		{
			SegFeeInfo &t_segFeeInfo = t_feeInfo.m_segInfoList[seg];
			t_begintTime = t_segFeeInfo.m_startDateTime.getTimeInteger();
			if(m_pmManager->getAcctDetailBill(t_feeField,
		                                m_pRateInfo->m_bizType,
		                                m_pRateInfo->m_processRule->m_acctFeeIdFactorIds,
		                                m_pcdr,
		                                m_pRateInfo->m_cycParam.m_cycleId,
		                                t_segFeeInfo.m_feePolicyId,
		                                t_begintTime,
		                                t_billId) ==false)
			{
				setErrorCode(E_GET_ACCTDBILL);
				return false;
			}
			//Added By Sunam 2009/10/26 22:35:01前面的分段合账时厘放到最后一个分段上
		    if(seg < t_feeInfo.m_seg-1)
		    {
                t_userBill.m_value       = t_segFeeInfo.m_value-t_segFeeInfo.m_value%10; 
                t_userBill.m_discntValue = t_segFeeInfo.m_value-t_segFeeInfo.m_value%10;
            }
            else
            {
                t_userBill.m_value       = t_segFeeInfo.m_value-t_segFeeInfo.m_value%10+t_feeInfo.m_lackvalue;
                t_userBill.m_discntValue = t_segFeeInfo.m_value-t_segFeeInfo.m_value%10+t_feeInfo.m_lackvalue;
            }			
			//t_userBill.m_value       = t_segFeeInfo.m_value;
      		//t_userBill.m_discntValue = t_segFeeInfo.m_value;
			t_userBill.m_billId = t_billId;
      		m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,t_userBill);
		}
    }
    //提交累计量
    for(int t_segs=0;t_segs<t_feeInfo.m_seg;++t_segs)
    {
      //将该段赠送量累计
      SegFeeInfo &t_segInfo = t_feeInfo.m_segInfoList[t_segs];
      for(vector<RateBillInfo>::const_iterator t_curItr= t_segInfo.m_billInfoList.begin();
          t_curItr!=t_segInfo.m_billInfoList.end();++t_curItr)
      {
        if(t_curItr->m_objType == BILLOBJ_DURATION) //赠送时长
        {
          t_flag = false;
          for(t_itr =m_fldSendMinList.begin();
              t_itr!=m_fldSendMinList.end();++t_itr)
          {
            if(*t_itr == *t_curItr)
            {
              t_flag = true;break;
            }
          }
          if(t_flag == false)
          {
            m_fldSendMinList.push_back(*t_curItr);
          }else
          { //取两者较大者
            t_itr->m_billValue += t_curItr->m_billValue;
          }
        }else //费用等
        {
          commitSendBillInfo(*t_curItr);
        }
      }
    }
    //将赠送时长累计量从m_fldSendMinList更新到m_tpSendMinList
    commitFldSendMin();
  }
  //将m_tpSendMinList中的累计量提交
  for(t_itr=m_tpSendMinList.begin();t_itr!=m_tpSendMinList.end();++t_itr)
  {
    commitSendBillInfo(*t_itr);
  }
  
  //added by yinb 2011/2/18 18:46:54
  //将m_pRateFeeInfo->m_addupInfos中累计量提交
  for(t_itr=m_pRateFeeInfo->m_addupInfos.begin();t_itr!=m_pRateFeeInfo->m_addupInfos.end();++t_itr)
  {
    commitSendBillInfo(*t_itr);
  }
  
  //计费优惠累计
  m_pRateInfo->m_cdr[ATTRI_FEE_ALL] = t_baseFee_All;
  m_pRateInfo->m_cdr[ATTRI_DISC_ALL]= t_discFee_All;
  m_pRateInfo->m_cdr[ATTRI_RATE_DISC_FEE] = t_baseFee_All - t_discFee_All;
  RateDiscntBill  t_rateDiscntBill;
  t_rateDiscntBill.m_userId    = m_pRateInfo->m_billUserId;
  t_rateDiscntBill.m_accountId = 0;
  t_rateDiscntBill.m_billId    = BILL_ID_RATEDISCNT;
  t_rateDiscntBill.m_value     = t_baseFee_All - t_discFee_All;
  if(t_rateDiscntBill.m_value != 0 )
  {
    m_pRateInfo->updateBillIncre<RateDiscntBill>(m_pRateInfo->m_rateDiscnts_new,t_rateDiscntBill);
  }
  long t_tpRentValue = 0;
  //套餐实扣费用
  for(vector<UserSumBill1>::const_iterator t_rentItr = m_tpRentFeeList.begin(); 
      t_rentItr != m_tpRentFeeList.end(); ++t_rentItr)
  {
    m_pRateInfo->updateBillIncre<UserSumBill1>(m_pRateInfo->m_userSumBill1s_new,*t_rentItr);
    t_tpRentValue += t_rentItr->m_value;
  }
  for(vector<UserBill>::const_iterator t_rentBillItr = m_tpRentBillList.begin();
      t_rentBillItr != m_tpRentBillList.end();++t_rentBillItr)
  {
    m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,*t_rentBillItr);
  }
  m_pRateInfo->m_cdr[ATTRI_TARIFF_FEE] = t_tpRentValue;
  return true;
}
//设置优惠轨迹
void PriceBaseProc::setRateTrack()
{
  //获取优惠轨迹信息
  const char * t_freeTrack = m_tpRentMgr.getRateTrackInfo();
  //设置优惠轨迹 
  m_pRateInfo->m_cdr.setSpLongValue(ATTRI_SPLONG_FREETRACK,t_freeTrack);
  //获取套餐实扣信息
  m_tpRentMgr.getTpRentInfos(&m_tpRentFeeList,&m_tpRentBillList);
}

//多个政策间的选取  
void PriceBaseProc::selectTpResult()
{
  if(m_pRateFeeInfo->m_curFeeIdx<=0) return;
  //1. 当前优先级内选择
  CdrFeeInfo  t_feeIno->m_userSumBill1s_new,*t_rentItr);
    t_tpRentValue += t_rentItr->m_value;
  }
  for(vector<UserBill>::const_iterator t_rentBillItr = m_tpRentBillList.begin();
      t_rentBillItr != m_tpRentBillList.end();++t_rentBillItr)
  {
    m_pRateInfo->updateBillIncre<UserBill>(m_pRateInfo->m_userBills_new,*t_rentBillItr);
  }
  m_pRateInfo->m_cdr[ATTRI_TARIFF_FEE] = t_tpRentValue;
  return true;
}
//璁剧疆浼樻儬杞ㄨ抗
void PriceBaseProc::setRateTrack()
{
  //鑾峰彇浼樻儬杞ㄨ抗淇℃伅
  const char * t_freeTrack = m_tpRentMgr.getRateTrackInfo();
  //璁剧疆浼樻儬杞ㄨ抗 
  m_pRateInfo->m_cdr.setSpLongValue(ATTRI_SPLONG_FREETRACK,t_freeTrack);
  //鑾峰彇濂楅