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

#include <vector>
#include "SendApp.h"
#include "base/SysParam.h"

OutputFile SendApp::m_OutputFile;


bool SendApp::setSrcPath(const char * r_srcFilePath)
{
  m_dir.setMaxScan(20000);
  m_dir.setPath(r_srcFilePath);
  m_dir.scanFiles(Directory::SF_MODIFYTIME);
  return true;
}
SendApp::SendApp(string theDataSource,int isShow)
{
	  string configName,configPath;
	  m_theSendParameter_   = 0;
	  m_theRawFileProcess = 0;
	  m_theCdrProcess     = 0;
    
	  configPath="../config/";
	  configName="ppdatesource.cfg";
    
	  m_SendLog_   = SendLog::getInstance();
	  m_cdrFlag    = CdrFlag::getInstance();
	  m_dataSource = atoi( theDataSource.c_str() );
    mb_CheckFileName = false;
	  if ( m_SendLog_ != 0  ) 
	  {
	     if ( isShow == 1 )
	  	   m_SendLog_->setShowFlag(true);	//显示调试信息
	     else
	  	   m_SendLog_->setShowFlag(false);	//不显示调试信息
	  }
    
	  m_theSendParameter_ = new SendParameterFromFile(configPath,configName,theDataSource);
}

SendApp::~SendApp()
{
	  free(m_srcCdrBuffer);
	  if( m_theRawFileProcess != 0 )
	  	delete m_theRawFileProcess;
	  if( m_theCdrProcess != 0 )
	  	delete m_theCdrProcess;
	  if( m_SendLog_ != 0 )
	  	delete m_SendLog_;
	  if( m_theSendParameter_ != 0 )
	  	delete m_theSendParameter_;
}

