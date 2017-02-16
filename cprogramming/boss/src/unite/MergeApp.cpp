// #################################################################
// Language:    C++
// OS Platform: UNIX
// Authors:     BOSS_DR GROUP
// Editors:     Chen Min  <chenm@lianchuang.com>
// Update       20040405
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// Comments: 
// #################################################################

#include "MergeApp.h"
#include "MergeAlertCode.h"
#include <errno.h>
#include "hessian/exceptions.h"

// application object, which will replaced by your application
MergeApp g_application;
Application * const theApp = &g_application;


bool MergeApp::initialization()
{
	setoptstr("c:"); 

	// normally, call standard initialization first
	if (!Application::initialization())
		return false;
	
	#ifndef _DEBUG_
		set_asdaemon(true);
		set_onlyone(true);
		set_runinbkg(true);
	#endif
	
	// ADD YOUR INITIALIZATION CODE HERE
	
	//20060110 文件传顺序的有问题 统一改称按文件名排序
    //setFileSortMethod(Directory::SF_MODIFYTIME);
    setFileSortMethod(Directory::SortFlags (Directory::SF_MODIFYTIME));
    setMaxScan(20000);
	
	try
	{
		getFileTypeConf();  //从配置文件中得到<FILE_TYPE>段的信息
		getChannelConf();   //从配置文件中得到通道号信息，创建各个section的缓冲池
	}
	catch(const string &e)
	{
		m_theErrorMessages->insert(E_MERGE_INITIALIZE,e);
		return false;	
	}

	// 初始化成员变量
	m_pLog       = Log::getInstance();
	m_fileSum    = 0;      //记录一个输入通道下累计处理的文件数
	m_subChanNo  = 0;
	m_inOutPut   = new InOutPut(m_savedPoolPath,m_beforeUnitePath,m_channelNo);
	
	// 处理器初始化,把输入输出类传给各处理器
	for(int i=0;i<m_vectCdrProc.size();++i)
	{
		m_vectCdrProc[i]->setInOutPut(m_inOutPut); //设置处理器的输入输出
	}
	
	// 读取上次进程退出时,写出的缓存信息
	m_inOutPut->readSavedPools(m_chanInfos, m_vectCdrProc ); //vector<ChanInfos*> m_chanInfos, 所处理的通道信息的集合
    
    getNextChanInfo();  //读取下一个通道信息
	        	
	return true;
}

//##ModelId=3BCAA28000B2
bool MergeApp::end()
{
  	if( m_pLog != NULL )
		delete m_pLog;

	int i,j;
	CDR_POOL_MAPS_ITR itrPoolMaps;
	for(i=0;i<m_chanInfos.size();++i)
	{
		// 保存缓存中的信息到文件中
		itrPoolMaps = (m_chanInfos[i]->m_cdrPoolMaps).begin();
		for(j=0;j<(m_chanInfos[i]->m_cdrPoolMaps).size();++j,++itrPoolMaps)
		{
			m_inOutPut->savePools2Files(itrPoolMaps->first,m_chanInfos[i]->m_subChanNo,itrPoolMaps->second,&(m_pCdrProcessor->m_lastCdr), &(m_pCdrProcessor->m_needUnite) );	
		}
		if( m_chanInfos[i] != NULL )
		{
			delete m_chanInfos[i];	
		}	
	}

	for(i=0;i<m_vectCdrProc.size();++i)
	{
		if( m_vectCdrProc[i] != NULL )
		{
			delete m_vectCdrProc[i];	
		}	
	}
	
	delete m_inOutPut;
	
    return Application::end();
}

