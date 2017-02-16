#include "CRecvMonfee.h"
CRecvMonfee::~CRecvMonfee()
{

}
CRecvMonfee::CRecvMonfee()
{
    m_userSubInfo = NULL;                  
    m_gRateInfo = NULL;
}

bool CRecvMonfee::freeInThreeDay(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	  bool	  isValid = false;//当月是否存在有效记录 
	  bool    isNormalValid = false;//是否为往月订购
	  bool    isPause = false;//是否全月暂停
	  bool		isRepeat = false;//是否当月重复订购
	  String<14>	cancelTime= m_UserSubInfoExtra.m_cancelTime;//紧随首次A状态的终止状态的开始时间
	  String<14>	priorTime= m_userSubInfo->m_priorTime;//首次订购时间	  
    String<14>  startTime= m_UserSubInfoExtra.m_startTime;//当月第一次订购开始时间
    String<14>  endTime= m_UserSubInfoExtra.m_endTime;//当月第一次订购结束时间
    String<14>  laststartTime= m_UserSubInfoExtra.m_laststartTime;//当月最后一次订购开始时间
    String<14>  lastendTime= m_UserSubInfoExtra.m_lastendTime;//当月最后一次订购结束时间
	  
	  if(m_UserSubInfoExtra.m_isValid == 1)
	  	isValid=true;
	  	
	  if(m_UserSubInfoExtra.m_subDateSign == 1)
	  	isNormalValid=true;
	  
	  if(m_UserSubInfoExtra.m_isAllPause == 1)
	  	isPause=true;
	  
	  if(m_UserSubInfoExtra.m_isRepeat == 1)
	  	isRepeat=true;

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
			(m_gRateInfo->m_cycParam).m_beginTime.substr(0,6).c_str(),
			monfeeTariff.m_judgeHalfTime);
				
		if (priorTime >= halfjudgetime)//21日0点--月底之间订购不收费用
		    {
		    	  return false;
		    }
		else
		{
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
					if(cancelTime < ((m_gRateInfo->m_cycParam)).m_endTime)
					{
						(m_gRateInfo->m_comBill).getSecond(cancelTime,priorTime,secondNum);
					}
					else
					{
						(m_gRateInfo->m_comBill).getSecond(m_currentDate,priorTime,secondNum);
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

bool CRecvMonfee::freeInOneMonth(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	bool	  isValid = false;//当月是否存在有效记录
	String<14>	priorTime = m_userSubInfo->m_priorTime;//首次订购时间
	
	if(m_UserSubInfoExtra.m_isValid == 1)
	  	isValid=true;
	  	
  if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "")
	{
		return false;
	}
	
	//	往月定购
	if (priorTime < ((m_gRateInfo->m_cycParam)).m_beginTime)
	{
		//取得上月判断时间
		String<14> lastjudgetime;
		char temp[15];
		int month = atoi((m_gRateInfo->m_cycParam).m_beginTime.substr(4,2).c_str());
		int year  = atoi((m_gRateInfo->m_cycParam).m_beginTime.substr(0,4).c_str());
		if (month > 1)
		{
			month -= 1;
		}
		else
		{
			month = 12;
			year -= 1;
		}
		sprintf(temp,"%04d%02d%02d000000",year,month,
									monfeeTariff.m_judgeHalfTime);
		lastjudgetime = temp;

		//定购时间是上月21日0点－月底之间定购业务
		if(priorTime >= lastjudgetime)
		{
			return false;
		}
		//定购时间是上月21日0点以前
		else
		{		
			return true;
		}
	}
	//	当月定购	
	else 
	{		
		return false;		
	}
	return false;	
}	  

bool CRecvMonfee::freeInMonthEnd(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	bool	  isValid = false;//当月是否存在有效记录 
	String<14>	priorTime= m_userSubInfo->m_priorTime;//首次订购时间	  
  String<14>	lastTime= m_userSubInfo->m_lastTime;//最后一次订购时间
  
  	
	if(m_UserSubInfoExtra.m_isValid == 1)
	  	isValid=true;
	  	
	  	
  if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "" || lastTime == "")
	{
		return false;
	}
	
	//	往月定购
	if (priorTime < (m_gRateInfo->m_cycParam).m_beginTime)
	{		
		return true;	
	}
	//	当月定购	
	else 
	{	
		char halfjudgetime[20];
		sprintf(halfjudgetime,"%s%02d000000",
			(m_gRateInfo->m_cycParam).m_beginTime.substr(0,6).c_str(),
			monfeeTariff.m_judgeHalfTime);	
			
		if (priorTime >= halfjudgetime)//21日0点--月底之间订购
		{
			//首次定购免费
			//月底前重复定购，不进行收费
			//总结： 免费
			return false;			
		}
		else //1日--21日0点之间订购                        
		{
			//首次定购, 收费
			//重复定购，收费
			//总结：收费
			return true;
		}					
	}
	return false;
}

