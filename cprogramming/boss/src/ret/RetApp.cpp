// ##########################################
// Source file : RetApp.h
// System      : Mobile Billing System
// Version     : 0.1.19
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020415
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <vector>
#include "RetApp.h"

Baf g_theBaf;

//OutputFile RetApp::m_OutputFile;

//##ModelId=3BD629650146
RetApp::RetApp(string theDataSource,int isShow)
{
	string configName,configPath;
	m_theRetParameter_   = 0;
	m_theRawFileProcess = 0;
	m_theCdrProcess     = 0;

	configPath="../config/";
	configName="ppdatesource.cfg";

	m_RetLog_     = RetLog::getInstance();
	m_cdrFlag    = CdrFlag::getInstance();
	m_dataSource = atoi( theDataSource.c_str() );

	if ( m_RetLog_ != 0  ) 
	{
	   if ( isShow == 1 )
		   m_RetLog_->setShowFlag(true);	//显示调试信息
	   else
		   m_RetLog_->setShowFlag(false);	//不显示调试信息
	}

	m_theRetParameter_ = new RetParameterFromFile(configPath,configName,theDataSource);
}

//##ModelId=3B6E476701B7
RetApp::~RetApp()
{
	free(m_srcCdrBuffer);
	if( m_theRawFileProcess != 0 )
		delete m_theRawFileProcess;
	if( m_theCdrProcess != 0 )
		delete m_theCdrProcess;
	if( m_RetLog_ != 0 )
		delete m_RetLog_;
	if( m_theRetParameter_ != 0 )
		delete m_theRetParameter_;
}

//##ModelId=3B4EB03E03D3
int RetApp::run(string aSrcFileName)
{
  int xxx;
  string filename;
  bool error=false;
  FieldInfo *pFieldInfo;
	int srcCdrLength,size,len;

	if ( !reset(aSrcFileName) ) 
	{
        #ifdef DEBUG
            cout<<"RetApp reset error!"<<endl;
        #endif
		return -1;
	}

	size = aSrcFileName.size();
	if( (len=aSrcFileName.find_last_of("/") ) < 0 ) 
	{
		filename	= aSrcFileName;
	} 
	else 
	{
		filename	= aSrcFileName.substr(len+1,size-len-1);
	}

  FP_BEGIN(run)
  	multimap<MyString,FieldInfo *>::iterator itr;
  	while ( (srcCdrLength = m_theRawFileProcess->getCdr(m_srcCdrBuffer)) > 0 )
  	{
      //showSrcCdrBuffer(m_srcCdrBuffer,srcCdrLength);
  		for(itr=m_FieldInfoMaps.begin();itr!=m_FieldInfoMaps.end();++itr)
  		{
  			if( itr->second != 0 )
  			{
  				delete itr->second;	
  			}	
  		}
  		m_FieldInfoMaps.clear();
  		
  		CdrFlag::getInstance()->clear();
  
  	  pFieldInfo = new FieldInfo(0);
      //pFieldInfo->m_IsFormat     = true;
  	  pFieldInfo->m_FieldName_     = "FILENAME";
  	  pFieldInfo->m_FieldDesValue  = filename;
      m_FieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );
  
      FP_BEGIN(getDesCdr)
          // 得到原始字段值
          if(m_theCdrProcess->getDesCdr(m_srcCdrBuffer,srcCdrLength,m_FieldInfoMaps)<0) 
          {
             continue;
          }
      FP_END(getDesCdr)
  
      FP_BEGIN(formatField)
          m_theCdrProcess->formatField(m_FieldInfoMaps);          // 字段值数据格式标准化
      FP_END(formatField)
  
      FP_BEGIN(convertCdr)
          m_theCdrProcess->convertCdr(m_FieldInfoMaps);           // 对字段值进行转换
      FP_END(convertCdr)
  
      FP_BEGIN(validCdr)
          m_theCdrProcess->validCdr(m_FieldInfoMaps);             // 对字段进行校验
      FP_END(validCdr)
  
      FP_BEGIN(write)
          if(m_OutputFile.write(m_FieldInfoMaps)<0)               // 写话单到标准话单
          {             
              error=true;
              break;
          }
      FP_END(write)
      writeRetLog();                                              // 写日志
  		showSrcFields(m_FieldInfoMaps);
  	}
  FP_END(run)

  if(error) 
  {
        #ifdef DEBUG
            cout<<"文件处理错误！"<<endl;
        #endif
	    m_OutputFile.closeSrcFiles(1);
	} 
	else 
	{
        #ifdef _DEBUG_
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tMOC   record sum is \t"<<m_RetLog_->getMocSum()<<endl;
            cout<<"\tMTC   record sum is \t"<<m_RetLog_->getMtcSum()<<endl;
            cout<<"\tPOC   record sum is \t"<<m_RetLog_->getPocSum()<<endl;
            cout<<"\tSMMO  record sum is \t"<<m_RetLog_->getSmocSum()<<endl;
            cout<<"\tSMTC  record sum is \t"<<m_RetLog_->getSmtcSum()<<endl;
            cout<<"\tFORW  record sum is \t"<<m_RetLog_->getForwSum()<<endl;
            cout<<"\tOther record sum is \t"<<m_RetLog_->getOtherSum()<<endl;
            cout<<"\tError record sum is \t"<<m_RetLog_->getErrorSum()<<endl;
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tTotal     CDR    is \t"<<m_RetLog_->getTotalCdrs()<<endl;
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tstart number     is \t"<<m_RetLog_->getBeginCdrNo()<<endl;
            cout<<"\tend   number     is \t"<<m_RetLog_->getEndCdrNo()<<endl;
            cout<<"-----------------------------------------------"<<endl;
        #endif
	    m_OutputFile.closeSrcFiles(0);
	    //m_RetLog_->clear();
	}

	for(itr=m_FieldInfoMaps.begin();itr!=m_FieldInfoMaps.end();++itr)
	{
		if( itr->second != 0 )
		{
			delete itr->second;	
		}	
	}
	m_FieldInfoMaps.clear();


	return 0;
}

