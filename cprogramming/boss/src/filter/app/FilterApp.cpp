// ############################################
// Source file : FilterApp.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : shenglj
// E_mail      : shenglj@lianchuang.com
// Create      : 2008-7-7
// Update      : 2008-7-7
// Copyright(C): shenglj, Linkage.
// ############################################

#include <unistd.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#include "FilterApp.h"

//-------------------------------------------------------------------------//
// application object, which will replaced by your application
//-------------------------------------------------------------------------//
FilterApp g_application;
Application * const theApp = &g_application;

/*
PM_Filter theParam;
PM_Filter  * const g_param = &theParam;

SortLog g_SortLog;
SortLog * const theSortLog = &g_SortLog;

extern map<string,string> g_BindMethod;  //绑定方式

*/


FilterApp::FilterApp()
{
}

FilterApp::~FilterApp()
{

}

bool FilterApp::processopt(int optopt, const char *optarg)
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
    // process opt
    bool rc = true;
    switch (optopt)
    {
            //-----------------------------------------------------------------------//
            // TODO: Add your specialized option process code here
            // case 'f':
            //   isFileDefiend = true;
            //   filename = optarg;
            //   break;
            //-----------------------------------------------------------------------//

        case 'c':
        case ':':
        case '?':
        default:
            rc = Application::processopt(optopt, optarg);
    }
    return rc;
}

bool FilterApp::initialization()
{

    //-----------------------------------------------------------------------//
    // call setoptstr before initialization()
    // set optstring -- reference unix getopt() call
    // the 'c' option is reserved for channel no option,
    // you can add your owner option
    //-----------------------------------------------------------------------//
    setoptstr("c:");

    //SET_VERINFO();
    //setVersion(g_versionInfo);

    // normally, call standard initialization first
    if (!Application::initialization())
        return false;

  //// 取配置文件参数
  if (!m_config.getSysParam(m_theSysParam,m_name,m_channelNo)) 
  {
    theErrorMessages->insert(EAPPFRM_SYSVAL,"read configuration fail");
    return false;
  }
  #ifdef _DEBUG_
    //m_config.dump();
  #endif
    // set application run method if need, all default is false
    #ifndef FP_DEBUG
        set_asdaemon(true);
        set_onlyone(true);
        set_runinbkg(true);
    #endif



                              
    return true;
}

bool FilterApp::beforeLoop()
{
	if (!Application::beforeLoop())
	{
		return false;
	}

  //各过程处理信息初始化
  if (!m_globalInfo.initialization(&m_config,&m_sortLog,&m_pmGlobalManager))
  {
    return false;
  }
  //输入信息初始化
  if (!m_inputercdr.initialize(&m_globalInfo,&m_config.m_inputRuleMgr))
  {
    return false;
  }

   //输出信息初始化
  if (!m_outputercdr.initialize(&m_globalInfo,&m_config.m_outputRuleMgr))
  {
    return false;
  }

   //处理流程信息初始化
  if (!m_processor.initialize(&m_globalInfo))
  {
    return false;
  }

   //参数管理初始化
   if (!m_pmGlobalManager.initialize(&m_config,0))
  {
    return false;
  }

  //初始化脚本处理类
  if (!CDRJScript::Initialize(&(m_globalInfo.m_cdr))) 
  {
    string errorMsg = CDRJScript::GetContainer()->getErrorInfo();
    theErrorMessages->insert(EAPPFRM_SYSVAL,"initialize script fail:" + errorMsg);
    return false;
  }
    
    return true;

}

bool FilterApp::end()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and call the base class
    //-----------------------------------------------------------------------//

    return Application::end();
}

bool FilterApp::recovery()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//

    // TO BE DETERMINED
    Application::recovery();
    return true;
}

bool FilterApp::loopProcess()
{
    if(!filterRun())
    {
       m_stop = true;
    }
    return Application::loopProcess();

}

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//

/*
void FilterApp::writeLog()
{

    
}
*/

bool FilterApp::begin()
{
  m_globalInfo.clearDealingNum();
  m_objectName = m_globalInfo.m_fileName.c_str();
  m_status = PROC_RUN;
  updateStatus();

  //1 日志开始
  m_sortLog.begin();

  //2 输出流
  if(!m_outputercdr.open())
  {
    m_outputercdr.close();
    return false;
  }
  //3 输入流
  if(!m_inputercdr.open())
  {
    m_inputercdr.close();
    m_outputercdr.close();
    return false;
  }

  return true;
}

