#include "Monfeeproc.h"
#include "RateError.h"

INSTALL_PROCEDURE_INSTANCE(Monfeeproc);

Monfeeproc::Monfeeproc()
{
  m_userId  = INVALID_USERID;
	m_unitfee_M = 1000;
	m_unitfee_T = 1000;
}
	
Monfeeproc::~Monfeeproc()
{
	m_monfeeDiscountVector.clear();
	m_monFeeBillNew.clear();
	m_monFeeBillOld.clear();
	m_cdrBillVectorNew.clear();
	m_cdrBillVectorOld.clear();
	m_outBillVector.clear();
	m_userBillVector.clear();
	m_iphoneVec.clear();
}


int Monfeeproc::execute()
{
	//清理
	m_monfeeDiscountVector.clear();
	m_monFeeBillNew.clear();
	m_monFeeBillOld.clear();
	m_cdrBillVectorNew.clear();
	m_cdrBillVectorOld.clear();
	m_outBillVector.clear();
	m_userBillVector.clear();
	m_outBillVector_inc.clear();
	m_userSubInfo.m_userDetailBill.clear();
  m_pRateInfo->outputBillVector.clear(); 

    
	if(!Initialize())
		{
			#ifdef _DEBUG_
        cout<<"Monfeeproc's Initialize failed! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
		  return 1;
		}

	if(!recvMonfee())
		{
	 		#ifdef _DEBUG_
        cout<<"deal MonfeeRecv failed! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
		  return 1;
		}
		
	if(!discntMonfee())
		{
	 		#ifdef _DEBUG_
        cout<<"deal MonfeeDiscnt failed! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
		  return 1;
		}

	if(!(m_pRateInfo->m_comBill).getUserMonfeeBill(m_userId,m_userSubInfo.m_spCode,
		                                             m_userSubInfo.m_spBizCode,
		                                             m_pRateInfo->m_userMonfeeBills_new,
		                                             m_monFeeBillOld))
		{
	 		#ifdef _DEBUG_
        cout<<"get old MonfeeBill empty! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
		}
	
	//根据用户账单生成新的包月费账单 、输出账单及bill_user_x
	if(!getAllNewBill())
	 	{
	 		#ifdef _DEBUG_
        cout<<"get new bill failed! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
		  return 1;
		 }
	
	//对bill_user的更新转到过程PriceProc中处理
	//更新bill_user_x数据，只更新fee_type=‘Y'的记录和iphone产品的MT记录。
//	for(vector<UserBill>::iterator itr_u = m_userBillVector.begin();
//		  itr_u != m_userBillVector.end(); itr_u++)
//	  {
//	    #ifdef _DEBUG_
//        cout<<"------------------------用户账单------------------"<<endl;
//        cout<<"m_userId;      "<<itr_u->m_userId      <<endl;
//        cout<<"m_billId;      "<<itr_u->m_billId      <<endl;
//        cout<<"m_value;       "<<itr_u->m_value       <<endl;
//        cout<<"m_discntValue; "<<itr_u->m_discntValue <<endl;
//        cout<<"m_adjustBefore;"<<itr_u->m_adjustBefore<<endl;
//      #endif
//	  	m_pRateInfo->updateBillTotal<UserBill>(m_pRateInfo->m_userBills_new,*itr_u);
//	  }
	//更新包月费账单MonFeeBill
	for(vector<UserMonfeeBill>::iterator itr_m = m_monFeeBillNew.begin();
		  itr_m != m_monFeeBillNew.end(); itr_m++)
	  {   
	  	m_pRateInfo->updateBillTotal<UserMonfeeBill>(m_pRateInfo->m_userMonfeeBills_new,*itr_m);
	  }

	//获取老账单
	 if(!getOldCdrBill())
	 	{
	 		#ifdef _DEBUG_
        cout<<"get old CdrBill failed!"<<__FILE__<<__LINE__<<endl;
      #endif
		 }
		
	//生成各类输出账单vector
	 if(!getOutBillVector())
	 	{
	 		#ifdef _DEBUG_
        cout<<"get Bills out  failed!"<<__FILE__<<__LINE__<<endl;
      #endif
		}
	//输出各类文件由outbill完成	
	
	//增加判断不存在MT值的不经过批价过程直接更新
	
	//根据outputBillVector格式化CDR	
	string t_dealtime(m_pRateInfo->m_dealTime);
	m_pcdr[ATTRI_SOURCE_TYPE]=MONFEE_SOURCE_TYPE;
	m_pcdr[ATTRI_RR_FID]=MONFEE_FID;
	m_pcdr[ATTRI_RR_FLAG]=MONFEE_RR_FLAG;
	m_pcdr[ATTRI_CYCLE_DATE]=(m_pRateInfo->m_cycParam).m_cycleId;
	
  char userType = '1';
	if(m_fileType == FILE_TYPE_IN_A)
	 	{
	 		userType = '0';
	 	}
	m_pcdr[ATTRI_USER_TYPE_A]=userType;
	
	m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ]=m_idInfo.m_idAttr[IdInfo::ID_BRAND_CODE];
	m_pcdr[ATTRI_USER_ID]=m_userId;
	m_pcdr[ATTRI_MSISDN_A]=m_idInfo.m_idAttr[IdInfo::ID_MSISDN];
	
	m_pcdr[ATTRI_HOME_CODE_A]=m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE];
	m_pcdr[ATTRI_APPLY_DATE]=t_dealtime.substr(0,8);
	m_pcdr[ATTRI_APPLY_TIME]=t_dealtime.substr(8,6);
	
	m_pcdr[ATTRI_CUST_ID]= m_idInfo.m_idAttr[IdInfo::ID_CUST_ID];
	
	m_pcdr[ATTRI_CHANNEL_NO]=m_pRateInfo->m_channelNo;
	
	m_pcdr[ATTRI_OFFICE_CODE]=m_idInfo.m_idAttr[IdInfo::ID_CITY_CODE];
	
  m_pcdr[ATTRI_OPEN_DATE]=m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE];
 
  vector<UserServState>::iterator iter=m_pRateInfo->m_vecUserServState.begin();
  
  int t_sign=0;
  char t_servStateCode;
  for(;iter<m_pRateInfo->m_vecUserServState.end();iter++)
  {
  	if(iter->m_endDate>(m_pRateInfo->m_cycParam).m_endTime && iter->m_mainTag=='1')
  		{
  			t_servStateCode=iter->m_servStateCode;
  			t_sign++;
  		}
  }
  
  if(t_sign>1)
  	{
  		#ifdef _DEBUG_
        cout<<"用户状态有误!"<<__FILE__<<__LINE__<<endl;
      #endif
  	}
  	
  m_pcdr[ATTRI_USER_STATE]=t_servStateCode;
  
  //文件名处理eventor中定义
  m_pcdr[ATTRI_FILE_NO] = m_pRateInfo->m_fileName.substr(1,m_pRateInfo->m_fileName.length()).c_str();
  m_pRateInfo->m_fileName = m_pcdr[ATTRI_FILE_NO];//重置文件名，保持流程一致性
  m_pcdr[ATTRI_ERROR_CODE]="000";

  m_pcdr[ATTRI_RATE_TIMES]='2';
  m_pcdr[ATTRI_USER_SERVICE_TYPE_A]="";
  
	m_pcdr[ATTRI_USER_STATE_GROUP]='0';
	m_pcdr[ATTRI_DOUBLEMODE]='0';
	
  bool istp = false;
	vector<OutBill>::iterator t_iter =m_outBillVector_inc.begin();
	for(;t_iter<m_outBillVector_inc.end();t_iter++)
	{	
		//如果是E文件对于M/T需要特殊处理	
		//格式化CDR
		m_pcdr[ATTRI_SP_BIZ_CODE]=t_iter->m_bizTypeCode;
		m_pcdr[ATTRI_SUBJECT]=t_iter->m_ecSerialNumber;
		m_pcdr[ATTRI_SUBSCRIBE_GATE]=t_iter->m_sourceType;
		m_pcdr[ATTRI_SP_CODE]=t_iter->m_spCode;
		m_pcdr[ATTRI_SERVICE_CODE]=t_iter->m_spServCode;
		m_pcdr[ATTRI_OPER_CODE]=(t_iter->m_spBizCode).substr(0,4)=="SPMT"?(*t_iter).m_spBizCode.substr(4,(*t_iter).m_spBizCode.length()-4):(*t_iter).m_spBizCode;
		m_pcdr[ATTRI_MONTH_FEE_ORG]=t_iter->m_fee;
		m_pcdr[ATTRI_ORGFEE_FIELD1]=t_iter->m_fee + t_iter->m_discntFee;
		m_pcdr[ATTRI_RECEIVE_TYPE]=t_iter->m_displayType;
		m_pcdr[ATTRI_SP_FEETYPE]=t_iter->m_feeType;
    
    if(t_iter->m_feeType != INIT_FEETYPE)
    { 
      //m_pRateInfo->m_pRateTpList.clear();
      //m_pRateInfo->m_pAcctTpList.clear();
      //m_pRateInfo->m_pRentTpList.clear();
      //m_pRateInfo->m_pDataTpList.clear();
      //m_pRateInfo->m_userTpList.clear();
      //m_pRateInfo->m_tpInfoList.clear();
      
      FP_BEGIN(TpProc)
      ProcedureHelper::ExecuteProcedure("TpProc");
      FP_END(TpProc);
      
      istp = true;
    }
    else
    	{
    		    m_pRateInfo->getFeeFields(m_pRateInfo->m_hasFeeFlag);
    	}
    FP_BEGIN(PriceProc)
    ProcedureHelper::ExecuteProcedure("PriceProc");
    FP_END(PriceProc);    
            
    //文件输出处理
    m_pcdr->outputCdr(m_pRateInfo->m_OutputRule->m_pFormat); //CDR转成字符串        
    #ifdef _DEBUG_
            cout << "m_pcdr->m_desBuff=" << m_pcdr->m_desBuff << endl;
    #endif
    m_pRateInfo->m_monfeeOutputCdrList.push_back(PString(m_pcdr->m_desBuff));
	}
	 if(!istp||m_outBillVector_inc.empty())
	 	{
  	FP_BEGIN(TpProc)
    ProcedureHelper::ExecuteProcedure("TpProc");
    FP_END(TpProc);
   }
	return 0; 	
}

