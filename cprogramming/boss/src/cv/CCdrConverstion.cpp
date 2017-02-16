#include <assert.h>
#include "CCdrConverstion.h"

long g_lPhoneNo ;
CCdrConverstion::CCdrConverstion()
{
	m_dbPara.setFieldLength(200);
	 
	return ;
}

CCdrConverstion::~CCdrConverstion()
{	 
	m_cdrCon.disConnect();
	//m_dbBill.disconnect();
	return ;
}


CCdrConverstion* CCdrConverstion::stCdrCV_Instance = NULL;

CCdrConverstion* CCdrConverstion::Create()
{
	if(stCdrCV_Instance == NULL);
		stCdrCV_Instance = new CCdrConverstion;
	return stCdrCV_Instance;
	
}
 
bool CCdrConverstion::Init()
{
	//先连接清单数据库	
	string sUserName,sPassWord,sService;
	CConvertConfig::GetInstance()->GetDbLinkInfo_bill(sService,sUserName,sPassWord);
	/*if(m_dbBill.connect(sUserName.c_str(),sPassWord.c_str(),sService.c_str()))
	{
		//连接清单库失败
	//	printf("lianjie bill db failure\n");
		#ifdef _DEBUG_
			cout<<"dbname = "<<sService<< ", " <<"userName="<<sUserName<<" , passwd="<<sPassWord<<endl;
		  cout<<"lianjie bill db failure!!! "<<" "<<__FILE__<<__LINE__<<endl;
		#endif
		theErrorMessages->insert(CV_ERROR_DB_CON, " billDb connect fail:" );                   
		return false;
	}
	*/
	if(!m_cdrCon.connect(sUserName,sPassWord,sService,true))
	{
		//连接清单库失败
	//	printf("lianjie bill db failure\n");
		#ifdef _DEBUG_
			cout<<"dbname = "<<sService<< ", " <<"userName="<<sUserName<<" , passwd="<<sPassWord<<endl;
		  cout<<"m_cdrCon lianjie bill db failure!!! "<<" "<<__FILE__<<__LINE__<<endl;
		#endif
		theErrorMessages->insert(CV_ERROR_DB_CON, " billDb connect fail:" );                   
		return false ;
	}	
	
	m_pEnv = Environment::createEnvironment ( Environment::DEFAULT );
	m_pConn = m_pEnv->createConnection ( sUserName, sPassWord, sService );	 
	
//	char sSqlBuf[MAX_SIZE]={"\0"};  
//	strcpy(sSqlBuf,"insert into tg_lsmeter_cdr_count(call_number,count,rec_date,meter_type,cdr_type,month_scale,month,file_type,description ) \
//	        values( :PhoneNo ,TO_NUMBER(:CdrMeterCount),TO_NUMBER(:RecDate),TO_NUMBER(:MeterType),TO_NUMBER(:CdrType),TO_NUMBER(:MonthScale),:Month,:FileType,:Des)");
	
	
	CConvertConfig::GetInstance()->GetDbLinkInfo_para(sService,sUserName, sPassWord);
	if(!m_cdrConPara.connect(sUserName,sPassWord,sService,true))
	{
		//连接参数库失败	
		#ifdef _DEBUG_
			cout<<"dbname = "<<sService<< ", " <<"userName="<<sUserName<<" , passwd="<<sPassWord<<endl;
		  cout<<"m_cdrCon lianjie para db failure!!! "<<" "<<__FILE__<<__LINE__<<endl;
		#endif
		theErrorMessages->insert(CV_ERROR_DB_CON, " billDb connect fail:" );                   
		return false ;
	}  
	
	
	m_fstreamExcpt.open("excp.log",ios::out);
	
	GetAllNbr();
	
	//获取所有话单文件名中的营业区和库中营业区对应关系
	  char sql[2000]={"\0"};   
    strcpy(sql,"select meter_section,db_section  from td_section_meter2db ");

    string sTmpMeter,sTmpDb;
		//从未上移表中获取所有未上移号码		     
    try
    {	   	
   	  OCCISQL  occisql(&m_cdrConPara,sql);       	     
      occisql.executeQuery();            
      while (occisql.next())
		  {			  
			  sTmpMeter = occisql.getString(1);		
			  sTmpDb  = occisql.getString(2);  
			  m_vecSecFileToDb.insert(map<string, string> :: value_type(sTmpMeter, sTmpDb)) ;
		  }   
    }
    catch (SQLException & e)
    {
     cout<<"新老营业区对应关系表异常!"<<endl;
     cout<<e.getMessage()<<endl;
     string sTmpStr = sql;		     
		 #ifdef _DEBUG_
		   cout<<"exec get bill sql error: "<<sTmpStr<<" "<<__FILE__<<__LINE__<<endl;
		 #endif				 
     theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get table td_section_meter2db sql error:  " + sTmpStr );	
     return false ;
    }				
	
	//m_pConnPara = m_pEnv->createConnection ( sUserName, sPassWord, sService );	 
	return true;
}
//读取话单文件中的话单
bool CCdrConverstion::GetCDR()
{	
	int iBufLen = MAX_SIZE;	
	if(!m_fstream.eof())
	{   	
    m_fstream.getline(m_sCdrBuf,iBufLen,'\n');    
    if(strlen(m_sCdrBuf)==0) 
    	{
    		theErrorMessages->insert(CV_ERROR_CDR_NO,"nomore cdr!");
    		return false;
    	}
    else 
    return true;
  
  }
  else
    return false;
  
	return true;
} 



