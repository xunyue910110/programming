#include "CRecvMonfee.h"
#include "param/CMonfeeParamMgr.h"

//Add by Malone 2009-07-16 start
#include "base/TimeUtil.h"
//Add by Malone 2009-07-16 end

bool operator==(const SpSvcKey& left,const SpSvcKey& right);
bool operator<(const SpSvcKey& left,const SpSvcKey& right);

CRecvMonfee::~CRecvMonfee()
{

}

CRecvMonfee::CRecvMonfee()
{
    m_userSubInfoVector = NULL;         
    m_userSubBillVector = NULL;         
    m_userSVCStateVector = NULL;    
    m_monfeeParamMgr = NULL; 
    m_validSubInfoVector.clear();
}


void CRecvMonfee::recvMonfee(const IdInfo &idInfo, 
                            const CycParam &cycPara,                             
                            const char  &dataType,
                            const String<14>& currentDate,
                            vector<UserSubInfo> *userSubInfoVector,
                            vector<UserServState> *userSVCStateVector,  
                            vector<IdErrorInfo>  *userErrorInfoVector, 
                            vector<UserSubInfo> *userSubBillVector,
                            //SpPunishMap * spPunishMap,
                            CMonfeeParamMgr *monfeeParamMgr)
{
    m_idInfo = idInfo;
    m_cycPara = cycPara;    
    m_isEndMonth = m_cycPara.m_addTag;
    m_dataType = dataType;
    m_currentDate = currentDate;        
    m_monfeeParamMgr = monfeeParamMgr;
    m_userSubInfoVector = userSubInfoVector;
    m_userSubBillVector = userSubBillVector;
    m_userSVCStateVector = userSVCStateVector;    
    m_userErrorInfoVector = userErrorInfoVector; 
//    m_spPunishMap = spPunishMap;

    //销号用户不收
		if(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] == "5" ||
			 ( (m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "0" && m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "5") &&
		    ( (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_PREDESTROY_TIME]) < m_cycPara.m_beginTime) ||
		      (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_DATE]) < m_cycPara.m_beginTime) )))
    {
        return;
    }
    
    //全月停机不收
    //modify by yangyb 20100728 手机报包年包半年注释掉
    /*
    if((*m_monfeeParamMgr).judgeNotOpen(m_cycPara.m_beginTime,
                                      m_cycPara.m_endTime,
                                      *m_userSVCStateVector))
    {
        return;
    }
    */

    m_tempSpCode = "";         
    m_tempSpBizCode = "";    
    m_userSubInfoIter = (*m_userSubInfoVector).begin();   
    while( getAllSubInfo(*m_userSubInfoVector) )
    {    
        dealTariff( m_validSubInfoVector );        
    }
}