//从unite.cfg中得到配置<FILE_TYPE>段的信息
bool MergeApp::getFileTypeConf()
{
    char cPath[64];
    sprintf(cPath, "%c%s%c%s", SECTDELIM, m_name, SECTDELIM, "FILE_TYPE");

	  m_theSysParam->setSectionPath(cPath);
  	
  	string section = "";
  	BaseCdrProcessor *  pTmpCdrProc;
    	
  	while (m_theSysParam->getSubSection(section))   //依次读取配置文件中FILE_TYPE段中各个section的信息
  	{
    	char lpath[64];
    	char buffer[1024];
				
	    // 根据类型创建处理器
	    if( section == "GSM" )
	    {
	    	pTmpCdrProc = new GsmCdrProcessor();
	    }
	    else if( section == "GPRS" )
	    {
	    	pTmpCdrProc = new GprsCdrProcessor();
	    }	    
	    else
	    {
	    	throw string("配置文件中 ")+section+ string("是未定义的处理器!");	
	    }
	    
	    
	    //更改相对路径
	    sprintf(lpath, "%s%c%s", cPath, SECTDELIM, section.c_str());
		    
	    //取该类型的文件前缀
	    strcpy(buffer, getParameter(lpath, "FILENAME_PREFIX").c_str());       //FILENAME_PREFIX    = VGP,VPP,YGP
	    for(char *p = strtok(buffer, ","); p != NULL; p = strtok(NULL, ",")) 
	    {
	    	pTmpCdrProc->setPreFix(p); //用配置文件中的信息初始化文件前缀容器
		  }
				
	    //取得用来查找的键值字段在cdr中的出现顺序
	    strcpy(buffer, getParameter(lpath, "SEARCH_KEY").c_str());     //SEARCH_KEY         = 6,4,7,65
	    for(char *p = strtok(buffer, ","); p != NULL; p = strtok(NULL, ","))
	    {
	    	pTmpCdrProc->setSearchFieldPosi( atoi(p) );          //用配置文件中的信息初始化关键search字段的容器
	    }

	    //取得用来创建索引的键值字段在cdr中的出现顺序
	    string Index_Key = getParameter(lpath, "INDEX_KEY");      //INDEX_KEY          = 9,10,5,3,6 
	    //for(char *p = strtok(buffer, ","); p != NULL; p = strtok(NULL, ","))
	    //{
	    	if (!pTmpCdrProc->setIndexFieldPosi( Index_Key ))           //用配置文件中的信息初始化关键index字段的容器
	    	{
	    	    throw string("日期时间字段数量错!");
	    	}
	    //}
	    
	    //取得cdr中的分割符
		  string cdrToken = getParameter(lpath, "CDR_TOKEN");       //CDR_TOKEN          =  ,
		  pTmpCdrProc->setCdrToken( cdrToken[0] );                 //设置分隔符
		  
	      //得到拆分话单标识在话单中的位置
		  string mergeFlagPosi = getParameter(lpath, "MERGE_FLAG_POSI");     //MERGE_FLAG_POSI    = 27
		  pTmpCdrProc->setMergeFlagPosi( mergeFlagPosi );
      
	      //取得缓冲池的最大允许记录数 
		  string maxStoredCdr = getParameter(lpath, "POOL_MAX_SIZE");        //POOL_MAX_SIZE      = 100
		  pTmpCdrProc->setMaxStoredCdr( atoi(maxStoredCdr.c_str()) );
      
	      //取得c缓冲池中的cdr最大延时
		  string maxDelaySecond = getParameter(lpath, "MAX_DELAY_TIME");     //MAX_DELAY_TIME     = 4  
		  pTmpCdrProc->setMaxDelay( maxDelaySecond );
		  
		  //取得用哪条话单作为合并结果，超时话单的处理方式,一次CALL_TYPE或ERROR_CODE的字段顺序
		  string cdrRightNum = getParameter(lpath,"CDR_NUM_KEY");
		  pTmpCdrProc->setMergeCdrFieldNum( cdrRightNum );                                                                                                                                               
	      //取得每条cdr中的总字段数
		  string fieldsCount = getParameter(lpath, "FIELDS_COUNT");          //FIELDS_COUNT       = 38    
		  pTmpCdrProc->setCdrFieldsCount( fieldsCount );		
      
	      //用来获得配置中的需要进行合并的字段,在cdr中的位置
		  string mergeFieldPosi = getParameter(lpath, "MERGE_FIELD_POSI");    //MERGE_FIELD_POSI   = 8,9,12,10,11 
		  if( ! pTmpCdrProc->setMergeFieldPos( mergeFieldPosi ) )      
		  	throw string("需合并字段数量错!");	
	      
		  // 把处理器指针压入处理器容器
	    m_vectCdrProc.push_back(pTmpCdrProc);      //vector<BaseCdrProcessor*>  m_vectCdrProc 处理器容器
  	}
  	
  	if( m_vectCdrProc.size() == 0 )
  	{
  		throw string("处理器没有配置!");	
  	}
  	
  	return true;
}

