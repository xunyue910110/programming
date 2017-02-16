//#include "PM_Account.h"
#include "PM_MonthAccount.h"
#include "MdbParamManager.h"
#include "pmbilling/Mdb_UserImportInfo.h"
#include "pmbilling/Mdb_UserInfo.h"
//#ifdef  __PROV022__
#include "pmbilling/Mdb_UserParam.h" //add by fug 2009-11-20
//#endif
#include <strstream>


//##ModelId=477DE75D006F
PM_MonthAccount::PM_MonthAccount()
{
	m_altiDbi             = abdb::AltibaseIntf::getInstance();
	m_nTotal              = 0;
	m_nSuccess            = 0;
	m_nError              = 0;	
	m_altibaseConnected   = false;
	m_pMonthEndOutputFile = NULL;
	m_feeSum              = 0;
  	m_lMaxUserNum         = 0;
  	m_zeroDetailBillIntoIntDbTag = '9';
  	m_divisor							= 10;//add for round 2009-9-9 13:53:13
  
  //add 2009-11-28 21:11:00
  	m_unAcctNetTypeCodes.clear();
	m_unAcctNetFee = 0;
	m_unAcctOtherFee = 0;
	m_unAcctRecords  = 0;	
	
	m_pMdbManager = new MdbParamManager();  

}

//##ModelId=477DE75D0070
PM_MonthAccount::~PM_MonthAccount()
{
	m_specialAcctIdVec.clear();
	delete m_pMdbManager;
}

//##ModelId=477DE75D0071
bool PM_MonthAccount::initialization()
{
	char tmpbuf[MAXLONGBUFFLEN+1];
	 if (reconnect())
  {  	
    sprintf(tmpbuf,"Error(%s:%d):Altibase database connect failed,reason:%s",
            __FILE__,__LINE__,m_msgbuf);

    THROW(DAOException, tmpbuf) ;
  }
  
  //设置通用资料参数管理类基本信息
    setDbConnInfo(m_userName, m_passWord, m_servName);

    setHostName(m_hostName.c_str());
    setAppName(m_appName);
    setUpdateInterval(updateIntervalTime);
    setChannelNo(m_channelNo);
    
    //如果是天津
//		if ()        
//    	getSpecialAcctId();

		m_divisor = PRECISION_FACTOR;//add for round 2009-9-9 13:53:13
    
    bool ret= ParamManager::initialization();
    if(!ret)
    {
      sprintf(tmpbuf,"Error(%s:%d):get param error,reason:%s",
            __FILE__,__LINE__,m_errorMessage.c_str());
    	#ifdef _DEBUG_
    	cout <<tmpbuf<< endl;
    	#endif   	
    	THROW(DAOException, tmpbuf) ;	
    	return ret;
    }	
    
    //add for round 2009-9-9 13:53:13	
   	if(!getDetailItem(m_detailItem,m_existFlag,m_carryModes))
		{
			sprintf(tmpbuf,"Error(%s:%d):get DetailItemInfo error,reason:%s",
            __FILE__,__LINE__,m_msgbuf);

    	THROW(DAOException, tmpbuf) ;	
		}	
		 	
  return true;
}

//##ModelId=477DE75D0078
bool PM_MonthAccount::end()
{
	//断开Altibase数据库连接
  if (m_altibaseConnected)
    m_altiDbi->disConnect();
  m_altibaseConnected=false;
    
  if(m_crmConnected)
  {
    m_crmConnected = false;
    if (m_crmDbi.disconnect() != 0)
    {
     THROW(DAOException, "数据库断开错误!");
    }
  }
 
	if(!m_pMdbManager->end())
  {
    return false;
  } 
		//自身清理工作
	if ( !ParamManager::end() )
	{
		return false;
	}
	
	return true;
	//通用资料参数管理类清理工作
	//return (*m_accountParam).end();
		
}

bool PM_MonthAccount::setMdbInfo(const MdbInfo &r_mdbInfo)
{
  if(!m_pMdbManager->initialization(r_mdbInfo))
  {
    return false;
  }
  return true;
}


//##ModelId=477DE75D007B
bool PM_MonthAccount::addParamObject()
{    
	  ParamContainerBase *paramcontainer;

//    paramcontainer = &m_PO_CycleTimeContainer;
//    m_paramContainerList.push_back(paramcontainer);

    paramcontainer = &m_PO_SysParamInfo;
    m_paramContainerList.push_back(paramcontainer); 
    
    paramcontainer = &m_PO_DetailItem;
    m_paramContainerList.push_back(paramcontainer);
    
    paramcontainer = &m_PO_CycleTimeEparchy;
    m_paramContainerList.push_back(paramcontainer);

//modified by chenyong 2009-10-25--begin 
    paramcontainer = &m_PO_FreeInfoDef;
    m_paramContainerList.push_back(paramcontainer);
//modified by chenyong 2009-10-25--end
  
    return true;
}