int SendApp::run(string aSrcFileName)
{
   #ifdef FP_DEBUG
    FP_BEGIN(run_construct)
  #endif

    int xxx;
    string filename;
    string sTemp;
    bool error=false;
    //FieldInfo aFieldInfo;
    FieldInfo *pFieldInfo;
	  int srcCdrLength,size,len;
	  char tmp[1024];
   #ifdef FP_DEBUG
    FP_END(run_construct)
  #endif
   #ifdef FP_DEBUG
    FP_BEGIN(run_reset)
  #endif

	  if ( !reset(aSrcFileName) ) 
	  {
      #ifdef DEBUG
          cout<<"SendApp reset error!"<<endl;
      #endif
	  	return -1;
	  }
   #ifdef FP_DEBUG
    FP_END(run_reset)
  #endif
    
	  size = aSrcFileName.size();
	  if( (len=aSrcFileName.find_last_of("/") ) < 0 ) 
	  {
	  	filename	= aSrcFileName;
	  } 
	  else 
	  {
	  	filename	= aSrcFileName.substr(len+1,size-len-1);
	  }
    
    //aFieldInfo.reset();
    //aFieldInfo.m_IsFormat       = true;
    //aFieldInfo.m_FieldName_     = "FILENAME";
    //aFieldInfo.m_FieldDesValue  = filename;

    FP_BEGIN(run)
    multimap<MyString,FieldInfo *>::iterator itr;
	  while ( (srcCdrLength = m_theRawFileProcess->getCdr(m_srcCdrBuffer)) > 0 )
	  {
          //showSrcCdrBuffer(m_srcCdrBuffer,srcCdrLength);
        FP_BEGIN(XXX) 
		      for(itr=m_FieldInfoMaps.begin();itr!=m_FieldInfoMaps.end();++itr)
		      {
		      	if( itr->second != 0 )
		      	{
		      		delete itr->second;	
		      	}	
		      }    
	  	    
	  	    m_FieldInfoMaps.clear();
	  	    CdrFlag::getInstance()->clear();
	  	    m_cdrFlag->m_fileHeadSize = m_output_filehead_size;	
	  	    m_cdrFlag->m_errcode_pos = m_errcode_pos;
	  	    
	  	    FP_BEGIN(srcdr)
	  	    sTemp="";
	        for(int i=0;i<srcCdrLength;i++) 
	        {
	        	sprintf(tmp,"%c",m_srcCdrBuffer[i]);
	        	sTemp += tmp;
	        }
          //#ifdef _DEBUG_
          //cout<<sTemp<<endl;
          //#endif
          m_cdrFlag->m_srcCdr = sTemp;
          FP_END(srcdr)

	        pFieldInfo = new FieldInfo(0);
	        pFieldInfo->m_FieldName_     = "FILENAME";
	        pFieldInfo->m_FieldDesValue  = filename;
          m_FieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );
        FP_END(XXX) 
          //m_FieldInfoMaps.push_back(aFieldInfo);
    
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
    
	        //m_PreProApi.processCdr(m_FieldInfoMaps,m_dataSource);       // 编程接口
          
          FP_BEGIN(write)
              if(m_OutputFile.write(m_FieldInfoMaps)<0)               // 写话单到标准话单
              {             
                  error=true;
                  break;
              }
          FP_END(write)
        
        FP_BEGIN(wlog)  
          writeSendLog();                                          // 写日志
	  	  FP_END(wlog)
	  	    //showSrcFields(m_FieldInfoMaps);
	  }
    FP_END(run)
    
    if(error) 
    {
        #ifdef _DEBUG_
            cout<<"文件处理错误！"<<endl;
        #endif
	      m_OutputFile.closeSrcFiles(1);
	  } 
	  else 
	  {
        #ifdef _DEBUG_
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tMOC   record sum is \t"<<m_SendLog_->getMocSum()<<endl;
            cout<<"\tMTC   record sum is \t"<<m_SendLog_->getMtcSum()<<endl;
            cout<<"\tPOC   record sum is \t"<<m_SendLog_->getPocSum()<<endl;
            cout<<"\tSMMO  record sum is \t"<<m_SendLog_->getSmocSum()<<endl;
            cout<<"\tSMTC  record sum is \t"<<m_SendLog_->getSmtcSum()<<endl;
            cout<<"\tFORW  record sum is \t"<<m_SendLog_->getForwSum()<<endl;
            cout<<"\tOther record sum is \t"<<m_SendLog_->getOtherSum()<<endl;
            cout<<"\tError record sum is \t"<<m_SendLog_->getErrorSum()<<endl;
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tTotal     CDR    is \t"<<m_SendLog_->getTotalCdrs()<<endl;
            cout<<"-----------------------------------------------"<<endl;
            cout<<"\tTotal SENDFEE    is \t"<<m_SendLog_->getTotalFee()<<endl;
            cout<<"\tTotal  ERRFEE    is \t"<<m_SendLog_->getErrorFee()<<endl;
            cout<<"-----------------------------------------------"<<endl;            
            cout<<"\tstart number     is \t"<<m_SendLog_->getBeginCdrNo()<<endl;
            cout<<"\tend   number     is \t"<<m_SendLog_->getEndCdrNo()<<endl;
            cout<<"-----------------------------------------------"<<endl;
        #endif
	      m_OutputFile.closeSrcFiles(0);
	      //m_SendLog_->clear();
	  }
   #ifdef FP_DEBUG
    FP_BEGIN(run_delete)
  #endif
	  
	  for(itr=m_FieldInfoMaps.begin();itr!=m_FieldInfoMaps.end();++itr)
	  {
	  	if( itr->second != 0 )
	  	{
	  		delete itr->second;	
	  	}	
	  }
	  m_FieldInfoMaps.clear();
   #ifdef FP_DEBUG
    FP_END(run_delete)
  #endif
    
	  return 1;
}

