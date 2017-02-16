/******************************************************************
 *Copyright (c) 2002-2003 南京联创公司联通软件开发中心
 *创建人: 陈忠敏
 *日　期: 2006.11.7
 *描　述:
 *版　本: 1.0
 *****************************************************************/

#include "PreCardDeal.h"
#include "pass/DBPassOper.h"

//全局变量
Environment *env=0;

Connection *conn_1=0;
Connection *conn_2=0;
Connection *conn_3=0;
Connection *conn_4=0;

Statement *stmt;

string DataFilePath="";
string DataFileBakPath="";

string LogFilePath="";
string LogFileName;
string IniFileName;

string strConnEparchyCode1="";
string strConnEparchyCode2="";
string strConnEparchyCode3="";
string strConnEparchyCode4="";

int iLogSuccessSNFlag=0;
int iSleepInterval=3;

int main(int argc,char **argv)
{
	if (argc != 2)
	{
		cout<<"格式(上线):nohup PreCardDeal 1\n格式(测试):nohup PreCardDeal 0"<<endl;
		return 0;
	}
	
	int iOnLine=atoi(argv[1]);
	
	//从环境变量和INI文件中获取系统的配置信息
	if (GetSysConfig() == -1)
		return 0;
	
	//守护进程申明
	daemon_init(argv[0],1);
	muserlog("系统启动在%s模式",(iOnLine == 1)?"在线":"测试");

	//创建到数据库的连接
	if (ConnectToOracle() == -1){
		cout<<"connect to oracle failed!"<<endl;
		return -1;
	}
	string AcctFileName,AcctBakFileName,CommandStr;
	while (1)
	{
		GetLogFileName();//生成日志文件名称-日期改变,文件名也要改变
		
		VFILELIST vFileList;
		//判断有没有计费文件生成
		if (!IsAcctFileExists(vFileList))
		{
			if (iOnLine == 1)
			{
				sleep(iSleepInterval);
				continue;
			}
			else
				break;//上线后使用上面2行代码
		}
		
		vector<string>::iterator it;
		for (it = vFileList.begin(); it != vFileList.end(); it ++)
		{
			//处理一个文件中的数据(不同地市使用不同的数据库连接)
			AcctFileName=DataFilePath + string("/") + *it;	
			if (DealData(AcctFileName) == -1){
				DisconnectFromOracle();
				return -1;
			}
		}
		if (iOnLine == 1)
		{
			
			//备份文件
			for (it = vFileList.begin(); it != vFileList.end(); it ++)
			{
				AcctFileName=DataFilePath + string("/") + *it;
				AcctBakFileName=DataFileBakPath + string("/") + *it;
				if (file_exists(AcctBakFileName.c_str()) == true)
				{
					CommandStr=string("rm -f ")+AcctBakFileName;
					system(CommandStr.c_str());
				}
				CommandStr = string("mv ") + AcctFileName + string(" ") + AcctBakFileName;
				system(CommandStr.c_str());
			}
			muserlog("[%s]备份文件......",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());
			//校验文件是否成功备份了
			for (it = vFileList.begin(); it != vFileList.end(); it ++)
			{
				AcctFileName=DataFilePath + string("/") + *it;
				AcctBakFileName=DataFileBakPath + string("/") + *it;
				while (1)
				{
					//备份文件已经生成，原文件已经删除
					if (file_exists(AcctBakFileName.c_str()) == true 
					&&  file_exists(AcctFileName.c_str()) == false )
					{
						break;
					}
					sleep(1);
				}
			}
			muserlog("[%s]备份完成......",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());			
		}
		else
		{
			break;//上线后注释掉
		}
	}
	//断开到数据库的连接
	DisconnectFromOracle();

	return 0;
}
int GetLogSuccessSNFlag()
{
	string strValue="";
	string strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"syscfg","LogSuccessSN",strValue,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	try
	{
		iLogSuccessSNFlag = atoi(strValue.c_str());
		muserlog("[%s]LogSuccessSN=%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),(iLogSuccessSNFlag == 1)?"成功记录日志":"成功不记录日志");
	}
	catch(...)
	{
		muserlog("[%s]无效的数值([syscfg] - LogSuccessSN)[%s]",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strValue.c_str());
		return -1;
	}
	return 0;
}
int GetSleepInterval()
{
	string strValue="";
	string strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"syscfg","SleepInterval",strValue,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	try
	{
		iSleepInterval = atoi(strValue.c_str());
		if (iSleepInterval <= 0)
			iSleepInterval = 1;
		muserlog("[%s]SleepInterval=%d秒",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),iSleepInterval);
			
	}
	catch(...)
	{
		muserlog("[%s]无效的数值([syscfg] - SleepInterval)[%s]",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strValue.c_str());
		return -1;
	}
	return 0;
}
bool IsAcctFileExists(VFILELIST &vFileList)
{
	string strCommand;
	strCommand="cd $ACCT_DATA_FILE_PATH \nls > file_list";
	system(strCommand.c_str());
	
	string strFileListName=DataFilePath + string("/file_list");
	sleep(1);
	
	if (file_exists(strFileListName.c_str()) == false)
		return false;

	if (file_size(strFileListName.c_str()) == 0)
	{
		//没有计费文件
		unlink(strFileListName.c_str());
		return false;
	}
	
	//将文件名载入vector
	vFileList.clear();
	
	FILE *fp=fopen(strFileListName.c_str(),"r");
	char cLineStr[256]="\0";
	int iLimitCount = 0;
	while (1)
	{
		memset(cLineStr,0,sizeof(cLineStr));
		if (fgets(cLineStr,256,fp) == NULL)
			break;
		rtrim(cLineStr);//去除\n字符
		if (strcmp(cLineStr,"file_list") == 0)
			continue;
		if (strcmp(cLineStr,"nohup.out") == 0)
			continue;

		vFileList.push_back(string(cLineStr));

		iLimitCount ++;
		if (iLimitCount > 1000) //一次最多处理1000个文件
			break;
	}
	fclose(fp);

	//删除列表文件
	unlink(strFileListName.c_str());
	if (iLimitCount == 0)
		return false;
	
	muserlog("[%s]计费文件已经生成",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());

	return true;	
}
int DealOneRecord(string &strSN, string &strEparchyCode, string &strStartTime)
{
	//cout<<strSN<<endl;
	//获取用户标识,网别
	string sqlStmt = "SELECT to_char(user_id),net_type_code FROM tf_f_user where serial_number=:v1 and remove_tag='0' and net_type_code in ('10','13','15')";
	ResultSet *rset=0;
	string strUserID="";
	string strNetTypeCode="";
	Connection *conn=GetConnect(strEparchyCode);
	if (conn == NULL)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-C->无效的地市编码"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str());		
		return 0;	
	}
	try
	{
		stmt = conn->createStatement(sqlStmt);
		stmt->setString(1,strSN);
		rset = stmt->executeQuery();
		int inum=0;
		while (rset->next())
		{
			  
		  	strUserID = rset->getString(1);
		  	strNetTypeCode = rset->getString(2);
		  	inum++;
		  	muserlog("user=%s,%s",strUserID,strNetTypeCode);
			break;
		}
		stmt->closeResultSet(rset);
		conn->terminateStatement(stmt);
		if (inum != 1)
		{
			muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-A->获取到[%d]个用户标识"
			         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
			         ,strSN.c_str()
			         ,strEparchyCode.c_str()
			         ,strStartTime.c_str()			         
					 ,inum);
			return 0;
		}
	}
	catch(SQLException &ex)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-B->获取用户标识失败:ErrorCode=%d,ErrorDesc=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,ex.getErrorCode()
		         ,ex.getMessage().c_str());
		conn->terminateStatement(stmt);         
		return 0;
	}
	string strSubscribeID,strTradeID;
	if (getSubscribeIDAndTradeID(strEparchyCode,strSubscribeID,strTradeID) == -1 )
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s"
				         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
				         ,strSN.c_str()
				         ,strEparchyCode.c_str()
				         ,strStartTime.c_str());		
		return 0;
	}
	if (strEparchyCode.length()!=4)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-E->EparchyCode应该为4位"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str());		
		conn->terminateStatement(stmt);
		return 0;		
	}

	
	//插入tf_b_cust_order
	sqlStmt = "insert into tf_b_cust_order ";
	sqlStmt+= "(SUBSCRIBE_ID,PARTITION_ID,ORDER_TRADE_TYPE,ORDER_TYPE,ORDER_STATE,IS_HEADQT,ACCEPT_DATE,TRADE_STAFF_ID,ORDER_REMARK) values ";
	sqlStmt+= "(:v1         ,:v2            ,'00002'         ,'00001'   ,'9'        ,'0'      ,sysdate    ,'SUPERUSR'    ,'计费首话单')";
	try
	{
		stmt = GetConnect(strEparchyCode)->createStatement (sqlStmt);
		stmt->setString(1,strSubscribeID);
		stmt->setString(2,strSubscribeID.substr(strSubscribeID.length()-4));
		stmt->executeUpdate();
		conn->terminateStatement(stmt);
	}
	catch (SQLException &ex)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-G->插入tf_b_cust_order失败:ErrorCode=%d,ErrorDesc=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,ex.getErrorCode()
		         ,ex.getMessage().c_str());
		conn->terminateStatement(stmt);
		conn->rollback();
		return 0;
	}	
	//插入tf_bh_trade
	sqlStmt = "insert into tf_bh_trade ";
	sqlStmt+= "(TRADE_ID      ,SUBSCRIBE_ID  ,TRADE_TYPE_CODE,IN_MODE_CODE,PRIORITY,SUBSCRIBE_STATE,NEXT_DEAL_TAG,USER_ID       ,NET_TYPE_CODE,SERIAL_NUMBER,ACCEPT_DATE,ACCEPT_MONTH  ,TRADE_STAFF_ID,TRADE_DEPART_ID,TRADE_CITY_CODE,TRADE_EPARCHY_CODE,EPARCHY_CODE,OLCOM_TAG,EXEC_TIME,FINISH_DATE,OPER_FEE,FOREGIFT,ADVANCE_PAY,FEE_STATE,CANCEL_TAG       ) values ";
	sqlStmt+= "(to_number(:v1),to_number(:v2),502            ,'0'         ,50      ,'9'            ,'0'          ,to_number(:v3),:v4          ,:v5          ,sysdate    ,to_number(:v6),'SUPERUSR'    ,'00000'        ,'0000'         ,:v7               ,:v8         ,'0'      ,sysdate  ,sysdate    ,0       ,0       ,0          ,'0'      ,'0'     ) ";
	try
	{
		stmt = GetConnect(strEparchyCode)->createStatement (sqlStmt);
		stmt->setString(1,strTradeID    );
		stmt->setString(2,strSubscribeID);
		stmt->setString(3,strUserID);
		stmt->setString(4,strNetTypeCode);
		stmt->setString(5,strSN);
		stmt->setString(6,strTradeID.substr(4,2));
		stmt->setString(7,strEparchyCode);
		stmt->setString(8,strEparchyCode);
		stmt->executeUpdate();
		conn->terminateStatement(stmt);
	}
	catch (SQLException &ex)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-H->插入tf_bh_trade失败:ErrorCode=%d,ErrorDesc=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,ex.getErrorCode()
		         ,ex.getMessage().c_str());
		conn->terminateStatement(stmt);
		conn->rollback();
		return 0;
	}		
	//调用存储过程
	sqlStmt = "BEGIN p_csm_firstcalldeal(:v1,  to_date(:v2,'yyyymmddhh24miss'), :v3, :v4, :v5); END;";
	try
	{
		stmt = GetConnect(strEparchyCode)->createStatement (sqlStmt);
		stmt->setString(1,strUserID);
		stmt->setString(2,strStartTime);
		stmt->setString(3,strTradeID);
		stmt->registerOutParam(4,OCCIINT,4);
		stmt->registerOutParam(5,OCCISTRING,256);
		stmt->executeUpdate();
	}
	catch (SQLException &ex)
	{
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-C->存储过程执行失败:ErrorCode=%d,ErrorDesc=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,ex.getErrorCode()
		         ,ex.getMessage().c_str());
		conn->terminateStatement(stmt);
		conn->rollback();
		return 0;
	}

    int  iResultCode = stmt->getInt(4);
    string strResultInfo = stmt->getString(5);

    if (iResultCode == 0)
    {
    	conn->commit();
		if (iLogSuccessSNFlag == 1)
		{
			muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-D->处理成功"
			         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
			         ,strSN.c_str()
			         ,strEparchyCode.c_str()
			         ,strStartTime.c_str());
		}
    }
    else if (iResultCode == 1)
    {
    	conn->commit();
		if (iLogSuccessSNFlag == 1)
		{
		    muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-D1->未处理:ResultCode=%d,ResultInfo=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,iResultCode
		         ,strResultInfo.c_str());
		}
    }
    
    else
    {
    	conn->rollback();	
		muserlog("[%s]SN:%s,EparchyCode:%s,StartTime:%s,<-E->处理失败:ResultCode=%d,ResultInfo=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,strSN.c_str()
		         ,strEparchyCode.c_str()
		         ,strStartTime.c_str()
		         ,iResultCode
		         ,strResultInfo.c_str());
	}
	conn->terminateStatement(stmt);
	return 0;
}
int getSubscribeIDAndTradeID(string sEparchyCode,string& strSubscribeID,string& strTradeID)
{
	string sqlStmt = "select f_sys_getseqid(:eparchycode1,:seqname1),f_sys_getseqid(:eparchycode2,:seqname2) from dual";
	ResultSet *rset=0;
	Statement *stmt1;
	Connection *conn=GetConnect(sEparchyCode);
	try
	{
		stmt1 = conn->createStatement(sqlStmt);
		stmt1->setString(1,sEparchyCode);
		stmt1->setString(2,"seq_order_id");
		stmt1->setString(3,sEparchyCode);
		stmt1->setString(4,"seq_trade_id");
		rset = stmt1->executeQuery();
		int inum=0;
		while (rset->next())
		{
		  	strSubscribeID = rset->getString(1);
		  	strTradeID = rset->getString(2);
		  	inum++;
			break;
		}
		stmt1->closeResultSet(rset);
		conn->terminateStatement(stmt1);
		return 0;
	}
	catch(SQLException &ex)
	{
		muserlog("[%s]EparchyCode:%s,<-F->获取订单标识，流水号失败:ErrorCode=%d,ErrorDesc=%s"
		         ,GetHostDate("%Y-%m-%d %H:%M:%S").c_str()
		         ,sEparchyCode.c_str()
		         ,ex.getErrorCode()
		         ,ex.getMessage().c_str());
		conn->terminateStatement(stmt1);         
		return -1;
	}
	
}
int ParseSNStarTime(char * src, string &strSN, string &strEparchyCode, string &strStartTime )
{
	int iLen=strlen(src);
	src[iLen - 1]=',';
	
	char dest[40]="\0";

	memset(dest,0,40);
	GetSubStr(src,1,',',dest);
	strSN=dest;
	
	memset(dest,0,40);
	GetSubStr(src,2,',',dest);
	strEparchyCode = dest;
	
	memset(dest,0,40);
	GetSubStr(src,3,',',dest);
	strStartTime = dest;
	
	memset(dest,0,40);
	GetSubStr(src,4,',',dest);
	strStartTime +=dest; 

	//muserlog("[%s]SN=%s,EparchyCode=%s,StartTime=%s",
	//			GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),
	//            strSN.c_str(),
	//            strEparchyCode.c_str(),
	//            strStartTime.c_str());
	return 0;
}
int DealData(string &AcctFileName)
{
	muserlog("\n[%s]开始处理文件(%s)......",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),AcctFileName.c_str());
	FILE * fp = fopen(AcctFileName.c_str(),"r");
	char LineStr[100]="\0";
	string strSN,strEparchyCode,strStartTime;
	int iCount=0;
	while (1)
	{
		//读取一行记录
		memset(LineStr,0,100);
		if (fgets(LineStr,100,fp) != NULL)
		{
			strSN="";strEparchyCode="";strStartTime="";
			//解析出电话号码和首次通话时间
			if (ParseSNStarTime(LineStr,strSN,strEparchyCode,strStartTime) == -1)
			{
				continue;
				//fclose(fp);
				//return -1;
			}
			//处理单条记录
			if (DealOneRecord(strSN, strEparchyCode, strStartTime) == -1)
			{
				fclose(fp);
				return -1;
			}
			iCount++;
		}
		else
		{
			break;
		}
	}
	fclose(fp);
	muserlog("[%s]文件处理完毕,共[%d]条记录......\n",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),iCount);
	return 0;
}