//根据PO文件中的配置，从数据库或文件取参数或资料数据
//##ModelId=477DE75D007D
bool PM_MonthAccount::getData()
{
    if (!ParamManager::getData())
    {
        return false;
    }

    return true;
}


//##ModelId=477DE75D00A0
/**
 * getMinRealCycId:取最小实时帐期信息
 * @param  minRealCycleId  最小实时帐期      .
 * @return  false 表示异常，true 表示正常
*/
bool PM_MonthAccount::getMinRealCycId(const int dbNo, int &r_minRealCycleId)
{
	int l_cycId = 205012;
	T_DATE_TIME l_beginTime;
	T_DATE_TIME l_endTime;
		
  vector<PO_CycleEparchy>              vPO_CycleTime;
  vector<PO_CycleEparchy>::iterator    iter;

  m_PO_CycleTimeEparchy.getParamVector(vPO_CycleTime);

  for (iter=vPO_CycleTime.begin(); iter!=vPO_CycleTime.end(); ++iter)
  {
	if(dbNo==iter->m_record.m_dbNo)//如果是本分区的数据
    {
      // 实时帐期的 useTag 为 '0'
      if ((iter->m_record.m_useTag == "0")  &&
          (iter->m_record.m_cycleId < l_cycId))
      {
          l_cycId = iter->m_record.m_cycleId;
          l_beginTime = iter->m_record.m_beginTime;
          l_endTime = iter->m_record.m_endTime;  
          //added by pangx on 2010-3-29 begin
          m_beginTime=iter->m_record.m_beginTime;
          m_endTime=iter->m_record.m_endTime;
          //added by pangx on 2010-3-29 end        
      }
    }
  }
  
  	m_minRealCycId   = l_cycId;
  	r_minRealCycleId = l_cycId;
  	
  	m_cycBeginTime   = l_beginTime;
  	m_cycEndTime   = l_endTime;  	
  
    if (l_cycId == 205012)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//##ModelId=477DE75D00A2
bool PM_MonthAccount::getSysParamInfo(const string& sysParamCode, vector<SysParamInfo>& sysParamInfoVector)
{
	  PO_SysParamInfo                        po;
    vector<PO_SysParamInfo>                rposVector;
    vector<PO_SysParamInfo>::iterator      iter;

    sysParamInfoVector.clear();

    po.m_record.m_sysParamCode=sysParamCode;
    
    m_PO_SysParamInfo.searchMulti(po,rposVector);

    for (iter=rposVector.begin(); iter!=rposVector.end(); iter++)
    {
        if (iter->m_record.m_useTag == '1')
        {
            sysParamInfoVector.push_back(iter->m_record);
        }
    }
	 
	 return (!sysParamInfoVector.empty());
}


//##ModelId=477DE75D0136
bool PM_MonthAccount::getLocalProvinceCode(string& r_provinceCode)
{ 
	  vector<SysParamInfo>            sysParamInfoVector;

    //获取当前的省份编码
    if (!getSysParamInfo("AS_ASP_CurrProvinceCode", sysParamInfoVector) )
    {
        return false;
    }

  	vector<SysParamInfo>::iterator  iter;
    for (iter = sysParamInfoVector.begin();
         iter != sysParamInfoVector.end(); ++iter)
    {
			r_provinceCode = iter->m_valueS;
			break;
    }
    
		m_provinceCode = r_provinceCode;
    
    return true;
	 	 
}

void PM_MonthAccount::getZeroDetailBillIntoDBTag( )
{
		vector<SysParamInfo>    sysParamInfoVector;
    vector<SysParamInfo>::iterator  iter;
    	
    //获取零帐单入综合账单库标志 0:零帐单不入库 1:零帐单入库
    if (!getSysParamInfo("AS_ASP_ZeroDetailBillIntoDB",
                            sysParamInfoVector))
    {
        m_zeroDetailBillIntoIntDbTag = '0';
    }
    else
    {
    	for (iter=sysParamInfoVector.begin();
             iter!=sysParamInfoVector.end(); iter++)
        {
            m_zeroDetailBillIntoIntDbTag =iter->m_valueS[0];
            
            break;
        }
    }	
	
}


//##ModelId=4781DCDB017F
bool PM_MonthAccount::getLocalProvinceCodeFromCrm(string &provinceNo)
{    
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
  
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;

  int temp_total = DEFAULTARRAYLINE;
	
	sprintf(sqlstr,"SELECT area_code FROM td_m_area WHERE  area_level=10 AND this_tag='1'  ");
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
			
		if(m_crmDbi.dataBind(outvector,errvector))
		{
		 sprintf(m_msgbuf,"[Error(%s:%d): m_crmDbi.dataBind() failed.]\n",
            __FILE__,__LINE__);
     
     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf); 
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
		 sprintf(m_msgbuf,"[Error(%s:%d): m_crmDbi.executeSql() failed. errormessage:%s]\n",
            __FILE__,__LINE__,m_crmDbi.errorMessage());
     
     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf); 					
		}
	           
		m_nTotal 		+= temp_total;
		m_nSuccess 	+= temp_succ;
		m_nError 		+= temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();it++)
		{
			provinceNo   = *it;
	   	break; 				 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);

	return true;          
              	
}


