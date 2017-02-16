#include "OperLog.h"

/**********************************************************************************
*
*
*
**********************************************************************************/
OperLog::OperLog()
{
	D_TdMOrgmanlogDAO = new TdMOrgmanlogDAO();
}
/**********************************************************************************
*
*
*
**********************************************************************************/
OperLog::~OperLog()
{
	delete D_TdMOrgmanlogDAO;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int OperLog::InsertOrgmanlog(CFmlBuf &inBuf)
{
	TdMOrgmanlog TOrgmanlog; 
	long i = 0;
	
	inBuf.GetValueEx("TRADE_EPARCHY_CODE",(void *)&(TOrgmanlog.OperAreaCode),i,false);
	inBuf.GetValueEx("AREA_CODE",(void *)&(TOrgmanlog.StartValue1),i,false);
	inBuf.GetValueEx("AREA_NAME",(void *)&(TOrgmanlog.EndValue1),i,false);
	inBuf.GetValueEx("PARENT_AREA_CODE",(void *)&(TOrgmanlog.StartValue2),i,false);
	inBuf.GetValueEx("USER_AREA_CODE",(void *)&(TOrgmanlog.EndValue2),i,false);
	inBuf.GetValueEx("AREA_LEVEL",(void *)&(TOrgmanlog.StartValue3),i,false);
	inBuf.GetValueEx("END_DATE",(void *)&(TOrgmanlog.EndValue3),i,false);
	inBuf.GetValueEx("OPER_TYPE_CODE",(void *)&(TOrgmanlog.OpertypeCode),i,false);
	inBuf.GetValueEx("OPER_INFO",(void *)&(TOrgmanlog.OperInfo),i,false);
	inBuf.GetValueEx("REMARK",(void *)&(TOrgmanlog.Remark),i,false);
	inBuf.GetValueEx("TRADE_STAFF_ID",(void *)&(TOrgmanlog.OperStaffId),i,false);
	inBuf.GetValueEx("TRADE_DEPART_ID",(void *)&(TOrgmanlog.OperDepartId),i,false);
	
	TOrgmanlog.SubsysCode = "SMS";
	GetSeqID * T_GetSeqID = new GetSeqID("LOG_ID","8");
	try
	{
		TOrgmanlog.LogId = T_GetSeqID->GetDateSequencesId();
		if(TOrgmanlog.LogId == "GetSeqFail")
		{
			THROW_C(BaseException,OPERLOG_COM_1,"获取LOGID失败！");
		}
		
		InsertOrgmanlog(TOrgmanlog);
		delete T_GetSeqID;
	}
	catch(DAOException &e)
	{
	    delete T_GetSeqID;
		THROW_A(BaseException,e,"填入LOG相应日志");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int OperLog::InsertOrgmanlog(TdMOrgmanlog &T_TdMOrgmanlog)
{
	try
	{
		D_TdMOrgmanlogDAO->jinsert(T_TdMOrgmanlog);
	}
	catch(DAOException &e)
	{
		THROW_AC(DAOException,e,OPERLOG_DAO_I,"插入LOG信息出错！");
	}
	return 0;
}
