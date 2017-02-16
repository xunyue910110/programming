#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <atmi.h>	/* TUXEDO Header File */
#include <userlog.h>	/* TUXEDO Header File */
#include <CFmlBuf.h>
#include <fml32.h>
#include <fml1632.h>
#include <iostream>
#include <occi.h>
#include "log4cpp/Logger.h"

using namespace log4cpp;
using namespace oracle::occi;
using namespace std;

Environment *env=NULL;
Connection *conn=NULL;
Statement *stmt=NULL;

Logger& logger = Logger::getLogger("gboss.scom.namingsvc");
  
/**
	服务初始化.
	初始化数据库连接。
*/
extern "C"
int tpsvrinit(int argc, char *argv[])
{
	ifstream oTotalStream;
	string username,password,sid;

    LOG_TRACE(logger, "Welcome to the naming server");
	
	char *pFileName=getenv("NAMING_CONFIG");
	
	if (!pFileName)
	{
		logger.alert("Fail to get environment NAMING_CONFIG. Config file with DB USERNAME PASSWORD SID must be in the file.");
		return(-1);
	}

  	oTotalStream.open(pFileName, ios::in);

	if (oTotalStream.rdstate() != ios::goodbit)
	{
		oTotalStream.close();
		logger.alert("Fail to open config file specify by NAMING_CONFIG (%s). ",pFileName);
		return(-1);
	}
	
	oTotalStream>>username>>password>>sid;

	oTotalStream.close();
	logger.info("username:(%s);password(******);sid:(%s)",username.c_str(),sid.c_str());
	try
	{
		env = Environment::createEnvironment (Environment::DEFAULT);
    	conn = env->createConnection (username.c_str(), password.c_str(), sid.c_str());
    }
	catch(SQLException &ex)
	{
		logger.alert("SQLException:[%d]|[%s] ",ex.getErrorCode(),ex.getMessage().c_str());
     	return(-1);
	}

	return(0);
}

/**
	断开服务器的连接.
*/
extern "C"
void tpsvrdone()
{
	try
	{
		if(env)
		{	
			env->terminateConnection (conn);
    		Environment::terminateEnvironment (env);
    	}
    }catch(...)
    {
    	//do nothing...
    }
}


/**
	根据逻辑名，查找名字服务信息
	输入缓冲区要求有CONTEXTID、LOGICALNAME字段
	返回缓冲区中有名字服务的信息
*/
extern "C"
void
LOOK_UP(TPSVCINFO *rqst)
{
	CFmlBuf *InBuf,*outBuf;
	FBFR   *outB;	/* fielded buffer of decoded message */
	string logicname;
	long contextid;
	int count=0;
	

	ResultSet *rset;
	try{

		InBuf=new CFmlBuf((FBFR *)rqst->data);
		outBuf=new CFmlBuf();
	
		//get context ID and Logic Name
		contextid=InBuf->GetInt("X_CONTEXT_ID");
		logicname=InBuf->GetStr("X_LOGICAL_NAME");
		
    	LOG_DEBUG_P2(logger, "LOOK_UP:[%d]|[%s]",contextid,logicname.c_str());
		
		//get Naming from database Td_m_Naming
		string sqlStmt = "SELECT machine_ip,machine_port,machine_name, "\
			"service_name,service_type_code,to_char(redeploy_time,'YYYYMMDDHH24MISS') "\
			 			 "FROM td_m_naming t " \
			 			 "WHERE CONTEXT_ID = :1 " \
			 			 "  AND LOGICAL_NAME = :2 ";
	    stmt = conn->createStatement (sqlStmt);
 
   	 	stmt->setInt(1,contextid);
    	stmt->setString(2,logicname);
    
   	 	rset = stmt->executeQuery ();

	    while (rset->next ())
   	 	{
			outBuf->SetString( "X_MACHINE_IP",   rset->getString(1),count);
			outBuf->SetString( "X_MACHINE_PORT", rset->getString(2),count);
			outBuf->SetString( "X_MACHINE_NAME", rset->getString(3),count);
			outBuf->SetString( "X_SERVICE_NAME", rset->getString(4),count);
			outBuf->SetString( "X_SERVICE_TYPE_CODE",   rset->getString(5),count);
			outBuf->SetString( "X_REDEPLOY_TIME",rset->getString(6),count);
			count++;
	    }
	   	stmt->closeResultSet (rset);
    	conn->terminateStatement (stmt);	
    	
    	//释放内存，只在tuxedo服务内部需要
    	InBuf->SetData();
    	outB=(FBFR*)outBuf->GetData();
		LOG_DEBUG_P1(logger,"LOOK_UP:::::::::::::\n%s",outBuf->ToString().c_str());
    	outBuf->SetData();
    	delete InBuf;
    	delete outBuf;
    	
    }catch(SQLException ex)
    {
		logger.alert("SQLException:[%d]|[%s] ",ex.getErrorCode(),ex.getMessage().c_str());
		if(InBuf)
			delete InBuf;
		if(outBuf)
			delete outBuf;
			
     	tpreturn(TPFAIL, 0,0, 0L, 0);
    }catch(MWException ex)
    {
		logger.alert("MWException:[1%s] ",ex.getMessage().c_str());

		if(InBuf)
			delete InBuf;
		if(outBuf)
			delete outBuf;

		tpreturn(TPFAIL, 0,0, 0L, 0);
   }

	//return to client
	tpreturn(TPSUCCESS, 0,(char *)outB, 0L, 0);
}

