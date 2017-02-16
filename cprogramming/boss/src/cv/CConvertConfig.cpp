#include <assert.h>
#include <stdio.h>
#include "CDRConvertApp.h"
#include "CConvertConfig.h"


CConvertConfig::CConvertConfig()
{

}

  
CConvertConfig:: ~CConvertConfig()
{
 
}


CConvertConfig *CConvertConfig::stCovertCfg_Instance = NULL;

//创建类实例
CConvertConfig* CConvertConfig::Create()
{
	if(stCovertCfg_Instance == NULL);
		stCovertCfg_Instance = new CConvertConfig;
	return stCovertCfg_Instance;
		
}

//读取配置文件，获得配置信息
bool	CConvertConfig::GetSystemConfigInfo(SysParam *pSysParam,string sProcName,int iChannelNo) 
{	
		
		assert(pSysParam != NULL);		
		char channelNo[4], c_subChanNo[10];
    float theErrorRate;
    int i_subProcessNo = 0;
    vector <string> pathMessage;
    string root, section, name, value, subPath, subPathTmp;
    string switchName, isBackupDes, isBackupSrc, isCheckCdrNo,isSplitDes, isSplitLongCdr;
    int MaxCdrNum;
    
    root = SECTDELIM + sProcName;    
    sprintf(channelNo, "%d", iChannelNo);
    subPathTmp = subPath = root + SECTDELIM + channelNo + SECTDELIM + "channel_0";      // "//pp//0//channel_0"
    pSysParam->setSectionPath(subPathTmp);                              //(root + SECTDELIM + channelNo);
    
    printf(subPathTmp.c_str());
    printf("\n");  
    string sTmpConfigFile = pSysParam->getFileName()  ;
    printf(sTmpConfigFile.c_str());
    while(pSysParam->getSubSection(subPathTmp))
    {
    		CHANNEL_INFO tmpChannelInfo;		
   			//printf("222222\n");
	
				// get the SOURCE_TYPE's value for the process's log
        name = "SOURCE_TYPE";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
       //	记录处理的源类型日志
       // m_PpLog_->setSourceType(value);
       
				//交换机
        name = "switch_name";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }        
        tmpChannelInfo.m_sSwitchName = value;
        
        //地市
        name = "eparchy";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }  
        tmpChannelInfo.m_sEparchy = value;
        
        //话单类型
        name = "cdr_type";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }        
        tmpChannelInfo.m_iCdrType = atoi(value.c_str());
        
        //连接数据库信息
        name = "server_name_para";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sServereName_para = value;
        
        name = "user_name_para";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sUserName_para = value;
        
        name = "password_para";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sPassword_para = value;
        
        name = "server_name_bill";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sServereName_bill = value;
        
        name = "user_name_bill";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sUserName_bill = value;
        
        name = "password_bill";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_sPassword_bill = value;
        
        name = "backup_srcfile";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        if(!strcasecmp(value.c_str(),"yes"))
        	tmpChannelInfo.m_bBackupSrcFile = true;
        else
        	tmpChannelInfo.m_bBackupSrcFile = false;
        
       		
        name = "backup_desfile";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        if(!strcasecmp(value.c_str(),"yes"))
        	tmpChannelInfo.m_bBackupDesFile = true;
        else
        	tmpChannelInfo.m_bBackupDesFile = false;
        	
        name = "check_cdrNo";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        if(!strcasecmp(value.c_str(),"yes"))
        	tmpChannelInfo.m_bCheck_cdrNo = true;
        else
        	tmpChannelInfo.m_bCheck_cdrNo = false;
       
        name = "separate_token";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_cSeprateToken = value;
        
        name = "meter_count";
        if (!pSysParam->getValue(subPath, name, value))
        {
          theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
          return false;
        }
        tmpChannelInfo.m_iMeterCount = atoi(value.c_str());
                 
                  
        //下面配置项暂时不用	
        name = "split_desfile";
        if (!pSysParam->getValue(subPath, name, value))
        {
            theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
            return false;
        }
        isSplitDes = value;
        name = "max_cdrNum";
        if (!pSysParam->getValue(subPath, name, value))
        {
            theErrorMessages->insert(EAPPFRM_SYSVAL, subPath + SECTDELIM + name);
            return false;
        }
        MaxCdrNum = atoi(value.c_str());
				
				if(!getPath(pSysParam,subPath, tmpChannelInfo ))
				{
					 theErrorMessages->insert(CV_ERROR_GET_CONFIG, "cannot get path's parameter! ");
           return false;
				}
       
        m_vecChannelInfo.push_back(tmpChannelInfo);        
        
        // 拼凑下一个子通道的路径信息
        ++i_subProcessNo;
        sprintf(c_subChanNo, "channel_%d", i_subProcessNo);
        subPathTmp = subPath = root + SECTDELIM + channelNo + SECTDELIM + c_subChanNo;  // "//pp//0//channel_1"
        pSysParam->setSectionPath(subPathTmp);
    }
   
    //cout<<m_channelInfos.size()<<endl;

    // 判断配置文件中的通道下是否有子通道信息
    if(m_vecChannelInfo.size() == 0)
    {
    	  #ifdef _DEBUG_
            cout << "sub channel number is 0!" << endl;
        #endif
        theErrorMessages->insert(CV_ERROR_INITIALIZE, "sub channel number is 0!");
        return false;
    }
    
    SetCdrMonth() ;
    GetAllParaInfo();
	return true;
}

