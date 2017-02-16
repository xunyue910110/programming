
#include "RecycleApp.h"
#include "BillingConstDef.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "base/StringUtil.h"
#include "log4cpp/Logger.h"


#include <iostream>
#include <algorithm>


RecycleApp g_application;
Application * const theApp = &g_application;

RecycleApp::RecycleApp()
{
    m_maxFieldNum   = 0;
    m_connected = 0;
}

RecycleApp::~RecycleApp()
{
}

bool RecycleApp::recovery()
{
    Application::recovery();
    return true;
}

bool RecycleApp::initialization()
{
	setoptstr("c:");
	#ifndef _DEBUG_
  set_asdaemon(true);
	set_onlyone(true);
	set_runinbkg(true); 
  #endif
	if (!Application::initialization())
	{
		return false;
	}
	

	LOG_TRACE((*theLogger),"初始化,载入配置信息...");
	m_objectName = "程序初始化";
	m_status = PROC_READY;
	updateStatus();
	if (getSysParams() == false)
	{
		theErrorMessages->insert(EAPPFRM_SYSVAL,"Read Configuration Error");
		return false;
	}
	if (!checkRecycleFlag())
	{
		return false;
	}
	
	char channel[20];
	sprintf(channel,"%03d",m_channelNo);
	m_config.m_channelNo = channel;
	
	m_db.setFieldLength(MAXFIELDLEN_REC);
	time(&(m_config.m_timeStamp));
	
	time_t now; struct tm *w;
	char   tmp[10+1];time(&now);
	w = localtime(&now);
	sprintf(tmp,"%02d%02d%02d%02d%02d",w->tm_mon + 1,w->tm_mday,w->tm_hour, w->tm_min,w->tm_sec);
	//m_outputFile.initialize(&m_outputRuleList,tmp,m_config.m_maxCdrNum,&m_config);
    
    if ( m_config.m_needRollbackBill )
    {
    	//内存帐单类初始化
  		if (!m_billManager.initialization(m_config.m_mdbInfo)) 
  		{
  		  #ifdef _DEBUG_
  		    cout<<"bill init fail!"<<__FILE__<<__LINE__<<endl;
  		  #else
  		    theErrorMessages->insert(E_BILL_INIT,"bill init fail! ");  
  		  #endif
  		  return false;
  		}
  		m_billUpdate.initialization(&m_billManager,m_channelNo);
		m_outPutBill.initialize(tmp,&m_config);

/*		if (access(minusBillFile, F_OK|R_OK))
		{
			LOG_TRACE((*theLogger),"总量账单不存在,请确认批价程序是否已经停止!");
			m_theErrorMessages->insert(E_BILL_INITIALIZE,"总量账单不存在,请确认批价程序是否已经停止!");
			return false;
		}    	*/
 /*   	
    	LOG_TRACE((*theLogger),"初始化账单文件");
    	m_objectName = "加载总量账单文件";
		m_status = PROC_READY;
		updateStatus();
    	if (!m_billManager.initialize(&m_config))
    	{
    		m_theErrorMessages->insert(E_BILL_INITIALIZE,"加载总量账单失败!");
    		LOG_TRACE((*theLogger),"加载总量账单失败");
    		return false;
    	}*/
    }

    return true;
}

bool RecycleApp::loopProcess()
{
	if (recycleRun() == false)
	{
		LOG_TRACE((*theLogger),"执行失败!!!");
		m_theErrorMessages->insert(E_BILL_INITIALIZE,"执行失败!");
		//insertLog(1,"执行失败!");
		rollBack();
		//重做生成的临时文件
		m_outputFile.rollBack();
		m_stop = true;
	}
	else
	{
		m_objectName =  "回退程序成功执行... ";
		//insertLog(0,"回退程序成功执行... ");
		m_status = PROC_EXIT;
		updateStatus();
		LOG_TRACE((*theLogger),m_objectName.c_str());   
		m_stop = true;
    }
    return Application::loopProcess();
}

