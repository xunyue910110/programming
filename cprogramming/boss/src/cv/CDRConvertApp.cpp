// ##########################################
// Source file : CDRConvertApp.cpp
// System      : Mobile Billing System
// Version     : bss1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : jianchun Shi
// E_mail      : shijc@lianchuang.com
// Create      : 20090807
// Update      : 20090807
// Copyright(C): 2009 by ShiJianChun, Linkage.
// ##########################################

#include "CDRConvertApp.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "appfrm/VersionInfo.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "CConvertConfig.h"


//#include "udbi/udbi.h"
//#include "udbi/JConnect.h"
//#using namespace UDBI;
//#include "udbi/JTable.h"
//#include "udbi/JSQL.h"
//#include "udbi/jdb.h"

#include "systemPara.h"
#include "CvLog.h"
CDRConvertApp g_application;

Application * const theApp = &g_application;

//##ModelId=48571D190271
CDRConvertApp::~CDRConvertApp()
{
   
}

//##ModelId=48571D5900FA
bool CDRConvertApp::processopt(int optopt, const char *optarg)
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
bool CDRConvertApp::initialization()
{
    //-----------------------------------------------------------------------//
    // call setoptstr before initialization()
    // set optstring -- reference unix getopt() call
    // the 'c' option is reserved for channel no option,
    // you can add your owner option
    //-----------------------------------------------------------------------//
    setoptstr("c:");		
		
    
    // normally, call standard initialization first
    if (!Application::initialization())
    {
    	theErrorMessages->insert(CV_ERROR_INITIALIZE, "app initialize failure!");
    	#ifdef _DEBUG_
		     cout<<"app initialize failure! "<<" "<<__FILE__<<__LINE__<<endl;		     
		  #endif
      return false;      
    }    
    //创建格式转换主基类对象实例
    if(NULL == m_pCdrBase)
    	m_pCdrBase = new CCdrConverstion;
    if(m_pCdrBase == NULL) 
    {
    	#ifdef _DEBUG_
		     cout<<"malloc CCdrConverstion buffer or create error! "<<" "<<__FILE__<<__LINE__<<endl;		     
		  #endif
    		theErrorMessages->insert(CV_ERROR_CREATE_OBJECT, "malloc CCdrConverstion buffer or create error!");
    }
    
    CConvertConfig *pConfig =  CConvertConfig::Create();    	
    if(pConfig == NULL) 
    {
    	theErrorMessages->insert(CV_ERROR_CREATE_OBJECT, "malloc CConvertConfig buffer or create error!");
    	#ifdef _DEBUG_
		     cout<<"malloc CConvertConfig buffer or create error! "<<" "<<__FILE__<<__LINE__<<endl;		     
		  #endif
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

   // setFileSortMethod(Directory::SortFlags (Directory::SF_MODIFYTIME));
    setFileSortMethod(Directory::SortFlags (Directory::SF_NAME));
    setMaxScan(20000);

    m_fileSum = 0;
    m_channelSequnce = 0;   
    m_roamPath = "";
    m_longPath = "";
    m_isSplitLongCdr = false;

		
    if(!GetConfigInfo())
    {
    	#ifdef _DEBUG_
		     cout<<"get config info error! "<<" "<<__FILE__<<__LINE__<<endl;		     
		  #endif
    	theErrorMessages->insert(CV_ERROR_GET_CONFIG, "get config info error!");
        return false;
    }
   
    if(!m_pCdrBase->Init())
    {
    	 cout<<"CCdrBase Init() failure! "<<" "<<__FILE__<<__LINE__<<endl;	
    	 theErrorMessages->insert(CV_ERROR_INITIALIZE, "initialize failure!");	
    	 return false;		 
    }
   // test();
    return true;
}


bool CDRConvertApp::GetConfigInfo()
{
//	printf("start to GetConfigInfo  \n");
	assert(m_theSysParam != NULL);
	
	//获得配置信息
	if(!CConvertConfig::GetInstance()->GetSystemConfigInfo(m_theSysParam,m_name,m_channelNo))
	{
		 #ifdef _DEBUG_
        cout << "GetSystemConfigInfo faliure!" << endl;
     #endif
     return false;
	} 
	
	//将通道下第一个子通道设置为当前获得目录	
	m_channelSequnce = 0;
	cout<<" first scan dir = " <<CConvertConfig::GetInstance()->GetSourcePath(0).c_str()<<endl;
	printf((CConvertConfig::GetInstance()->GetSourcePath(0)).c_str());
	m_sourcePath = CConvertConfig::GetInstance()->GetSourcePath(m_channelSequnce);
	setPathIn(m_sourcePath.c_str());
	return true;
}
//##ModelId=48571D860232
bool CDRConvertApp::recovery()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//

    // TO BE DETERMINED
    Application::recovery();
    return true;
}