bool SendApp::initialize(string theSwitchName,vector <string> &pathMessage,
                         float errorRate,const string isBackupSrc,const string isBackupDes,
                         const string isIndb,string isCheckCdrNo,bool bCheckFilename)
{
    vector<string> theConvertRule,theValidRule;
	  string aRawFileType,aCdrType,out_file_type,out_file_htype/*add by dingj */,token,errcode_pos;
	  string file_header_size,file_tail_size,cdr_size,real_block_size;    // FIX Cdrs block parameters
	  string file_name_head,file_dateinfo,file_namefield3,file_name_end,file_namedate;//outputfile parameters
	  string output_filehead_size;
	  string file_freq;//add for vpdn 2010-4-9 17:54:53 by zhaoly	  
	  vector<string> theStandardFile,theStandardFileHead,theStandardFileTail;
	  vector<string> theRawFileParameter,theCdrParameter,theBafParameter;
	  
    aCdrType = "2";
    //申请记录缓存
	  if ( (m_srcCdrBuffer = (unsigned char *)malloc(20480)) == 0 )
    {
      #ifdef DEBUG
          cout<<"malloc src cdr buffer error!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc src cdr buffer error!");
	    return false;
    }
	  
	  //设置标准输出为不带缓冲的输出	  
	  setbuf(stdout,NULL);
    
	  if ( m_theSendParameter_ == 0 ) 
	  {
      #ifdef DEBUG
          cout<<"cannot create theSendParameter object!"<<endl;
      #endif
	  	theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create theSendParameter object!");
	  	return false;
	  }
    
    if(!theSwitchName.empty()) 
    {
	     if ( !m_theSendParameter_->initialize(theSwitchName,pathMessage) ) 
	     {
            #ifdef DEBUG
                cout<<"cannot initialize SendParameter!"<<endl;
            #endif
	          theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize SendParameter!");
	     	    return false;
	      }
	  }

    m_theSendParameter_->getValue("token", "token",token);
	  m_theSendParameter_->getValue("output_file_type","output_file_type", out_file_type);
	  /* add by dingj 2010-3-28 09:47:56 
	   * for demiliter of header in output file
	  */
	  m_theSendParameter_->getValue("output_file_type","output_file_htype", out_file_htype);
	  	  
	  m_theSendParameter_->getValue("output_headtail_size", "output_filehead_size",output_filehead_size);
	  m_output_filehead_size = atoi(output_filehead_size.c_str());
	  //m_cdrFlag->m_fileHeadSize = m_output_filehead_size;
	  m_theSendParameter_->getValue("output_file_name", "file_name_head",m_filename_head);
	  m_theSendParameter_->getValue("output_file_name", "file_dateinfo",m_file_dateinfo);
	  //add by zhaoly for 3G-DM-IMEI 2009-4-28 0:22:19
    m_theSendParameter_->getValue("output_file_name", "file_namefield2",m_file_namefield2);
    
	  m_theSendParameter_->getValue("output_file_name", "file_namefield3",m_file_namefield3);
	  m_theSendParameter_->getValue("output_file_name", "file_name_end",m_filename_end);
    
    m_theSendParameter_->getValue("src_filename","file_pattern",m_filepattern);
    mb_CheckFileName = bCheckFilename;
    
    m_theSendParameter_->getValue("errcode_pos","valid_errcode_pos",errcode_pos);
    m_errcode_pos = atoi(errcode_pos.c_str());
    
    
    
    m_theSendParameter_->getValue("file_frequency","output_file_freq",file_freq); 
        
	  if ( createFileObject() == 0 )
	  {
      #ifdef DEBUG
          cout<<"cannot create file object!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create file object!");
	  	return false;
	  } 
	  else if ( createCdrProcessObject() == 0 ) 
	  {
      #ifdef DEBUG
          cout<<"cannot create CDR Process object!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create CDR Process object!");
	  	return false;
	  }
	  
	  //得到section为"record"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("record",theCdrParameter) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot get record  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get record  SendParameter!");
	  	return false;
	  }
	  //得到section为"convert"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("convert",theConvertRule) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot get convert  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get convert  SendParameter!");
	  	return false;
	  }
	  //得到section为"valid"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("valid",theValidRule) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot get valid  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot get valid  SendParameter!");
	  	return false;
	  }
	  //得到section为"output_file"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("output_file",theStandardFile) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot coutput file  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot coutput file  SendParameter!");
	  	return false;
	  }
	  //得到section为"output_file_head"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("output_file_head",theStandardFileHead) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot coutput file  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot coutput file  SendParameter!");
	  	return false;
	  }	
	  //得到section为"output_file_tail"的配置文件内容
	  if ( !m_theSendParameter_->getSectionList("output_file_tail",theStandardFileTail) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot coutput file  SendParameter!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_GET_CONFIG, "cannot coutput file  SendParameter!");
	  	return false;
	  }	
    
    m_FieldValidators.setValidType("fields");
    //m_FieldValidators.setValidType("fields");
    if (!m_FieldValidators.initialize(theValidRule)) 
    {              // 文件校验规则初始化
      #ifdef DEBUG
          cout<<"cannot initialize file Valid rule!"<<endl;
      #endif
      theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize file Valid rule!");
      return false;
	  }
	  else if ( !m_theCdrProcess->setToken(token) ) 
	  {
      #ifdef DEBUG
          cout<<"cannot initialize CdrProcess setToken!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess setToken!");
	  	return false;
    }
    else if ( m_theRawFileProcess->initialize(m_theSendParameter_,atoi( aCdrType.c_str() ) ) == 0 )
    {
      #ifdef DEBUG
          cout<<"cannot initialize RawFileProcessor!"<<endl;
      #endif
	  	theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize RawFileProcessor!");
	  	return false;
	  }
	  else if ( !m_theCdrProcess->initialize(theCdrParameter) )
	  {
      #ifdef DEBUG
          cout<<"cannot initialize CdrProcess!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess!");
	    return false;
	  }
	  else if ( !m_theCdrProcess->setRule(theConvertRule,theValidRule))
	  {
      #ifdef DEBUG
          cout<<"cannot initialize CdrProcess setRule!"<<endl;
      #endif
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize CdrProcess setRule!");
	  	return false;
	  }
	  else if ( !m_OutputFile.initialize(theStandardFile,
	  	                                 theStandardFileHead,theStandardFileTail,
	  	                                 pathMessage,isBackupSrc,isBackupDes,isIndb,
	                                     isCheckCdrNo,out_file_type,out_file_htype,
	                                     file_freq,output_filehead_size,errorRate) ) /* add by dingj : parameter out_file_htype*/			                                     
	  {
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

RawFileProcessor *SendApp::createFileObject()
{
	  m_theRawFileProcess = 0;
	  m_theRawFileProcess = new GeneralFile;
	  m_SendLog_->ShowMessage("Create General file object.\n",0);
    return m_theRawFileProcess;
}

RawCdrProcessor *SendApp::createCdrProcessObject()
{
	  m_theCdrProcess = 0;
	  m_theCdrProcess = new TokenRawCdrProcessor;
	  m_SendLog_->ShowMessage("Create Token cdr object.\n",0);	  
	  return m_theCdrProcess;
}

//##ModelId=3BB322560003
bool SendApp::reset(string srcFileName)
{
	  int fileLength;
	  FieldInfo *pFieldInfo;
	  unsigned char *srcFileBuffer;
	  multimap<MyString,FieldInfo *> theFieldInfoMaps;
    
    //aFieldInfo.m_IsFormat       = true;
    //aFieldInfo.m_FieldName_     = "FILENAME";
    //aFieldInfo.m_FieldDesValue  = srcFileName;
    //theFieldInfoMaps.push_back(aFieldInfo);
    pFieldInfo = new FieldInfo(0);
    pFieldInfo->m_FieldName_     = "FILENAME";
    pFieldInfo->m_FieldDesValue  = srcFileName;
    theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );
    
    //m_SendLog_->clear();
	  m_SendLog_->setFileName(srcFileName);
        
	  if ( (fileLength=m_OutputFile.setFileBuffer(srcFileName,srcFileBuffer)) <= 0 ) 
	  {
	      theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "cannot open "+srcFileName+"!");
	      m_OutputFile.mvToErrDir(srcFileName);          // 移动文件到错误目录
	      return false;
    }
         //#ifdef _DEBUG_
         //   cout<<"SRC CDR!!: "<<srcFileBuffer<<endl;
         //#endif

    // add by chenm 2005-8-2 记录输入文件长度
    m_SendLog_->setFileLenth(fileLength);
    
	  if (!m_theRawFileProcess->setRawFile(srcFileName,srcFileBuffer,fileLength)) 
	  {
	      theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize setRawFile!");
	      #ifdef DEBUG
             cout<<"cannot initialize setRawFile!"<<endl;
          #endif
	      m_OutputFile.mvToErrDir(srcFileName);    // 移动文件到错误目录
	      return false;
    }

	  return true;
}

