#include "SysLogManager.h"

using namespace std;
using namespace UDBI;

/********************************************************************
*函数：		SysOperLog(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		提供给流程管理系统接口函数
*返回：		FML域
********************************************************************/
extern "C" void SysOperLog(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	SysLogManager * TSysLogManager = new SysLogManager();
	try
	{	
		TSysLogManager->str_OpTypeCode = inBuf.GetString("OPER_TYPE_CODE");
		TSysLogManager->str_TradeStaffid = inBuf.GetString("TRADE_STAFF_ID");
		TSysLogManager->str_SubSysCode = inBuf.GetString("SUBSYS_CODE");
		TSysLogManager->str_InIP = inBuf.GetString("IN_IP");
		TSysLogManager->str_Remark = inBuf.GetString("REMARK");
		
		//Modified by Zhu_ZhiMin On : 2006-05-26 Remark : 增加字段判断，否则影响网上营业厅业务
		if (inBuf.IsFldExist("LOG_ID"))
		    TSysLogManager->str_InputLogId = inBuf.GetString("LOG_ID");
		else
		    TSysLogManager->str_InputLogId = "";
		    
		outBuf = TSysLogManager->SysOperLog();
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","记录日志业务处理时资料获取出错:"+ex.toString());
	}
	delete TSysLogManager;
	return;
}

/********************************************************************
*函数：     StaffLogManagerLog(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：     提供给登录系统相关接口函数（员工解锁，登录锁定等）
*返回：     FML域
*作者：     Zhu_ZhiMin On : 2006-05-21
********************************************************************/
extern "C" void StaffLogManagerLog(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	SysLogManager * TSysStaffLogManager = new SysLogManager();
	try
	{	
		TSysStaffLogManager->str_OpTypeCode = inBuf.GetString("OPER_TYPE_CODE");
		TSysStaffLogManager->str_TradeStaffid = inBuf.GetString("TRADE_STAFF_ID");
		TSysStaffLogManager->str_OperedStaffId = inBuf.GetString("OPERED_STAFF_ID");
		TSysStaffLogManager->str_TradeDepartid = inBuf.GetString("TRADE_DEPART_ID");
		TSysStaffLogManager->str_Remark = inBuf.GetString("REMARK");
		outBuf = TSysStaffLogManager->StaffLogManagerLog();
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","记录登录日志业务出错:"+ex.toString());
	}
	delete TSysStaffLogManager;
	return;
}
/********************************************************************
*函数：		SysLogManager()
*功能：		构造函数
********************************************************************/
SysLogManager::SysLogManager()
{
	pcon = jdb::getConnect();
	i_ResultCode = -1;
}
/******************str_ResultInfo**************************************************
*函数：		~SysLogManager()
*功能：		析构函数
********************************************************************/
SysLogManager::~SysLogManager()
{
	
}
/********************************************************************
*函数：		SysOperLog()
*功能：		记录登陆日志控制函数
*参数：		str_OpTypeCode：	0登陆，1退出登陆
*返回：		FML域
*修改：     Modified By Zhu_ZhiMin On : 2006-05-25 Remark : 增加流水号返回和按照流水号更新功能
********************************************************************/
CFmlBuf SysLogManager::SysOperLog()
{
	int tempResult = -1;
	string strInsLogId = "";
	
	if(str_OpTypeCode == "0")
	{
		try
		{
			tempResult = SysDoInsert(strInsLogId);
		}
		catch(Exception &e)
		{
			THROW_AC(BaseException,e,SYSLOG_ERR,"登陆日志记录失败：");
		}
	}
	else if(str_OpTypeCode == "1")
	{
		try
		{
			tempResult = SysDoUpdate();
		}
		catch(Exception &e)
		{
			THROW_AC(BaseException,e,SYSLOG_ERR,"退出登陆日志记录失败：");
		}
	}
	i_ResultCode = tempResult;
	if(i_ResultCode==0)
	{
		str_ResultInfo = "记录相应的系统日志成功！"+str_ResultInfo;
	}
	
	B_TempBuf.SetString("LOG_ID", strInsLogId);
	B_TempBuf.SetInt("X_RESULTCODE",i_ResultCode);
	B_TempBuf.SetString("X_RESULTINFO",str_ResultInfo);
	
	return B_TempBuf;
}

