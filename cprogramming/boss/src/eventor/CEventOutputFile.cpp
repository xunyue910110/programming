#include "CEventOutputFile.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include <unistd.h>


extern ErrorMessages *const theErrorMessages;
extern ErrorMessages g_errormessages;
extern log4cpp::Logger *theLogger;


CEventOutputFile::CEventOutputFile()
{
    m_eventConfig =NULL;
}

    
CEventOutputFile::~CEventOutputFile()
{
}    


void CEventOutputFile::initialize(//const string& performId,
                                       EventConfig *eventConfig)
{
    //初始化配置信息、日志信息
    m_eventConfig = eventConfig;
    //m_performId = performId;
    m_rateReadyFlag = false;
    
    
    //文件头名称
    if((m_eventConfig->m_eventorType=="0") ||
       (m_eventConfig->m_eventorType=="1") ||
       (m_eventConfig->m_eventorType=="3"))
    {
        m_fileHead = "URP";
    }
    else if(m_eventConfig->m_eventorType=="2")//一次性费用计算
    {
        m_fileHead = "OTP";
    }
    else if(m_eventConfig->m_eventorType=="7" ||
            m_eventConfig->m_eventorType=="11")//包月费订购关系M文件
    {
        m_fileHead = "MRSP";
    }
    else if(m_eventConfig->m_eventorType=="8")//包月费订购关系ADC，A文件
    {
        m_fileHead = "ARSP";
    }
    else if(m_eventConfig->m_eventorType=="9")//包月费订购关系ADC，B文件
    {
        m_fileHead = "BRSP";
    }
    else if(m_eventConfig->m_eventorType=="11")//包月费清退E文件
    {
        m_fileHead = "ERSP";
    }
	m_ugpFileHead = "URG";   
	m_acctFileHead = "URA";   
    char channelNo[2+1];
    string  strchannelNo;
    //初始化输出结构信息    
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        m_outputFileInfo[i].m_idNum = 0;
        m_outputFileInfo[i].m_fileStat = 0;
        m_outputFileInfo[i].m_batchNo = 0;
        
        sprintf(channelNo, "%02d", i);
        strchannelNo = channelNo;
        
        memset(channelNo, 0, sizeof(channelNo));
        
        m_outputFileInfo[i].m_workFileName = m_eventConfig->m_workFilePath + m_fileHead + strchannelNo;

		//limeng 20090814 用户群
        m_ugpOutputFileInfo[i].m_idNum = 0;
        m_ugpOutputFileInfo[i].m_fileStat = 0;
        m_ugpOutputFileInfo[i].m_batchNo = 0;

        m_ugpOutputFileInfo[i].m_workFileName = m_eventConfig->m_workFilePath + m_ugpFileHead + strchannelNo;

		//limeng 20090902 账户
        m_acctOutputFileInfo[i].m_idNum = 0;
        m_acctOutputFileInfo[i].m_fileStat = 0;
        m_acctOutputFileInfo[i].m_batchNo = 0;

        m_acctOutputFileInfo[i].m_workFileName = m_eventConfig->m_workFilePath + m_acctFileHead + strchannelNo;
    }
}    