//校验话单的有效性
bool CCdrConverstion::ValidCDR(int iRuleChannelIndex)
{
		assert(strlen(m_sCdrBuf)>0);
		char sValidPara[256]={"\0"};
		strcpy(sValidPara,CConvertConfig::GetInstance()->GetSeparateToken(iRuleChannelIndex).c_str());
		assert(strlen(sValidPara)>0);
		int i;
		char *pTmpPos = NULL,*pTmpBuf = NULL;
		pTmpBuf = m_sCdrBuf;
		//根据校验话单分割符序列，逐个校验话单是否完全按分割符分割
		//printf("aaaaaaaaa =%s\n",sValidPara);
		//printf("bbbbbbbbb =%s\n",pTmpBuf);
		for(i=0;i<strlen(sValidPara);i++)
		{
			if( (pTmpPos=strchr(pTmpBuf,sValidPara[i])) == NULL)
				break;
			pTmpBuf = pTmpPos+1;				
		}
		if(i == strlen(sValidPara))
		{	
			int k;
			for (k =0 ;k<strlen(pTmpBuf);k++)
			{				
				if(pTmpBuf[k]	>'9' || pTmpBuf[k]<'0')
				{
					
					string sTmp = m_sCdrBuf;
					#ifdef _DEBUG_
					 cout<<"cdrformate error:: "<<sTmp<<" "<<__FILE__<<__LINE__<<endl;
					#endif
					theErrorMessages->insert(CV_ERROR_CDR_SIZE_ERROR, " cdrformate error:" + sTmp);
					return false;	
				}
			}
			return true;				
		}
		else
		{
			string sTmp =	m_sCdrBuf;	
			theErrorMessages->insert(CV_ERROR_CDR_SIZE_ERROR, " cdrformate error:" + sTmp);
			return false;
		}
		return true;
} 

 //处理净跳次的话单
bool CCdrConverstion::ProcessJingMeterCDR(const int& iRuleChannelIndex,const int& nCdrType)
{  
		
		
	  char sValidPara[256]={"\0"},sPhoneNo[50]={"\0"},sMeterBuf[20]={"\0"};
		strcpy(sValidPara,CConvertConfig::GetInstance()->GetSeparateToken(iRuleChannelIndex).c_str());
		int i;
		long lCdr_MeterCount;
		char *pTmpPos = NULL,*pTmpBuf = NULL;
		char sSqlBuf[MAX_SIZE]={"\0"};
		char cMeterType[2]={"\0"};
		int nTotal,nSuccess,nError;		
		char sOutBuf[1024];
		pTmpBuf = m_sCdrBuf;
		//根据校验话单分割符序列，首先取出电话号码		
		assert((pTmpPos=strchr(pTmpBuf,sValidPara[0])) != NULL);
		strncpy(sPhoneNo,pTmpBuf,pTmpPos-pTmpBuf);
		
		if(!isOutputCdr(sPhoneNo))
		{
			 m_fstreamError << m_sCdrBuf <<endl;
		 	 return true;
		}
	  
		pTmpBuf = pTmpPos + 1 ;
		
		int nMeter_seq = 0;
		int   nMeterCount = strlen(sValidPara) -1;
		//校验参数形如：，，，，
		//只取前个跳次，最后一个跳次忽略
		for(i=1;i<=strlen(sValidPara)-1;i++)
		{
			//取得话单中跳表跳次,该跳表的满刻度值
			if( strlen(sValidPara) == i ) //最后一个跳表，获取跳次
			{
					strcpy(sMeterBuf,pTmpBuf);				
			}
			else													//获取非最后一个跳表的跳次
			{
				pTmpPos=strchr(pTmpBuf,sValidPara[i]);
				memset(sMeterBuf,0x0,sizeof(sMeterBuf));
				strncpy(sMeterBuf,pTmpBuf,pTmpPos-pTmpBuf);
				pTmpBuf = pTmpPos+1;	
			}
			
			//取得净跳次
			lCdr_MeterCount = atol(sMeterBuf);			
			
			//取得跳次 的类型
		  switch (i)
			{
			case 1:
				cMeterType[0] = '0';            // 区内通话
				break;
			case 2:
				cMeterType[0] = '1';            // 区内通话
				break;
			case 3:
			  cMeterType[0] = '2';            // 上网
			  break;
			default:
			  cMeterType[0] = '3';
			  break;				
			}		
			
				//输出保存到输出文件中
			memset(sOutBuf,0x0,sizeof(sOutBuf));
			nMeter_seq ++;
			int nCdrSeq = m_cvLog.getCdrCount() * nMeterCount + nMeter_seq ;
			char sTmpBuf[MAX_SIZE]={"\0"};			
		//	cout<< "m_sFileName= " <<m_sFileName.c_str() <<endl;
			char *pTmpPos = strrchr((char *)m_sFileName.c_str(),'/');
		//	cout<< "pTmpPos= " <<pTmpPos <<endl;
			pTmpPos++;
			sprintf(sOutBuf,"75,15,,0,01,,%s,,,%ld,000000,,,%ld,JTC,,,,,,,,,,,,,,,,,,%s,,,,,,,",sPhoneNo,m_nActiveDate,lCdr_MeterCount,sPhoneNo);
			sprintf(sTmpBuf,"%c,,,,,,,,,,0,0,0,%d,%s,1,,,,,,,,,",cMeterType[0],nCdrSeq,pTmpPos);	
			
			//sprintf(sOutBuf,"77,11,,0,,,,,,%ld,000000,%ld,000000,%ld,TC022,,,,,,,,,,,,,,,,,,,,,,,,,",m_nActiveDate,m_nActiveDate,lCdr_MeterCount);
			//sprintf(sTmpBuf,"%c,,,,,,,,,,%ld,0,0,%d,%s,1,,,,,,,,,,",cMeterType[0],lCdr_MeterCount,nCdrSeq,pTmpPos);		  
			strcat(sOutBuf,sTmpBuf);		
			#ifdef _DEBUG
					printf("output buf :\n");
					printf(	sOutBuf);
					printf("\n");
			#endif
			m_fstreamOut<<sOutBuf<<endl;		
	
			//将当前跳表刻度保存到清单库中
		/*	sprintf(sSqlBuf,"insert into tg_lsmeter_cdr_count values( '%s' ,'%s' ,'%c',%d,%d,'测试跳表1','FFFF')",sPhoneNo,vtRecd[iRuleChannelIndex].sSwitchName,cMeterType[0],lCdr_MeterCount,m_nActiveDate);						
			printf(sSqlBuf);			
	   */ 
	    	
		}
		return true;
		
}

