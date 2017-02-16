#include "MonthEndApp.h"
#include "base/Directory.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"


MonthEndApp  g_application;
Application * const theApp = &g_application;
extern log4cpp::Logger *theLogger;


//##ModelId=477C4655000C
MonthEndApp::MonthEndApp()
{
	m_monthAccountParam = NULL;
	m_fileBatchNum 			= 1;
}

//##ModelId=477C4655000D
MonthEndApp::~MonthEndApp()
{
}

//##ModelId=477C4655000E
bool MonthEndApp::initialization()
{
	    setoptstr("c:");

    // 调用父类的方法进行标准初始化
    if (!Application::initialization())
    {
        return false;
    }

    //置状态
    m_objectName = "程序初始化";
    m_status = PROC_READY;
    updateStatus();

    //将程序设置成后台执行
#ifdef _DEBUG_
    set_asdaemon(false);
    set_onlyone(false);
    set_runinbkg(false);
#else
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif
    return true;
	
}

//##ModelId=477C46550010
bool MonthEndApp::beforeLoop()
{
	 if ( !Application::beforeLoop() )
    {
        return false;
    }
        
    //-----------------------------------------------------------------------//
    // 增加直接的初始化代码
    //-----------------------------------------------------------------------//
    //取配置文件参数(theApp-->m_monthEndConfig)
  try
  {
    if (!getSysParams())
    {
        return false;
    }
    
    MdbInfo  t_mdbInfo;

    t_mdbInfo.m_dbType = m_monthEndConfig.m_mdbType;
    t_mdbInfo.m_hostName = m_monthEndConfig.m_mdbHostName;
    t_mdbInfo.m_user = m_monthEndConfig.m_mdbUserName;
    t_mdbInfo.m_passwd= m_monthEndConfig.m_mdbPassWord;
    t_mdbInfo.m_port = m_monthEndConfig.m_mdbPort;
    t_mdbInfo.m_dbServer = m_monthEndConfig.m_mdbserver;    
    
    int rd = 0;
    m_monthAccountParam = new PM_MonthAccount();
    
    //设置用户资料参数管理的类基本信息
    (*m_monthAccountParam).setDbConnInfo(m_monthEndConfig.m_userName,
                                        m_monthEndConfig.m_passWord,
                                        m_monthEndConfig.m_servName);
    (*m_monthAccountParam).setHostName(m_monthEndConfig.m_hostName.c_str());
    (*m_monthAccountParam).setAppName(m_name);
    (*m_monthAccountParam).setUpdateInterval(m_monthEndConfig.m_interval);
    (*m_monthAccountParam).setChannelNo(m_channelNo);
    
    (*m_monthAccountParam).setAltiBaseConnInfo(m_monthEndConfig.m_altiUserName,
                                        			m_monthEndConfig.m_altiPassWord,
                                        			m_monthEndConfig.m_altiServName,
                                        			m_monthEndConfig.m_altiPort);
    (*m_monthAccountParam).setCrmConnInfo(m_monthEndConfig.m_crmUserName,
	                                   			m_monthEndConfig.m_crmPassWord,
	                                   			m_monthEndConfig.m_crmServName);
   
    (*m_monthAccountParam).setCbsProcInfo(m_monthEndConfig.m_cbsProcMap);
    (*m_monthAccountParam).setCrmProcInfo(m_monthEndConfig.m_crmProcMap);
    (*m_monthAccountParam).setAltibaseProcInfo(m_monthEndConfig.m_altiProcMap);
    
     (*m_monthAccountParam).setMaxRecordNum(m_monthEndConfig.m_maxRecordNum);
     
     //把配置对象传给param
     (*m_monthAccountParam).setUnAcctNetCodes(m_monthEndConfig.m_unAcctNetTypeCodes);
     //added by pangx 2010-5-12 begin
     (*m_monthAccountParam).setOneClickPayProvinceCode(m_monthEndConfig.m_strOneClickPayProvinceCode);
     
	if(!(*m_monthAccountParam).setMdbInfo(t_mdbInfo))
    {
        #ifdef _DEBUG_
            cout<<"setMdbInfo("<<t_mdbInfo<<") false!"
                <<__FILE__<<__LINE__<<endl;
        #endif
        return false;
    }
    else
    {
        #ifdef _DEBUG_
            cout<<"setMdbInfo("<<t_mdbInfo<<") OK!" << endl;
        #endif
    }     
     //added by pangx 2010-5-12 end  
     
    //用户资料参数管理类初始化
    if( !(*m_monthAccountParam).initialization() )
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
                                   "UserParam Initialization Error" );
        return false;
    }


    //获取数据库中的参数信息
    if (!getSysParamsFromDb())
    {
        return false;
    }
  }
  catch (exception &e )
  {     	
   	DEBUG_PRINTF("%d:%d: %s",__FILE__,__LINE__,e.what());
    writeAlarm(EMonthEndBASE,e.what());
    return false;
  }
    //置状态
    m_objectName = "初始化取配置";
    m_status = PROC_READY;
    updateStatus();
    

    return true;
	
}