bool RecycleApp::recycleRun()
{
	time_t now;
	struct tm *w;
	char   tmp[10+1];
	int    i;
	long nTotal = 0, nSuccess = 0, nError = 0;
	int temp_total = 0, temp_succ = 0, temp_error = 0;
	StringVector dataColumnVector;    
	StringVector errorColumnVector;
	string bizType;
	int rateTimes;
	
	if(m_connected)
	{
		m_db.disconnect();
		m_connected=0;
	}
	
	LOG_TRACE((*theLogger),"连接数据通道");
	//1、连接数据库
	if(m_db.connect(m_config.m_userName.c_str(),m_config.m_passwd.c_str(),m_config.m_serverName.c_str()))
	{
		m_theErrorMessages->insert(ERROR_CONNECTDB,"连接数据库失败!");
		return false;
	}
	else 
	{
		m_connected=1;
	}
	
	
    if ( m_config.m_needRollbackBill )
	{//先回退BILL_ROLLBACK表中的记录
		if (!m_billUpdate.rollbackOldBill())
		{
			return false;
		}
	}
	
    for( i = 0; i < m_redoFlag.size(); i ++)
    {
    	int j;
		nTotal = 0;
		nSuccess = 0;
		nError = 0;
		temp_total = 0;
		temp_succ = 0;
		temp_error = 0;
    	
    	bizType = m_redoFlag[i].m_bizType;

    	//保存日志信息
    	m_status = m_redoFlag[i].m_status; 
    	
    	//得到当前重做业务的数据源格式
    	for(m_config.m_cdrFormatIndex = 0; m_config.m_cdrFormatIndex < m_config.m_cdrFormat.size(); m_config.m_cdrFormatIndex ++)
    	{
    		if (bizType == m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_bizType)
    		{
    			break;
    		}
    	}
    	//输出文件格式
    	if (getOutPutFileTypes() == false)
	    {
    		return false;
    	}
    	time(&now);
		w = localtime(&now);
		sprintf(tmp,"%02d%02d%02d%02d%02d",w->tm_mon + 1,w->tm_mday,w->tm_hour, w->tm_min, w->tm_sec);
		
		//Commented by Sunam 2006-3-31 16:14
		//m_outputFile.setDealTime(tmp);
		m_outputFile.initialize(&m_outputRuleList,tmp,m_config.m_maxCdrNum,&m_config);
		/*if ( m_config.m_needRollbackBill )
		{
			m_outPutBill.initialize(tmp,&m_config);
		}*/
    	//得到当前重做业务的sql语句
	
	for(m_config.m_dataBaseConfigIndex = 0; m_config.m_dataBaseConfigIndex < m_config.m_dataBaseConfig.size(); m_config.m_dataBaseConfigIndex ++)
    	{
    		if (m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_bizType == bizType)
    		{
    			m_config.m_selectSql = "select " ;
    			m_config.m_selectSql += m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_sqlColumns;
    			m_config.m_selectSql += " from ";
    			m_config.m_selectSql += m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_cdrTable;
    			break;
    		}
    	}

		//保存日志信息
		m_tableName = m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_cdrTable;
		
		//2、 如果需要调用存储过程，则调用存储过程
		if (m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_executeProc == true)
		{
			m_objectName =  "正在执行存储过程";
			m_objectName += m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_procName;
			m_objectName += "...";
			m_status = PROC_EXIT;
			updateStatus();
			LOG_TRACE_P1((*theLogger),"执行存储过程 %s",m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_procName.c_str());
			if (executeProc(m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_procName.c_str()) == false)
			{
				return false;
			}
		}

		//3、 新一轮的账单文件
/*		if ( m_config.m_needRollbackBill )
		{
			if (!m_billManager.begin())
			{
				return false;
			}
		}*/
		
		//4、从结果表中导出话单
		LOG_TRACE_P1((*theLogger),"检索数据源 %s",m_config.m_selectSql.c_str());
		m_db.dataBind(dataColumnVector, errorColumnVector);

		while (1)
		{
			//cout<<m_config.m_selectSql<<endl;
			if (m_db.executeSql(m_config.m_selectSql.c_str(),temp_total, temp_succ, temp_error) != 0)
			{
				m_theErrorMessages->insert(ERROR_EXECUTESQL,"执行SQL语句失败!");
				return false;
			}
			nTotal   += temp_total;
			nSuccess += temp_succ;
			nError   += temp_error;

			for (StringVector::iterator its = dataColumnVector.begin();its <= dataColumnVector.end() - m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_maxFieldNum; its += m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_maxFieldNum)
			{	
				int k = 0;
				StringVector::iterator b;
				for(b = its; b != its + m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_maxFieldNum; b++,k++)
				{
					strcpy(m_cdr[k],b->c_str());
					StringUtil::trim(m_cdr[k]);
				}
				//cdrFieldProcess();
				//4.1、 更新帐单信息
				if ( m_config.m_needRollbackBill )
				{//cout<<"cdr:"<<m_cdr[m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_userIdIndex-1]<<",user:"<<m_cdr[m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_billItemIndex-1]<<endl;
					T_SERVER_ID t_userId = m_cdr[m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_userIdIndex-1];
					//char t_billItem[MAXFIELDLEN_REC];
		
					m_billingCycle = m_redoFlag[i].m_billingCycle;//cout<<"cycle:"<<m_billingCycle<<endl;
					m_billUpdate.update(m_billingCycle,t_userId,
					                    m_cdr[m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_billItemIndex-1],
					                    m_rollBackBillVec);
					
/*					if ( ! m_billManager.update(m_cdr))
					{
						string rawCdr ;
						for(i=0;i<m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_maxFieldNum;++i)
						{
							rawCdr += m_cdr[i];rawCdr += ",";
						}
						LOG_TRACE((*theLogger),rawCdr.c_str());
						m_theErrorMessages->insert(E_BILL_FILE,"更新账单失败");
						return false;
					}*/
				}
				/*rateTimes为99的为历史无主话单回收，不做处理
				  其它情况下rateTimes个位存放重做次数
				  rateTimes10位为1时为无主回收
				               为2时为错单回收
				               为3是既做过无主回收又做过错单回收
				*/
				CdrFormat cdrFormat = m_config.m_cdrFormat[m_config.m_cdrFormatIndex];
                rateTimes = (cdrFormat.m_rateTimesIndex<=0?0:atoi(m_cdr[cdrFormat.m_rateTimesIndex-1])	);
				if ( rateTimes != 99)
				{
				    int rateTimes1,rateTimes2;
				    rateTimes1 = rateTimes % 10;
				    rateTimes2 = rateTimes / 10;
				    if (m_redoFlag[i].m_status == "REDO")
				    {
				        rateTimes1 = rateTimes1>8?9:rateTimes1+1;
				    }
				    else if (m_redoFlag[i].m_status == "NOINFORECYCLE")
				    {
				        rateTimes2 |= 1;
				    }
				    else if (m_redoFlag[i].m_status == "ERRRECYCLE")
				    {
				        rateTimes2 |= 2;
				    }
				    rateTimes = rateTimes2*10 + rateTimes1;
			        sprintf(m_cdr[cdrFormat.m_rateTimesIndex -1 ],"%d",rateTimes);
			    }
				//4.2、 将该cdr分发并按照规定的格式处理
				if (m_outputFile.putCdr(m_cdr)==false)
				{
					string rawCdr ;
					for(int i1=0;i1<m_config.m_cdrFormat[m_config.m_cdrFormatIndex].m_maxFieldNum;++i1)
					{
						rawCdr += m_cdr[i1];rawCdr += ",";
					}                    
					//LOG_TRACE((*theLogger),rawCdr.c_str());
					return false;
				}
				
				if ( m_config.m_needRollbackBill )
				{
					m_outPutBill.outputUserBill(m_billingCycle,m_rollBackBillVec);
				}
			}
			if ((nTotal % 1000) == 0) 
			{
				m_objectName =  "已处理清单 ";
				m_objectName = m_objectName + StringUtil::toString(nTotal) + " 条";
				m_status = PROC_RUN;
				updateStatus();
				LOG_TRACE((*theLogger),m_objectName.c_str());
			}
			if(temp_total != DEFAULTARRAYLINE)
			{
				if(!updateFlag(i))
				{
					m_theErrorMessages->insert(ERROR_FLAGERROR,"重新重做标志表状态失败!");
					return false;
				}
				break;
			}
		}
		
		LOG_TRACE_P1((*theLogger),"共处理 %d 条",nTotal);
		
		//5、从临时文件中正式输出到目标文件中
    	if (! m_outputFile.commit())
    	{
			m_theErrorMessages->insert(ERROR_FILECOMMITERROR,"文件提交失败！");
			m_outputFile.rollBack();
			return false;
		}
		LOG_TRACE((*theLogger),"清单输出成功！");
	
		//6、输出帐单信息
    	if ( m_config.m_needRollbackBill )
    	{
    		if(!m_outPutBill.commit())
    		{
    			m_theErrorMessages->insert(ERROR_FILECOMMITERROR,"负账单文件提交失败！");
    			m_outPutBill.rollBack();
					return false;
    		} 
    		
    		LOG_TRACE((*theLogger),"负账单输出成功！");
    		if(!m_billUpdate.commit())
    		{
    			m_theErrorMessages->insert(ERROR_FILECOMMITERROR,"账单回滚失败");
					return false;
    		}
    		
    		if(!m_billUpdate.clearRollBackBill())
    		{
    			m_theErrorMessages->insert(ERROR_FILECOMMITERROR,"清空内存回滚表失败！");
					return false;
    		} 
    		/*
			if (!m_billManager.commit() )
			{
				m_theErrorMessages->insert(E_FILE_BILL_COMMIT,"账单文件提交失败!");
				return false;
			}
			LOG_TRACE((*theLogger),"增量账单输出成功！");*/
    	}
    	m_redoFlag[i].m_execResult = "SUCCESS";

		//写日志
		m_bizType      = m_redoFlag[i].m_bizType;
		m_billingCycle = m_redoFlag[i].m_billingCycle;
		m_nTotal       = nTotal;
		writeLog();
	}
	//7、执行额外的语句 流程结束时，删除回收重做表里的数据
	for( i = 0; i < m_redoFlag.size(); i ++)
    	{
 		for(m_config.m_dataBaseConfigIndex = 0; m_config.m_dataBaseConfigIndex < m_config.m_dataBaseConfig.size(); m_config.m_dataBaseConfigIndex ++)
 		{
 			if(m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_ifTruncate == true)
 			{
 				if(m_redoFlag[i].m_bizType==m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_bizType)
 				{
 					if (m_db.executeSql(m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_truncateSql.c_str(),temp_total, temp_succ, temp_error) != 0)
					{
						m_theErrorMessages->insert(ERROR_EXECUTESQL,"Execute TruncateSql Error");
						LOG_TRACE_P1((*theLogger),"执行语句%s失败！",m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_truncateSql.c_str());
						//return false;
					}
					else
					{
						LOG_TRACE_P1((*theLogger),"执行语句%s成功！",m_config.m_dataBaseConfig[m_config.m_dataBaseConfigIndex].m_truncateSql.c_str());
					}
 				}
 			}
 		}
    	}
    	
        /*
    	if (m_config.m_ifTruncate == true)
    	{
		if (m_db.executeSql(m_config.m_truncateSql.c_str(),temp_total, temp_succ, temp_error) != 0)
		{
			m_theErrorMessages->insert(ERROR_EXECUTESQL,"Execute TruncateSql Error");
			return false;
		}
		LOG_TRACE_P1((*theLogger),"执行语句%s成功！",m_config.m_truncateSql.c_str());
	}
	*/

	//8、断开数据通道连接
	if(m_connected)
	{
		m_db.disconnect();
		m_connected=0;
	}
	return true;
}