//获取配置文件子通道项下 path节点的配置信息
bool  CConvertConfig ::getPath(SysParam *pSysParam,string subPath,CHANNEL_INFO &channel_Info )
{
			
		string mypath, value, name;
    mypath = subPath + SECTDELIM + "path";
    name = "source_path";
    if (!pSysParam->getValue(mypath, name, value))
    {
    	  theErrorMessages->insert(EAPPFRM_SYSVAL, mypath + SECTDELIM + name + value);
        return false;
    }
    channel_Info.m_Path.m_sSourcePath = value;
    
    name = "src_backup_path";
    if (!pSysParam->getValue(mypath, name, value))
    {
        theErrorMessages->insert(EAPPFRM_SYSVAL, mypath + SECTDELIM + name + value);
        return false;
    }
    channel_Info.m_Path.m_sSrcBackupPath = value;
    
    name = "destination_path";
    if (!pSysParam->getValue(mypath, name, value))
    {
        theErrorMessages->insert(EAPPFRM_SYSVAL, mypath + SECTDELIM + name + value);
        return false;
    }   
    channel_Info.m_Path.m_sDestPath = value;
    
    name = "des_backup_path";
    if (!pSysParam->getValue(mypath, name, value))
    {
        theErrorMessages->insert(EAPPFRM_SYSVAL, mypath + SECTDELIM + name + value);
        return false;
    }
    channel_Info.m_Path.m_sDstBakupPath = value;
     
    name = "file_error_path";
    if (!pSysParam->getValue(mypath, name, value))
    {
        theErrorMessages->insert(EAPPFRM_SYSVAL, mypath + SECTDELIM + name + value);
        return false;
    }
    channel_Info.m_Path.m_sErrPath = value;
    
    
    return true;
	
}

//获取参数的数据库的连接参数  
bool	CConvertConfig::GetDbLinkInfo_para(string &sSevername,string &sUser, string &sPassword)const
{	
	sSevername = m_vecChannelInfo[0].m_sServereName_para;
	sUser = m_vecChannelInfo[0].m_sUserName_para;
	sPassword = m_vecChannelInfo[0].m_sPassword_para;
	return true;
}

//获取清单的数据库的连接参数
bool	CConvertConfig::GetDbLinkInfo_bill(string &sSevername,string &sUser, string &sPassword)const
{	
	sSevername = m_vecChannelInfo[0].m_sServereName_bill;
	sUser = m_vecChannelInfo[0].m_sUserName_bill;
	sPassword = m_vecChannelInfo[0].m_sPassword_bill;
	return true;
}

