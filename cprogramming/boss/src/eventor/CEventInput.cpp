#include "CEventInput.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include <unistd.h>

extern ErrorMessages g_errormessages;
extern ErrorMessages *const theErrorMessages=&g_errormessages;
extern log4cpp::Logger *theLogger;


CEventInput::CEventInput()
{

}    


CEventInput::~CEventInput()
{
    
} 


void CEventInput::initialize(const EventConfig *eventorConfig, PM_Billing *pEventParamMgr)
{
    m_userName       = eventorConfig->m_userName;    
    m_passWord       = eventorConfig->m_passWord;
    m_servName       = eventorConfig->m_servName;
    m_eventorType    = eventorConfig->m_eventorType;
    m_eparchyCode    = eventorConfig->m_eparchyCode;
    m_netTypeCodes   = eventorConfig->m_netTypeCodes; //add by xuf 2009-11-26 20:20:25
    m_fixNetTypeCodes =eventorConfig->m_fixNetTypeCodes;  //Added by jlxu@neimeng 2009-12-16 16:45:58
    m_pEventParamMgr = pEventParamMgr;
    
    //add 2009-1-30 21:45:03
    BACTCHNUM 			= eventorConfig->m_autoOutputFileUserNum*10;
    /*
    int i = 0;
    m_pEventParamMgr->getMinRealCycId(i);
    m_defaultCycId = i;
    CycParam r_cycleParam;
    m_pEventParamMgr->getCycleTime(m_defaultCycId, r_cycleParam);
    m_monthEndTag = '0';
    if(r_cycleParam.m_addTag>=1)
    {
        //cout << "m_monthEndTag = '1'" << endl;
        m_monthEndTag = '1';
    }
    */
//#ifdef __PROV019__  //如果是山西    
    //add by xuf 2010-1-28 16:55:44
    if(m_eventorType=="6"||
    	m_eventorType=="3")//增量绑定资费
    {
    	m_fullSign = 1;
    }
    else //全量绑定资费
    	m_fullSign = 0;
    	
    //线程数
     m_connectNum = 10; 	
     
     for(int i=0;i<m_connectNum;i++)
       m_bConn[i] = false;
//#endif    	    
}


bool CEventInput::prePareOtpUserDataFromDb()
{
    char         procedureName[500];
    PROCPARAM    procedureParam[3];    
    
    //limeng 20090821
    strcpy(procedureName,"p_asp_otp_refreshinfo");
//    strcpy(procedureName,"p_asp_otp_lmtest");

    procedureParam[0].action = PARAM_ACT_IN;
    procedureParam[0].type = PARAM_TYPE_STR;
    procedureParam[1].action = PARAM_ACT_IN;
    procedureParam[1].type = PARAM_TYPE_INT;
    procedureParam[1].intValue = 0;
    procedureParam[2].action = PARAM_ACT_OUT;
    procedureParam[2].type = PARAM_TYPE_STR;
    strcpy(procedureParam[2].strValue,"");
    strcpy(procedureParam[0].strValue,m_eparchyCode.c_str());
    
    vector<string> resultVector;
    vector<string> errorVector;
    
    resultVector.clear();
    errorVector.clear();
#ifdef _DEBUG_
    cout << "call prePareUserDataFromDb("
         << procedureParam[0].strValue << "," << procedureParam[1].intValue <<"," << procedureParam[2].strValue << ")" << endl;
#endif
    
    m_dbinterface.dataBind(resultVector,errorVector);

    if ( m_dbinterface.callProcedure(procedureName,
                                     sizeof(procedureParam)/sizeof(PROCPARAM),
                                     procedureParam)
         || procedureParam[1].intValue == -1)
    {
        string errorString;
        errorString = "Call Procedure Error!procedureName:"
                      + string(procedureName) + ":errorReason:";

        // 避免汉字显示错误
        if (errorString.length()%2 == 0) {
            errorString += " ";
        }
        errorString += string(procedureParam[2].strValue);
        errorString.erase(errorString.length()-1);
        theErrorMessages->insert(-1,errorString);
        #ifdef _DEBUG_
        cout << "Call Procedure Error!" << endl;
        #endif

        m_dbinterface.disconnect();
        memset(procedureName,0,sizeof(procedureName));

        return false;
    }

    memset(procedureName,0,sizeof(procedureName));

    return true;

}