bool RecycleApp::checkRecycleFlag()
{
	StringVector dataColumnVector;
	StringVector errorColumnVector;
	char logFileName[200];
	//char whereText[150];
	int i,j;
	
	if(m_db.connect(m_config.m_userName.c_str(),m_config.m_passwd.c_str(),m_config.m_serverName.c_str()))
	{
		m_theErrorMessages->insert(ERROR_CONNECTDB,"连接数据库失败!");
		return false;
	}
	else 
	{
		m_connected=1;
	}
	
	m_db.dataBind(dataColumnVector, errorColumnVector);

	//sprintf(whereText," where channel_no= %d ",m_channelNo); 
	int nTotal = 0, nSuccess = 0, nError = 0;
	//add auto_flag
	string sqlStr = "select biz_type,billing_cycle,channel_no,status,rowid from " + m_config.m_flagTable + " order by biz_type ";
	if (m_db.executeSql(sqlStr.c_str(), nTotal, nSuccess, nError) != 0)
	{
		m_theErrorMessages->insert(ERROR_EXECUTESQL,"执行SQL语句" + sqlStr + "失败");
		return false;
	}
	
	//Modified By Sunam 2009/9/16 11:27:50
	//if(dataColumnVector.size() > 35)
	if(dataColumnVector.size() > 55)
	{
		m_errMsg = "重做标志表 ";
		m_errMsg += m_config.m_flagTable;
		m_errMsg += " 中记录超过六条";
		m_theErrorMessages->insert(ERROR_FLAGERROR,m_errMsg);
		return false;
	}
	
	if(dataColumnVector.size() < 5)
	{
		m_errMsg = "重做标志表 ";
		m_errMsg += m_config.m_flagTable;
		m_errMsg += " 中无记录";
		m_theErrorMessages->insert(ERROR_FLAGERROR,m_errMsg);
		return false;
	}
	
	//重做账期及通道只以最前面一个为准
	if (m_config.m_needRollbackBill)
	{
		m_config.m_cycleDate = atoi(dataColumnVector[1].c_str());
	/*		if(m_config.m_billingChannalNo != atoi(dataColumnVector[2].c_str()))
		{
			m_theErrorMessages->insert(ERROR_FLAGERROR,"重做标志表通道与配置不符！");
			return false;
		}
		for (i = 0; i < (dataColumnVector.size() / 5); i ++)
		{
			if (m_config.m_cycleDate != atoi(dataColumnVector[i*5 + 1].c_str()))
			{
				m_errMsg = "重做标志表 ";
				m_errMsg += m_config.m_flagTable;
				m_errMsg += " 中账期不完全一致";
				LOG_TRACE((*theLogger),m_errMsg.c_str());
				m_theErrorMessages->insert(ERROR_CYCLEORCHANNELNO,m_errMsg);
				return false;
			}
			if (m_config.m_billingChannalNo != atoi(dataColumnVector[i*5 + 2].c_str()))
			{
				m_errMsg = "重做标志表 ";
				m_errMsg += m_config.m_flagTable;
				m_errMsg += " 中通道不完全一致";
				LOG_TRACE((*theLogger),m_errMsg.c_str());
				m_theErrorMessages->insert(ERROR_CYCLEORCHANNELNO,m_errMsg);
				return false;
			}
		}*/
	}
	
	RedoFlagConfig redoFlag;
	
	for (i = 0; i < dataColumnVector.size(); )
	{
		//bizType
		redoFlag.m_bizType = dataColumnVector[i++];
		
		//重做标志表不充许有一条以上的记录
		for (j = 0; j < m_redoFlag.size(); j ++)
		{
			if (redoFlag.m_bizType == m_redoFlag[j].m_bizType)
			{
				m_errMsg = "重做标志表业务类型 ";
				m_errMsg += redoFlag.m_bizType;
				m_errMsg += " 有一条以上的记录";
				LOG_TRACE((*theLogger),m_errMsg.c_str());
				m_theErrorMessages->insert(ERROR_FLAGERROR,m_errMsg);
				return false;
			}
		}
		//检查重做的业务类型
		if ((redoFlag.m_bizType != BIZ_TYPE_GSM)
			&&(redoFlag.m_bizType != BIZ_TYPE_SMS)
			&&(redoFlag.m_bizType != BIZ_TYPE_GPRS)
			&&(redoFlag.m_bizType != BIZ_TYPE_UNICOMSP)
			&&(redoFlag.m_bizType != BIZ_TYPE_STATIS)
			&&(redoFlag.m_bizType != BIZ_TYPE_UNICOMIP)
			&&(redoFlag.m_bizType != BIZ_TYPE_SPMONTH)
			&&(redoFlag.m_bizType != BIZ_TYPE_FIXVOICE)
			&&(redoFlag.m_bizType != BIZ_TYPE_FIXSMS)
			&&(redoFlag.m_bizType != BIZ_TYPE_NET)
			&&(redoFlag.m_bizType != BIZ_TYPE_FISC))
		{
			m_errMsg = redoFlag.m_bizType;
			m_errMsg += " 无效的重做业务类型！";
			LOG_TRACE((*theLogger),m_errMsg.c_str());
			m_theErrorMessages->insert(ERROR_FLAGERROR,m_errMsg);
			return false;
		}
		//billingCycle
		redoFlag.m_billingCycle = atoi(dataColumnVector[i++].c_str());
		//channelNo
		redoFlag.m_channelNo = atoi(dataColumnVector[i++].c_str());
		//status
		redoFlag.m_status = dataColumnVector[i++];
		//检查重做状态
		//Modified By Sunam 2009/9/16 11:29:34
		//if ((redoFlag.m_status != "REDO") && (redoFlag.m_status != "RECYCLE"))
		if ((redoFlag.m_status != "REDO") && 
		    (redoFlag.m_status != "NOINFORECYCLE") &&
		    (redoFlag.m_status != "ERRRECYCLE"))
		{
			m_errMsg = "重做标志表 ";
			m_errMsg += m_config.m_flagTable;
			m_errMsg += " 中STATUS必须为REDO、NOINFORECYCLE、ERRRECYCLE";
			LOG_TRACE((*theLogger),m_errMsg.c_str());
			//insertLog(1,m_errMsg.c_str());
			m_theErrorMessages->insert(ERROR_FLAGERROR,m_errMsg);
			
			return false;
		}
		redoFlag.m_rowid = dataColumnVector[i++];
		m_redoFlag.push_back(redoFlag);
	}
	
	if (m_config.m_needRollbackBill)
	{
	/*	memset(logFileName,0,sizeof(logFileName));
		sprintf(logFileName,"%s%03dBILL.LOG",m_config.m_billLogPath.c_str(),m_config.m_billingChannalNo);
		ifstream in(logFileName);
		
		if(in.fail())
		{
			m_theErrorMessages->insert(ERROR_BILLLOGERROR,"账单日志错误");
			return false;
		}
		
		char buffer[2][256];
		in.getline(buffer[0], sizeof(buffer[0]));
		if(in.fail())
		{
			m_theErrorMessages->insert(ERROR_BILLLOGERROR,"账单日志错误");
			return false;
		}
		sprintf(buffer[1], "%06d%03d", m_config.m_cycleDate, m_config.m_billingChannalNo);
		
		if(strcmp(buffer[0], buffer[1]) != 0)
		{
			m_theErrorMessages->insert(ERROR_CYCLEORCHANNELNO,"帐期或通道不符");
			return false;
		}
		in.close();
		in.clear();*/
	}
	
	if(m_connected)
	{
		m_db.disconnect();
		m_connected=0;
	}

	return true;
}

