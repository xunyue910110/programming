#include "CEventInput.h"
#include "CEventOutputFile.h"
#include "CEventInfoCreator.h"

#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include "CEventCreatorApp.h"
#include "base/Directory.h"
#include <occi.h>  
#include "base/TimeUtil.h"

using namespace oracle::occi; 

CEventCreatorApp g_application;
Application * const theApp = &g_application;
extern log4cpp::Logger *theLogger;

CEventCreatorApp::CEventCreatorApp()
{
	//add 2009-11-11 22:10:10
	m_pEventParam = NULL;
}


CEventCreatorApp::~CEventCreatorApp()
{
    if (m_pEventParam)
    {
        m_pEventParam->end();
        delete m_pEventParam;
        m_pEventParam = 0;
    }
}

bool CEventCreatorApp::beforeLoop()
{
    if ( !Application::beforeLoop() )
    {
        return false;
    }
    
     //取配置文件参数(theApp-->m_eventConfig)
    if (!getSysParams())
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,"GetSysParams Error");
        
        m_objectName = "从系统配置文件中获取系统参数出错，请检查。";
        updateStatus();
        return false;
    }
    
    
    m_pEventParam = new PM_Billing;
    if ( m_pEventParam == NULL ) {
        
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
            "Param initialization Error" );
        return false;
    }
    
    m_pEventParam->setDbNo(m_eventConfig.m_dbNo);
     
    T_EPARCHY_CODE          t_eparchyCode=m_eventConfig.m_eparchyCode;//add by xuf 2010-1-26 21:04:14    
    //limeng 20090817 设置地市编码
   // m_pEventParam->setEparchyCode(m_eventConfig.m_eparchyCode);
    //init param
    m_pEventParam->setDbConnInfo(m_eventConfig.m_paramUserName,
                                 m_eventConfig.m_paramPassWord,
                                 m_eventConfig.m_paramServName);
    m_pEventParam->setHostName(m_eventConfig.m_hostName.c_str());
    m_pEventParam->setAppName(m_name);
    m_pEventParam->setUpdateInterval(m_eventConfig.m_interval);
    
    m_pEventParam->setChannelNo(m_channelNo);
    
      
    //内存数据库   
    MdbInfo  t_mdbInfo;  
   
    t_mdbInfo.m_dbType  = m_eventConfig.m_mdbType;
    #ifdef _DEBUG_
    cout <<"m_dbType=" <<t_mdbInfo.m_dbType << endl;
    #endif
    t_mdbInfo.m_hostName = m_eventConfig.m_mdbHostName;
    t_mdbInfo.m_user =m_eventConfig.m_mdbUserName;
    t_mdbInfo.m_passwd=m_eventConfig.m_mdbPassWord;
    t_mdbInfo.m_port = m_eventConfig.m_mdbPort;
    t_mdbInfo.m_dbServer = m_eventConfig.m_mdbServer;
  
    if((*m_pEventParam).setMdbInfo(t_mdbInfo)==false)
    {
      #ifdef _DEBUG_
        cout<<"setMdbInfo("<<t_mdbInfo<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }

    //pm_accountrent initialize
    if (!(*m_pEventParam).initialization())
    {
        m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE,
                                   "Param Initialization Error");
        m_objectName = "从数据库和数据文件中获取系统参数出错，请检查。";
        updateStatus();
        
        return false;
    }
    
     //limeng add 20100125 epachyCode=0时取td_b_userchannel同库第一个地市
    if(m_eventConfig.m_eparchyCode == "0")
    {
	    vector<UserChannel>	t_userChannel;
	    vector<UserChannel>::iterator iter;
	    
	    m_pEventParam->getUserChannel(m_eventConfig.m_dbNo,t_userChannel);
	    iter = t_userChannel.begin();
	    if (iter==t_userChannel.end()) 
	    {
	    	 m_theErrorMessages->insert(ERROR_PARAM_INITIALIZE, "get param from td_user_channel errror!" );
	       return false;
	    }
	    t_eparchyCode = (*iter).m_eparchyCode;
	    #ifdef _DEBUG_
	     cout<<"(*iter).m_eparchyCode:"<<(*iter).m_eparchyCode<<endl;
	    #endif
	    theLogger->info("epachyCode=0时取td_b_userchannel同库第一个地市:%s\n",(*iter).m_eparchyCode.c_str());
	  }
    
     m_pEventParam->setEparchyCode(t_eparchyCode);
    
    int i = 0;
    (*m_pEventParam).getMinRealCycId(i);
    m_minRealCycId = i;
    
    getDealDate();
 
