// ##########################################
// Source file : MyApplication.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "MyApplication.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "appfrm/VersionInfo.h"
#include "AlertCode.h"
#include "Pp_Exception.h"
#include "OutputFile.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

MyApplication g_application;
Application * const theApp = &g_application;

//##ModelId=48571D190271
MyApplication::~MyApplication()
{
    for(int i = 0; i < m_channelInfos.size(); ++i)
    {
        if(m_channelInfos[i] != 0)
        {
            delete m_channelInfos[i];
        }
    }
    m_channelInfos.clear();
}

//##ModelId=48571D5900FA
bool MyApplication::processopt(int optopt, const char *optarg)
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
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

//##ModelId=48571D77035B
bool MyApplication::initialization()
{
    //-----------------------------------------------------------------------//
    // call setoptstr before initialization()
    // set optstring -- reference unix getopt() call
    // the 'c' option is reserved for channel no option,
    // you can add your owner option
    //-----------------------------------------------------------------------//
    setoptstr("c:");

    // 设置版本信息
    setPpVersion();

    // normally, call standard initialization first
    if (!Application::initialization())
    {
        return false;
    }

    m_PpLog_ = PpLog::getInstance();

    if (m_PpLog_ == 0)
    {
        m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create PpLog object!");
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

    setFileSortMethod(Directory::SortFlags (Directory::SF_MODIFYTIME));
    //setFileSortMethod(Directory::SortFlags (Directory::SF_NAME));
    setMaxScan(20000);

    m_fileSum = 0;
    m_channelSequnce = 0;
    m_channelInfos.clear();
    m_roamPath = "";
    m_longPath = "";
    m_isSplitLongCdr = false;

    if(!getSysParam())
    {
        return false;
    }
    return true;
}

//##ModelId=48571D860232
bool MyApplication::recovery()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//

    // TO BE DETERMINED
    Application::recovery();
    return true;
}

//##ModelId=48571D95034B
bool MyApplication::loopProcess()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
	if (!Application::beforeLoop())
	{
		return false;
	}
    char fileNameTmp[80 + 1];
    string fileName;
    char cmd[100];
    int iStatus = 0;
    memset(fileNameTmp, 0, sizeof(fileNameTmp));
    //当处理同一输入目录下的标准文件超过40个的时候,强制转换输入通道
    if(m_fileSum >= 40)
    {
        m_fileSum = 0;
        if(!getNextChanInfo())
        {
            m_theErrorMessages->insert(PP_ERROR_INITIALIZE,"cannot get next channel info!");
            m_stop = true;
            return false;
        }
    }
    //当扫描所有输入目录都没有文件时,进程等待"SLEEP_TIME"
    int i = 1;
    while(!getFile(fileNameTmp, 80))
    {
        if(i >= m_channelInfos.size())
        {
            i = 1;
            m_status = PROC_IDLE;
            m_objectName = "";
            updateStatus();
            sleep(SLEEP_TIME);
            return Application::loopProcess();
        }
        i++;
        m_fileSum = 0;

        if(!getNextChanInfo())
        {
            m_theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot get next channel info!");
            m_stop = true;
            return false;
        }

        memset(fileNameTmp, 0, sizeof(fileNameTmp));
    }
    // ------ 开始处理文件 --------
	fileName = fileNameTmp;
    //不处理临时文件:以'#'或'%'开头文件名的文件 add by gaoxl 2009-8-14 17:40
    if((fileName.find("#") == 0) || (fileName.find("%") == 0))
    {
        return true;
    }
    //日志处理初始化
    theLoggerProcessID = theLogger->pBegin();
    //if(fileName.find_last_of(".Z") == ( fileName.size() - 1))
    if(fileName.find(".Z") == ( fileName.size() - 2))
    {
        sprintf(cmd, "uncompress %s%s", m_sourcePath->c_str(), fileName.c_str());
        if((iStatus=system(cmd)) != 0)
        {
            m_theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "不能解压文件" + fileName + "!移至备份目录");
            m_PreProcApplication->m_pOutputFile->mvToErrDir(fileName);
            return Application::loopProcess();
        }
        fileName.erase(fileName.find_last_not_of(".Z") + 1, fileName.npos);
    }
    m_objectName = fileName;
    m_status = PROC_RUN;
    updateStatus();

    try
    {
		m_PreProcApplication->run( fileName ) ;
    }
    catch(Pp_Exception &e)
    {                                                    //run error;
		m_theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "处理文件" + fileName + "失败!" + e.GetString());
	 #ifdef _DEBUG_
	 	cout<<e.GetString()<<endl;
	 #endif
		writeAlert();
        m_status = PROC_WARN;
        m_objectName = "file " + fileName + " error!";
        writeLog();
        updateStatus();
        theLogger->pEnd(theLoggerProcessID);//added by zaiww 20090915 此处空文件日志输出要完整
        return Application::loopProcess();
    }
    writeAlert();
    writeLog();
    theLogger->pEnd(theLoggerProcessID);
    // ------ 处理文件结束 --------
    ++m_fileSum;
    return true;
}