//##ModelId=3BD42636015F
bool RetApp::initialize(string theSwitchName,vector <string> &pathMessage,
                                    float errorRate,const string isBackupSrc,const string isBackupDes,string isCheckCdrNo)
{
  vector<string> theConvertRule,theValidRule;
	string aRawFileType,aCdrType,out_file_type,token;
	string ret_file_type;
	string file_header_size,file_tail_size,cdr_size,real_block_size;    // added by gaoxl 20080305 定长话单块大小判断
  string errbegin_pos,errcode_len;
  
	vector<string> theStandardFile,theRawFileParameter,theCdrParameter,theBafParameter;

	if ( (m_srcCdrBuffer = (unsigned char *)malloc(20480)) == 0 )	// 申请记录缓存
    {
        #ifdef DEBUG
            cout<<"malloc src cdr buffer error!"<<endl;
        #endif
		theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc src cdr buffer error!");
		return false;
    }
	setbuf(stdout,NULL);				   //设置标准输出为不带缓冲的输出

	if ( m_theRetParameter_ == 0 ) 
	{
        #ifdef DEBUG
            cout<<"cannot create theRetParameter object!"<<endl;
        #endif
		theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create theRetParameter object!");
		return false;
	}
    
  if(!theSwitchName.empty()) 
  {
	   if ( !m_theRetParameter_->initialize(theSwitchName,pathMessage) ) 
	   {
            #ifdef DEBUG
                cout<<"cannot initialize RetParameter!"<<endl;
            #endif
	        theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize RetParameter!");
	   	    return false;
	    }
	}

  m_theRetParameter_->getValue("token", "token",token);
	m_theRetParameter_->getValue("cdr_type", "cdr_type",aCdrType);
	m_theRetParameter_->getValue("output_file_type","output_file_type", out_file_type);
	m_theRetParameter_->getValue("rawFile_type", "rawFile_type",aRawFileType);

	// add by gaoxl 20080305 定长文件块大小判断
	m_theRetParameter_->getValue("file_header", "file_header_size",file_header_size);
	m_theRetParameter_->getValue("file_header", "file_tail_size",file_tail_size);
	m_theRetParameter_->getValue("cdr_size", "cdr_size",cdr_size);
	m_file_head_size = atoi(file_header_size.c_str());
	m_file_tail_size = atoi(file_tail_size.c_str());

	m_theRetParameter_->getValue("filerr_info", "errbegin_pos",errbegin_pos);
	m_theRetParameter_->getValue("filerr_info", "errcode_len",errcode_len);
	m_errBegin=atoi(errbegin_pos.c_str());
	m_errCodeLen=atoi(errcode_len.c_str());

	//get org-send filename info for get ret filename
	m_theRetParameter_->getValue("ret_file_name", "file_type",ret_file_type);
	m_retfile_type = atoi(ret_file_type.c_str());
	m_theRetParameter_->getValue("ret_file_name", "switch_part",m_switch_part);
	m_part_length = m_switch_part.length();
	

	if (aCdrType == "1")
	{
	    if (!isdigit(cdr_size[0]))
	    {
	        m_cdr_size = 0;
	    }
	    else
	    {
	        m_cdr_size = atoi(cdr_size.c_str());
	    }
    }
	// over 20080305

	//m_theRetParameter_->getValue("backup_srcfile", isBackupSrc);
	m_cdrType = atoi(aCdrType.c_str());

	//if ( createFileObject(atoi( aRawFileType.c_str() )) == 0 )
	if ( createFileObject() == 0 )
	{
        #ifdef DEBUG
            cout<<"cannot create file object!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create file object!");
		return false;
	} 
	else if ( createCdrProcessObject( atoi(aCdrType.c_str()) ) == 0 ) 
	{
        #ifdef DEBUG
            cout<<"cannot create CDR Process object!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create CDR Process object!");
		return false;
	}
	
	//得到section为"record"的配置文件内容
	if ( !m_theRetParameter_->getSectionList("record",theCdrParameter) ) 
	{
        #ifdef DEBUG
            cout<<"cannot get record  RetParameter!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get record  RetParameter!");
		return false;
	}

	//得到section为"BAF"的配置文件内容
	if ( !m_theRetParameter_->getSectionList("BAF",theBafParameter) )  //add by chenmin at 20020415
    {
        #ifdef DEBUG
            cout<<"cannot get BAF RetParameter!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get BAF RetParameter!");
		return false;
	}
	//得到section为"convert"的配置文件内容
	if ( !m_theRetParameter_->getSectionList("convert",theConvertRule) ) 
	{
        #ifdef DEBUG
            cout<<"cannot get convert  RetParameter!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get convert  RetParameter!");
		return false;
	}
	//得到section为"valid"的配置文件内容
	if ( !m_theRetParameter_->getSectionList("valid",theValidRule) ) 
	{
        #ifdef DEBUG
            cout<<"cannot get valid  RetParameter!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get valid  RetParameter!");
		return false;
	}
	//得到section为"output_file"的配置文件内容
	if ( !m_theRetParameter_->getSectionList("output_file",theStandardFile) ) 
	{
        #ifdef DEBUG
            cout<<"cannot coutput file  RetParameter!"<<endl;
        #endif
	   theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot coutput file  RetParameter!");
		return false;
	}

    m_FieldValidators.setValidType("system");
    if (!m_FieldValidators.initialize(theValidRule)) 
    {              // 文件校验规则初始化
        #ifdef DEBUG
            cout<<"cannot initialize file Valid rule!"<<endl;
        #endif
        theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize file Valid rule!");
        return false;
	} else if ( !m_theCdrProcess->setToken(token) ) {
        #ifdef DEBUG
            cout<<"cannot initialize CdrProcess setToken!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess setToken!");
		return false;
    } else if ( m_theRawFileProcess->initialize(m_theRetParameter_,atoi( aCdrType.c_str() ) ) == 0 ) {
        #ifdef DEBUG
            cout<<"cannot initialize RawFileProcessor!"<<endl;
        #endif
		theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize RawFileProcessor!");
		return false;
	} else if ( !m_theCdrProcess->initialize(theCdrParameter) ) {
        #ifdef DEBUG
            cout<<"cannot initialize CdrProcess!"<<endl;
        #endif
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess!");
		return false;
	} else if ( !m_theCdrProcess->setRule(theConvertRule,theValidRule)) {
        #ifdef DEBUG
            cout<<"cannot initialize CdrProcess setRule!"<<endl;
        #endif
	   theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess setRule!");
		return false;
	} else if ( !m_OutputFile.initialize(theStandardFile,pathMessage,isBackupSrc,isBackupDes,
	                                     isCheckCdrNo,out_file_type,errorRate) ) {
        #ifdef DEBUG
            cout<<"cannot initialize OutputFile!"<<endl;
        #endif
		theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize OutputFile!");
		return false;
	} else if ( !g_theBaf.initialize(theBafParameter) ) {     //add by chenmin at 20020415
        #ifdef DEBUG
            cout<<"cannot initialize OutputFile!"<<endl;
        #endif
		theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize OutputFile!");
		return false;

	}
	
	//传入token话单的"分割符",用于token话单的"字段数"校验
	m_theRawFileProcess->setToken(token);
	
    return true;
}