//应统一密码管理要求，修改数据库连接信息获取方式  modified by qiumb@2007-11-02
int GetConnectInfo(int ino ,string &user,string &passwd,string &sid)
{
	string strErrMsg;
	
	string strDBCode;
	string strDBUserName;
	DBPass dbpass;
	DBPassOper* dbPassOper = NULL;
	
	try
	{
		dbPassOper = DBPassOper::getOper("DB");
	
		if (ino == 1)
		{
			if (ReadIniFileInfo(IniFileName.c_str(),"con1","DBCode",strDBCode,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if (ReadIniFileInfo(IniFileName.c_str(),"con1","DBUserName",strDBUserName,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if(dbPassOper->GetDBPass(dbpass, strDBCode, strDBUserName))
			{
				user = strDBUserName;
				passwd = dbpass.DecPasswd;
				sid = dbpass.ServerName;
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息成功。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
			}
			else
			{
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息时失败。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
		}
		if (ino == 2)
		{
			if (ReadIniFileInfo(IniFileName.c_str(),"con2","DBCode",strDBCode,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if (ReadIniFileInfo(IniFileName.c_str(),"con2","DBUserName",strDBUserName,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if(dbPassOper->GetDBPass(dbpass, strDBCode, strDBUserName))
			{
				user = strDBUserName;
				passwd = dbpass.DecPasswd;
				sid = dbpass.ServerName;
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息成功。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
			}
			else
			{
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息时失败。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
		}
		if (ino == 3)
		{
			if (ReadIniFileInfo(IniFileName.c_str(),"con3","DBCode",strDBCode,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if (ReadIniFileInfo(IniFileName.c_str(),"con3","DBUserName",strDBUserName,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if(dbPassOper->GetDBPass(dbpass, strDBCode, strDBUserName))
			{
				user = strDBUserName;
				passwd = dbpass.DecPasswd;
				sid = dbpass.ServerName;
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息成功。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
			}
			else
			{
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息时失败。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
		}
		if (ino == 4)
		{
			if (ReadIniFileInfo(IniFileName.c_str(),"con4","DBCode",strDBCode,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if (ReadIniFileInfo(IniFileName.c_str(),"con4","DBUserName",strDBUserName,strErrMsg) == -1){
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
			
			if(dbPassOper->GetDBPass(dbpass, strDBCode, strDBUserName))
			{
				user = strDBUserName;
				passwd = dbpass.DecPasswd;
				sid = dbpass.ServerName;
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息成功。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
			}
			else
			{
				strErrMsg = "统一密码管理：获取DBCode[" + strDBCode + "] DBUserName[" + strDBUserName + "]的数据库连接信息时失败。";
				muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
				return -1;
			}
		}
		delete dbPassOper;
		dbPassOper = NULL;
	}
	catch(exception &e)
	{
		if( dbPassOper != NULL ) delete dbPassOper;
		return -1;
	}
	return 0;
}

int ConnectToOracle()
{
	string user1,password1,sid1;
	string user2,password2,sid2;
	string user3,password3,sid3;
	string user4,password4,sid4;
	try
	{
		env = Environment::createEnvironment(Environment::DEFAULT);
		muserlog("\n[%s]成功创建OCCI环境变量!",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());
		
		user1 = GetEnv("szUser1");
		password1 = GetEnv("szPswd1");
		sid1 = GetEnv("szServ1");
		user2 = GetEnv("szUser2");
		password2 = GetEnv("szPswd2");
		sid2 = GetEnv("szServ2");
		user3 = GetEnv("szUser3");
		password3 = GetEnv("szPswd3");
		sid3 = GetEnv("szServ3");
		user4 = GetEnv("szUser4");
		password4 = GetEnv("szPswd4");
		sid4 = GetEnv("szServ4");
		//if (GetConnectInfo(1 ,user,password,sid) == -1) return -1;	    
	    conn_1 = env->createConnection(user1.c_str(),password1.c_str(),sid1.c_str());
		muserlog("[%s]连接1创建成功:%s******@%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),user1.c_str(),sid1.c_str());
		
		//if (GetConnectInfo(2 ,user,password,sid) == -1) return -1;
	    conn_2 = env->createConnection(user2.c_str(),password2.c_str(),sid2.c_str());
		muserlog("[%s]连接2创建成功:%s******@%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),user2.c_str(),sid2.c_str());

		//if (GetConnectInfo(3 ,user,password,sid) == -1) return -1;
	    conn_3 = env->createConnection(user3.c_str(),password3.c_str(),sid3.c_str());
		muserlog("[%s]连接3创建成功:%s******@%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),user3.c_str(),sid3.c_str());

		//if (GetConnectInfo(4,user,password,sid) == -1) return -1;
	    conn_4 = env->createConnection(user4.c_str(),password4.c_str(),sid4.c_str());
	    muserlog("[%s]连接4创建成功:%s******@%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),user4.c_str(),sid4.c_str());

	    return 0;
	}
	catch (SQLException &ex)
	{
		muserlog("[%s]ConnectToOracle :ErrorCode=%d,ErrorMessage=%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),ex.getErrorCode(),ex.getMessage().c_str());
		DisconnectFromOracle();
		return -1;
	}
}

void DisconnectFromOracle()
{
    if (conn_1) env->terminateConnection (conn_1);
    if (conn_2) env->terminateConnection (conn_2);
    if (conn_3) env->terminateConnection (conn_3);
    if (conn_4) env->terminateConnection (conn_4);
    muserlog("[%s]断开所有oracle连接",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());
    
    if (env) Environment::terminateEnvironment(env);
    muserlog("[%s]注销OCCI环境变量\n",GetHostDate("%Y-%m-%d %H:%M:%S").c_str());
}
int GetConnEparchyCode()
{
		
	string strErrMsg;

	strConnEparchyCode1="";strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"con_eparchy_code","con1",strConnEparchyCode1,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	else
	{
	    muserlog("连接地州编码为 [%s] 数据库成功!",strConnEparchyCode1.c_str());
	}
		
	strConnEparchyCode2="";strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"con_eparchy_code","con2",strConnEparchyCode2,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	else
	{
	    muserlog("连接地州编码为 [%s] 数据库成功!",strConnEparchyCode2.c_str());
	}

	strConnEparchyCode3="";strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"con_eparchy_code","con3",strConnEparchyCode3,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	else
	{
	    muserlog("连接地州编码为 [%s] 数据库成功!",strConnEparchyCode3.c_str());
	}

	strConnEparchyCode4="";strErrMsg="";
	if (ReadIniFileInfo(IniFileName.c_str(),"con_eparchy_code","con4",strConnEparchyCode4,strErrMsg) == -1){
		muserlog("[%s]%s",GetHostDate("%Y-%m-%d %H:%M:%S").c_str(),strErrMsg.c_str());
		return -1;
	}
	else
	{
	    muserlog("连接地州编码为 [%s] 数据库成功!",strConnEparchyCode4.c_str());
	}
	
	return 0;
}
Connection * GetConnect(string &strEparchyCode)
{
	/*
		江苏
			
	*/
	if (strstr(strConnEparchyCode1.c_str(),strEparchyCode.c_str())!=NULL)
	{
		return conn_1;
    }

    if (strstr(strConnEparchyCode2.c_str(),strEparchyCode.c_str())!=NULL)
    {
		return conn_2;
    }
	if (strstr(strConnEparchyCode3.c_str(),strEparchyCode.c_str())!=NULL)
	{
		return conn_3;
    }
	if (strstr(strConnEparchyCode4.c_str(),strEparchyCode.c_str())!=NULL)
	{
		return conn_4;
    }
    else
    {
        muserlog("返回地州编码为[%s] 数据库连接失败!",strEparchyCode.c_str());
    }

	return NULL;
}
void GetLogFileName()
{
	LogFileName = LogFilePath + "/PreCardDeal_" +GetHostDate("%Y%m%d") + ".log";
}
int GetSysConfig()
{
	DataFilePath=GetEnv("ACCT_DATA_FILE_PATH");
	if (DataFilePath == ""){
		cout<<"请设置计费文件保存的路径:环境变量 ACCT_DATA_FILE_PATH"<<endl;
		return -1;
	}
	DataFileBakPath=GetEnv("ACCT_DATA_FILE_BAK_PATH");
	if (DataFileBakPath == ""){
		cout<<"请设置计费文件备份的路径:环境变量 ACCT_DATA_FILE_BAK_PATH"<<endl;
		return -1;
	}
	if (DataFilePath == DataFileBakPath){
		cout<<"计费文件存放路径:"<<DataFilePath<<"不能等于计费文件备份路径:"<<DataFileBakPath<<endl;
		return -1;
	}
	LogFilePath=GetEnv("PRE_CARD_DEAL_LOG_PATH");
	if (LogFilePath == ""){
		cout<<"请设置计费文件处理错误日志路径:环境变量 PRE_CARD_DEAL_LOG_PATH"<<endl;
		return -1;
	}
	if (file_exists(LogFilePath.c_str()) == false){
		cout<<"日志文件和配置文件保存路径:"<<LogFilePath<<"不存在!"<<endl;
		return -1;
	}
	
	GetLogFileName();//生成日志文件名称

	//设置配置ini文件名
	//IniFileName = LogFilePath + "/PreCardDeal.cfg";
	IniFileName = GetEnv("WORK_HOME") + "/etc/PreCardDeal.cfg";
	
	if (file_exists(IniFileName.c_str()) == false){
		cout<<"配置文件:"<<IniFileName<<"不存在!"<<endl;
		return -1;
	}

    //获取号码处理成功是否记录日志的标记
	if (GetLogSuccessSNFlag() == -1)
		return -1;
	
	//获取空闲时进程sleep的时间，单位(秒)
	if (GetSleepInterval() == -1)
		return -1;

    //获取每个连接对应哪些地市信息
    if (GetConnEparchyCode()==-1)
    	return -1;
	return 0;
}