void SendApp::writeSendLog()
{
	  char sourcetype[4];
	  memset(sourcetype,0,4);
	  memcpy(sourcetype,(m_SendLog_->getSourceType()).c_str(),2);

	  if( strcmp(m_cdrFlag->m_callType,(char *)"01") == 0 ) {        // MOC 话单数
	      m_SendLog_->addMocSum();
	  } else if( strcmp(m_cdrFlag->m_callType,(char *)"02") == 0 ) { // MTC 话单数
	      m_SendLog_->addMtcSum();
	  } else if( strcmp(m_cdrFlag->m_callType,(char *)"03") == 0 ) { // FORW 话单数
	      m_SendLog_->addForwSum();
	  } else if( strcmp(m_cdrFlag->m_callType,(char *)"07") == 0 ) { // SMMO 话单数
	      m_SendLog_->addSmocSum();
	  } else if( strcmp(m_cdrFlag->m_callType,(char *)"08") == 0 ) { // SMMT 话单数
	      m_SendLog_->addSmtcSum();
	  } else if( strcmp(m_cdrFlag->m_callType,(char *)"11") == 0 ) { // POC 话单数
	      m_SendLog_->addPocSum();
	  } else {
	      m_SendLog_->addOtherSum();
	  }

	  if( m_cdrFlag->m_isError == true ) 
	  {
	     m_SendLog_->addErrorSum();
	     m_SendLog_->addErrorFee(m_cdrFlag->m_totalFee);
	  }
	  m_SendLog_->addTotalCdrs();
	  m_SendLog_->addTotalFee(m_cdrFlag->m_totalFee); 
}

