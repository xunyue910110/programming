#include "config-all.h"
#include "CdrRelApp.h"
#ifdef FP_DEBUG
#include "base/supdebug.h"
#endif
#include "base/StringUtil.h"
#include "base/TimeUtil.h"
#include <algorithm>

CdrRelApp   g_CdrRelApp;
Application * const theApp = &g_CdrRelApp;

CdrRelApp::CdrRelApp()
{
	
} 
  
CdrRelApp::~CdrRelApp() 
{
} 
  
bool CdrRelApp::initialization() 
{
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
    
    if (!m_config.getSysParam(m_theSysParam,m_name,m_channelNo))
    {
        return false;
    }
    if(m_dbinterface.connect(m_config.m_dbUser.c_str(),m_config.m_dbPassWord.c_str(),m_config.m_dbServerName.c_str()))
    {
        return false;
    }
    m_dbinterface.setFieldLength(LINEMAXLEN);

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

bool CdrRelApp::beforeLoop()
{
	if (!Application::beforeLoop())
	{
		return false;
	}
  return true;
}

bool CdrRelApp::loopProcess()
{
    if(!CdrRelRun())
    {
        m_stop = true;
    } 
    return Application::loopProcess();
}

bool CdrRelApp::end() 
{
  return Application::end();
}


bool CdrRelApp::CdrRelRun()
{
    if (!getCdr())
    {
        return false;
    }
    else if (m_cdr.size() > 0 )
    {
        if (!cdrIndb())
        {
            return false;
        }
    }
    for (int i = 0; i < m_config.m_fileType.size(); ++i)
    {
        if (!execProc(m_config.m_fileType[i]))
        {
            return false;
        }
        
        if (!outputCdr(m_config.m_fileType[i]))
        {
            return false;
        }
    }
    sleep(10);
    return true;
}

bool CdrRelApp::getCdr()
{
    int ret;
    char fileName[PATH_MAX];
    char lineBuff[LINEMAXLEN];
    time_t fileTime;
    ifstream inputFile;
    
    m_dir.setPath(m_config.m_inputPath.c_str()); 
	m_dir.scanFiles(Directory::SF_MODIFYTIME);
	    
    while((ret = m_dir.getFile(fileName, PATH_MAX)) > 0)
    {
      if(fileName[0] == '#')
      {
        continue;
      }
      else break;
    }
    if (ret <= 0)//从目录中读取文件失败
    {
      return true;
    }
    cout << " fileName = " << fileName << endl;
    m_fileName     = fileName;
    m_fullFileName = m_config.m_inputPath + fileName;
    inputFile.open(m_fullFileName.c_str(),ios::in);
    while(!inputFile.eof())
    {
        inputFile.getline(lineBuff,LINEMAXLEN);
        if(lineBuff[0]==0)
        {
            continue;
        }
        else
        {
            m_cdr.push_back(lineBuff);
        }
    }
    inputFile.close();
    inputFile.clear();
    return true;
}

bool CdrRelApp::cdrIndb()
{
    StringVector inVector,errVector;
    int nTotal,nSuccess,nError;
    vector<string> cdrFieldList;
    char sqlCmd[512];
    FileType fileType;
    
    if (!m_config.getFileType(m_fileName,fileType))
    {
        cout << m_fileName << "查找文件类型定义失败！"<< endl;
        return false;
    }
    m_dbinterface.dataBind(inVector,errVector);
    
    for (int i = 0; i < m_cdr.size(); ++i)
    {
        cdrFieldList.clear();
        StringUtil::split(m_cdr[i],",",cdrFieldList);
        
        inVector.push_back(m_fileName);
        inVector.push_back(cdrFieldList[fileType.m_msisdnIndex   ]);
        inVector.push_back(cdrFieldList[fileType.m_beginDateIndex]);
        inVector.push_back(cdrFieldList[fileType.m_beginTimeIndex]);
        fileType.m_key1Index == -1 ? inVector.push_back("0") : inVector.push_back(cdrFieldList[fileType.m_key1Index]);
        fileType.m_key2Index == -1 ? inVector.push_back("0") : inVector.push_back(cdrFieldList[fileType.m_key2Index]);
        fileType.m_key3Index == -1 ? inVector.push_back("0") : inVector.push_back(cdrFieldList[fileType.m_key3Index]);
        fileType.m_key4Index == -1 ? inVector.push_back("0") : inVector.push_back(cdrFieldList[fileType.m_key4Index]);
        fileType.m_key5Index == -1 ? inVector.push_back("0") : inVector.push_back(cdrFieldList[fileType.m_key5Index]); 
        inVector.push_back("0");
        inVector.push_back(m_cdr[i]);
    }
    m_cdr.clear();
    sprintf(sqlCmd,"INSERT INTO %s(FILE_NAME,MSISDN,BEGIN_DATE,BEGIN_TIME,KEY1,KEY2,KEY3,KEY4,KEY5,DEAL_FLAG,CDR) VALUES(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11)",fileType.m_table.c_str());
    m_dbinterface.beginTransaction();  
    if(m_dbinterface.bulkCopy(sqlCmd,nTotal,nSuccess,nError))
    {
        m_dbinterface.rollbackTransaction();
        cout << "话单入库失败!" << endl;
        return false;
    }
    
    //删除输入文件
    if (m_config.m_backupPath.length() > 0)
    {
        string backupFileName = m_config.m_backupPath + m_fileName;
        rename(m_fullFileName.c_str(),backupFileName.c_str());
    }
    else
    {
        unlink(m_fullFileName.c_str());
    }
    m_dbinterface.commitTransaction();
    
    return true;
}

bool CdrRelApp::execProc(const FileType &fileType)
{
    //调用存储过程
    int ret,err;
    PROCPARAM procParam[3];
    procParam[0].action=PARAM_ACT_IN;
    procParam[0].type=PARAM_TYPE_STR;
    strcpy(procParam[0].strValue,"TEST");
    procParam[1].action=PARAM_ACT_OUT;
    procParam[1].type=PARAM_TYPE_INT;
    procParam[1].intValue=m_channelNo;
    procParam[2].action=PARAM_ACT_OUT;
    procParam[2].type=PARAM_TYPE_STR;
    strcpy(procParam[2].strValue,"");

    ret=m_dbinterface.callProcedure(fileType.m_procedure.c_str(),3,procParam);
    
    err=procParam[1].intValue;
    
    if(ret||err)
    {
        cout << "调用存储过程失败!" << endl;
        return false;
    }
    return true;
}

bool CdrRelApp::outputCdr(const FileType &fileType)
{
    char sqlCmd[512],curTime[15];
    StringVector recVector,errVector;
    int nTotal,nSuccess,nError;
    string tmpOutputFileName,outputFileName;
    ofstream outputFile;
    
    sprintf(sqlCmd,"SELECT FILE_NAME,CDR FROM %s WHERE DEAL_FLAG = '1'",fileType.m_table.c_str());
    m_dbinterface.dataBind(recVector,errVector);
    do
    {
    	if(m_dbinterface.executeSql(sqlCmd,nTotal,nSuccess,nError))
    	{
    	    return false;
    	}
    }while(nTotal>=DEFAULTARRAYLINE);
    
    if (recVector.size() > 0)
    {
        TimeUtil::now(curTime);
        tmpOutputFileName = m_config.m_outputPath + "#" + recVector[0] + "_" + curTime;
        outputFileName = m_config.m_outputPath + recVector[0] + "_" + curTime;
        outputFile.open(tmpOutputFileName.c_str(),ios::out);
        for (int i = 0; i < recVector.size(); i+=2)
        {
            outputFile << recVector[i+1] << endl;
        }
        outputFile.close();
        outputFile.clear();  
        
        sprintf(sqlCmd,"DELETE FROM %s WHERE DEAL_FLAG = '1'",fileType.m_table.c_str());
        m_dbinterface.beginTransaction();
        if(m_dbinterface.executeSql(sqlCmd,nTotal,nSuccess,nError))
        {
            return false;
        }
        if (rename(tmpOutputFileName.c_str(),outputFileName.c_str()) == 0)
        {
        	m_dbinterface.commitTransaction();
        	return true;
        }
        else
        {
            m_dbinterface.rollbackTransaction();
            return false;
        }
    }
    
    return true;
}