//	处理累计跳次的话单
bool CCdrConverstion::ProcessLeiJiMeterCDR(const int& iRuleChannelIndex,const int& nCdrType)
{
	  char sValidPara[256]={"\0"},sPhoneNo[50]={"\0"},sMeterBuf[20]={"\0"};
		strcpy(sValidPara,CConvertConfig::GetInstance()->GetSeparateToken(iRuleChannelIndex).c_str());
		
		int i,iFullMeter;
		long lCdr_MeterCount,lLastMonth_MeterCount,lMeterCount;
		char *pTmpPos = NULL,*pTmpBuf = NULL;
		char sSqlBuf[MAX_SIZE]={"\0"};
		char cMeterType[2]={"\0"};
		int nTotal,nSuccess,nError;		
		char sOutBuf[1024];
		StringVector  svParaRecordSet,svErrorSet;
		    
		//取得上月表底的年月，先从本月的开头最近一天算起		
		int nCycleMonth = m_nActiveDate /100 + 1;
		char sMonth[3]={"\0"};
		char sDateBuf[7]={"\0"};
		int  nMonth;
		bool bOutput = true; 
		sprintf(sDateBuf,"%d",m_nActiveDate);
		strncpy(sMonth,sDateBuf+4,2);
						  
		pTmpBuf = m_sCdrBuf;
		//根据校验话单分割符序列，首先取出电话号码		
		assert((pTmpPos=strchr(pTmpBuf,sValidPara[0])) != NULL);
		strncpy(sPhoneNo,pTmpBuf,pTmpPos-pTmpBuf);			
		bOutput	= isOutputCdr(sPhoneNo);
		pTmpBuf = pTmpPos + 1 ;		
		
		int nMeter_seq = 0; 		
		int   nMeterCount = strlen(sValidPara) -1;	
	  bool bNoLastScale = false; 
	  RECORD_METER_COUNT  record_Meter_Count;
	  string sTmpSectionCode_Num;
	  GetSectionCodeFromCallNum(sPhoneNo,sTmpSectionCode_Num);
	  string sTmpSectionCode = m_sIndbSectionCode +"%"; //m_sSectionCode_File,sTmpSectionCode_Num + "%" ;
		for(i=1;i<=strlen(sValidPara)-1;i++)     //只取话单中前三个跳次的值，最后一个无用
		{
			
			//获取非最后一个跳表的跳次	
			pTmpPos=strchr(pTmpBuf,sValidPara[i]);
			memset(sMeterBuf,0x0,sizeof(sMeterBuf));
			strncpy(sMeterBuf,pTmpBuf,pTmpPos-pTmpBuf);
			pTmpBuf = pTmpPos+1;				
			lCdr_MeterCount = atol(sMeterBuf);
			
			switch (i)
			{
			case 1:
				cMeterType[0] = '0';            // 区内通话
				break;
			case 2:
				cMeterType[0] = '1';            // 区内通话
				break;
			case 3:
			  cMeterType[0] = '2';            // 上网
			  break;
			default:
			  cMeterType[0] = '3';
			  break;				
			}					
			
			//若是查询不到相关记录未超出3个月内，并且还未能确认向下三个月也无记录。逐次向下查询
			int k=0;			 
			if( bNoLastScale) 
				k = 3;
				
			#ifdef FP_DEBUG
         FP_BEGIN(GETLASTSCALE)
      #endif	
      
      char item[512];
      char items[100][512];
      ub2  len[100];	
      char sql[2000]={"\0"};
      //rec_date >=:recStartdate1 and rec_date <=:recEnddate2  and
      strcpy(sql,"select MONTH_SCALE from tg_lsmeter_cdr_count where call_number =:call_number  and  meter_type =:type and month =:month and section_code like :V1 order by rec_date desc"); //
	     
      OCCISQL  occisql(&m_cdrCon,sql);
       
			while(k<3 && !bNoLastScale) 
			{
					
				 memset(sSqlBuf,0x0,sizeof(sSqlBuf));
				 long nTmpFirstDay = nCycleMonth*100 + 0;
				 long nTmpLastDay = nCycleMonth*100 +32;				   
			        
			  // occisql.setSQL(sql);
	       unsigned int iCount=0;	
	       //从清单库中获得上月跳表次数		     
	       try
	       {			       	
			      
			    //  occisql.setInt(2,nTmpFirstDay);
			    //  occisql.setInt(3,nTmpLastDay);
			      occisql.setString(1,sPhoneNo);
			      occisql.setString(2,cMeterType);
			      occisql.setString(3,sMonth);
			      
			      occisql.setString(4,sTmpSectionCode);//sTmpSectionCode_Num
			      occisql.executeQuery();
			      occisql.setResultDataBuffer(1,items,OCCI_SQLT_STR,sizeof(items[0]),len);  
			      occisql.next(100);
		     }
	       catch (SQLException & e)
	       {
		       cout<<"查询表底清单刻度异常!"<<endl;
			     cout<<e.getMessage()<<endl;
			     string sTmp = sSqlBuf;		      
					 #ifdef _DEBUG_
					   cout<<"exec get bill sql error: "<<sTmp<<" "<<__FILE__<<__LINE__<<endl;
					 #endif
					 string sTmpStr = sql;
		       theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get bill sql error:  " + sTmpStr );	
			     exit(0) ;
	      }		
					
		    if( occisql.getNumArrayRows()>0)  //取出上月表底刻度，退出		    	
		    {		 		    	   	
		    	  break;
		    }
		    else
		    {
		    	#ifdef _DEBUG_
		    	  char sErrMsg[100]={"\0"};
		    	 	sprintf(sErrMsg,"callNumber =%s,month %s scale not exist",sPhoneNo,sMonth);
						cout<<sErrMsg<<__FILE__<<__LINE__<<endl;
					#endif  
						
		    	k++;				     
			//    sprintf(sDateBuf,"%d",nCycleMonth);
			//		strncpy(sMonth,sDateBuf+4,2);
					
					nMonth = atoi(sMonth);
					//若当前账期是1月份，则上次账期为去年的12月份
					if(1 == nMonth)     
					{
						int nTmpYear;
						char sTmpYear[5]={"\0"};
						char sTmpDate[7]={"\0"};			
						strncpy(sTmpYear,sDateBuf,4);
						nTmpYear = atoi(sTmpYear);
						nTmpYear = nTmpYear -1;
						sprintf(sTmpDate,"%d12",nTmpYear);
						nCycleMonth = atoi(sTmpDate);
						
						memset(sMonth,0x0,sizeof(sMonth));
						strcpy(sMonth,"12");
					}
					else
					{
						nCycleMonth = nCycleMonth-1 ;
						
						nMonth -- ;						
						sprintf(sMonth,"%02d",nMonth);
					}	    	
			    continue;		    	
		    }
	    }
	    if(k<3 )
	    {	    	 
	      int j = 0;
	      #ifdef _DEBUG_
	      		printf("发现记录存在\n");
	      #endif	      
			  lLastMonth_MeterCount = atol(items[0]);
			  
		    //	printf("last month meter_count=%d\n",lLastMonth_MeterCount);
		  }
		  else   //若往上三个月都没该话单的跳次记录，则将上次表底刻度置为0
		  {
		  	#ifdef _DEBUG_
		  			printf("无记录存在\n");
		  	#endif
		  	lLastMonth_MeterCount = 0; 
		  	bNoLastScale = true;
		  }		  
		  
			//计算求得净跳次数			
			if(lCdr_MeterCount >= lLastMonth_MeterCount)
				lMeterCount = lCdr_MeterCount-lLastMonth_MeterCount;
			else   //本次话单中跳次 < 上次跳次 的异常处理
			{
				#ifdef _DEBUG_
						char sErrBuf[256]={"\0"};
						sprintf(sErrBuf,"cdr_type =%d ,meter_type=%c,phoneNo =%s,now meter =%ld,last meter=%ld",nCdrType,cMeterType[0],sPhoneNo,lCdr_MeterCount,lLastMonth_MeterCount);
					  cout<<"current meter less than last meter: "<<sErrBuf<<" "<<__FILE__<<__LINE__<<endl;
					  theErrorMessages->insert(CV_ERROR_LEJMTER_LESS_LASTVALUE,"leiji meter less than last meter value  "  );	
					  					  
				#endif	
			 m_fstreamExcpt	<<"exception callNumber:"<<m_sCdrBuf<<"  in file:"<<m_sFileName<<endl;
			 
			 //获取该号码满刻度
			 long lFullScale;
			 CConvertConfig::GetInstance()->getSwitchFullMeter(m_sSectionCode_File,lFullScale);
			 lMeterCount = lFullScale -lLastMonth_MeterCount + lCdr_MeterCount;
			// m_vecRecMeterCount.clear();
			// return false;				
			}
			
			//printf("current month meter count =%d\n",lMeterCount);
			
			 #ifdef FP_DEBUG
            FP_END(GETLASTSCALE)
       #endif
       
			//输出保存到输出文件中
			#ifdef FP_DEBUG
          FP_BEGIN(OutPutMeterValue)
      #endif
      
			memset(sOutBuf,0x0,sizeof(sOutBuf));
			nMeter_seq ++ ;
		  int nCdrSeq = m_cvLog.getCdrCount()*nMeterCount + nMeter_seq;					
		  
		  char sTmpBuf[MAX_SIZE]={"\0"};			
		//	cout<< "m_sFileName= " <<m_sFileName.c_str() <<endl;
			char *pTmpPos = strrchr((char *)m_sFileName.c_str(),'/');
		//	cout<< "pTmpPos= " <<pTmpPos <<endl;
			pTmpPos++;		
			sprintf(sOutBuf,"75,15,,0,01,,%s,,,%ld,000000,,,%ld,TC022,,,,,,,,,,,,,,,,,,%s,,,,,,,",sPhoneNo,m_nActiveDate,lCdr_MeterCount,sPhoneNo);		
			sprintf(sOutBuf,"75,15,,0,01,,%s,,,%ld,000000,,,%ld,TC022,%ld,%ld,,,,,,,,,,,,,,,,%s,,,,,,,",sPhoneNo,m_nActiveDate,lMeterCount,lCdr_MeterCount,lLastMonth_MeterCount,sPhoneNo);
			sprintf(sTmpBuf,"%c,,,,,,,,,,0,0,0,%d,%s,1,,,,,,,,,",cMeterType[0],nCdrSeq,pTmpPos);			
			strcat(sOutBuf,sTmpBuf);
					  
		/*	#ifdef _DEBUG_
					cout<<"output buf : \n"<<sOutBuf <<endl;					
			#endif
		*/	
		
		//话单文件的营业区号和号码所属的 营业区号一致，则输出，否则不输出。只做记录
		if( bOutput)
			 m_fstreamOut<<sOutBuf<<endl;			
		else
		{
			cout<<"call number sectionCode not equal file sectionCode:"<<endl;
			
		} 
		 	
			#ifdef FP_DEBUG
         FP_END(OutPutMeterValue)
      #endif
      
			//将当前跳表刻度保存到清单库中
			#ifdef FP_DEBUG
          FP_BEGIN(SaveToDB)
      #endif      
      
			char sTmpMonth[3]={"\0"};
			char sTmpMonthBuf[20]={"\0"};
			sprintf(sTmpMonthBuf,"%d",m_nActiveDate);
			strncpy(sTmpMonth,sTmpMonthBuf+4,2);
		  //	sprintf(sSqlBuf,"insert into tg_lsmeter_cdr_count values( '%s' ,%ld,'%s' ,'%c',%d,%ld,'%s','','FFFF')",sPhoneNo,lCdr_MeterCount,m_CdrFilesDate.c_str(),cMeterType[0],nCdrType,lCdr_MeterCount,sTmpMonth);										
		 // sprintf(sSqlBuf,"insert into tg_lsmeter_cdr_count values( :PhoneNo ,:CdrMeterCount,:RecDate,:MeterType,:CdrType,:MonthScale,:Month,:FileType,:Des)",sPhoneNo,lCdr_MeterCount,m_CdrFilesDate.c_str(),cMeterType[0],nCdrType,lCdr_MeterCount,sTmpMonth);										
		 //获得当前记录值
		 
		 record_Meter_Count.sCallNumber = sPhoneNo;
     record_Meter_Count.lCdrCount = lCdr_MeterCount;
     record_Meter_Count.sRecDate = m_CdrFilesDate;
     memset(record_Meter_Count.cMeterType,0x0,sizeof(record_Meter_Count.cMeterType));
     record_Meter_Count.cMeterType[0] = cMeterType[0];     
     char sTmpCdrType[2];
     sprintf(sTmpCdrType,"%d",nCdrType);
     memset(record_Meter_Count.nCdrType,0x0,sizeof(record_Meter_Count.nCdrType));
     record_Meter_Count.nCdrType[0] = sTmpCdrType[0];     
     if('2' == record_Meter_Count.nCdrType[0] )                    //累计跳次，保留当前刻度值作为下次计算的表底
       record_Meter_Count.lMonthScale = lCdr_MeterCount;
     else if( '3' ==record_Meter_Count.nCdrType[0] )
       record_Meter_Count.lMonthScale = 0;                    //反转跳次，将表底设置为0，下次计算跳次，以0刻度作为表底
     record_Meter_Count.sMonth = sTmpMonth;
     strcpy(record_Meter_Count.cFileType ,"");     
     record_Meter_Count.sDes	= "FFFF";	 
     
     m_vecRecMeterCount.push_back(record_Meter_Count);         
	/*	 try
      {			       	
	     //static OCCISQL tmpOccisql(&m_cdrCon,sSqlBuf);
	      static int stTmpCount = 0;	      
	      stTmpCount ++ ;
	      m_pOccisql->setString(1,sPhoneNo);
			  m_pOccisql->setString(2,lCdr_MeterCount);
			  m_pOccisql->setString(3,m_CdrFilesDate);
			  m_pOccisql->setString(4,cMeterType);
			  m_pOccisql->setString(5,nCdrType);
			  m_pOccisql->setString(6,lCdr_MeterCount);
			  m_pOccisql->setString(7,sTmpMonth);
			  m_pOccisql->setString(8,"");
			  m_pOccisql->setString(9,"FFFF");
	      
	      pOccisql->executeUpdate();
			  pOccisql->commit();
			  exit(0);
	      if(stTmpCount == 100)
	      {
	      	pOccisql->executeUpdate();
			    pOccisql->commit();
			    stTmpCount = 0;
	      }
			  	
      }
      catch (SQLException & e)
      {
        cout<<"插入表底清单刻度异常!"<<endl;
	      cout<<e.getMessage()<<endl;
	      string sTmp = sSqlBuf;		      
			  #ifdef _DEBUG_
			   cout<<"exec insert bill sql error: "<<sSqlBuf<<" "<<__FILE__<<__LINE__<<endl;
			  #endif
			  string sTmpStr = sql;
        theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get bill sql error:  " + sTmpStr );	
	      exit(0) ;
      }		
			*/	 
			#ifdef FP_DEBUG
        FP_END(SaveToDB)
      #endif
      	
		} //end of for(i=1;i<=strlen(sValidPara)-1;i++)
	return true;
}