void SendApp::showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer,int size)
{
    int i;
	  char tmp[1024];
	  string srcCdr,strtmp;
    
	  if ( !m_SendLog_->isShowFlag() )
	  	return ;
    
	  sprintf(tmp,"%d",size);
	  m_SendLog_->ShowMessage("\nCdr size is "+string(tmp)+", ",0);
    
	  for(i=0;i<size;i++) 
	  {
	  	sprintf(tmp,"%02X ",theSrcCdrBuffer[i]);
	  	srcCdr += tmp;
	  }
    
    m_SendLog_->ShowMessage(srcCdr+string(" .\n"),0);


}

//void SendApp::showSrcFields(vector<FieldInfo> &theFieldInfoMaps)
void SendApp::showSrcFields(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int len;
	  char tmp[4],show[2000];
	  string srcField;
      
      
	  if ( !m_SendLog_->isShowFlag() )
	  	return ;
      
      //len=theFieldInfoMaps.size();
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
	    m_SendLog_->ShowMessage(show,0);
	  }
	  sprintf(show,"%-24s %03d %-40s\n",(char *)"ERROR_CODE",m_cdrFlag->m_errorCode,m_cdrFlag->m_fieldName.c_str());
	  m_SendLog_->ShowMessage(show,0);
}

//文件生成主函数
int SendApp::mainProcess(int flag,int iFileNo,string aSrcFilePath,int isHeadOrTail)
{
    int iReturn = 1;
    bool error=false;
   
   #ifdef FP_DEBUG
    FP_BEGIN(mainProcess_0)
  #endif
    //1.根据文件号获得上传文件名(移到MyApplication.cpp)
    //getOutFileName(iFileNo,iFileNoLen);  
    m_SendLog_->setDesFileName(m_outfile_name);
   #ifdef FP_DEBUG
    FP_END(mainProcess_0)
  #endif
    
   #ifdef FP_DEBUG
    FP_BEGIN(mainProcess_1)
  #endif
    //2.重置上传文件句柄
    if ( !m_OutputFile.reset(m_outfile_name) ) 
	  {
	    theErrorMessages->insert(PP_ERROR_INITIALIZE, "cannot initialize m_OutputFile!");
	    #ifdef DEBUG
           cout<<m_outfile_name<<",cannot initialize m_OutputFile!"<<endl;
      #endif
	    return -1;
	  }
   #ifdef FP_DEBUG
    FP_END(mainProcess_1)
  #endif
    
   #ifdef FP_DEBUG
    FP_BEGIN(mainProcess_2)
  #endif
    //3.根据上传标志生成文件 1.生成完整文件 0.仅生成空文件 4.生成实时文件
    if(flag==1)
      iReturn = getDataFromFile(aSrcFilePath);
    else if(flag==4)
    	iReturn = getDataFromFileRt(m_outfile_name);
   #ifdef FP_DEBUG
    FP_END(mainProcess_2)
  #endif
      
    if(iReturn <0)
    {
    	theErrorMessages->insert(PP_ERROR_INITIALIZE, "目录中取文件失败");
    	return -1;
    }	
    //else if(iReturn == NOFILE_IDLE)
    //{
    	//return iReturn;
    //}
   #ifdef FP_DEBUG
    FP_BEGIN(mainProcess_3)
  #endif
    
    if(isHeadOrTail==1)
    {
      if(m_OutputFile.writeFileHead(iFileNo)<0)
      {
    	   error=true;
      }
    }
    else if(isHeadOrTail==2)
    {
    	if(m_OutputFile.writeFileHead(iFileNo)<0)
      {
    	   error=true;
      }
    	if(m_OutputFile.writeFileTail(iFileNo)<0)
      {
    	   error=true;
      }
    }
          
    //4.关闭输出文件
    if(error) {
      m_OutputFile.closeDesFiles(1); }
    else {
      m_OutputFile.closeDesFiles(0); }
    #ifdef FP_DEBUG
    FP_END(mainProcess_3)
  #endif
   
    return iReturn; 	
}