//初始化操作
bool Monfeeproc::Initialize()
{
	//m_pcdr=m_pRateInfo->m_pcdr;
	
	
   m_fileType = m_pRateInfo->m_fileName[0];

	if(m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_MONFEE && m_fileType != FILE_TYPE_IN_E)
		return false;
  

  if(m_fileType == FILE_TYPE_IN_E)
  	{
  		m_isFileE = FILE_TYPE_IN_E;
  	}
  	
  //获取系统参数信息
  ProcSysParam l_sysParam;
  if (m_pRateInfo->m_paramMgr->getSysParam("MONFEE_UNITFEE_M",0,l_sysParam))
  {
    if(l_sysParam.m_valueN != 0)
  	  m_unitfee_M =l_sysParam.m_valueN;
  	else
  		{
  		#ifdef _DEBUG_
        cout<<"获取系统参数：MONFEE_UNITFEE_M失败! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
  		}
  }
  
  if (m_pRateInfo->m_paramMgr->getSysParam("MONFEE_UNITFEE_T",0,l_sysParam))
  {
    if(l_sysParam.m_valueN != 0)
  	  m_unitfee_T =l_sysParam.m_valueN;
  	 else
  		{
  		#ifdef _DEBUG_
        cout<<"获取系统参数：MONFEE_UNITFEE_T失败! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
  		}
  }
  
  if (m_pRateInfo->m_paramMgr->getSysParam("MONFEE_IPHONE_PRODUCT",0,l_sysParam))
  {
  	if(l_sysParam.m_valueS != "")
  		{
      	m_iphoneVec = StringUtil::split(l_sysParam.m_valueS,";",true);
       ::sort(m_iphoneVec.begin(),m_iphoneVec.end()); 
      }
    else
  		{
  		#ifdef _DEBUG_
        cout<<"获取系统参数：MONFEE_IPHONE_PRODUCT失败! user_id:"<<m_userId<<__FILE__<<__LINE__<<endl;
      #endif
  		}
  }	
  
	m_userId = 	m_pcdr[ATTRI_USER_ID];
	m_pRateInfo->m_userId=m_userId;	
	m_userSubInfo.m_userId=m_userId;
	
	string t_startDate,t_startTime,t_endDate,t_endTime;

	m_userSubInfo.m_spBizCode     =  m_pcdr[ATTRI_SP_BIZ_CODE];  //SP业务代码                     
  m_userSubInfo.m_spCode        =  m_pcdr[ATTRI_SP_CODE_1];      //SP(SI)编码                     
  m_userSubInfo.m_spServCode    =  m_pcdr[ATTRI_SP_SERV_CODE];  //SP服务代码                    
  m_userSubInfo.m_sourceType    =  m_pcdr[ATTRI_SOURCE_TYPE_SP];    //定购渠道                     
  t_startDate                   =  m_pcdr[ATTRI_BEGIN_DATE];
  t_startTime                   =  m_pcdr[ATTRI_BEGIN_TIME];                                    
  t_endDate                     =  m_pcdr[ATTRI_END_DATE_1];   
  t_endTime                     =  m_pcdr[ATTRI_END_TIME_1];                                       
  m_userSubInfo.m_bizStateCode  =  m_pcdr[ATTRI_BIZ_STATE_CODE][0];                                
  m_userSubInfo.m_billFlag      =  m_pcdr[ATTRI_BILL_FLAG_SP][0];                                     
  m_userSubInfo.m_ecSerialNumber=  m_pcdr[ATTRI_ECSERIAL_NUMBER];//集团客户标识                 
  m_userSubInfo.m_ecUserId      =  m_pcdr[ATTRI_ECUSER_ID];			//集团用户标识                  
  m_userSubInfo.m_bizTypeCode   =  m_pcdr[ATTRI_BIZ_TYPE_CODE];//业务类型代码  
  
  //m_pcdr[ATTRI_ERROR_CODE] = E_ACCT_DEFAULT;
 // m_pcdr[ATTRI_NEED_OUTPUTCDR] = FLAG_NO;
  
  t_startDate += t_startTime;
  t_endDate += t_endTime;
  
  	m_userSubInfo.m_startDate = t_startDate;
	  m_userSubInfo.m_endDate   = t_endDate;
                   
  if(m_fileType == FILE_TYPE_IN_M)
  	{
  		  m_userSubInfo.m_priorTime     =  m_pcdr[ATTRI_PRIOR_TIME];     //历史首次定购时间，对M文件有效
        m_userSubInfo.m_lastTime      =  m_pcdr[ATTRI_LAST_TIME];      //最后一次定购时间，对M文件有效
  	}
  //求取订购关系中扩展字段信息
   strcpy(m_preTemp,m_pcdr[ATTRI_PRE_VALUE].c_str());//获取保留字段信息
  //解析保留字段
  m_seperate="|";//定义分隔符
 //int subDateSign;//订购时间,1:往月订购;2:本月上半月订购;3:本月下半月订购 
 //int subStopSign;//终止时间,1:当月上半月终止;2:当月下半月终止;3:非当月终止
 //int subUseDayNum;//当月正常使用天数
 //int isRepeat;//是否重复订购,1:当月重复订购
 //string<14> cancelTime;//当月订购后的第一次终止的开始时间（暂停也算）（状态‘E’，‘P’）
 //int isAllPause;//是否全月暂停，1：当月全月暂停
 //string<14> startTime;//当月第一次订购开始时间（状态‘A’，‘N’）
 //string<14> endTime;//当月第一次订购结束时间（状态‘A’，‘N’）
 //string<14> laststartTime;//当月最后一次订购开始时间（状态‘A’，‘N’）
 //string<14> lastendTime;//当月最后一次订购结束时间（状态‘A’，‘N’）
 
   UserSubInfoExtra i_UserSubInfoExtra;
   
   bool isValidRecord = false;
   
   char *tempchr=m_preTemp;
   //            
   tempchr = strtok(tempchr, m_seperate.c_str());
   if (tempchr != NULL)
   {
       i_UserSubInfoExtra.m_isValid = atoi(tempchr);
       isValidRecord = true;
   }
   
   //订购时间
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_subDateSign = atoi(tempchr);
            isValidRecord = true;
        }
    }
    
    
   //终止时间
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_subStopSign = atoi(tempchr);
            isValidRecord = true;
        }
    }
    
    //当月正常使用天数
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_subUseDayNum = atoi(tempchr);
            isValidRecord = true;
        }
    }
    
     //是否重复订购
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_isRepeat = atoi(tempchr);
            isValidRecord = true;
        }
    }
    
    //当月订购后的第一次终止的开始时间（暂停也算）（状态‘E’，‘P’）
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_cancelTime = tempchr;
            isValidRecord = true;
        }
    }
    
    //是否全月暂停
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_isAllPause = atoi(tempchr);
            isValidRecord = true;
        }
    }   
    
    //当月第一次订购开始时间（状态‘A’，‘N’）
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_startTime = tempchr;
            isValidRecord = true;
        }
    } 
    
    //当月第一次订购结束时间（状态‘A’，‘N’）
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_endTime = tempchr;
            isValidRecord = true;
        }
    }        
    
    //当月最后一次订购结束时间（状态‘A’，‘N’）
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_laststartTime = tempchr;
            isValidRecord = true;
        }
    }
    
    //当月最后一次订购开始时间（状态‘A’，‘N’）
    if (isValidRecord)
    {
        isValidRecord = false;
        
        tempchr = strtok(NULL, m_seperate.c_str());
        if (tempchr != NULL)
        {
            i_UserSubInfoExtra.m_lastendTime = tempchr;
            isValidRecord = true;
        }
    }
    
    m_spRelaDiscntMap.clear();
    //初始化用户                      
		if(!(m_pRateInfo->m_comBill).initUser(m_pRateInfo->m_billUserId,m_pRateInfo->m_cycParam))
	  {
	  	(m_pRateInfo->m_comBill).setErrorCode(m_userId,E_ACCT_INITIALIZE,"0");
			DEBUG_PRINT("[%s:%d] Initialize comBill error!\n",__FILE__,__LINE__);
			return false;
		}
		
    //获取用户信息
    if(!(m_pRateInfo->m_comBill).getUserInfo(m_userId,m_idInfo))
    	{
    		(m_pRateInfo->m_comBill).setErrorCode(m_userId,E_ACCT_GET_USERINFO,"0");
    		RegisterErrorUser(m_pRateInfo->m_userId, E_ACCT_GET_USERINFO);
    		DEBUG_PRINT("[%s:%d] Get user info error! userId=%s \n",__FILE__,__LINE__,m_userId.c_str());
    	  return false;
    	}
    m_pRateInfo->m_idInfo=m_idInfo;
     

    	  
      #ifdef _DEBUG_
    	  cout<<m_pRateInfo->m_idInfo<<endl;
    	  cout<<"-----------------------------------------------------"<<endl;
    	  cout<<m_idInfo<<endl;
      #endif
    int acctTag=0;
    
    if((m_pRateInfo->m_comBill).m_ifExistUserInfo)
    	{
    		if ( m_idInfo.m_idAttr[IdInfo::ID_ACCOUNT_TAG] == "2" || 
    			 (T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE]) > (m_pRateInfo->m_cycParam).m_endTime))
    	       	acctTag = 2;
    	  if (m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE] == "15" || m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE] == "14")
    		      acctTag = 2;
    	}
    else
    	{
    		return false;
    	}
    		
     	
     	if(acctTag == 2)//预开用户、智能网用户不处理
     		{
     			return false;
     		}
     		
    m_isiphone=false;
    strstream ss;
    string t_prod;
    ss << m_idInfo.m_idAttr[IdInfo::ID_PRODUCT_ID] ;
    ss >> t_prod;		
    m_isiphone = binary_search(m_iphoneVec.begin(), m_iphoneVec.end(),t_prod);
     		
    //获取用户状态
    if((*(m_pRateInfo->m_paramMgr)).getUserServState(m_userId,m_pRateInfo->m_vecUserServState)<=0)
    {
    	(m_pRateInfo->m_comBill).setErrorCode(m_userId,E_ACCT_GET_USERSERV,"0");
    	RegisterErrorUser(m_pRateInfo->m_userId, E_ACCT_GET_USERSERV);
    	return false;
    }
    //初始化recvMonfee
    if(!m_recvmonfee.Initialize(i_UserSubInfoExtra,&m_userSubInfo,
                                 m_fileType,m_idInfo,
                                 m_pRateInfo,m_isFileE))
    	return false;
    
    vector<sysTariff> sysDiscountVector;
    //获取包月费订购系统优惠
    (*(m_pRateInfo->m_paramMgr)).getBaseTp("7",(m_pRateInfo->m_cycParam).m_beginTime,
								                               (m_pRateInfo->m_cycParam).m_endTime,
								                                 sysDiscountVector); 
	 //获取用户包月费订购优惠
	 (m_pRateInfo->m_comBill).getUserSpDiscnt(m_fileType,m_idInfo,
	                                           USER_ID_TYPE,
	                                           m_userSubInfo.m_spCode,
	                                           m_userSubInfo.m_spBizCode,
	                                           sysDiscountVector,
	                                           m_monfeeDiscountVector);
						
    //获取关联优惠
    if(!(m_pRateInfo->m_comBill).getAllRelaDiscount(m_spRelaDiscntMap))	
    	  DEBUG_PRINT("[%s:%d] 获取包月费关联优惠失败! userId=%s \n",__FILE__,__LINE__,m_userId.c_str());

    //初始化DiscntMonfee
    if(!m_discntmonfee.Initialize(&m_userSubInfo,&m_monfeeDiscountVector,
    	                             m_fileType,m_idInfo,
    	                             m_pRateInfo,&m_spRelaDiscntMap))
    	return false;   
                      
    return true;                    
}