bool CRecvMonfee::freeIn24Hours(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	bool	  isValid = false;//当月是否存在有效记录 
	String<14>	priorTime= m_userSubInfo->m_priorTime;//首次订购时间	  
  String<14>	lastTime= m_userSubInfo->m_lastTime;//最后一次订购时间
  String<14>	cancelTime= m_userSubInfo->m_endDate;//A状态下的最大结束时间
  
  	
	if(m_UserSubInfoExtra.m_isValid == 1)
	  	isValid=true;
	  	
	  	
  if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "" || lastTime == "")
	{
		return false;
	}
	
	//	历史上没有定购过该业务	
	if (priorTime == lastTime)
	{		
		unsigned int secondNum;
		if(cancelTime < (m_gRateInfo->m_cycParam).m_endTime)  //当月退订
		{
			(m_gRateInfo->m_comBill).getSecond(cancelTime,priorTime,secondNum);
			if(secondNum > 24*3600) //定购超过24小时
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else  //没有退订 
		{
			(m_gRateInfo->m_comBill).getSecond(m_currentDate,priorTime,secondNum);
			if(secondNum > 24*3600) //定购超过24小时
			{
				return true;
			}
			else
			{
				return false;
			}					
		}		
	}
	//	历史上定购过该业务	
	else 
	{	
		return true;							
	}
	return false;
}

bool CRecvMonfee::freeInOneMonthHistory(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	bool	  isValid = false;//当月是否存在有效记录
	String<14>	priorTime= m_userSubInfo->m_priorTime;//首次订购时间	  
	
	if(m_UserSubInfoExtra.m_isValid == 1)
	  isValid=true;
	  
  if (!isValid)
	{
		return false;
	}
	
	if (priorTime == "")
	{
		return false;
	}
	
	//	往月定购
	if (priorTime < (m_gRateInfo->m_cycParam).m_beginTime)
	{	
			return true;
	}
	//	当月定购	
	else 
	{		
		return false;
	}
	return false;		
}

bool CRecvMonfee::freeIn72Hours(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff)
{
	  bool	  isValid = false;//当月是否存在有效记录 
	  bool    isNormalValid = false;//是否为往月订购
	  bool    isPause = false;//是否全月暂停
	  bool		isRepeat = false;//是否当月重复订购
	  String<14>	cancelTime= m_UserSubInfoExtra.m_cancelTime;//紧随首次A状态的终止状态的开始时间
	  String<14>	priorTime= m_userSubInfo->m_priorTime;//首次订购时间	  
    String<14>  startTime= m_UserSubInfoExtra.m_startTime;//当月第一次订购开始时间
    String<14>  endTime= m_UserSubInfoExtra.m_endTime;//当月第一次订购结束时间
    String<14>  laststartTime= m_UserSubInfoExtra.m_laststartTime;//当月最后一次订购开始时间
    String<14>  lastendTime= m_UserSubInfoExtra.m_lastendTime;//当月最后一次订购结束时间
	  
	  if(m_UserSubInfoExtra.m_isValid == 1)
	  	isValid=true;
	  	
	  if(m_UserSubInfoExtra.m_subDateSign == 1)
	  	isNormalValid=true;
	  
	  if(m_UserSubInfoExtra.m_isAllPause == 1)
	  	isPause=true;
	  
	  if(m_UserSubInfoExtra.m_isRepeat == 1)
	  	isRepeat=true;
	  
	  
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
			(m_gRateInfo->m_cycParam).m_beginTime.substr(0,6).c_str(),
			monfeeTariff.m_judgeHalfTime);
			
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
					if(cancelTime < (m_gRateInfo->m_cycParam).m_endTime)
					{
						(m_gRateInfo->m_comBill).getSecond(cancelTime,priorTime,secondNum);
					}
					else
					{
						(m_gRateInfo->m_comBill).getSecond(m_currentDate,priorTime,secondNum);
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
	return false;		  
}
	