//从源文件目录取文件并调用RUN函数进行处理
int SendApp::getDataFromFile(string aSrcFilePath)
{
  	int i;
  	int nTotal=0;
  	int total=0;
  	int iReturn=1;
  	
    char fileNameTmp[80+1];
    string fileName;
    char cmd[100];
    int iStatus=0;
    //int iFlag = 0;

    m_dir.setMaxScan(20000);
    
    m_dir.setPath(aSrcFilePath.c_str());
    m_dir.scanFiles(Directory::SF_MODIFYTIME);

    memset(fileNameTmp,0,sizeof(fileNameTmp));
    m_SendLog_->clear();
    while((m_dir.getFile(fileNameTmp, 80)>0)&&(total<=79999))
    {
  	   fileName = fileNameTmp;
  	   if ((!mb_CheckFileName) ||
  	    	  (mb_CheckFileName && fileName.length() >= m_filepattern.length() && strncmp(fileName.c_str(),m_filepattern.c_str(),strlen(m_filepattern.c_str())) == 0))
  	   {
  	   	  iReturn  = run( fileName );
  	   	  total = m_SendLog_->getTotalCdrs();
  	   }
  	   else
  	   	continue;
  	   	
       //iFlag = 1;  		
    }
  	
  	//if(!iFlag)
      //return NOFILE_IDLE;
  	
  	return iReturn;
}

//从源文件目录取文件并调用RUN函数进行处理
int SendApp::getDataFromFileRt(string aSrcFileName)
{
  	int i;
  	int iReturn=1;
   #ifdef FP_DEBUG
    FP_BEGIN(getDataFromFileRt)
  #endif

    m_SendLog_->clear();    
    iReturn = run(aSrcFileName);
   #ifdef FP_DEBUG
    FP_END(getDataFromFileRt)
  #endif

  	return iReturn;
}