//从merge.cfg中得到配置<1></1>段的信息
bool MergeApp::getChannelConf()
{
	char cPath[64],c_subChanNo[64];
	int i_subProcessNo=0;

    sprintf(cPath, "%c%s%c%d", SECTDELIM, m_name, SECTDELIM, m_channelNo);

    //防止启动程序时，输入不存在的通道号
    if( !m_theSysParam->setSectionPath(cPath))
    {
    	throw(string("通道不存在!"));	
    }
    
    // 得到保存缓存信息的文件的目录
    m_savedPoolPath = getParameter(cPath,"SAVED_POOLS_PATH");     //SAVED_POOLS_PATH = /gboss/billing/src/merge/data/pool/
    if( m_savedPoolPath[m_savedPoolPath.length()-1] != '/' )
    {
    	m_savedPoolPath += "/";	
    }
    // 得到保存合并话单之前的首话单、中间话单、尾话单的目录   add by lizhao 2008-10-25
    m_beforeUnitePath = getParameter(cPath,"BEFORE_UNITE_PATH");
    if( m_beforeUnitePath[m_beforeUnitePath.length()-1] != '/')
    {
        m_beforeUnitePath += "/";
    }
    
    sprintf(c_subChanNo,"%s%cchan_%d",cPath,SECTDELIM,i_subProcessNo);    //i_subProcessNo = 0
    string strSubChanNo = c_subChanNo;
    
	// 得到 chan_'n'的信息
	ChanInfo * tmpChanInfo;
	while( m_theSysParam->getSubSection( strSubChanNo ) )
    {    
	    tmpChanInfo = new ChanInfo();
	    m_chanInfos.push_back( tmpChanInfo );
	    
	    tmpChanInfo->m_srcPath     = getParameter(c_subChanNo, "srcPath");
	    tmpChanInfo->m_desPath     = getParameter(c_subChanNo, "desPath");

		if( tmpChanInfo->m_srcPath[tmpChanInfo->m_srcPath.length()-1] != '/' )
	    {
	    	tmpChanInfo->m_srcPath += "/";	
	    }
		if( tmpChanInfo->m_desPath[tmpChanInfo->m_desPath.length()-1] != '/' )
	    {
	    	tmpChanInfo->m_desPath += "/";	
	  	}
	  	
	  	// 子通道号
	  	tmpChanInfo->m_subChanNo = i_subProcessNo;
	
	    // 根据不同种类的cdr,在同一个chanInfo中创建不同种类的缓冲池
	    for(int i=0;i<m_vectCdrProc.size();++i)
	    {
	    	CDR_POOL * pCdrPool;                            //class CDR_POOL:public map<string,vector<string>*>{};
	    	pCdrPool = new CDR_POOL();                      //创建缓冲池
	    	string poolType = m_vectCdrProc[i]->getFileType();
	    	
	    	tmpChanInfo->m_cdrPoolMaps.insert( CDR_POOL_MAPS::value_type(poolType,pCdrPool) );		//m_cdrPoolMaps不同业务合并的缓冲池集合
	    }
	    
	    ++i_subProcessNo;     //子通道号自加
	    sprintf(c_subChanNo,"%s%cchan_%d",cPath,SECTDELIM,i_subProcessNo);
	    strSubChanNo = c_subChanNo;
	}   
    return true;	
}