bool Monfeeproc::recvMonfee()
{
	return m_recvmonfee.recvMonfee();
}

bool Monfeeproc::discntMonfee()
{
	return m_discntmonfee.discntMonfee();
}




bool Monfeeproc::getAllNewBill()
{
  //直接更新账单，在配置中使用的就是明细账目，需要在程序中添加明细账目判断机制，保证算费的正常！
  UserMonfeeBill i_monFeeBill;
  OutBill        i_outBill;
  
  i_monFeeBill.m_userId       = m_userId;
  i_monFeeBill.m_dateType[0]  = m_fileType;
  i_monFeeBill.m_dateType[1]  = '\0';
  
  i_monFeeBill.m_serialNumber = m_idInfo.m_idAttr[IdInfo::ID_MSISDN];
  i_monFeeBill.m_spBizCode    = m_userSubInfo.m_spBizCode;            
  i_monFeeBill.m_spCode       = m_userSubInfo.m_spCode; 
  i_monFeeBill.m_itemCode     = 0;
  i_monFeeBill.m_fee          = 0;//原始费用
  i_monFeeBill.m_discntFee    = 0;//优惠费用           
  i_monFeeBill.m_displayType  = 0;
  i_monFeeBill.m_feeType[0]   = 'Y'; 
  i_monFeeBill.m_feeType[1]   = '\0';   
  
  i_outBill.m_userId           = m_userId;
  i_outBill.m_serialNumber     = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
  i_outBill.m_ecSerialNumber   = m_userSubInfo.m_ecSerialNumber;
  i_outBill.m_spServCode       = m_userSubInfo.m_spServCode;
  i_outBill.m_spBizCode        = m_userSubInfo.m_spBizCode;
  i_outBill.m_bizTypeCode      = m_userSubInfo.m_bizTypeCode;
  i_outBill.m_spCode           = m_userSubInfo.m_spCode;   
  i_outBill.m_itemCode         = 0;
  i_outBill.m_fee              = 0;//原始费用
  i_outBill.m_discntFee        = 0;//优惠费用 
  i_outBill.m_validTag         = INIT_VALID_Tag_2;
  i_outBill.m_sourceType       = m_userSubInfo.m_sourceType;
  i_outBill.m_displayType      = '0';
  i_outBill.m_feeType          = INIT_FEETYPE;       
        
  int fee = 0;
  int discntFee = 0;
	int tmp_itemcodeO = 0;
	int tmp_itemcodeN = 0;
	char tmp_feetype;
	::partial_sort (m_userSubInfo.m_userDetailBill.begin(),
                  m_userSubInfo.m_userDetailBill.end(),
                  m_userSubInfo.m_userDetailBill.end());
  
  	#ifdef _DEBUG_	
	for(vector<UserDetailBillMonfee>::iterator itr_u = m_userSubInfo.m_userDetailBill.begin();
		  itr_u !=  m_userSubInfo.m_userDetailBill.end(); itr_u++)
	 {
	 	  cout<<"m_itemCode   "<<itr_u->m_itemCode   <<endl;
      cout<<"m_fee        "<<itr_u->m_fee        <<endl;        
      cout<<"m_disPlayFee "<<itr_u->m_disPlayFee <<endl;
      cout<<"m_discntFee  "<<itr_u->m_discntFee  <<endl;
      cout<<"m_discntCode "<<itr_u->m_discntCode <<endl;
      cout<<"m_discntType "<<itr_u->m_discntType <<endl; 
      cout<<"m_displayType"<<itr_u->m_displayType<<endl;
      cout<<"m_feeType    "<<itr_u->m_feeType    <<endl;
	 }
	#endif
	
  char t_feetype;//记录最后一条记录的feetype
  
  vector<UserDetailBillMonfee>::iterator itr_u = m_userSubInfo.m_userDetailBill.begin();               
	tmp_itemcodeO= itr_u->m_itemCode;
	t_feetype = itr_u->m_feeType;
	
	for(; itr_u !=  m_userSubInfo.m_userDetailBill.end(); itr_u++)
	 {
	 	  if(itr_u->m_fee == 0 && itr_u->m_discntFee ==0)
	 	  	{
	 	  		continue;
	 	  	}
	 	  	
	 	     fee       += itr_u->m_fee;
	 	     discntFee += itr_u->m_discntFee;
	 	  
	 	  tmp_itemcodeN = itr_u->m_itemCode;
	 	  tmp_feetype = itr_u->m_feeType;
	 	  
	 	  if((tmp_itemcodeN == tmp_itemcodeO) && (tmp_feetype==t_feetype))
	 	  	{        
          i_monFeeBill.m_itemCode     = itr_u->m_itemCode;
          i_monFeeBill.m_fee         += itr_u->m_fee;//原始费用
          i_monFeeBill.m_discntFee   += itr_u->m_discntFee;//优惠费用           
          i_monFeeBill.m_displayType  = itr_u->m_displayType-'0';
          i_monFeeBill.m_feeType[0]      = itr_u->m_feeType;
            
          i_outBill.m_itemCode         = itr_u->m_itemCode;
          i_outBill.m_fee             += itr_u->m_fee;//原始费用
          i_outBill.m_discntFee       += itr_u->m_discntFee;//优惠费用 
          i_outBill.m_displayType      = itr_u->m_displayType;
          i_outBill.m_feeType          = itr_u->m_feeType;
          
          t_feetype= itr_u->m_feeType;    
        }
       else
       	{ 
       		m_monFeeBillNew.push_back(i_monFeeBill);
          m_outBillVector.push_back(i_outBill); 
          
       		i_monFeeBill.m_fee = 0;
          i_monFeeBill.m_discntFee = 0;
                
          i_outBill.m_fee = 0;
				  i_outBill.m_discntFee = 0;
				  
          i_monFeeBill.m_itemCode     = itr_u->m_itemCode;
          i_monFeeBill.m_fee         += itr_u->m_fee;//原始费用
          i_monFeeBill.m_discntFee   += itr_u->m_discntFee;//优惠费用(fee+discntFee才为优惠后的费用)          
          i_monFeeBill.m_displayType  = itr_u->m_displayType-'0';
          i_monFeeBill.m_feeType[0]      = itr_u->m_feeType;
            
          i_outBill.m_itemCode         = itr_u->m_itemCode;
          i_outBill.m_fee             += itr_u->m_fee;//原始费用
          i_outBill.m_discntFee       += itr_u->m_discntFee;//优惠费用 
          i_outBill.m_displayType      = itr_u->m_displayType;
          i_outBill.m_feeType          = itr_u->m_feeType;
       	}
      tmp_itemcodeO = tmp_itemcodeN;
      t_feetype =  itr_u->m_feeType;  
	 }
	 //将最后一个压入
	 m_monFeeBillNew.push_back(i_monFeeBill);
   m_outBillVector.push_back(i_outBill);
	 
	 CdrBill cdrBill;
	 
	 cdrBill.m_fee = fee;                               
   cdrBill.m_discntFee = fee + discntFee;
   
   cdrBill.m_validTag = INIT_VALID_Tag_2;                        
   cdrBill.m_userId         = m_userId;
   cdrBill.m_serialNumber   = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
   cdrBill.m_ecSerialNumber = m_userSubInfo.m_ecSerialNumber;
   cdrBill.m_sourceType     = m_userSubInfo.m_sourceType;
   cdrBill.m_spCode         = m_userSubInfo.m_spCode;
   cdrBill.m_spBizCode      = m_userSubInfo.m_spBizCode;
   cdrBill.m_spServCode     = m_userSubInfo.m_spServCode;
   cdrBill.m_bizTypeCode    = m_userSubInfo.m_bizTypeCode;
   m_cdrBillVectorNew.push_back(cdrBill);
   return true;
}