//##ModelId=3B6F84CF0102
RawFileProcessor *RetApp::createFileObject()
{
  m_theRawFileProcess=0;
  m_theRawFileProcess = new GeneralFile;
  return m_theRawFileProcess;
}

//##ModelId=3B6F84D000D2
RawCdrProcessor *RetApp::createCdrProcessObject(int cdrType)
{
	switch (cdrType) {
		case 1 :
			m_theCdrProcess = new FixedRawCdrProcessor;
			m_RetLog_->ShowMessage("Create Fixed cdr object.\n",0);
			break;
		case 2 :
			m_theCdrProcess = new TokenRawCdrProcessor;
			m_RetLog_->ShowMessage("Create Token cdr object.\n",0);
			break;
		default :
			m_theCdrProcess=0;
			break;
	}
	return m_theCdrProcess;
}

//##ModelId=3BB322560003
bool RetApp::reset(string srcFileName)
{
	int fileLength;
	FieldInfo *pFieldInfo;
	unsigned char *srcFileBuffer;
	multimap<MyString,FieldInfo *> theFieldInfoMaps;

  pFieldInfo = new FieldInfo(0);
  //pFieldInfo->m_IsFormat       = true;
  pFieldInfo->m_FieldName_     = "FILENAME";
  pFieldInfo->m_FieldDesValue  = srcFileName;
  theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );

  m_RetLog_->clear();
  m_RetLog_->setFileName(srcFileName);
  
  if ( (fileLength=m_OutputFile.setFileBuffer(srcFileName,srcFileBuffer)) <= 0 ) 
  {

      theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "cannot open "+srcFileName+"!");
      m_OutputFile.mvToErrDir(srcFileName);    // 移动文件到错误目录
   	  return false;
  }
  // add by chenm 2005-8-2 记录输入文件长度
  m_RetLog_->setFileLenth(fileLength);
  
  if (!m_theRawFileProcess->setRawFile(srcFileName,srcFileBuffer,fileLength)) 
  {
       theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize setRawFile!");
       m_OutputFile.mvToErrDir(srcFileName);    // 移动文件到错误目录
   	return false;
  }
  
  if ( !m_OutputFile.reset(srcFileName) ) 
  {
      theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize m_OutputFile!");
      #ifdef DEBUG
            cout<<srcFileName<<",cannot initialize m_OutputFile!"<<endl;
         #endif
  	return false;
  }
  return true;
}