//根据文件号取得输出文件名
int SendApp::getOutFileName(int iFileNo,int iFileNoLen,string &sOutFileName)
{
	  string sNameDate;
	  string sTempField;
	  char cTemp[5+1];
	  
	  setNowTime();
	  
    if (strcmp(m_file_dateinfo.c_str(),"YYYYMMDD")==0)
      sNameDate = m_nowTime.substr(0,8); 
    else if (strcmp(m_file_dateinfo.c_str(),"YYMMDD")==0)
      sNameDate = m_nowTime.substr(2,6);
    else if (strcmp(m_file_dateinfo.c_str(),"MMDD")==0)
      sNameDate = m_nowTime.substr(4,4);
    else if (strcmp(m_file_dateinfo.c_str(),"YYYYMMDDHHMM")==0)
      sNameDate = m_nowTime.substr(0,12);
    else if (strcmp(m_file_dateinfo.c_str(),"YYYYMMDDHHMMSS")==0)
      sNameDate = m_nowTime.substr(0,14);
    else if (strcmp(m_file_dateinfo.c_str(),"YYMMDDHHMM")==0)
      sNameDate = m_nowTime.substr(2,10);
    // added by houyq 20100407 start
    // 为适应VPDN一点支付文件，文件名配置中增加日期格式。
    else if (strcmp(m_file_dateinfo.c_str(),"YYYYMM")==0)
      sNameDate = m_nowTime.substr(0,6);
    // added by houyq 20100407 end
    
    if(strcmp(m_file_namefield3.c_str(),"FILENO")==0)
    {
    	memset(cTemp,0,sizeof(cTemp));
    	sprintf(cTemp,"%d",iFileNo);
    	FormatConversion::changeFormatRz(cTemp,iFileNoLen);
    	sTempField = string(cTemp);
    }
    else
    {
    	sTempField = m_file_namefield3;
    }
    
    m_outfile_name = "";
    m_outfile_name = m_filename_head;
    m_outfile_name += sNameDate;
    //add by zhaoly for 3G-DM-IMEI 2009-4-28 0:22:19
    m_outfile_name += m_file_namefield2;      
   
    m_outfile_name += sTempField;
    m_outfile_name += ".";
    m_outfile_name += m_filename_end;	
    
    sOutFileName = m_outfile_name;
    
    return 1;
} 

int SendApp::getOutFileNameRt(const string &aSrcFilePath,string &sOutFileName)
{
    char fileNameTmp[80+1];
    string fileName;
    int iFlag=0;
    int iRet = 0;
    
    memset(fileNameTmp,0,sizeof(fileNameTmp));
    iRet = m_dir.getFile(fileNameTmp, 80);
    if(iRet == 0)
    {
      m_dir.scanFiles(Directory::SF_MODIFYTIME);
      iRet = m_dir.getFile(fileNameTmp, 80);
    }
    while(iRet>0)
    {
    	fileName = fileNameTmp;
    	#ifdef _DEBUG_
    	cout << "CURRENT FILENAME IS: "<< fileName << endl;
    	#endif
      if ((!mb_CheckFileName) ||
  	    	(mb_CheckFileName && fileName.length() >= m_filepattern.length() && strncmp(fileName.c_str(),m_filepattern.c_str(),strlen(m_filepattern.c_str())) == 0)) 
    	{
    	  iFlag=1;
    	  break;
    	} else 
    	{  
      	memset(fileNameTmp,0,sizeof(fileNameTmp));
      	iRet = m_dir.getFile(fileNameTmp, 80);
      	continue;
      }
    }
    
    if(iFlag == 1)
    {
    	m_outfile_name = fileName;
    	sOutFileName = fileName;
    }
    else
    {
    	sOutFileName = "";
    	m_outfile_name = "";
    }

    return 1;
} 

void SendApp::setNowTime(void)
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



