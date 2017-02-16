#include "CDiscntMonfee.h"

CDiscntMonfee::~CDiscntMonfee()
{

}

CDiscntMonfee::CDiscntMonfee()
{
	m_userSubInfo = NULL;
	m_monfeeDiscountVector = NULL;
	m_spRelaDiscntMap = NULL;
	m_RateInfo = NULL;
}

bool CDiscntMonfee::Initialize(UserSubInfo *userSubInfo,vector<MonfeeDiscount>* monfeeDiscountVector,
                               char fileType,IdInfo idInfo,
                               RateGlobalInfo  *pRateInfo,SpRelaDiscntMap* spRelaDiscntMap)
{
	m_userSubInfo        = userSubInfo;
	m_RateInfo            = pRateInfo;
	m_spRelaDiscntMap    = spRelaDiscntMap;
	m_monfeeDiscountVector  = monfeeDiscountVector;
	m_fileType           = fileType;
	m_idInfo             = idInfo;
	getDealDate();
	return true;
}

bool CDiscntMonfee::discntMonfee()
{
	//销号用户不收
		if( m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] == "5" ||
			 ((m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "0" && m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "5") &&
		   ((T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_PREDESTROY_TIME]) < (m_RateInfo->m_cycParam).m_beginTime) ||
		   (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_DATE]) < (m_RateInfo->m_cycParam).m_beginTime))))
    {
        return false;
    }
    
    bool isNotOpen = false;
    //全月停机不收,包年、包半年不在此列
    if((m_RateInfo->m_comBill).judgeNotOpen((m_RateInfo->m_cycParam).m_beginTime,
                                 (m_RateInfo->m_cycParam).m_endTime,
                                  m_RateInfo->m_vecUserServState))
    {
        isNotOpen = true;
    }
    
	  if(isNotOpen && m_userSubInfo->m_billFlag !=ADDUP_CYCLE_TYPE_YEAR&&
	  	 m_userSubInfo->m_billFlag!=ADDUP_CYCLE_TYPE_YEAR_HALF)
	  {
	  	return false;
	  }
	  
  vector<MonfeeDiscount>::iterator litr;
	for (litr =(*m_monfeeDiscountVector).begin();litr!=(*m_monfeeDiscountVector).end(); ++litr)
    {    	
    	if(isValidDiscount(*litr))
    	{    
        	dealDiscount( *litr);        
       }
    }
    return true;
}

bool CDiscntMonfee::isValidSubInfo(UserSubInfo &userSubInfo)
{
	if(userSubInfo.m_startDate>(m_RateInfo->m_cycParam).m_endTime ||
	   userSubInfo.m_endDate<=(m_RateInfo->m_cycParam).m_beginTime ||
	   userSubInfo.m_startDate>userSubInfo.m_endDate)	
	{
		return false;
	}
	
	return true;
}

bool CDiscntMonfee::isValidDiscount(const MonfeeDiscount &monfeeDiscount)
{
    String<14> startDate = monfeeDiscount.m_startDate;
    String<14> endDate = monfeeDiscount.m_endDate;
    char commonValidType = monfeeDiscount.m_validType; 
      
    
    //当月申请立即生效，当月取消，下月生效
    if (commonValidType == '0')
    {            	
    	;
    }
    //当月申请，立即生效；当月终止，立即生效
    else if (commonValidType == '1')
    {
       if (endDate <= (m_RateInfo->m_cycParam).m_endTime) 
        {
            return false;
        }
    }    
    //当月申请下月生效;当月终止，立即生效
    else if (commonValidType == '2')
    {
      if ((endDate <= (m_RateInfo->m_cycParam).m_endTime) || (startDate>= (m_RateInfo->m_cycParam).m_beginTime))
        {
            return false;
        }
    }
    //当月申请下月生效;当月终止，下月生效
    else if (commonValidType == '3')
    { 
       if (startDate >= (m_RateInfo->m_cycParam).m_beginTime)
        {  
            return false;
        }
    }        
    else
    {
        return false;
    }
    
	if(monfeeDiscount.m_startCycId>0 && monfeeDiscount.m_endCycId>0)
	{
		CycParam startCycPara;			
		if((m_RateInfo->m_paramMgr)->getcycPara(startDate, startCycPara))
		{
			int startYear, endYear;
			endYear = (m_RateInfo->m_cycParam).m_cycleId/100;
			startYear = startCycPara.m_cycleId/100;
			int cycNum = (endYear - startYear)*12 + ((m_RateInfo->m_cycParam).m_cycleId%100 - startCycPara.m_cycleId%100) + 1;
			
			if( (cycNum<monfeeDiscount.m_startCycId) || (cycNum>monfeeDiscount.m_endCycId) )
			{
				return false;
			}
		}
	}   
	
	return true;
}

