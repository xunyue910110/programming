/*员工资料管理*/

#ifndef STAFFMANAGER_H
#define STAFFMANAGER_H

#include <string.h>
#include <stdio.h>
#include "mwci/mwci.h"
#include "OrgMError.h"
#include "OperLog.h"
//#include "common.h"
#include "OrgTdMStaffDAO.h"
#include "TfFCustomerDAO.h"
#include "GetSeqID.h"

using namespace std;
using namespace UDBI;
using namespace CustDom;

//JConnect * pcon;
extern "C" int GetStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);
extern "C" int SaveStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);

class StaffManager
{
	public:
		StaffManager();
		~StaffManager();

		CFmlBuf GetStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff,const int GetModeCode);
		int SaveStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff,const int SaveModeCode);

	private:
		int InsertStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff);
		int UpdateStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff);
		int DeleteStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff);

		ORG_TdMStaffDAO * D_ORG_TdMStaffDAO;
    public:
        JConnect * pcon;
};

#endif