bool PM_MonthAccount::reconnect()
{  
  try
  { 
  	if(m_altibaseConnected)
      m_altiDbi->disConnect();
    m_altibaseConnected=false;
    m_altiDbi->setConnInfo(m_altibaseUserName, m_altibasePasswd, m_altibaseServer, m_altibasePort);         
    m_altiDbi->connect();    
    m_altibaseConnected=true;  
    
    m_crmDbi.connect(m_crmUserName.c_str(), m_crmPasswd.c_str(),m_crmServer.c_str());     
  }
  catch (const exception & e)
  {
  	sprintf(m_msgbuf,"%s",e.what());  	
    
    THROW(DAOException,m_msgbuf);
  }  
  return false;
}

void PM_MonthAccount::setAltiBaseConnInfo(const string &s_userName,const string &s_passWord,
                                 					const string &s_servName,const string &s_port)
{
	m_altibaseUserName = s_userName;   
	m_altibasePasswd   = s_passWord;
	m_altibaseServer   = s_servName;	
	m_altibasePort     = s_port;
}

void PM_MonthAccount::setCrmConnInfo(const string &s_userName,
	                                   const string &s_passWord,
	                                   const string &s_servName)
{
	m_crmUserName = s_userName;   
	m_crmPasswd   = s_passWord;
	m_crmServer   = s_servName;	
}