//##ModelId=3BD82E8E00CD
void RetApp::writeRetLog()
{
	char sourcetype[4];
	memset(sourcetype,0,4);
	memcpy(sourcetype,(m_RetLog_->getSourceType()).c_str(),2);
	if( strcmp(m_cdrFlag->m_callType,(char *)"01") == 0 ) {        // MOC 话单数
	    m_RetLog_->addMocSum();
	} else if( strcmp(m_cdrFlag->m_callType,(char *)"02") == 0 ) { // MTC 话单数
	    m_RetLog_->addMtcSum();
	} else if( strcmp(m_cdrFlag->m_callType,(char *)"03") == 0 ) { // FORW 话单数
	    m_RetLog_->addForwSum();
	} else if( strcmp(m_cdrFlag->m_callType,(char *)"07") == 0 ) { // SMMO 话单数
	    m_RetLog_->addSmocSum();
	} else if( strcmp(m_cdrFlag->m_callType,(char *)"08") == 0 ) { // SMMT 话单数
	    m_RetLog_->addSmtcSum();
	} else if( strcmp(m_cdrFlag->m_callType,(char *)"11") == 0 ) { // POC 话单数
	    m_RetLog_->addPocSum();
	} else {
	    m_RetLog_->addOtherSum();
  }
  
	if( m_cdrFlag->m_isError == true ) 
	{
	   m_RetLog_->addErrorSum();
	}
	m_RetLog_->addTotalCdrs();
}