//#ifdef __PROV019__  //如果是山西    
     //取资费绑定偏移时间add by xuf 2010-2-3 10:52:59
     m_bindTpOffsetSecond  = 0;
     ProcSysParam            t_sysParamInfo;
     if(m_pEventParam->getSysParam("EVENTOR_COMPUTE_CYC",m_channelNo,t_sysParamInfo))
     {
    	   m_bindTpOffsetSecond = t_sysParamInfo.m_valueN;
     }
//#endif    
    
    return true;
}

bool CEventCreatorApp::initialization()
{

    setoptstr("c:");

    if (!Application::initialization())
    {
        return false;
    }

    //置状态
    m_compFlag = true;
    m_rateReadyFlag = false;
    m_objectName = "程序初始化";
    m_status = PROC_READY;
    updateStatus();
    
    //程序设置成后台执行
#ifdef _DEBUG_
    set_asdaemon(false);
    set_onlyone(false);
    set_runinbkg(false);
#else
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif
    //-----------------------------------------------------------------------//
    // ADD YOUR INITIALIZATION CODE HERE
    //-----------------------------------------------------------------------//

   
    
//  m_objectName = "资料数据准备";
//  m_status = PROC_READY;
//  updateStatus();

    return true;
}


bool CEventCreatorApp::loopProcess()
{
  //如果正确执行，退出
    if(eventRun())
    {
    	#ifdef _DEBUG_
        cout << "eventRun() return true!" << endl;
      #endif
        if(m_eventConfig.m_eventorType=="0"
        	|| m_eventConfig.m_eventorType=="4"
        	|| m_eventConfig.m_eventorType=="5")
        {
        	//limeng
            //如果增量，删除增量表
//            endDeal();
//            sleep(3600);
//m_stop = true;
        }
        
        //limeng modified 20090811 全量，计算完毕后退出，
        if(m_eventConfig.m_eventorType=="1")
        {
        }
        else if(m_eventConfig.m_eventorType=="2")//否则是代收费（userid item fee）休眠若干秒
        {
            m_stop = true;
            //sleep(3600*24);
        }
        else if(m_eventConfig.m_eventorType=="3")//实时
        {
           sleep(6);       
        }
        else //其他计算完毕退出
        {
        	m_stop = true;
        }
    }
    else
    {
    	#ifdef _DEBUG_
        cout << "eventRun() return false!" << endl;
      #endif
        m_stop = true;
    }
    
    return Application::loopProcess();

}


bool CEventCreatorApp::recovery()
{
    Application::recovery();
    
    return true;
}


bool CEventCreatorApp::end()
{  

   if (m_pEventParam)
    {
        m_pEventParam->end();
        delete m_pEventParam;
        m_pEventParam = 0;
    }
   //改为短连接，在使用的地方断开数据库连接   
   //m_dbInterface.disconnect();
   //m_actDbInterface.disconnect();

    m_objectName = "程序退出";
    m_status = PROC_EXIT;
    updateStatus();
    return Application::end();
}