//读取boss.conf中参数的方法
string MergeApp::getParameter(const char* path, const char* name)
{
	string result;
	
	if (!m_theSysParam->getValue(path, name, result) || result.length() == 0)
	{
		throw string( string(path) + SECTDELIM + name);
	}
	
	return result;
}

// TODO: Add your owner member function here
void MergeApp::writeLog()
{
	int prov;
	char Tmp[256];
	
	string logfileNameWithPath = InOutPut::m_LogFileNameWithPath;
	sprintf( Tmp,"merged=%d,pushed2pool=%d,overstored=%d,overtime=%d,out_total_num=%d,%s",
	             m_pLog->getMerged(),      // 已经合并完成而且输出的
	             m_pLog->getPushed2pool(), //被压入缓存的话单数
	             m_pLog->getPool2file(),   //从缓存中提取的话单数
	             m_pLog->getOverTime(),
	             m_inOutPut->getWrite2file(),
	             logfileNameWithPath.c_str() );
	m_logstr = Tmp;
	string srcfileNameWithPath = m_pChanInfo->m_srcPath+m_pLog->getFileName();
	
	theLogger->pInput(theLoggerProcessID,srcfileNameWithPath,m_pLog->getTotal());
	theLogger->pOutput(theLoggerProcessID,srcfileNameWithPath,m_pLog->getTotal(),Tmp);        
}

// 根据文件名排序选项,选择读入文件列表中的文件,并跳过'#'开头的文件
int MergeApp::getFile(char *filename, const int length)
{
	int i,ret;
	
	i=0;
	ret=1;
	while (i<=ret)
	{
	    if ((ret = m_dir.getFile(filename, length)) != 0) 
	    {
	       if(strncmp(filename,"#",1)!=0) 
	       {
	          return ret;
	       }
	    }
	    i++;
	}
	return 0;
}

//##ModelId=3BCAA28000C6

bool MergeApp::loopProcess()
{
 	char fileName[NAMELEN_MAX];
  	int  ret;
    	
	// 预处理本进程所负责的通道
	ret = preProessChannel(fileName) ;
	if( ret == 0)     //致命错误
	{
		return false;	
	}
	else if( ret == 1 )  //所有目录下没有待处理的文件
	{ 
	  for(int i=0;i<m_vectCdrProc.size();i++)
	  {
	  	  m_pCdrProcessor = m_vectCdrProc[i];
	  }
	  
	  m_pCdrProcessor = m_vectCdrProc[0];
		return Application::loopProcess();		
	}
	else                //有待处理的文件
	{		
		//日志处理初始化
        theLoggerProcessID = theLogger->pBegin();
		// 处理一个文件内容
		try
		{
		   	m_status     = PROC_RUN;
		   	m_objectName = fileName;
		   	updateStatus();
			  m_pLog->reset();
			  m_pLog->setFileName(fileName);
			  FP_BEGIN(process)
			  process(fileName);
			  FP_END(process)
		}
		catch(const string &e)
		{
			  m_status     = PROC_WARN;
			  m_objectName = string("deal with file :") + string(fileName) + "failure!";
			  updateStatus();
    		m_theErrorMessages->insert(E_MERGE_INITIALIZE, string(fileName) + e);	
		}
		++m_fileSum;
		// 日志和告警	
		writeAlert();
		writeLog();
		
		theLogger->pEnd(theLoggerProcessID);
		return Application::loopProcess();
	}
}