//##ModelId=477C46550012
bool MonthEndApp::loopProcess()
{
	    if(monthEndRun()) //执行一次
    {
        m_stop = true;
    }

    return  true;//Application::loopProcess();
	
}

//##ModelId=477C46550014
bool MonthEndApp::recovery()
{
	 Application::recovery();

    return true;
}

//##ModelId=477C46550016
bool MonthEndApp::end()
{
	
    //清空内存
 //   m_userBillManager.end();
    (*m_monthAccountParam).end();
//    (*m_accountParam).end();

//    m_accountCheckInfoVector.clear();

//	m_monthEndOutputFile.end();

    //析构参数管理器
    if	(m_monthAccountParam!=NULL)
    	delete m_monthAccountParam;
    	
    m_monthAccountParam = NULL;

    //置状态
    m_objectName = "程序退出";
    m_status = PROC_EXIT;
    updateStatus();

    return Application::end();
}

//取boss.conf的配置
//##ModelId=477C46550018
bool MonthEndApp::getSysParams()
{
	string	name;
	string	value;
	string	C;
	string  LA;
	string  AF;
	string  LB;
	string  LBF;
	string	L;
	string  LC;
	string  LM;
	string  LCF;
	string	t_channelNo;

  ImportMode im_tmp;

	char cTmp[4];
	sprintf(cTmp,"%d", m_channelNo);
	t_channelNo = cTmp;
	memset(cTmp,0,sizeof(cTmp));

	L += SECTDELIM;
	L += m_name ;
	L += SECTDELIM;
	L += t_channelNo;

	C += SECTDELIM;
	C += m_name ;
	C += SECTDELIM;
	C += "commParam";
		
	LA = L;
	LA += SECTDELIM;
	LA += "altibase";
	
	LB = L;
	LB += SECTDELIM;
	LB += "oraclecbs";
	
	LC = L;
	LC += SECTDELIM;
	LC += "oraclecrm";
	
	LM = L;
	LM += SECTDELIM;
	LM += "mdb";	

	//#define GET_PARAM(X,Y)  name = Y; if (!m_theSysParam->getValue(X, name, value)) { m_theErrorMessages->insert(ERROR_PARAM_FILE,X + SECTDELIM + name); return false; };

  #define GET_PARAM(X,Y)  name = Y;                                       \
  if (!m_theSysParam->getValue(X, name, value))                        \
  {                                                                    \
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,X + SECTDELIM + name); \
      return false;                                                    \
  };

	GET_PARAM(C,"hostName");
	m_monthEndConfig.m_hostName = value;

	GET_PARAM(C,"timeInterval");
	m_monthEndConfig.m_interval = atoi(value.c_str());
	
	 //是否备份合法文件
	GET_PARAM(C,"isBackupValidFile");
	m_monthEndConfig.m_isBackupValidFile = (bool)atoi(value.c_str());
    
   //(7)是否备份非法文件
	GET_PARAM(C,"isBackupInvalidFile");
	m_monthEndConfig.m_isBackupInvalidFile =  (bool)atoi(value.c_str());
	
	//added by pangx on 2010-3-29 begin

	GET_PARAM(C,"oneClickPayProvinceCode");
	m_monthEndConfig.m_strOneClickPayProvinceCode = value;
    //added by pangx on 2010-3-29 end
	//不开账用户网别 add 2009-11-28 16:22:21
	GET_PARAM(C,"unAcctNetTypeCodes");
	
	m_monthEndConfig.m_unAcctNetTypeCodes.clear();
	
	if (value!="")
		splitEnumValue(value.c_str(), m_monthEndConfig.m_unAcctNetTypeCodes);
	
	GET_PARAM(LB,"userName");
	m_monthEndConfig.m_userName = value;

	GET_PARAM(LB,"passWord");
	m_monthEndConfig.m_passWord = value;

	GET_PARAM(LB,"servName");
	m_monthEndConfig.m_servName = value;
	
	
	int formatNum=0;
  char tmp[MAXBUFFLEN+1];

  //---------------crm---------------
  
  GET_PARAM(LC,"userName");
	m_monthEndConfig.m_crmUserName = value;

	GET_PARAM(LC,"passWord");
	m_monthEndConfig.m_crmPassWord = value;

	GET_PARAM(LC,"servName");
	m_monthEndConfig.m_crmServName = value;

  GET_PARAM(LC,"FORMATNUM");
	formatNum= atoi(value.c_str());
	
  // 不同的格式配置信息
  for(int i=1;i<formatNum+1;i++) 
  {
		sprintf(tmp,"FORMAT%d",i);
    LCF = LC +SECTDELIM+ tmp;
    
		GET_PARAM(LCF,"PROCNAME");
		im_tmp.m_procName= value;   
    
		GET_PARAM(LCF,"PROCEDURE");
		im_tmp.m_procedure= value;    
   
		GET_PARAM(LCF,"FLAGOFEXEPROC");
 		im_tmp.m_execProc= value;
   
		m_monthEndConfig.m_crmProcMap.insert(make_pair(im_tmp.m_procName,im_tmp));
  } 
  
  	//------------altibase-------------
	GET_PARAM(LA,"USER");
	m_monthEndConfig.m_altiUserName = value;

	GET_PARAM(LA,"PASSWD");
	m_monthEndConfig.m_altiPassWord = value;

	GET_PARAM(LA,"SERVER");
	m_monthEndConfig.m_altiServName = value;
	
	GET_PARAM(LA,"PORT");
	m_monthEndConfig.m_altiPort = value;
	
	//-------------------mdb---------------
	GET_PARAM(LM,"MDBTYPE");
	m_monthEndConfig.m_mdbType = (T_DATABASETYPE)atoi(value.c_str());;

	GET_PARAM(LM,"MDBSERVER");
	m_monthEndConfig.m_mdbserver = value;

	GET_PARAM(LM,"MDBHOSTNAME");
	m_monthEndConfig.m_mdbHostName = value;
	
	GET_PARAM(LM,"MDBPORT");
	m_monthEndConfig.m_mdbPort = atoi(value.c_str());
	
	GET_PARAM(LM,"MDBUSERNAME");
	m_monthEndConfig.m_mdbUserName = value;
	
	GET_PARAM(LM,"MDBPASSWORD");
	m_monthEndConfig.m_mdbPassWord = value;
			

   //输出文件工作路径
	GET_PARAM(L,"outputFileWorkPath");
	m_monthEndConfig.m_outputFileWorkPath = value;
	if(!isValidPath(m_monthEndConfig.m_outputFileWorkPath))
	{
	   //throw 
	    return false;
	}
	if(m_monthEndConfig.m_outputFileWorkPath
	                    [m_monthEndConfig.m_outputFileWorkPath.length()-1] !='/')
	{
	    m_monthEndConfig.m_outputFileWorkPath.append("/");
	}
  
	GET_PARAM(L,"outputFilePath_S");
	m_monthEndConfig.m_outputFilePath = value;
	if(!isValidPath(m_monthEndConfig.m_outputFilePath)) //add by xuf 2009-12-23 16:47:32
	{
    	return false;
	}
	if (m_monthEndConfig.m_outputFilePath.substr(
            m_monthEndConfig.m_outputFilePath.length()-1) != "/")
	{
		m_monthEndConfig.m_outputFilePath =
            m_monthEndConfig.m_outputFilePath + "/";
	}

   //add by dingr
   //公免用户输出路径
	GET_PARAM(L,"outputFilePath_Z");
	m_monthEndConfig.m_outputFilePath_Z = value;
	if(!isValidPath(m_monthEndConfig.m_outputFilePath_Z))//add by fug 2009-12-23 16:47:39
	{
    	return false;
	}
	if (m_monthEndConfig.m_outputFilePath_Z.substr(
            m_monthEndConfig.m_outputFilePath_Z.length()-1) != "/")
	{
		m_monthEndConfig.m_outputFilePath_Z =
            m_monthEndConfig.m_outputFilePath_Z + "/";
	}

	if(m_monthEndConfig.m_isBackupValidFile)
	{
	    //(14)获取备份文件路径
	    GET_PARAM(L,"backupFilePath");
	    m_monthEndConfig.m_backupFilePath = value;
	    if(!isValidPath(m_monthEndConfig.m_backupFilePath))
	    {
	    	 //throw 
	        return false;
	    }
	    if(m_monthEndConfig.m_backupFilePath
	                        [m_monthEndConfig.m_backupFilePath.length()-1]!='/')
	    {
	        m_monthEndConfig.m_backupFilePath.append("/");
	    }    
	}
	
	
	if(m_monthEndConfig.m_isBackupInvalidFile)
	{
	    //非法文件放置路径
	    GET_PARAM(L,"invalidFilePath");
	    m_monthEndConfig.m_invalidFilePath = value;
	    if(!isValidPath(m_monthEndConfig.m_invalidFilePath))
	    {
	    	 //throw 
	        return false;
	    }
	    if(m_monthEndConfig.m_invalidFilePath
	                        [m_monthEndConfig.m_invalidFilePath.length()-1]!='/')
	    {
	        m_monthEndConfig.m_invalidFilePath.append("/");
	    }
	}    
  //added by pangx on 2010-3-29 begin
     GET_PARAM(L,"vpdnOutputFilePath");
	m_monthEndConfig.m_vpdnOutputFilePath = value;
	if(!isValidPath(m_monthEndConfig.m_vpdnOutputFilePath))
	{
	    return false;
	}
	if(m_monthEndConfig.m_vpdnOutputFilePath
	                    [m_monthEndConfig.m_vpdnOutputFilePath.length()-1] !='/')
	{
	    m_monthEndConfig.m_vpdnOutputFilePath.append("/");
	}
	
	GET_PARAM(L,"vpdnBackupFilePath");
	m_monthEndConfig.m_vpdnBackupFilePath = value;
	if(!isValidPath(m_monthEndConfig.m_vpdnBackupFilePath))
	{
	    return false;
	}
	if(m_monthEndConfig.m_vpdnBackupFilePath
	                    [m_monthEndConfig.m_vpdnBackupFilePath.length()-1] !='/')
	{
	    m_monthEndConfig.m_vpdnBackupFilePath.append("/");
	}
    //added by pangx on 2010-3-29 end  

	GET_PARAM(L,"sleepTime");
	m_monthEndConfig.m_sleepTime = atoi(value.c_str());

  m_monthEndConfig.m_channelNo = m_channelNo;
  	
 	GET_PARAM(L,"maxRecordNum");
	m_monthEndConfig.m_maxRecordNum = atoi(value.c_str());

	GET_PARAM(L,"dbNo");
	m_monthEndConfig.m_dbNo = atoi(value.c_str());
	

	return true;
	
}