//一次性费用计算生成
bool CEventInput::getEventInfo(vector<OtpEventInfo>& otpEventInfoVector)
{
    otpEventInfoVector.clear();
    
    //连接数据库
    if(m_dbinterface.connect(m_userName.c_str(),m_passWord.c_str(),
                             m_servName.c_str()))
    {
          theErrorMessages->insert(ERROR_DB_CONNECT, "Connect DB Error !"+m_servName);
          #ifdef _DEBUG_
          cout << "Connect DB Error !" << endl;
          #endif
          return false;
    }
    
    if(!prePareOtpUserDataFromDb())
    {
    	#ifdef _DEBUG_
        cout << "prePareOtpUserDataFromDb() false" << endl;   
      #endif    
        m_dbinterface.disconnect(); 
        return false;
    }
    #ifdef _DEBUG_
    cout << "prePareOtpUserDataFromDb() successfully called" << endl;  
    #endif
    char tempchr[1000+1] = "\0";
    StringVector                recordVector;
    StringVector                errorVector;
    sprintf(tempchr,"SELECT USER_ID,PRE_VALUE_N1,PRE_VALUE_N2 FROM TM_A_SMSDISCOUNT");
//    sprintf(tempchr,"SELECT USER_ID,PRE_VALUE_N1,PRE_VALUE_N2 FROM tm_sms_test");
        
    m_selectInfoSql = tempchr;
        
    //绑定数据    
    recordVector.clear();
    errorVector.clear();
    if(m_dbinterface.dataBind(recordVector,errorVector))
    {
        return false;
    }
    
    //执行SQL语句
    int                         total = 0;
    int                         success = 0;
    int                         error = 0;
    int                         recordCount = 0;
    int                         fields = 3; 
    vector<string>::iterator    iter;              
    do
    {
        if(m_dbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
        {
            return false;
        }
    
        for (iter=recordVector.begin(); iter!=recordVector.end(); iter+=fields)
        {
            string      tempstr = "";
            OtpEventInfo    otpEventInfo;
        
            otpEventInfo.m_userId = *iter;
            tempstr = *(iter+1);
            otpEventInfo.m_itemCode = atoi(tempstr.c_str());
            tempstr = *(iter+2);
            otpEventInfo.m_fee = atoi(tempstr.c_str());
            
            #ifdef _DEBUG_
            //cout << "otpEventInfo.m_userId=" << otpEventInfo.m_userId << endl;
            //cout << "otpEventInfo.m_itemCode=" << otpEventInfo.m_itemCode << endl;
            //cout << "otpEventInfo.m_fee=" << otpEventInfo.m_fee << endl;
            #endif
            
            otpEventInfoVector.push_back(otpEventInfo);
        }
    }
    while(total >= DEFAULTARRAYLINE);
    
    m_dbinterface.disconnect();
    
    return true;
}


//用户重出帐事件生成
bool CEventInput::getEventInfo(vector<ReAccountEventInfo>& reAccountEventVector)
{
    reAccountEventVector.clear();
    
    char tempchr[1000+1] = "\0";
//    StringVector                recordVector;
    recordVector.clear();
    StringVector                errorVector;
    
    #ifdef _DEBUG_
      //连接数据库
      cout << "m_userName=" << m_userName << endl
           << "m_passWord=" << m_passWord << endl
           << "m_servName=" << m_servName << endl;
    #endif
    theLogger->info("m_userName=%s,m_passWord=%s,m_servName=%s\n",
    	m_userName.c_str(),m_passWord.c_str(),m_servName.c_str());
    if(m_dbinterface.connect(m_userName.c_str(),m_passWord.c_str(),
                             m_servName.c_str()))
    {
        return false;
    }
    if (m_eventorType == "0")
    {
        //帐单、内存查询语句赋值        
        
        //sprintf(tempchr,"SELECT USER_ID,CYCLE_ID,MONTH_END "
         //               "FROM TI_B_REACCOUNTUSER ORDER BY CYCLE_ID, MONTH_END ");
//         sprintf(tempchr,"SELECT a.USER_ID,a.CYCLE_ID,a.MONTH_END "
//                        "FROM TI_B_REACCOUNTUSER a, tf_f_user b where a.user_id = b.user_id and "
//						"b.DUMMY_TAG = '0' ORDER BY a.CYCLE_ID, a.MONTH_END ");

         sprintf(tempchr,"SELECT a.USER_ID,a.CYCLE_ID,a.MONTH_END "
                        "FROM TI_B_REACCOUNTUSER a, tf_f_user b where a.user_id = b.user_id  "
						            " ORDER BY a.CYCLE_ID, a.MONTH_END ");
						
        m_selectInfoSql = tempchr;
        
        //绑定数据    
        recordVector.clear();
        errorVector.clear();
        if(m_dbinterface.dataBind(recordVector,errorVector))
        {
            return false;
        }
        
        //执行SQL语句
        int                         total = 0;
        int                         success = 0;
        int                         error = 0;
        int                         recordCount = 0;
        int                         fields = 3; 
        vector<string>::iterator    iter;              
        do
        {
            if(m_dbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
            {
                return false;
            }
        
//#ifdef _DEBUG_       
//cout<<"sql=["<<m_selectInfoSql<<"],total="<<total<<",success="<<success<<",error="<<error<<endl;
//#endif
            for (iter=recordVector.begin(); iter!=recordVector.end(); iter+=fields)
            {
                string      tempstr = "";
                ReAccountEventInfo    reAccountEventInfo;
            
                reAccountEventInfo.m_userId = *iter;
                tempstr = *(iter+1);
                reAccountEventInfo.m_cycId = atoi(tempstr.c_str());
                tempstr = *(iter+2);
                //cout <<   "if(tempstr[0]!='\0')" << endl;
                if(tempstr[0]!='\0')
                {
                    reAccountEventInfo.m_monthEndTag = tempstr[0];
                }
                else
                {
                    reAccountEventInfo.m_monthEndTag = m_monthEndTag;
                }
                reAccountEventVector.push_back(reAccountEventInfo);
                if((reAccountEventVector.size()%50000)==0)
                {
                	#ifdef _DEBUG_
                    cout << "reAccountEventVector.size()=" << reAccountEventVector.size() << endl;
                  #endif
                }
            }
        }
        while(total >= DEFAULTARRAYLINE);
        
    }
    else if (m_eventorType == "1" || m_eventorType == "3")//limeng 20090813 从主表查询，实时与全量数据相同
    {
        //vector<IdInfo> vecIdInfo;
#ifdef _FROMMDB_  //从内存数据库  add 2009-1-30 21:27:33    
        //帐单、内存查询语句赋值        
        if(m_pEventParamMgr->getAllUserReAccount(reAccountEventVector)==-1)
        {
        	#ifdef _DEBUG_
            cout << "m_pEventParamMgr->getAllUserReAccount false" << endl;
          #endif
            return false;
        }
        
        #ifdef _DEBUG_
        cout << "m_pEventParamMgr->getAllUserReAccount true" << endl;
        #endif
        
#else  //从磁盘数据库      
    		CycParam t_cycleParam;
//				m_currCycId = 0;
//    		m_pEventParamMgr->getMinRealCycId(m_currCycId);
    		string t_strSql="";
    		
    		m_monthEndTag = '0';
    		m_pEventParamMgr->getCycleTime(m_currCycId, t_cycleParam);
    		if(t_cycleParam.m_addTag>=1)
    		{
        	m_monthEndTag = '1';
    		}
    		

//#ifdef __PROV010__     //如果是内蒙          
////modify 2009-3-11 23:08:31
////过滤用户（如智能网，往月销户，往月停机用户）
////limeng 20090820 区分地市
//
//			if(m_eparchyCode != "0")
//      {
//       sprintf(tempchr,	"Select  USER_ID,SERIAL_NUMBER,EPARCHY_CODE,OPEN_DATE  From tf_f_user a "
//          "Where a.DUMMY_TAG = '0' and a.net_type_code Not In %s And a.acct_tag='0' "
//          "And (a.remove_tag='0' Or (a.remove_tag!='0' And  nvl(DESTROY_DATE,nvl(PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
//		  "AND (a.net_type_code In %s or  ( a.net_type_code Not In %s "
//          "And(a.service_state_code='0'  or  ( a.service_state_code !='0' 	and nvl(last_stop_date,to_date('20500101000000','YYYYMMDDHH24MISS')) >=to_date('%s','YYYYMMDDHH24MISS'))))) "
//		  "AND a.open_date <=to_date('%s','YYYYMMDDHH24MISS') and a.EPARCHY_CODE='%s'",
//         m_netTypeCodes.c_str(),
//         t_cycleParam.m_beginTime.c_str(),
//         m_fixNetTypeCodes.c_str(),m_fixNetTypeCodes.c_str(),
//         t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str(),m_eparchyCode.c_str());
//	    }
//	    else
//	    {
// 	      sprintf(tempchr,	"Select  USER_ID,SERIAL_NUMBER,EPARCHY_CODE,OPEN_DATE  From tf_f_user a "
//          "Where a.DUMMY_TAG = '0' and a.net_type_code Not In %s And a.acct_tag='0' "
//          "And (a.remove_tag='0' Or (a.remove_tag!='0' And  nvl(DESTROY_DATE,nvl(PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
//		  "AND (a.net_type_code In %s or  ( a.net_type_code Not In %s "
//          "And(a.service_state_code='0'  or  ( a.service_state_code !='0' 	and nvl(last_stop_date,to_date('20500101000000','YYYYMMDDHH24MISS')) >=to_date('%s','YYYYMMDDHH24MISS'))))) "
//		  "AND a.open_date <=to_date('%s','YYYYMMDDHH24MISS')",
//         m_netTypeCodes.c_str(),
//         t_cycleParam.m_beginTime.c_str(),
//         m_fixNetTypeCodes.c_str(),m_fixNetTypeCodes.c_str(),
//         t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str());
//      }	
//#else //其他省份

	    if(m_eparchyCode != "0")
        {
       sprintf(tempchr,	"Select  USER_ID,SERIAL_NUMBER,EPARCHY_CODE,OPEN_DATE  From tf_f_user a "
          "Where a.DUMMY_TAG = '0' and a.net_type_code Not In %s And a.acct_tag='0' "
          "And (a.remove_tag='0' Or (a.remove_tag!='0' And  nvl(DESTROY_DATE,nvl(PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
		  		"AND a.open_date <=to_date('%s','YYYYMMDDHH24MISS') and a.EPARCHY_CODE='%s'",
         m_netTypeCodes.c_str(),
         t_cycleParam.m_beginTime.c_str(),
         t_cycleParam.m_endTime.c_str(),m_eparchyCode.c_str());
	}
	else
	{
 	sprintf(tempchr,	"Select  USER_ID,SERIAL_NUMBER,EPARCHY_CODE,OPEN_DATE  From tf_f_user a "
          "Where a.DUMMY_TAG = '0' and a.net_type_code Not In %s And a.acct_tag='0' "
          "And (a.remove_tag='0' Or (a.remove_tag!='0' And  nvl(DESTROY_DATE,nvl(PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
		  		"AND a.open_date <=to_date('%s','YYYYMMDDHH24MISS')",
         m_netTypeCodes.c_str(),
         t_cycleParam.m_beginTime.c_str(),
         t_cycleParam.m_endTime.c_str());
	}	
	
//#endif
        

        m_selectInfoSql = tempchr;
  
#ifdef _POSTPAIDSP_                    
        //如果固网后付费用户不出帐 add by xuf 2010-1-16 10:22:32 
       if (m_fixNetTypeCodes!="(' ')") //如果固网网别非空
       {    
       	if (t_cycleParam.m_fileCreateTag!="3"&&
       	  t_cycleParam.m_fileCreateTag!="4")//不生成固网后付费
       	{
       	 	t_strSql = " and ( a.net_type_code not in  " +m_fixNetTypeCodes
       	            + " or ( a.PREPAY_TAG <> '0' and a.net_type_code in  " + m_fixNetTypeCodes + " ) )";
			 	}
			 	else if (t_cycleParam.m_fileCreateTag=="4") //只生成固网后付用户
			 	{
				 	t_strSql = " and ( a.PREPAY_TAG = '0' and a.net_type_code in  " + m_fixNetTypeCodes + " )";
			 	}	
			 
        m_selectInfoSql += t_strSql;		 
       }
#endif  
       
#ifdef _DEBUG_       
cout<<"sql=["<<m_selectInfoSql<<"]"<<endl;
#endif
  theLogger->info("%s\n",m_selectInfoSql.c_str());
        
        //绑定数据    
        recordVector.clear();
        errorVector.clear();
        if(m_dbinterface.dataBind(recordVector,errorVector))
        {
            return false;
        }
        
        //add 2009-1-30 21:03:46
        if(!getNextEventInfo(reAccountEventVector))
        	return false; 
        	
#endif   
    }
    //断开和数据库的连接
    m_dbinterface.disconnect();
    
        
    //返回
    return true;        
}


//重出帐事件生成
bool CEventInput::getNextEventInfo(vector<ReAccountEventInfo>& reAccountEventVector)
{
		int total = 0;
		int success = 0;
		int error = 0;
		int recordCount = 0;
		int fields = 4;
		int t_linenum = 0;
	 
	 do{ 
			if(m_dbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
	 		{
	    	 return true;
	 		}
			
		 	for (vector<string>::iterator iter=recordVector.begin(); iter!=recordVector.end(); iter+=fields)
	 		{
	     string      tempstr = "";
	     ReAccountEventInfo    reAccountEventInfo;
	     
	     reAccountEventInfo.m_userId      = *iter; 
	     if((iter+1) != recordVector.end())
	     {
		     tempstr = *(iter+1);                                            
		     reAccountEventInfo.m_msisdn      = tempstr;
	     }                          
	     if((iter+2) != recordVector.end())
	     {
		     tempstr = *(iter+2);                                            
		     reAccountEventInfo.m_eparchyCode      = tempstr;
	     }                          
	     if((iter+3) != recordVector.end())
	     {
		     tempstr = *(iter+3);                                            
		     reAccountEventInfo.m_openDate      = tempstr;
	     }                          
			 reAccountEventInfo.m_cycId       = m_defaultCycId;
			 reAccountEventInfo.m_monthEndTag = m_monthEndTag;
	     
	     reAccountEventVector.push_back(reAccountEventInfo);
	 		}
	 		
	
	 		//add 2009-1-30 20:48:26
	 		t_linenum+=total;
	 		if (t_linenum >= BACTCHNUM) return false;
	 		
	  }while(total >= DEFAULTARRAYLINE);
	  
	
	//断开和数据库的连接
	m_dbinterface.disconnect();
	return true;
}


void CEventInput::setCurrCycId(const int currCycId)
{
	m_currCycId = currCycId;
}

bool CEventInput::getUgpEventInfo(vector<ReAccountEventInfo>& reAccountUgpEventVector)
{
    reAccountUgpEventVector.clear();
    
    char tempchr[1000+1] = "\0";
//    StringVector                recordVector;
    recordVector.clear();
    StringVector                errorVector;
    
  #ifdef _DEBUG_  
      //连接数据库
      cout << "m_userName=" << m_userName << endl
           << "m_passWord=" << m_passWord << endl
           << "m_servName=" << m_servName << endl;
  #endif
    theLogger->info("m_userName=%s,m_passWord=%s,m_servName=%s\n",
    	m_userName.c_str(),m_passWord.c_str(),m_servName.c_str());
    if(m_ugpDbinterface.connect(m_userName.c_str(),m_passWord.c_str(),
                             m_servName.c_str()))
    {
        return false;
    }
    if (m_eventorType == "1" || m_eventorType == "3")//limeng 20090813 从主表查询，实时与全量数据相同
    {
        //vector<IdInfo> vecIdInfo;
#ifdef _FROMMDB_  //从内存数据库  add 2009-1-30 21:27:33    
        //帐单、内存查询语句赋值        
        if(m_pEventParamMgr->getAllUserReAccount(reAccountUgpEventVector)==-1)
        {
        	#ifdef _DEBUG_
            cout << "m_pEventParamMgr->getAllUserReAccount false" << endl;
          #endif
            return false;
        }
       #ifdef _DEBUG_
        cout << "m_pEventParamMgr->getAllUserReAccount true" << endl;
       #endif
        
#else  //从磁盘数据库      
    		CycParam t_cycleParam;
//				m_currCycId = 0;
//    		m_pEventParamMgr->getMinRealCycId(m_currCycId);
    		
    		m_monthEndTag = '0';
    		m_pEventParamMgr->getCycleTime(m_currCycId, t_cycleParam);
    		if(t_cycleParam.m_addTag>=1)
    		{
        	m_monthEndTag = '1';
    		}
                
        //limeng add 20090813 用户群事件生成	
        if(m_eparchyCode != "0")
        {
         sprintf(tempchr,	"Select distinct a.id  From tf_f_feepolicy a, tf_f_user b,td_b_discnt c ,td_b_feepolicy_comp d "
          									"WHERE  a.id = b.user_id and b.dummy_tag ='1' "
          									"and a.feepolicy_id = c.discnt_code " 
														"and c.b_discnt_code = d.feepolicy_id "
														"and d.event_type_id in  (22,24) "
          									"AND b.PREPAY_TAG <> '2' And b.acct_tag='0' and b.eparchy_code='%s' "
          									"And (b.remove_tag='0' Or (b.remove_tag!='0' And  nvl(b.DESTROY_DATE,nvl(b.PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
          									"AND (b.service_state_code='0'  or (b.service_state_code !='0' and nvl(b.last_stop_date,to_date('20500101000000','YYYYMMDDHH24MISS')) >=to_date('%s','YYYYMMDDHH24MISS'))) "
          									"AND b.open_date <=to_date('%s','YYYYMMDDHH24MISS') "
          									"AND nvl(a.end_date,to_date('20500101000000','YYYYMMDDHH24MISS'))>to_date('%s','YYYYMMDDHH24MISS') "
          									"AND a.start_date <=to_date('%s','YYYYMMDDHH24MISS')  ",
          									m_eparchyCode.c_str(),t_cycleParam.m_beginTime.c_str(),
          									t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str(),
          									t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str());
        }
        else
        {
         sprintf(tempchr,	"Select distinct a.id  From tf_f_feepolicy a, tf_f_user b ,td_b_discnt c ,td_b_feepolicy_comp d "
          									"WHERE  a.id = b.user_id  and  b.dummy_tag ='1' "
                             "and a.feepolicy_id = c.discnt_code " 
														"and c.b_discnt_code = d.feepolicy_id "
														"and d.event_type_id in  (22,24) "
          									"AND b.PREPAY_TAG <> '2' And b.acct_tag='0' "
          									"And (b.remove_tag='0' Or (b.remove_tag!='0' And  nvl(b.DESTROY_DATE,nvl(b.PRE_DESTROY_DATE,to_date('20500101000000','YYYYMMDDHH24MISS')))>to_date('%s','YYYYMMDDHH24MISS'))) "
          									"AND (b.service_state_code='0'  or (b.service_state_code !='0' and nvl(b.last_stop_date,to_date('20500101000000','YYYYMMDDHH24MISS')) >=to_date('%s','YYYYMMDDHH24MISS'))) "
          									"AND b.open_date <=to_date('%s','YYYYMMDDHH24MISS') "
          									"AND nvl(a.end_date,to_date('20500101000000','YYYYMMDDHH24MISS'))>to_date('%s','YYYYMMDDHH24MISS') "
          									"AND a.start_date <=to_date('%s','YYYYMMDDHH24MISS') ",
          									t_cycleParam.m_beginTime.c_str(),
          									t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str(),
          									t_cycleParam.m_beginTime.c_str(),t_cycleParam.m_endTime.c_str());
        }
        
          								                      
          									
#ifdef _DEBUG_       
cout<<"sql=["<<tempchr<<"]"<<endl;
#endif
    theLogger->info("%s\n",tempchr);
        m_selectInfoSql = tempchr;

        //绑定数据    
        ugpRecordVector.clear();
        errorVector.clear();
        if(m_ugpDbinterface.dataBind(ugpRecordVector,errorVector))
        {
            return false;
        }
        
        if(!getNextUgpEventInfo(reAccountUgpEventVector))
        	return false; 	
    
#endif   
    }
	else if (m_eventorType == "4")	//用户群增量
	{
        //帐单、内存查询语句赋值        
        
       // sprintf(tempchr,"SELECT USER_ID,CYCLE_ID,MONTH_END "
         //               "FROM TI_B_REACCOUNTUSER ORDER BY CYCLE_ID, MONTH_END ");
        sprintf(tempchr,"SELECT a.USER_ID,a.CYCLE_ID,a.MONTH_END "
                        "FROM TI_B_REACCOUNTUSER a, tf_f_user b where a.user_id = b.user_id "
						" and b.DUMMY_TAG = '1' ORDER BY a.CYCLE_ID, a.MONTH_END ");
        m_selectInfoSql = tempchr;
        
        //绑定数据    
        ugpRecordVector.clear();
        errorVector.clear();
        if(m_ugpDbinterface.dataBind(ugpRecordVector,errorVector))
        {
            return false;
        }
        
        //执行SQL语句
        int                         total = 0;
        int                         success = 0;
        int                         error = 0;
        int                         recordCount = 0;
        int                         fields = 3; 
        vector<string>::iterator    iter;              
        do
        {
            if(m_ugpDbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
            {
                return false;
            }
        
//#ifdef _DEBUG_       
//cout<<"sql=["<<m_selectInfoSql<<"],total="<<total<<",success="<<success<<",error="<<error<<endl;
//#endif
            for (iter=ugpRecordVector.begin(); iter!=ugpRecordVector.end(); iter+=fields)
            {
                string      tempstr = "";
                ReAccountEventInfo    reAccountEventInfo;
            
                reAccountEventInfo.m_userId = *iter;
                tempstr = *(iter+1);
                reAccountEventInfo.m_cycId = atoi(tempstr.c_str());
                tempstr = *(iter+2);
                //cout <<   "if(tempstr[0]!='\0')" << endl;
                if(tempstr[0]!='\0')
                {
                    reAccountEventInfo.m_monthEndTag = tempstr[0];
                }
                else
                {
                    reAccountEventInfo.m_monthEndTag = m_monthEndTag;
                }
                reAccountUgpEventVector.push_back(reAccountEventInfo);
                if((reAccountUgpEventVector.size()%50000)==0)
                {
                	#ifdef _DEBUG_
                    cout << "reAccountUgpEventVector.size()=" << reAccountUgpEventVector.size() << endl;
                  #endif
                }
            }
        }
        while(total >= DEFAULTARRAYLINE);
        
	}
    //断开和数据库的连接
    m_ugpDbinterface.disconnect();
    
        
    //返回
    return true;        
}

//重出帐事件生成
bool CEventInput::getNextUgpEventInfo(vector<ReAccountEventInfo>& reAccountUgpEventVector)
{
		int total = 0;
		int success = 0;
		int error = 0;
		int recordCount = 0;
		int fields = 1;
		int t_linenum = 0;
	 
	 do{ 
			if(m_ugpDbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
	 		{
	    	 return true;
	 		}
			
	 		fields =1;
	 	  for (vector<string>::iterator iter=ugpRecordVector.begin(); iter!=ugpRecordVector.end(); iter+=fields)
	 		{
	     string      tempstr = "";
	     ReAccountEventInfo    reAccountEventInfo;
	     
	     reAccountEventInfo.m_userId      = *iter;                                   
	     reAccountEventInfo.m_openDate    = tempstr;
			 reAccountEventInfo.m_cycId       = m_defaultCycId;
			 reAccountEventInfo.m_monthEndTag = m_monthEndTag;
	     
	     reAccountUgpEventVector.push_back(reAccountEventInfo);
	 		}
	
	 		//add 2009-1-30 20:48:26
	 		t_linenum+=total;
	 		if (t_linenum >= BACTCHNUM) return false;
	 		
	  }while(total >= DEFAULTARRAYLINE);
	  
	
	//断开和数据库的连接
	m_ugpDbinterface.disconnect();
	return true;
}

bool CEventInput::getAcctEventInfo(vector<ReAccountEventInfo>& reAccountAcctEventVector)
{
    reAccountAcctEventVector.clear();
    
    char tempchr[1000+1] = "\0";
//    StringVector                recordVector;
    recordVector.clear();
    StringVector                errorVector;
  
  #ifdef _DEBUG_  
      //连接数据库
      cout << "m_userName=" << m_userName << endl
           << "m_passWord=" << m_passWord << endl
           << "m_servName=" << m_servName << endl;
  #endif
    theLogger->info("m_userName=%s,m_passWord=%s,m_servName=%s\n",
    	m_userName.c_str(),m_passWord.c_str(),m_servName.c_str());
    if(m_acctDbinterface.connect(m_userName.c_str(),m_passWord.c_str(),
                             m_servName.c_str()))
    {
        return false;
    }
    if (m_eventorType == "1" || m_eventorType == "3")//limeng 20090813 从主表查询，实时与全量数据相同
    {
        //vector<IdInfo> vecIdInfo;
#ifdef _FROMMDB_  //从内存数据库  add 2009-1-30 21:27:33    
        //帐单、内存查询语句赋值        
        if(m_pEventParamMgr->getAllUserReAccount(reAccountAcctEventVector)==-1)
        {
        	#ifdef _DEBUG_
            cout << "m_pEventParamMgr->getAllUserReAccount false" << endl;
          #endif
            return false;
        }
        #ifdef _DEBUG_
        cout << "m_pEventParamMgr->getAllUserReAccount true" << endl;
        #endif
       
        
#else  //从磁盘数据库      
    		CycParam t_cycleParam;
//				m_currCycId = 0;
//    		m_pEventParamMgr->getMinRealCycId(m_currCycId);
    		
    		m_monthEndTag = '0';
    		m_pEventParamMgr->getCycleTime(m_currCycId, t_cycleParam);
    		if(t_cycleParam.m_addTag>=1)
    		{
        	m_monthEndTag = '1';
    		}
                
        //limeng add 20090813 账户全量事件生成	
        if(m_eparchyCode != "0")
        {
    		 sprintf(tempchr,	"  select distinct a.id from  (select f.id,f.id_type,f.start_date ,f.end_date  from  tf_f_feepolicy f "
                          "  union  select d.id ,d.id_type,to_date(d.start_cycle_id,'YYYYMM')  , to_date(d.end_cycle_id,'YYYYMM')  "
                          "  from tf_b_adjustbbill d ) a , " 
    		                  " tf_f_account b  "
               						" where a.id_type =1 "
               						" and a.id = b.acct_id "
               						" and  a.start_date   <= to_date('%s','YYYYMMDDHH24MISS') "
               						" and  a.end_date     >= to_date('%s','YYYYMMDDHH24MISS') "
               						" and  b.eparchy_code='%s' ",
          							  t_cycleParam.m_endTime.c_str(),t_cycleParam.m_beginTime.c_str(),m_eparchyCode.c_str()); 
        }
        else
        {
    		 sprintf(tempchr,	"  select distinct a.id from (select f.id,f.id_type,f.start_date ,f.end_date  from  tf_f_feepolicy f "
                          "  union  select d.id ,d.id_type,to_date(d.start_cycle_id,'YYYYMM')  , to_date(d.end_cycle_id,'YYYYMM')  "
                          "  from tf_b_adjustbbill d ) a  " 
               						" where id_type =1 "
               						" and  a.start_date   <= to_date('%s','YYYYMMDDHH24MISS') "
               						" and  a.end_date     >= to_date('%s','YYYYMMDDHH24MISS') ",
                           t_cycleParam.m_endTime.c_str(),t_cycleParam.m_beginTime.c_str()); 
        }
        
          								
#ifdef _DEBUG_       
cout<<"sql=["<<tempchr<<"]"<<endl;
#endif
    theLogger->info("%s\n",tempchr);
        m_selectInfoSql = tempchr;

        //绑定数据    
        acctRecordVector.clear();
        errorVector.clear();
        if(m_acctDbinterface.dataBind(acctRecordVector,errorVector))
        {
            return false;
        }
        
        if(!getNextAcctEventInfo(reAccountAcctEventVector))
        	return false; 	
    
#endif   
    }
	else if (m_eventorType == "5")	//账户增量
	{
        //帐单、内存查询语句赋值        
        
//        sprintf(tempchr,"SELECT USER_ID,CYCLE_ID,MONTH_END "
//                        "FROM TI_B_REACCOUNTUSER ORDER BY CYCLE_ID, MONTH_END ");
		//关联账户资料表筛选账户
        sprintf(tempchr,"SELECT a.USER_ID,a.CYCLE_ID,a.MONTH_END "
                        "FROM TI_B_REACCOUNTUSER a, TF_F_ACCOUNT b "
						"where A.USER_ID = B.ACCT_ID ORDER BY a.CYCLE_ID, a.MONTH_END ");
        m_selectInfoSql = tempchr;
        
        //绑定数据    
        acctRecordVector.clear();
        errorVector.clear();
        if(m_acctDbinterface.dataBind(acctRecordVector,errorVector))
        {
            return false;
        }
        
        //执行SQL语句
        int                         total = 0;
        int                         success = 0;
        int                         error = 0;
        int                         recordCount = 0;
        int                         fields = 3; 
        vector<string>::iterator    iter;              
        do
        {
            if(m_acctDbinterface.executeSql(m_selectInfoSql.c_str(),total,success,error))
            {
                return false;
            }
        
//#ifdef _DEBUG_       
//cout<<"sql=["<<m_selectInfoSql<<"],total="<<total<<",success="<<success<<",error="<<error<<endl;
//#endif
            for (iter=acctRecordVector.begin(); iter!=acctRecordVector.end(); iter+=fields)
            {
                string      tempstr = "";
                ReAccountEventInfo    reAccountEventInfo;
            
                reAccountEventInfo.m_userId = *iter;
                tempstr = *(iter+1);
                reAccountEventInfo.m_cycId = atoi(tempstr.c_str());
                tempstr = *(iter+2);
                //cout <<   "if(tempstr[0]!='\0')" << endl;
                if(tempstr[0]!='\0')
                {
                    reAccountEventInfo.m_monthEndTag = tempstr[0];
                }
                else
                {
                    reAccountEventInfo.m_monthEndTag = m_monthEndTag;
                }
                reAccountAcctEventVector.push_back(reAccountEventInfo);
                if((reAccountAcctEventVector.size()%50000)==0)
                {
                #ifdef _DEBUG_	
                    cout << "reAccountAcctEventVector.size()=" << reAccountAcctEventVector.size() << endl;
                #endif
                }
            }
        }
        while(total >= DEFAULTARRAYLINE);
        
	}
	
    //断开和数据库的连接
    m_acctDbinterface.disconnect();
    
        
    //返回
    return true;        
}

bool CEventInput::getNextAcctEventInfo(vector<ReAccountEventInfo>& reAccountAcctEventVector)
{
		int total = 0;
	tAcctEventVector.size()=" << reAccountAcctEventVector.size() << endl;
                #endif
                }
            }
        }
        while(total >= DEFAULTARRAYLINE);
        
	}
	
    //鏂