//##ModelId=48571DA40203
bool MyApplication::end()
{
    return Application::end();
}

//##ModelId=48571DFA009C
bool MyApplication::getPath(string path, vector <string> &pathMessage, channelInfo *pChanInfo)
{
    string mypath, value, name;
    mypath = path + SECTDELIM + "path";
    name = "source_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    *(pChanInfo->m_pInPutPathStr) = value;
    name += " " + value;
    pathMessage.push_back(name);
    name = "src_backup_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    name += " " + value;
    pathMessage.push_back(name);
    name = "destination_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    *(pChanInfo->m_pOutPutPathStr) = value;
    name += " " + value;
    pathMessage.push_back(name);
    name = "des_backup_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    name += " " + value;
    pathMessage.push_back(name);
    name = "file_error_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    name += " " + value;
    pathMessage.push_back(name);
    name = "roamlog_path";
    if (!m_theSysParam->getValue(mypath, name, value))
    {
        return false;
    }
    m_roamPath = value;
    if (m_isSplitLongCdr)
    {
        name = "long_cdr_path";
        if (!m_theSysParam->getValue(mypath, name, value))
        {
            return false;
        }
        m_longPath = value;
    }
    return true;
}

//##ModelId=48571E18033C
void MyApplication::writeLog()
{
	char Tmp[256];

	//	 struct tm* stNowTime;      //测试效率用，取文件结束处理时间，added by zhongh 2010/11/04 14:44:06
	//char tmp[15];
  // long  m_tNowTime;
	//
	//time(&m_tNowTime);
	//stNowTime = localtime(&m_tNowTime);
  //
  //  memset(tmp,0,sizeof(tmp));
  //
	//sprintf(tmp,"%04d%02d%02d%02d%02d%02d",stNowTime->tm_year + 1900,stNowTime->tm_mon+1,stNowTime->tm_mday,
  //                                      stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
  //
  //                                      
	//sprintf(Tmp,"newpp@%s@%s@%s@%d@%d@%d@%d@%d@%d@%d@%d@%d@%d@%d",
  //              (m_PpLog_->getStartDateTime()).c_str(),      //开始时间
  //              tmp,                                         //结束时间
  //             (m_PpLog_->getSrcFileName()).c_str(),
  //              m_PpLog_->getTotalCdrs(),
  //              m_PpLog_->getNoUseSum(),
  //              m_PpLog_->getErrorSum() ,               
  //              m_PpLog_->getMocSum(),
  //              m_PpLog_->getMtcSum(),
  //              m_PpLog_->getPocSum(),
  //              m_PpLog_->getPtcSum(),
  //              m_PpLog_->getForwSum(),
  //              m_PpLog_->getSmocSum(),
  //              m_PpLog_->getSmtcSum(),
  //              m_PpLog_->getOtherSum());
	//      m_logstr = Tmp;
	//Application::writeLog();
	
sprintf(Tmp,"SOURCE_TYPE=%s,Error=%d,SUM(01)=%d,SUM(02)=%d,SUM(03)=%d,SUM(07)=%d,SUM(08)=%d,SUM(11)=%d,SUM(OTHER)=%d,delay1=%s,delay2=%s,delay3=%d,delay4=%d,delay5=%d,delay6=%d,delay7=%d",
             m_PpLog_->getSourceType().c_str(),
             m_PpLog_->getErrorSum(),
             m_PpLog_->getMocSum(),
             m_PpLog_->getMtcSum(),
             m_PpLog_->getForwSum(),
             m_PpLog_->getSmocSum(),
             m_PpLog_->getSmtcSum(),
             m_PpLog_->getPocSum(),
             m_PpLog_->getOtherSum(),
             m_PpLog_->getMinStartDateTime().c_str(),
             m_PpLog_->getMaxStartDateTime().c_str(),
             m_PpLog_->getBeginCdrNo(),
             m_PpLog_->getEndCdrNo(),
             m_PpLog_->getFileLenth(),
             m_PpLog_->getDelaySum6(),
             m_PpLog_->getDelaySum7()
             );
string sourceFileNameWithPath   = m_sourcePath->c_str() + m_PpLog_->getSrcFileName();
string desFileNameWithPath      = m_desPath->c_str() + m_PpLog_->getSrcFileName();
theLogger->pInput(theLoggerProcessID,sourceFileNameWithPath,m_PpLog_->getFileLenth());
theLogger->pOutput(theLoggerProcessID,desFileNameWithPath,m_PpLog_->getTotalCdrs(),Tmp);
}

//##ModelId=48571E24958F
bool MyApplication::getNextChanInfo()
{
    // 如果配置了一个输入目录,则直接返回
    if( m_channelInfos.size() == 1)
    {
        return true;
    }
    else // 轮询下一个输入通道,并提取相应的文件处理器,同时重置输入目录
    {
        if(m_channelSequnce == m_channelInfos.size())
        {
            m_channelSequnce = 0;
        }
        m_PreProcApplication = m_channelInfos[m_channelSequnce]->m_pProcessor;
        m_sourcePath         = m_channelInfos[m_channelSequnce]->m_pInPutPathStr;
        m_desPath            = m_channelInfos[m_channelSequnce]->m_pOutPutPathStr;
        m_PpLog_->setOutPutFile(m_PreProcApplication->m_pOutputFile);
        setPathIn(m_sourcePath->c_str());
        ++m_channelSequnce;

        return true;
    }
}