//关联bill_account_X,TS_B_IMME_ADJUST  
//取出综合账单
void PM_MonthAccount::getTsAbillFromDetailAdjust()
{
	char   sqlcmd[MAXLONGBUFFLEN+1];
	string  l_strSql;
    int partitionCode = m_channelNo%10;
	
	try
	{
		int startpartitionid = partitionCode*1000;
  		int endpartitionid   = (partitionCode+1)*1000 - 1;
	
		l_strSql=" substr(a.user_id,1,2) = substr(b.eparchy_code,3,2)    AND ";
	
		if (m_provinceCode=="TJIN"||m_provinceCode=="HAIN")
  		{
  			l_strSql = "";
  		}
		else if (m_provinceCode=="XINJ")
		{
			l_strSql = " decode(substr(a.user_id,1,2),'11','01','12','02','13','03','16','06','18','08','19','09',substr(a.user_id,1,2)) = substr(b.eparchy_code,3,2)    AND ";
		}
		else if (m_provinceCode=="0017")
		{
	  		l_strSql =    " b.eparchy_code='0534' AND ";
		}
		else 
		{
			l_strSql = "";
		}
	
		int oeflag = (m_minRealCycId)%2;

		if (m_zeroDetailBillIntoIntDbTag=='0')
		{
			sprintf(sqlcmd	,"SELECT  a.acct_id,a.user_id,a.eparchy_code,a.detail_item_code,sum(a.fee) fee ,sum(a.a_discnt) a_discnt ,sum(a.adjust_before) adjust_before, " 
                 			 "to_char(sysdate,'YYYYMMDDHH24MISS') update_time"
			           		 " FROM (select acct_id,user_id,eparchy_code,detail_item_code,fee,a_discnt,adjust_before from BILL_IMME_ADJUST"
							 "      where cycle_id=%d"
							 "      union all "
							 "      select acct_id,user_id,eparchy_code,detail_item_code,fee,a_discnt,adjust_before from bill_account_%d)  a "  
			           	   	 " WHERE  %s "
			                 "   ( a.fee!=0  OR a.a_discnt!=0   OR " 
			                 "    		 a.adjust_before!=0 ) 	AND "
			               //" a.acct_id not like '99%%' AND "
			                 " mod(a.acct_id,10000) >= %d   AND "
			                 " mod(a.acct_id,10000) <= %d   " 
							 " group by acct_id,user_id,eparchy_code,detail_item_code"  			                 
			           		 "  ORDER by a.acct_id,a.user_id " ,m_minRealCycId,oeflag,l_strSql.c_str(),startpartitionid,endpartitionid );
		
		}
		else if (m_zeroDetailBillIntoIntDbTag=='1')
		{
		 sprintf(sqlcmd	,"SELECT  a.acct_id,a.user_id,a.eparchy_code,a.detail_item_code,sum(a.fee) fee ,sum(a.a_discnt) a_discnt ,sum(a.adjust_before) adjust_before, " 
                 		"to_char(sysdate,'YYYYMMDDHH24MISS') update_time"
			           	" FROM (select acct_id,user_id,eparchy_code,detail_item_code,fee,a_discnt,adjust_before from BILL_IMME_ADJUST"
							"  where cycle_id=%d"
							"  union all "
							"  select acct_id,user_id,eparchy_code,detail_item_code,fee,a_discnt,adjust_before from bill_account_%d)  a "  
			           	" WHERE %s "
			          //" a.acct_id not like '99%%' AND "
			            " mod(a.acct_id,10000) >= %d   AND "
			            " mod(a.acct_id,10000) <= %d   "
			            " group by acct_id,user_id,eparchy_code,detail_item_code"  
			           	" ORDER by a.acct_id,a.user_id " ,m_minRealCycId,oeflag,l_strSql.c_str() ,startpartitionid,endpartitionid);
		
		}
		
	/*	if (m_zeroDetailBillIntoIntDbTag=='0')
		{
		 sprintf(sqlcmd	," SELECT  a.acct_id,a.user_id,a.eparchy_code,a.detail_item_code,a.fee ,a.a_discnt ,a.adjust_before,"  
                 		 " to_char(sysdate,'YYYYMMDDHH24MISS') update_time " 
			               "  FROM bill_account_%d a " 
			           	   "	WHERE  %s "
			               "   		 ( a.fee!=0      OR a.a_discnt!=0   OR " 
			               "    		 a.adjust_before!=0 ) 	AND "
			               "        a.acct_id not like '99%%' AND "
			               "    		mod(a.acct_id,10000) >= %d   AND "
			               "        mod(a.acct_id,10000) <= %d   " 
			           		 "  ORDER by a.acct_id,a.user_id " ,oeflag,l_strSql.c_str(),startpartitionid,endpartitionid );
		
		}
		else if (m_zeroDetailBillIntoIntDbTag=='1')
		{
		 sprintf(sqlcmd	,"SELECT  a.acct_id,a.user_id,a.eparchy_code,a.detail_item_code,a.fee ,a.a_discnt ,a.adjust_before, " 
                 			"to_char(sysdate,'YYYYMMDDHH24MISS') update_time"
			           			"FROM bill_account_%d  a "  
			           			"WHERE %s "
			           			"a.acct_id not like '99%%' AND "
			                "mod(a.acct_id,10000) >= %d   AND "
			                "mod(a.acct_id,10000) <= %d   "  
			           			"ORDER by a.acct_id,a.user_id " ,oeflag,l_strSql.c_str() ,startpartitionid,endpartitionid);
		
		}*/		
		else 
		{	
			sprintf(m_msgbuf,"[Error(%s:%d):m_zeroDetailBillIntoIntDbTag=%d]\n",
                __FILE__,__LINE__,m_zeroDetailBillIntoIntDbTag);        
     
      		DEBUG_PRINTF("%s",m_msgbuf);
      
	  		THROW(DAOException, m_msgbuf) ;
	 	
		}
		
#ifdef _DEBUG_
	cout<<sqlcmd<<endl;
#endif		
			
		SQLHSTMT l_stmt=m_altiDbi->setSql(sqlcmd);    	 
    
		m_altiDbi->execQuery(l_stmt,sqlcmd);	
		
		getIntergrateBill();
	  
		m_altiDbi->commit();	
 	 
	}catch(const exception &e)
  {
    	sprintf(m_msgbuf,"[Error(%s:%d):%s]\n",
                __FILE__,__LINE__,e.what());        
     
      DEBUG_PRINTF("%s",m_msgbuf);
      
	  	THROW(DAOException, m_msgbuf) ;
  }   
    
}


//获取系统是否实时销帐标志0:不支持实时销帐 1:支持实时销帐
bool PM_MonthAccount::getRealWriteoffTag(char &realWriteoffTag)
{
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
  
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;

  int temp_total = DEFAULTARRAYLINE;
	
	sprintf(sqlstr,"SELECT tag_char FROM td_s_tag WHERE tag_code = 'AS_ASP_RealWriteOff' AND use_tag = '1' AND ROWNUM < 2 ");
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
			
		if(m_crmDbi.dataBind(outvector,errvector))
		{
	  	sprintf(m_msgbuf,"[%s:%d: m_crmDbi.dataBind() failed!]\n",
							__FILE__,__LINE__);
     
     	DEBUG_PRINTF("%s\n",m_msgbuf);
     
     	THROW(MonthEndException, m_msgbuf); 
			return false;
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
			sprintf(m_msgbuf,"[%s:%d: m_crmDbi.executeSql() failed. errormessage:%s!]\n",
							__FILE__,__LINE__,m_crmDbi.errorMessage());
     
     	DEBUG_PRINTF("%s\n",m_msgbuf);
     
     	THROW(MonthEndException, m_msgbuf); 				
							
			return false;
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();it++)
		{
			realWriteoffTag   = atoi((*it).c_str());
	   	break; 				 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);

	return true;          
	
}


