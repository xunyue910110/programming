/*地洲资料管理*/

#ifndef AREAMANAGER_H
#define AREAMANAGER_H

#include <string.h>
#include <stdio.h>
#include <mwci/mwci.h>
#include "OperLog.h"
#include "OrgMError.h"
//#include "common.h"
#include "OrgTdMAreaDAO.h"
#include "GetSeqID.h"


using namespace UDBI;
using namespace SysMan;

//JConnect * pcon;
extern "C" int GetAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);
extern "C" int SaveAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf);

class AreaManager
{
	public:
		AreaManager();
		~AreaManager();

		CFmlBuf GetAreaInfo(ORG_TdMArea &T_ORG_TdMArea,const int GetModeCode);
		int SaveAreaInfo(ORG_TdMArea &T_ORG_TdMArea,const int SaveModeCode);

	private:
		int InsertAreaInfo(ORG_TdMArea &T_ORG_TdMArea);
		int UpdateAreaInfo(ORG_TdMArea &T_ORG_TdMArea);
		int DeleteAreaInfo(ORG_TdMArea &T_ORG_TdMArea);

		ORG_TdMAreaDAO * D_ORG_TdMAreaDAO;
		
	public:
        JConnect * pcon;
};

#endif