//改变扫描的通道
bool CDRConvertApp::getNextChanInfo()
{
	// 如果配置了一个输入目录,则直接返回
    if( CConvertConfig::GetInstance()->GetChannelCount() == 1)
    {
        return true;
    }
    else // 轮询下一个输入通道,并提取相应的文件处理器,同时重置输入目录
    {
    		++m_channelSequnce;
        if(m_channelSequnce == (CConvertConfig::GetInstance()->GetChannelCount()))
        {
            m_channelSequnce = 0;
        }    
        
        m_sourcePath = CConvertConfig::GetInstance()->GetSourcePath(m_channelSequnce);
       // m_desPath  = m_channelInfos[m_channelSequnce]->m_pOutPutPathStr;
       // m_PpLog_->setOutPutFile(&m_PreProcApplication->m_OutputFile);       
        setPathIn(m_sourcePath.c_str());        

        return true;
    }
	
	return true;
}



//获取下一个话单文件名,文件的产生日期，以及输出文件名
bool CDRConvertApp::GetCdrFileName(string &sFileName,string &sSrcFileDate,string &sFileNameOut)
{
	 	char fileNameTmp[MAX_SIZE]={"\0"};    
    memset(fileNameTmp, 0, sizeof(fileNameTmp));
    //当处理同一输入目录下的标准文件超过40个的时候,强制转换输入通道
    if(m_fileSum >= 40)
    {
        m_fileSum = 0;
        if(!getNextChanInfo())
        {
            #ifdef _DEBUG_
		     		  cout<<"getNextChanInfo error! "<<" "<<__FILE__<<__LINE__<<endl;		     
		 		 	  #endif
		 		 	  
            m_theErrorMessages->insert(CV_ERROR_INITIALIZE,"cannot get next channel info!");
            m_stop = true;
            return false;
        }
       return false ; //转换通道后暂时不再取文件，等待下次循环开始取
    }
    //当扫描所有输入目录都没有文件时,进程等待"SLEEP_TIME"
    int i = 1;
    
   // printf("start getfile\n");    
    while(!getFile(fileNameTmp, 512))
    {
    	  printf(fileNameTmp);
        if(i > CConvertConfig::GetInstance()->GetChannelCount())
        {
        	 
            i = 1;
            m_status = PROC_IDLE;
            m_objectName = "";
            updateStatus();
            sleep(SLEEP_TIME);   
            break;         
        }
        i++;
        m_fileSum = 0;
			
        if(!getNextChanInfo())
        {
        	  #ifdef _DEBUG_
		     		  cout<<"getNextChanInfo error! "<<" "<<__FILE__<<__LINE__<<endl;		     
		 		 	  #endif
            m_theErrorMessages->insert(CV_ERROR_INITIALIZE, "cannot get next channel info!");
            m_stop = true;
            return false;
        }
        printf(fileNameTmp);
				sFileName = fileNameTmp;
        memset(fileNameTmp, 0, sizeof(fileNameTmp));
        
        return false ; //转换通道后暂时不再取文件，等待下次循环开始取  added by shijianchun in 20090927
    }
    // ------ 开始处理文件 --------
        
    if(strlen(fileNameTmp) < 1 )
    	 return false;  
    sFileName = fileNameTmp;  
    
    //取得文件的日期。文件名形如：1025_NECS03_20090601112100_MET_L.DAT-ytd_nec_sigma.200906011121.{met0601.dat}.NEC_S
    //日期为20090601
    char sFileBuf[255]={"\0"},sDate[20]={"\0"};
    strcpy(sFileBuf,sFileName.c_str());
    char *pTmp = strchr(sFileBuf,'_');
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"getNextChanInfo error! "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif
       m_theErrorMessages->insert(CV_ERROR_FILE_NAME, "filename invalide ,cannot get date");
       m_stop = true; 
       return false;
    }
    pTmp++;
    pTmp = strchr(pTmp,'_');
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"getNextChanInfo error! "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif
       m_theErrorMessages->insert(CV_ERROR_FILE_NAME, "filename invalide ,cannot get date");
       m_stop = true;
       return false;
    }
    pTmp++ ;
    strncpy(sDate,pTmp,8);
    sSrcFileDate = sDate;
    
    //取输出文件全路径名
    assert(CConvertConfig::GetInstance() != NULL);
  //  sFileNameOut = CConvertConfig::GetInstance()->GetDestPath(m_channelSequnce) + '/' +sFileName;
    sFileNameOut = sFileName  + ".out";
    //sFileName = CConvertConfig::GetInstance()->GetSourcePath(m_channelSequnce) + '/' + sFileName;
    
    return true;
	
}
//##ModelId=48571D95034B
bool CDRConvertApp::loopProcess()
{
    //-----------------------------------------------------------------------//
    // TODO: Add your specialized code here and/or call the base class
    //-----------------------------------------------------------------------//
		if (!Application::beforeLoop())
		{
			return false;
		}
		cout<<"0000000000000000000000000"<<endl;		
		//获取话单文件
		vector<string> vecFiles;
		vector<string>::iterator itrFile;
		string sCdrFileName,sCdrFileNameOut,sCdrFileDate;
		char sFileBuf[100]={"\0"};
		//处理净跳次通道下的话单
		if(1 == CConvertConfig::GetInstance()->GetCdrType(m_channelSequnce) )  
		{
		  if( !GetCdrFileName( sCdrFileName,sCdrFileDate, sCdrFileNameOut))
			{				
				 return false;
			}
			ProcessFile(sCdrFileName,sCdrFileNameOut,sCdrFileDate);
	  }  
	  //处理累计和反转跳次通道下的话单
		else         
		{
			cout<<"     get all files      "<<endl;
			
			vecFiles.clear();
			int nTmpCount = vecFiles.size();
			setPathIn(m_sourcePath.c_str());
		  GetAllFiles(vecFiles);
		  nTmpCount = vecFiles.size();
		  char *pTmp = NULL;
		  char sTmpDate[20]={"\0"};
		  for(itrFile = vecFiles.begin(); itrFile!= vecFiles.end() && nTmpCount>0 ;itrFile++)
		  {
		  	 sCdrFileName = (*itrFile).c_str(); 
		  	 memset(sFileBuf,0x0,sizeof(sFileBuf));
		  	 strcpy(sFileBuf,sCdrFileName.c_str());
         pTmp = strchr(sFileBuf,'_');    
         //取文件日期
         if(pTmp == NULL)
         {
    	      #ifdef _DEBUG_
   		        cout<<"leiji or fanzhuan filename error "<<" "<< sFileBuf<< "  "<<__FILE__<<__LINE__<<endl;		     
	 	        #endif
	 	        m_theErrorMessages->insert(CV_ERROR_FILE_NAME, "filename invalide ,cannot get date");
            exit(0);
         }
         pTmp++;
         pTmp = strchr(pTmp,'_');
         if(pTmp == NULL)
         {
    	      #ifdef _DEBUG_
   		        cout<<"leiji or fanzhuan filename error "<<" "<< sFileBuf<< "  "<<__FILE__<<__LINE__<<endl;     
	 	        #endif  
	 	        m_theErrorMessages->insert(CV_ERROR_FILE_NAME, "filename invalide ,cannot get date");     
            exit(0);
         }
         pTmp++ ;
         strncpy(sTmpDate,pTmp,8); 
         sCdrFileDate = sTmpDate;
         sCdrFileNameOut = sCdrFileName + ".out";
         ProcessFile(sCdrFileName,sCdrFileNameOut,sCdrFileDate);
		   }	
		  vecFiles.clear();	
		  nTmpCount = vecFiles.size();
		  
		  getNextChanInfo();
		}
		
		
    // ------ 处理文件结束 --------
   // exit(0);
    ++m_fileSum;
    return true;
}