//##ModelId=477C46550019
bool MonthEndApp::getSysParamsFromDb()
{
	   //获取当前的省份编码
    (*m_monthAccountParam).getLocalProvinceCode(m_provinceCode);

		//modified by chenyong 2009-10-25--begin  
		(*m_monthAccountParam).getUnAccInfo(m_monthEndConfig.m_dbNo);
		//modified by chenyong 2009-10-25--end  
		    
	  //获取当前需要开帐的帐期
    if (!(*m_monthAccountParam).getMinRealCycId(m_monthEndConfig.m_dbNo, m_minRealCycId))
    {
    	 theErrorMessages->insert(ERROR_PARAM_DATABASE, 
      													           "get min real cycleId error," + m_minRealCycId);		
        return false;
    }
    
    //获取需要开账的地市
    if(!(*m_monthAccountParam).getActEparchyCode(m_minRealCycId,m_monthEndConfig.m_dbNo,m_actEparchyCode))
    {
#ifdef _DEBUG_
   cout<<"没有需要开账的地市,td_b_cycle_eparchy.acct_tag all is 0"<<endl;
#endif
    	
       theErrorMessages->insert(ERROR_PARAM_DATABASE, 
      													           "td_b_cycle_eparchy.acct_tag all is 0,cycle_id =" + m_minRealCycId);		
        return false;
    	
    }    
            
    //获取零帐单入综合账单库标志 0:零帐单不入库 1:零帐单入库
    (*m_monthAccountParam).getZeroDetailBillIntoDBTag( );
    
    //获取输出综合账单文件的通道划分
    (*m_monthAccountParam).getOutPutChannelNoFromCrm(m_channelStVec);
    
    //获取输出综合账单文件的批次划分
     (*m_monthAccountParam).getOutPutBatchNoFromCrm(m_fileBatchNum);
    
    
    return true;
}