//处理翻转跳次的话单
bool CCdrConverstion::ProcessFanZhuanMeterCDR(const int& iRuleChannelIndex,const int& nCdrType)
{
	  char sValidPara[256]={"\0"},sPhoneNo[50]={"\0"},sMeterBuf[20]={"\0"};
		strcpy(sValidPara,CConvertConfig::GetInstance()->GetSeparateToken(iRuleChannelIndex).c_str());
		
		int i,iFullMeter;
		long lCdr_MeterCount,lLastMonth_MeterCount,lMeterCount;
		char *pTmpPos = NULL,*pTmpBuf = NULL;
		char sSqlBuf[MAX_SIZE]={"\0"};
		char cMeterType[2]={"\0"};
		int nTotal,nSuccess,nError;		
		char sOutBuf[1024];
		StringVector  svParaRecordSet,svErrorSet;
		
    //取得活动账期表底的年月，先从本月的开头最近一天算起    
		int nCycleMonth = m_nActiveDate /100;		
		char sMonth[3]={"\0"};
		char sDateBuf[7]={"\0"};
		int  nMonth;
		
		//根据校验话单分割符序列，首先取出电话号码		
		pTmpBuf = m_sCdrBuf;			
		assert((pTmpPos=strchr(pTmpBuf,sValidPara[0])) != NULL);
		strncpy(sPhoneNo,pTmpBuf,pTmpPos-pTmpBuf);
				
		pTmpBuf = pTmpPos + 1 ;		
		
		int nMeter_seq = 0;              
		int   nMeterCount = strlen(sValidPara) -1;
		
		 bool bNoLastScale = false;
		for(i=1;i<=strlen(sValidPara)-1;i++)     //只取话单中前三个跳次的值，最后一个无用
		{
			//取得话单中跳表跳次
		
			//获取非最后一个跳表的跳次	
			pTmpPos=strchr(pTmpBuf,sValidPara[i]);
			memset(sMeterBuf,0x0,sizeof(sMeterBuf));
			strncpy(sMeterBuf,pTmpBuf,pTmpPos-pTmpBuf);
			pTmpBuf = pTmpPos+1;	
			
			lCdr_MeterCount = atol(sMeterBuf);
			
			switch (i)
			{
			case 1:
				cMeterType[0] = '0';            // 区内通话
				break;
			case 2:
				cMeterType[0] = '1';            // 区内通话
				break;
			case 3:
			  cMeterType[0] = '2';            // 上网
			  break;
			default:
			  cMeterType[0] = '3';
			  break;				
			}					
			
			//若是查询不到相关记录未超出3个月内，并且还未能确认向下三个月也无记录。逐次向下查询
			int k=0;			 
			if( bNoLastScale) 
				k = 3;
			
			#ifdef FP_DEBUG
         FP_BEGIN(GETLASTSCALE)
      #endif	
			int nDate  = nCycleMonth ;	
			char item[512];
      char items[100][512];
      ub2  len[100];	
      char sql[2000]={"\0"};
      strcpy(sql,"select MONTH_SCALE from tg_lsmeter_cdr_count where call_number =:call_number  and rec_date >=:recStartdate1 and rec_date <=:recEnddate2 and meter_type =:type");
	     
      OCCISQL  occisql(&m_cdrCon,sql); 				  
			while(k<3)
			{
					
				memset(sSqlBuf,0x0,sizeof(sSqlBuf));
				long nTmpFirstDay = nCycleMonth*100 + 0;
				long nTmpLastDay = nCycleMonth*100 +32;
				 
				//从清单库中获得上月跳表次数				
			  try
	       {			       	
			      occisql.setString(1,sPhoneNo);
			      occisql.setInt(2,nTmpFirstDay);
			      occisql.setInt(3,nTmpLastDay);
			      occisql.setString(4,cMeterType);
			      occisql.executeQuery();
			      occisql.setResultDataBuffer(1,items,OCCI_SQLT_STR,sizeof(items[0]),len);  
			      occisql.next(100);
		     }
	       catch (SQLException & e)
	       {
		       cout<<"查询表底清单刻度异常!"<<endl;
			     cout<<e.getMessage()<<endl;
			     string sTmp = sSqlBuf;		      
					 #ifdef _DEBUG_
					   cout<<"exec get bill sql error: "<<sTmp<<" "<<__FILE__<<__LINE__<<endl;
					 #endif
					 string sTmpStr = sql;
		       theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get bill sql error:  " + sTmpStr );	
			     exit(0) ;
	      }		
					
		    if( occisql.getNumArrayRows()>0)  //取出上月表底刻度，退出		    	
		    {		 		    	   	
		    	  break;
		    }      	
		    
		    else
		    {
		    	#ifdef _DEBUG_
		    	  char sErrMsg[100]={"\0"};
		    	 	sprintf(sErrMsg,"callNumber =%s,%d scale not exist",sPhoneNo,nCycleMonth);
						cout<<sErrMsg<<__FILE__<<__LINE__<<endl;
					#endif  
						
		    	k++;				     
			    sprintf(sDateBuf,"%d",nCycleMonth);
					strncpy(sMonth,sDateBuf+4,2);
					nMonth = atoi(sMonth);
					//若当前账期是1月份，则上次账期为去年的12月份
					if(1 == nMonth)     
					{
						int nTmpYear;
						char sTmpYear[5]={"\0"};
						char sTmpDate[7]={"\0"};			
						strncpy(sTmpYear,sDateBuf,4);
						nTmpYear = atoi(sTmpYear);
						nTmpYear = nTmpYear -1;
						sprintf(sTmpDate,"%d12",nTmpYear);
						nCycleMonth = atoi(sTmpDate);
					}
					else
					{
						nCycleMonth = nCycleMonth-1 ;
					}	    	      	
			    continue;		    	
		    }
	   }
	    if(k<3 )
	    {	    	 
	      int j = 0;
	      #ifdef _DEBUG_
	      	cout<<"发现记录存在"<<endl;
	      #endif	     
			  lLastMonth_MeterCount = atoi(items[0]); 
		  }
		  else   //若往上三个月都没改话单的跳次记录，则将上次表底刻度置为0
		  {
		  	#ifdef _DEBUG_
		  		cout<<"无记录存在"<<endl;
		  	#endif
		  	lLastMonth_MeterCount = 0;
		  	bNoLastScale = true;
		  }
		  
		  #ifdef _DEBUG_
		  	printf("cccccccccccccccccccccccccccccccccccc\n");
		  #endif
			//计算求得净跳次数			
			if(lCdr_MeterCount >= lLastMonth_MeterCount)
				lMeterCount = lCdr_MeterCount-lLastMonth_MeterCount;
			else 
			{
				#ifdef _DEBUG_
						char sErrBuf[256]={"\0"};
						sprintf(sErrBuf,"cdr_type =%d ,meter_type=%c,phoneNo =%s,now meter =%ld,last meter=%ld",nCdrType,cMeterType[0],sPhoneNo,lCdr_MeterCount,lLastMonth_MeterCount);
					  cout<<"current meter less than last meter: "<<sErrBuf<<" "<<__FILE__<<__LINE__<<endl;
				#endif		
				
			}
			
			 #ifdef FP_DEBUG
            FP_END(GETLASTSCALE)
       #endif
 			//printf("current month meter count =%ld\n",lMeterCount);
			
			//输出保存到输出文件中
			#ifdef FP_DEBUG
          FP_BEGIN(OutPutMeterValue)
      #endif
			memset(sOutBuf,0x0,sizeof(sOutBuf));
			nMeter_seq ++ ;
		  int nCdrSeq = m_cvLog.getCdrCount()*nMeterCount + nMeter_seq;		
		  
		  char sTmpBuf[MAX_SIZE]={"\0"};			
		//	cout<< "m_sFileName= " <<m_sFileName.c_str() <<endl;
			char *pTmpPos = strrchr((char *)m_sFileName.c_str(),'/');
		//	cout<< "pTmpPos= " <<pTmpPos <<endl;
			pTmpPos++;			
			sprintf(sOutBuf,"75,15,,0,,,%s,,,%ld,000000,%ld,000000,%ld,TC022,%ld,%ld,,,,,,,,,,,,,,,,%s,,,,,,,",sPhoneNo,m_nActiveDate,m_nActiveDate,lMeterCount,lCdr_MeterCount,lLastMonth_MeterCount,sPhoneNo);
			sprintf(sTmpBuf,"%c,,,,,,,,,,0,0,0,%d,%s,1,,,,,,,,,",cMeterType[0],nCdrSeq,pTmpPos);					
			strcat(sOutBuf,sTmpBuf);
		
			#ifdef _DEBUG_
				printf("output buf :\n");
				printf(	sOutBuf);
				printf("\n");
			#endif
			m_fstreamOut<<sOutBuf<<endl;		
			
			#ifdef FP_DEBUG
         FP_END(OutPutMeterValue)
      #endif
      
			//将当前跳表刻度保存到清单库中
			#ifdef FP_DEBUG
          FP_BEGIN(SaveToDB)
      #endif
			char sTmpMonth[3]={"\0"};
			strcpy(sTmpMonth,m_CdrFilesDate.substr(4,2).c_str());
			sprintf(sSqlBuf,"insert into tg_lsmeter_cdr_count values( '%s' ,%ld,'%s' ,'%c',%d,0,'%s','','FFFF')",sPhoneNo,lCdr_MeterCount,m_CdrFilesDate.c_str(),cMeterType[0],nCdrType,sTmpMonth);						
		
			try
      {			       	
	      OCCISQL tmpOccisql(&m_cdrCon,sSqlBuf);
			  tmpOccisql.executeUpdate();
			  tmpOccisql.commit();	
      }
      catch (SQLException & e)
      {
        cout<<"插入表底清单刻度异常!"<<endl;
	      cout<<e.getMessage()<<endl;
	      string sTmp = sSqlBuf;		      
			  #ifdef _DEBUG_
			    cout<<"exec insert bill sql error: "<<sSqlBuf<<" "<<__FILE__<<__LINE__<<endl;
			  #endif
			  string sTmpStr = sql;
        theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec get bill sql error:  " + sTmpStr );	
	      exit(0) ;
     }		
			/*
			if(m_dbBill.executeSql(sSqlBuf,	nTotal,nSuccess,nError))
			{
		      //插入清单记录失败
		      string sTmp = sSqlBuf;
		      #ifdef _DEBUG_
					  cout<<"exec insert bill sql error: "<<sTmp<<" "<<__FILE__<<__LINE__<<endl;	
					  printf("insert bill meter_count fail： nErrorCode =%d\n",nError);
					#endif
		      theErrorMessages->insert(CV_ERROR_DB_EXEC,"exec insert bill sql error:  '" + sTmp );	
		      exit(0);	     
		      continue;	    
			}			
			m_dbBill.commitTransaction();
			svParaRecordSet.clear();
		  svErrorSet.clear();
		  */
			#ifdef FP_DEBUG
        FP_END(SaveToDB)
      #endif
		}
	return true;
}
		