bool FilterApp::commit()
{
  m_inputercdr.close();
  m_outputercdr.close();
  if(m_sortLog.m_ferrCdrs>0) m_ferroutcdr.close();

  if(!m_outputercdr.fileCommit())
  {
     return false;
  }

  if(!m_inputercdr.fileCommit())
  {
     return false;
  }


  //日志结束
  m_sortLog.commit();

  return true;
}

bool FilterApp::rollback()
{
  m_inputercdr.close();
  m_outputercdr.close();
  if(m_sortLog.m_ferrCdrs>0) m_ferroutcdr.close();

  if(!m_outputercdr.fileRollback())
  {
     return false;
  }
  return true;
}

int FilterApp::filterRun()
{
  int     t_iRet=0;
  PString t_fileName;

  ///1.参数更新
  if( m_pmGlobalManager.needUpdate())
  {
      m_objectName = "更新参数中...";
      m_status     = PROC_RUN;
      updateStatus();
     if (!m_pmGlobalManager.update())
      {
          theErrorMessages->insert(FILTER_ERROR_DBUPDATE," m_pmGlobalManager update error!" );
          sleep(SLEEP_TIME);
          return true;
      }
      m_objectName = "更新参数完成";
      updateStatus();
  }

  ///2. 获得待处理文件
  t_iRet = m_inputercdr.getFile(t_fileName);
  if(t_iRet == -2)//配置获取失败
  {
//cout<<"配置获取失败  "<<__FILE__<<__LINE__<<endl;
    m_objectName.clear();
    m_status = PROC_IDLE;
    updateStatus();
    theErrorMessages->insert(E_GET_INPUT_PARAM,"read m_inputRule config fail");
    return false;
  }
  else if(t_iRet == -1)//处理文件超过20 or没有遍历完 切换目录
  {
//cout<<"已处理20个，切换目录  "<<__FILE__<<__LINE__<<endl;
     m_objectName = "------";
     m_status = PROC_IDLE;
     updateStatus();
     return true;
  }
  else if(t_iRet == 0)//空闲
  { 
//cout<<"目录空闲  "<<__FILE__<<__LINE__<<endl;
     m_objectName = "------";
     m_status = PROC_IDLE;
     updateStatus();
     sleep(SLEEP_TIME);
     return true;
  }

  ///3. 文件获取到，处理准备
  if(!begin())
  {
     return true;
  }
 #ifdef FP_DEBUG
   FP_BEGIN(fileRun)
 #endif

 #ifdef FP_DEBUG
   FP_BEGIN(cdr_read)
 #endif
  ///4. 读取有效话单
  while (m_inputercdr.getCdr()>0) 
  {
    ///5. m_cdr记录清空
    m_globalInfo.reset();

    //6. 解析检验话单格式是否与配置一致
    if(m_inputercdr.formatCdr() < 0)
    {
    	 /*
       if(m_inputercdr.moveErrFile()<0)
       {
   	       #ifdef _DEBUG_
             cout<<"移动 '"<<t_fileName<<"' 至分拣无效目录失败！ "<<__FILE__<<__LINE__<<endl;
   	       #endif
          theErrorMessages->insert(E_MOVE_FILE_ERR,"remove "+ t_fileName +" invaid directory fail!");
          if(!rollback())
          {
          	return false;
          }
          return false;
       }
       */
       #ifdef _DEBUG_
         cout<<t_fileName<<"话单中第 "<<m_globalInfo.m_totalCdrNum+1<<" 行字段数校验错误！ "<<__FILE__<<__LINE__<<endl;
       #endif
       theErrorMessages->insert(E_CDR_FIELD_NUMBER,t_fileName + "第 "+StringUtil::toString(m_globalInfo.m_totalCdrNum+1) +" 行 cdr field number define error!");
       //由于单条记录个数不完整引起的整个话单被丢弃机制，重新调整，将该记录单独输出 add by shenglj 2010-10-25 16:10:01
       if(!writeFerrCdr())
       {
         return false;
       }
       else
       {
       	 //写ferr错单后继续处理
         continue;
       }
       /*
			 if(!rollback())
       {
       	 return false;
       }
       */
       return true;
    }

     //m_cdr读入话单后初始化
      m_globalInfo.canonicalize();

 #ifdef FP_DEBUG
   FP_BEGIN(process)
 #endif
     ///7. m_cdr记录处理
     if(m_processor.process()<0)
     {
			 if(!rollback())
       {
       	 return false;
       }
	     return false;
     }
 #ifdef FP_DEBUG
   FP_END(process)
 #endif

     ///8. 与MDB的链接是否存在过异常，告警提示
	   if(m_config.m_inputRuleMgr.m_isNeeduseMdb == 'Y' || m_config.m_inputRuleMgr.m_isNeeduseMdb == 'y')
	   {
       string  r_alarmstr;
	   	 if(m_pmGlobalManager.MdbNotify(m_mdbErrorMsg))
	   	 {
	        #ifdef _DEBUG_
	          cout<<"file:"<<m_globalInfo.m_cdr[ATTRI_FILE_NO]<<" msisdn:"<<m_globalInfo.m_cdr[ATTRI_MSISDN_A]<<" starttime:"
	              <<m_globalInfo.m_cdr[ATTRI_BEGIN_DATE]+m_globalInfo.m_cdr[ATTRI_BEGIN_TIME]<<m_mdbErrorMsg<<__FILE__<<__LINE__<<endl;
	          cout<<m_globalInfo.m_cdr[ATTRI_FILE_NO]<<m_mdbErrorMsg<<__FILE__<<__LINE__<<endl;
	        #endif
	       r_alarmstr = string("file:")+m_globalInfo.m_cdr[ATTRI_FILE_NO].c_str()+" msisdn:" + m_globalInfo.m_cdr[ATTRI_MSISDN_A].c_str()+
	       " starttime:" + m_globalInfo.m_cdr[ATTRI_BEGIN_DATE].c_str()+m_globalInfo.m_cdr[ATTRI_BEGIN_TIME].c_str()+m_mdbErrorMsg;
	       theErrorMessages->insert(FILTER_ERROR_SEARCH_MDB,r_alarmstr);
	   	 }
	   }

 #ifdef FP_DEBUG
   FP_BEGIN(cdr_output)
 #endif

     //m_cdr话单输出前规整
      m_globalInfo.finalize();

     ///9. m_cdr记录输出
     if(!m_outputercdr.output())
     {
       theErrorMessages->insert(EAPPFRM_SYSVAL,"m_cdrrrrrrr 记录输出失败! " + t_fileName);
			 if(!rollback())
       {
       	 return false;
       }
	     return false;
     }
 #ifdef FP_DEBUG
   FP_END(cdr_output)
 #endif
  }

 #ifdef FP_DEBUG
   FP_END(cdr_read)
 #endif

   ///10. 与MDB的链接是否存在过异常，存在，回退
   if(m_config.m_inputRuleMgr.m_isNeeduseMdb == 'Y' || m_config.m_inputRuleMgr.m_isNeeduseMdb == 'y')
   {
   	 if(m_pmGlobalManager.MdbNotify(m_mdbErrorMsg))
   	 {
        #ifdef _DEBUG_
          cout<<m_mdbErrorMsg<<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_SEARCH_MDB,m_mdbErrorMsg);
	      if(!rollback())
	      {
	      	return false;
	      }
	      return false;
   	 }
   }

  ///11. 话单提交
  if(commit())
  {
 #ifdef FP_DEBUG
   FP_END(fileRun)
 #endif
    return true;
  }
  theErrorMessages->insert(EAPPFRM_SYSVAL,"m_cdrrrrrrr 话单提交失败!" + t_fileName);
  return false;
}

bool FilterApp::writeFerrCdr()
{
  if(!m_ferroutcdr.is_open())
  {
  	m_ferroutcdr.setName(m_globalInfo.m_fileName.c_str(),m_config.m_errorPath.c_str());
  	m_ferroutcdr.open(ios::out);
  }

  if(!m_ferroutcdr.write(m_globalInfo.m_cdr.m_orgBuff))
  {
    theErrorMessages->insert(EAPPFRM_SYSVAL, m_config.m_errorPath + m_globalInfo.m_fileName + "ferr话单写失败!");
  	return false;
  }

  //记录结束加入换行符
  PString r_lineDeliminater = "\n";
  m_ferroutcdr.write(r_lineDeliminater.c_str());

  m_sortLog.addFerrCdr();
  m_globalInfo.addDealingNum();

  return true;
}