bool RecycleApp::updateFlag(const int index)
{
	string sqlStr = "UPDATE " + m_config.m_flagTable + " SET STATUS = 'SUCCESS' WHERE ROWID = '" + m_redoFlag[index].m_rowid;
	sqlStr += "'";
	int nTotal,nSuccess,nError;
	if (m_db.executeSql(sqlStr.c_str(), nTotal, nSuccess, nError) != 0)
	{
		m_theErrorMessages->insert(ERROR_EXECUTESQL,"执行SQL语句" + sqlStr + "失败");
		return false;
	}
	return true;
}

bool RecycleApp::executeProc(string procName) {
	char temp[256];temp[0]=0;

	PROCPARAM parameters[5];
	
	parameters[0].action = PARAM_ACT_IN;	parameters[0].type = PARAM_TYPE_STR;	parameters[0].strValue[0] = '\0';
	parameters[1].action = PARAM_ACT_IN;	parameters[1].type = PARAM_TYPE_STR;	parameters[1].strValue[0] = '\0';
	parameters[2].action = PARAM_ACT_IN;	parameters[2].type = PARAM_TYPE_STR;	parameters[2].strValue[0] = '\0';
	parameters[3].action = PARAM_ACT_OUT;	parameters[3].type = PARAM_TYPE_INT;	parameters[3].intValue = 0;
	parameters[4].action = PARAM_ACT_OUT;	parameters[4].type = PARAM_TYPE_STR;	parameters[4].strValue[0] = '\0';
	
	if (m_db.callProcedure(procName.c_str(), 5, parameters) != 0) {
        m_theErrorMessages->insert(ERROR_PROCEDURE,"a- execute procedure fail!");
        return false;
	}
	
	if(parameters[3].intValue != 0) {  
	   sprintf(temp,"result:%d, %s",parameters[3].intValue,parameters[4].strValue);
	   m_theErrorMessages->insert(ERROR_PROCEDURE,"b-execute procedure fail!");
	   m_theErrorMessages->insert(ERROR_PROCEDURE,  temp );
       return false;
	}
    return true;
}


