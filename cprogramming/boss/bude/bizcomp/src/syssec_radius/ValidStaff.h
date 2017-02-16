/*登陆认证*/

#ifndef ValidStaff_H
#define ValidStaff_H

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "udbi.h"
#include "mwci/CFmlBuf.h"
#include "AuthSecuError.h"
#include "NewAuthenClient.h"

using namespace std;
using namespace UDBI;

//JConnect * pcon;
extern "C" void CheckLogStaff(CFmlBuf &inBuf,CFmlBuf &outBuf);

class ValidStaff
{
	public:
		ValidStaff(const string login_staffid,const string login_password,const string login_eparchy,
		            const string str_DealFlag, const string str_Second_Passwd, 
		            const string str_Answer_State, const string str_Oper_Second);
		~ValidStaff();
		CFmlBuf CheckLogStaff();
        
        vector<string> v_IP;
	private:
		bool ValidIP();
		bool ValidStaffid();
		bool ValidDate();
		//bool ValidPasswd();
		int ValidPasswd();                                          //Modified By Zhu_ZhiMin On : 2006-05-19
		bool GetStaffInfo();

  		string str_staffid;
  		string str_passwd;

  		int i_ResultCode;
		string str_ResultInfo;
		string str_StaffName;
		string str_DepartId;
		string str_AreaCode;
		string str_EparchyCode;
		string str_DepartCode;
		string str_DepartName;
		string str_HintInfo;
		string str_SecFlag;
		string str_SecPasswd;
		string str_FirstReturnState;
		string str_OperSecondFlag;
		
		int readAuthenConfig(char*, char*, char*);
		void FoxTrimRight(char*);
		void getErrorMessage(int, char*);

		CFmlBuf B_TemBuf;
	public:
	    JConnect * pcon;
};
#endif
