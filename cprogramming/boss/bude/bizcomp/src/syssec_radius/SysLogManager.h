/*记录登陆日志*/

#ifndef SysLogManager_H
#define SysLogManager_H

#include <string.h>
#include <stdio.h>
#include "mwci.h"
#include "udbi.h"
#include "GetSeqID.h"
#include "AuthSecuError.h"

using namespace std;
using namespace UDBI;

//JConnect * pcon;
extern "C" void SysOperLog(CFmlBuf &inBuf,CFmlBuf &outBuf);

//Add By Zhu_ZhiMin On : 2006-05-21 Remark : 增加员工登录相关日志记录
extern "C" void StaffLogManagerLog(CFmlBuf &inBuf,CFmlBuf &outBuf);

class SysLogManager
{
	public:
		SysLogManager();
		~SysLogManager();
		CFmlBuf SysOperLog();
		
		CFmlBuf StaffLogManagerLog();
		
		string str_OpTypeCode;
		string str_TradeStaffid;
		string str_SubSysCode;
		string str_InIP;
		string str_Remark;
		string str_OperedStaffId;
		string str_TradeDepartid;
		int iOperCount;
		string str_InputLogId;

	private:
		int SysDoInsert(string &strInsLogId);
		int SysDoUpdate();
		
		int StaffLogInsRightLog();
		int StaffOperUpdRightLog();
		
		int i_ResultCode;
		string str_ResultInfo;
		
		CFmlBuf B_TempBuf;
	public:
	    JConnect * pcon;
};
#endif