bool RecycleApp::end() {
    if (m_connected) {
        m_db.disconnect();
        m_connected=0;
    }
    m_outputFile.end();
    
    if (m_config.m_needRollbackBill)
    {
    /*    if (!m_billManager.end()) {
            m_theErrorMessages->insert(E_BILL_END,"bill file fail!");
        }*/
    }    

    return Application::end();
}
//------------------------------------------------------------------------------

bool RecycleApp::getSysParams() {
    string  path, name, value;
    string  t_channelNo;
    char    cTmp[4];
    
    sprintf(cTmp, "%d", m_channelNo);
    t_channelNo =  cTmp;
    
    // 应用全局配置
    path = SECTDELIM; path += m_name ;
    name = "DBPARAM_SERVERNAME";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_dbParamServName = value;

    name = "DBPARAM_USERNAME";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_dbParamUser = value;
    
    name = "DBPARAM_PASSWORD";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_dbParamPassword = value;


	
    //Added by Sunam 2006-3-14 11:42 FOR DR
/*    name = "DROUTPUT";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    if (value == "YES")
    {
    	m_config.m_drOutPut = true;
    }
    else
    {
    	m_config.m_drOutPut = false;
    }*/
    
    //Added by Sunam 2005-12-7 从批价配置中读取省代码
    path = SECTDELIM;
    path += "rate";
	path += SECTDELIM;
	path += "common";
	name = "province_code";
	if (!m_theSysParam->getValue(path, name, value))
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
		return false;
	}
	m_config.m_provinceCode = value;
	//Added by Sunam 2005-12-13 GPRS流量处理
	name = "gprsFlowDeal";
	if (!m_theSysParam->getValue(path, name, value))
	{
		//m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
		//return false;
		m_config.m_gprsFlowDeal = "";
	}
	else
	{
		m_config.m_gprsFlowDeal = value;
	}

    // 对应通道下的配置信息
    path  = SECTDELIM;
    path += m_name;
    path += SECTDELIM;
    path += t_channelNo;
    name = "SERVERNAME";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_serverName = value;
    
    name = "USERNAME";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_userName = value;
    
    name = "PASSWORD";
    if (!m_theSysParam->getValue(path, name, value))  {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_passwd = value;

    name = "mdb_dbtype";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_mdbInfo.m_dbType = T_DATABASETYPE(atoi(value.c_str()));

    name = "mdb_host";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_mdbInfo.m_hostName = value;

    name = "mdb_user";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_mdbInfo.m_user = value;

    name = "mdb_pass";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_mdbInfo.m_passwd = value;

    name = "mdb_port";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_mdbInfo.m_port = atoi(value.c_str());

    name = "MAXCDRNUM";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_maxCdrNum = atol(value.c_str());

    name = "RAWDATAPATH"; value = "";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    if(value.length() > 0)
    {
	    if (value[value.length() - 1] != '/')
	    {
	    	m_config.m_rawDataPath = value + "/";
	    }
	    else
	    {
	    	m_config.m_rawDataPath = value;
	    }
	}
	else
	{
	    m_config.m_rawDataPath = value;
	}

    name = "TEMP_RAWDATAPATH"; value = "";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    if(value.length() > 0)
    {
        if (value[value.length() - 1] != '/')
	    {
	    	m_config.m_rawDataPathTemp = value + "/";
	    }
	    else
	    {
	    	m_config.m_rawDataPathTemp = value;
	    }
	}
	else
	{
	    m_config.m_rawDataPathTemp = value;
	}
    //Added by Sunam 2006-3-14 15:02 FOR DR
    /*if (m_config.m_drOutPut)
    {
    	name = "DROUTPUTPATH";
    	value = "";
    	if (!m_theSysParam->getValue(path, name, value))
    	{
        	m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        	return false;
    	}
    	if (value.length() == 0)
    	{
    		m_config.m_drOutPut = false;
    	}
    	else
    	{
    		if (value[value.length() - 1] != '/')
			{
				m_config.m_drOutPutPath = value + "/";
			}
			else
			{
				m_config.m_drOutPutPath = value;
			}
    	}
    }*/
    
    name = "ROLLBACK_BILL";
    if (!m_theSysParam->getValue(path, name, value)) {
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
        return false;
    }
    m_config.m_needRollbackBill = (StringUtil::toUpperCase(value) == "YES");
    
	if (m_config.m_needRollbackBill )
	{
		LOG_TRACE((*theLogger),"系统配置要求回减账单")
		name = "MINUSBILL_PATH";                                                 
		if (!m_theSysParam->getValue(path, name, value))
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
			return false;                                                       
		}
		if (value.length() == 0)
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name + " 为空"); 
			return false;
		}
		if (value[value.length() - 1] != '/')
		{
			m_config.m_minusBillPath = value + "/";
		}
		else
		{
			m_config.m_minusBillPath = value;
		}
		
		name = "TEMPMINUSBILL_PATH";                                                 
		if (!m_theSysParam->getValue(path, name, value))
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
			return false;                                                       
		}
		if (value.length() == 0)
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name + " 为空"); 
			return false;
		}
		if (value[value.length() - 1] != '/')
		{
			m_config.m_tmpminusBillPath = value + "/";
		}
		else
		{
			m_config.m_tmpminusBillPath = value;
		}

		name = "BAKBILL_PATH";                                                 
		if (!m_theSysParam->getValue(path, name, value))
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
			return false;                                                       
		}
		if (value.length() == 0)
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name + " 为空"); 
			return false;
		}
		if (value[value.length() - 1] != '/')
		{
			m_config.m_bakBillPath = value + "/";
		}
		else
		{
			m_config.m_bakBillPath = value;
		}

		name = "BILLLOG_PATH";                                                 
		if (!m_theSysParam->getValue(path, name, value))
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
			return false;                                                       
		}
		if (value.length() == 0)
		{
			m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name + " 为空"); 
			return false;
		}
		m_config.m_billLogPath = value; 
		if (m_config.m_billLogPath[m_config.m_billLogPath.length() - 1] != '/')
		{
			m_config.m_billLogPath = m_config.m_billLogPath + "/";
		}
	}

    /*
    name = "CYCLE_SQL";                                                 
    if (!m_theSysParam->getValue(path, name, value)) {                      
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
        return false;                                                       
    }                                                                       
    m_config.m_cycleSql = value;   
    */        
    name = "BILLING_CHANNALNO";                                                 
    if (!m_theSysParam->getValue(path, name, value)) {                      
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
        return false;                                                       
    }                                                                       
    m_config.m_billingChannalNo = atoi(value.c_str());

	name = "FLAGTABLE";
	if (!m_theSysParam->getValue(path, name, value))
	{
	    m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
	    return false;
	}
	m_config.m_flagTable = value;

