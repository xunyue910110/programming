// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// Editors:     Yang Zhang  <zhangy@lianchuang.com>
// Update       20020325
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
//
// Comments:
//




#include "MyApplication.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"

#include <assert.h>
#include <unistd.h>




//-------------------------------------------------------------------------//
// application object, which will replaced by your application
//-------------------------------------------------------------------------//
MyApplication g_application;
Application * const theApp = &g_application;


bool MyApplication::processopt(int optopt, const char *optarg)
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


//##ModelId=3BCAA280003A
bool MyApplication::initialization()
{
   char channelNo[4];
   float theErrorRate;
   vector <string> pathMessage;
   string root,ppPath,section,name,value;
   string switchName,isBackupDes,isBackupSrc,isCheckCdrNo;
   string retlogType;

   //-----------------------------------------------------------------------//
   // call setoptstr before initialization()
   // set optstring -- reference unix getopt() call
   // the 'c' option is reserved for channel no option,
   // you can add your owner option
   //-----------------------------------------------------------------------//
   setoptstr("c:");

   // normally, call standard initialization first
   if (!Application::initialization())
   	return false;

	m_RetLog_ = RetLog::getInstance();
	if ( m_RetLog_ == 0 ) {
	   m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create RetLog object!");
		return false;
	}

   // set application run method if need, all default is false
#ifndef _DEBUG_
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif
   //-----------------------------------------------------------------------//
   // ADD YOUR INITIALIZATION CODE HERE
   //-----------------------------------------------------------------------//

   setFileSortMethod(Directory::SortFlags (1) );
   setMaxScan(20000);
   
   root=SECTDELIM+string(m_name);// \pp
   name="switch_name";
   m_RetApp=0;
   sprintf(channelNo,"%d",m_channelNo);
   ppPath = root + SECTDELIM + channelNo ;// \pp\1

   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   switchName = value;

   name="retfile_type";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_retFileType = atoi(value.c_str()); 

   name="retlog_type";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   retlogType = value;  

   name="error_rate";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   theErrorRate=atof(value.c_str());

   name="backup_srcfile";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   isBackupSrc=value;
   
   name="backup_desfile";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   isBackupDes=value;
   
   // get the SOURCE_TYPE's value for the process's log,add by chenm 2005-8-3 
   name="SOURCE_TYPE";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_RetLog_->setSourceType(value);

   name="check_cdrNo";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   isCheckCdrNo=value;

   ppPath = root + SECTDELIM + channelNo ;
   if( !getPath(ppPath,pathMessage) ) {
      m_theErrorMessages->insert(PP_ERROR_GET_CONFIG,"cannot get path's parameter! ");
      return false;
   }
   //setPathIn(m_sourcePath.c_str());
   //setPathIn(m_retlogPath.c_str());

   m_RetApp = new RetApp(channelNo,0); 
   
   if( strcmp(retlogType.c_str(),"pp")== 0 )
      m_RetLogRecord = new PpRetLogCdr();
      //return true;  

   if(m_RetApp==0) {
      m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT,"cannot create RetApp Object!");
      return false;
   }

	if ( m_RetApp->initialize(switchName,pathMessage,theErrorRate,
	                                      isBackupSrc,isBackupDes,isCheckCdrNo) == false ){
	   m_theErrorMessages->insert(PP_ERROR_INITIALIZE,"cannot initialize RetApp!");
		return false;
	}

   m_filecount=1; // 满足m_proccount>=m_filecount的条件       // 070308   gaoxl
   m_proccount=1;

   return true;
}


//##ModelId=3BCAA28000B2
bool MyApplication::end()
{
   //-----------------------------------------------------------------------//
   // TODO: Add your specialized code here and call the base class
   //-----------------------------------------------------------------------//
   return Application::end();
}


//##ModelId=3BCAA2800076
bool MyApplication::recovery()
{
   //-----------------------------------------------------------------------//
   // TODO: Add your specialized code here and/or call the base class
   //-----------------------------------------------------------------------//

   // TO BE DETERMINED
   Application::recovery();
   return true;
}