//##ModelId=3B936C65007B
void RetApp::showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer,int size)
{
    int i;
	char tmp[1024];
	string srcCdr,strtmp;

	if ( !m_RetLog_->isShowFlag() )
		return ;

	sprintf(tmp,"%d",size);
	m_RetLog_->ShowMessage("\nCdr size is "+string(tmp)+", ",0);
    if (m_cdrType==2) 
    {
        memcpy(tmp,theSrcCdrBuffer,size);
        tmp[size]=0;
        srcCdr = tmp;
    } 
    else 
    {
	    for(i=0;i<size;i++) 
	    {
	    	sprintf(tmp,"%02X ",theSrcCdrBuffer[i]);
	    	srcCdr += tmp;
	    }
    }
    m_RetLog_->ShowMessage(srcCdr+string(" .\n"),0);


}

//##ModelId=3B936C65025B
void RetApp::showSrcFields(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int len;
	char tmp[4],show[2000];
	string srcField;
    
    
	if ( !m_RetLog_->isShowFlag() )
		return ;
    
    multimap<MyString,FieldInfo *>::iterator itr;
    	
	for ( itr=theFieldInfoMaps.begin();itr!=theFieldInfoMaps.end();++itr )
	{
		srcField=string("[ ");
		for( int i=0 ; i<(itr->second)->m_FieldLength ; i++ ) 
		{
			sprintf(tmp,"%02X ",(itr->second)->m_FieldSrcValue[i]);
			srcField += tmp;
		}
		srcField+="]";
		sprintf(show,"%-24s %-22s %-40s \n",(itr->second)->m_FieldName_.c_str(),
			          (itr->second)->m_FieldDesValue.c_str(),srcField.c_str() );
		m_RetLog_->ShowMessage(show,0);
	}
	sprintf(show,"%-24s %03d %-40s\n",(char *)"ERROR_CODE",m_cdrFlag->m_errorCode,m_cdrFlag->m_fieldName.c_str());
	m_RetLog_->ShowMessage(show,0);
}