//##ModelId=48571DA40203
bool CDRConvertApp::end()
{
    return Application::end();
}



//##ModelId=48571E18033C
void CDRConvertApp::writeLog()
{
	char Tmp[256];		             
	//string sourceFileNameWithPath   = m_sourcePath->c_str() + m_PpLog_->getSrcFileName();
	//string desFileNameWithPath      = m_desPath->c_str() + m_PpLog_->getSrcFileName();
	//theLogger->pInput(theLoggerProcessID,sourceFileNameWithPath,m_PpLog_->getFileLenth());
	//theLogger->pOutput(theLoggerProcessID,desFileNameWithPath,m_PpLog_->getTotalCdrs(),Tmp);
}

//备份文件
bool CDRConvertApp::BackupFile(string sSrcFileName,string sDestFileName)
{
	
	if(access(sDestFileName.c_str(), F_OK) == 0)
  {
    //备份文件存在先删除
    ::unlink(sDestFileName.c_str());
  }
  if(link(sSrcFileName.c_str(), sDestFileName.c_str()) != 0)
  {
    	theErrorMessages->insert(E_MOVE_FILE_ERR,"backup '" + sSrcFileName +  + "' to "+ sDestFileName + " false!");
			#ifdef __DEBUG__
					cout<<"原始文件 " << m_srcFileName << " cannot be saved!";
			#endif
			return false;
  }	
	return true;
}