void CDiscntMonfee::dealDiscount(MonfeeDiscount &monfeeDiscount)
{
	if (monfeeDiscount.m_discountType == '5')
	{
		spRelaDiscntMapIter spRelaDiscntMapIter = (*m_spRelaDiscntMap).find(monfeeDiscount.m_discountId);
		
		dealRelaDiscount(monfeeDiscount, (*spRelaDiscntMapIter).second);
		
	}
	else
	{
		if ((monfeeDiscount.m_spBizCode != m_userSubInfo->m_spBizCode) || (monfeeDiscount.m_spCode != m_userSubInfo->m_spCode))
			{
				return;
			}

		SpStateVector spStateVector;
		
    //判断sp是否被处罚
    if ((m_RateInfo->m_comBill).getSpPunishRule(monfeeDiscount.m_spCode, "0", spStateVector))
    {	
    	if (spStateVector.size() != 0)
      {
      	return;
      } 
    }       
    
    //判断sp业务是否被处罚
    spStateVector.clear();
    
   	if ((m_RateInfo->m_comBill).getSpPunishRule(monfeeDiscount.m_spCode, monfeeDiscount.m_spBizCode, spStateVector))
   	{
      if(spStateVector.size() != 0)
      {
      	return;
      }
    }
       
		if(m_fileType=='B')//ADC  集团成员
	  {    	
			if((m_RateInfo->m_comBill).judgeECAbnormalState(m_userSubInfo->m_ecUserId,m_userSubInfo->m_spBizCode))
			{
				    return ;
			}
		}                                           
	  dealEffect((*m_userSubInfo),monfeeDiscount);
	}  
}