bool Monfeeproc::getOldCdrBill()
{
	::partial_sort (m_monFeeBillOld.begin(),m_monFeeBillOld.end(), m_monFeeBillOld.end());                       
  vector<UserMonfeeBill>::iterator iter_u;
  vector<CdrBill>::iterator iter_c;
      
  for (iter_u=m_monFeeBillOld.begin();iter_u!=m_monFeeBillOld.end(); ++iter_u)
  {                        
    for (iter_c=m_cdrBillVectorOld.begin();iter_c!=m_cdrBillVectorOld.end(); ++iter_c)
    {         
        if(((*iter_c).m_spCode == (*iter_u).m_spCode)&&
          ((*iter_c).m_spBizCode == (*iter_u).m_spBizCode))
        {                
          (*iter_c).m_fee += (*iter_u).m_fee;
          (*iter_c).m_discntFee += (*iter_u).m_fee + (*iter_u).m_discntFee;
          break;            
        }
    }
    if(iter_c == m_cdrBillVectorOld.end())
    {            
        CdrBill cdrBill;
        cdrBill.m_userId =m_userId;        
        cdrBill.m_fee = iter_u->m_fee;                                 
        cdrBill.m_discntFee = iter_u->m_fee + iter_u->m_discntFee;        
        cdrBill.m_validTag = INIT_VALID_Tag_2;                                                                  
        cdrBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);            
        cdrBill.m_spCode = iter_u->m_spCode;
        cdrBill.m_spBizCode = iter_u->m_spBizCode;            
        m_cdrBillVectorOld.push_back(cdrBill);            
    }            
  }	
  
  return true;
}