bool PM_MonthAccount::execMonthTransbillCrm()
{
	
  char Val[10][40];
  char Sym[10][10];
  char procName[500];
  int sum;

  int ret=0,err=0;
  memset(procName,0,sizeof(procName));

  if (m_crmProcedureMap[string("p_asp_monthtransbillcrm")].m_execProc!="EXE")
  {
  	return false;
  }	
  	
  // parse procedure parameter string
  if (!parseProcString(m_crmProcedureMap[string("p_asp_monthtransbillcrm")].m_procedure.c_str(),procName,Val,Sym,sum))
  {
     sprintf(m_msgbuf,"[Error(%s:%d):ProcessProc(%s) failed!]\n",
            __FILE__,__LINE__,procName);
     
     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf);        
  }

    //indb procparam ,Hard-Code
    PROCPARAM procParam[3];
    procParam[0].action=PARAM_ACT_IN;
    procParam[0].type=PARAM_TYPE_STR;
    char l_strChanNo[8];
	//add 2009-02-10 zhouq begin
   int partitionCode = m_channelNo%10;
	//add 2009-02-10 zhouq end
	
    //sprintf(l_strChanNo,"%d",m_channelNo);
    sprintf(l_strChanNo,"%d",partitionCode);
    strcpy(procParam[0].strValue,l_strChanNo);
    procParam[1].action=PARAM_ACT_OUT;
    procParam[1].type=PARAM_TYPE_INT;
    //procParam[1].intValue=m_channelNo;
    procParam[1].intValue=partitionCode;
    procParam[2].action=PARAM_ACT_OUT;
    procParam[2].type=PARAM_TYPE_STR;
    strcpy(procParam[2].strValue,"");

    ret=m_crmDbi.callProcedure(procName,3,procParam);

    err=procParam[1].intValue;

    // call procedure failed!
    if(ret||err)
    {
      sprintf(m_msgbuf,"[Error(%s:%d):channel(%s) m_crmDbi.callProcedure(%s) failed. return:%d,errno=%d,errtext=%s!]\n",
            __FILE__,__LINE__,l_strChanNo,procName,ret,err,procParam[2].strValue);

     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf); 
    }

		return true;
}

bool PM_MonthAccount::parseProcString(const char * procNameStr, char * procName, char Val[10][40],char Sym[10][10],int& Sum)
{

  int sum;
  char val[10][50];
    
  if(procNameStr==NULL)
      return false;

    int len=strlen(procNameStr);
    int i,h,g,loc,j[10];

    i=0;
    for(h=0;;h++)
    {
      for(;i<len;i++)
      {
        if(procNameStr[i]=='(')
        {
            j[h]=i;
            break;
        }
        else if(procNameStr[i]==',')
        {
            j[h]=i;
            break;
        }
        else if(procNameStr[i]==')')
        {
            j[h]=i;
            break;
        }
      }
      if(procNameStr[i]==')')
          break;
      i++;
    }

    sum=++h;

    if(j[0]==0)
      return false;

    // get procedure name
    for(i=0;i<j[0];i++)
      procName[i]=procNameStr[i];


    // get parameter varible
    for(i=0;i<sum-1;i++)
    {
      if(j[i]+1==j[i+1])
          return false;

      for(h=0,g=j[i]+1;g<j[i+1];h++,g++)
          val[i][h]=procNameStr[g];
    }


    for(i=0;i<sum-1;i++)
    {
      len=strlen(val[i]);
      loc=len-strlen(strchr(val[i],':'));
      if((loc==0)||(loc==len-1))
          return false;

        // parameter name
      for(h=0;h<loc;h++)
          Val[i][h]=val[i][h];

        //  parameter indication
      for(g=0,h=loc+1;h<len;g++,h++)
          Sym[i][g]=val[i][h];
    }
    Sum =  sum-1;
    return true;
}

void PM_MonthAccount::setCrmProcInfo(map <string,ImportMode> ProcedureMap)
{
	m_crmProcedureMap=ProcedureMap;
	
}

void PM_MonthAccount::setCbsProcInfo(map <string,ImportMode> ProcedureMap)
{
	m_cbsProcedureMap=ProcedureMap;
	
}

void PM_MonthAccount::setAltibaseProcInfo(map <string,ImportMode> ProcedureMap)
{
	m_altibaseProcedureMap=ProcedureMap;
}



bool PM_MonthAccount::getOutPutChannelNoFromCrm(vector<channelSt> &channelStVec)
{    
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
       
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;

  int temp_total = DEFAULTARRAYLINE;
    	
  sprintf(sqlstr, " SELECT channel_no,start_id,end_id FROM  td_b_drecv_channel WHERE task_code= 'Drecv' AND  step_code = 'DrecvCalc' " ) ;
    	               
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
		channelSt l_channelSt;	
					
		if(m_crmDbi.dataBind(outvector,errvector))
		{
			DEBUG_PRINTF("m_crmDbi.dataBind() failed");
			return false;
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
			DEBUG_PRINTF("m_crmDbi.executeSql() failed. errormessage:%s",
							m_crmDbi.errorMessage());
			
			throw m_crmDbi.errorMessage();						  					
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();)
		{                   
			l_channelSt.m_channelNo=atoi((*it++).c_str());                           
			l_channelSt.m_startId=atoi((*it++).c_str());                          
			l_channelSt.m_endId=atoi((*it++).c_str());                          
		
	   	channelStVec.push_back(l_channelSt);			 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);
 
	return true;                     	
}