//处理跳表话单 ,nCdrType为该话单对应文件类型，iRuleChannelIndex为话单所属通道
bool CCdrConverstion::ProcessMeterCDR(const int &iRuleChannelIndex,const int &nCdrType)
{
	
	//	int nCdrType = CConvertConfig::GetInstance()->GetCdrType(iRuleChannelIndex);
		
		switch(nCdrType)			
		{
			case JINT_METER:
						//printf("proces Jing cdr\n");
						ProcessJingMeterCDR(iRuleChannelIndex,nCdrType);
						break;
			case LEIJI_METER:
					//	#ifdef FP_DEBUG
               FP_BEGIN(ProcessLeiJiMeterCDR)
           // #endif
						ProcessLeiJiMeterCDR(iRuleChannelIndex,nCdrType);
					//	#ifdef FP_DEBUG
               FP_END(ProcessLeiJiMeterCDR)
           // #endif
						break;
			case FANZHUAN_METER:
			      ProcessLeiJiMeterCDR(iRuleChannelIndex,nCdrType);
						//ProcessFanZhuanMeterCDR(iRuleChannelIndex,nCdrType);
						break;
			default:
						break;
		}
		
		
		return true;
		
	  
	return true;
}	

//获取该类的静态实例
CCdrConverstion* CCdrConverstion::getInstance()
{
	if(stCdrCV_Instance!= NULL)
		return stCdrCV_Instance;
	else
		return NULL;
} 

