/*获取员工权限*/

#ifndef StaffRight_H
#define StaffRight_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "mwci.h"
#include "udbi.h"
#include "AuthSecuError.h"

using namespace std;
using namespace UDBI;

//JConnect * pcon;
extern "C" int GetStaffRight(CFmlBuf &inBuf,CFmlBuf &outBuf);
extern "C" int SetTempRightTimes(CFmlBuf &inBuf,CFmlBuf &outBuf);
class StaffRight
{
	public:
		StaffRight();
		~StaffRight();
		CFmlBuf GetStaffRight(const int GetMode,const string TradeStaffid,const string SubSysCode,const string TradeEparchyCode);
		CFmlBuf SetTempRightTimes(const string TradeStaffid,const string RightCode);
		CFmlBuf SetTempRightTimes(const string TradeStaffid,const string DataCode,const string DataType);
	private:
		CFmlBuf StaffGUIMenuRight(JTable * TGetStaffRight);
		CFmlBuf StaffCommFuncRight(JTable * TGetStaffRight);
		CFmlBuf StaffSysFuncRight(JTable * TGetStaffRight);
		CFmlBuf StaffFeeDataRight(JTable * TGetStaffRight);
		CFmlBuf StaffSysDataRight(JTable * TGetStaffRight);
		CFmlBuf StaffTempFuncRight(JTable * TGetStaffRight);
		CFmlBuf StaffTempSysDateRight(JTable * TGetStaffRight);
		CFmlBuf	SysTagInfo(JTable * TGetStaffRight);
		CFmlBuf SubSysInfo(JTable * TGetStaffRight);

        CFmlBuf StaffQrgInfo(JTable * TGetStaffRight);
		
		int i_GetMode;
		string str_TradeStaffid;
		string str_TradeEparchyCode;
		string str_SubSysCode;
		CFmlBuf B_TempBuf;
			
		int i_ErrCode;
		string str_ErrInfo;
    public:
        JConnect * pcon;
};
#endif