//比较两个文件名大大小，按日期比较
//左边小于右边返回true ,否则false
bool  CDRConvertApp::VecSort::operator ()(const string &left,const string &right)
{
	  //取得文件的日期。文件名形如：1025_NECS03_20090601112100_MET_L.DAT-ytd_nec_sigma.200906011121.{met0601.dat}.NEC_S
    //日期为20090601
    
    if(left =="") return true; //若left为空，则认为小于
    if(right == "") return false; //若right为空，则认为大于
    char sFileBuf[255]={"\0"},sDate[20]={"\0"};
    long lLeftDate,lRightDate,lLeftTime,lRightTime;
    strcpy(sFileBuf,left.c_str());
    char *pTmp = strchr(sFileBuf,'_');
    
    //取左边参数的文件日期
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"filename error "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif
       exit(0);
    }
    pTmp++;
    pTmp = strchr(pTmp,'_');
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"filename error ! "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif       
       exit(0);
    }
    pTmp++ ;
    strncpy(sDate,pTmp,8);     
    lLeftDate = atol(sDate);
    strncpy(sDate,pTmp+8,6);
    lLeftTime    =atol(sDate);
    
    //取右边参数的文件日期
    memset(sDate,0x0,sizeof(sDate));
    strcpy(sFileBuf,right.c_str());
    pTmp = strchr(sFileBuf,'_');
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"filename error! "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif
       exit(0);
    }
    pTmp++;
    pTmp = strchr(pTmp,'_');
    if(pTmp == NULL)
    {
    	 #ifdef _DEBUG_
   		  cout<<"file name error "<<" "<<__FILE__<<__LINE__<<endl;		     
	 	   #endif       
       exit(0);
    }
    pTmp++ ;
    strncpy(sDate,pTmp,8);    
    lRightDate = atol(sDate);
    strncpy(sDate,pTmp+8,6);
    lRightTime    =atol(sDate);
    
    if(lLeftDate < lRightDate)
     	return true;
    else if(lLeftDate == lRightDate)
    {
    	if( lLeftTime<lRightTime)
    	   return true;    	
    	 else
    	   return false;
    }
    else if (lLeftDate > lRightDate)
    		return false;
	return true;
}


