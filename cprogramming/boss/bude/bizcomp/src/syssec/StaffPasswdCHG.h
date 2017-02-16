/*修改员工密码*/

#ifndef StaffPasswdCHG_H
#define StaffPasswdCHG_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "mwci.h"
#include "udbi.h"
#include "AuthSecuError.h"
#include "GetSeqID.h"

using namespace std;
using namespace UDBI;

//JConnect * pcon;
extern "C" void StaffPasswdChange(CFmlBuf &inBuf,CFmlBuf &outBuf);

class StaffPasswdCHG
{
	public:
		StaffPasswdCHG(const string staffid,const string passwd);
		~StaffPasswdCHG();
		CFmlBuf StaffPasswdChange();

	private:
		string str_TradeStaffid;
		string str_TradePasswd;
		
		int i_ResultCode;
		string str_ResultInfo;
		
		CFmlBuf B_TempBuf;
    public:
        JConnect * pcon;
};
#endif