/**
	输入缓冲区要求有CONTEXTID、LOGICALNAME字段
	返回缓冲区中有名字服务的信息
*/
extern "C"
void
LIST(TPSVCINFO *rqst)
{
	CFmlBuf *outBuf;
	FBFR  *outB;	/* fielded buffer of decoded message */
	string logicname;
	long iContextid;
	int count=0;
	
	ResultSet *rset;
	try{

		outBuf=new CFmlBuf(131072);

		//get Naming from database T_Naming
		string sqlStmt = "SELECT context_id,logical_name,machine_ip,machine_port,machine_name," \
			"service_name,service_type_code,to_char(redeploy_time,'YYYYMMDDHH24MISS') " \
		 	 "FROM td_m_naming t ";

	    stmt = conn->createStatement (sqlStmt);
   	 	rset = stmt->executeQuery ();

	    while (rset->next ())
   	 	{
    
   	   		outBuf->SetInt("X_CONTEXT_ID",   rset->getInt(1), count);
			outBuf->SetString("X_LOGICAL_NAME", rset->getString(2), count);
			outBuf->SetString("X_MACHINE_IP",   rset->getString(3), count);
			outBuf->SetString("X_MACHINE_PORT", rset->getString(4), count);
			outBuf->SetString("X_MACHINE_NAME", rset->getString(5), count);
			outBuf->SetString("X_SERVICE_NAME", rset->getString(6), count);
			outBuf->SetChar  ("X_SERVICE_TYPE_CODE",   *(rset->getString(7).c_str()), count);
			outBuf->SetString("X_REDEPLOY_TIME",rset->getString(8), count);
			count++;
	    }
	    
	    outBuf->SetInt("X_RESULTCODE",1234,0);
	    
	   	stmt->closeResultSet (rset);
    	conn->terminateStatement (stmt);
		
		LOG_DEBUG_P1(logger,"LIST:::::::::::::\n%s",outBuf->ToString().c_str());
		outB=(FBFR*)outBuf->GetData();
		outBuf->SetData();
		delete outBuf;
        
    }catch(SQLException ex)
    {
		logger.alert("SQLException:[%d]|[%s] ",ex.getErrorCode(),ex.getMessage().c_str());
		if(outBuf)
			delete outBuf;
     	tpreturn(TPFAIL, 0,0, 0L, 0);
    }catch(MWException ex)
    {
		logger.alert("MWException:[%s] ",ex.getMessage().c_str());
		if(outBuf)
			delete outBuf;
		tpreturn(TPFAIL, 0,0, 0L, 0);
   }
	//return to client
	tpreturn(TPSUCCESS, 0,(char *)outB, 0L, 0);
}

extern "C"
void BATCH_UPDATE(TPSVCINFO *rqst)
{
	char *ptheData;
	int aa=0;
	string temp;
	CFmlBuf *InBuf,*outBuf;
	
	string logicname;
	int contextid;
   	     
	try
	{
		InBuf=new CFmlBuf((FBFR *)rqst->data);
		aa=InBuf->GetCount("X_LOGICAL_NAME");
		cout<<"Total:"<<aa<<endl;

		//delete from DataBase with CONTEXTID and LOGICALNAME

		string sqlStmt = "DELETE FROM td_m_naming t " \
			 			 "WHERE CONTEXT_ID = :1 " \
			 			 "  AND LOGICAL_NAME = :2 ";
	    stmt = conn->createStatement (sqlStmt);
		
		for (int i=0;i<aa;i++)
		{
     	 	contextid=InBuf->GetInt("X_CONTEXT_ID",i);
     	 	logicname=InBuf->GetStr("X_LOGICAL_NAME",i);
     	 	stmt->setInt(1,contextid);
    		stmt->setString(2,logicname);
			stmt->executeUpdate();
		}

		conn->terminateStatement (stmt);

		//Insert New Records into DataBase
		sqlStmt ="INSERT INTO td_m_naming (context_id,logical_name,machine_ip, " \
		"machine_port,machine_name,service_name,service_type_code,redeploy_time) " \
			 	 "VALUES(:1,:2,:3,:4,:5,:6,:7,to_date(:8,'yyyymmddhh24miss')) ";
	    stmt = conn->createStatement (sqlStmt);
		for (int i=0;i<aa;i++)
		{
     	 	stmt->setInt(1,InBuf->GetInt("X_CONTEXT_ID",i));
    		stmt->setString(2,InBuf->GetStr("X_LOGICAL_NAME",i));
    		stmt->setString(3,InBuf->GetStr("X_MACHINE_IP",i));
    		stmt->setString(4,InBuf->GetStr("X_MACHINE_PORT",i));
    		stmt->setString(5,InBuf->GetStr("X_MACHINE_NAME",i));
    		stmt->setString(6,InBuf->GetStr("X_SERVICE_NAME",i));
    		temp=InBuf->GetChar("X_SERVICE_TYPE_CODE",i);
    		stmt->setString(7,temp);
    		stmt->setString(8,InBuf->GetStr("X_REDEPLOY_TIME",i));
			stmt->executeUpdate();
		}
		
		conn->terminateStatement (stmt);
		
	 	conn->commit();
				
		//release CFmlBuf InBuf
		InBuf->SetData();
		delete InBuf;
	}
	catch (SQLException ex)
	{
 		conn->rollback();
		logger.alert("[ROLLBACK]SQLException:[%d]|[%s] ",ex.getErrorCode(),ex.getMessage().c_str());
		if(InBuf)
			delete InBuf;
     	tpreturn(TPFAIL, 0,0, 0L, 0);

	}
	catch (MWException &ex)
	{
		logger.alert("[ROLLBACK]MWException:[%s] ",ex.getMessage().c_str());
		conn->rollback();
		if(InBuf)
			delete InBuf;
		tpreturn(TPFAIL, 0,0, 0L, 0);
	}
	//return to client
	tpreturn(TPSUCCESS, 0,0, 0L, 0);
}

