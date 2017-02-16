#include "CDiscntMonfee.h"
#include "param/CMonfeeParamMgr.h"


CDiscntMonfee::~CDiscntMonfee()
{

}


CDiscntMonfee::CDiscntMonfee()
{
    m_userSubInfoVector = NULL;         
    m_userSVCStateVector = NULL; 
    m_monfeeDiscountVector = NULL;   
    m_monfeeParamMgr = NULL;                            
}

void CDiscntMonfee::discntMonfee(const IdInfo &idInfo, 
                            const CycParam &cycPara,                             
                            const char &fileType,
                            vector<UserSubInfo> *userSubInfoVector,
                            vector<MonfeeDiscount>    *discountVector,
                            vector<UserServState> *userSVCStateVector,
														vector<IdErrorInfo>  *userErrorInfoVector,
                            CMonfeeParamMgr *monfeeParamMgr,
                            SpRelaDiscntMap *spRelaDiscntMap)
{
 	m_idInfo = idInfo;
	m_cycPara = cycPara;
	m_fileType = fileType;		
	m_monfeeParamMgr = monfeeParamMgr;
	m_userSubInfoVector = userSubInfoVector;	
	m_monfeeDiscountVector = discountVector;
	m_userSVCStateVector = userSVCStateVector;
	m_userErrorInfoVector = userErrorInfoVector;
	
	m_spRelaDiscntMap = spRelaDiscntMap;
	
	//销号用户不作用
	if(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] == "5" ||
			 (  (m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "0" ) &&
	    ( (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_PREDESTROY_TIME]) < m_cycPara.m_beginTime) ||
	      (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_DATE]) < m_cycPara.m_beginTime) )))
    {
		return;
	}
        
	//全月停机不作用
	if((*m_monfeeParamMgr).judgeNotOpen(m_cycPara.m_beginTime,
                                      m_cycPara.m_endTime,
                                      *m_userSVCStateVector))
    {
		return;
	}	

	vector<MonfeeDiscount>::iterator lit;
	for (lit =(*m_monfeeDiscountVector).begin();
         lit!=(*m_monfeeDiscountVector).end(); ++lit)
    {    	
    	if(isValidDiscount(*lit))
    	{    
        	dealDiscount( *lit);        
       }
    }
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

		SpStateVector spStateVector;
		
    //判断sp是否被处罚
    if ((*m_monfeeParamMgr).getSpPunishRule(m_cycPara, monfeeDiscount.m_spCode, "0", spStateVector))
    {	
    	if (spStateVector.size() != 0)
      {
      	return;
      } 
    }       
    
    //判断sp业务是否被处罚
    spStateVector.clear();
    
   	if ((*m_monfeeParamMgr).getSpPunishRule(m_cycPara, monfeeDiscount.m_spCode, monfeeDiscount.m_spBizCode, spStateVector))
   	{
      if(spStateVector.size() != 0)
      {
      	return;
      }
    }
    
		vector<UserSubInfoIter> userSubInfoVector;
	
		(*m_monfeeParamMgr).getSubInfoByDiscnt(monfeeDiscount, m_cycPara, 
	                                         *m_userSubInfoVector,
	                                         userSubInfoVector);                                         
	    
		if(m_fileType=='B')//ADC  集团成员
	  {    	
			if(userSubInfoVector.size()>0)
			{
				//集团在本月没有'正常商用'状态
				if( (*m_monfeeParamMgr).judgeECAbnormalState(
				                (*userSubInfoVector[0]).m_ecUserId,
				                (*userSubInfoVector[0]).m_spBizCode) )
				{
				    return ;
				}
			}
		}              
                               
		for (vector<UserSubInfoIter>::iterator dit= userSubInfoVector.begin(); dit!=userSubInfoVector.end(); ++dit)
    {
			dealEffect(*(*dit),monfeeDiscount);
    } 
	}   
}