//获取子通道目录下所有的文件存放到容器vecFiles中
bool CDRConvertApp::GetAllFiles(vector<string> & vecFiles)
{
	char sFileName[512]={"\0"};
	string sTmpFile;
	string sFirstFileName;
	if(getFile(sFileName, 512))
	{
		 sFirstFileName = sFileName;
		 vecFiles.push_back(sFirstFileName);
		 cout<< sFileName<<endl;	
	}
	else 
	   return false;
	   
	//遍历目录下所有的文件
	while(getFile(sFileName, 512))
	{
		sTmpFile = sFileName;
		if( sFirstFileName == sTmpFile)
			break;
			
		vecFiles.push_back(sTmpFile);				
		cout<< sFileName<<endl;
	}
	
	//对文件按日期进行显示
	vector<string>::iterator itrFile;
	for (itrFile = vecFiles.begin();itrFile != vecFiles.end();itrFile++)
	{
		cout<< (*itrFile).c_str()<<endl;
		
	}
	//按文件名中显示的采集日期排序，日期由远到近
	sort(vecFiles.begin(),vecFiles.end(),VecSort());
	

	return true;
}

//处理话单文件中的话单
bool CDRConvertApp::ProcessFile(const string &sSrcFileName,const string &sDestFileName,const string &sSrcFileDate)
{
	   string sCdrFileName,sCdrTmpFileNameOut,sCdrFileNameOut;			  
		
		//格式转换基类 轮询执行处理操作
		sCdrFileName = CConvertConfig::GetInstance()->GetSourcePath(m_channelSequnce) + sSrcFileName;		
		sCdrFileNameOut = CConvertConfig::GetInstance()->GetDestPath(m_channelSequnce) + sDestFileName;
		sCdrTmpFileNameOut = CConvertConfig::GetInstance()->GetDestPath(m_channelSequnce) + '#'+ sDestFileName;
		printf("source filename = %s,\n filenameOut = %s\n",sCdrFileName.c_str(),sCdrFileNameOut.c_str());
		cout<<	"source filename = "<<	sCdrFileName.c_str()<<endl;
		cout<<"filenameOut = "<<sCdrFileNameOut.c_str()<<endl;
    if(m_pCdrBase != NULL)
    {
    	m_pCdrBase->setCdrFileDate(sSrcFileDate);
			m_pCdrBase->process(sCdrFileName ,sCdrTmpFileNameOut,m_channelSequnce); 
			//处理完毕，输出文件名去掉开头的*
			rename(sCdrTmpFileNameOut.c_str(),sCdrFileNameOut.c_str());
		}
		
	
		//处理完毕，如需备份源文件，则备份
		if(CConvertConfig::GetInstance()->IsBackupSrcFile(m_channelSequnce))
		{
			cout<<"beifen source file"<<endl;
			string sTmpBackupFile = CConvertConfig::GetInstance()->GetSrcBakupPath(m_channelSequnce)+sSrcFileName;
			BackupFile(sCdrFileName,sTmpBackupFile);	
			if(unlink(sCdrFileName.c_str())<0)
		  {
		    #ifdef __DEBUG__
		      cout<<"delete "<<sCdrFileName<<" false!"
		          <<__FILE__<<__LINE__<<endl;
		    #endif
		    theErrorMessages->insert(CV_ERROR_UNLINK_FILE,"delete '" + sCdrFileName +  " false!");
		    
		  }		
    }
    //输出文件若是大小为0,则删除,
    struct stat fileStat;
    if(stat((char*)sCdrFileNameOut.c_str(),&fileStat)==0)
    {  	
      if( fileStat.st_size == 0)
      {
        if(unlink(sCdrFileNameOut.c_str())<0)
		    {
		     #ifdef __DEBUG__
		       cout<<"delete "<<sCdrFileName<<" false!"
		           <<__FILE__<<__LINE__<<endl;
		     #endif
		     theErrorMessages->insert(CV_ERROR_UNLINK_FILE,"delete output empty file'" + sCdrFileName +  " false!");		    
		    }	
		  }
		  
    }
    if(CConvertConfig::GetInstance()->IsBackupDstFile(m_channelSequnce) && fileStat.st_size > 0)
		{
			string sTmpBackupFile = CConvertConfig::GetInstance()->GetDstBackupPath(m_channelSequnce)+sDestFileName;
			cout << " dst file sTmpBackupFile = " <<sTmpBackupFile.c_str() <<endl;
			BackupFile(sCdrFileNameOut,sTmpBackupFile);			
    }
    
    unlink(sCdrFileName.c_str());
    return true;
	
}