bool Monfeeproc::getOutBillVector()
{
	//生成待输出的CdrBillVector
	(m_pRateInfo->outputCdrVector).clear();
  vector<CdrBill>::iterator iter;
  
  for(iter=m_cdrBillVectorOld.begin();iter!=m_cdrBillVectorOld.end();++iter)
  {            
      vector<CdrBill>::iterator iterC = ::find(m_cdrBillVectorNew.begin(),m_cdrBillVectorNew.end(),(*iter) );        
      if(iterC != m_cdrBillVectorNew.end())
      {  
          if( ((*iterC).m_fee == (*iter).m_fee) && 
              ((*iterC).m_discntFee == (*iter).m_discntFee) )
          {
              (*iterC).m_validTag = INIT_VALID_Tag_3;
              continue;
          }
          else
          {
              (*iter).m_validTag = '1';//重做标志
              (*iter).m_fee = - ((*iter).m_fee);
              (*iter).m_discntFee = - ((*iter).m_discntFee);
              (*iter).m_bizTypeCode = (*iterC).m_bizTypeCode;
              (m_pRateInfo->outputCdrVector).push_back(*iter);
              
              (*iterC).m_validTag = '0';//普通记录
              (m_pRateInfo->outputCdrVector).push_back(*iterC); 
              (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
          }
      }
      else
      {
          (*iter).m_validTag = '1';//重做标志
          (*iter).m_fee = - ((*iter).m_fee);
          (*iter).m_discntFee = - ((*iter).m_discntFee);
          (m_pRateInfo->outputCdrVector).push_back(*iter);
      }
  }
  
  for(iter= m_cdrBillVectorNew.begin();iter!=m_cdrBillVectorNew.end(); ++iter)
  {           
      if((*iter).m_validTag == INIT_VALID_Tag_2)                
      {
          if((*iter).m_fee == 0 && (*iter).m_discntFee == 0)
          {
              continue;
          }
        (m_pRateInfo->outputCdrVector).push_back(*iter);  
      }
  }
  
  //生成待输出的OutBill
    OutBill		outputBill;
    vector<UserMonfeeBill>::iterator iterU;
    vector<OutBill>::iterator iterC;
    
    for(iterU=m_monFeeBillOld.begin();iterU!=m_monFeeBillOld.end();++iterU)
    {
    	  outputBill.m_userId       = m_userId;            
        outputBill.m_spCode       = (*iterU).m_spCode;
        outputBill.m_spBizCode    = (*iterU).m_spBizCode;
        outputBill.m_itemCode     = (*iterU).m_itemCode;
        outputBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
       	outputBill.m_displayType  = (*iterU).m_displayType+'0';
       	outputBill.m_feeType      = (*iterU).m_feeType[0];
        
        iterC = ::find(m_outBillVector.begin(),m_outBillVector.end(),outputBill ); 
               
        if(iterC != m_outBillVector.end())
        {  
            if( ((*iterC).m_fee       == (*iterU).m_fee)       && 
                ((*iterC).m_discntFee == (*iterU).m_discntFee) &&
                ((*iterC).m_feeType   == (*iterU).m_feeType[0]))
            {
                (*iterC).m_validTag = INIT_VALID_Tag_3;
                continue;
            }
            else
            {
            	if ((*iterC).m_feeType == (*iterU).m_feeType[0])
            	{
            		(*iterC).m_fee = (*iterC).m_fee - (*iterU).m_fee;
	              (*iterC).m_discntFee = (*iterC).m_discntFee - (*iterU).m_discntFee;
	              	              
                (m_pRateInfo->outputBillVector).push_back(*iterC);  
                              
                (*iterC).m_validTag = INIT_VALID_Tag_3;
               }
               else
               {
	            		(*iterC).m_fee = - ((*iterU).m_fee);
		              (*iterC).m_discntFee = - ((*iterU).m_discntFee);
		              (*iterC).m_feeType = (*iterU).m_feeType[0];
		              
	                (m_pRateInfo->outputBillVector).push_back(*iterC);
	                
	                (*iterC).m_validTag = INIT_VALID_Tag_3;
               }
            }
        }
        else
        {
        	  outputBill.m_userId       = m_userSubInfo.m_userId;  
            outputBill.m_serialNumber = (*iterU).m_serialNumber;
            outputBill.m_spCode       = (*iterU).m_spCode;
            outputBill.m_spBizCode    = (*iterU).m_spBizCode;
            outputBill.m_itemCode     = (*iterU).m_itemCode;
            outputBill.m_fee          = - ((*iterU).m_fee);
            outputBill.m_discntFee    = - ((*iterU).m_discntFee);
            outputBill.m_displayType  = (*iterU).m_displayType+'0';
            outputBill.m_feeType      = (*iterU).m_feeType[0];
            
            (m_pRateInfo->outputBillVector).push_back(outputBill);
        }
    }
    
    for(iterC= m_outBillVector.begin();iterC!=m_outBillVector.end(); ++iterC)
    {           
        if((*iterC).m_validTag == INIT_VALID_Tag_2)
        	
        {
            if((*iterC).m_fee == 0 && (*iterC).m_discntFee == 0)
            {
                continue;
            }
            (m_pRateInfo->outputBillVector).push_back(*iterC);
         }
     }
     
     iterC = (m_pRateInfo->outputBillVector).begin();
    for(;iterC != (m_pRateInfo->outputBillVector).end() ; iterC++)
    {
	     int unitfee_MT=1;
       if (m_isiphone &&  (*iterC).m_feeType != INIT_FEETYPE)
       {
       	if ((*iterC).m_feeType == 'M')
       	{
       		unitfee_MT = m_unitfee_M;
       	}
       	else
       	{
       		unitfee_MT = m_unitfee_T;
       	}	
       	
       (*iterC).m_fee *= unitfee_MT;
      (*iterC).m_discntFee *=  unitfee_MT;
      (*iterC).m_feeType = INIT_FEETYPE;
       }
       m_pRateInfo->updateBillIncre(m_outBillVector_inc,*iterC);
       //m_outBillVector_inc.push_back(*iterC); 
    }
     
  //生成待输出的UserDetailBillMonfee

   	m_pRateInfo->outputUserSubInfoBill = m_userSubInfo;  	
  
  return true;
}


void Monfeeproc::RegisterErrorUser(const T_USER_ID &r_id,const int r_errCode)
{
  AdditionalBill   l_idErrorInfo;
  l_idErrorInfo.m_billType    = "3";     
  l_idErrorInfo.m_id          = r_id;           //用户ID
  l_idErrorInfo.m_idType      = USER_ID_TYPE;   // FieldsXX.h中枚举enum ID_TYPE类型，表示类型为"用户",区别于账户、客户
  l_idErrorInfo.m_operateType = '0'; 
  l_idErrorInfo.m_itemCode    = r_errCode;      //错误编码
  l_idErrorInfo.m_fValue      = 0;              //保留
  l_idErrorInfo.m_sValue      = 0;              //保留

  m_pRateInfo->m_addBillVector.push_back(l_idErrorInfo);   
}