//##ModelId=477C4655001D
void MonthEndApp::getDealDate()
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

    m_dealDate = tmpDate;

    memset(tmpDate,0,sizeof(tmpDate));
		
}

//##ModelId=477C46550025
bool MonthEndApp::monthEndRun()
{
	#ifdef _DEBUG_
    cout <<"============monthEndRun begin============="<< endl;
    #endif  
    //置状态 add by xuf 2010-1-1 8:23:02
    m_objectName = "文件生成中...";
    m_status = PROC_RUN;
    updateStatus();  
    
	   //日志开始
	m_performId = theLogger->pBegin();
	
	if(m_actEparchyCode.size() > 0)
	{
       try
       {
       	//获取当前处理时间
        getDealDate();
        FEESUM feeSum;
        char l_strCurrentCycId[6+1];
        sprintf(l_strCurrentCycId,"%d\0",m_minRealCycId); 
        string  l_currentCycId =  l_strCurrentCycId;
        
        
       //完成输出路径2008-3-31 14:51
		//m_monthEndConfig.m_outputFilePath += l_currentCycId.substr(0,6);
    	m_monthEndConfig.m_outputFilePath += m_dealDate.substr(0,6);//应销账组要求，此处改成系统时间所在年月 add by xuf 2010-1-1 22:57:09
    	if(!isValidPath(m_monthEndConfig.m_outputFilePath))
		{
    		if(-1==mkdir(m_monthEndConfig.m_outputFilePath.c_str(),0777))
    		{
    			char strErrNo[120];
    			sprintf(strErrNo,"errno is:[%d]",errno);
    			theErrorMessages->insert(ERROR_OPEN_FILE, 
      												m_monthEndConfig.m_outputFilePath + "--mkdir error," + strErrNo);		
    		}	
		}
    //added by pangx on 2010-3-29 begin
                    if(!isValidPath(m_monthEndConfig.m_vpdnOutputFilePath))
				{
       				if(-1==mkdir(m_monthEndConfig.m_vpdnOutputFilePath.c_str(),0777))
        		 	{
        				char strErrNo[120];
        				sprintf(strErrNo,"errno is:[%d]",errno);
        				theErrorMessages->insert(ERROR_OPEN_FILE,
                            m_monthEndConfig.m_vpdnOutputFilePath + "--mkdir error," + strErrNo);       
        			}	
				}
    			  //added by pangx on 2010-3-29 end
		m_monthEndConfig.m_outputFilePath += string("/DRECV/");
		if(!isValidPath(m_monthEndConfig.m_outputFilePath))
		{
   			if(-1==mkdir(m_monthEndConfig.m_outputFilePath.c_str(),0777))
    		{
    			char strErrNo[120];
    			sprintf(strErrNo,"errno is:[%d]",errno);
    			theErrorMessages->insert(ERROR_OPEN_FILE, 
      												m_monthEndConfig.m_outputFilePath + "--mkdir error," + strErrNo);		
    		}	
		}
			
		//公免用户账单输出路径add by dingr 2009-12-05	
	    m_monthEndConfig.m_outputFilePath_Z += l_currentCycId.substr(0,6);
         if(!isValidPath(m_monthEndConfig.m_outputFilePath_Z))
	  {
    		if(-1==mkdir(m_monthEndConfig.m_outputFilePath_Z.c_str(),0777))
    		{
    			char strErrNo[120];
    			sprintf(strErrNo,"errno is:[%d]",errno);
    			theErrorMessages->insert(ERROR_OPEN_FILE, 
      												m_monthEndConfig.m_outputFilePath_Z + "--mkdir error," + strErrNo);		
    		}	
	   }
           m_monthEndConfig.m_outputFilePath_Z +=string("/");
			//备份路径
		 if (m_monthEndConfig.m_isBackupValidFile)
		 {
			//m_monthEndConfig.m_backupFilePath += l_currentCycId.substr(0,6);
			m_monthEndConfig.m_backupFilePath += m_dealDate.substr(0,6); //应销账组要求，接口路径改成系统时间所在年月 modified by xuf 2010-1-1 22:54:50
    		if(!isValidPath(m_monthEndConfig.m_backupFilePath))
			{
    			if(-1==mkdir(m_monthEndConfig.m_backupFilePath.c_str(),0777))
    			{
    			char strErrNo[120];
    			sprintf(strErrNo,"errno is:[%d]",errno);
    			theErrorMessages->insert(ERROR_OPEN_FILE, 
      												m_monthEndConfig.m_backupFilePath + "--mkdir error," + strErrNo);		
    			}	
			}
     //added by pangx on 2010-3-29 begin
                    if(!isValidPath(m_monthEndConfig.m_vpdnBackupFilePath))
				{
       				if(-1==mkdir(m_monthEndConfig.m_vpdnBackupFilePath.c_str(),0777))
        			{
        				char strErrNo[120];
        				sprintf(strErrNo,"errno is:[%d]",errno);
        				theErrorMessages->insert(ERROR_OPEN_FILE,
                            m_monthEndConfig.m_vpdnBackupFilePath + "--mkdir error," + strErrNo);       
        			}	
				}
               //added by pangx on 2010-3-29 end
			m_monthEndConfig.m_backupFilePath += string("/DRECV/");
			if(!isValidPath(m_monthEndConfig.m_backupFilePath))
			{
   				if(-1==mkdir(m_monthEndConfig.m_backupFilePath.c_str(),0777))
    			{
    				char strErrNo[120];
    				sprintf(strErrNo,"errno is:[%d]",errno);
    				theErrorMessages->insert(ERROR_OPEN_FILE, 
      												m_monthEndConfig.m_backupFilePath + "--mkdir error," + strErrNo);		
    			}	
			}
		 }
        
        
        //用户出帐输出文件类初始化
        m_monthEndOutputFile.init(m_performId,
                                  &m_monthEndConfig,
                                  m_channelStVec,
                                  m_fileBatchNum,
                                  l_currentCycId,
                                  m_dealDate,
                                  m_actEparchyCode
                                  );
                                           
        
        m_monthInternalData.initialize(m_minRealCycId,
                                       m_monthAccountParam,
                                       m_zeroDetailBillIntoIntDbTag
                                       );
        
        
        (*m_monthAccountParam).setOutPutFile(&m_monthEndOutputFile);//2008-3-4 20:26
        
                
        m_monthInternalData.transDetailbillToIntegratebill( );  
        
                          
        (*m_monthAccountParam).getFeeSum(feeSum);  
              
        theLogger->pEndOutput(m_performId,
                              "合账后:通道:",
                               m_channelNo,
                               "DETAILBILLRECORDS=%lld,"
                              "DETAILBILL_FEE_SUM=%lld,UNACCT_FEE_SUM=%lld,"
                              "NOINFOUSERFEE=%lld,INTEGRATE_FEE_SUM=%lld,"
                              "UNACCTRECORDS=%lld,"
							  							"INVALIDRECORDS=%lld,"
                              "INTERBILLRECORDS=%lld",
                              (*m_monthAccountParam).m_recordNum,
                               feeSum.m_detailBillFeeSum,
                               feeSum.m_unAcctFeeSum,
                               feeSum.m_noInfoFeeSum,
                               feeSum.m_IntegrateFeeSum,
                               (*m_monthAccountParam).m_unAcctRecords,
                               m_monthEndOutputFile.m_invalidRecords, //记录无效记录数 add by xuf 2010-1-7 17:30:06
                               m_monthEndOutputFile.m_integrateBillRecords
                               );
                                                        
                                                                              
         //输出文件关闭并清除
        m_monthEndOutputFile.closeFile();
        if (!m_monthEndOutputFile.linkUnlink( ))
        {
            return true;
        }
        
       }catch(Exception &e )
       {     	
       	DEBUG_PRINTF("%d:%d: %s",__FILE__,__LINE__,e.what());
       	writeAlarm(EMonthEndBASE,e.what());
       } 		
	}
    //日志结束
    theLogger->pEnd(m_performId);
        
	return true;
}