void CDiscntMonfee::dealEffect(UserSubInfo &userSubInfo, MonfeeDiscount &monfeeDiscount)
{
	UserSubInfo t_UserSubInfo = userSubInfo;
	if(!isValidSubInfo(userSubInfo))
	{
		return;
  }
		 long t_discountValue=0;
		 if(monfeeDiscount.m_discountValue[0] == 'E')
    	{
    		 char *expr;
    		 expr = &monfeeDiscount.m_discountValue[1];
			   if (!m_RateInfo->explanExpr(t_discountValue,expr))
			   {
			   	
			   	return;
			   }
    	}
    	else
    	{
    		t_discountValue = atoi(monfeeDiscount.m_discountValue);
    	}
		int effectFee=0;		
				
		//getfoundFee(userSubInfo,monfeeDiscount.m_itemCode,effectFee);
	vector<UserDetailBillMonfee>::iterator iterU=t_UserSubInfo.m_userDetailBill.begin();

	for(;iterU!=t_UserSubInfo.m_userDetailBill.end();iterU++)
	{
		if(iterU->m_itemCode != monfeeDiscount.m_itemCode)//账目不一致 continue
			continue;
			
		   effectFee =iterU->m_fee;	    
    	if (monfeeDiscount.m_discountType == '0') //减免
		{				
    		if (monfeeDiscount.m_discountValueType == '0') //作用值类型:金额
    		{    	        	    
		        if (t_discountValue < effectFee)
    	        {
    	            effectFee = t_discountValue;
    	        }    	        
    		}
    		else
    		{
    		    effectFee = 1.0 * effectFee * t_discountValue/100 + 0.5;
    		}
		}
		else if (monfeeDiscount.m_discountType == '1') //补收
		{			    
    		if (monfeeDiscount.m_discountValueType == '0') //作用值类型:金额
    		{    	        	    
		        effectFee=t_discountValue;    	        
    		}
    		else
    		{
    		    effectFee=1.0 * effectFee * t_discountValue/100 + 0.5;
    		}
    		effectFee = -effectFee;
		}
		else if (monfeeDiscount.m_discountType == '2') //封顶
		{			    
    		if (effectFee>t_discountValue) 
    		{    	        	    
		        effectFee= effectFee - t_discountValue;    	        
    		}
    		else
    		{
    			effectFee = 0;
    		}    		
		}
		else if (monfeeDiscount.m_discountType == '3') //保底
		{			    
    		if (effectFee<t_discountValue) 
    		{    	        	    
		        effectFee= effectFee - t_discountValue;
    		}
    		else
    		{
    			effectFee = 0;
    		}
    		
		}
		
		//ADC业务按人数分段打折需求
    //1.月功能费200元，包10个用户，每超出一个用户加收28元。
    //2.月功能费300元，包20个用户，每超出一个用户加收28元。		
		else if (monfeeDiscount.m_discountType == '4') //按人数补收
		{			
		    int usernum = 0;
		    effectFee = 0;
		    
		    (m_RateInfo->m_comBill).getUserNum(T_SERVER_ID(m_idInfo.m_idAttr[IdInfo::ID_ATTR_ID]),
		    							 userSubInfo.m_spBizCode,usernum);		                                 
		    
		    if (usernum >= monfeeDiscount.m_minValue && usernum != 0)
		    {
		    	if (usernum <= monfeeDiscount.m_maxValue)
		    	{
		    		effectFee = t_discountValue * (usernum - monfeeDiscount.m_referValue);
		    	}
		    	else
		    	{
		    		effectFee = t_discountValue * (monfeeDiscount.m_maxValue - monfeeDiscount.m_minValue + 1);
		    	}
		    }		    
		    effectFee = -effectFee;			    	        
		}
		else if (monfeeDiscount.m_discountType == '6') //设定
		{
			if (effectFee<t_discountValue) 
    		{    	        	    
		        effectFee= effectFee - t_discountValue;
    		}
    	else if(effectFee>t_discountValue)
    		{  	        	    
		        effectFee= effectFee - t_discountValue;    	        
    		}
    	 else
    	 	{
    	 		;
    	 		
    	 	}
		}
		else
		{
			return;
		}
    long t_addupValue = -1;
    bool t_flag1 = false;
    if(	monfeeDiscount.m_addupMethod != '0')
    	{
    		t_flag1 = getAddupInfo(monfeeDiscount.m_tpObjId,atol(monfeeDiscount.m_startDate.c_str()),
		    		             monfeeDiscount.m_spCode,monfeeDiscount.m_spBizCode,t_addupValue);
    		
    		if(!t_flag1)
    	  {
    	  	t_addupValue=0;
        	  	DEBUG_PRINT("[%s:%ld] 获取包月费累积量失败！ m_tpObjId=%s \n",__FILE__,__LINE__,monfeeDiscount.m_tpObjId);
    	  }
    	      	 
    		switch(monfeeDiscount.m_addupMethod)
		    {
		    	case 'A'://普通累计判断方式（累计满多少个月才收钱）
		    		if(t_addupValue < monfeeDiscount.m_addupValue)
		    			{
		    				effectFee=0;
		    			}		    		             
		    		       break;
		      default: break;
		    }		
        long t_fstarTime=0;
        String<14> t_dealTime = m_currentDate;
        CycParam startCycPara; 
        getSumTimeInfo(monfeeDiscount.m_startDate,t_fstarTime,monfeeDiscount.m_addupType);
        switch(monfeeDiscount.m_addupType)
		         {
		         	case 0://按天累计
		         		if(m_currentDate>(m_RateInfo->m_cycParam).m_endTime)
		         			 t_dealTime = (m_RateInfo->m_cycParam).m_endTime;
		         			
		         		 t_addupValue=m_RateInfo->diffDays(atol(t_dealTime.c_str()),t_fstarTime);
		         		 break;
		         	case 1://按月累计		    		                            
                   if((m_RateInfo->m_paramMgr)->getcycPara(monfeeDiscount.m_startDate, startCycPara))
                   {
                       t_addupValue = (m_RateInfo->m_cycParam).m_cycleId - startCycPara.m_cycleId + 1;
                   }
		         		 break;
		         	case 2://按年累计
		         		   t_addupValue = atol(t_dealTime.c_str())/1000000000 - t_fstarTime/1000000000 +1;
		         		 break; 	
		           default: break;
		         }		
        
        
        updateAddupInfo(monfeeDiscount.m_tpObjId,monfeeDiscount.m_spCode,monfeeDiscount.m_spBizCode,
                    t_fstarTime,atol(monfeeDiscount.m_endDate.c_str()),t_addupValue);
      }
    
    //      if(!(m_RateInfo->m_comBill).ifItemExists(monfeeDiscount.m_itemCode))
    //  	  {
    //      	#ifdef DEBUG
    //       	cout<<__FILE__<<":"<<__LINE__<<" Item: "<<r_detailItem<<" length is out of range! MAX_ITEM="<<MAX_ITEM<<endl;
    //        #endif	 	
    //    		 (m_RateInfo->m_comBill).setErrorCode(m_RateInfo->m_userId,E_ACCT_ITEM,"0",USER_ID_TYPE,monfeeDiscount.m_itemCode);
    //	 	     return;
    //      }
  
  	if(effectFee != 0)
  	{
  		if (userSubInfo.m_userDetailBill.size() == 1 && 
			    userSubInfo.m_userDetailBill[0].m_itemCode == 0)	
		  {
				userSubInfo.m_userDetailBill[0].m_itemCode=monfeeDiscount.m_itemCode;
				userSubInfo.m_userDetailBill[0].m_fee=0;
				userSubInfo.m_userDetailBill[0].m_disPlayFee=0;
				userSubInfo.m_userDetailBill[0].m_discntFee=-effectFee;
				userSubInfo.m_userDetailBill[0].m_discntCode=monfeeDiscount.m_discountId;
    				userSubInfo.m_userDetailBill[0].m_feeType=iterU->m_feeType;
				userSubInfo.m_userDetailBill[0].m_discntType=monfeeDiscount.m_discountType+1;
			}
			else
			{ 
	  		UserDetailBillMonfee userDetailBill;
	  		userDetailBill.m_itemCode=monfeeDiscount.m_itemCode;
				userDetailBill.m_fee=0;
				userDetailBill.m_disPlayFee=0;
				userDetailBill.m_discntFee=-effectFee;
				userDetailBill.m_discntCode=monfeeDiscount.m_discountId;
				userDetailBill.m_discntType=monfeeDiscount.m_discountType+1;
    				userDetailBill.m_feeType=iterU->m_feeType;
				
				userSubInfo.m_userDetailBill.push_back(userDetailBill);
			}
		}
  
}
}