/********************************************************************
*函数：     StaffLogManagerLog()
*功能：     记录登陆日志控制函数
*参数：     str_OpTypeCode：	0插入，1更新
*返回：     FML域
*作者：     Zhu_ZhiMin On : 2006-05-21
********************************************************************/
CFmlBuf SysLogManager::StaffLogManagerLog()
{
	int tempResult = -1;
	if(str_OpTypeCode == "0")
	{
		try
		{
			tempResult = StaffLogInsRightLog();
		}
		catch(Exception &e)
		{
			THROW_AC(BaseException,e,SYSLOG_ERR,"操作日志插入失败：");
		}
	}
	else if(str_OpTypeCode == "1")
	{
		try
		{
			tempResult = StaffOperUpdRightLog();
		}
		catch(Exception &e)
		{
			THROW_AC(BaseException,e,SYSLOG_ERR,"操作日志更新失败：");
		}
	}
	i_ResultCode = tempResult;
	if(i_ResultCode==0)
	{
		str_ResultInfo = "记录相应的操作日志成功！"+str_ResultInfo;
	}
	B_TempBuf.SetInt("X_RESULTCODE",i_ResultCode);
	B_TempBuf.SetString("X_RESULTINFO",str_ResultInfo);
	
	return B_TempBuf;
}
/********************************************************************
*函数：		SysDoInsert()
*功能：		员工登陆日志插入记录
*参数：		str_OpTypeCode＝0
*参数：     strInsLogId保存返回的流水号
*返回：		返回插入结果
********************************************************************/
int SysLogManager::SysDoInsert(string &strInsLogId)
{
	string str_SeqNumber;
	GetSeqID * TGetSeqID = new GetSeqID("LOG_ID","8");
	str_SeqNumber = TGetSeqID->GetDateSequencesId();
	if(str_SeqNumber == "GetSeqFail")
	{
		str_ResultInfo = "获取Logid失败！";
		delete TGetSeqID;
		return -1;
	}
	else
	{
		delete TGetSeqID;
	}
	
	//Add By Zhu_ZhiMin On : 2006-05-25 Remark :增加流水号的返回，前台退出只更新本条记录
	strInsLogId = str_SeqNumber;
	
	JTable * TSysDoInsert = pcon->CreateJTable();
	try
	{
    	TSysDoInsert->AddSql("INSERT INTO TL_M_STAFFLOG(LOG_ID,STAFF_ID,IN_TIME,SUBSYS_CODE,IN_IP,REMARK) VALUES(:1,:2,SYSDATE,:3,:inip,:remark)");
    	TSysDoInsert->SetParam(":1",str_SeqNumber);
    	TSysDoInsert->SetParam(":2",str_TradeStaffid);
    	TSysDoInsert->SetParam(":3",str_SubSysCode);
    	TSysDoInsert->SetParam(":inip",str_InIP);
    	TSysDoInsert->SetParam(":remark",str_Remark);
    	TSysDoInsert->ExecInsert();
    }
    catch(Exception &ex)
	{
	    delete TSysDoInsert;
		THROW_AC(BaseException,ex,SYSLOG_ERR,"插入员工日志异常" + ex.toString());
	}
	delete TSysDoInsert;
	return 0;  
}
/********************************************************************
*函数：		StaffLogInsRightLog()
*功能：		员工操作日志插入记录
*参数：		str_OpTypeCode＝0
*返回：		返回插入结果
*作者：     Zhu_ZhiMin On : 2006-05-22
********************************************************************/
int SysLogManager::StaffLogInsRightLog()
{
//	string str_SeqNumber;
//	GetSeqID * TGetSeqID = new GetSeqID("LOG_ID","8");
//	str_SeqNumber = TGetSeqID->GetDateSequencesId();
//	if(str_SeqNumber == "GetSeqFail")
//	{
//		str_ResultInfo = "获取Logid失败！";
//		delete TGetSeqID;
//		return -1;
//	}
//	else
//	{
//		delete TGetSeqID;
//	}
//	JTable * TStaffInsertLog = pcon->CreateJTable();
//	try
//	{
//    	TStaffInsertLog->AddSql("INSERT INTO tl_m_rightlog(RIGHT_CODE,OPERED_STAFF_ID,RIGHT_ATTR,LOG_ID, ");
//		TStaffInsertLog->AddSql("OPER_TYPE,RIGHT_TYPE,ROLE_CODE,AREA_CODE, ");
//		TStaffInsertLog->AddSql("OPER_STAFF_ID,OPER_DEPART_ID,OPER_DATE,RIGHT_CLASS, ");
//		TStaffInsertLog->AddSql("OPER_SPECIAL,EXTEND_VALUE1,EXTEND_VALUE2, ");
//		TStaffInsertLog->AddSql("RSVALUE1,RSVALUE2,USE_TAG,TIMES,START_DATE,END_DATE, ");
//		TStaffInsertLog->AddSql("RSRV_STR1,RSRV_STR2,RSRV_STR3,RSRV_STR4,RSRV_STR5,REMARK ) ");
//    	
//    	TStaffInsertLog->AddSql("VALUES('STAFF_UNLOG_FLAG', :1, '0', :2, ");
//		TStaffInsertLog->AddSql(" '0', '7000', '', '', ");
//		TStaffInsertLog->AddSql(" :3, :4, SYSDATE, '', ");
//		TStaffInsertLog->AddSql(" '', '', 0, ");
//		TStaffInsertLog->AddSql(" '', '', '2', :5, SYSDATE, SYSDATE+ 5000, ");
//		TStaffInsertLog->AddSql(" '', '', '', '', '', '[员工登录多次失败锁定]' ) ");
//		
//    	TStaffInsertLog->SetParam(":1", str_TradeStaffid);
//    	TStaffInsertLog->SetParam(":2", str_SeqNumber);
//    	TStaffInsertLog->SetParam(":3", str_TradeStaffid);
//    	TStaffInsertLog->SetParam(":4", str_TradeDepartid);
//    	TStaffInsertLog->SetParam(":5", iOperCount);
//    	TStaffInsertLog->ExecInsert();
//    	
//    }
//    catch(Exception &ex)
//	{
//	    delete TStaffInsertLog;
//		THROW_AC(BaseException,ex,SYSLOG_ERR,"插入员工日志异常" + ex.toString());
//	}
//	delete TStaffInsertLog;
	return 0;  
}
/********************************************************************
*函数：		SysDoUpdate()
*功能：		员工登陆日志修改记录（退出登陆）
*参数：		str_OpTypeCode＝1
*返回：		返回修改结果
********************************************************************/
int SysLogManager::SysDoUpdate()
{
	JTable * TSysDoUpdate = pcon->CreateJTable();
	try
	{
    	//Modified By Zhu_ZhiMin On : 2006-05-25 
    	//Remark : 修改退出更新机制，按照流水号更新，否则一个应用退出，将更新完其它未退出的应用时间
    	//Modified By Zhu_ZhiMin On : 2006-05-26 
    	//Remark : 判断流水号是否为空
    	
    	if (str_InputLogId == "")
    	{
        	TSysDoUpdate->AddSql("UPDATE TL_M_STAFFLOG SET OUT_TIME = SYSDATE,REMARK = REMARK||:1  WHERE STAFF_ID =:2 AND SUBSYS_CODE =:3 AND IN_IP =:4 AND OUT_TIME IS NULL");
        	TSysDoUpdate->SetParam(":1",str_Remark);
        	TSysDoUpdate->SetParam(":2",str_TradeStaffid);
        	TSysDoUpdate->SetParam(":3",str_SubSysCode);
        	TSysDoUpdate->SetParam(":4",str_InIP);
    	}
    	else
    	{
        	TSysDoUpdate->AddSql("UPDATE TL_M_STAFFLOG SET OUT_TIME = SYSDATE,REMARK = REMARK||:1  WHERE LOG_ID =:2");
        	TSysDoUpdate->SetParam(":1",str_Remark);
        	TSysDoUpdate->SetParam(":2",str_InputLogId);
        }
        
    	TSysDoUpdate->ExecUpdate();
    }
    catch(Exception &ex)
	{
	    delete TSysDoUpdate;
		THROW_AC(BaseException,ex,SYSLOG_ERR,"修改员工退出时间异常" + ex.toString());
	}
	delete TSysDoUpdate;
	return 0;  
}

