#include "config-all.h"
#include "RateApp.h"
#include "RateConstDef.h"
#include "RateError.h"
#ifdef FP_DEBUG
#include "base/supdebug.h"
#endif
#include "Procedure.h"
#include "base/StringUtil.h"
//#include <sys/time.h>
//#include <string>

const int RATE_SLEEP_TIME = 1  ; 

RateApp   g_rateApp;
Application * const theApp = &g_rateApp;

RateGlobalInfo  g_globalInfo;

RateApp::RateApp()
{
	
} 
  
RateApp::~RateApp() 
{
} 
  
bool RateApp::initialization() 
{
  if(installProc()==false)
  {
    #ifdef _DEBUG_
      cout<<"installProc() false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  setoptstr("c:");
  // normally, call standard initialization first
  if (!Application::initialization())
  {
    #ifdef _DEBUG_
      cout<<"Application::initialization() false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }    
  //// 取配置文件参数
  if (!m_config.getSysParam(m_theSysParam,m_name,m_channelNo)) 
  {
    theErrorMessages->insert(EAPPFRM_SYSVAL,"read configuration fail");
    return false;
  }
  m_runInfo.m_channelNo = m_channelNo;
  #ifdef _DEBUG_
    //m_config.dump();
  #endif
  //set application run method if need, all default is false
  
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
  
  time(&m_pmUpdateTime); //初始化
  return true;
}

bool RateApp::beforeLoop()
{
	if (!Application::beforeLoop())
	{
		return false;
	}
	m_pmManager.setDbNo(m_config.m_cycleDbNo); //add by gaojf 2009-2-13 19:07
	//Added By Sunam 2009/9/19 11:20:27 EparchyCode
	m_pmManager.setEparchyCode(m_config.m_eparchyCode.c_str());
  //参数配置
  if (!m_pmManager.initialize(&m_config)) 
  {
    #ifdef _DEBUG_
      cout<<"m_pmManager init fail!"<<m_pmManager.errorMessage()
          <<" "<<__FILE__<<__LINE__<<endl;
    #else
      theErrorMessages->insert(E_PARAM_INI,"dbparam initialize fail! " 
          + m_pmManager.errorMessage());
    #endif
    return false;
  }
  //帐单初始化
  //if (!m_billManager.initialization(m_config.m_mdbInfo)) 
  if (!m_billManager.initialization(m_config.m_mdbInfo_billInfo)) 
  {
    #ifdef _DEBUG_
      cout<<"bill init fail!"<<__FILE__<<__LINE__<<endl;
    #else
      theErrorMessages->insert(E_BILLMGR_INIT,"bill init fail! ");  
    #endif
    return false;
  }
  //初始化m_runInfo
  if (!m_billManager.getRateRunInfo(m_runInfo))
  {
    #ifdef _DEBUG_
      cout<<"getRateRunInfo m_runInfo fail!"<<__FILE__<<__LINE__<<endl;
    #else
      theErrorMessages->insert(E_GET_RATEINFO,"getRateRunInfo m_runInfo fail! ");  
    #endif
    return false;
  }
  #ifdef _DEBUG_
    //cout<<"m_runInfo="<<m_runInfo<<endl;
  #endif
  // 各过程处理信息初始化
  if (!g_globalInfo.initialization(&m_config,&m_rateLog,&m_pmManager,
      &m_billManager,&m_cycParam,&m_sysCycle,theErrorMessages)) 
  {
    #ifdef _DEBUG_
      cout<<"g_globalInfo initialization fail!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  
  //输入管理器初始化
  if (!m_inputer.initialize(&g_globalInfo,&(m_config.m_inputRuleMgr))) 
  {
    #ifdef _DEBUG_
      cout<<"m_inputer init fail!"<<__FILE__<<__LINE__<<endl;
    #else
      theErrorMessages->insert(E_INITIALIZE,"m_inputer init fail! ");  
    #endif
    return false;
  }
  //处理器初始化
  if(!m_processor.initialize(&g_globalInfo)) 
  {
    #ifdef _DEBUG_
      cout<<"m_processor.initialize(&g_globalInfo) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
      theErrorMessages->insert(E_INITIALIZE,"m_processor init fail! ");
    return false;
  }
  
  //输出管理器初始化
  if (!m_outputer.initialize(&g_globalInfo,&(m_config.m_outputRuleMgr),m_runInfo)) 
  {
    #ifdef _DEBUG_
      cout<<"m_outputer.initialize false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
      theErrorMessages->insert(E_INITIALIZE,"m_outputer init fail! ");
    return false;
  }
  // 初始化脚本处理类
  if (!CDRJScript::Initialize(&(g_globalInfo.m_cdr))) 
  {
    string errorMsg = CDRJScript::GetContainer()->getErrorInfo();
    theErrorMessages->insert(EAPPFRM_SYSVAL,"initialize script fail:" + errorMsg );
    return false;
  }
  m_billManager.clearUserLock(m_channelNo);
  //执行回滚过程:将异常退出需要回退的总量回退掉
  ProcedureHelper::ExecuteProcedure("RollBackBillProc");
  
  return true;
}

bool RateApp::loopProcess()
{
  if(!RateRun())
  {
    m_stop = true;
  } 
  return Application::loopProcess();
}

bool RateApp::end() 
{
  m_billManager.end();
  m_pmManager.end();
  return Application::end();
}


bool RateApp::RateRun() 
{   
	time_t t_nowTime;
	time(&t_nowTime);
	if(difftime(t_nowTime,m_pmUpdateTime)>=60)
	{
		m_pmUpdateTime = t_nowTime;
		//1.检查是否需要更新并更新参数
		if (m_pmManager.needUpdate())
		{
			m_objectName = "更新参数中...";
			m_status = PROC_RUN;
			updateStatus();
			if (!m_pmManager.update())
			{
				m_theErrorMessages->insert(E_PARAM_UPDATING,"dbparam update fail!");
				sleep(RATE_SLEEP_TIME);
				//Modified By Sunam 2009/9/25 22:24:33 参数更新失败时退出
				//return true;
				return false;
			}
		   //获取帐目信息
		   if (!g_globalInfo.m_comBill.getDetailItems() )
		   {
		   	 m_theErrorMessages->insert(E_PARAM_UPDATING,
		                                 "update dbparam Item Error!");
		     #ifdef _DEBUG_
		      cout<<"getItemInfo false! "<<__FILE__<<__LINE__<<endl;
		     #endif                                   
		     return false;
		   }	
		}  
	}               
	//2. 帐期流程控制
	if(cycInfoCtl()==false) return false;
	if(m_stop==true) return true; //帐期切换完可能需要退出
	
	m_rateLog.reset();
	//3.取得要处理的对象
	int t_iRet=m_inputer.getFile(m_rateLog.m_fileName,m_rateLog.m_fullFileName);
	if(t_iRet < 0)
	{
		m_objectName = m_rateLog.m_fileName.c_str();
		m_status = PROC_IDLE;
		updateStatus();
		m_theErrorMessages->insert(E_INVALID_FILE,m_rateLog.m_fileName.c_str());
		return true;
	}
	else if(t_iRet == 0)
	{ 
		m_objectName = "------";
		m_status = PROC_IDLE;
		updateStatus();
		sleep(RATE_SLEEP_TIME);
		return true;
	}
  
	string t_fileName;
	t_fileName = m_rateLog.m_fileName.c_str();
	snprintf(m_runInfo.m_fileName,63+1,"%s",t_fileName.c_str());
	#ifdef FP_DEBUG
		FP_BEGIN(CDRPROCESS_rateind)
	#endif
	//4. 开始处理事务   
	if (!begin()) 
	{
		return true;
	}
  
	int t_rentFlag = 0;//0 非固定费清单,1 固定费清单
	if(strncmp(t_fileName.c_str(),RENTFEE_FILE_INCRE.c_str(),strlen(RENTFEE_FILE_INCRE.c_str()))==0 ||
	   strncmp(t_fileName.c_str(),RENTFEE_FILE_ALL.c_str(),strlen(RENTFEE_FILE_ALL.c_str()))==0 )
	{ //固定费
		dealRentFeeFile(t_fileName.c_str());
	}
	else
	{ //普通清单
		#ifdef FP_DEBUG
      		FP_BEGIN(CDRPROCESS_lock)
		#endif     
    	
    	bool runFlag = true;
    	while (runFlag)
    	{
    		//每次处理文件先初始化
    		g_globalInfo.fileReset();
    		//产生未处理用户列表
    		m_inputer.genNoDealUser();
    		//批量加锁所有用户
    		if(g_globalInfo.lockUserId(g_globalInfo.m_lockUserList,g_globalInfo.m_lockAcctList)==false)
    		{ 
    		    //用户锁超过200S未释放，退出
                theErrorMessages->insert(E_LOCKUSER,"用户锁资源超过200S未释放，程序退出");
                return false;
    		}
    		#ifdef FP_DEBUG
    			FP_END(CDRPROCESS_lock)
			#endif
			//5 处理输入话单 
    		// a.读取有效话单记录
    		while (m_inputer.getCdr()>0) 
    		{
				g_globalInfo.cdrReset();
				// b.输入话单内容
				if (m_inputer.formatCdr() > 0 )
				{
					continue;
				}
				//Modified by xuf 2009-10-2 11:41:36
				if(!g_globalInfo.canonicalize())
				{
#ifdef _DEBUG_	
				cout<<"["<<__FILE__<<":"<<__LINE__<<"]"<<"g_globalInfo.canonicalize error!"<<endl;
#endif							
				theErrorMessages->insert(E_INITIALIZE,"g_globalInfo.canonicalize error!");	
         //Added By Sunam 2009/11/26 14:01:09 输出处理后的话单
				 m_outputer.output((int)(g_globalInfo.m_cycParam.m_cycleId));
         continue;
				}
				//如果是回退用户，则需要延迟处理
				if(g_globalInfo.isDelayedCdr())
				{
					m_inputer.outputDelayCdr();
					continue;
				}
				//Modified By Sunam 改单次加解锁为批量加解锁
				//if(g_globalInfo.lockUserId(g_globalInfo.m_billUserId)==false)
				//{ //加锁失败，则将该话单先作为错单处理并告警
				//  //coding here later
				//}
				//add for rent new version begin
      			if(strncmp(t_fileName.c_str(),USER_FILE_BILL.c_str(),
             	strlen(USER_FILE_BILL.c_str()))==0 ||
             	g_globalInfo.m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_MONFEE)
      			{ 
  	      			g_globalInfo.m_OutputRule= m_outputer.getOutputRule();
  	  			}
  	 			//add for rent new version end
				// c.处理该话单
 				#ifdef FP_DEBUG
					FP_BEGIN(CDRPROCESS_process)
 				#endif
					m_processor.process();
 				#ifdef FP_DEBUG
					FP_END(CDRPROCESS_process)
 				#endif
 				//判断一下锁状态，如果加锁失败，则提交一次账单，并中断当前流程
 				if (!g_globalInfo.m_lockStatus)
 				{
 					//提交账单至内存数据库，并中断当前流程
    				ProcedureHelper::ExecuteProcedure("UpdateBillProc(COMMITTOMDB1)");
    				//当前话单需要重新处理
    				m_inputer.previousCdr();
    				//解锁当前所有加锁用户
    				m_billManager.clearUserLock(m_channelNo);
    				//Sleep 2S
    				sleep(2);
    				//结束当前处理流程
    				break;
 				}
				g_globalInfo.finalize();
				
				//add for rent new version begin
				if(strncmp(t_fileName.c_str(),USER_FILE_BILL.c_str(),
	             strlen(USER_FILE_BILL.c_str()))==0 )
    			{
					m_outputer.ouputRentCdr((int)(g_globalInfo.m_cycParam.m_cycleId),g_globalInfo.m_rentOutputCdrList);
					g_globalInfo.m_rentOutputCdrList.clear();
				}
				//add for monfee out file
				else if(g_globalInfo.m_pCdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_MONFEE)
					{
						//包月费清单文件输出
				   m_outputer.ouputRentCdr((int)(g_globalInfo.m_cycParam.m_cycleId),g_globalInfo.m_monfeeOutputCdrList);
					 g_globalInfo.m_monfeeOutputCdrList.clear();
					}
				else
				{
					// d.输出处理后的话单
					m_outputer.output((int)(g_globalInfo.m_cycParam.m_cycleId));
				}
				//Added By Sunam 2009/10/6 10:58:09 用户群解锁
				g_globalInfo.unlockGroupUser();
				//add for rent new version end
    		  	//Modified By Sunam 改单次加解锁为批价加解锁
				//#ifdef FP_DEBUG
				//	FP_BEGIN(CDRPROCESS_unlock)
				//#endif
				//	g_globalInfo.unlockId(); //用户帐户解锁
				//#ifdef FP_DEBUG
				//	FP_END(CDRPROCESS_unlock)
				//#endif
				//致命错误异常处理 2009-2-20 19:19 gaojf
				if(g_globalInfo.m_fatalECode != 0)
				{
					string t_errorMsg = g_globalInfo.m_fatalEMsg.c_str();
					theErrorMessages->insert(E_FATAL_ERROR,t_errorMsg);
					g_globalInfo.writeErrMsg(__FILE__,__LINE__,t_errorMsg.c_str());
					return false;
				}
			}
			//如果未发生加锁失败的情况则中止循环,文件已经处理完
			if (g_globalInfo.m_lockStatus)
			{
				runFlag = false;
			}
		}
	}
    
   //add by xuf 2009-11-24 19:00:31
   //若需要时处理群优惠，且为非U文件触发
   if (m_pmManager.m_isNeedRealGrpDiscntTag&&
   	  strncmp(t_fileName.c_str(),GRP_FILE_BILL.c_str(),strlen(GRP_FILE_BILL.c_str()))!=0&&
	  strncmp(t_fileName.c_str(),ACCT_FILE_BILL.c_str(),strlen(ACCT_FILE_BILL.c_str()))!=0)
   {
     if(!realUserGrpDiscntProcess())
     {
     	string t_errorMsg;
     	t_errorMsg="FILE: "+t_fileName+" realUserGrpDiscntProcess error! ";
    #ifdef _DEBUG_     
      cout<<t_errorMsg<<__FILE__<<":"<<__LINE__<<endl;
    #endif
      theErrorMessages->insert(E_FATAL_ERROR,t_errorMsg);
     }
   } 
    
    
	#ifdef FP_DEBUG
		FP_BEGIN(CDRPROCESS_commit)
	#endif
	//6.结束该处理事务
	if (commit()==true)
	{
		#ifdef FP_DEBUG
			FP_END(CDRPROCESS_commit)
		#endif
		//7. 输出处理日志信息 
		writeLog();
		#ifdef FP_DEBUG
			FP_END(CDRPROCESS_rateind)
		#endif
		return true;
	}
	else
	{
		rollback();
	}
	return false;
}


//4. 开始处理事务
bool RateApp::begin() 
{
  // a.更新程序状态    
  m_objectName = m_rateLog.m_fileName.c_str();
  m_status = PROC_RUN;
  updateStatus();
  // b.输出器管理事务开始(准备各种文件名)
  if (!m_outputer.fileInit(m_runInfo.m_rateDate)) 
  {
    #ifdef _DEBUG_
      cout<<"m_outputer.fileInit("<<m_runInfo.m_rateDate<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
      theErrorMessages->insert(E_INITIALIZE,"m_outputer.fileInit init fail! ");
    return false;
  }
  
  //  c.打开输入器管理器
  if (!m_inputer.open()) 
  {
  	m_inputer.close();
    m_outputer.closeTmpFile();
    #ifdef _DEBUG_
      cout<<"m_inputer.open() false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
      theErrorMessages->insert(E_INITIALIZE,"m_inputer.open() init fail! ");
    return false;
  }
  m_rateLog.logBegin();
  return true;
}

//5.回滚处理事务
bool RateApp::rollback() 
{
  m_inputer.close();
  m_outputer.closeTmpFile();
  
  //执行回滚过程
  ProcedureHelper::ExecuteProcedure("RollBackBillProc");
  return true;
}

//6.结束该处理事务
bool RateApp::commit()
{
	// a.关闭输入输出器，释放输入输出器资源
	m_inputer.close();	
	#ifdef FP_DEBUG
		FP_BEGIN(clearRollBackBill)
	#endif
	if(m_billManager.clearRollBackBill(m_config.m_channelNo)==false)
	{
		#ifdef _DEBUG_
			cout<<"致命异常发生"<<endl;
		#endif
		theErrorMessages->insert(E_INITIALIZE,"m_billManager.clearRollBackBill fail! ");
		return false;
	}
	#ifdef FP_DEBUG
		FP_END(clearRollBackBill)
	#endif
	//提交账单至内存数据库
    ProcedureHelper::ExecuteProcedure("UpdateBillProc(COMMITTOMDB2)"); 
    //如果提交账单失败，退出，不提交文件
    if(g_globalInfo.m_fatalECode != 0)
	{
		string t_errorMsg = g_globalInfo.m_fatalEMsg.c_str();
		theErrorMessages->insert(E_FATAL_ERROR,t_errorMsg);
		g_globalInfo.writeErrMsg(__FILE__,__LINE__,t_errorMsg.c_str());
		return false;
	}
	// c.提交输入
	m_inputer.fileCommit();
	// d.提交输出
	m_outputer.fileCommit(m_runInfo.m_nextFileNo);
	// e.提交批价信息
	m_billManager.updateRateRunInfo(m_runInfo);
	//Added By Sunam 2009/8/11 9:50:49
	// f.批量解锁
	#ifdef FP_DEBUG
    	FP_BEGIN(CDRPROCESS_unlock)
    #endif
	    m_billManager.clearUserLock(m_channelNo);
	#ifdef FP_DEBUG
    	FP_END(CDRPROCESS_unlock)
    #endif
	return true;
}

//获取处理日期、时间
//r_curDate:     YYYYMMDD  系统日期
//r_cycle: YYMM    系统日期对应的帐期
bool RateApp::getDealTime(int &r_curDate,int &r_cycle) const
{
  time_t t_nowTime;	
  int    t_sysDate,t_delayDate;
  
  
  time(&t_nowTime);	
  t_sysDate   = atoi((StringUtil::format(t_nowTime  ,"%Y%m%d")).c_str());
  strcpy(g_globalInfo.m_dealTime,(StringUtil::format(t_nowTime  ,"%Y%m%d%H%M%S")).c_str());
  //cout<<"m_dealTime="<<g_globalInfo.m_dealTime<<endl;
  r_curDate = t_sysDate ;
  if (!m_pmManager.getBillCycle(r_curDate,r_cycle)) 
  {
    m_theErrorMessages->insert(E_BILL_CYCLE,"get bill cycle fail");
  	return false;
  }
  return true;
}
//帐期流程控制
bool RateApp::cycInfoCtl()
{
  //2.获取当前日期、帐期
  int  t_curDate;
  if(getDealTime(t_curDate,m_sysCycle)==false)
  {
    #ifdef _DEBUG_
      cout<<"getDealTime false!"<<__FILE__<<__LINE__<<endl;
    #endif
      m_theErrorMessages->insert(E_BILL_CYCLE,"getDealTime false!");
    return false;
  }
  if(m_runInfo.m_rateDate  < t_curDate)
  { //日切换
    m_runInfo.m_rateDate = t_curDate;
    m_runInfo.m_nextFileNo = 0;
    m_billManager.updateRateRunInfo(m_runInfo);
  }
  
  if(!(m_config.m_channelType == CHANNEL_PROV))
  {
    //1.获取最新的帐期信息
    if (m_pmManager.getCycleTime(m_sysCycle,m_cycParam)==false)
    {
      #ifdef _DEBUG_
        cout<<"m_pmManager.getCycleTime("<<m_runInfo.m_billCycle
            <<",CycParam) false!"<<__FILE__<<__LINE__<<endl;
      #endif
        m_theErrorMessages->insert(E_BILL_CYCLE,"m_pmManager.getCycleTime false!");
      return false;
    }
    m_runInfo.m_billCycle = m_sysCycle;
  }else
  {
    //1.获取最新的帐期信息
    if (m_pmManager.getCycleTime(m_runInfo.m_billCycle,m_cycParam)==false)
    {
      #ifdef _DEBUG_
        cout<<"m_pmManager.getCycleTime("<<m_runInfo.m_billCycle
            <<",CycParam) false!"<<__FILE__<<__LINE__<<endl;
      #endif
        m_theErrorMessages->insert(E_BILL_CYCLE,"m_pmManager.getCycleTime false!");
      return false;
    }
    if(m_cycParam.m_addTag == CYC_ADDTAG_CHECKEND)
    { //月末出帐结束,进行月切换
      m_runInfo.m_billCycle = m_sysCycle;
      m_billManager.updateRateRunInfo(m_runInfo);
      m_outputer.billCycleSwitch(m_runInfo.m_billCycle);
      //帐期切换完毕，将延迟话单移到正常目录下
      m_inputer.dealDelayedFile();
      if(m_pmManager.getCycleTime(m_runInfo.m_billCycle,m_cycParam)==false)
      {
        #ifdef _DEBUG_
          cout<<"m_pmManager.getCycleTime("<<m_runInfo.m_billCycle
              <<",CycParam) false!"<<__FILE__<<__LINE__<<endl;
        #endif
          m_theErrorMessages->insert(E_BILL_CYCLE,"m_pmManager.getCycleTime false!");
        return false;
      }    
      if(m_config.m_doubleCycMode != 1)
      { //不是双帐期模式,帐期切换完后,退出
        m_stop = true;
      }
    }
  }
  return true;
}

//处理固定费文件
void RateApp::dealRentFeeFile(const char *t_fileName)
{
  //取固定费帐期
  char t_chCycle[6+1];
  char t_chDay[8+1];
  snprintf(t_chCycle,6+1,"%s",t_fileName+6);
  snprintf(t_chDay,8+1,"%s",t_fileName+6);
  int t_cycle = atoi(t_chCycle);
  g_globalInfo.m_acctDay = atoi(t_chDay);
  if( t_cycle <  m_cycParam.m_cycleId ||
     (t_cycle == m_cycParam.m_cycleId &&
      CYC_ADDTAG_CHECKSTART == m_cycParam.m_addTag)||
      t_cycle > m_sysCycle)
  { //不正常固定费事件直接判为无效文件
    m_inputer.mvFileToInvalidPath();
    return;
  }

  map<int,int** >::iterator iterMap;
  //获取子账期开始结束时间的数组指针
  iterMap = g_globalInfo.m_paramMgr->m_subCycStartDayMap.find(t_cycle);
  if(iterMap!=g_globalInfo.m_paramMgr->m_subCycStartDayMap.end())
  {	
    g_globalInfo.m_subCycStartDay = iterMap->second;
  }else
  { //不正常固定费事件直接判为无效文件
    m_inputer.mvFileToInvalidPath();
    return;
  }
  
  iterMap = g_globalInfo.m_paramMgr->m_subCycEndDayMap.find(t_cycle);
  if(iterMap!=g_globalInfo.m_paramMgr->m_subCycEndDayMap.end())
  {	
    g_globalInfo.m_subCycEndDay = iterMap->second;
  }else
  { //不正常固定费事件直接判为无效文件
    m_inputer.mvFileToInvalidPath();
    return;
  }
  
  g_globalInfo.m_inputRule = m_inputer.getInputRule();
  g_globalInfo.m_OutputRule= m_outputer.getOutputRule();
  while(m_inputer.getCdr(g_globalInfo.m_billUserId,
                         g_globalInfo.m_rentInputCdrList)>0)
  {
    g_globalInfo.cdrReset();
    g_globalInfo.lockUserId(g_globalInfo.m_billUserId);
    // c.处理该话单
    ProcedureHelper::ExecuteProcedure("RentProc");
    // d.输出处理后的话单、账单等
    m_outputer.ouputRentCdr((int)(g_globalInfo.m_cycParam.m_cycleId),
                            g_globalInfo.m_rentOutputCdrList);
    g_globalInfo.m_rentOutputCdrList.clear();
    g_globalInfo.unlockId(); //用户帐户解锁
  }
}

void RateApp::writeLog()
{
  m_rateLog.writeLog();
  m_rateLog.logEnd();
}

//add by xuf 2009-11-24 19:03:05
bool RateApp::realUserGrpDiscntProcess()
{
		if(g_globalInfo.m_comBill.m_vUserFileMap.empty())
		{
 			return true;
  	}
  	
		if(!g_globalInfo.lockAcctUserGrp(GRPDISCNT_LOCK_ID))
		{
			//用户锁超过设定时间未释放，退出
			theErrorMessages->insert(E_LOCKUSER,"请检查是否有锁-2的进程已经退出");
			return false;	
		}
 
 //Modified by xuf 2009-12-2 18:59:05
 	g_globalInfo.m_cdr[ATTRI_BIZ_TYPE] = BIZ_TYPE_GRPBILL;
 	g_globalInfo.m_bizType = BIZ_TYPE_GRPBILL;
 	g_globalInfo.m_cdr[ATTRI_NEED_OUTPUTCDR] = FLAG_NO;
	
	for (map <T_SERVER_ID,T_VFILE>::iterator t_itr= g_globalInfo.m_comBill.m_vUserFileMap.begin();
		   t_itr != g_globalInfo.m_comBill.m_vUserFileMap.end();++t_itr)
  {
  		g_globalInfo.m_userBills.clear();       
  		g_globalInfo.m_userSumBill2s.clear();   
  		g_globalInfo.m_accountBills.clear();    
  		g_globalInfo.m_userBills_new.clear();   
  		g_globalInfo.m_userSumBill2s_new.clear();
  		g_globalInfo.m_accountBills_new.clear();
  		g_globalInfo.m_rollBackBills.clear();   
  		g_globalInfo.m_addBillVector.clear();   
  		g_globalInfo.m_balAcctIdList.clear();   
  		g_globalInfo.m_discountBills.clear();   
  		g_globalInfo.m_vecUserServ.clear();     
  		g_globalInfo.m_vecUserServState.clear();
      
  	   g_globalInfo.m_billUserId = t_itr->first;
  	   g_globalInfo.m_cycParam   = t_itr->second.m_cycParam;	   	 
  	   	
  	  if(ProcedureHelper::ExecuteProcedure("UserGrpDiscntProc")!=0)
  	  {
  	  	if(!m_outputer.output((int)(g_globalInfo.m_cycParam.m_cycleId)))
  	  		return false;
  	  	continue;
  	  }
  	  	
  	  if(ProcedureHelper::ExecuteProcedure("PayRelationProc")!=0)
  	  {
  	  	if(!m_outputer.output((int)(g_globalInfo.m_cycParam.m_cycleId)))
  	  		return false;
  	  	continue;
  	  }
  	  	
  	  ProcedureHelper::ExecuteProcedure("UpdateBillProc");
  	  	
  	  if(!m_outputer.output((int)(g_globalInfo.m_cycParam.m_cycleId)))
  	  	return false; 	
  }
  
	
	return true;
}