bool CDiscntMonfee::dealRelaDiscount(MonfeeDiscount &monfeeDiscount,
																		 vector<SpRelaDiscntDetail>& spRelaDiscntDetailVector)
{

  vector<SpRelaDiscntDetail>::iterator iterS;
  vector<UserSubInfo>::iterator iterU;
  
  UserSubInfoIterVector userSubInfoIterVector;
  UserSubInfoIterVector::iterator iterVectorIter;
  
  int feeSum = 0;
  int relaSpSvcNum = 0;  
  int effectFee = 0;
  char feeType='0';  	//add by yuxk for monfeediscnt	
  //获取该用户拥有的该关联优惠下业务个数
  for (iterS=spRelaDiscntDetailVector.begin(); iterS!=spRelaDiscntDetailVector.end(); ++iterS)
  {
      
      for (iterU = (*m_userSubInfoVector).begin(); iterU != (*m_userSubInfoVector).end(); ++iterU)
      {
          if ( (*iterU).m_spCode == (*iterS).m_spCode &&
          	     (*iterU).m_spBizCode == (*iterS).m_spBizCode)
          {
              if ( isValidSubInfo((*iterU)))
              {
                  userSubInfoIterVector.push_back(iterU);
                  getfoundFee((*iterU),(*iterS).m_itemCode,effectFee,feeType);//modify by yuxk for monfeediscnt	 
                  feeSum += effectFee;
                  relaSpSvcNum++;
              }
              break;
          }
      }
  }
  
  int nFee = 0;    
  UserDetailBill userDetailBill;
    	  
  //进行优惠处理
  if (relaSpSvcNum != 0 && relaSpSvcNum == spRelaDiscntDetailVector.size())
  {
		if (monfeeDiscount.m_discountValueType == '0')		//按金额优惠
		{
			if (monfeeDiscount.m_discountValue < feeSum)
  	  {
    		effectFee = monfeeDiscount.m_discountValue;
      }  
      else
      {
      	effectFee = feeSum;
      }
		}
		else																							//按比例优惠
		{
		    effectFee = 1.0 * feeSum * monfeeDiscount.m_discountValue / 100 + 0.5;
		}
    
    if (monfeeDiscount.m_discountMode == '0')					//按金额几何平均作用
    {
    	int discntFee = 0;
    	int discntFeeAll;
    	int	itemCode = 0;
    	
    	discntFeeAll = effectFee;
    	
    	for (iterS = spRelaDiscntDetailVector.begin(); iterS != spRelaDiscntDetailVector.end(); ++iterS)
		  {
	      
	      for (iterVectorIter=userSubInfoIterVector.begin(); iterVectorIter!=userSubInfoIterVector.end(); ++iterVectorIter)
	      {
          if ( (*iterVectorIter)->m_spCode == (*iterS).m_spCode &&
          	   (*iterVectorIter)->m_spBizCode == (*iterS).m_spBizCode)
          {
          getfoundFee((*(*iterVectorIter)),(*iterS).m_itemCode,nFee,feeType);	//modify by yuxk for monfeediscnt  
            if(nFee > 0 && discntFeeAll > 0)
			    	{
			    		discntFee = 1.0 * effectFee * nFee / feeSum + 0.5;
			    		discntFeeAll -= discntFee;
		    			userDetailBill.m_itemCode = (*iterS).m_itemCode;
							userDetailBill.m_fee = -discntFee;
							userDetailBill.m_disPlayFee = 0;
							userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
							userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
							userDetailBill.m_feeType=feeType;	//add by yuxk for monfeediscnt
							(*iterVectorIter)->m_userDetailBill.push_back(userDetailBill);
							itemCode = (*iterS).m_itemCode;
            }
            break;
          }
	      }
		  }
		  if (discntFeeAll !=0)		// 补全归整
		  {
		  	userDetailBill.m_itemCode = itemCode;
				userDetailBill.m_fee = -discntFeeAll;
				userDetailBill.m_disPlayFee = 0;
				userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
				userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
				userDetailBill.m_feeType=feeType;	//add by yuxk for monfeediscnt
				(*iterVectorIter)->m_userDetailBill.push_back(userDetailBill);
		  }
    }
    else  																						//按帐目优先级作用
    {
    	for (iterS = spRelaDiscntDetailVector.begin(); iterS != spRelaDiscntDetailVector.end(); ++iterS)
		  {
	      
	      for (iterVectorIter=userSubInfoIterVector.begin(); iterVectorIter!=userSubInfoIterVector.end(); ++iterVectorIter)
	      {
          if ( (*iterVectorIter)->m_spCode == (*iterS).m_spCode &&
          	   (*iterVectorIter)->m_spBizCode == (*iterS).m_spBizCode)
          {
            getfoundFee((*(*iterVectorIter)),(*iterS).m_itemCode,nFee,feeType);//modify by yuxk for monfeediscnt
            if(nFee > 0 && effectFee > 0)
			    	{
			    		if (effectFee >= nFee)
              {
				    		userDetailBill.m_itemCode = (*iterS).m_itemCode;
								userDetailBill.m_fee = -nFee;
								userDetailBill.m_disPlayFee = 0;
								userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
								userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
								userDetailBill.m_feeType=feeType;	//add by yuxk for monfeediscnt
								(*iterVectorIter)->m_userDetailBill.push_back(userDetailBill);
								
								effectFee -= nFee;
              }
              else
              {
              	userDetailBill.m_itemCode = (*iterS).m_itemCode;
								userDetailBill.m_fee = -effectFee;
								userDetailBill.m_disPlayFee = 0;
								userDetailBill.m_discntCode = monfeeDiscount.m_discountId;
								userDetailBill.m_discntType = monfeeDiscount.m_discountType+1;
								userDetailBill.m_feeType=feeType;	//add by yuxk for monfeediscnt
								(*iterVectorIter)->m_userDetailBill.push_back(userDetailBill);
								effectFee = 0;
              }
            }
            break;
          }
	      }
	      if (effectFee <= 0)
	      	break;
		  }
    }
  } 

    return true;
}