//处理主函数
bool CCdrConverstion::process()
{
	CCdrBase::process();	
//	printf("DerivedClass Process\n");
	return true;
}

//打开话单源文件后，批处理话单中的文件。
bool CCdrConverstion::batchProcessCdr( int iChannelIndex,int nCdrType)
{
	
	bool bRet = false; ;
	string  sSqlBuf = "insert into tg_lsmeter_cdr_count(call_number,count,rec_date,meter_type,cdr_type,month_scale,month,file_type,description,section_code ) \
	                  values( :PhoneNo ,TO_NUMBER(:CdrMeterCount),:RecDate,:MeterType,:CdrType,TO_NUMBER(:MonthScale),:Month,:FileType,:Des,:SectionCode)";
	
  
	//从话单文件种读取一条话单
	int i =0;	
	long  lCount = 0;
	static int  stCount = 0;
	char sTmpCdrCount[50]={"\0"},sTmpMonthScale[50]={"\0"};
	
	//循环读取文件中每条话单进行处理
	while(true)
	{
		FP_BEGIN(GetCDR)
		   if(!GetCDR()) 
			   break;		
     FP_END(GetCDR)  
    
		//校验话单有效性
		FP_BEGIN(ValidCDR)
		 bool bTmpValidCDR = CConvertConfig::GetInstance()->IsCheckCDR(iChannelIndex);
		 if(bTmpValidCDR)
		 {
			 if(!ValidCDR(iChannelIndex))
			 {
				 //错误话单，跳过不处理
				 //printf("cdr invalid :%s\n",m_sCdrBuf);
				 string sTmp = m_sCdrBuf;
				 #ifdef _DEBUG_
			      cout<<"cdr invalid : "<<" "<<sTmp<<" "<<__FILE__<<__LINE__<<endl;
			   #endif
				 theErrorMessages->insert(CV_ERROR_CDR, "cdr error :" + sTmp);    
				 m_fstreamError << m_sCdrBuf <<endl;  
			 	continue;
			 }	
     }
		FP_END(ValidCDR)  
		
		//该话单若是跳过，则不处理 。不在未上移话单表中的跳过不处理
		if(isPassCurrentCDREx(iChannelIndex)) //isPassCurrentCDR
		{
			//不处理的话单输出到无效文件中
			 m_fstreamError << m_sCdrBuf <<endl; 
		   continue;
		}
		   
		//****************处理话单*******************************//
		//若话单呼叫号码所属的实际营业区，和话单文件名所指示的营业区不一致，则跳过，不处理，只入库
		FP_BEGIN(ProcessMeterCDR)		 
		  char *pTmpBuf = m_sCdrBuf;
		  char *pTmpPos = NULL;
		  char sPhoneNo[20]={"\0"};
		  char sValidPara[20]={"\0"};
		 //根据校验话单分割符序列，首先取出电话号码	
		  strcpy(sValidPara,CConvertConfig::GetInstance()->GetSeparateToken(iChannelIndex).c_str());	
	  	assert((pTmpPos=strchr(pTmpBuf,sValidPara[0])) != NULL);
	  	strncpy(sPhoneNo,pTmpBuf,pTmpPos-pTmpBuf);	
	  	//取出该号码对应的营业区
	  	string sTmpSectionCode_Num; 
	  	string sTmpPhoneNo = sPhoneNo;
	  	GetSectionCodeFromCallNum(sTmpPhoneNo,sTmpSectionCode_Num);
	  	m_sIndbSectionCode = m_sSectionCode_File;
		//  if( sTmpSectionCode_Num == m_sSectionCode_File)//sTmpSectionCode_Num == m_sSectionCode_Real
		  {	
		     m_vecRecMeterCount.clear();
		     ProcessMeterCDR(iChannelIndex,nCdrType);		
		     //若是净跳次，则不再做入库处理
		     if( nCdrType == JINT_METER)
		       continue;
		  }
		FP_END(ProcessMeterCDR)
		
		//*********************入库保存表底*********************//
	   try
     {  
     	FP_BEGIN(ADD_INSERTED_RECORD)  
     	  vector<RECORD_METER_COUNT>::iterator itrRec;     
     	 
     	  StatementVar  startMent(m_pConn ,sSqlBuf);  
        startMent->setMaxIterations(BATCH_INSERT_COUNT * 3 + 2 );	
        startMent->setMaxParamSize(1,32);
      	startMent->setMaxParamSize(2,20);
      	startMent->setMaxParamSize(3,8);
      	startMent->setMaxParamSize(4,1);
	      startMent->setMaxParamSize(5,1);
	      startMent->setMaxParamSize(6,20);
	      startMent->setMaxParamSize(7,2);
	      startMent->setMaxParamSize(8,1);
	      startMent->setMaxParamSize(9,32);	
	      startMent->setMaxParamSize(10,10);
		  
     	  for( itrRec=m_vecRecMeterCount.beallNum(sTmpPhoneNo,sTmpSectionCode_Num);
	  	m_sIndbSectionCode = m_sSectionCode_File;
		//  if( sTmpSectionCode_Num == m_sSectionCode_File)//sTmpSectionCode_Num == m_sSectionCode_Real
		  {	
		     m_vecRecMeterCount.clear();
		     ProcessMeterCDR(iChannelIndex,nCdrType);		
		     //鑻ユ槸鍑