/********************************************************************
*函数：		StaffOperUpdRightLog()
*功能：		员工登陆日志更新记录
*参数：		str_OpTypeCode＝1
*返回：		返回修改结果
*作者：     Zhu_ZhiMin On : 2006-05-22
********************************************************************/
int SysLogManager::StaffOperUpdRightLog()
{
//	JTable * TStaffUpdateLog = pcon->CreateJTable();
//	try
//	{
//    	TStaffUpdateLog->AddSql("UPDATE tl_m_rightlog SET use_tag = '3', oper_staff_id=:2, ");
//    	TStaffUpdateLog->AddSql("oper_depart_id=:3,end_date=SYSDATE, remark=remark||[解锁] ");
//    	TStaffUpdateLog->AddSql(" WHERE right_code ='STAFF_UNLOG_FLAG' AND opered_staff_id=:1 ");
//        TStaffUpdateLog->AddSql("   AND right_attr='0' AND right_type='7000' AND use_tag='2' ");
//    	
//    	TStaffUpdateLog->SetParam(":1",str_OperedStaffId);
//    	TStaffUpdateLog->SetParam(":2",str_TradeStaffid);
//    	TStaffUpdateLog->SetParam(":3",str_TradeDepartid);
//    	TStaffUpdateLog->ExecUpdate();
//    }
//    catch(Exception &ex)
//	{
//	    delete TStaffUpdateLog;
//		THROW_AC(BaseException,ex,SYSLOG_ERR,"修改员工操作信息异常" + ex.toString());
//	}
//	delete TStaffUpdateLog;
	return 0;  
}