void CRecvMonfee::dealTariff(vector<UserSubInfoIter>  &validSubInfoVector)
{    
    SpStateVector spStateVector;
    
    if(validSubInfoVector.size()>0)
    {
        UserSubInfo userSubInfo = *(validSubInfoVector[0]);

        if ((*m_monfeeParamMgr).getSpPunishRule(m_cycPara, userSubInfo.m_spCode, "0", spStateVector))
        {	
        	if (spStateVector.size() != 0)
	        {
	        	  UserDetailBill usrDetailBill;
	            usrDetailBill.m_itemCode=0;
	            usrDetailBill.m_fee=0;        
	            usrDetailBill.m_disPlayFee=0;
	            usrDetailBill.m_discntCode=0;
	            usrDetailBill.m_discntType='0';
	            
	            userSubInfo.m_userDetailBill.clear();
	            userSubInfo.m_userDetailBill.push_back(usrDetailBill);   
	        	  (*m_userSubBillVector).push_back(userSubInfo);
	        	  return;
	        } 
	      }       
	      
        //判断sp业务是否被处罚
        spStateVector.clear();
        
       	if ((*m_monfeeParamMgr).getSpPunishRule(m_cycPara, userSubInfo.m_spCode, userSubInfo.m_spBizCode, spStateVector))
       	{
	        if(spStateVector.size() != 0)
	        {
	        	  UserDetailBill usrDetailBill;
	            usrDetailBill.m_itemCode=0;
	            usrDetailBill.m_fee=0;        
	            usrDetailBill.m_disPlayFee=0;
	            usrDetailBill.m_discntCode=0;
	            usrDetailBill.m_discntType='0';
	            
	            userSubInfo.m_userDetailBill.clear();
	            userSubInfo.m_userDetailBill.push_back(usrDetailBill);   
	        	  (*m_userSubBillVector).push_back(userSubInfo);
	        	  return;
	        }
	      }
        
        //获取资费
        int iRet = 0;
        iRet = (*m_monfeeParamMgr).getTariffBySubInfo(m_dataType,
					                                             userSubInfo,
					                                             m_cycPara, 
					                                             m_monfeeTariffVector);

				if (iRet <= 0)
				{
					return;
				}

        for (vector<MonfeeTariff>::iterator 
            dit= m_monfeeTariffVector.begin();
            dit!=m_monfeeTariffVector.end();++dit)
        {
        		
            //modify by yangyb 2010-07-15 手机报包年包半年 start
            if (dit->m_billingCycle==ADDUP_CYCLE_TYPE_YEAR
             	  ||dit->m_billingCycle==ADDUP_CYCLE_TYPE_YEAR_HALF
             	  ||dit->m_billingCycle==ADDUP_CYCLE_TYPE_QUARTER)
        		{    //cout<<"yyb::包年" <<endl;
	               for(int i=0; i < validSubInfoVector.size();i++)
	               {
	               	    if(getValidSubInfo(*(validSubInfoVector[i]), *dit))
                      { 
                      	   //cout<<"yyb::包年 getValidSubInfo 成功" <<endl;
                      	   userSubInfo.m_startDate=(*(validSubInfoVector[i])).m_startDate;
                      	   userSubInfo.m_endDate=(*(validSubInfoVector[i])).m_endDate;
                      	   int dayNum=0;
                           dealEffect(*dit,userSubInfo,dayNum);
                      }
	               }
        		}
        		else
        		{
        			//add by yangyb 20100728 start
        			 if((*m_monfeeParamMgr).judgeNotOpen(m_cycPara.m_beginTime,
                                      m_cycPara.m_endTime,
                                      *m_userSVCStateVector))
               {
                     return;
               }
               // add by yangyb 20100728 end
               if(getValidSubInfo(validSubInfoVector, *dit))
               {
                String<14>   startDate = "20370101000000";
	      				String<14>   endDate = "19990101000000";
	      				//cout<<"validSubInfoVector.size():"<<validSubInfoVector.size()<<endl;
	              if(validSubInfoVector.size()>1)
	              {
		              for (vector<UserSubInfoIter>::iterator iterc = validSubInfoVector.begin();
		              			iterc != validSubInfoVector.end();++iterc)
		              {
		              	if ( startDate > (*iterc)->m_startDate)
		              		startDate = (*iterc)->m_startDate;
		              		
		              	if ( endDate < (*iterc)->m_endDate)
		              		endDate = (*iterc)->m_endDate;	              	
		              }         
	              
		              userSubInfo.m_startDate = startDate;
		              userSubInfo.m_endDate = endDate;
	              }
	              
                //Add by Malone 2009-07-16 start
	              /*dealEffect(*dit,userSubInfo);*/
                  int dayNum=dayDiff(validSubInfoVector);
                  dealEffect(*dit,userSubInfo,dayNum);
                //Add by Malone 2009-07-16 end
               }
            }
            //modify by yangyb 2010-07-15 手机报包年包半年 end 
        }
        //为防止用户由于暂停服务不收费，没有账单输出，以前的收费无法清理
        //需要增加0账单，触发明细帐单入库，将以前的账单 zhaogc add 20070706
        if( userSubInfo.m_userDetailBill.size() == 0 )
        {
            UserDetailBill usrDetailBill;
            usrDetailBill.m_itemCode=0;
            usrDetailBill.m_fee=0;        
            usrDetailBill.m_disPlayFee=0;
            usrDetailBill.m_discntCode=0;
            usrDetailBill.m_discntType='0';
            
            userSubInfo.m_userDetailBill.push_back(usrDetailBill);         
        }
        
       	(*m_userSubBillVector).push_back(userSubInfo);
        
    }
    return;        
}