bool PM_MonthAccount::getOutPutBatchNoFromCrm(int &r_batchNo)
{    
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
       
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;

  int temp_total = DEFAULTARRAYLINE;
    	
  sprintf(sqlstr, " select batch_no from td_b_drecv where task_code = 'Drecv' and step_code = 'DrecvCalc' " ) ;
    	               
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
					
		if(m_crmDbi.dataBind(outvector,errvector))
		{
			DEBUG_PRINTF("m_crmDbi.dataBind() failed");
			return false;
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
			DEBUG_PRINTF("m_crmDbi.executeSql() failed. errormessage:%s",
							m_crmDbi.errorMessage());
			
			throw m_crmDbi.errorMessage();						  					
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();)
		{                   
			r_batchNo=atoi((*it).c_str());;                            
                        
	   	break;	 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);
 
	return true;                     	
}



bool PM_MonthAccount::execMonthTransBillPreCrm()
{	
  char Val[10][40];
  char Sym[10][10];
  char procName[500];
  int sum;

  int ret=0,err=0;
  memset(procName,0,sizeof(procName));
  
  if( !connected )
	{
		if(m_crmDbi.connect(m_userName.c_str(),
							m_passWord.c_str(),m_servName.c_str()))
		{
			m_errorCode=RC_CONN_FAIL;
			m_errorMessage=RM_CONN_FAIL;
      
      sprintf(m_msgbuf,"[Error(%s:%d):connect database error:%d : %s!]\n",
            __FILE__,__LINE__,m_errorCode,m_errorMessage.c_str());
            
			DEBUG_PRINTF("%s\n",m_msgbuf);
     
     	THROW(MonthEndException, m_msgbuf);
			
		}
		connected=1;
	}
  
	  if (m_crmProcedureMap[string("p_asp_monthtransbillpre")].m_execProc!="EXE")
  	{
  		return false;
  	}	

  // parse procedure parameter string
  if (!parseProcString(m_crmProcedureMap[string("p_asp_monthtransbillpre")].m_procedure.c_str(),procName,Val,Sym,sum))
  {
     sprintf(m_msgbuf,"[Error(%s:%d):ProcessProc(%s) failed!]\n",
            __FILE__,__LINE__,procName);
       
     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf);        
  }

    // indb procparam ,Hard-Code
    PROCPARAM procParam[3];
    //strcpy(pp[0].name,Val[0]);
    procParam[0].action=PARAM_ACT_IN;
    procParam[0].type=PARAM_TYPE_STR;
    char l_strChanNo[8];
	//add 2009-02-10 zhouq begin
   int partitionCode = m_channelNo%10;
	//add 2009-02-10 zhouq end
    //sprintf(l_strChanNo,"%d",m_channelNo);
    sprintf(l_strChanNo,"%d",partitionCode);
    strcpy(procParam[0].strValue,l_strChanNo);
    //strcpy(pp[1].name,Val[1]);
    procParam[1].action=PARAM_ACT_OUT;
    procParam[1].type=PARAM_TYPE_INT;
    //procParam[1].intValue=m_channelNo;
    procParam[1].intValue=partitionCode;
    procParam[2].action=PARAM_ACT_OUT;
    procParam[2].type=PARAM_TYPE_STR;
    strcpy(procParam[2].strValue,"");

    ret=m_crmDbi.callProcedure(procName,3,procParam);

    err=procParam[1].intValue;
    //printf("return value:%d errno=%d, errtext=%s\n",ret,outerr,pp[2].strValue);

    // call procedure failed!
    if(ret||err)
    {
      sprintf(m_msgbuf,"Error(%s:%d):channel(%s) m_crmDbi.callProcedure(%s) failed. return:%d,errno=%d,errtext=%s",
            __FILE__,__LINE__,l_strChanNo,l_strChanNo,ret,err,procParam[2].strValue);
        
     if(connected)
			{
				m_crmDbi.disconnect();
				connected=0;
			}
 
     DEBUG_PRINTF("%s\n",m_msgbuf);
     
     THROW(MonthEndException, m_msgbuf); 
    }
  

  return true;
	
}


//2008-3-4 20:12
void PM_MonthAccount::setOutPutFile(MonthEndOutputFile *pMonthEndOutputFile)
{
	m_pMonthEndOutputFile = pMonthEndOutputFile;
}