// 预处理本进程所负责的通道
// return: 0--致命错误
//         1--所有目录都没有待处理的文件
//         2--有待处理的文件,文件名通过fileName带出
int MergeApp::preProessChannel(char *fileName) 
{	
	int i=0;
	
	//当处理同一输入目录下的标准文件超过MAX_FILE_COUNT的时候,强制转换输入通道
	if(m_fileSum >= MAX_FILE_COUNT)
	{
	   	m_fileSum = 0;
	   	if( !getNextChanInfo() )
    	{
    		m_theErrorMessages->insert(E_MERGE_INITIALIZE,"cannot get next channel info!");
			m_stop = true; 
			return 0;
		}
	}

	//当扫描所有输入目录都没有文件时,进程等待"SLEEP_TIME"
	while( getFile(fileName,80) == 0 )
	{
		if( i>=m_chanInfos.size() ) 
		{
			i=0;
		   	m_status = PROC_IDLE;
		   	m_objectName = "";
		   	updateStatus();
		   	sleep(SLEEP_TIME);
		   	return 1;
		}
		i++;
		m_fileSum=0;
		
		if( !getNextChanInfo() )
    	{
    		m_theErrorMessages->insert(E_MERGE_RESET,"cannot get next channel info!");
			m_stop = true;   
			return 0;
		}
		memset(fileName,0,sizeof(fileName));
	}
	return 2;
}

//推测该文件的类型
//return: true--匹配到
//        false--没有匹配到
bool MergeApp::determineFileType(const char* filename)
{
  	for(int i=0; i<m_vectCdrProc.size(); i++)
  	{                  
    	if( (m_vectCdrProc[i])->matchPreFix(filename) )
    	{
    		m_pCdrProcessor = m_vectCdrProc[i];  //取得对应的处理器
    		return true;
    	}
    }

  	return false;
}

//得到下一处理通道信息(如:chan_0)
bool MergeApp::getNextChanInfo()
{
	int len;
	
	//循环DistRule容器,得到通道信息
	++m_subChanNo;
	if( m_subChanNo+1>m_chanInfos.size() ) 
	{
	   m_subChanNo = 0;
	}
	
	//输入路径重置
	m_pChanInfo = m_chanInfos[m_subChanNo];
	m_inOutPut->reset(m_pChanInfo->m_srcPath,m_pChanInfo->m_desPath);
	m_dir.setPath( (m_pChanInfo->m_srcPath).c_str() );
	m_dir.scanFiles(Directory::SF_MODIFYTIME);	
		
	return true; 
}

// 处理一个文件的内容
bool MergeApp::process(const char *fileName)
{
	char cpErrMsg[100];
	
	if( determineFileType(fileName) )
	{
		 string s_filetype = m_pCdrProcessor->getFileType();
		 if(strncmp(s_filetype.c_str(),"GSM",3)==0)
		 {
		    m_pCdrProcessor->process(fileName,m_pChanInfo);       //GSM处理文件	
		 }
		 else if(strncmp(s_filetype.c_str(),"GPRS",4)==0)
		 {
		 	  m_pCdrProcessor->process_gprs(fileName,m_pChanInfo);  //GPRS处理文件
		 }
		 else
		 {
		 	  string desName = m_pChanInfo->m_desPath + fileName;
		    string srcName = m_pChanInfo->m_srcPath + fileName;
	      
		    if( rename(srcName.c_str(),desName.c_str()) != 0 )
		    {
		    	sprintf(cpErrMsg,"ERROR(%s:%d):移动%s:错误代码%d",__FILE__,__LINE__,srcName.c_str(),errno);
		    	throw string(cpErrMsg);
		    }
		 }
	}
	// 没有匹配到文件名前缀的文件,直接移动到输出目录下
	else
	{
		string desName = m_pChanInfo->m_desPath + fileName;
		string srcName = m_pChanInfo->m_srcPath + fileName;
	
		if( rename(srcName.c_str(),desName.c_str()) != 0 )
		{
			sprintf(cpErrMsg,"ERROR(%s:%d):移动%s:错误代码%d",__FILE__,__LINE__,srcName.c_str(),errno);
			throw string(cpErrMsg);
		}
	}
	return true;
}