//Deleted by kongqm 2007-01-21 22:05:00 去除特殊账单项(杨屹确认特殊账单项在批价中已去除)	
//    name = "DISCOUNT_FEEID";                                                 
//  if (!m_theSysParam->getValue(path, name, value)) {                      
//      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
//      return false;                                                       
//  }                                                                       
//  m_config.m_discountFeeId = atoi(value.c_str());   
//
//    name = "INTERPROV_ROAM_FEEID";                                                 
//    if (!m_theSysParam->getValue(path, name, value)) {                      
//        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
//        return false;                                                       
//    }                                                                       
//    m_config.m_interProvRoamFeeId = atoi(value.c_str()); 
//    
//    name = "INTERNATIONAL_ROAM_FEEID";                                                 
//    if (!m_theSysParam->getValue(path, name, value)) {                      
//        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
//        return false;                                                       
//    }                                                                       
//    m_config.m_interNationalRoamFeeId = atoi(value.c_str()); 
    //Added by Xiel 2006-01-15  增加HOME_AREA_CODE配置
    name = "HOME_AREA_CODE";                                                 
    if (!m_theSysParam->getValue(path, name, value)) {                      
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
        return false;                                                       
    }                                                                       
    m_config.m_homeAreaCode = value; 
    
    //Added by Sunam 2005-12-27 读取批价对应通道的channelType
/*    path = SECTDELIM;
    path += "rate";
	path += SECTDELIM;
	path += StringUtil::toString(m_config.m_billingChannalNo);
	name = "channel_type";
	
	if (!m_theSysParam->getValue(path, name, value))
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name); 
		return false;
    }
    m_config.m_channelType = StringUtil::toUpperCase(value);*/

	// 根据引用的格式具体的格式描述
	/*name = "CDR_FORAMT";
	if (!m_theSysParam->getValue(path, name, val