bool CEventOutputFile::dispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator)
{        
    //如果文件没有打开，打开文件
    if (m_outputFileInfo[channelNo].m_fileStat == 0)
    {
        if (!openFile(channelNo))
        {
        	#ifdef _DEBUG_
            cout << "openFile false" << endl;
          #endif
            return false;
        }    
        m_outputFileInfo[channelNo].m_fileStat = 1;
    }
    
    //limeng modified 20090813 输出数据
    if(m_eventConfig->m_eventorType!="7"&&
        m_eventConfig->m_eventorType!="8"&&
        m_eventConfig->m_eventorType!="9"&&
        m_eventConfig->m_eventorType!="10"&&
        m_eventConfig->m_eventorType!="11"&&
        m_eventConfig->m_eventorType!="2")//重出帐文件
    {
        eventInfoCreator.outReAcct(channelNo, m_outputFileInfo[channelNo].m_workOfStream, 21);
    }
    else if (m_eventConfig->m_eventorType=="2")//一次性费用计算
    {
        eventInfoCreator.outOtp(channelNo, m_outputFileInfo[channelNo].m_workOfStream);
    }
    //7、M文件；8、A文件；9、B文件
    else if((m_eventConfig->m_eventorType=="7")||(m_eventConfig->m_eventorType=="8")||(m_eventConfig->m_eventorType=="9"))
    {
      eventInfoCreator.outMonfeeSp(channelNo, m_outputFileInfo[channelNo].m_workOfStream,BIZ_TYPE_MONFEE);
    }
    //10、包月费E文件，11、包月费增量文件
    else if((m_eventConfig->m_eventorType=="10")||(m_eventConfig->m_eventorType=="11"))
    {
      eventInfoCreator.outMonfeeSp(channelNo, m_outputFileInfo[channelNo].m_workOfStream,BIZ_TYPE_MONFEE);
    }
    //记录户数
    m_outputFileInfo[channelNo].m_idNum += 1;
    
    if ((m_outputFileInfo[channelNo].m_idNum > 0 )
        && (m_outputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum == 0))
    {
        //cout << "m_outputFileInfo[" << channelNo 
        //     << "].m_idNum%m_eventConfig->m_autoOutputFileUserNum=" 
        //     << m_outputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum
        //     << endl;
        closeFile(channelNo);
        if (!linkUnlink(channelNo,dealCycId,dealDate))
        {
        	#ifdef _DEBUG_
            cout << "linkUnlink false" << endl;
          #endif
            return false;
        }
        
        m_outputFileInfo[channelNo].m_idNum = 0;
    }
    
    return true;    
}

//limeng 20090814 用户群
bool CEventOutputFile::ugpDispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator)
{        
    //如果文件没有打开，打开文件
    if (m_ugpOutputFileInfo[channelNo].m_fileStat == 0)
    {
        if (!ugpOpenFile(channelNo))
        {
        	#ifdef _DEBUG_
            cout << "openFile false" << endl;
          #endif
            return false;
        }    
        m_ugpOutputFileInfo[channelNo].m_fileStat = 1;
    }
    
    //limeng modified 20090813 输出数据
    if((m_eventConfig->m_eventorType=="4")
    	||(m_eventConfig->m_eventorType=="1")
    	||(m_eventConfig->m_eventorType=="3"))//重出帐文件
    {
        eventInfoCreator.outReAcct(channelNo, m_ugpOutputFileInfo[channelNo].m_workOfStream, 25);
    }
    
    //记录户数
    m_ugpOutputFileInfo[channelNo].m_idNum += 1;
    
    if ((m_ugpOutputFileInfo[channelNo].m_idNum > 0 )
        && (m_ugpOutputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum == 0))
    {
        //cout << "m_ugpOutputFileInfo[" << channelNo 
        //     << "].m_idNum%m_eventConfig->m_autoOutputFileUserNum=" 
        //     << m_ugpOutputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum
        //     << endl;
        ugpCloseFile(channelNo);
        if (!ugpLinkUnlink(channelNo,dealCycId,dealDate))
        {
        	#ifdef _DEBUG_
            cout << "linkUnlink false" << endl;
          #endif
            return false;
        }
        
        m_ugpOutputFileInfo[channelNo].m_idNum = 0;
    }
    
    return true;    
}

void CEventOutputFile::closeFile()
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        closeFile(i);
    }    
}

void CEventOutputFile::ugpCloseFile()
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        ugpCloseFile(i);
    }    
}

void CEventOutputFile::closeFile(const int& channelNo)
{
    if (m_outputFileInfo[channelNo].m_fileStat == 1)
    {
        //关闭流
        m_outputFileInfo[channelNo].m_workOfStream.close();
        m_outputFileInfo[channelNo].m_workOfStream.clear();
    }    
}

void CEventOutputFile::ugpCloseFile(const int& channelNo)
{
    if (m_ugpOutputFileInfo[channelNo].m_fileStat == 1)
    {
        //关闭流
        m_ugpOutputFileInfo[channelNo].m_workOfStream.close();
        m_ugpOutputFileInfo[channelNo].m_workOfStream.clear();
    }    
}

bool CEventOutputFile::linkUnlink(const T_CYCLE& cycId, const string& dealDate)
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        if (!linkUnlink(i, cycId, dealDate))
        {
            return false;
        }
    }    
    return true;
}

bool CEventOutputFile::ugpLinkUnlink(const T_CYCLE& cycId, const string& dealDate)
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        if (!ugpLinkUnlink(i, cycId, dealDate))
        {
            return false;
        }
    }    
    return true;
}