void CRecvMonfee::dealEffect(MonfeeTariff &monfeeTariff,
                        UserSubInfo &userSubInfo,const int &dayNum)
{
      string userSubInfoStartDateStringI = userSubInfo.m_startDate.substr(0,6);
      string userSubInfoStartDateStringII = userSubInfo.m_startDate.substr(6,2);
      int userSubInfoStartDateI = atoi(userSubInfoStartDateStringI.c_str());
      int userSubInfoStartDateII = atoi(userSubInfoStartDateStringII.c_str());
        
      if( (monfeeTariff.m_judgeHalfTime != 0) &&
        (userSubInfoStartDateI  == m_cycPara.m_cycleId) &&
        (userSubInfoStartDateII >= 
                                       monfeeTariff.m_judgeHalfTime))                                  
    //Modified by Malone 2007-7-25 15:51 end
    {            
        m_fee = monfeeTariff.m_halfPrice;
    }
    else
    {
        m_fee = monfeeTariff.m_price;
    }

    //add by yih 2009-01-14 start
    //江苏联通收取模式需求:
    //            recvMode 0    :  全月一次性收取
    //            recvMode 1    :  上下半月按天月末补收
    //            recvMode 2    :  上下半月一次性收取
    //            recvmode 3    :  按实际生效天数收取 
    //            recvMode 其他 :  全月收取，按天显示  
    int iCurrentDate;
    bool bTempVaildTag = false;  
    
    String<14> currentDate=m_currentDate;    
    //获取当前天数 zhaogc add begin
    if (currentDate > m_cycPara.m_endTime)
    {
        currentDate=m_cycPara.m_endTime;
    }
    //获取当前天数 zhaogc add end 
    iCurrentDate = atoi(currentDate.substr(0,8).c_str()); 
        
    if(monfeeTariff.m_displayType == '0')
    {
        m_displayFee = m_fee;
    }
    else if (monfeeTariff.m_displayType == '1')
    {
        m_displayFee = 0;
        bTempVaildTag = adjustRecvFeeByRealDay(m_cycPara,
                                               userSubInfo.m_startDate,
                                               iCurrentDate,
                                               monfeeTariff.m_displayType,
                                               m_fee,m_displayFee);
        
        if(!bTempVaildTag)
        {
            m_displayFee = m_fee;
        }    
    }
    else if (monfeeTariff.m_displayType == '2')
    {
        m_displayFee = 0;
        bTempVaildTag = adjustRecvFeeByRealDay(m_cycPara,
                                               userSubInfo.m_startDate,
                                               iCurrentDate,
                                               monfeeTariff.m_displayType,
                                               m_fee,m_displayFee);
        
        if(!bTempVaildTag)
        {
            m_displayFee = m_fee;
        }  
    }
    
    //Add by Malone 2009-07-16 start
    else if (monfeeTariff.m_displayType == '3')
    {
        //int monthDayNum=31;
         //获取当月天数
        //getDuration(m_cycPara.m_endTime,
                    //m_cycPara.m_beginTime,monthDayNum); 
        //m_displayFee = 1.0*m_fee*dayNum/monthDayNum+0.5;
        m_displayFee = m_fee * dayNum;
    }
    //Add by Malone 2009-07-16 end
    else
    {
        int currentDayNum=0,monthDayNum=31;    	
        getDuration(currentDate, 
                    m_cycPara.m_beginTime,currentDayNum);
        //获取当月天数
        getDuration(m_cycPara.m_endTime,
                    m_cycPara.m_beginTime,monthDayNum);    
                    
        m_displayFee = 1.0 * m_fee * currentDayNum / monthDayNum + 0.5;            
    }

    //add by yih 2009-01-14 end    
    UserDetailBill userDetailBill;
    userDetailBill.m_itemCode=monfeeTariff.m_itemCode;
    userDetailBill.m_fee=m_displayFee;        
    userDetailBill.m_disPlayFee=m_displayFee;
    userDetailBill.m_discntCode=0;
    userDetailBill.m_discntType='0';
    userDetailBill.m_displayType=monfeeTariff.m_displayType;
    userDetailBill.m_feeType=monfeeTariff.m_feeType;	//add by yuxk for MT
   
    userSubInfo.m_userDetailBill.push_back(userDetailBill);         
}    
//add by yangyb 2010-07-15 手机报包年、包半年
bool CRecvMonfee::getValidSubInfo(
                          UserSubInfo & userSubInfo,
                          const MonfeeTariff & monfeeTariff)
{    
    // Add by Malone 2008-1-26 start
    // 是否需要判断沉默单高用户

    if(monfeeTariff.m_judgeSilence == '1' || monfeeTariff.m_judgeAverage == '1' )
    {
    	//需要判断，并且是沉默单高用户，则不计费
    	if ( (*m_monfeeParamMgr).judgeSilenceUser(m_cycPara.m_cycleId,T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]), 
	                                           userSubInfo.m_spCode,
	                                           userSubInfo.m_spBizCode,
	                                           monfeeTariff))
    	{
    			return false;
    	}	
    }
    // Add by Malone 2008-1-26 end
    if(monfeeTariff.m_recvObjectType=='1')//对集团收费
    {
        if(m_dataType != FILE_TYPE_IN_A)
            return false;
    }
    //普通生效类型判断
    //正常生效
    char commonValidType = monfeeTariff.m_newUserRecvType; 
    
    if(monfeeTariff.m_billingCycle==ADDUP_CYCLE_TYPE_YEAR
	      ||monfeeTariff.m_billingCycle==ADDUP_CYCLE_TYPE_YEAR_HALF
	      ||monfeeTariff.m_billingCycle==ADDUP_CYCLE_TYPE_QUARTER)
    {
		         //订购包年首月收费，其他月不收费
	           if(commonValidType == 'A' )
	           {
	                char    t_time[20];
		              TimeUtil::addSeconds(t_time,userSubInfo.m_startDate.c_str(),72*3600);
	            	  if((t_time<m_cycPara.m_beginTime) || (strncmp(t_time,m_currentDate.c_str(),8)>0)
	            	  	|| (strncmp(userSubInfo.m_endDate.c_str(),t_time,8)<0) )
	            	  {  
	            	  	  return false ; 
	            	  }
	            	  
	           }
			       else if(userSubInfo.m_startDate<m_cycPara.m_beginTime )    
		         {
				          return false;
			       }

    }
	  else if(monfeeTariff.m_billingCycle>MONTH_CYCTYPE_CODE)
    {        
        return false;    
    }  
    if(m_dataType=='B')//ADC  集团成员
    {
        if(userSubInfo.m_ecSerialNumber != "0")
        {
            //集团在本月没有'正常商用'状态
            if( (*m_monfeeParamMgr).judgeECAbnormalState(
                            userSubInfo.m_ecUserId,       
                            userSubInfo.m_spBizCode) )
            {
                return false;
            }
        }
    }           
    return true;    
}
// add by yangyb 2010-07-15 手机报包年、包半年 end
bool CRecvMonfee::getAllSubInfo(vector<UserSubInfo> &userSubInfoVector)
{    
    m_validSubInfoVector.clear();        
    
    for( ;m_userSubInfoIter!=userSubInfoVector.end();m_userSubInfoIter++)
    {
        if((*m_userSubInfoIter).m_endDate<=m_cycPara.m_beginTime ||       
           (*m_userSubInfoIter).m_startDate>m_cycPara.m_endTime )    
        {
            continue;
        }        
        
        //第一条订购
        if(m_tempSpCode == "" || m_tempSpBizCode == "" )
        {
            m_tempSpCode = (*m_userSubInfoIter).m_spCode;
            m_tempSpBizCode = (*m_userSubInfoIter).m_spBizCode;
        }
         
        //没有出现新的定购，则收集定购的信息
        if(m_tempSpCode == (*m_userSubInfoIter).m_spCode &&
           m_tempSpBizCode == (*m_userSubInfoIter).m_spBizCode )
        {
            //if((*m_userSubInfoIter).m_bizStateCode != 'E')//不考虑结束状态
            //{
                m_validSubInfoVector.push_back(m_userSubInfoIter);  
            //}                
        }
        //出现了新的定购（或者是用户定购结束标志），信息收集完成，可以收费了
        else
        {
            m_tempSpCode = (*m_userSubInfoIter).m_spCode;
            m_tempSpBizCode = (*m_userSubInfoIter).m_spBizCode;          
            return true;
        }    
    }
    
    
    if(m_validSubInfoVector.size()>0)
    {
        return true;
        
    }
    else
    {
        return false;
    }
}

