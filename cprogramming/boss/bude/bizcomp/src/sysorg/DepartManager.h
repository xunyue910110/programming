/*部门资料管理*/

#ifndef DEPARTMANAGER_H
#define DEPARTMANAGER_H

#include <string.h>
#include <stdio.h>
#include "mwci/mwci.h"
#include "OrgMError.h"
#include "OperLog.h"
//#include "common.h"
#include "OrgTdMDepartDAO.h"
#include "GetSeqID.h"

using namespace std;
using namespace UDBI;
using namespace SysMan;

//JConnect * pcon;
extern "C" int GetDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);
extern "C" int SaveDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);

class DepartManager
{
	public:
		DepartManager();
		~DepartManager();

		CFmlBuf GetDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart,const int GetModeCode);
		int SaveDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart,const int SaveModeCode);

	private:
		int InsertDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart);
		int UpdateDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart);
		int DeleteDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart);

		ORG_TdMDepartDAO 	* D_ORG_TdMDepartDAO;
	
	public:
        JConnect * pcon;

};

#endif