int RetApp::mainProcess( int iRetFileType,string aSrcPath,string aSrcFileName,string sErrorType,
                         string sErrorCode,string sRetHead, string sRetTail)
{
  int iReturn = 1;
  bool error=false;
  int error_type = 0;
  string fullSrcFileName;
  char cTemp[256];
  
  //1.如果是上传文件回执，则进行错误判别：
  //如果是文件级错误，则进行告警，并将文件MV到错误目录
  //如果是正确回执，则直接将文件MV到备份目录
  //如果是记录级错误，则进行后续处理
  if(iRetFileType==1)
  {	
    fullSrcFileName= aSrcPath;
    fullSrcFileName+= aSrcFileName;
    iReturn =judgeErrType(fullSrcFileName);
    if(iReturn <0)
    {
    	theErrorMessages->insert(PP_ERROR_INITIALIZE, "the retFile is null!");
      return false;
    }
    error_type =m_retFlag;
    if(error_type ==1)
    {
    	 m_OutputFile.mvToBakDir(aSrcFileName);
    	 return true;
    }
    else if(error_type == 2)
    {
       sprintf(cTemp,"发生记录级错误回执！回执文件名：%s！",aSrcFileName.c_str());
       theErrorMessages->insert(PP_ERROR_INITIALIZE,cTemp);
    }	
    else if(error_type == 3)
    {
       sprintf(cTemp,"发生文件级错误回执！回执文件名：%s！错误代码:%s",aSrcFileName.c_str(),m_retCode.c_str());
       theErrorMessages->insert(PP_ERROR_INITIALIZE,cTemp);
       m_OutputFile.mvToErrDir(aSrcFileName);
       return true;
    }
  }
  
  //2.取得输出文件名
  //如果是上传回执，则输出文件名为输入文件名
  //如果是下发回执，则调用getRetFileName取得输出文件名
  if(iRetFileType==1)
  {
  	m_retfile_name = aSrcFileName;
  }
  else if(iRetFileType==2)
  {
    getRetFileName(aSrcFileName); 
    error_type = atoi(sErrorType.c_str()); 
  }
  
  //3.调用OutPutFile.reset进行重置输出句柄
  if ( !m_OutputFile.reset(m_retfile_name) ) 
	{
	  theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize m_OutputFile!");
	  #ifdef DEBUG
         cout<<m_retfile_name<<",cannot initialize m_OutputFile!"<<endl;
      #endif
	  return false;
	}
  
  //4.根据错误类型进行RUN
  //都是在记录级错误的情况下才对文件进行处理
  if (error_type == 2)//记录级错误
  	iReturn = run(aSrcFileName);
  
  if (iReturn<0)
  {
  	theErrorMessages->insert(PP_ERROR_INITIALIZE, "retfile run error!!");
  	return iReturn;
  }	  
  
  //5.如果是下发文件名，增加文件头尾记录
  if(iRetFileType==2)
  {	
    if (sRetHead.length()>0)
    {
    	m_OutputFile.writeFileHead(sRetHead);
    	iReturn =1;
    }
    else
    {
    	theErrorMessages->insert(PP_ERROR_INITIALIZE, "ret head is null!!");
    	iReturn = -1;
    }
  }   
  
  //6.关闭输出文件
  m_OutputFile.closeDesFiles(0);
  return iReturn; 	
}

void RetApp::getRetFileName(string sRetFileName)
{
   string sTempName;
   int iLength;
   
   iLength = sRetFileName.length();
   
   if(m_retfile_type==1)
   {
   	 sTempName = sRetFileName.substr(m_part_length,iLength-m_part_length);
   	 m_retfile_name = m_switch_part+sTempName;
   }
   else if(m_retfile_type==2)
   {
   	 sTempName = sRetFileName.substr(0,iLength-m_part_length);
   	 m_retfile_name = sTempName + m_switch_part;
   }
   else if(m_retfile_type==3)
   {
     m_retfile_name = m_switch_part + sRetFileName;
   }
   else
   {
     m_retfile_name = sRetFileName + m_switch_part;
   }
}

int RetApp::judgeErrType(string sFullSrcFileName)
{
    ifstream fpTemp;
    string sTemp;
    int tmplen;
    fpTemp.open((char *)sFullSrcFileName.c_str());
    
    getline(fpTemp,sTemp);//取第一行
    if(fpTemp.eof())
    {
      fpTemp.close();
      return -1;
    }
    tmplen=sTemp.length();
    if( (tmplen>m_errBegin)&&(sTemp.substr(m_errBegin,2)=="::") )//如果第一行的最后是"::"
    {
    	fpTemp.close();
      m_retFlag=3;
      m_retCode=sTemp.substr(m_errBegin+2,m_errCodeLen);
      if(m_retCode=="000")
      {
        m_retFlag=1;
        return 1;
      }
      return 1;
    }
    
    getline(fpTemp,sTemp);
    if(fpTemp.eof())
    {
      fpTemp.close();
      m_retFlag=1;
      return 1;
    }
    else
    {
    	fpTemp.close();
      m_retFlag=2;
    }
    return 1;
}