bool CEventOutputFile::linkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate)
{
    char    fileName[100+1];
    char    batchNo[6+1];
    string  strBacthNo;
    string  strFileName;
    memset(fileName, 0, sizeof(fileName));
    memset(batchNo, 0, sizeof(batchNo));
    
    sprintf(batchNo, "%04d", m_outputFileInfo[channelNo].m_batchNo);
    strBacthNo = batchNo;
    
    m_outputFileInfo[channelNo].m_batchNo++;
//    cout << "m_outputFileInfo[" << channelNo << "].m_batchNo =" 
//         << m_outputFileInfo[channelNo].m_batchNo << endl;
    
    sprintf(fileName, "%s", m_fileHead.c_str());
    sprintf(fileName+strlen(m_fileHead.c_str()), "%02d_%06d", channelNo,(long)cycId);
    strFileName = fileName + dealDate.substr(4,10) + strBacthNo;
    //cout << "dealDate.substr(4,10)=" << dealDate.substr(4,10) << endl;
    //cout << "m_outputFileInfo["<<channelNo<<"].m_outputFileName="
    //     << m_outputFileInfo[channelNo].m_outputFileName << endl
    //     << "m_eventConfig->m_outputEventFilePath +strFileName" 
    //     << m_eventConfig->m_outputEventFilePath +strFileName << endl;
    //cout << "m_outputFileInfo[" << channelNo<< "].m_outputFileName=" 
    //     << m_outputFileInfo[channelNo].m_outputFileName << endl;
    

    memset(batchNo, 0, sizeof(batchNo));
    memset(fileName, 0, sizeof(fileName));
    
        //如果该类型文件无输出，继续
    if (m_outputFileInfo[channelNo].m_fileStat == 0)
    {
        return true;
    }
        
        //如果该类型工作文件中数据记录数大于0，改名为输出文件    
    if (m_outputFileInfo[channelNo].m_idNum > 0)
    {
      //limeng 20090812 用户事件转移到批价目录
//      	cout << "copy" + m_outputFileInfo[channelNo].m_workFileName + " ->" + 
//                  m_outputFileInfo[channelNo].m_outputFileName<< endl; 
          
//		if(access(m_outputFileInfo[channelNo].m_outputFileName.c_str(), F_OK) < 0)
//		{
//			if(-1==mkdir(m_outputFileInfo[channelNo].m_outputFileName.c_str(),0777))
//			{
//		      theErrorMessages->insert(ERROR_LINK_UNLINK,
//		              "mkdir " + m_outputFileInfo[channelNo].m_outputFileName);    
//		      return false;
//			}	
//		}
		string destFile,bakdestFile;
		if(m_rateReadyFlag)
		{
			m_outputFileInfo[channelNo].m_outputFileName = m_outputFileInfo[channelNo].m_outputPath + strFileName;
		  	m_outputFiles.push_back(m_outputFileInfo[channelNo].m_outputFileName); 
		  	destFile =  m_outputFileInfo[channelNo].m_outputFileName;
	    }
	    else
	    {
		    //limeng 20090902 先转移到中间目录
		    m_outputFileInfo[channelNo].m_outputBakupFileName = m_eventConfig->m_outputEventFilePath +strFileName;
		  	destFile =  m_outputFileInfo[channelNo].m_outputBakupFileName;
	    }
	    
	  //add by wanglu 2009-12-9
	  if (m_eventConfig->m_isBackupValidFile != 0)
	  {
	  	bakdestFile = m_eventConfig->m_backupFilePath + strFileName;
      if(copy(m_outputFileInfo[channelNo].m_workFileName.c_str(),bakdestFile.c_str())!=0)
      {
        #ifdef _DEBUG_
        cout<<"link "<<m_outputFileInfo[channelNo].m_workFileName.c_str()<<" "<<bakdestFile.c_str()<<" false!"<<endl;
        #endif
        return false;
      }
	  }
	  //end 2009-12-9	  
	  
	  if (access(m_outputFileInfo[channelNo].m_workFileName.c_str(), F_OK) == 0&&
	  	  m_outputFileInfo[channelNo].m_idNum > 0) //add by xuf 2009-12-29 17:01:50
	  {
	  //limeng 20091228 rename不支持不同文件系统下操作
      char syscmd[1024]; 	
	  sprintf(syscmd,"mv %s %s",m_outputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      char cmdMsg[1024]; 	
	  sprintf(cmdMsg,"mv %s %s\n",m_outputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      theLogger->info(cmdMsg);
	  system(syscmd);
	 }
      
      /*//针对输出文件日志处理
      theLogger->pBeginOutput(m_performId,
                          m_outputFileInfo[channelNo].m_outputFileName.c_str(),
                          "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");          

      theLogger->pEndOutput(m_performId,
                          m_outputFileInfo[channelNo].m_outputFileName.c_str(),
                          m_outputFileInfo[channelNo].m_idNum,
                          "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=0",
                          m_outputFileInfo[channelNo].m_idNum,0);*/
    }
    //如果该类型工作文件中数据记录数等于0，直接删除
    else
    {
        if (unlink(m_outputFileInfo[channelNo].m_workFileName.c_str()) != 0)
        {
            theErrorMessages->insert(ERROR_LINK_UNLINK,
                    m_outputFileInfo[channelNo].m_workFileName + 
                    "--file unlink error");
            return false;                                
        }    
    }        
    
    //初始化输出文件
    m_outputFileInfo[channelNo].m_idNum = 0;
    m_outputFileInfo[channelNo].m_fileStat = 0;    
    //m_outputFileInfo[channelNo].m_batchNo = 0;    
    
    return true;                    
}
    
    
bool CEventOutputFile::ugpLinkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate)
{
    char    fileName[100+1];
    char    batchNo[6+1];
    string  strBacthNo;
    string  strFileName;
    memset(fileName, 0, sizeof(fileName));
    memset(batchNo, 0, sizeof(batchNo));
    
    sprintf(batchNo, "%04d", m_ugpOutputFileInfo[channelNo].m_batchNo);
    strBacthNo = batchNo;
    
    m_ugpOutputFileInfo[channelNo].m_batchNo++;
//    cout << "m_ugpOutputFileInfo[" << channelNo << "].m_batchNo =" 
//         << m_ugpOutputFileInfo[channelNo].m_batchNo << endl;
    
    sprintf(fileName, "%s", m_ugpFileHead.c_str());
    sprintf(fileName+strlen(m_ugpFileHead.c_str()), "%02d_%06d", channelNo,(long)cycId);
    strFileName = fileName + dealDate.substr(4,10) + strBacthNo;
    
    memset(batchNo, 0, sizeof(batchNo));
    memset(fileName, 0, sizeof(fileName));
    
        //如果该类型文件无输出，继续
    if (m_ugpOutputFileInfo[channelNo].m_fileStat == 0)
    {
        return true;
    }
        
        //如果该类型工作文件中数据记录数大于0，改名为输出文件    
    if (m_ugpOutputFileInfo[channelNo].m_idNum > 0)
    {
    	string destFile, bakdestFile;
		//limeng 20090814 用户群事件先转移到中间目录
		m_ugpOutputFileInfo[channelNo].m_outputBakupFileName = m_eventConfig->m_outputEventFilePath +strFileName;
	    destFile = m_ugpOutputFileInfo[channelNo].m_outputBakupFileName;
	    if(m_rateReadyFlag)
	    {
		    //最后输出路径
		    m_ugpOutputFileInfo[channelNo].m_outputFileName = m_ugpOutputFileInfo[channelNo].m_outputPath +strFileName;
	      	m_ugpBackupFiles.push_back(m_ugpOutputFileInfo[channelNo].m_outputBakupFileName);    
	      	m_ugpOutputFiles.push_back(m_ugpOutputFileInfo[channelNo].m_outputFileName);  
		    if(m_eventConfig->m_eventorType=="4")
		    	destFile = m_ugpOutputFileInfo[channelNo].m_outputFileName;
		}
		
	 //added by jlxu@NeiMeng Dec12,2009  UGP file back up
	  if (m_eventConfig->m_isBackupValidFile != 0)
	  {
	  	bakdestFile = m_eventConfig->m_backupFilePath + strFileName;
      if(copy(m_ugpOutputFileInfo[channelNo].m_workFileName.c_str(),bakdestFile.c_str())!=0)
      {
        #ifdef _DEBUG_
        cout<<"link "<<m_ugpOutputFileInfo[channelNo].m_workFileName.c_str()<<" "<<bakdestFile.c_str()<<" false!"<<endl;
        #endif
        return false;
      }
	  }
	  //end of amendment by jlxu@NeiMeng Dec12,2009	
    
    if (access(m_ugpOutputFileInfo[channelNo].m_workFileName.c_str(), F_OK) == 0&&
	  	  m_ugpOutputFileInfo[channelNo].m_idNum > 0) //add by xuf 2009-12-29 17:01:50
    {  	    
	  //limeng 20091228 rename不支持不同文件系统下操作
      char syscmd[1024]; 	
	  sprintf(syscmd,"mv %s %s",m_ugpOutputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      char cmdMsg[1024]; 	
	  sprintf(cmdMsg,"mv %s %s\n",m_ugpOutputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      theLogger->info(cmdMsg);
	  system(syscmd);
   }  
      /*//针对输出文件日志处理
      theLogger->pBeginOutput(m_performId,
                          m_ugpOutputFileInfo[channelNo].m_outputFileName.c_str(),
                          "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");          

      theLogger->pEndOutput(m_performId,
                          m_ugpOutputFileInfo[channelNo].m_outputFileName.c_str(),
                          m_ugpOutputFileInfo[channelNo].m_idNum,
                          "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=0",
                          m_ugpOutputFileInfo[channelNo].m_idNum,0);*/
    }
    //如果该类型工作文件中数据记录数等于0，直接删除
    else
    {
        if (unlink(m_ugpOutputFileInfo[channelNo].m_workFileName.c_str()) != 0)
        {
            theErrorMessages->insert(ERROR_LINK_UNLINK,
                    m_ugpOutputFileInfo[channelNo].m_workFileName + 
                    "--file unlink error");
            return false;                                
        }    
    }        
    
    //初始化输出文件
    m_ugpOutputFileInfo[channelNo].m_idNum = 0;
    m_ugpOutputFileInfo[channelNo].m_fileStat = 0;    
    //m_ugpOutputFileInfo[channelNo].m_batchNo = 0;    
    
    return true;                    
}
    
bool CEventOutputFile::openFile(const int &channelNo)
{
    //关闭清除输出流
    m_outputFileInfo[channelNo].m_workOfStream.close();
    m_outputFileInfo[channelNo].m_workOfStream.clear();
    
    //将输出流和输出工作文件关联
    m_outputFileInfo[channelNo].m_workOfStream.open(
                m_outputFileInfo[channelNo].m_workFileName.c_str(),ios::out);
    if (m_outputFileInfo[channelNo].m_workOfStream.rdstate() != ios::goodbit)
    {
        m_outputFileInfo[channelNo].m_workOfStream.close();
        theErrorMessages->insert(ERROR_OPEN_FILE,
              m_outputFileInfo[channelNo].m_workFileName + "--file open error");    
        
        return false;
    }    
    
    return true;
}

bool CEventOutputFile::ugpOpenFile(const int &channelNo)
{
    //关闭清除输出流
    m_ugpOutputFileInfo[channelNo].m_workOfStream.close();
    m_ugpOutputFileInfo[channelNo].m_workOfStream.clear();
    
    //将输出流和输出工作文件关联
    m_ugpOutputFileInfo[channelNo].m_workOfStream.open(
                m_ugpOutputFileInfo[channelNo].m_workFileName.c_str(),ios::out);
    if (m_ugpOutputFileInfo[channelNo].m_workOfStream.rdstate() != ios::goodbit)
    {
        m_ugpOutputFileInfo[channelNo].m_workOfStream.close();
        theErrorMessages->insert(ERROR_OPEN_FILE,
              m_ugpOutputFileInfo[channelNo].m_workFileName + "--file open error");    
        
        return false;
    }    
    
    return true;
}

//limeng add 20090812 用户重出帐事件是否处理完
bool CEventOutputFile::isUserEventFinished()
{
//	cout << "CEventOutputFile::isUserEventFinished" <<endl;
	
    for (int i=0; i<m_outputFiles.size(); i++)
    {
//    	cout<<"CEventOutputFile:"<<m_outputFiles[i] <<endl;
        if (access(m_outputFiles[i].c_str(), F_OK) == 0)
        {
            return false;
        }
    }
        
    return true;
}

//limeng add 20090812 复制用户群重出帐事件
bool CEventOutputFile::linkUgpEvent()
{
//	cout << "CEventOutputFile::linkUgpEvent" <<endl;
	
    for (int i=0; i<m_ugpBackupFiles.size(); i++)
    {
//	cout << "copy " + m_ugpBackupFiles[i] + " ->" + 
//                  m_ugpOutputFiles[i] <<endl;
    	if (access(m_ugpBackupFiles[i].c_str(), F_OK) != 0)
    		continue;
   
	  //limeng 20091228 rename不支持不同文件系统下操作
      char syscmd[1024]; 	
	  sprintf(syscmd,"mv %s %s",m_ugpBackupFiles[i].c_str(),
                 m_ugpOutputFiles[i].c_str());
      char cmdMsg[1024]; 	
	  sprintf(cmdMsg,"mv %s %s\n",m_ugpBackupFiles[i].c_str(),
                 m_ugpOutputFiles[i].c_str());
      theLogger->info(cmdMsg);
	  system(syscmd);
    }
//    m_outputFiles.clear();
//    m_ugpBackupFiles.clear();
//    m_ugpOutputFiles.clear();
    return true;   
}

//limeng add 20090812 将通道对应的批价目录保存到m_outputFileInfo
bool CEventOutputFile::setOutFile(const int& r_channelNo,const string& r_subDir)
{
	m_outputFileInfo[r_channelNo].m_outputPath = m_eventConfig->m_outputRateFilePath +r_subDir + "/uniq/";
	m_acctOutputFileInfo[r_channelNo].m_outputPath = m_ugpOutputFileInfo[r_channelNo].m_outputPath = m_outputFileInfo[r_channelNo].m_outputPath;
	m_rateReadyFlag = true;
	return true;
}

int CEventOutputFile::copy(const char *r_srcFile,const char *r_desFile) const
{
  FILE *srcFile,*desFile;
  int readSize;
  const int BUFFLEN=1024;
  char msgBuff[BUFFLEN+1];
  char tmpFile[256+1];
  int  sLen,tLen;
  
  if(strrchr(r_desFile,'/')==NULL)
  {
    sprintf(tmpFile,"%%%s",r_desFile);
  }else
  {
    sprintf(tmpFile,"%s",r_desFile);
    tLen= strlen(r_desFile);
    sLen = strlen(strrchr(r_desFile,'/')+1);
    sprintf(tmpFile+tLen-sLen,"%%%s",strrchr(r_desFile,'/')+1);
  } 
  if((srcFile=fopen(r_srcFile,"rb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<r_srcFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  if((desFile=fopen(tmpFile,"wb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    fclose(srcFile);
    return -1;
  }
  while(!feof(srcFile))
  {
    readSize=fread(msgBuff,1,BUFFLEN,srcFile);
    if(readSize == 0) break;
    if(fwrite(msgBuff,1,readSize,desFile)!=readSize)
    {
      fclose(srcFile);
      fclose(desFile);
      return -1;
    }
  }
  fclose(srcFile);
  fclose(desFile);
  if(rename(tmpFile,r_desFile)!=0)
  {
    #ifdef _DEBUG_
      cout<<"copy file:"<<tmpFile<<" "<<r_desFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  return 0;
}

bool CEventOutputFile::acctDispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator)
{
    //如果文件没有打开，打开文件
    if (m_acctOutputFileInfo[channelNo].m_fileStat == 0)
    {
        if (!acctOpenFile(channelNo))
        {
        	#ifdef _DEBUG_
            cout << "openFile false" << endl;
          #endif
            return false;
        }    
        m_acctOutputFileInfo[channelNo].m_fileStat = 1;
    }
    
    //limeng modified 20090813 输出数据
    if((m_eventConfig->m_eventorType !="2"))//重出帐文件
    {
        eventInfoCreator.outReAcct(channelNo, m_acctOutputFileInfo[channelNo].m_workOfStream, 26);
    }
    
    //记录户数
    m_acctOutputFileInfo[channelNo].m_idNum += 1;
    
    if ((m_acctOutputFileInfo[channelNo].m_idNum > 0 )
        && (m_acctOutputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum == 0))
    {
        //cout << "m_acctOutputFileInfo[" << channelNo 
        //     << "].m_idNum%m_eventConfig->m_autoOutputFileUserNum=" 
        //     << m_acctOutputFileInfo[channelNo].m_idNum%m_eventConfig->m_autoOutputFileUserNum
        //     << endl;
        acctCloseFile(channelNo);
        if (!acctLinkUnlink(channelNo,dealCycId,dealDate))
        {
        	#ifdef _DEBUG_
            cout << "linkUnlink false" << endl;
          #endif
            return false;
        }
        
        m_acctOutputFileInfo[channelNo].m_idNum = 0;
    }
    
    return true;    
}
        
void CEventOutputFile::acctCloseFile()
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        acctCloseFile(i);
    }    
}
        
bool CEventOutputFile::acctLinkUnlink(const T_CYCLE& cycId, const string& dealDate)
{
    for (int i=0; i<MAX_OUTFILE_CHANNELNO; i++)
    {
        if (!acctLinkUnlink(i, cycId, dealDate))
        {
            return false;
        }
    }    
    return true;
}
        
bool CEventOutputFile::acctOpenFile(const int &channelNo)
{
    //关闭清除输出流
    m_acctOutputFileInfo[channelNo].m_workOfStream.close();
    m_acctOutputFileInfo[channelNo].m_workOfStream.clear();
    
    //将输出流和输出工作文件关联
    m_acctOutputFileInfo[channelNo].m_workOfStream.open(
                m_acctOutputFileInfo[channelNo].m_workFileName.c_str(),ios::out);
    if (m_acctOutputFileInfo[channelNo].m_workOfStream.rdstate() != ios::goodbit)
    {
        m_acctOutputFileInfo[channelNo].m_workOfStream.close();
        theErrorMessages->insert(ERROR_OPEN_FILE,
              m_acctOutputFileInfo[channelNo].m_workFileName + "--file open error");    
        
        return false;
    }    
    
    return true;
}
                       
void CEventOutputFile::acctCloseFile(const int& channelNo)
{
    if (m_acctOutputFileInfo[channelNo].m_fileStat == 1)
    {
        //关闭流
        m_acctOutputFileInfo[channelNo].m_workOfStream.close();
        m_acctOutputFileInfo[channelNo].m_workOfStream.clear();
    }    
}
        
bool CEventOutputFile::acctLinkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate)
{
    char    fileName[100+1];
    char    batchNo[6+1];
    string  strBacthNo;
    string  strFileName;
    memset(fileName, 0, sizeof(fileName));
    memset(batchNo, 0, sizeof(batchNo));
    
    sprintf(batchNo, "%04d", m_acctOutputFileInfo[channelNo].m_batchNo);
    strBacthNo = batchNo;
    
    m_acctOutputFileInfo[channelNo].m_batchNo++;
//    cout << "m_acctOutputFileInfo[" << channelNo << "].m_batchNo =" 
//         << m_acctOutputFileInfo[channelNo].m_batchNo << endl;
    
    sprintf(fileName, "%s", m_acctFileHead.c_str());
    sprintf(fileName+strlen(m_acctFileHead.c_str()), "%02d_%06d", channelNo,(long)cycId);
    strFileName = fileName + dealDate.substr(4,10) + strBacthNo;
    
    memset(batchNo, 0, sizeof(batchNo));
    memset(fileName, 0, sizeof(fileName));
    
        //如果该类型文件无输出，继续
    if (m_acctOutputFileInfo[channelNo].m_fileStat == 0)
    {
        return true;
    }
        
        //如果该类型工作文件中数据记录数大于0，改名为输出文件    
    if (m_acctOutputFileInfo[channelNo].m_idNum > 0)
    {
    	string destFile, bakdestFile;
	    //limeng 20090814 账户事件先转移到中间目录
	    m_acctOutputFileInfo[channelNo].m_outputBakupFileName = m_eventConfig->m_outputEventFilePath +strFileName;
	    destFile = m_acctOutputFileInfo[channelNo].m_outputBakupFileName;
		if(m_rateReadyFlag)
		{
		    //最后输出路径
		    m_acctOutputFileInfo[channelNo].m_outputFileName = m_acctOutputFileInfo[channelNo].m_outputPath +strFileName;
	      	m_acctBackupFiles.push_back(m_acctOutputFileInfo[channelNo].m_outputBakupFileName);    
	      	m_acctOutputFiles.push_back(m_acctOutputFileInfo[channelNo].m_outputFileName);  
		    if(m_eventConfig->m_eventorType=="5")
		    	destFile = m_acctOutputFileInfo[channelNo].m_outputFileName;
	    }
	    
	    //added by jlxu@NeiMeng Dec12,2009  URA file back up
	  if (m_eventConfig->m_isBackupValidFile != 0)
	  {
	  	bakdestFile = m_eventConfig->m_backupFilePath + strFileName;
      if(copy(m_acctOutputFileInfo[channelNo].m_workFileName.c_str(),bakdestFile.c_str())!=0)
      {
        #ifdef _DEBUG_
        cout<<"link "<<m_acctOutputFileInfo[channelNo].m_workFileName.c_str()<<" "<<bakdestFile.c_str()<<" false!"<<endl;
        #endif
        return false;
      }
	  }
	  //end of amendment by jlxu@NeiMeng Dec12,2009
                
   	if (access(m_acctOutputFileInfo[channelNo].m_workFileName.c_str(), F_OK) == 0&&
	  	  m_acctOutputFileInfo[channelNo].m_idNum > 0) //add by xuf 2009-12-29 17:01:50
	  {     
	  //limeng 20091228 rename不支持不同文件系统下操作
      char syscmd[1024]; 	
	  sprintf(syscmd,"mv %s %s",m_acctOutputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      char cmdMsg[1024]; 	
	  sprintf(cmdMsg,"mv %s %s\n",m_acctOutputFileInfo[channelNo].m_workFileName.c_str(),
                 destFile.c_str());
      theLogger->info(cmdMsg);
	  system(syscmd);
	 }//if
	  
      
      /*//针对输出文件日志处理
      theLogger->pBeginOutput(m_performId,
                          m_acctOutputFileInfo[channelNo].m_outputFileName.c_str(),
                          "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");          

      theLogger->pEndOutput(m_performId,
                          m_acctOutputFileInfo[channelNo].m_outputFileName.c_str(),
                          m_acctOutputFileInfo[channelNo].m_idNum,
                          "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                          "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=0",
                          m_acctOutputFileInfo[channelNo].m_idNum,0);*/
    }
    //如果该类型工作文件中数据记录数等于0，直接删除
    else
    {
        if (unlink(m_acctOutputFileInfo[channelNo].m_workFileName.c_str()) != 0)
        {
            theErrorMessages->insert(ERROR_LINK_UNLINK,
                    m_acctOutputFileInfo[channelNo].m_workFileName + 
                    "--file unlink error");
            return false;                                
        }    
    }        
    
    //初始化输出文件
    m_acctOutputFileInfo[channelNo].m_idNum = 0;
    m_acctOutputFileInfo[channelNo].m_fileStat = 0;    
    //m_acctOutputFileInfo[channelNo].m_batchNo = 0;    
    
    return true;                    
}

bool CEventOutputFile::linkAcctEvent()
{
//	cout << "CEventOutputFile::linkacctEvent" <<endl;
	
    for (int i=0; i<m_acctBackupFiles.size(); i++)
    {
//	cout << "copy " + m_acctBackupFiles[i] + " ->" + 
//                  m_acctOutputFiles[i] <<endl;
    	if (access(m_acctBackupFiles[i].c_str(), F_OK) != 0)
    		continue;
    		
	  //limeng 20091228 rename不支持不同文件系统下操作
      char syscmd[1024]; 	
	  sprintf(syscmd,"mv %s %s",m_acctBackupFiles[i].c_str(),
                 m_acctOutputFiles[i].c_str());
      char cmdMsg[1024]; 	
	  sprintf(cmdMsg,"mv %s %s\n",m_acctBackupFiles[i].c_str(),
                 m_acctOutputFiles[i].c_str());
      theLogger->info(cmdMsg);
	  system(syscmd);
    }
    m_outputFiles.clear();
    m_ugpBackupFiles.clear();
    m_ugpOutputFiles.clear();
    m_acctBackupFiles.clear();
    m_acctOutputFiles.clear();
    return true;   
}
        
bool CEventOutputFile::isUgpEventFinished()
{
//	cout << "CEventOutputFile::isUgpEventFinished" <<endl;
	
    for (int i=0; i<m_ugpOutputFiles.size(); i++)
    {
//    	cout<<"CEventOutputFile:"<<m_ugpOutputFiles[i] <<endl;
        if (access(m_ugpOutputFiles[i].c_str(), F_OK) == 0)
        {
            return false;
        }
    }
        
    return true;
}