bool CRecvMonfee::getValidSubInfo(
                          const vector<UserSubInfoIter> &userSubInfoVector,
                          const MonfeeTariff & monfeeTariff)
{    
    // Add by Malone 2008-1-26 start
    // 是否需要判断沉默单高用户

    if(monfeeTariff.m_judgeSilence == '1' || monfeeTariff.m_judgeAverage == '1' )
    {
    	//需要判断，并且是沉默单高用户，则不计费
    	if ( (*m_monfeeParamMgr).judgeSilenceUser(m_cycPara.m_cycleId,T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]), 
	                                           (*userSubInfoVector[0]).m_spCode,
	                                           (*userSubInfoVector[0]).m_spBizCode,
	                                           monfeeTariff))
    	{
    			return false;
    	}	
    }
    // Add by Malone 2008-1-26 end
    if(monfeeTariff.m_recvObjectType=='1')//对集团收费
    {
        if(m_dataType != FILE_TYPE_IN_A)
            return false;
    }
    if(monfeeTariff.m_billingCycle>MONTH_CYCTYPE_CODE)
    {        
        return false;    
    }   
    if(userSubInfoVector.size()<1)
    {
        return false;
    }   
    if(m_dataType=='B')//ADC  集团成员
    {
        if((*userSubInfoVector[0]).m_ecSerialNumber != "0")
        {
            //集团在本月没有'正常商用'状态
            if( (*m_monfeeParamMgr).judgeECAbnormalState(
                            (*userSubInfoVector[0]).m_ecUserId,       
                            (*userSubInfoVector[0]).m_spBizCode) )
            {
                return false;
            }
        }
    }    
    
    String<14> startDate = "20501231235959";
    String<14> endDate =   "19000101000000";
    bool recvTag = false;    
    //普通生效类型判断
    //正常生效
    char commonValidType = monfeeTariff.m_newUserRecvType;      

    //当月申请立即生效，当月取消，下月生效
    if (commonValidType == '0')
    {    
        vector<UserSubInfoIter>::const_iterator userSubInfoIter;
        for(userSubInfoIter=userSubInfoVector.begin();
            userSubInfoIter!=userSubInfoVector.end();userSubInfoIter++)
        {
            if((*(*userSubInfoIter)).m_bizStateCode == NORMAL_BIZSTATE_CODE)
            {
                startDate = (*(*userSubInfoIter)).m_startDate;
                break;
            }
        }
        if(userSubInfoIter==userSubInfoVector.end())
        {
            return false;
        }
    }
    //当月申请，立即生效；当月终止，立即生效
    else if (commonValidType == '1')
    {
        vector<UserSubInfoIter>::const_iterator userSubInfoIter;
        for(userSubInfoIter=userSubInfoVector.begin();
            userSubInfoIter!=userSubInfoVector.end();userSubInfoIter++)
        {
            if( (*(*userSubInfoIter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
            {            
                recvTag = true;
                if( (*(*userSubInfoIter)).m_endDate > endDate )
                {
                    endDate = (*(*userSubInfoIter)).m_endDate;
                }
                if((*(*userSubInfoIter)).m_startDate < startDate)
                {
                    startDate = (*(*userSubInfoIter)).m_startDate;
                }
            }
        }    
        if(!recvTag)
        {
            return false;
        }   
        if (endDate <= m_cycPara.m_endTime) 
        {
            return false;
        }
    }    
    //当月申请下月生效;当月终止，立即生效
    else if (commonValidType == '2')
    {
        vector<UserSubInfoIter>::const_iterator userSubInfoIter;
        for(userSubInfoIter=userSubInfoVector.begin();
            userSubInfoIter!=userSubInfoVector.end();userSubInfoIter++)
        {
            if( (*(*userSubInfoIter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
            {
                recvTag = true;
                if((*(*userSubInfoIter)).m_endDate > endDate)
                {
                    endDate = (*(*userSubInfoIter)).m_endDate;
                }
                if((*(*userSubInfoIter)).m_startDate < startDate)
                {
                    startDate = (*(*userSubInfoIter)).m_startDate;
                }
            }
        }
        if(!recvTag)
        {
            return false;
        } 
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
        vector<UserSubInfoIter>::const_iterator userSubInfoIter;
        for(userSubInfoIter=userSubInfoVector.begin();
            userSubInfoIter!=userSubInfoVector.end();userSubInfoIter++)
        {            
        	if( (*(*userSubInfoIter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
            {
                recvTag = true;
            	if((*(*userSubInfoIter)).m_startDate < startDate)
            	{
            	    startDate = (*(*userSubInfoIter)).m_startDate;
            	}
            }
        }
        if(!recvTag)
        {
            return false;
        } 
        //如果该用户是当月新开户用户，无效
        if (startDate >= m_cycPara.m_beginTime)
        {
            return false;
        }
    }        
    //月初免费三日、月末免费计费策略
	else if (commonValidType == '4' ) 
	{
		if(!freeInThreeDay(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	//免费试用一个月计费策略
	else if (commonValidType == '5' )
	{
		if (!freeInOneMonth(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	
  // Add by Malone 2008-5-28 start
  //免费试用一个月计费策略(历史首次订购)
	else if (commonValidType == '9' )
	{
		if (!freeInOneMonthHistory(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	// Add by Malone 2008-5-28 end	
	
	// Add by Malone 2008-6-27 start
  //免费试用72小时计费策略(历史首次订购)
	else if (commonValidType == 'A' )
	{
		if (!freeIn72Hours(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	// Add by Malone 2008-6-27 end
	
	//月末免费试用计费策略
	else if (commonValidType == '6' )
	{
		if (!freeInMonthEnd(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	//免费试用24小时计费策略
	else if (commonValidType == '7' )
	{
		if (!freeIn24Hours(userSubInfoVector,monfeeTariff))
		{
			return false;
		}
	}
	//立即计费策略
	else if (commonValidType == '8' )
	{
		vector<UserSubInfoIter>::const_iterator userSubInfoIter;
        for(userSubInfoIter=userSubInfoVector.begin();
            userSubInfoIter!=userSubInfoVector.end();userSubInfoIter++)
        {
            if((*(*userSubInfoIter)).m_bizStateCode == NORMAL_BIZSTATE_CODE)
            {                
                break;
            }
        }
        if(userSubInfoIter==userSubInfoVector.end())
        {
            return false;
        }
	}
    //不可识validType
    else
    {
        return false;
    }
        //优惠几个月
    if(monfeeTariff.m_validStartCycle>0)
    {
        CycParam startCycPara;
        if( (*m_monfeeParamMgr).getcycPara(startDate, startCycPara))
        {
            int cycNum=0;
            char cycleId1[6];
            char cycleId2[6];
            memset(cycleId1,0,6);
            memset(cycleId2,0,6);
            sprintf(cycleId1,"%ld",(long)m_cycPara.m_cycleId);
            sprintf(cycleId2,"%ld",(long)startCycPara.m_cycleId);
            monthDiff(cycleId2,cycleId1,cycNum);
            cycNum++;
            if(cycNum<=monfeeTariff.m_validStartCycle)
            {
                return false;
            }
        }
    } 

    if(monfeeTariff.m_validStartCycle>0 &&
       monfeeTariff.m_validEndCycle>0 )
    {
        CycParam startCycPara;            
        if( (*m_monfeeParamMgr).getcycPara(startDate, startCycPara))
        {
            int cycNum = m_cycPara.m_cycleId - startCycPara.m_cycleId + 1;
            if( (cycNum<monfeeTariff.m_validStartCycle) || 
                (cycNum>monfeeTariff.m_validEndCycle) )
            {
                return false;
            }
        }
    }        
    return true;    
}

void CRecvMonfee::getDuration(const String<14>& endDate, 
                               const String<14>& startDate,
                               int& dayNum)
{
    time_t endsecond,startsecond;  
    int    endyear,endmonth,endday,endhour,endminute,endseconds;
    int    startyear,startmonth,startday;
    struct tm p_end;
    struct tm p_start;
    
    endyear = atoi(endDate.substr(0,4).c_str());
    endmonth = atoi(endDate.substr(4,2).c_str());
    endday = atoi(endDate.substr(6,2).c_str());
    endhour = atoi(endDate.substr(8,2).c_str());
    endminute = atoi(endDate.substr(10,2).c_str());
    endseconds = atoi(endDate.substr(12,2).c_str());
    
    startyear = atoi(startDate.substr(0,4).c_str());
    startmonth = atoi(startDate.substr(4,2).c_str());
    startday = atoi(startDate.substr(6,2).c_str());  
    
    p_end.tm_sec = 0;
    p_end.tm_min = 0;
    p_end.tm_hour = 0;
    p_end.tm_mday = endday;
    p_end.tm_mon = endmonth - 1;
    p_end.tm_year = endyear - 1900;
    
    endsecond = mktime(&p_end);
    
    p_start.tm_sec = 0;
    p_start.tm_min = 0;
    p_start.tm_hour = 0;
    p_start.tm_mday = startday;
    p_start.tm_mon = startmonth - 1;
    p_start.tm_year = startyear - 1900;  
    
    startsecond = mktime(&p_start);
    
    if (endhour==0 && endminute==0 && endseconds==0)
    {
        dayNum = difftime(endsecond,startsecond)/24/3600;
    }
    else
    {
        dayNum = difftime(endsecond,startsecond)/24/3600 + 1;
    }
}

void CRecvMonfee::getSecond(const String<14> &end, 
                             const String<14> &start,
                             unsigned int& secondNum)
{	
	int stSecond=atoi(start.substr(12,2).c_str());
	int stMinute=atoi(start.substr(10,2).c_str());
	int stHour	=atoi(start.substr(8,2).c_str());
	int stDay	=atoi(start.substr(6,2).c_str());
	int stMonth	=atoi(start.substr(4,2).c_str());
	int stYear	=atoi(start.substr(0,4).c_str());

	int edSecond=atoi(end.substr(12,0).c_str());
	int edMinute=atoi(end.substr(10,2).c_str());
	int edHour	=atoi(end.substr(8,2).c_str());
	int edDay	=atoi(end.substr(6,2).c_str());
	int edMonth	=atoi(end.substr(4,2).c_str());
	int edYear	=atoi(end.substr(0,4).c_str());

	tm sttime,edtime;
	sttime.tm_sec	= stSecond;
	sttime.tm_min	= stMinute;
	sttime.tm_hour	= stHour;
	sttime.tm_mday	= stDay;
	sttime.tm_mon	= stMonth - 1;
	sttime.tm_year	= stYear - 1900;

	edtime.tm_sec	= edSecond;
	edtime.tm_min	= edMinute;
	edtime.tm_hour	= edHour;
	edtime.tm_mday	= edDay;
	edtime.tm_mon	= edMonth - 1;
	edtime.tm_year	= edYear - 1900;

	time_t time_st,time_ed;
	time_st= mktime(&sttime);
	time_ed= mktime(&edtime);
	secondNum = time_ed - time_st;
	return;
}

bool CRecvMonfee::freeInThreeDay(
						const vector<UserSubInfoIter> &userSubInfoVector,
						const MonfeeTariff & monfeeTariff)
{
	  bool	  isValid = false;  
	  bool    isNormalValid = false;//是否为往月订购
	  bool    isPause = false;//是否全月暂停
	  bool		isRepeat = false;//是否当月重复订购
	  String<14>	cancelTime="20501231000000";//紧随首次A状态的终止状态的开始时间
	  String<14>	priorTime="";//首次订购时间
	  
    String<14>  startTime="";//当月第一次订购开始时间
    String<14>  endTime="";//当月第一次订购结束时间
    String<14>  laststartTime="";//当月最后一次订购开始时间
    String<14>  lastendTime="";//当月最后一次订购结束时间

	vector<UserSubInfoIter>::const_iterator iter,iter1,iter2;
	for (iter=userSubInfoVector.begin();iter!=userSubInfoVector.end();++iter)
	{
				//全月都是暂停状态(N)
				if (((*(*iter)).m_bizStateCode == 'N' ||
	  	  	   (*(*iter)).m_bizStateCode == 'P')&&
	  	  	   (*(*iter)).m_startDate <= m_cycPara.m_beginTime &&
	  	  	   (*(*iter)).m_endDate >= m_cycPara.m_endTime )
	  	  {
	  	      isPause = true;
	  	  }
		
				if ( ((*(*iter)).m_bizStateCode == NORMAL_BIZSTATE_CODE||(*(*iter)).m_bizStateCode == 'N') && 
            (*(*iter)).m_endDate >= m_cycPara.m_beginTime &&
            (*(*iter)).m_startDate < m_cycPara.m_endTime 
            )
				{
						isValid=true;

						if (priorTime == "" )
						{
							priorTime=(*(*iter)).m_priorTime;
							startTime=(*(*iter)).m_startDate;
							endTime=(*(*iter)).m_endDate;
							laststartTime=(*(*iter)).m_startDate;
							lastendTime=(*(*iter)).m_endDate;
						}
						
						//当月第一次订购
						if ((*(*iter)).m_startDate < startTime)
						{
							startTime=(*(*iter)).m_startDate;
							endTime = (*(*iter)).m_endDate;
						}
						//当月最后一次订购
						if ((*(*iter)).m_startDate > laststartTime)
						{
							laststartTime=(*(*iter)).m_startDate;
							lastendTime = (*(*iter)).m_endDate;
						}
				}

	}
	
	 for (iter1=userSubInfoVector.begin();iter1!=userSubInfoVector.end();++iter1)
	  {        
        //取当月订购后的第一次终止的开始时间(暂停也算)
        if ( ((*(*iter1)).m_bizStateCode == 'E' ||(*(*iter1)).m_bizStateCode == 'P') &&
        		(*(*iter1)).m_startDate >= endTime)
        {
        	  if ((*(*iter1)).m_startDate < cancelTime)
        	  {
        	      cancelTime = (*(*iter1)).m_startDate;
        	  }
        }
    }
    
  if (priorTime < m_cycPara.m_beginTime)
		{
			isNormalValid=true;
		}
    
	if (!isValid || isPause)
	{
		return false;
	}
		
	//	往月定购
	if (isNormalValid)
	{		
		return true;
	}
	//	当月定购	
	else 
	{
		char halfjudgetime[20];
		sprintf(halfjudgetime,"%s%02d000000",
			m_cycPara.m_beginTime.substr(0,6).c_str(),
			monfeeTariff.m_judgeHalfTime);
				
		if (priorTime >= halfjudgetime)//21日0点--月底之间订购不收费用
		    {
		    	  return false;
		    }
		else
		{
			
			//判断是否重复订购
			if (lastendTime>endTime)
			{
					for (iter2=userSubInfoVector.begin();iter2!=userSubInfoVector.end();++iter2)
				  {
							if ( ( (*(*iter2)).m_bizStateCode != 'N' && (*(*iter2)).m_bizStateCode != NORMAL_BIZSTATE_CODE) && 
									 (*(*iter2)).m_startDate >= endTime &&
									 (*(*iter2)).m_startDate < lastendTime )
							{
								isRepeat=true;
							}
			    }
			}
			
			  if (lastendTime>cancelTime)
				{
					cancelTime=lastendTime;
				}  
			
			if (isRepeat)
			{	
				return true;
			}
			else
			{
					unsigned int secondNum;
					if(cancelTime < m_cycPara.m_endTime)
					{
						getSecond(cancelTime,priorTime,secondNum);
					}
					else
					{
						getSecond(m_currentDate,priorTime,secondNum);
					}
					if(secondNum > 72*3600) //定购超过3天
					{
						return true;
					}
					else
					{
						return false;
					}
			}

		}

	}
	return false;	
}

// Add by Malone 2008-5-28 start
//免费试用一个月计费策略(历史首次订购)
bool CRecvMonfee::freeInOneMonthHistory(
							const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff)
{
	bool	isValid = false;	
	String<14>	priorTime="";	

	vector<UserSubInfoIter>::const_iterator iter;
	for (iter=userSubInfoVector.begin();iter!=userSubInfoVector.end();++iter)
	{
		if ((*(*iter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
		{
			isValid=true;
			
			if (priorTime == "")
			{
				priorTime=(*(*iter)).m_priorTime;				
			}
			
			if(priorTime != "")
			{
				break;
			}
		}			
	}

	if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "")
	{
		return false;
	}
	
	//	往月定购
	if (priorTime < m_cycPara.m_beginTime)
	{	
			return true;//收费
	}
	//	当月定购	
	else 
	{		
		return false;//不收费	
	}
	return false;	
}
// Add by Malone 2008-5-28 end

bool CRecvMonfee::freeInOneMonth(
							const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff)
{
	bool	isValid = false;	
	String<14>	priorTime="";	
	
	vector<UserSubInfoIter>::const_iterator iter;
	for (iter=userSubInfoVector.begin();iter!=userSubInfoVector.end();++iter)
	{
		if ((*(*iter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
		{
			isValid=true;
			
			if (priorTime == "")
			{
				priorTime=(*(*iter)).m_priorTime;
			}
			
			if(priorTime != "")
			{
				break;
			}
		}
	}
	
	if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "")
	{
		return false;
	}
	
	//	往月定购
	if (priorTime < m_cycPara.m_beginTime)
	{
		//取得上月判断时间
		String<14> lastjudgetime;
		char temp[15];
		int month = atoi(m_cycPara.m_beginTime.substr(4,2).c_str());
		int year  = atoi(m_cycPara.m_beginT
bool CRecvMonfee::freeInOneMonth(
							const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff)
{
	bool	isValid = false;	
	String<14>	priorTime="";	
	
	vector<UserSubInfoIter>::const_iterator iter;
	for (iter=userSubInfoVector.begin();iter!=userSubInfoVector.end();++iter)
	{
		if ((*(*iter)).m_bizStateCode == NORMAL_BIZSTATE_CODE )
		{
			isValid=true;
			
			if (priorTime == "")
			{
				priorTime=(*(*iter)).m_priorTime;
			}
			
			if(priorTime != "")
			{
				break;
			}
		}
	}
	
	if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "")
	{
		return false;
	}
	
	//	寰