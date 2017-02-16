// ##########################################
// Source file : MyApplication..cpp
// System      : NEW GENERATION BILLING SYSTEM
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Zhao Liyang
// E_mail      : zhaoly@lianchuang.com
// Create      : 20081018
// Update      : 20081029
// Copyright(C): 2008 by Zhao Liyang, Linkage.
// ##########################################

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
   bool bCheck_Filename = false;
   float theErrorRate;
   vector <string> pathMessage;
   string root,ppPath,section,name,value;
   string switchName,isBackupDes,isBackupSrc,isIndb,isCheckCdrNo;

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

	 m_SendLog_ = SendLog::getInstance();
	 if ( m_SendLog_ == 0 ) {
	    m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create SendLog object!");
	 	return false;
	 }

   // set application run method if need, all default is false
   
   #ifndef _DEBUG_
       set_asdaemon(true);
       set_onlyone(true);
       set_runinbkg(true);
   #endif
   
   
   setFileSortMethod(Directory::SortFlags (1) );
   //setMaxScan(20000);
   
   root=SECTDELIM+string(m_name);// \pp
   
   name="switch_name";
   m_SendApp=0;
   sprintf(channelNo,"%d",m_channelNo);
   ppPath = root + SECTDELIM + channelNo ;// \pp\1

   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   switchName = value;
   
   //m_fileType于头文件中定义
   name="file_type";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_fileType=value;   

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

   name="need_indbfile";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   isIndb=value;
      
   // get the SOURCE_TYPE's value for the process's log,add by chenm 2005-8-3 
   name="SOURCE_TYPE";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_SendLog_->setSourceType(value);

   name="check_cdrNo";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   isCheckCdrNo=value;
   
   name="head_or_tail";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_isHeadOrTail=atoi(value.c_str());

   name="len_fileNo";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_fileNoLen=atoi(value.c_str());
   
   name="first_fileno";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_firstFileNo=atoi(value.c_str());

   name="interval_time";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_intervalTime=atoi(value.c_str());

   name="before_time";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   m_beforeTime=atoi(value.c_str());
   
   name="check_filename";
   if (!m_theSysParam->getValue(ppPath, name, value)) {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,ppPath + SECTDELIM + name);
     return false;
   }
   bCheck_Filename=(bool)atoi(value.c_str());
         
   ppPath = root + SECTDELIM + channelNo ;
   if( !getPath(ppPath,pathMessage) ) {
      m_theErrorMessages->insert(PP_ERROR_GET_CONFIG,"cannot get path's parameter! ");
      return false;
   }

   //m_SendApp = new SendApp(channelNo,0);
   m_SendApp = new SendApp(channelNo,1);

   if(m_SendApp==0) {
      m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT,"cannot create SendApp Object!");
      return false;
   }

	 if ( m_SendApp->initialize(switchName,pathMessage,theErrorRate,
	                            isBackupSrc,isBackupDes,isIndb,
	                            isCheckCdrNo,bCheck_Filename) == false ){
	    m_theErrorMessages->insert(PP_ERROR_INITIALIZE,"cannot initialize SendApp!");
	 	return false;
	 }

   m_filecount=1; // 满足m_proccount>=m_filecount的条件       // 070308   gaoxl
   m_proccount=1;

   //add by gaojf 2009-2-27 13:44
   if(m_SendApp->setSrcPath(m_sourcePath.c_str())==false)
   {
     m_theErrorMessages->insert(PP_ERROR_INITIALIZE,"m_SendApp->setSrcPath false!");
     return false;
   }
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
   int iReturn;
   
   //从.rcd文件中得文件号fileNo
   //如果间隔时间为0，说明是实时话单，不需要文件号
   if(m_intervalTime>0)
   {
     iReturn = getFileNo();
     if (iReturn<0)
     	  return false;
   }
   else
   	 m_fileNo=0;
   
   Application::recovery();
   return true;
}