void CRecvMonfee::getDealDate()
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

bool CRecvMonfee::adjustRecvFeeByRealDay(const int currentDate,const char recvMode,const int referFee,int &newFee)
{
	
  //当月总天数
  int  month_day;
  //半月天数
  int  half_days;
  month_day=half_days=0;
  switch(recvMode)
  {
    case '0':   //正常显示（按汇总费用显示） NORMAL_DISPALY
    case '2':   //江苏联通模式3(按月收取，一次性收取，区分上下半月。月初显示,区分新享受用户)
      newFee = referFee;
      break;
    case '1':   //江苏联通模式2(按月收取，按天分摊收取，区分上下半月,区分新享受用户 )
      month_day  = atoi(((m_gRateInfo->m_cycParam).m_endTime.substr(0,8)).c_str())%100;
      half_days  = atoi(((m_gRateInfo->m_cycParam).m_halfTime.substr(0,8)).c_str())%100;
      if(m_UserSubInfoExtra.m_subDateSign != 1)
      {       
        if((currentDate%100)==month_day)
        {      
           newFee= referFee;         
        }
        else
        {
          int stServDay = atoi((m_userSubInfo->m_startDate).substr(6,2).c_str());
          if((currentDate%100)<stServDay)
          {
            newFee = 0;
          }
          else
          {
            newFee = (referFee*1.0/month_day)*(currentDate%100-stServDay+1)+0.5;
          }
        }
      }  
      else
      {        
        newFee=referFee*1.0*(currentDate%100)/month_day+0.5;      
      }            
      break;
    default: ;
  }  
  return true;
}

