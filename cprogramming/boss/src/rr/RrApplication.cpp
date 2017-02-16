#include "config-all.h"
#include "RrApplication.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include "base/SysParam.h"
#include "base/supdebug.h"
#include "base/Exception.h"
#include "AbstractFieldSelector.h"
#include "RrException.h"
#include "base/supdebug.h"

RrApplication   g_rrApp;
Application * const theApp = &g_rrApp;
extern log4cpp::Logger *theLogger;
extern string theLoggerProcessID;

RrApplication::RrApplication()
{
  m_fidInfoList.clear();
  m_fidCount=m_fidCapcity=0;
  m_lastWarnCode=0;
  m_fileRrRule=NULL;
  m_firstFileFlag =false;
  m_openFlag = false;
}

RrApplication::~RrApplication()
{
}


//初始化
bool RrApplication::initialization()
{
  setoptstr("c:");

  #ifndef _DEBUG_
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
  #endif

  if (!Application::initialization())
  {
    #ifdef _DEBUG_
      cout<<"Application::initialization() false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  try
  {
    //加载配置参数信息
    getSysParam();
    //初始化
    m_filterInfoMgr.initialization(m_config);
    //设置取原始文件的路径
    m_dir.setPath(m_config.m_inputPath.c_str());
    
    //按时间提取文件
    m_sortMethod = Directory::SF_MODIFYTIME; //Directory::SF_NAME
    m_dir.scanFiles(m_srcFileNames,m_sortMethod);    
  }catch(RrException &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    warning(e.getErrcode(), e.GetString());
    return false;
  }
  return true;
}
bool RrApplication::recovery()
{
  if(Application::recovery()==false)
  {
    #ifdef _DEBUG_
      cout<<"Application::recovery() false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}
bool RrApplication::beforeLoop()
{
  if(Application::beforeLoop()==false)
  {
    #ifdef _DEBUG_
      cout<<"Application::beforeLoop() false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  //预分配内存，预留合并数的20%
  m_fidInfoList.reserve((m_config.m_mergeCount * 120)/100);
  return true;
}

bool RrApplication::loopProcess()
{
  char   t_fileName[PATH_MAX];
  time_t t_fileTime;
  int    t_ret;
  
  while((t_ret = m_dir.getFile(t_fileName, PATH_MAX,t_fileTime)) > 0)
  {
    if(t_fileName[0] == '#' || t_fileName[0] == '%' )
    {
      continue;
    }else break;
  }
  try
  {
    if (t_ret < 0)//从目录中读取文件失败
    {
      updateRtStatus(PROC_WARN, "", "取文件错误");
      warning(E_RR_GETFILE_ERROR,m_config.m_inputPath.c_str());
      sleep(E_RR_SLEEPTIME);
      return true;
    }
    if(t_ret == 0) //目录下无文件
    {
      //在已经有累积的文件
      // 但又超时的情况下或者其它文件等待时间较长的情况下
      if(m_srcFileNames.empty()==false && 
         (time(NULL)-m_curDealTime  >= m_config.m_waiteTimeOut ||
          (m_firstFileFlag == true &&
           time(NULL)-m_firstFileTime>= m_config.m_waiteTimeOut)))
      {
        m_fileRrRule = m_lastRrRule;
        doProcess();
      }
      updateRtStatus(PROC_WAIT, "", m_srcFileNames.empty()? "没有同类型文件" : "没有新文件");
      sleep(E_RR_SLEEPTIME);
      m_dir.scanFiles(m_srcFileNames,m_sortMethod);      
      rewind();
      return true;
    }
    //跳过已经处理过的文件
    if(isDealedFile(t_fileName)==true)
    { 
      m_dir.setDealFlag();
      return true;
    }
    //判断t_fileName和已处理的文件是否同类
    if(isSameFileType(t_fileName,m_fileRrRule)==false)
    { //不同类型的文件
      return true;
    }//else
    {
      try
      {
      	//if((m_config.getRrRule(t_fileName))->m_rrType == E_RR_RRTYPE_FILE)
      	if(m_fileRrRule->m_rrType == E_RR_RRTYPE_FILE)
      	{ //文件级排重不合并
      	  getNameFilterInfo(t_fileName);
          m_dir.setDealFlag();
      	  doProcess();
      	}
      	else
      	{
          getCdrFilterInfo(t_fileName);
          m_dir.setDealFlag();
          if(m_fidCount>=m_config.m_mergeCount ||
             time(NULL)-m_curDealTime  >= m_config.m_waiteTimeOut)
          {
            doProcess();
          }
        }
      }catch(RrException &e)
      {
        doProcess();
        throw e;
      };
    }
  }catch(RrException &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    warning(e.getErrcode(), e.GetString());
    return true;
  }
  return true;
}

bool RrApplication::end()
{
  try
  {
    //将最后没有处理的提交
    doProcess(); 
  }catch(RrException &e)
  {
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
    warning(e.getErrcode(), e.GetString());
    return true;
  }
  m_filterInfoMgr.end();
  m_config.end();
  m_fidInfoList.clear();
  FileRrRule::clear();
  if(m_openFlag==true)
  {
    theLogger->pEnd(theLoggerProcessID);
    m_openFlag=false;
  }
  return Application::end();
}

bool RrApplication::getSysParam()
{
  string t_rootPath;
  t_rootPath = SECTDELIM ;t_rootPath+=m_name;
  
  m_config.getSysParam(m_theSysParam,t_rootPath.c_str(),m_channelNo);
  #ifdef _DEBUG_
    //cout<<m_config<<endl;
  #endif
  return true;
}

//获得处理时间
long RrApplication::getDealTime()//added by hanyu 08-12-01
{
	time_t t_nowTime;
	time(&t_nowTime);
	struct tm *t_time;
	t_time = localtime(&t_nowTime);
	char t_strTime[14+1];
	sprintf(t_strTime,"%4d%02d%02d000000",t_time->tm_year + 1900,
    	                                   t_time->tm_mon+1,
    	                                   t_time->tm_mday);
	return atol(t_strTime);
}

bool RrApplication::getNameFilterInfo(const char * r_fileName)//added by hanyu 08-11-27
{
	int         t_filePos = 0;
  int         t_iResult = 0;
  int         t_errorCode=0;
  FilterInfo  t_filterInfo;
  const char* t_pCdrEnd;
  size_t      t_lineNum = 0;  
  #ifdef FP_DEBUG
    FP_BEGIN(getNameFilterInfo)
  #endif
  
  //根据排重规则 m_fileRrRule 读取一个文件中的文件
  t_filePos = m_srcFileNames.size();
  generateFileName(r_fileName);
  
  t_filterInfo.m_filePos = t_filePos;
  #ifdef _DEBUG_ 
    t_filterInfo.m_fileName = r_fileName;//源文件名
  #endif
  strcpy(m_lineBuff,r_fileName);
  t_pCdrEnd = m_lineBuff+strlen(m_lineBuff);
  #ifdef _DEBUG_
    t_filterInfo.m_line = 0;//在文件中行数
  #endif
  t_filterInfo.m_uniqFlag = E_RR_REPFLAG_UNIQ;
  //if(m_fileRrRule->m_rrType!=E_RR_RRTYPE_NONE)
  //{
    //t_iResult=m_fileRrRule->m_fidSelect->selectFieldValue(m_lineBuff,t_pCdrEnd,m_fieldValue,E_RR_LINEMAXLEN);
    //if(t_iResult<0) t_errorCode=E_RR_REPFLAG_ERROR;
    //t_filterInfo.m_fid = m_fieldValue;
    t_filterInfo.m_fid = r_fileName;
    
    t_filterInfo.m_beginTime = getDealTime(); //文件产生时间
    t_filterInfo.m_endTime = t_filterInfo.m_beginTime;   //
    t_filterInfo.m_filePos =0;   //该文件在处理时m_srcFileNames的位置
    t_filterInfo.m_rrFlag=0; //排重标记：0 未经排重，1已经过排重
    t_filterInfo.m_uniqFlag = 0; //-1: 不需排重 0: 非重单、1 文件内重单、
                      // 2：内存文件间重单 3: 历史文件间重单 
  //}else  
  //{ 
  //  t_filterInfo.m_uniqFlag = E_RR_REPFLAG_NONE;
  //  t_filterInfo.m_rrFlag   = E_RR_RRFLAG_NONE;
  //}
  addFilterInfo(t_filterInfo);
  m_srcFileNames.push_back(r_fileName);
  
  sprintf(m_srcFile_fullName ,"%s/%s", m_config.m_inputPath.c_str(), r_fileName);
  t_lineNum = getCdrNum(m_srcFile_fullName);
  m_mergLog.addFileLog(m_srcFile_fullName,t_lineNum);
  m_rrLog.m_totalCdr+=t_lineNum;
  
  #ifdef FP_DEBUG
    FP_END(getNameFilterInfo)
  #endif
  
  return true;
}

long RrApplication::getCdrNum(const char * r_fileName)
{
	char t_lineBuff[E_RR_LINEMAXLEN+1];
	int t_lineNum = 0;
  ifstream t_ifSrcFile;
  t_ifSrcFile.open(r_fileName,ios::in);
  if(!t_ifSrcFile)
  {
    #ifdef _DEBUG_
      cout<<"open file:{"<<r_fileName<<"} failed!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    RR_THROW_CP1(E_RR_FILE_OPEN,"无效的文件:{%s}!",r_fileName);
    return false;
  }

  while(!t_ifSrcFile.eof())
  {
    t_ifSrcFile.getline(t_lineBuff,E_RR_LINEMAXLEN);
    if(t_lineBuff[0]==0) continue;
    t_lineNum++;
  }
  t_ifSrcFile.close();
  t_ifSrcFile.clear();
  return t_lineNum;
}

bool RrApplication::getCdrFilterInfo(const char * r_fileName)
{
  int         t_filePos = 0;
  int         t_iResult = 0;
  int         t_errorCode=0;
  FilterInfo  t_filterInfo;
  const char* t_pCdrEnd;
  size_t      t_lineNum = 0;  
  #ifdef FP_DEBUG
    FP_BEGIN(getCdrFilterInfo)
  #endif
  
  //根据排重规则 m_fileRrRule 读取一个文件中的文件
  t_filePos = m_srcFileNames.size();
  generateFileName(r_fileName);
  sprintf(m_srcFile_fullName ,"%s/%s", m_config.m_inputPath.c_str(), r_fileName);
  //遍历文件中的所有记录，提取过滤信息
  ifstream t_ifSrcFile;
  t_ifSrcFile.open(m_srcFile_fullName,ios::in);
  if(!t_ifSrcFile)
  {
    #ifdef _DEBUG_
      cout<<"open file:{"<<m_srcFile_fullName<<"} failed!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    RR_THROW_CP2(E_RR_FILE_OPEN,"无效的文件:{%d,%s}!",errno,m_srcFile_fullName);
    return false;
  }
  t_filterInfo.m_filePos = t_filePos;
  #ifdef _DEBUG_ 
    t_filterInfo.m_fileName = r_fileName;
  #endif
  while(!t_ifSrcFile.eof())
  {
    t_errorCode=0;
    t_ifSrcFile.getline(m_lineBuff,E_RR_LINEMAXLEN);
    if(m_lineBuff[0]==0)
    {
      continue;
    }    
    t_pCdrEnd = m_lineBuff+strlen(m_lineBuff);
    ++t_lineNum;
    #ifdef _DEBUG_
      t_filterInfo.m_line = t_lineNum;
    #endif
    t_filterInfo.m_uniqFlag = E_RR_REPFLAG_UNIQ;
    if(m_fileRrRule->m_rrType!=E_RR_RRTYPE_NONE)
    {
      //获取排重标记
      t_iResult = m_fileRrRule->m_rrFlagSelect->selectFieldValue(m_lineBuff,t_pCdrEnd,m_fieldValue,E_RR_LINEMAXLEN);
      if(t_iResult<0) t_errorCode=E_RR_REPFLAG_ERROR;
      t_filterInfo.m_rrFlag=atoi(m_fieldValue);
      
      if(t_filterInfo.m_rrFlag!=E_RR_RRFLAG_NONE)
      {//已经过排重
        t_filterInfo.m_fid       = "";
        t_filterInfo.m_beginTime = 0;
        t_filterInfo.m_endTime   = 0;
        t_filterInfo.m_uniqFlag  = E_RR_REPFLAG_NONE;
      }else
      {//未经过排重
        //获取FID
        t_iResult=m_fileRrRule->m_fidSelect->selectFieldValue(m_lineBuff,t_pCdrEnd,m_fieldValue,E_RR_LINEMAXLEN);
        if(t_iResult<0) t_errorCode=E_RR_REPFLAG_ERROR;
        t_filterInfo.m_fid = m_fieldValue;
        //获取排重时间
        t_iResult=m_fileRrRule->m_beginTimeSelect->selectFieldValue(m_lineBuff,t_pCdrEnd,m_fieldValue,E_RR_LINEMAXLEN);
        if(t_iResult<0) t_errorCode=E_RR_REPFLAG_ERROR;
        t_filterInfo.m_beginTime=atol(m_fieldValue);
        if(m_fileRrRule->m_rrType==E_RR_RRTYPE_CONTAIN)
        {
          t_iResult=m_fileRrRule->m_endTimeSelect->selectFieldValue(m_lineBuff,t_pCdrEnd,m_fieldValue,E_RR_LINEMAXLEN);
          if(t_iResult<0) t_errorCode=E_RR_REPFLAG_ERROR;
          t_filterInfo.m_endTime=atol(m_fieldValue);
        }else
        {
          t_filterInfo.m_endTime = 0;
        }
      }
      if(t_errorCode==0)
      {
        //设置已排重标记
        if(m_config.m_rrUpdateFlag == true)
        {
          m_fileRrRule->updateRrFlagValue(m_lineBuff);
        }
      }else
      {
        t_filterInfo.m_uniqFlag = E_RR_REPFLAG_ERROR;
      }
    }else  
    { 
      t_filterInfo.m_uniqFlag = E_RR_REPFLAG_NONE;
      t_filterInfo.m_rrFlag   = E_RR_RRFLAG_NONE;
    }
    m_mergFile<<m_lineBuff<<"\n"; 
    addFilterInfo(t_filterInfo);
  }  
  t_ifSrcFile.close();
  t_ifSrcFile.clear();
  m_srcFileNames.push_back(r_fileName);
  
  //产生合并文件日志
  m_mergLog.addFileLog(m_srcFile_fullName,t_lineNum);
  m_rrLog.m_totalCdr+=t_lineNum;
  #ifdef FP_DEBUG
    FP_END(getCdrFilterInfo)
  #endif
  
  return true;
}

bool RrApplication::doProcess()
{
  if(m_srcFileNames.empty()==true)
  {
    return true;
  }
  #ifdef FP_DEBUG
    FP_BEGIN(doProcess)
  #endif
  updateRtStatus(PROC_RUN, m_srcFileNames[0].c_str(), "开始处理...");
	if(m_mergFile.is_open())
	{ //防止第一个文件打开后，处理过程中又失效的情况
	  m_mergFile.close();
	  m_mergFile.clear();
	}
  #ifdef FP_DEBUG
    FP_BEGIN(setRepeatFlag)
  #endif
  //打重单标记
  m_filterInfoMgr.setRepeatFlag(m_fidInfoList,m_fidCount,m_fileRrRule);
  #ifdef FP_DEBUG
    FP_END(setRepeatFlag)
  #endif

  //输出祥单到临时文件

  if(m_fileRrRule->m_rrType == E_RR_RRTYPE_FILE)//added by hanyu 08-11-27
  {
  	outputFile();
  }else
  {
  #ifdef FP_DEBUG
    FP_BEGIN(outputCdr)
  #endif
    outputCdr();
  #ifdef FP_DEBUG
    FP_END(outputCdr)
  #endif
  }
  try
  {
  #ifdef FP_DEBUG
    FP_BEGIN(commitFilterInfo)
  #endif
    //提交过滤信息
    m_filterInfoMgr.commitFilterInfo();
  #ifdef FP_DEBUG
    FP_END(commitFilterInfo)
  #endif
  }catch(RrException &e)
  {
    unlinkTmpFile();
    throw e;
  }
  #ifdef FP_DEBUG
    FP_BEGIN(commitFile)
  #endif
  //提交文件
  commitFile();
  m_srcFileNames.clear();
  rewind();   //下一次处理新的文件
  m_fidCount=0;
  writeLog();
  #ifdef FP_DEBUG
    FP_END(commitFile)
  #endif
  #ifdef FP_DEBUG
    FP_END(doProcess)
  #endif
  //m_stop = true;//only for test
  return true;
}

bool RrApplication::writeLog()
{
  //theLoggerProcessID = theLogger->pBegin();
  for(vector<RrFileLog>::iterator t_strItr=m_mergLog.m_srcFileList.begin();
      t_strItr!=m_mergLog.m_srcFileList.end();++t_strItr)
  {
    theLogger->pInput(theLoggerProcessID,t_strItr->m_fileName.c_str(),
                      t_strItr->m_totalCdr);
    #ifdef _DEBUG_
      cout<<t_strItr->m_fileName<<" "<<t_strItr->m_totalCdr<<endl;
    #endif
  }
  if (m_rrLog.m_uniqCdr > 0) 
  {
    theLogger->pOutput(theLoggerProcessID,m_uniqueFileName,
                       m_rrLog.m_uniqCdr,"type=unique");
    #ifdef _DEBUG_
      cout<<m_uniqueFileName<<" "<<m_rrLog.m_uniqCdr<<endl;
    #endif
  }
  if (m_rrLog.m_errCdr > 0) 
  {
    theLogger->pOutput(theLoggerProcessID,m_errorFileName,
                       m_rrLog.m_errCdr,"type=unique");
    #ifdef _DEBUG_
      cout<<m_uniqueFileName<<" "<<m_rrLog.m_uniqCdr<<endl;
    #endif
  }
  if ((m_rrLog.m_repeatInFile + m_rrLog.m_repeatHis) >0 ) 
  {
    theLogger->pOutput(theLoggerProcessID,m_repeatFileName, 
                       m_rrLog.m_repeatInFile+m_rrLog.m_repeatHis,
                       "type=repeat");
    #ifdef _DEBUG_
      cout<<m_repeatFileName<<" "<<m_rrLog.m_repeatInFile + m_rrLog.m_repeatHis<<endl;
    #endif
  }
  theLogger->pEnd(theLoggerProcessID);
  m_openFlag=false; 
  m_mergLog.clear();
  m_rrLog.clear();
  return true;
}
void RrApplication::unlinkTmpFile()
{
  unlink(m_uniqueFileName_tmp);
  unlink(m_repeatFileName_tmp);
}
void RrApplication::warning(const int &r_warncode,const string &r_warnMsg)
{
  time_t t_nowTime;
  time(&t_nowTime);
  if(m_lastWarnCode==r_warncode &&
     m_lastWarnMsg ==r_warnMsg  &&
     t_nowTime-m_lastWarnTime< E_RR_WARNTIME)
  {
    return ;
  }
  m_theErrorMessages->insert(r_warncode,r_warnMsg);
  m_lastWarnCode=r_warncode;
  m_lastWarnMsg =r_warnMsg;
  m_lastWarnTime=t_nowTime;
}    
bool RrApplication::isSameFileType(const char * r_fileName,const FileRrRule* &r_fileRrRule)
{
  r_fileRrRule = m_config.getRrRule(r_fileName);
  if(r_fileRrRule==NULL)
  {
    #ifdef _DEBUG_
      cout<<"m_config.getRrRule("<<r_fileName<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    char t_errMsg[256];
    //告警，并将该文件移到错误目录中
    sprintf(t_errMsg,"无对应的排重规则:{%s}",r_fileName);
    warning(E_RR_PARAM_NORRRULE,t_errMsg);
    char t_errorFileName[PATH_MAX];
    char t_srcFile_fullName[PATH_MAX];
    sprintf(t_errorFileName    ,"%s/%s" , m_config.m_errPath.c_str() , r_fileName);
    sprintf(t_srcFile_fullName ,"%s/%s" , m_config.m_inputPath.c_str() , r_fileName);
    if(rename(t_srcFile_fullName,t_errorFileName)<0)
    {
      cpFile(t_srcFile_fullName,t_errorFileName);
      unlink(t_srcFile_fullName);
    }
    m_dir.setDealFlag();
    return false;
  }
  if(m_srcFileNames.empty())
  {
    m_lastFileType_merge= r_fileRrRule->m_fileType_merge;
    m_lastRrRule = r_fileRrRule; //add by gaojf 2009-2-19 3:30
    time(&m_curDealTime);
    if(m_openFlag == true)
    {
      theLogger->pEnd(theLoggerProcessID);
    }
    theLoggerProcessID = theLogger->pBegin();
    m_openFlag = true;
    return true;
  }else
  {
    if(r_fileRrRule->m_fileType_merge == m_lastFileType_merge)
    {
      return true;
    }else
    {
      if(m_firstFileFlag == false)
      { 
        m_firstFileFlag = true;
        time(&m_firstFileTime);
      }
      return false;
    }
  }
}


void RrApplication::rewind()
{
  m_dir.rewind();
  m_firstFileFlag = false;
}

void RrApplication::generateFileName(const char *r_fileName)
{
  if(m_srcFileNames.empty()==false)
  { 
    return;  
  }
  //第一个文件记录开始处理时间
	char t_sFileName[PATH_MAX];
	strcpy(t_sFileName,r_fileName);          
	for (int k=0; ;k++) 
	{
		sprintf(m_uniqueFileName, "%s/%s", m_config.m_uniqPath.c_str(), t_sFileName);
		sprintf(m_repeatFileName, "%s/%s", m_config.m_repeatPath.c_str(), t_sFileName);
		sprintf(m_uniqueFileName_tmp, "%s/#%s", m_config.m_uniqPath.c_str(), t_sFileName);
		sprintf(m_repeatFileName_tmp, "%s/#%s", m_config.m_repeatPath.c_str(), t_sFileName);
		
		
		//如果没有重复文件名，就跳出循环
		if(access(m_uniqueFileName, F_OK) != 0 && 
		   access(m_repeatFileName, F_OK) != 0)
			break;
			
		//否则修改文件名
		if(m_config.getRrRule(r_fileName)->m_rrType != E_RR_RRTYPE_FILE) //added by hanyu 08-12-02
		  sprintf(t_sFileName, "%s.%d",r_fileName,k+1);
		else
		  break;
	}
	
	sprintf(m_workFile_fullName,"%s/#%s", m_config.m_workPath.c_str(), t_sFileName);  
	sprintf(m_errorFileName    ,"%s/%s" , m_config.m_errPath.c_str() , t_sFileName);
	sprintf(m_errorFileName_tmp,"%s/#%s", m_config.m_errPath.c_str() ,t_sFileName);
	if(m_mergFile.is_open())
	{ //防止第一个文件打开后，处理过程中又失效的情况
	  m_mergFile.close();
	  m_mergFile.clear(); 
	}
	m_mergFile.open(m_workFile_fullName,ios::out);
  if(!m_mergFile)
  {
    RR_THROW_CP1(E_RR_FILE_OPEN,"无效的目录:{%s}!",m_workFile_fullName);
  }	
}

void RrApplication::addFilterInfo(const FilterInfo &r_filterInfo)
{
  if(m_fidCount<m_fidCapcity)
  {
    m_fidInfoList[m_fidCount]=r_filterInfo;
    ++m_fidCount;
  }else
  {
    m_fidInfoList.push_back(r_filterInfo);
    ++m_fidCount;
    ++m_fidCapcity;
  }
}
bool RrApplication::outputCdr()
{
  size_t   t_lineNum=0;
  ofstream t_uniqFile,t_repeatFile,t_errFile;
  bool     t_bRet = true;
  const char *t_pErrFile=NULL;
  
	m_mergFile.open(m_workFile_fullName,ios::in);
  if(!m_mergFile)
  {
    #ifdef _DEBUG_
      cout<<"open file :{"<<m_workFile_fullName<<"} failed!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_workFile_fullName);
    return false;
  }
  while(!m_mergFile.eof())
  {
    m_mergFile.getline(m_lineBuff,E_RR_LINEMAXLEN);
    if(m_lineBuff[0]==0)
    {
      continue;
    }
    //不需排重或非重单，则输出到正常文件中
    if(m_fidInfoList[t_lineNum].m_uniqFlag==E_RR_REPFLAG_NONE ||
       m_fidInfoList[t_lineNum].m_uniqFlag==E_RR_REPFLAG_UNIQ)
    {
      if(!(t_uniqFile.rdbuf()->is_open()))
      {
        t_uniqFile.open(m_uniqueFileName_tmp,ios::out);
        if (t_uniqFile.fail())
        {
          #ifdef _DEBUG_
            cout<<"open file {"<<m_uniqueFileName_tmp<<"} false!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          t_bRet = false;
          t_pErrFile = m_uniqueFileName_tmp;
          break;
        }
      }
      t_uniqFile<<m_lineBuff<<"\n";
      m_rrLog.m_uniqCdr++;
    }else if(m_fidInfoList[t_lineNum].m_uniqFlag==E_RR_REPFLAG_ERROR)
    { //错单
      if(!(t_errFile.rdbuf()->is_open()))
      {
        t_errFile.open(m_errorFileName_tmp,ios::out);
        if (t_errFile.fail())
        {
          #ifdef _DEBUG_
            cout<<"open file {"<<m_errorFileName_tmp<<"} false!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          t_bRet = false;
          t_pErrFile = m_errorFileName_tmp;
          break;
        }
      }
      t_errFile<<m_lineBuff<<"\n";
      m_rrLog.m_errCdr++;
    }else //否则输出到重单文件中
    {
      if(!(t_repeatFile.rdbuf()->is_open()))
      {
        t_repeatFile.open(m_repeatFileName_tmp,ios::out);
        if (t_repeatFile.fail())
        {
          #ifdef _DEBUG_
            cout<<"open file {"<<m_repeatFileName_tmp<<"} false!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          t_bRet = false;
          t_pErrFile = m_repeatFileName_tmp;
          break;
        }
      }
      if(m_fidInfoList[t_lineNum].m_uniqFlag==E_RR_REPFLAG_INFILE)
      {
        t_repeatFile<<"F,"<<m_lineBuff<<"\n";
        m_rrLog.m_repeatInFile++;
      }else if(m_fidInfoList[t_lineNum].m_uniqFlag==E_RR_REPFLAG_INMEM)
      {
        t_repeatFile<<"M,"<<m_lineBuff<<"\n";
        m_rrLog.m_repeatHis++;
      }else
      {
        t_repeatFile<<"H,"<<m_lineBuff<<"\n";
        m_rrLog.m_repeatHis++;
      }
    }
    ++t_lineNum;
  };
  m_mergFile.close();
  m_mergFile.clear();
  if(t_repeatFile.rdbuf()->is_open())
  {
    t_uniqFile.close();
    t_uniqFile.clear();
  }
  if(t_repeatFile.rdbuf()->is_open())
  {
    t_repeatFile.close();
    t_repeatFile.clear();
  }
  unlink(m_workFile_fullName);
  if(t_bRet==false)
  {
    unlinkTmpFile();
    RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",t_pErrFile);
  }
  return t_bRet;
}

bool RrApplication::outputFile()//added by hanyu 08-11-28
{
	size_t   t_lineNum=0;
	bool t_bRet = true;
	const char *t_pErrFile=NULL;
	//sprintf(m_srcFile_fullName ,"%s/%s", m_config.m_inputPath.c_str(), m_fidInfoList[0].m_fid);
	sprintf(m_srcFile_fullName ,"%s/%s", m_config.m_inputPath.c_str(), m_srcFileNames[0].c_str());
	if(m_fidInfoList[0].m_uniqFlag==E_RR_REPFLAG_NONE ||
     m_fidInfoList[0].m_uniqFlag==E_RR_REPFLAG_UNIQ)
  {
  	int t_uniqCdr = getCdrNum(m_srcFile_fullName);
  	if(rename(m_srcFile_fullName,m_uniqueFileName_tmp) == -1)
  	{
  		if(cpFile(m_srcFile_fullName,m_uniqueFileName_tmp)==false)
  		{
  		  t_bRet = false;
  		  t_pErrFile = m_srcFile_fullName;
  		}else
  		{
  		  unlink(m_srcFile_fullName);
  		  m_rrLog.m_uniqCdr = t_uniqCdr;
  		}
  	}
  	else
  	{
  		m_rrLog.m_uniqCdr = t_uniqCdr;
    }
  }
  else if(m_fidInfoList[0].m_uniqFlag==E_RR_REPFLAG_ERROR)
  {
  	int t_errCdr = getCdrNum(m_srcFile_fullName);
  	if(rename(m_srcFile_fullName,m_errorFileName_tmp) == -1)
  	{
  		if(cpFile(m_srcFile_fullName,m_errorFileName_tmp)==false)
  		{
    		t_bRet = false;
    		t_pErrFile = m_srcFile_fullName;
  		}else
  		{
  		  unlink(m_srcFile_fullName);
  		  m_rrLog.m_errCdr += t_errCdr;
  		}
  	}
  	{
  	  m_rrLog.m_errCdr += t_errCdr;
    }
  }
  else
  {
  	int t_repeatCdr = getCdrNum(m_srcFile_fullName);
  	if(rename(m_srcFile_fullName,m_repeatFileName_tmp) == -1)
  	{
  		if(cpFile(m_srcFile_fullName,m_repeatFileName_tmp)==false)
  		{
    		t_bRet = false;
    		t_pErrFile = m_srcFile_fullName;
  		}else
  		{
  		  unlink(m_srcFile_fullName);
  		  m_rrLog.m_repeatHis += t_repeatCdr;
  		}
  	}
  	else
  	{
  	  m_rrLog.m_repeatHis += t_repeatCdr;
    }
  }             
  //unlink(m_srcFile_fullName);
  unlink(m_workFile_fullName);
  if(t_bRet == false)
  {
  	unlinkTmpFile();
    RR_THROW_CP1(E_RR_FILE_OPEN,"移动文件失败:{%s}!",t_pErrFile);
  }
	return t_bRet;
}

bool RrApplication::commitFile()
{
  for(vector<string>::const_iterator t_strItr=m_srcFileNames.begin();
      t_strItr!=m_srcFileNames.end();++t_strItr)
  {
    sprintf(m_srcFile_fullName ,"%s/%s", m_config.m_inputPath.c_str(), t_strItr->c_str());
    unlink(m_srcFile_fullName);
  }
  if(m_rrLog.m_uniqCdr >0)
    rename(m_uniqueFileName_tmp,m_uniqueFileName);
  if(m_rrLog.m_repeatInFile+m_rrLog.m_repeatHis >0)
    rename(m_repeatFileName_tmp,m_repeatFileName);
  if(m_rrLog.m_errCdr > 0)
    rename(m_errorFileName_tmp,m_errorFileName);
  return true;  
}

//更新状态
void RrApplication::updateRtStatus(int r_state, const char* r_objectName, const char *r_remark)
{
  m_objectName = r_objectName;
  m_reserved = r_remark;
  m_status = r_state;

  updateStatus();
}
//判断是否为已经处理过的文件
bool RrApplication::isDealedFile(const char * r_fileName)
{
  for(vector<string>::const_iterator r_itr=m_srcFileNames.begin();
      r_itr!=m_srcFileNames.end();++r_itr)
  {
    if(strcmp(r_itr->c_str(),r_fileName)==0)
    {
      return true;
    }
  }
  return false;
}


/**
 * cpFile    复制一个文件.
 * @param   srcFileName: 源文件名
 * @param   desFileName：目标文件名
 * @return  true 成功, false 失败
 * 注： 采用%开头作为临时文件名
 */
bool RrApplication::cpFile(const char *srcFileName,const char *desFileName)
{
  const int FTPDMSGLEN=1024*8;
  const int FILENAMELEN=256;
  const char DIRSEPERATECHAR = '/';
  FILE *srcFile,*desFile;
  int readSize;
  char msgBuff[FTPDMSGLEN+1];
  char tmpFile[FILENAMELEN+1];
  int  sLen,tLen;
  
  if(strrchr(desFileName,DIRSEPERATECHAR)==NULL)
  {
    sprintf(tmpFile,"%%%s",desFileName);
  }else
  {
    sprintf(tmpFile,"%s",desFileName);
    tLen= strlen(desFileName);
    sLen = strlen(strrchr(desFileName,DIRSEPERATECHAR)+1);
    sprintf(tmpFile+tLen-sLen,"%%%s",strrchr(desFileName,DIRSEPERATECHAR)+1);
  } 
  if((srcFile=fopen(srcFileName,"rb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<srcFileName<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if((desFile=fopen(tmpFile,"wb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    fclose(srcFile);
    return false;
  }
  while(!feof(srcFile))
  {
    readSize=fread(msgBuff,1,FTPDMSGLEN,srcFile);
    if(readSize == 0) break;
    if(fwrite(msgBuff,1,readSize,desFile)!=readSize)
    {
      fclose(srcFile);
      fclose(desFile);
      return false;
    }
  }
  fclose(srcFile);
  fclose(desFile);
  if(rename(tmpFile,desFileName)!=0)
  {
    #ifdef _DEBUG_
      cout<<"rename file:"<<tmpFile<<" "<<desFileName<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