//2008-3-6 21:15
void PM_MonthAccount::setMaxRecordNum(unsigned long lMaxRecordNum)
{
	if (lMaxRecordNum<1000||lMaxRecordNum>1000000) lMaxRecordNum = MAX_RECORD_NUM;
	
	m_lMaxRecordNum = lMaxRecordNum;
	m_lMaxUserNum= m_lMaxRecordNum/10;
	
}



void PM_MonthAccount::getFeeSum(FEESUM &feeSum)
{
		feeSum = m_feeSum;
}

bool PM_MonthAccount::IsSpecialAcctId(ACCTID acct_id)   
{
	for(vector<long>::iterator itervec =  m_specialAcctIdVec.begin();
		  itervec!=m_specialAcctIdVec.end();
		  itervec++)
	{
	  
		  if (*itervec==atol((char*)acct_id))
				return true;
	}
				
	return false;
}


void PM_MonthAccount::getSpecialAcctId()
{
  char   sqlcmd[MAXLONGBUFFLEN+1];
	m_specialAcctIdVec.clear();
	long acct_id;
	try
	{
		sprintf(sqlcmd	,"SELECT acct_id FROM tf_f_specialacctid ");
			
		SQLHSTMT l_stmt=m_altiDbi->setSql(sqlcmd);    	 
		m_altiDbi->execQuery(l_stmt,sqlcmd);			
		while(m_altiDbi->next())
		{
			acct_id=m_altiDbi->getLong(1);
			m_specialAcctIdVec.push_back(acct_id);	
	  }
	    
		 m_altiDbi->commit();
	}catch(const exception &e)
  {
    	sprintf(m_msgbuf,"[Error(%s:%d):%s]\n",
                __FILE__,__LINE__,e.what());        
     
      DEBUG_PRINTF("%s",m_msgbuf);
      
	  	THROW(DAOException, m_msgbuf) ;
  }   
    
	
}