extern "C"
void GET_UPDATED(TPSVCINFO *rqst)
{
	CFmlBuf *InBuf,*outBuf;	/* fielded buffer of decoded message */
	FBFR32 * outB;
	char *ptheData;
	string LocalTime;
	string logicname;
	bool IsAllData=false;		//Full Syn tag 
	int contextid;
	short int iTypeCode;
	int count=0;
	
	//get REDEPLOYTIME , if Not found ,all date is request
	try 
	{
		//New Buf
		outBuf= new CFmlBuf();
		InBuf= new CFmlBuf((FBFR *)rqst->data);
		if(InBuf->IsFldExist("X_REDEPLOY_TIME"))
			LocalTime=InBuf->GetStr("X_REDEPLOY_TIME");
		else
			IsAllData=true;

		ResultSet *rset;	
		//get context ID
		contextid=InBuf->GetInt("X_CONTEXT_ID");	

		//get Naming from database T_Naming
		string sqlStmt = 
		"SELECT logical_name,machine_ip,machine_port,machine_name, "
		" service_name,service_type_code,to_char(redeploy_time,'YYYYMMDDHH24MISS') "
		"FROM td_m_naming t " 
		"WHERE CONTEXT_ID = :1 " ;
 		 
		if(!IsAllData) 
    		sqlStmt+="  AND REDEPLOY_TIME > TO_DATE(:2,'YYYYMMDDHH24MISS') ";

	    stmt = conn->createStatement (sqlStmt);
 
   	 	stmt->setInt(1,contextid);
    	
    	if(!IsAllData)	stmt->setString(2,LocalTime);
    
   	 	rset = stmt->executeQuery ();

	    while (rset->next ())
   	 	{
   	     	outBuf->SetString("X_LOGICAL_NAME", rset->getString(1),count);
			outBuf->SetString("X_MACHINE_IP",   rset->getString(2),count);
			outBuf->SetString("X_MACHINE_PORT", rset->getString(3),count);
			outBuf->SetString("X_MACHINE_NAME", rset->getString(4),count);
			outBuf->SetString("X_SERVICE_NAME", rset->getString(5),count);
			outBuf->SetString("X_SERVICE_TYPE_CODE",   rset->getString(6),count);
			outBuf->SetString("X_REDEPLOY_TIME",rset->getString(7),count);
			count++;
	    }
	   	stmt->closeResultSet (rset);
    	conn->terminateStatement (stmt);
    	
		//清除零时变量
    	InBuf->SetData();
    	delete InBuf;
    	outB=(FBFR*)outBuf->GetData();
    	outBuf->SetData();
    	delete outBuf;
    }catch(SQLException ex)
    {
		logger.alert("SQLException:[%d]|[%s] ",ex.getErrorCode(),ex.getMessage().c_str());
		if(InBuf)
			delete InBuf;
		if(outBuf)
			delete outBuf;
     	tpreturn(TPFAIL, 0,0, 0L, 0);
    }catch(MWException ex)
    {
		logger.alert("MWException:[%s] ",ex.getMessage().c_str());
		if(InBuf)
			delete InBuf;
		if(outBuf)
			delete outBuf;
		tpreturn(TPFAIL, 0,0, 0L, 0);
   }
	//return to client
	tpreturn(TPSUCCESS, 0,(char *)outB, 0L, 0);

}