//##ModelId=486B1B940177
bool MyApplication::getSysParam()
{
    char channelNo[4], c_subChanNo[10];
    float theErrorRate;
    int i_subProcessNo = 0;
    vector <string> pathMessage;
    string root, section, name, value, subPath, subPathTmp;
    string switchName, isBackupDes, isBackupSrc, isCheckCdrNo,isSplitDes, isSplitLongCdr;
    int MaxCdrNum;
    channelInfo *pChanInfo;
    root = SECTDELIM + string(m_name);
    m_PreProcApplication = 0;
    sprintf(channelNo, "%d", m_channelNo);
    subPathTmp = subPath = root + SECTDELIM + channelNo + SECTDELIM + "channel_0";      // "//pp//0//channel_0"
    m_theSysParam->setSectionPath(root + SECTDELIM + channelNo);
    //m_theSysParam->setSectionPath(subPathTmp);                              //(root + SECTDELIM + channelNo);
    //m_theSysParam->setSectionPath(root + SECTDELIM + channelNo);
    while(m_theSysParam->getSubSection(subPathTmp))
    {
        pChanInfo = new channelInfo(subPath.c_str(), 0);
        pathMessage.clear();
        name = "switch_name";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        switchName = value;
        name = "error_rate";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        theErrorRate = atof(value.c_str());
        name = "backup_srcfile";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        isBackupSrc = value;
        name = "backup_desfile";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        isBackupDes = value;
        // get the SOURCE_TYPE's value for the process's log
        name = "SOURCE_TYPE";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        m_PpLog_->setSourceType(value);
        name = "check_cdrNo";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        isCheckCdrNo = value;
        name = "split_longcdr";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
            m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
            return false;
        }
        isSplitLongCdr = value;
        if (isSplitLongCdr.compare("yes") == 0)
        {
            m_isSplitLongCdr = true;
        }
        name = "split_desfile";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
            m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
            return false;
        }
        isSplitDes = value;
        name = "max_cdrNum";
        if (!m_theSysParam->getValue(subPath, name, value))
        {
            m_theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
            return false;
        }
        MaxCdrNum = atoi(value.c_str());

        if(!getPath(subPath, pathMessage, pChanInfo))
        {
           m_theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get path's parameter! ");
           return false;
        }
        if(pChanInfo->m_pProcessor == 0)
        {
           m_theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create PreProcApplication Object!");
           return false;
        }
		try
		{
			(pChanInfo->m_pProcessor)->initialize(switchName, pathMessage, theErrorRate, isBackupSrc, isBackupDes,
	                                              isCheckCdrNo, isSplitDes, MaxCdrNum, m_roamPath, m_isSplitLongCdr, m_longPath) ;
		}   
		catch(Pp_Exception &e)
        {
            m_theErrorMessages->insert(PP_ERROR_INITIALIZE, "子通道"+subPath+":"+e.GetString());
	     #ifdef _DEBUG_
		 cout<<e.GetString()<<endl;
	     #endif
            return false;
        }
        m_channelInfos.push_back(pChanInfo);
        // 拼凑下一个子通道的路径信息
        ++i_subProcessNo;
        sprintf(c_subChanNo, "channel_%d", i_subProcessNo);
        subPathTmp = subPath = root + SECTDELIM + channelNo + SECTDELIM + c_subChanNo;  // "//pp//0//channel_1"
        m_theSysParam->setSectionPath(subPathTmp);
    }
    
    //cout<<m_channelInfos.size()<<endl;

    // 判断配置文件中的通道下是否有子通道信息
    if(m_channelInfos.size() == 0)
    {
        m_theErrorMessages->insert(PP_ERROR_INITIALIZE, "sub channel number is 0!");
        return false;
    }
    else
    {
        // 首次运行,直接取第一个子通道的配置
        m_PreProcApplication = m_channelInfos[0]->m_pProcessor;
        m_sourcePath         = m_channelInfos[0]->m_pInPutPathStr;
        m_desPath            = m_channelInfos[0]->m_pOutPutPathStr;
        m_PpLog_->setOutPutFile(m_PreProcApplication->m_pOutputFile);
        setPathIn(m_sourcePath->c_str());
        ++m_channelSequnce;
        return true;
    }
    return true;
}

//##ModelId=486B10987AF4
void MyApplication::setPpVersion()
{
    VersionInfo theVersionInfo;

    theVersionInfo.m_date   = "2008/07/04";
    theVersionInfo.m_main   = 1;
    theVersionInfo.m_mini   = 2;
    theVersionInfo.m_patch  = 5;
    theVersionInfo.m_comment= "New pp for NGBOSS";

    setVersion(theVersionInfo);

    return;
}