//获取单体实例的对象指针
CConvertConfig *CConvertConfig::GetInstance()
{
	 assert(stCovertCfg_Instance != NULL);
	return stCovertCfg_Instance;
}

//获取通道下子通道的数量
int CConvertConfig::GetChannelCount()
{	
	return m_vecChannelInfo.size();
}	


//获得索引标识得子通道的跳表满刻度值
int	CConvertConfig::GetMeterCount(int iIndex)const
{	
  return	m_vecChannelInfo[iIndex].m_iMeterCount;  
}

//获得索引标识得子通道的交换机名
string	CConvertConfig::GetSwitchName(int iIndex)const
{	
	return m_vecChannelInfo[iIndex].m_sSwitchName;
}

//获得索引标识得子通道的话单分割序列符合
string	CConvertConfig::GetSeparateToken(int iIndex)const
{
		return m_vecChannelInfo[iIndex].m_cSeprateToken;
}

//获得索引标识得子通道的源路径
string	CConvertConfig::GetSourcePath(int iIndex)const
{	
	return	m_vecChannelInfo[iIndex].m_Path.m_sSourcePath;
}

//获得索引标识得子通道的备份源路径
string	CConvertConfig::GetSrcBakupPath(int iIndex)const
{
	return	m_vecChannelInfo[iIndex].m_Path.m_sSrcBackupPath;
	
}

//获得索引标识得子通道的目标输出路径
string	CConvertConfig::GetDestPath(int iIndex)const
{	
	return	m_vecChannelInfo[iIndex].m_Path.m_sDestPath;
	
}
//获得索引标识得子通道的备份输出路径
string	CConvertConfig::GetDstBackupPath(int iIndex)const
{
	return m_vecChannelInfo[iIndex].m_Path.m_sDstBakupPath;
	
}


//获得索引标识子通道的错误话单输出路径
string CConvertConfig::GetErrCdrOutputPath(int iIndex)const
{	
	return m_vecChannelInfo[iIndex].m_Path.m_sErrPath;
}
	
	
//获得索引标识得子通道的源文件是否备份，true校验份；false 不备份
bool	CConvertConfig::IsBackupSrcFile(int iIndex)const
{	
	return m_vecChannelInfo[iIndex].m_bBackupSrcFile;
	
}

// 备获得索引标识得子通道的输出文件是否备份，true校验份；false 不备份
bool	CConvertConfig::IsBackupDstFile(int iIndex)const
{
	return m_vecChannelInfo[iIndex].m_bBackupDesFile;
	return true;
}

//获得索引标识得子通道的话单是否校验，true校验；false 不校验
bool	CConvertConfig::IsCheckCDR(int iIndex)const
{
	return m_vecChannelInfo[iIndex].m_bCheck_cdrNo;
}
//获取索引标识得子通道的地市名
string CConvertConfig::GetEparchy(int iIndex)
{
	return m_vecChannelInfo[iIndex].m_sEparchy;
	
}
	
	//获取索引标识得子通道的跳表话单类型。净跳次，累计跳次 或 翻转跳次
int CConvertConfig::GetCdrType( int iIndex)
{
  return m_vecChannelInfo[iIndex].m_iCdrType;	
}