bool CDiscntMonfee::dealRelaDiscount(MonfeeDiscount &monfeeDiscount,vector<SpRelaDiscntDetail> & spRelaDiscntDetailVector)
{
	vector<SpRelaDiscntDetail>::iterator iterS;
  
  int feeSum = 0;
  int relaSpSvcNum = 0;  
  int effectFee = 0;
  
  for (iterS=spRelaDiscntDetailVector.begin(); iterS!=spRelaDiscntDetailVector.end(); ++iterS)
  {
      if ( m_userSubInfo->m_spCode == (*iterS).m_spCode &&
      	   m_userSubInfo->m_spBizCode == (*iterS).m_spBizCode)
      {
          if ( isValidSubInfo(*m_userSubInfo))
          {
              getfoundFee(*m_userSubInfo,(*iterS).m_itemCode,effectFee);
              feeSum += effectFee;
              relaSpSvcNum++;
          }
      }
  }
  
  int nFee = 0;    
  UserDetailBillMonfee userDetailBill;
  
  long t_discountValue=0;
  if(monfeeDiscount.m_discountValue[0] == 'E')
  {
  	 char *expr;
  	 expr = &monfeeDiscount.m_discountValue[1];
     if (!m_RateInfo->explanExpr(t_discountValue,expr))
     {
     	
     	return false;
     }
  }
    	 	  
  if (relaSpSvcNum != 0 && relaSpSvcNum == spRelaDiscntDetailVector.size())
  {
		if (monfeeDiscount.m_discountValueType == '0')		//按金额优惠
		{
			if (t_discountValue  < feeSum)
  	  {
    		effectFee = t_discountValue ;
      }  
      else
      {
      	effectFee = feeSum;
      }
		}
		else																							//按比例优惠
		{
		    effectFee = 1.0 * feeSum * t_discountValue / 100 + 0.5;
		}
    
    if (monfeeDiscount.m_discountMode == '0')					//按金额几何平均作用
    {
    	int discntFee = 0;
    	int discntFeeAll;
    	int	itemCode = 0;
    	
    	discntFeeAll = effectFee;
    	
    	for (iterS = spRelaDiscntDetailVector.begin(); iterS != spRelaDiscntDetailVector.end(); ++iterS)
		  {
          if ( m_userSubInfo->m_spCode == (*iterS).m_spCode &&
          	   m_userSubInfo->m_spBizCode == (*iterS).m_spBizCode)
          {
            getfoundFee((*m_userSubInfo),(*iterS).m_itemCode,nFee);
            if(nFee > 0 && discntFeeAll > 0)
			    	{
			    		discntFee = 1.0 * effectFee * nFee / feeSum + 0.5;
			    		discntFeeAll -= discntFee;
		    			userDetailBill.m_itemCode = (*iterS).m_itemCode;
							userDetailBill.m_fee = 0;
							userDetailBill.m_disPlayFee = 0;
							userDetailBill.m_discntFee = -discntFee;
							userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
							userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
							
							m_userSubInfo->m_userDetailBill.push_back(userDetailBill);
							itemCode = (*iterS).m_itemCode;
            }
          }
		  }
		  if (discntFeeAll !=0)		// 补全归整
		  {
		  	userDetailBill.m_itemCode = itemCode;
				userDetailBill.m_fee = 0;
				userDetailBill.m_disPlayFee = 0;
				userDetailBill.m_discntFee = -discntFeeAll;
				userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
				userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
				
				m_userSubInfo->m_userDetailBill.push_back(userDetailBill);
		  }
    }
    else //按帐目优先级作用
    {
    	for (iterS = spRelaDiscntDetailVector.begin(); iterS != spRelaDiscntDetailVector.end(); ++iterS)
		  {
          if ( m_userSubInfo->m_spCode == (*iterS).m_spCode &&
          	   m_userSubInfo->m_spBizCode == (*iterS).m_spBizCode)
          {
            getfoundFee((*m_userSubInfo),(*iterS).m_itemCode,nFee);
            if(nFee > 0 && effectFee > 0)
			    	{
			    		if (effectFee >= nFee)
              {
				    		userDetailBill.m_itemCode = (*iterS).m_itemCode;
								userDetailBill.m_fee = 0;
								userDetailBill.m_disPlayFee = 0;
								userDetailBill.m_discntFee = -nFee;
								userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
								userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
								
								m_userSubInfo->m_userDetailBill.push_back(userDetailBill);
								
								effectFee -= nFee;
              }
              else
              {
              	userDetailBill.m_itemCode = (*iterS).m_itemCode;
								userDetailBill.m_fee = 0;
								userDetailBill.m_disPlayFee = 0;
								userDetailBill.m_discntFee = -effectFee;
								userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
								userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
								
								m_userSubInfo->m_userDetailBill.push_back(userDetailBill);
								effectFee = 0;
              }
            }
          }
	      if (effectFee <= 0)
	      	break;
		  }
    }
  } 

    return true;
}    