//##ModelId=3BCAA28000C6
bool MyApplication::loopProcess()
{
  //-----------------------------------------------------------------------//
  // TODO: Add your specialized code here and/or call the base class
  //-----------------------------------------------------------------------//
  int iTempNo;
  int iReturn,iReturn1;
  char cFileNo[20];
  string sTempName;
  bool t_needSleepFlag = true;

  //1.根据当前时间计算得到当前文件号
  iTempNo = getNowNo();

  //2.判断上传标志：0.生成空文件 1.正常生成上传文件 3.上传时间未到进程IDLE
  if(m_intervalTime>0)
  {	
    if(iTempNo == m_fileNo)      
      m_sendFlag = 1;
    else if(iTempNo < m_fileNo)
     	m_sendFlag = 3;
    else
    	m_sendFlag = 0;
  }
  else
  {
    m_sendFlag = 4;
    m_fileNo = 0;
  }
  
  //3.进入文件生成主函数
  if((m_sendFlag ==0)||(m_sendFlag==1)||(m_sendFlag==4))
  {
    //根据文件号得到上传文件名，实时情况
    if(m_sendFlag==4)
    {
    	if( m_SendApp->getOutFileNameRt(m_sourcePath,sTempName) <1)
      {
      	m_status = PROC_WARN;
      	m_objectName = "Get outFileName Error!";
      	updateStatus();
      	sleep(SLEEP_TIME);
      	return Application::loopProcess();
      }
      t_needSleepFlag = false;
    }
    ///根据文件号得到上传文件名，定时情况
    else
    {
      if( m_SendApp->getOutFileName(m_fileNo,m_fileNoLen,sTempName) <1)
      {
      	m_status = PROC_WARN;
      	m_objectName = "Get outFileName Error!";
      	updateStatus();
      	sleep(SLEEP_TIME);
      	return Application::loopProcess();
      }
    }
    
    //如果取得上传文件名为空，则进程IDLE
    if(sTempName=="")
    {
      m_status = PROC_IDLE;
      m_objectName = "";
      updateStatus();
      sleep(SLEEP_TIME);
      return Application::loopProcess();
    }
    else
    {
      m_status = PROC_RUN;
      m_objectName = sTempName;
      updateStatus();
      #ifdef FP_DEBUG
        FP_BEGIN(pBegin)
      #endif
      theLoggerProcessID = theLogger->pBegin();
      #ifdef FP_DEBUG
        FP_END(pBegin)
      #endif
    }
        
    //调用SendApp中的mainProcess开始进行上发处理
  	if (iReturn = m_SendApp->mainProcess(m_sendFlag,m_fileNo,m_sourcePath,m_isHeadOrTail)>0)
  	{	 
  		if(m_sendFlag!=4)	
  		  writeRcdFile();
      #ifdef FP_DEBUG
        FP_BEGIN(writeLog)
      #endif
  	  writeLog();
      #ifdef FP_DEBUG
        FP_END(writeLog)
      #endif
     #ifdef FP_DEBUG
        FP_BEGIN(pEnd)
      #endif
  theLogger->pEnd(theLoggerProcessID);
       #ifdef FP_DEBUG
        FP_END(pEnd)
      #endif
   	}
  	//else if(iReturn == NOFILE_IDLE)//如果接收到IDLE信号(SRC目录下没有文件),则IDLE
  	//{
     // m_status = PROC_IDLE;
      //m_objectName = "";
      //updateStatus();
    //}		
  	else
  	{
   		writeAlert();
      m_status = PROC_WARN;
      m_objectName = "file " + sTempName + " error!";
      writeLog();
      updateStatus();
      return Application::loopProcess();
    }
  }
 
  //跨过凌晨时间点的处理，writeRcdFile()仅仅是fileNo++
  //生成最后一个文件时，.rcd文件中放的是max+1  
  //完成每天的最后一个文件，跨过凌晨这个点时 ，更新文件号
  if(m_intervalTime>0)
  {
    if((m_fileNo>=((int)(24*60*60/m_intervalTime)+m_firstFileNo))&& 
       (getNowNo()<((int)(24*60*60/m_intervalTime)+m_firstFileNo-1)))
    {
       m_fileNo  = m_firstFileNo;//直接置为firstFileNo
       m_sendFlag = 1;           //下一次循环时还要重新判断  
    }
  } 
  
  //4.进程IDLE
  m_status = PROC_IDLE;
  m_objectName = "";
  updateStatus();
  if(t_needSleepFlag==true)
  {
    sleep(SLEEP_TIME);
  }
  return true;
}

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//