bool CRecvMonfee::isValidSubInfo(const UserSubInfo &userSubInfo,const MonfeeTariff & monfeeTariff)
{
    // 是否需要判断沉默单高用户
    if(monfeeTariff.m_judgeSilence == '1' || monfeeTariff.m_judgeAverage == '1' )
    {
    	if ((m_gRateInfo->m_comBill).judgeSilenceUser((m_gRateInfo->m_cycParam).m_cycleId,T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]),monfeeTariff))
    	{
    			return false;
    	}	
    }

    if(monfeeTariff.m_recvObjectType=='1')//对集团收费
    {
        if(m_fileType != FILE_TYPE_IN_A)
            return false;
    }
    if(monfeeTariff.m_billingCycle>MONTH_CYCTYPE_CODE)
    {        
        return false;    
    }   
       
    if(m_fileType=='B')//ADC  集团成员
    {
        if(userSubInfo.m_ecSerialNumber != "0")
        {
            //集团在本月没有'正常商用'状态
            if((m_gRateInfo->m_comBill).judgeECAbnormalState(userSubInfo.m_ecUserId,userSubInfo.m_spBizCode))
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
        if(userSubInfo.m_bizStateCode != NORMAL_BIZSTATE_CODE)
        {
            return false;
        }
        else
        {
            startDate = userSubInfo.m_startDate;//最小的开始时间
        }
    }
    //当月申请，立即生效；当月终止，立即生效
    else if (commonValidType == '1')
    {
        if( userSubInfo.m_bizStateCode == NORMAL_BIZSTATE_CODE )
        {            
            recvTag = true;
            endDate = userSubInfo.m_endDate;//最大的结束时间
            startDate = userSubInfo.m_startDate;//最小的开始时间
        }  
        if(!recvTag)
        {
            return false;
        }   
        if (endDate <= (m_gRateInfo->m_cycParam).m_endTime) 
        {
            return false;
        }
    }    
    //当月申请下月生效;当月终止，立即生效
    else if (commonValidType == '2')
    {
        if( userSubInfo.m_bizStateCode == NORMAL_BIZSTATE_CODE )
         {
            recvTag = true;            
            endDate = userSubInfo.m_endDate;
            startDate = userSubInfo.m_startDate;
         }
        if(!recvTag)
        {
            return false;
        } 
        if ((endDate <= (m_gRateInfo->m_cycParam).m_endTime) || (startDate>= (m_gRateInfo->m_cycParam).m_beginTime))
        {
            return false;
        }
    }
    //当月申请下月生效;当月终止，下月生效
    else if (commonValidType == '3')
    {           
       if( userSubInfo.m_bizStateCode == NORMAL_BIZSTATE_CODE )
         {
            recvTag = true;
            startDate = userSubInfo.m_startDate;
         }
        if(!recvTag)
        {
            return false;
        } 
        if (startDate >= (m_gRateInfo->m_cycParam).m_beginTime)
        {
            return false;
        }
    }        
    //月初免费三日、月末免费计费策略
    else if (commonValidType == '4' ) 
    {
    	if(!freeInThreeDay(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }
    //免费试用一个月计费策略
    else if (commonValidType == '5' )
    {
    	if (!freeInOneMonth(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }
    //免费试用一个月计费策略(历史首次订购)
    else if (commonValidType == '9' )
    {
    	if (!freeInOneMonthHistory(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }
    //免费试用72小时计费策略(历史首次订购)
    else if (commonValidType == 'A' )
    {
    	if (!freeIn72Hours(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }	
    //月末免费试用计费策略
    else if (commonValidType == '6' )
    {
    	if (!freeInMonthEnd(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }
    //免费试用24小时计费策略
    else if (commonValidType == '7' )
    {
    	if (!freeIn24Hours(userSubInfo,monfeeTariff))
    	{
    		return false;
    	}
    }
    //立即计费策略
    else if (commonValidType == '8' )
    {
          if(userSubInfo.m_bizStateCode != NORMAL_BIZSTATE_CODE)
          {
              return false;
          }
    }
    else
    {
          return false;
    }
      
    if(monfeeTariff.m_validStartCycle>0 && monfeeTariff.m_validEndCycle>0)
      {
          CycParam startCycPara;            
          if((m_gRateInfo->m_paramMgr)->getcycPara(startDate, startCycPara))
          {
              int cycNum = (m_gRateInfo->m_cycParam).m_cycleId - startCycPara.m_cycleId + 1;
              if( (cycNum<monfeeTariff.m_validStartCycle) || (cycNum>monfeeTariff.m_validEndCycle) )
              {
                  return false;
              }
          }
      }        
    return true;    
}

bool CRecvMonfee::dealEffect(MonfeeTariff &monfeeTariff,UserSubInfo &userSubInfo,const int &dayNum)
{
    string userSubInfoStartDateStringI = m_UserSubInfoExtra.m_laststartTime.substr(0,6);
    string userSubInfoStartDateStringII = m_UserSubInfoExtra.m_laststartTime.substr(6,2);
    int userSubInfoStartDateI = atoi(userSubInfoStartDateStringI.c_str());
    int userSubInfoStartDateII = atoi(userSubInfoStartDateStringII.c_str());
    

    long t_halfPrice = 0,t_price = 0;
    char *expr = NULL;
    if(monfeeTariff.m_halfPrice[0] == 'E')
    	{
    		 expr = &monfeeTariff.m_halfPrice[1];
			   if (!m_gRateInfo->explanExpr(t_halfPrice,expr))
			   {
			   	return false;
			   }
    	}
     else
     	{
     		t_halfPrice = atoi(monfeeTariff.m_halfPrice);
     	}
     	
     if(monfeeTariff.m_price[0] == 'E')
    	{
    		 expr = &monfeeTariff.m_price[1];
			   if (!m_gRateInfo->explanExpr(t_price,expr))
			   {
			   	return false;
			   }
    	}
    	else
    	{
    		t_price = atoi(monfeeTariff.m_price);
    	}
    if((monfeeTariff.m_judgeHalfTime != 0) &&
      (userSubInfoStartDateI  == (m_gRateInfo->m_cycParam).m_cycleId) &&
      (userSubInfoStartDateII >= monfeeTariff.m_judgeHalfTime))   
    {            
        m_fee = t_halfPrice;
    }
    else
    {
        m_fee = t_price;
    }
    
    //江苏联通收取模式需求:
    //            recvMode 0    :  全月一次性收取
    //            recvMode 1    :  上下半月按天月末补收
    //            recvMode 2    :  上下半月一次性收取
    //            recvmode 3    :  包天（按当月订购次数计算：订购次数*包天费用） 
    //            recvMode 其他 :  全月收取，按天显示  
    int iCurrentDate;
    bool bTempVaildTag = false;  
    
    String<14> currentDate=m_currentDate;
       
    if (currentDate > (m_gRateInfo->m_cycParam).m_endTime)
    {
        currentDate=(m_gRateInfo->m_cycParam).m_endTime;
    }
    iCurrentDate = atoi(currentDate.substr(0,8).c_str()); 
        
    if(monfeeTariff.m_displayType == '0')
    {
        m_displayFee = m_fee;
    }
    else if (monfeeTariff.m_displayType == '1')
    {
        m_displayFee = 0;
        bTempVaildTag = adjustRecvFeeByRealDay(iCurrentDate,monfeeTariff.m_displayType,m_fee,m_displayFee);
        
        if(!bTempVaildTag)
        {
            m_displayFee = m_fee;
        }    
    }
    else if (monfeeTariff.m_displayType == '2')
    {
        m_displayFee = 0;
        bTempVaildTag = adjustRecvFeeByRealDay(iCurrentDate,monfeeTariff.m_displayType,m_fee,m_displayFee);
        
        if(!bTempVaildTag)
        {
            m_displayFee = m_fee;
        }  
    }
    
    else if (monfeeTariff.m_displayType == '3')
    {
        m_displayFee = m_fee * dayNum;
    }
    else
    {
        int currentDayNum=0,monthDayNum=31;    	
        (m_gRateInfo->m_comBill).getDuration(currentDate, 
                    (m_gRateInfo->m_cycParam).m_beginTime,currentDayNum);

        (m_gRateInfo->m_comBill).getDuration((m_gRateInfo->m_cycParam).m_endTime,
                    (m_gRateInfo->m_cycParam).m_beginTime,monthDayNum);    
                    
        m_displayFee = 1.0 * m_fee * currentDayNum / monthDayNum + 0.5;            
    }
  if(!(m_gRateInfo->m_comBill).ifItemExists(monfeeTariff.m_itemCode))
  	  {
  	#ifdef DEBUG
   	cout<<__FILE__<<":"<<__LINE__<<" Item: "<<monfeeTariff.m_itemCode<<" length is out of range! MAX_ITEM="<<MAX_ITEM<<endl;
    #endif	 	
		 (m_gRateInfo->m_comBill).setErrorCode(m_gRateInfo->m_userId,E_ACCT_ITEM,"0",USER_ID_TYPE,monfeeTariff.m_itemCode);
	 	 return false;
  }
  
    UserDetailBillMonfee userDetailBill;
    userDetailBill.m_itemCode=monfeeTariff.m_itemCode;
    userDetailBill.m_fee=m_displayFee;        
    userDetailBill.m_disPlayFee=m_displayFee;
    userDetailBill.m_discntFee=0;
    userDetailBill.m_discntCode=0;
    userDetailBill.m_discntType='0';
    userDetailBill.m_displayType=monfeeTariff.m_displayType; 
    userDetailBill.m_feeType=monfeeTariff.m_feeType;	
    userSubInfo.m_userDetailBill.push_back(userDetailBill);

  	
    
    return true;         
}    

bool CRecvMonfee::dealTariff(UserSubInfo & userSubInfo)
{  
	 if ( m_isFileE == FILE_TYPE_IN_E )
    {
    		UserDetailBillMonfee userDetailBill;
        userDetailBill.m_itemCode=0;
        userDetailBill.m_fee=0;        
        userDetailBill.m_disPlayFee=0;
        userDetailBill.m_discntFee=0;
        userDetailBill.m_discntCode=0;
        userDetailBill.m_discntType='0';
    
        userSubInfo.m_userDetailBill.clear();
        
        userSubInfo.m_userDetailBill.push_back(userDetailBill);   
        
        //(*m_userSubBillVector).push_back(userSubInfo);
        
        return true;
    }  
    
   SpStateVector spStateVector;
   if ((m_gRateInfo->m_comBill).getSpPunishRule(userSubInfo.m_spCode, "0", spStateVector))
   {	
   	if (spStateVector.size() != 0)
	   {
	   	  UserDetailBillMonfee usrDetailBill;
	      usrDetailBill.m_itemCode=0;
	      usrDetailBill.m_fee=0;        
	      usrDetailBill.m_disPlayFee=0;
	      usrDetailBill.m_discntFee=0;
	      usrDetailBill.m_discntCode=0;
	      usrDetailBill.m_discntType='0';
	      
	      userSubInfo.m_userDetailBill.clear();
	      userSubInfo.m_userDetailBill.push_back(usrDetailBill);   
	   	  //(*m_userSubBillVector).push_back(userSubInfo);
	   	   return true;
	   } 
	 }       
	 
   //判断sp业务是否被处罚
   spStateVector.clear();
   
   if ((m_gRateInfo->m_comBill).getSpPunishRule(userSubInfo.m_spCode, userSubInfo.m_spBizCode, spStateVector))
   {
	   if(spStateVector.size() != 0)
	   {
	   	  UserDetailBillMonfee usrDetailBill;
	      usrDetailBill.m_itemCode=0;
	      usrDetailBill.m_fee=0;        
	      usrDetailBill.m_disPlayFee=0;
	      usrDetailBill.m_discntFee=0;
	      usrDetailBill.m_discntCode=0;
	      usrDetailBill.m_discntType='0';
	      
	      userSubInfo.m_userDetailBill.clear();
	      userSubInfo.m_userDetailBill.push_back(usrDetailBill);   
	   	  //(*m_userSubBillVector).push_back(userSubInfo);
	   	   return true;
	   }
	 }
   
   //获取资费
   int iRet = 0;
   iRet = (m_gRateInfo->m_comBill).getTariffBySubInfo(m_fileType,userSubInfo,m_monfeeTariffVector);

		if (iRet <= 0)
		{
			return false;
		}

   for (vector<MonfeeTariff>::iterator 
       dit= m_monfeeTariffVector.begin();
       dit!=m_monfeeTariffVector.end();++dit)
   {
   		
       if(isValidSubInfo(userSubInfo, *dit))
       {
            int dayNum= m_UserSubInfoExtra.m_subUseDayNum;
             
            if(!dealEffect(*dit,userSubInfo,dayNum))
            	return false;
       }
   }
   if( userSubInfo.m_userDetailBill.size() == 0 )
   {
       UserDetailBillMonfee usrDetailBill;
       usrDetailBill.m_itemCode=0;
       usrDetailBill.m_fee=0;        
       usrDetailBill.m_disPlayFee=0;
       usrDetailBill.m_discntFee=0;
       usrDetailBill.m_discntCode=0;
       usrDetailBill.m_discntType='0';
       
       userSubInfo.m_userDetailBill.push_back(usrDetailBill);         
   }   
   //(*m_userSubBillVector).push_back(userSubInfo);
   return true;          
}

bool CRecvMonfee::Initialize(UserSubInfoExtra userSubInfoExtra,UserSubInfo *userSubInfo,
                             char fileType,IdInfo idInfo,
                             RateGlobalInfo  *pRateInfo,char isFileE)
{
	m_userSubInfo         = userSubInfo;
  m_fileType            = fileType;
  m_idInfo              = idInfo;
  m_isFileE             = isFileE;
  m_gRateInfo           = pRateInfo;
  	m_UserSubInfoExtra    = userSubInfoExtra;
  getDealDate();
  return true;
}

bool CRecvMonfee::recvMonfee()
{
	
    //销号用户不收
		if(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] == "5" ||
			 ((m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "0" && m_idInfo.m_idAttr[IdInfo::ID_REMOVE_TAG] != "5") &&
		   ((T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_PREDESTROY_TIME]) < (m_gRateInfo->m_cycParam).m_beginTime) ||
		   (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_REMOVE_DATE]) < (m_gRateInfo->m_cycParam).m_beginTime))))
    {
        return false;
    }
    
    bool isNotOpen = false;
    //全月停机不收,包年、包半年不在此列
    if((m_gRateInfo->m_comBill).judgeNotOpen((m_gRateInfo->m_cycParam).m_beginTime,
                                 (m_gRateInfo->m_cycParam).m_endTime,
                                  m_gRateInfo->m_vecUserServState))
    {
        isNotOpen = true;
    }
    
	  if(isNotOpen && m_userSubInfo->m_billFlag !=ADDUP_CYCLE_TYPE_YEAR&&
	  	 m_userSubInfo->m_billFlag!=ADDUP_CYCLE_TYPE_YEAR_HALF)
	  {
	  	return false;
	  }
    if(!dealTariff(*m_userSubInfo))
    	return false;
    return true;
}