bool CEventCreatorApp::eventRun()
{
    T_CYCLE dealCycId, cycIdOld;
    T_CHANNEL channelNo;
    cycIdOld = 0;
    dealCycId    = 0;
    IdInfo    idInfo;
    int retrunValue = 0; 
    bool m_bEnd = true;
    bool bugpEnd = true;	//limeng 20090822 用户群事件标志
    bool bacctEnd = true;	//limeng 20090902 账户事件标志
    
    //判断用户出帐资料参数是否需要更新
    if ((*m_pEventParam).needUpdate())
    {
        //更新参数错误
        if (!(*m_pEventParam).update())
        {
            m_theErrorMessages->insert(ERROR_PARAM_UPDATING, "UserParam Update Error" );
            return true;
        }
        int i = 0;
        (*m_pEventParam).getMinRealCycId(i);
        m_minRealCycId = i; 
      
//#ifdef __PROV019__  //如果是山西	    
       //取资费绑定偏移时间add by xuf 2010-2-3 10:52:59
         m_bindTpOffsetSecond  = 0;
    	 ProcSysParam            t_sysParamInfo;
    	 if(m_pEventParam->getSysParam("EVENTOR_COMPUTE_CYC",m_channelNo,t_sysParamInfo))
    	 {
    	   m_bindTpOffsetSecond = t_sysParamInfo.m_valueN;
    	 }
//#endif
           
    }
    
    //用户出帐输入文件类初始化
    m_eventInputer.initialize(&m_eventConfig, m_pEventParam);
      
    //输出文件初始化
    m_eventOutputFile.initialize(&m_eventConfig);  
    m_ugpEventOutputFile.initialize(&m_eventConfig);  
    
    m_eventInfoCreator.initialize(m_minRealCycId, m_pEventParam);

    //limeng add 20090812 根据库号、通道号从td_userchannel取子目录
    if(m_eventConfig.m_outputRateFilePath != "")
    {
	    vector<UserChannel>	t_userChannel;
	    vector<UserChannel>::iterator iter;
	    
	    m_rateReadyFlag = true;
	    m_pEventParam->getUserChannel(m_eventConfig.m_dbNo,t_userChannel);
	    for (iter = t_userChannel.begin();iter != t_userChannel.end();iter++)
	    {
	    	m_eventOutputFile.setOutFile((*iter).m_channelNo,(*iter).m_ratePath);
	    }
	  }

//    //limeng add 20100125 epachyCode=0时取td_b_userchannel同库第一个地市
//    if(m_eventConfig.m_eparchyCode == "0")
//    {
//	    vector<UserChannel>	t_userChannel;
//	    vector<UserChannel>::iterator iter;
//	    
//	    m_pEventParam->getUserChannel(m_eventConfig.m_dbNo,t_userChannel);
//	    iter = t_userChannel.begin();
//	    m_pEventParam->setEparchyCode((*iter).m_eparchyCode);
//	    theLogger->info("epachyCode=0时取td_b_userchannel同库第一个地市:%s\n",(*iter).m_eparchyCode.c_str());
//	}

    if(m_eventConfig.m_eventorType=="0") //用户增量
    {
    	//limeng modified 20090811
//	    if(!m_compFlag)
//	    {
//	    	sleep(m_eventConfig.m_sleepTime);
//	      	return true;
//	    }
        vector<ReAccountEventInfo> t_reAccountEventVector;
        t_reAccountEventVector.clear();
        
        m_bEnd = m_eventInputer.getEventInfo(t_reAccountEventVector);
//		if(!t_reAccountEventVector.empty())
//			m_compFlag = false;
        
//        cout << "getEventInfo ok..." << endl;
//        sleep(10);
        //清理接口表
        if(!t_reAccountEventVector.empty())
        {
	        #ifdef _DEBUG_
	        cout << "清理接口表..." << endl;
	        #endif
        	endDeal(t_reAccountEventVector);
        }
        
        
        //add 2009-1-30 20:57:07
        do{
	        #ifdef _DEBUG_
	        cout << "t_reAccountEventVector.size=" << t_reAccountEventVector.size() <<endl;
	        #endif
			theLogger->info("用户U文件大小:%d\n",t_reAccountEventVector.size());
        
        	if (t_reAccountEventVector.size() != 0)
        	{
            vector<ReAccountEventInfo>::iterator iter;
                
            for (iter=t_reAccountEventVector.begin(); iter!=t_reAccountEventVector.end(); ++iter)
            {             
                #ifdef _DEBUG_
                // cout << "(*iter).m_cycId" << (*iter).m_cycId <<endl;
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;
                #endif
                //帐期小于最小帐期的直接扔掉
                if ((*iter).m_cycId != 0 && ((*iter).m_cycId < m_minRealCycId || m_dealDate.substr(0,6) < (*iter).m_cycId))
                    continue;
                    
                if (cycIdOld == 0 && (*iter).m_cycId != 0)
                    cycIdOld = (*iter).m_cycId;
                
                dealCycId = (*iter).m_cycId;
                
                //求m_acctDay江苏联通首长需求 
                int acctDay;
                int idealCycId = dealCycId;
                if(atoi(m_dealDate.substr(0,6).c_str())>idealCycId)
                {
                     int days = getMonthDays(idealCycId);
                     acctDay = idealCycId*100+days;
                }
                else
                {
                    acctDay = atoi(m_dealDate.substr(0,8).c_str());
                }
                m_eventInfoCreator.m_acctDay = acctDay;
                  
                //如果输出用户数大于系统定义输出文件的用户数
                if (cycIdOld != (*iter).m_cycId && cycIdOld != 0)
                {
                    m_eventOutputFile.closeFile();
                    if (!m_eventOutputFile.linkUnlink(dealCycId,m_dealDate))
                    {
                        #ifdef _DEBUG_
                        cout << "!m_eventOutputFile.linkUnlink false"
                             << __FILE__ << __LINE__ << endl;
                        #endif
						m_theErrorMessages->insert(ERROR_EVENT_DEAL, m_dealDate + "linkUnlink error!");
                        return false;
                    }
                }      
            
                //获取用户信息
                retrunValue = (*m_pEventParam).getUserInfo((*iter).m_userId,idInfo);
                if (retrunValue < 0)
                {
                    m_theErrorMessages->insert(ERROR_EVENT_DEAL, m_dealDate + "--Mdb deal error!");
                    #ifdef _DEBUG_
                    cout << "(*m_pEventParam).getUserInfo((*iter).m_userId,idInfo)" << endl
                             << __FILE__ << __LINE__ << endl;
                    #endif
                    return false;
                }            
                if (retrunValue == 0)
                {
                    continue;
                }
                else
                {
                    if(!isNeedAccoutUser(idInfo))
                        continue;
                        
                //added by jlxu@NeiMeng Nov18,2009  
                  if((m_eventConfig.m_eparchyCode!="0")&&(m_eventConfig.m_eparchyCode!= idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE]))
                   	{
                   		m_theErrorMessages->insert(ERROR_PARAM_EPARCHYCODE, (*iter).m_userId + "--Inconsistent Eparchycode in ORACLE and MDB!");
                   		#ifdef _DEBUG_
                   		cout <<"m_eventConfig.m_eparchyCode is:"<<m_eventConfig.m_eparchyCode<<","
                   		      <<" idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] is:"<< idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] <<endl
                   		            << __FILE__ << __LINE__ << endl;
                   		#endif
                   		continue;  //return false;					
                  }
                  //end of amendment by jlxu@NeiMeng Nov18,2009  
                    
                    (*iter).m_eparchyCode = idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] ;
                    (*iter).m_msisdn = T_MSISDN(idInfo.m_idAttr[IdInfo::ID_MSISDN]);
                    (*iter).m_openDate = T_DATE_TIME(idInfo.m_idAttr[IdInfo::ID_OPEN_DATE]);
                }
                              
                if (!m_pEventParam->getChannelNoByUserId((*iter).m_userId,(*iter).m_eparchyCode,channelNo))
                {
                    #ifdef _DEBUG_
                    cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                         <<",) return false" << __FILE__ << __LINE__ << endl;
                    #endif
                	
                	//modify 2009-3-11 23:07:39
                	m_theErrorMessages->insert(ERROR_EVENT_DEAL,
            															  (*iter).m_userId+ ": get Channel Error!" ); 
            			continue;												     
                  //  return false;
                }
                #ifdef _DEBUG_
                //cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                //     <<" channelNo=" << channelNo << endl;
                //cout << "(*iter).m_cycId=" << (*iter).m_cycId << endl;
                #endif
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;                            
                if(m_eventInfoCreator.deal(*iter))
                {
                
                    if (!m_eventOutputFile.dispatch(channelNo,dealCycId,m_dealDate,m_eventInfoCreator))
                    {
                        #ifdef _DEBUG_
                        cout << "m_eventOutputFile.dispatch" << "channelNo=" << channelNo 
                             << " dealCycId=" << dealCycId << "m_dealDate=" << m_dealDate
                             <<" return false" << __FILE__ << __LINE__ << endl;
                        #endif
                    	m_theErrorMessages->insert(ERROR_EVENT_DEAL, "m_eventOutputFile.dispatch error" );
                        
                        return false;
                    }            
                    
                    if ((*iter).m_cycId != 0)
                        cycIdOld = (*iter).m_cycId;
                }
                else
                {
                    return false;
                }
            }
              
            //输出文件关闭并清除
            m_eventOutputFile.closeFile();
            if (!m_eventOutputFile.linkUnlink(dealCycId,m_dealDate))
            {
                return false;
            }
            
            t_reAccountEventVector.clear(); //add 2009-1-30 21:01:44
           
            if (!m_bEnd) 
            	m_bEnd = m_eventInputer.getNextEventInfo(t_reAccountEventVector);
        	}
        	else
       		{
            m_objectName = "------";
            m_status = PROC_IDLE;
            updateStatus();
            sleep(m_eventConfig.m_sleepTime);
        	}
        	
      	}while(!(m_bEnd&&t_reAccountEventVector.empty()));//while
    }    
    else if(m_eventConfig.m_eventorType=="4") //用户群增量
    {
    	//limeng modified 20090811
//	    if(!m_compFlag)
//	    {
//	    	sleep(m_eventConfig.m_sleepTime);
//	      	return true;
//	    }
        vector<ReAccountEventInfo> t_reAccountUgpEventVector;
        t_reAccountUgpEventVector.clear();
        
        m_bEnd = m_eventInputer.getUgpEventInfo(t_reAccountUgpEventVector);
//		if(!t_reAccountUgpEventVector.empty())
//			m_compFlag = false;
        
//        cout << "getEventInfo ok..." << endl;
//        sleep(10);
        //清理接口表
        if(!t_reAccountUgpEventVector.empty())
        {
	        #ifdef _DEBUG_
	        cout << "清理接口表..." << endl;
	        #endif
        	endDeal(t_reAccountUgpEventVector);
        }
        
      	//limeng add 20090813 处理用户群
         do{
	        #ifdef _DEBUG_
	        cout << "t_reAccountUgpEventVector.size=" << t_reAccountUgpEventVector.size() <<endl;
	        #endif
			theLogger->info("用户群U文件大小:%d\n",t_reAccountUgpEventVector.size());
        
        	if (t_reAccountUgpEventVector.size() != 0)
        	{
            vector<ReAccountEventInfo>::iterator iter;
                
            for (iter=t_reAccountUgpEventVector.begin(); iter!=t_reAccountUgpEventVector.end(); ++iter)
            {             
                #ifdef _DEBUG_
                // cout << "(*iter).m_cycId" << (*iter).m_cycId <<endl;
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;
                #endif
                //帐期小于最小帐期的直接扔掉
                if ((*iter).m_cycId != 0 && ((*iter).m_cycId < m_minRealCycId || m_dealDate.substr(0,6) < (*iter).m_cycId))
                    continue;
                    
                if (cycIdOld == 0 && (*iter).m_cycId != 0)
                    cycIdOld = (*iter).m_cycId;
                
                dealCycId = (*iter).m_cycId;
                
                //求m_acctDay江苏联通首长需求 
                int acctDay;
                int idealCycId = dealCycId;
                if(atoi(m_dealDate.substr(0,6).c_str())>idealCycId)
                {
                     int days = getMonthDays(idealCycId);
                     acctDay = idealCycId*100+days;
                }
                else
                {
                    acctDay = atoi(m_dealDate.substr(0,8).c_str());
                }
                m_eventInfoCreator.m_acctDay = acctDay;
                  
                //如果输出用户数大于系统定义输出文件的用户数
                if (cycIdOld != (*iter).m_cycId && cycIdOld != 0)
                {
                    m_eventOutputFile.ugpCloseFile();
                    if (!m_eventOutputFile.ugpLinkUnlink(dealCycId,m_dealDate))
                    {
                        #ifdef _DEBUG_
                        cout << "!m_eventOutputFile.ugpLinkUnlink false"
                             << __FILE__ << __LINE__ << endl;
                        #endif
                    	m_theErrorMessages->insert(ERROR_EVENT_DEAL, "m_eventOutputFile.ugpLinkUnlink false");
                        return false;
                    }
                }      
            
                //获取用户信息
                retrunValue = (*m_pEventParam).getUserInfo((*iter).m_userId,idInfo);
                if (retrunValue < 0)
                {
                    m_theErrorMessages->insert(ERROR_EVENT_DEAL, m_dealDate + "--Mdb deal error!");
                    #ifdef _DEBUG_
                    cout << "(*m_pEventParam).getUserInfo((*iter).m_userId,idInfo)" << endl
                             << __FILE__ << __LINE__ << endl;
                    #endif
                    return false;
                }            
                if (retrunValue == 0)
                {
                    continue;
                }
                else
                {
                    if(!isNeedAccoutUser(idInfo))
                        continue;
                        
                //added by jlxu@NeiMeng Nov18,2009  
                  if((m_eventConfig.m_eparchyCode!="0")&&(m_eventConfig.m_eparchyCode!= idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE]))
                   	{
                   		m_theErrorMessages->insert(ERROR_PARAM_EPARCHYCODE, (*iter).m_userId + "--Inconsistent Eparchycode in ORACLE and MDB!");
                   		#ifdef _DEBUG_
                   		cout <<"m_eventConfig.m_eparchyCode is:"<<m_eventConfig.m_eparchyCode<<","
                   		      <<" idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] is:"<< idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] <<endl
                   		            << __FILE__ << __LINE__ << endl;
                   		#endif
                   		continue;  //return false;					
                  }
                  //end of amendment by jlxu@NeiMeng Nov18,2009  
                    
                    
                    (*iter).m_eparchyCode = idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] ;
                    (*iter).m_msisdn = T_MSISDN(idInfo.m_idAttr[IdInfo::ID_MSISDN]);
                    (*iter).m_openDate = T_DATE_TIME(idInfo.m_idAttr[IdInfo::ID_OPEN_DATE]);
                }
                              
                if (!m_pEventParam->getChannelNoByUserId((*iter).m_userId,(*iter).m_eparchyCode,channelNo))
                {
                    #ifdef _DEBUG_
                    cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                         <<",) return false" << __FILE__ << __LINE__ << endl;
                    #endif
                	
                	//modify 2009-3-11 23:07:39
                	m_theErrorMessages->insert(ERROR_EVENT_DEAL,
            															  (*iter).m_userId+ ": get Channel Error!" ); 
            			continue;												     
                  //  return false;
                }
                #ifdef _DEBUG_
                //cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                //     <<" channelNo=" << channelNo << endl;
                //cout << "(*iter).m_cycId=" << (*iter).m_cycId << endl;
                #endif
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;                            
                if(m_eventInfoCreator.deal(*iter))
                {
                
                    if (!m_eventOutputFile.ugpDispatch(channelNo,dealCycId,m_dealDate,m_eventInfoCreator))
                    {
                        #ifdef _DEBUG_
                        cout << "m_eventOutputFile.ugpDispatch" << "channelNo=" << channelNo 
                             << " dealCycId=" << dealCycId << "m_dealDate=" << m_dealDate
                             <<" return false" << __FILE__ << __LINE__ << endl;
                        #endif
                		m_theErrorMessages->insert(ERROR_EVENT_DEAL,"ugpDispatch error!" ); 
                        return false;
                    }            
                    
                    if ((*iter).m_cycId != 0)
                        cycIdOld = (*iter).m_cycId;
                }
                else
                {
                    return false;
                }
            }
              
            //输出文件关闭并清除
            m_eventOutputFile.ugpCloseFile();
            if (!m_eventOutputFile.ugpLinkUnlink(dealCycId,m_dealDate))
            {
                return false;
            }
            
            t_reAccountUgpEventVector.clear(); //add 2009-1-30 21:01:44
           
            if (!m_bEnd) 
            	m_bEnd = m_eventInputer.getNextUgpEventInfo(t_reAccountUgpEventVector);
        	}
        	else
       		{
            m_objectName = "------";
            m_status = PROC_IDLE;
            updateStatus();
            sleep(m_eventConfig.m_sleepTime);
        	}
        	
      	}while(!(m_bEnd&&t_reAccountUgpEventVector.empty()));//while
		
    }    
    else if(m_eventConfig.m_eventorType=="5") //账户增量
    {
    	//limeng modified 20090811
//	    if(!m_compFlag)
//	    {
//	    	sleep(m_eventConfig.m_sleepTime);
//	      	return true;
//	    }
        vector<ReAccountEventInfo> t_reAccountAcctEventVector;
        t_reAccountAcctEventVector.clear();
        
        m_bEnd = m_eventInputer.getAcctEventInfo(t_reAccountAcctEventVector);
//		if(!t_reAccountAcctEventVector.empty())
//			m_compFlag = false;
        
//        cout << "getEventInfo ok..." << endl;
//        sleep(10);
        //清理接口表
        if(!t_reAccountAcctEventVector.empty())
        {
	        #ifdef _DEBUG_
	        cout << "清理接口表..." << endl;
	        #endif
        	endDeal(t_reAccountAcctEventVector);
        }
        
      	//limeng add 20090901 处理账户
         do{
	        #ifdef _DEBUG_
	        cout << "t_reAccountAcctEventVector.size=" << t_reAccountAcctEventVector.size() <<endl;
	        #endif
			theLogger->info("账户U文件大小:%d\n",t_reAccountAcctEventVector.size());
        
        	if (t_reAccountAcctEventVector.size() != 0)
        	{
            vector<ReAccountEventInfo>::iterator iter;
                
            for (iter=t_reAccountAcctEventVector.begin(); iter!=t_reAccountAcctEventVector.end(); ++iter)
            {             
                #ifdef _DEBUG_
                // cout << "(*iter).m_cycId" << (*iter).m_cycId <<endl;
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;
                #endif
                //帐期小于最小帐期的直接扔掉
                if ((*iter).m_cycId != 0 && ((*iter).m_cycId < m_minRealCycId || m_dealDate.substr(0,6) < (*iter).m_cycId))
                    continue;
                    
                if (cycIdOld == 0 && (*iter).m_cycId != 0)
                    cycIdOld = (*iter).m_cycId;
                
                dealCycId = (*iter).m_cycId;
                
                //求m_acctDay江苏联通首长需求 
                int acctDay;
                int idealCycId = dealCycId;
                if(atoi(m_dealDate.substr(0,6).c_str())>idealCycId)
                {
                     int days = getMonthDays(idealCycId);
                     acctDay = idealCycId*100+days;
                }
                else
                {
                    acctDay = atoi(m_dealDate.substr(0,8).c_str());
                }
                m_eventInfoCreator.m_acctDay = acctDay;
                  
                //如果输出用户数大于系统定义输出文件的用户数
                if (cycIdOld != (*iter).m_cycId && cycIdOld != 0)
                {
                    m_eventOutputFile.acctCloseFile();
                    if (!m_eventOutputFile.acctLinkUnlink(dealCycId,m_dealDate))
                    {
                        #ifdef _DEBUG_
                        cout << "!m_eventOutputFile.acctLinkUnlink false"
                             << __FILE__ << __LINE__ << endl;
                        #endif
                    	m_theErrorMessages->insert(ERROR_EVENT_DEAL, "m_eventOutputFile.acctLinkUnlink false!");
                        return false;
                    }
                }      
            
                //获取账户信息
                retrunValue = (*m_pEventParam).getAcctInfo((*iter).m_userId,idInfo);
                if (retrunValue < 0)
                {
                    m_theErrorMessages->insert(ERROR_EVENT_DEAL, (*iter).m_userId + "--Mdb deal error!");
                    #ifdef _DEBUG_
                    cout << "(*m_pEventParam).getUserInfo((*iter).m_userId,idInfo)" << endl
                             << __FILE__ << __LINE__ << endl;
                    #endif
                    return false;
                }            
                if (retrunValue == 0)
                {
                    continue;
                }
                else
                {
                    if(!isNeedAccoutUser(idInfo))
                        continue;
					//limeng modified 20090811                        
//					if(m_eventConfig.m_eparchyCode != "0")
//					{                       
//	                    if(!isNeedAccoutEparchyCode(dealCycId,idInfo))
//	                        continue;
//                    }
 
                //added by jlxu@NeiMeng Nov18,2009  
                  if((m_eventConfig.m_eparchyCode!="0")&&(m_eventConfig.m_eparchyCode!= idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE]))
                   {
                   		m_theErrorMessages->insert(ERROR_PARAM_EPARCHYCODE, (*iter).m_userId + "--Inconsistent Eparchycode in ORACLE and MDB!");
                   		#ifdef _DEBUG_
                   		cout <<"m_eventConfig.m_eparchyCode is:"<<m_eventConfig.m_eparchyCode<<","
                   		      <<" idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] is:"<< idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] <<endl
                   		            << __FILE__ << __LINE__ << endl;
                   		#endif
                   		continue;  //return false;					
                  }
                  //end of amendment by jlxu@NeiMeng Nov18,2009  
                    
                    (*iter).m_eparchyCode = idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] ;
                    (*iter).m_msisdn = T_MSISDN(idInfo.m_idAttr[IdInfo::ID_MSISDN]);
                    (*iter).m_openDate = T_DATE_TIME(idInfo.m_idAttr[IdInfo::ID_OPEN_DATE]);
                }
                              
                if (!m_pEventParam->getChannelNoByUserId((*iter).m_userId,(*iter).m_eparchyCode,channelNo))
                {
                    #ifdef _DEBUG_
                    cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                         <<",) return false" << __FILE__ << __LINE__ << endl;
                    #endif
                	
                	//modify 2009-3-11 23:07:39
                	m_theErrorMessages->insert(ERROR_EVENT_DEAL,
            															  (*iter).m_userId+ ": get Channel Error!" ); 
            			continue;												     
                  //  return false;
                }
                #ifdef _DEBUG_
                //cout << "m_pEventParam->getChannelNoByUserId("<< (*iter).m_userId << ","<<(*iter).m_eparchyCode
                //     <<" channelNo=" << channelNo << endl;
                //cout << "(*iter).m_cycId=" << (*iter).m_cycId << endl;
                #endif
                //cout << "iter-> m_monthEndTag:" << iter-> m_monthEndTag << endl;                            
                if(m_eventInfoCreator.deal(*iter))
                {
                
                    if (!m_eventOutputFile.acctDispatch(channelNo,dealCycId,m_dealDate,m_eventInfoCreator))
                    {
                        #ifdef _DEBUG_
                        cout << "m_eventOutputFile.acctDispatch" << "channelNo=" << channelNo 
                             << " dealCycId=" << dealCycId << "m_dealDate=" << m_dealDate
                             <<" return false" << __FILE__ << __LINE__ << endl;
                        #endif
                	m_theErrorMessages->insert(ERROR_EVENT_DEAL,"m_eventOutputFile.acctDispatch error!");
                        return false;
                    }            
                    
                    if ((*iter).m_cycId != 0)
                        cycIdOld = (*iter).m_cycId;
                }
                else
                {
                    return false;
                }
            }
 