void CDiscntMonfee::dealEffect(UserSubInfo &userSubInfo,
                              MonfeeDiscount &monfeeDiscount)
{
	if(isValidSubInfo(userSubInfo))
	{
		int effectFee=0;		
		char feeType='0';//add by yuxk for monfeediscnt	 
		getfoundFee(userSubInfo,monfeeDiscount.m_itemCode,effectFee,feeType);	   //modify by yuxk for monfeediscnt  
    	if (monfeeDiscount.m_discountType == '0') //减免
		{				
    		if (monfeeDiscount.m_discountValueType == '0') //作用值类型:金额
    		{    	        	    
		        if (monfeeDiscount.m_discountValue < effectFee)
    	        {
    	            effectFee = monfeeDiscount.m_discountValue;
    	        }    	        
    		}
    		else
    		{
    		    effectFee = 1.0 * effectFee * monfeeDiscount.m_discountValue 
    		                / 100 + 0.5;
    		}
		}
		else if (monfeeDiscount.m_discountType == '1') //补收
		{			    
    		if (monfeeDiscount.m_discountValueType == '0') //作用值类型:金额
    		{    	        	    
		        effectFee=monfeeDiscount.m_discountValue;    	        
    		}
    		else
    		{
    		    effectFee=1.0 * effectFee * monfeeDiscount.m_discountValue 
    		                / 100 + 0.5;
    		}
    		effectFee = -effectFee;
		}
		else if (monfeeDiscount.m_discountType == '2') //封顶
		{			    
    		if (effectFee>monfeeDiscount.m_discountValue) 
    		{    	        	    
		        effectFee= effectFee - monfeeDiscount.m_discountValue;    	        
    		}
    		else
    		{
    			effectFee = 0;
    		}    		
		}
		else if (monfeeDiscount.m_discountType == '3') //保底
		{			    
    		if (effectFee<monfeeDiscount.m_discountValue) 
    		{    	        	    
		        effectFee= effectFee - monfeeDiscount.m_discountValue;
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
		    //定义有效的用户数
		    int usernum = 0;
		 
		    //作用费用赋初始值0(当优惠类型为4时，作用费用与原始费用无关。)
		    effectFee = 0;
		    
		    //取有效的用户数
		    (*m_monfeeParamMgr).getUserNum(USERID(m_idInfo.m_idAttr[IdInfo::ID_ATTR_ID]),
		    							 userSubInfo.m_spBizCode,
		    							 /*userSubInfo.m_spCode,*/
		                                 usernum);		                                 
		    
		    if (usernum >= monfeeDiscount.m_minValue && usernum != 0)
		    {
		    	if (usernum <= monfeeDiscount.m_maxValue)
		    	{
		    		effectFee = monfeeDiscount.m_discountValue * (usernum - monfeeDiscount.m_referValue);
		    	}
		    	else
		    	{
		    		effectFee = monfeeDiscount.m_discountValue * (monfeeDiscount.m_maxValue - monfeeDiscount.m_minValue + 1);
		    	}
		    }		    
		    effectFee = -effectFee;			    	        
		}
		else
		{
			return;
		}
    	
  	if(effectFee != 0)
  	{
  		if (userSubInfo.m_userDetailBill.size() == 1 && 
			   userSubInfo.m_userDetailBill[0].m_itemCode == 0)	
		  {
				userSubInfo.m_userDetailBill[0].m_itemCode=monfeeDiscount.m_itemCode;
				userSubInfo.m_userDetailBill[0].m_fee=-effectFee;
				userSubInfo.m_userDetailBill[0].m_disPlayFee=0;
				userSubInfo.m_userDetailBill[0].m_discntCode=monfeeDiscount.m_discountId;
				userSubInfo.m_userDetailBill[0].m_discntType=monfeeDiscount.m_discountType+1;
			  userSubInfo.m_userDetailBill[0].m_feeType=feeType;//add by yuxk for monfeediscnt
			}
			else
			{ 
	  		UserDetailBill userDetailBill;
	  		userDetailBill.m_itemCode=monfeeDiscount.m_itemCode;
				userDetailBill.m_fee=-effectFee;
				userDetailBill.m_disPlayFee=0;
				userDetailBill.m_discntCode=monfeeDiscount.m_discountId;
				userDetailBill.m_discntType=monfeeDiscount.m_discountType+1;
				userDetailBill.m_feeType=feeType;//add by yuxk for monfeediscnt	
				userSubInfo.m_userDetailBill.push_back(userDetailBill);
			}
		}
  }	
}

bool CDiscntMonfee::isValidSubInfo(UserSubInfo &userSubInfo)
{	
	if(userSubInfo.m_startDate>m_cycPara.m_endTime ||
	   userSubInfo.m_endDate<=m_cycPara.m_beginTime ||
	   userSubInfo.m_startDate>userSubInfo.m_endDate)	
	{
		return false;
	}
	
//	//对于需要清理费用的用户，不做优惠处理 zhaogc add 20080130
//	if(userSubInfo.m_userDetailBill.size() == 1 && 
//	   userSubInfo.m_userDetailBill[0].m_itemCode == 0)	
//  {
//		return false;
//	}	   
	
	//if(userSubInfo.m_bizStateCode != NORMAL_BIZSTATE_CODE)
	//{
	//	return false;	
	//}
	
	//if(userSubInfo.m_billFlag != '2' &&
	//   userSubInfo.m_billFlag != '1')
	//{
	//	return false;	
	//}
	
	return true;
}


bool CDiscntMonfee::isValidDiscount(const MonfeeDiscount &monfeeDiscount)
{		
	
	//普通生效类型判断
    //正常生效
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
       if (endDate <= m_cycPara.m_endTime) 
        {
            return false;
        }
    }    
    //当月申请下月生效;当月终止，立即生效
    else if (commonValidType == '2')
    {
        //如果(1)该优惠生效终止时间小于优惠判断时间点，无效或者
        //如果(2)该用户是当月新开户用户，无效
        if ((endDate <= m_cycPara.m_endTime)  ||
            (startDate>= m_cycPara.m_beginTime))
        {
            return false;
        }
    }
    //当月申请下月生效;当月终止，下月生效
    else if (commonValidType == '3')
    { 
        //如果该用户是当月新开户用户，无效
        if (startDate >= m_cycPara.m_beginTime)
        {  
            return false;
        }
    }        
    //不可识validType
    else
    {
        return false;
    }
    
	if(monfeeDiscount.m_startCycId>0 &&
	   monfeeDiscount.m_endCycId>0)
	{
		CycParam startCycPara;			
		if( (*m_monfeeParamMgr).getcycPara(startDate, startCycPara))
		{
			int startYear, endYear;
			endYear = m_cycPara.m_cycleId/100;
			startYear = startCycPara.m_cycleId/100;
			int cycNum = (endYear - startYear)*12 + (m_cycPara.m_cycleId%100 - startCycPara.m_cycleId%100) + 1;
			
			if( (cycNum<monfeeDiscount.m_startCycId) || 
			    (cycNum>monfeeDiscount.m_endCycId) )
			{
				return false;
			}
		}
	}   
	
	return true;	
}

int CDiscntMonfee::getfoundFee(UserSubInfo &userSubInfo,
                             	 const int& itemCode, int &fee,char &feeType)//modify by yuxk for monfeediscnt
{
	fee = 0;
	
	vector<UserDetailBill>::iterator iterU;
	for(iterU=userSubInfo.m_userDetailBill.begin();
	    iterU!=userSubInfo.m_userDetailBill.end();iterU++)
	{
		if(iterU->m_itemCode == itemCode)
		{
			fee +=iterU->m_fee;
			feeType=iterU->m_feeType;//add by yuxk for monfeediscnt
		}
	}
	return fee;
}
