/*资料修改日志*/

#ifndef OPERLOG_H
#define OPERLOG_H

#include <string.h>
#include <stdio.h>
#include "mwci/mwci.h"
#include "OrgMError.h"
#include "OperLog.h"
#include "TdMOrgmanlogDAO.h"
#include "GetSeqID.h"

using namespace std;
using namespace UDBI;
using namespace SysMan;

class OperLog
{
	public:
		OperLog();
		~OperLog();
		int InsertOrgmanlog(CFmlBuf &inBuf);
		int InsertOrgmanlog(TdMOrgmanlog &T_TdMOrgmanlog);
	private:
		TdMOrgmanlogDAO * D_TdMOrgmanlogDAO;
};

#endif