//设置活动账期所属的月份
bool  CConvertConfig::SetCdrMonth()
{
	
	DbInterface dbPara;				//连接参数库
	StringVector  svParaRecordSet,svErrorSet;
	string sUserName,sPassWord,sService;
	string sEparch,sSwitchName;
	GetDbLinkInfo_para(sService,sUserName, sPassWord);
//	sEparch = CConvertConfig::GetInstance()->GetEparchy(iRuleChannelIndex);
//	sSwitchName = CConvertConfig::GetInstance()->GetSwitchName(iRuleChannelIndex);
	if(dbPara.connect(sUserName.c_str(),sPassWord.c_str(),sService.c_str())) // "ucr_param","tjdev","gwacttst_tj"
	{
		theErrorMessages->insert(CV_ERROR_DB_CON,"connect para db failure");
			#ifdef _DEBUG_
		     cout<<"connect para db failure : "<<" "<<__FILE__<<__LINE__<<endl;
		     printf("连接参数库失败\n");
		  #endif
		
		//连接参数库失败
		return false;
	}	
	
	svParaRecordSet.clear();
  svErrorSet.clear();  
  if(dbPara.dataBind(svParaRecordSet,svErrorSet))
	{
		//参数记录集绑定失败   
		theErrorMessages->insert(CV_ERROR_DB_BIND,"bind para db fail!");
		#ifdef _DEBUG_
		     cout<<"bind para db failure : "<<" "<<__FILE__<<__LINE__<<endl;
		     printf("参数记录集绑定失败  \n");  
		#endif
		              
		return false;
	}
	
	string strSql;
	strSql = "select cycle_id from TD_LSMETER_CYCLE  where cycle_state=1";//	
	printf(strSql.c_str());
	printf("\n");
	int nTotal,nSuccess,nError;
	if(dbPara.executeSql(strSql.c_str(),	nTotal,nSuccess,nError))
	{
		//参数查询失败
		theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get para sql error : " + strSql);
		#ifdef _DEBUG_
		   cout<<"exec get para sql error: "<<" "<<strSql<<__FILE__<<__LINE__<<endl;
		   printf("参数查询失败  \n");	     
		#endif	
		dbPara.disconnect();
		exit(0);	  
		return false;
	}
	
  string tmpstr;	
	//将该条记录转换成各个字段的值并保存		
	int i = 0;
	tmpstr=*(svParaRecordSet.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_nActiveDate = atol(tmpstr.c_str());
	
	return true;
}
//获取当前索引通道所指向的交换机话单所属的月份
long  CConvertConfig::GetCdrMonth()
{
	return m_nActiveDate;
	
}


//获取所有交换机的满表值. 所有号段的营业区信息
bool CConvertConfig::GetAllParaInfo()
{
	 OCCIConnect    cdrConPara ;     //到参数库的oracle连接对象  	 
	 StringVector  svParaRecordSet,svErrorSet;
	 string sUserName,sPassWord,sService;
	 string sEparch,sSwitchName;
	 GetDbLinkInfo_para(sService, sUserName, sPassWord);
	 if(!cdrConPara.connect(sUserName,sPassWord,sService,true))
	 {
		 //连接参数库失败	
		 #ifdef _DEBUG_
			 cout<<"dbname = "<<sService<< ", " <<"userName="<<sUserName<<" , passwd="<<sPassWord<<endl;
		   cout<<"m_cdrCon lianjie para db failure!!! "<<" "<<__FILE__<<__LINE__<<endl;
		 #endif
		 theErrorMessages->insert(CV_ERROR_DB_CON, " billDb connect fail:" );                   
		 exit(0) ;
	 } 
	
	
	//*************获取所有号段所属的营业区	***********************//
    char sql[2000]={"\0"};   
    strcpy(sql,"select distinct a.exchange_code, b.section_code  from td_fixphone_seg a,td_exchange_section_rel b where \
                 a.phone_seg = b.exchange_code");

    string sTmpExchange,sTmpSection;
		//从固网号段定义表，局向-营业区对应关系表获取所有号段对应的营业区		     
    try
    {	   	
   	  OCCISQL  occisql(&cdrConPara,sql);       	     
      occisql.executeQuery();            
      while (occisql.next())
		  {			  
			  sTmpExchange = occisql.getString(1);		
			  sTmpSection  = occisql.getString(2);  
			  m_mapSection.insert(map<string, string> :: value_type(sTmpExchange, sTmpSection)) ;
		  }   
    }
    catch (SQLException & e)
    {
     cout<<"查询未上移表异常!"<<endl;
     cout<<e.getMessage()<<endl;
     string sTmpStr = sql;		     
		 #ifdef _DEBUG_
		   cout<<"exec get bill sql error: "<<sTmpStr<<" "<<__FILE__<<__LINE__<<endl;
		 #endif				 
     theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get table td_tj_select_nbr sql error:  " + sTmpStr );	
     return false ;
    }				


	
	
	
	//****************获取所有交换机各个跳次类型的满刻度值***************//
	//SELECT A.OVERFLOW_ID,A.OVERFLOW  FROM BILL.T_OVER_FLOW A, BILL.T_FILE_CLASS B WHERE TO_CHAR(A.OVERFLOW_ID) = B.SWITCH_ID \
   //              GROUP BY A.OVERFLOW_ID ,A.OVERFLOW ORDER BY A.OVERFLOW_ID 
	  strcpy(sql,"select distinct overflow_id ,overflow from td_b_over_flow ");

    string sTmpSectionCode,sTmpFullScale;
		//从满刻度表中获取所有营业区的满刻度值
    try
    {	   	
   	  OCCISQL  occisql(&cdrConPara,sql);       	     
      occisql.executeQuery();            
      while (occisql.next())
		  {			  
			  sTmpSectionCode = occisql.getString(1);		
			  sTmpFullScale  = occisql.getString(2);  
			  m_mapFullScale.insert(map<string, string> :: value_type(sTmpSectionCode, sTmpFullScale)) ;
		  }   
    }
    catch (SQLException & e)
    {
      cout<<"查询所有交换机满表刻度异常!"<<endl;
      cout<<e.getMessage()<<endl;
      string sTmpStr = sql;		     
		  #ifdef _DEBUG_
		    cout<<"exec get bill sql error: "<<sTmpStr<<" "<<__FILE__<<__LINE__<<endl;
		  #endif				 
      theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get meter full scale sql error:  " + sTmpStr );	
      return false ;
    }				
    
/*
	svParaRecordSet.clear();
  svErrorSet.clear();
  
  if(dbPara.dataBind(svParaRecordSet,svErrorSet))
	{
		//参数记录集绑定失败   
		theErrorMessages->insert(CV_ERROR_DB_BIND,"bind para db fail!");
		#ifdef _DEBUG_
		     cout<<"bind para db failure : "<<" "<<__FILE__<<__LINE__<<endl;
		     printf("参数记录集绑定失败  \n");  
		#endif		              
		return false;
	}
	
	
	strSql = "select switch_type,meter_type,full_meter from TD_LSMETER_CFG ";//	
	printf(strSql.c_str());
	printf("\n");

	if(dbPara.executeSql(strSql.c_str(),	nTotal,nSuccess,nError))
	{
		//参数查询失败
		theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get para sql error : " + strSql);
		#ifdef _DEBUG_
		   cout<<"exec get para sql error: "<<" "<<strSql<<__FILE__<<__LINE__<<endl;
		   printf("参数查询失败  \n");	     
		#endif	
		dbPara.disconnect();
		exit(0);	  
		return false;
	}
	
 	
	//将该条记录转换成各个字段的值并保存		
	i = 0;
	tmpstr=*(svParaRecordSet.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_nActiveDate = atol(tmpstr.c_str());	
	*/
	
	cdrConPara.disConnect();
	return true;
}

//获取特定号段的营业区号
bool CConvertConfig::getSectionCodeByExchange(const string & sExchange,string &sSectionCode)
{
	map<string,string>::iterator itr;
	string sTmpStr ;
	int i;
	int nLength = strlen((char*)sExchange.c_str());
	
	for (i =nLength ;i>2 ;i --)  //号段最低到3位
	{
		 sTmpStr = sExchange.substr(0,i);
	   itr = m_mapSection.find(sTmpStr);
	   if(itr != m_mapSection.end())
	   {
		   sSectionCode = (*itr).second;
		   break;
	   }
	   else
	      continue;
	}
	if(i == 2 || (m_mapSection.empty())) 
	   return false;
	   
	return true;
	
}

//获取特定交换机的所有满表值
bool CConvertConfig::getSwitchFullMeter(const string & sSectionCode,long & lFullScaleVar)
{	
	map<string,string>::iterator itr;	
	itr = m_mapFullScale.find( sSectionCode );
	if(itr == m_mapFullScale.end())
	{
		 return false;
	} 
	lFullScaleVar = atol( (*itr).second.c_str());
	return true;	
	
}