//##ModelId=3BCAA28000C6
bool MyApplication::loopProcess()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
    char fileNameTmp[80+1];
    string srcFileName,errorType,errorCode,retHead,retTail;
    string sTemp;
    string fullLogName;
    char cmd[100];
    int iStatus=0;
	  memset(fileNameTmp,0,sizeof(fileNameTmp));
	  ifstream fpRetLog;
	  int iReturn=0;
	  bool bReturn = false;

    theLoggerProcessID = theLogger->pBegin() ;    
    if(m_retFileType==2)
    {
    	setPathIn(m_retlogPath.c_str());
    	if(getFile(fileNameTmp,80))
	    {
		    fullLogName = m_retlogPath+fileNameTmp;
    	  
    	  m_status = PROC_RUN;
    	  m_objectName = fileNameTmp;
    	  updateStatus();
        
        //日志处理初始化

        fpRetLog.open((char *)fullLogName.c_str());
        
        while(1)
        {
        	if(fpRetLog.eof())
        		break;
        	getline(fpRetLog,sTemp);
        	if(sTemp.size()==0)
        		break;
        	
        	iReturn = m_RetLogRecord->getLogInfo(&sTemp);
        }
        if(iReturn>0)
        {
          srcFileName=m_RetLogRecord->m_retInfo.ret_name;
          errorType=m_RetLogRecord->m_retInfo.error_type;
          errorCode=m_RetLogRecord->m_retInfo.error_code;
          retHead=m_RetLogRecord->m_retInfo.ret_head;
          retTail=m_RetLogRecord->m_retInfo.ret_tail;
          
          setPathIn(m_sourcePath.c_str());
          bReturn = m_dir.file_matches(srcFileName.c_str(),"*");
          if(bReturn)
          {
             if ( m_RetApp->mainProcess(m_retFileType,m_sourcePath,srcFileName,
             	                          errorType,errorCode,retHead,retTail) < 0 ) 
    	       {                                                    //run error;
    	       	writeAlert();
    	       	m_status = PROC_WARN;
    	       	m_objectName = "file " + srcFileName + " error!";
    	       	writeLog();
    	       	updateStatus();
    	       	return Application::loopProcess();
    	       }
          }
        }
    	  
    	  fpRetLog.close();
    	  unlink(fullLogName.c_str());
    	  
    	  writeAlert();
    	  writeLog();
      }
    }
    else if(m_retFileType==1)
    {
    	 setPathIn(m_sourcePath.c_str());
    	 if(getFile(fileNameTmp,80))
    	 {
    	   srcFileName=fileNameTmp;
 	   
    	   m_status = PROC_RUN;
    	   m_objectName = srcFileName;
    	   updateStatus();

    	   errorType="";
    	   errorCode="";
    	   retHead="";
    	   retTail="";
    	   if ( m_RetApp->mainProcess(m_retFileType,m_sourcePath,srcFileName,
               	                    errorType,errorCode,retHead,retTail) < 0 ) 
         {                                                    //run error;
    	      writeAlert();
    	      m_status = PROC_WARN;
    	      m_objectName = "file " + srcFileName + " error!";
    	      writeLog();
    	      updateStatus();
    	      return Application::loopProcess();
    	   }
    	   writeAlert();
    	   writeLog();
    	  
    	 }
    }    	 
    theLogger->pEnd(theLoggerProcessID); 

    m_status = PROC_IDLE;
    m_objectName = "";
    updateStatus();
    sleep(SLEEP_TIME);
    return Application::loopProcess();
}

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//

void MyApplication::writeLog()
{
	char Tmp[256];
	
	//sprintf(Tmp,"SOURCE_TYPE=%s,Error=%d,SUM(01)=%d,SUM(02)=%d,SUM(03)=%d,SUM(07)=%d,SUM(08)=%d,SUM(11)=%d,SUM(OTHER)=%d,delay1=%s,delay2=%s,delay3=%d,delay4=%d,delay5=%d,delay6=%d,delay7=%d",
	//             m_RetLog_->getErrorSum() ,
	//             m_RetLog_->getMocSum(),
	//             m_RetLog_->getMtcSum(),
	//             m_RetLog_->getForwSum(),
	//             m_RetLog_->getSmocSum(),
	//             m_RetLog_->getSmtcSum(),
	//             m_RetLog_->getPocSum(),
	//             m_RetLog_->getOtherSum(),
	//             //m_RetLog_->getDelaySum1(),
	//             //m_RetLog_->getDelaySum2(),
	//             (m_RetLog_->getMinStartDateTime()).c_str(),
	//             (m_RetLog_->getMaxStartDateTime()).c_str(),
	//             //min_StartDateTime,
	//             //max_StartDateTime,
	//             //m_RetLog_->getDelaySum3(),
	//             //m_RetLog_->getDelaySum4(),
	//             m_RetLog_->getBeginCdrNo(),
	//             m_RetLog_->getEndCdrNo(),
	//             //m_RetLog_->getDelaySum5(), modified by chenm 2006-4-28 增加输出文件的大小
	//             m_RetLog_->getFileLenth(),
	//             m_RetLog_->getDelaySum6(),
	//             m_RetLog_->getDelaySum7()
	//             );
	string sourceFileNameWithPath = m_sourcePath+m_RetLog_->getSrcFileName();
	string desFileNameWithPath = m_desPath + m_RetLog_->getSrcFileName();//zaiww added 20060518
	theLogger->pInput(theLoggerProcessID,sourceFileNameWithPath,m_RetLog_->getFileLenth());
	theLogger->pOutput(theLoggerProcessID,desFileNameWithPath,m_RetLog_->getTotalCdrs());        
}

MyApplication::~MyApplication()
{
   if(m_RetApp!=0)
      delete m_RetApp;
}

bool MyApplication::getPath(string path,vector <string> &pathMessage)
{
   string mypath,value,name;

   mypath=path+SECTDELIM+"path";
   name="source_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   m_sourcePath =value;
   name+=" "+value;
   pathMessage.push_back(name);
   name="src_backup_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   name+=" "+value;
   pathMessage.push_back(name);
   name="destination_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   m_desPath = value;//zaiww added 20060518
   name+=" "+value;
   pathMessage.push_back(name);
   name="des_backup_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   name+=" "+value;
   pathMessage.push_back(name);
   name="file_error_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   name+=" "+value;
   pathMessage.push_back(name);
   
   name="ret_log_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   m_retlogPath =value;
   name+=" "+value;
   pathMessage.push_back(name);
   
   return true;
}