int  CDiscntMonfee::getfoundFee(UserSubInfo &userSubInfo, const int& itemCode, int &fee)
{
	fee = 0;	
	vector<UserDetailBillMonfee>::iterator iterU;
	for(iterU=userSubInfo.m_userDetailBill.begin();
	    iterU!=userSubInfo.m_userDetailBill.end();iterU++)
	{
		if(iterU->m_itemCode == itemCode)
		{
			fee +=iterU->m_fee;
		}
	}
	return fee;
}		 	                

bool CDiscntMonfee::getAddupInfo(const long &r_tpObjId, 
    	                           const long &r_beginTime, 
    	                           const string &r_spCode,
    	                           const string &r_spBizCode,
    	                           long &r_addupValue)
{
	UserSumBill3 l_sumBill3;
  const UserSumBill3 *l_pLastSumBill3=NULL;
  
  l_sumBill3.m_userId       = m_RateInfo->m_userId;     
  l_sumBill3.m_tpObjId      = r_tpObjId;    
  l_sumBill3.m_beginTime    = r_beginTime;  
  l_sumBill3.m_cycleId      = (m_RateInfo->m_cycParam).m_cycleId;     
  l_sumBill3.m_spCode       = r_spCode;
  l_sumBill3.m_spBizCode    = r_spBizCode;
  	
  if (!m_RateInfo->getLastSumBill3(m_RateInfo->m_userSumBill3s_new,
                                    l_sumBill3,
                                    l_pLastSumBill3)) 	
  {
    return false;	
  }
  
  r_addupValue=l_pLastSumBill3->m_value;
  
	return true;
} 