void CDRConvertApp::test()
{
	/*
	JConnect *pcon;
	char sPhoneNo[20]={"\0"};
	int lRecDate = 20090729;
	char cType = '0';
	strcpy(sPhoneNo,"25980005");	
	try 
	{
	jdb::jinit("gwbiltst_tj", "ucr_tj01", "test123");
	pcon = jdb::getConnect();
	JTable *pTab = pcon->CreateJTable();
	pTab->AddSql("select MONTH_SCALE from tg_lsmeter_cdr_count where call_number ==:call_number  and rec_date =:recdate and meter_type =:type");
	pTab->SetParam(":call_number", sPhoneNo);
  pTab->SetParam(":recdate",lRecDate);
  pTab->SetParam(":type",cType);
  pTab->clear();
  pTab->ExecSelect();
  	while(pTab->Fetch()) {
  int nMontScale = pTab->GetInt("MONTH_SCALE");
}
	}
	 catch (Exception &er)
	{
		cout << er << endl;
	}
	delete pcon;	
	cout << "wait ... " << endl;
	sleep(3);
	*/
//	OCCIConnect * m_cdrCon = NULL;//到清单库的oracle连接对象指针
	//m_cdrCon = new OCCIConnect("ucr_tj01","test123","gwbiltst_tj" ,true);
	OCCIConnect  m_cdrCon ;
	bool bRet ;
	bRet = m_cdrCon.connect("ucr_tj01","test123","gwbiltst_tj" ,true);
	char sql[2000]={"\0"};
	strcpy(sql,"select MONTH_SCALE from tg_lsmeter_cdr_count where call_number =:call_number  and rec_date >=:recdate1 and rec_date <=:recdate2 and meter_type =:type");
	char sPhoneNo[20]={"\0"};
	int lRecDate1 = 20090700,lRecDate2=20090732;
	char cType[2]={"\0"};
	cType[0] = '0';
	strcpy(sPhoneNo,"25980005");
	
	OCCISQL occisql(&m_cdrCon,sql);
	char item[512];
	char items[100][512];
	ub2  len[100];
	unsigned int iCount=0;
	int iTotal=0;
	try
	{
		occisql.setString(1,sPhoneNo);
		occisql.setInt(2,lRecDate1);
		occisql.setInt(3,lRecDate2);
		occisql.setString(4,cType);
		occisql.executeQuery();
		occisql.setResultDataBuffer(1,items,OCCI_SQLT_STR,sizeof(items[0]),len);
		while (occisql.next(100))
		{
			iCount = occisql.getNumArrayRows();
			iTotal+=iCount;
			for (int i=0;i<iCount;i++)
			{
				int m =9;
			}
		}
		return ;
	}
	catch (SQLException & e)
	{
		cout<<"查询清单表失败!"<<endl;
		cout<<e.getMessage()<<endl;
		return ;
	}		
	
}