int MonthEndApp::writeAlarm(const int ecode,const char *msg)
{
  m_theErrorMessages->insert(ecode,msg);
  writeAlert();
  return 0;
}

bool MonthEndApp::isValidPath(const string& path)
{                                                 
    DIR *dir;                                     //
    if((dir=opendir(path.c_str())) == NULL )      
    {  
		char t_errMsg[MAXBUFFLEN];  
		sprintf(t_errMsg,"path:%s is not exists!",path.c_str());  	
#ifdef _DEBUG_   
	 cout<<t_errMsg<<endl;
#endif    
		writeAlarm(EMonthEndBASE,t_errMsg); 	                                           
        return false;                             
    }                                             
                                                  
    return true;                                  
}                                                 

//分割枚举
void MonthEndApp::splitEnumValue(const char* r_pEnumValue, 
	                            		set<string> &r_sValueList)
{
	char paramValue[MAX_STR_LEN+1];
	char * pValue;
	string t_value("");
	int t_len=strlen(r_pEnumValue)< MAX_STR_LEN?strlen(r_pEnumValue):MAX_STR_LEN;
	strncpy(paramValue, r_pEnumValue,t_len);
	paramValue[t_len]='\0';
		
	r_sValueList.clear();
	pValue = strtok(paramValue, ",");	
		 
	while (pValue != NULL)
	{	
		t_value = pValue;
		if (r_sValueList.find(t_value)==r_sValueList.end())
			r_sValueList.insert(t_value);
		pValue = strtok(NULL, ",");
	}
	
#ifdef __DEBUG__
cout<<"before spilt unacctNetTypeCodes:"<<r_pEnumValue<<endl;	
cout<<"after  spilt unacctNetTypeCodes:";	
for(set<string>::iterator t_itr=r_sValueList.begin();
	  t_itr!=r_sValueList.end();++t_itr)
	  cout<<*t_itr<<",";
cout<<endl;	
#endif
		 
}	