void MyApplication::writeLog()
{
	char Tmp[256];
	int iSendCdr,iSendFee;
	
	iSendCdr = m_SendLog_->getTotalCdrs()- m_SendLog_->getErrorSum();
	iSendFee = m_SendLog_->getTotalFee()- m_SendLog_->getErrorFee();
	
	sprintf(Tmp,"type=%s,SEND_CDR=%d,ERROR_CDR=%d,SEND_FEE=%d,ERROR_FEE=%d,TOTAL_FEE=%d",
	             "RoamSend",
	             iSendCdr,
	             m_SendLog_->getErrorSum(),
	             iSendFee,
	             m_SendLog_->getErrorFee(),
	             m_SendLog_->getTotalFee()
	             );
	string sourceFileNameWithPath = m_sourcePath+m_SendLog_->getSrcFileName();
	string desFileNameWithPath = m_desPath + m_SendLog_->getDesFileName();
	//theLogger->pInput(theLoggerProcessID,sourceFileNameWithPath,m_SendLog_->getFileLenth());
	theLogger->pInput(theLoggerProcessID,desFileNameWithPath,m_SendLog_->getTotalCdrs());
	theLogger->pOutput(theLoggerProcessID,desFileNameWithPath,m_SendLog_->getTotalCdrs(),Tmp);        
}

MyApplication::~MyApplication()
{
   if(m_SendApp!=0)
      delete m_SendApp;
}

//取得send.cfg中<path>标签下的所有路径信息
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
   name="file_indb_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   name+=" "+value;
   pathMessage.push_back(name);
   name="log_path";
   if (!m_theSysParam->getValue(mypath, name, value))
     return false;
   m_logPath =value;
   name+=" "+value;
   pathMessage.push_back(name);

   return true;
}

//根据.rcd文件中的文件号信息，取得目前的文件号
int MyApplication::getFileNo()
{
   FILE *fLastSend;//将指向.rcd文件
   char temp[300],timestr[20],str1[20],buf[200];
   string sTemp;
   //string fileNo;
   struct tm stLastTime;//.rcd文件中的上次发送年月日时分
   int itemp;

   m_rcdFileName = m_logPath;
   m_rcdFileName += "//";
   m_rcdFileName += m_fileType;
   m_rcdFileName += ".rcd"; //  /home/billing/bin/log/ooxx.rcd
    
   //读取.rcd文件，对.rcd文件进行判断及操作
   //1..rcd文件不存在
   if((fLastSend=fopen(m_rcdFileName.c_str(),"r"))==NULL)
   {
       	if((fLastSend=fopen(m_rcdFileName.c_str(),"w"))==NULL)
       	{
       	    sprintf(temp,"打开文件失败,文件名:%s",m_rcdFileName.c_str());
       	    m_theErrorMessages->insert(PP_ERROR_INITIALIZE,temp);
       	    return -1;
       	}
       	fclose(fLastSend);
       	
       	//1.1 .rcd文件不存在但可写,即可以生成一个新的文件,则fileNo 设为当前文件号
       	m_fileNo = getNowNo();
         
        //1.2 特殊情况:时间是凌晨,计算出来的文件号<最小文件号,此时被置为m_firstFileNo
        if (m_fileNo < m_firstFileNo)
       	    m_fileNo = m_firstFileNo;
   }
   //2..rcd文件存在且可读(含内容长度正常与不正常)
   else 
   {
       	fgets(buf,200,fLastSend);
       	fclose(fLastSend);
       	
       	//2.1 .rcd内容长度不正常(含.rcd文件为空文件)
       	if (strlen(buf)!= m_fileNoLen+8+1)
       	{
       	    sprintf(temp,"文件号长度错误,请检查！文件名:%s",m_rcdFileName.c_str());
       	    m_theErrorMessages->insert(PP_ERROR_INITIALIZE,temp);
       	    return -1;
       	}
       	//2.2 .rcd文件存在且可读，且内容长度正常(含文件号，日期是否正常)
       	else
       	{
            sprintf(timestr,"%4.4s\0",buf);
           	stLastTime.tm_year=atoi(timestr);
           	sprintf(timestr,"%2.2s\0",buf+4);
           	stLastTime.tm_mon=atoi(timestr);
           	sprintf(timestr,"%2.2s\0",buf+6);
           	stLastTime.tm_mday=atoi(timestr);

           	sTemp = buf;
           	m_fileNo=atoi(sTemp.substr(8+1).c_str());//文件号
           	//2.2.1 检查文件号
           	itemp = checkFileNo(m_fileNo);

           	//2.2.2 文件号不正常
           	if (itemp == 0)
       	    {
       	    	 sprintf(temp,"checkFileNo错误,请检查！文件名:%s",m_rcdFileName.c_str());
       	    	 m_theErrorMessages->insert(PP_ERROR_INITIALIZE,temp);
       	    	 return -1;
       	    }
           	//2.2.3 文件号正常，再判断日期是否是今天
           	else if (itemp == 1)
            {
             	if (!checkDate(stLastTime.tm_year,stLastTime.tm_mon,stLastTime.tm_mday))
               {
                    	m_fileNo = m_firstFileNo;//不是前一天停发的情况
             	 }
            }
           	else if (itemp == 2)//文件号正常 ,但为特殊情况,即已完成当天的文件,则等到第二天
           	{
           	    	m_fileNo = m_firstFileNo;
           	    	while(checkDate(stLastTime.tm_year,stLastTime.tm_mon,stLastTime.tm_mday))
           	    	    	sleep (SLEEP_TIME);
           	}
       	}
   }

   return 1;
}