void PM_MonthAccount::GetBillId(int billnum)
{

	char   sqlcmd[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
  
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;

  int temp_total = DEFAULTARRAYLINE;	
 	long billid=0;
 	m_billidvec.clear();
	
	sprintf(sqlcmd	,"select f_sys_getseqid(null,'seq_bill_id') from ts_b_bill where rownum <%d ",billnum);
   
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
			
		if(m_crmDbi.dataBind(outvector,errvector))
		{
			sprintf(m_msgbuf,"[Error(%s:%d): m_crmDbi.dataBind() failed!]\n",
            __FILE__,__LINE__);
     
     	printf("%s\n",m_msgbuf);
         
     	//THROW(MonthEndException, m_msgbuf); 
		}
	      
		if(m_crmDbi.executeSql(sqlcmd,temp_total, temp_succ, temp_error))
		{
		  sprintf(m_msgbuf,"[%s:%d: m_crmDbi.executeSql() failed. errormessage:%s!]\n",
							__FILE__,__LINE__,m_crmDbi.errorMessage());
     
     	printf("%s\n",m_msgbuf);
     
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();)
		{
			billid=atol((*it++).c_str());
			m_billidvec.push_back(billid);
			
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);

  
}

//取下一个billid
long PM_MonthAccount::GetNextBillId()
{
  int billnum = 500;
	 if (m_billidvecit!=  m_billidvec.end()) 
	 		return *m_billidvecit++;
	 else 
	 {	 	
	   GetBillId(billnum)	;
	   m_billidvecit=m_billidvec.begin();
	   return *m_billidvecit++;
	 }	
	 	
}

//add 2009-02-10 zhouq begin
void PM_MonthAccount::getUserNetTypeCode()
{
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
       
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;
  
  
  int partitionCode = m_channelNo%10;
  int temp_total = DEFAULTARRAYLINE;
  
  sprintf(sqlstr, " SELECT user_id FROM tf_f_user WHERE net_type_code in ('16', '15') " ) ;
    	               
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
					
		if(m_crmDbi.dataBind(outvector,errvector))
		{
			DEBUG_PRINTF("m_crmDbi.dataBind() failed");
			throw m_crmDbi.errorMessage();
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
			DEBUG_PRINTF("m_crmDbi.executeSql() failed. errormessage:%s",
							m_crmDbi.errorMessage());
			
			throw m_crmDbi.errorMessage();						  					
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
	  long tm_user_id;
		for(it=outvector.begin();it!=outvector.end();)
		{                   
			tm_user_id=atol((*it++).c_str());                             
		       m_userNetTypecode.insert(tm_user_id); 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);
                     	
   if (m_nError)
   	{    
   	 sprintf(m_msgbuf,"%d",m_nError);	
   	 DEBUG_PRINTF("%s\n",m_msgbuf);
     THROW(MonthEndException, m_msgbuf); 
    }
}
//add 2009-02-10 zhouq end

void PM_MonthAccount::getUnacctUser()
{
	char sqlstr[MAXLONGBUFFLEN+1];
	
	StringVector outvector;
	StringVector errvector;
	StringVectorIterator it;
       
	m_nTotal=0;
	m_nSuccess=0;
	m_nError=0;
  
  long unacctuser=0;
  
  int temp_total = DEFAULTARRAYLINE;

  //add 2009-02-10 zhouq begin 
    int partitionCode = m_channelNo%10; 
  //add 2009-02-10 zhouq end
    	
  //sprintf(sqlstr, " SELECT user_id FROM tf_f_user partition (par_tf_f_user_%d) WHERE acct_tag='Z'",m_channelNo ) ;
   sprintf(sqlstr, " SELECT user_id FROM tf_f_user WHERE acct_tag='Z' and dummy_tag='0' " ) ; //只判普通用户2009-12-3 5:18:42
    	               
	do
	{ 
		int  temp_succ = 0, temp_error = 0;	 
					
		if(m_crmDbi.dataBind(outvector,errvector))
		{
			DEBUG_PRINTF("m_crmDbi.dataBind() failed");
			throw m_crmDbi.errorMessage();
		}
	      
		if(m_crmDbi.executeSql(sqlstr,temp_total, temp_succ, temp_error))
		{
			DEBUG_PRINTF("m_crmDbi.executeSql() failed. errormessage:%s",
							m_crmDbi.errorMessage());
			
			throw m_crmDbi.errorMessage();						  					
		}
	           
		m_nTotal += temp_total;
		m_nSuccess += temp_succ;
		m_nError += temp_error;
	   
		for(it=outvector.begin();it!=outvector.end();)
		{                   
			unacctuser=atol((*it++).c_str());                           
		
	   	m_unacctUser.insert(unacctuser);		   		 
		}  
	         
	}while (temp_total == DEFAULTARRAYLINE);
                     	
   if (m_nError)
   	{    
   	 sprintf(m_msgbuf,"%d",m_nError);	
   	 DEBUG_PRINTF("%s\n",m_msgbuf);
     THROW(MonthEndException, m_msgbuf); 
    }
}

void PM_MonthAccount::DeleteUnacctUser()
{
 	for (map<CAccUser,long>::iterator iterb=m_tmabillidMap.begin() ;iterb!=m_tmabillidMap.end(); iterb++)
 	{
 	   if( m_unacctUser.find(iterb->first.user_id)!=m_unacctUser.end())
 	    	 iterb->second =0;
 	
 	}	
 	
}


void PM_MonthAccount::UpdateBillId()
{
	map<CAccUser,long>::iterator iterb;
	CAccUser acctuser;	
	for(vector <IntergrateBill>::iterator iterS=m_intergrateBillVector.begin(); 
		  iterS!=m_intergrateBillVector.end() ;iterS++)
	{
		   acctuser.acct_id=iterS->acct_id;
		   acctuser.user_id=iterS->user_id;
		   if((iterb=m_tmabillidMap.find(acctuser))!=m_tmabillidMap.end())
		  {
		    	 	iterS->bill_id = iterb->second;	
		  }	
//		   if(iterS->bill_id ==0)
//		   {
//		   	m_freeFeeUserBillVector.push_back(*iterS);
//		   }
	}
}

//add 2009-02-10 zhouq begin
bool PM_MonthAccount::getNetType(long user_id)
{
	set<long>::iterator it = m_userNetTypecode.find(user_id);
	if (it == m_userNetTypecode.end())
	  return false;
	else 
	  return true;	
}
//add 2009-02-10 zhouq end
//added by pangx 2010-5-12 begin

void PM_MonthAccount::setOneClickPayProvinceCode(string &strOneClickPayProvinceCode)
{
    m_strOneClickPayProvinceCode=strOneClickPayProvinceCode;
}


bool PM_MonthAccount::getOneClickPayAccountInfo()
{    
    char sqlstr[MAXLONGBUFFLEN+1];

    StringVector outvector;
    StringVector errvector;
    StringVectorIterator it;
    StringVectorIterator itTemp;
       
    m_nTotal=0;
    m_nSuccess=0;
    m_nError=0;

    int temp_total = DEFAULTARRAYLINE;

    int partitionCode = m_channelNo%10; 	
    int startpartitionid = partitionCode*1000;
    int endpartitionid   = (partitionCode+1)*1000 - 1;

  
    sprintf(sqlstr, " SELECT a.account_id,b.group_id FROM TF_F_ACCOUNT_PARAM a ,TF_F_CUST_GROUP b ,TF_F_ACCOUNT c"
                  " WHERE b.cust_id=c.cust_id "
                  " AND a.account_id=c.acct_id"
                  " AND a.param_id='200'"
                  " AND a.param_value in('1','2')"
                  " AND a.start_date <= to_date('%s','YYYYMMDDHH24MISS') "
    		        " AND a.end_date >= to_date('%s','YYYYMMDDHH24MISS') "
    		        " AND c.partition_id >= %d "
    		        " AND c.partition_id <= %d "
    		         " AND c.re