bool CDiscntMonfee::getSumTimeInfo(const T_DATE_TIME &r_tpObjTime,long &r_fStartTime,const char &r_adduptype)  
{
  char t_timeStr[14+1];
  switch(r_adduptype )//周期类型
  {
    case '1'         : //月
      {
        snprintf(t_timeStr,6+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+6,"01000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    case '2'          ://年
      {
        snprintf(t_timeStr,4+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+4,"0101000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    case '0'           : //天 
      {
        snprintf(t_timeStr,8+1,"%s",r_tpObjTime.c_str());
        sprintf(t_timeStr+8,"000000");
        r_fStartTime = atol(t_timeStr);
        break;
      }
    default:
      {
        return false;
        break;
      }
  }  
  return true;
}

void CDiscntMonfee::getDealDate()
{
    time_t now;
    char tmpDate[14+1];
    struct tm *w;
    memset(tmpDate,0,sizeof(tmpDate));

    time(&now);
    w = localtime(&now);
    sprintf(tmpDate,"%04d%02d%02d%02d%02d%02d",
            w->tm_year + 1900,w->tm_mon + 1,
            w->tm_mday,w->tm_hour,
            w->tm_min, w->tm_sec);

    m_currentDate = tmpDate;
    		
    memset(tmpDate,0,sizeof(tmpDate));
}

void CDiscntMonfee::updateAddupInfo(const long &r_tpObjId,
                                    const string &r_spCode,
    	                              const string &r_spBizCode, 
    	                              const long &r_beginTime,
    	                              const long &r_endTime,    	                              
    	                              const long &r_addupvalue)
{	
	UserSumBill3 l_sumBill;
  l_sumBill.m_userId	  	=	m_RateInfo->m_userId;     
	l_sumBill.m_tpObjId			=	r_tpObjId;
	l_sumBill.m_cycleId			=	(m_RateInfo->m_cycParam).m_cycleId; 
	l_sumBill.m_spCode      =r_spCode;
	l_sumBill.m_spBizCode   =r_spBizCode;
	l_sumBill.m_beginTime		=	r_beginTime;   
	l_sumBill.m_endTime			=	r_endTime;     
	l_sumBill.m_value			=	r_addupvalue;      
	
	m_RateInfo->updateBillTotal(m_RateInfo->m_userSumBill3s_new,l_sumBill);
	
}