//文件号校验，返回0 不正常;1  正常;2 已完成当天的文件
int MyApplication::checkFileNo(const int &sendNo)
{
    	int iMax,iMin;
    	iMax = (int)(60*60*24/m_intervalTime) + m_firstFileNo - 1; //UC:96
    	iMin = m_firstFileNo;                                      //UC:1 or 0

    	if ((m_fileNo<=iMax) && (m_fileNo>=iMin))
    	    	return 1;
    	else if (m_fileNo == (iMax+1))//97  写进了.rcd文件
    	    	return 2;
    	else
    	    	return 0;
}

bool MyApplication::checkDate(int Year, int Month, int Day)
{
    	time_t lNowTime;
    	struct tm* stNowTime;
    	
    	time(&lNowTime);
    	stNowTime=localtime(&lNowTime);
    	if((stNowTime->tm_year+1900==Year)&&
    	   (stNowTime->tm_mon+1==Month)&&
    	   (stNowTime->tm_mday==Day))
    	     	return true;
    	else
    	     	return false;
}

//更新.rcd文件信息
int MyApplication::writeRcdFile()
{
    	string sTemp;
    	char cTemp[5+1];
    	FILE *fp;

    	m_fileNo++;

      if((fp=fopen(m_rcdFileName.c_str(),"w"))==NULL)
    	{
    	    	return -1;
    	}

      memset(cTemp,0,sizeof(cTemp));
      sprintf(cTemp,"%d",m_fileNo);
      FormatConversion::changeFormatRz(cTemp,m_fileNoLen);
      
      setNowTime();
      
      sTemp = "";
      sTemp = m_nowTime.substr(0,8);
      sTemp += ".";
      sTemp += string(cTemp);
      
    	fputs(sTemp.c_str(),fp);
    	fclose(fp);

    	return 1;
}

//计算系统现在时该的文件号(零时零分返回0,零时13分返回1)
int MyApplication::getNowNo()
{
    	int tempNo,maxNo;
    	time_t lNowTime;
    	struct tm* stNowTime;
    	
    	time(&lNowTime);
    	stNowTime=localtime(&lNowTime);

    	//全部转化成分钟来算
    	tempNo=(int)((stNowTime->tm_hour*60+stNowTime->tm_min+m_beforeTime/60)/
    	             (m_intervalTime/60.0))+m_firstFileNo-1;

    	return tempNo;
}

void MyApplication::setNowTime(void)
{
  char cTemp[15];
  time_t lNowTime;
  struct tm* stNowTime;
  time(&lNowTime);
  stNowTime=localtime(&lNowTime);
  sprintf(cTemp,"%04d%02d%02d%02d%02d%02d\0",stNowTime->tm_year+1900,stNowTime->tm_mon+1,
    stNowTime->tm_mday,stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
  m_nowTime = cTemp;
}

// add n from s to d formed as 'YYYYMMDD'
int MyApplication::dayadd(char *d,char *s,long n)
{
  	char tmp[10];
  	time_t now,ret;
  	struct tm when;

  	time(&now);
  	when=*localtime(&now);

  	memcpy(tmp,s,4);
  	tmp[4]=0;
  	when.tm_year=atoi(tmp)-1900;
  	memcpy(tmp,s+4,2);
  	tmp[2]=0;
  	when.tm_mon=atoi(tmp)-1;
  	memcpy(tmp,s+6,2);
  	tmp[2]=0;
  	when.tm_mday=atoi(tmp)+n;
  	when.tm_isdst = -1;  // please attention
  	if((ret=mktime(&when))!=-1)
  	{
  	  	sprintf(d,"%04d%02d%02d",when.tm_year+1900,when.tm_mon+1,when.tm_mday);
  	}
  	return ret;
}

