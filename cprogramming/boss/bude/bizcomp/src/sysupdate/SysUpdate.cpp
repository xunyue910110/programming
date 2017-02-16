#include "SysUpdate.h"

#include "FtphostExt.h"
#include "UpdatemodExt.h"
#include "StaffUpdatemodLogExt.h"

using namespace ParaDom;

extern "C" void E_GetFtpHost(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 得到输入数据
    string vEparchyCode = InBuf.GetString("TRADE_EPARCHY_CODE");

    // 定义Dao
	FtphostDAO   daoFtphost ;

    // 初始化Dao
	daoFtphost.Clear();
    daoFtphost.SetParam(":VEPARCHY_CODE", vEparchyCode);

    int     iCount = 0;

    try
	{
        // 执行Dao
	    Ftphost eFtphost;
        iCount = daoFtphost.jselect(eFtphost, "SEL_PK_TD_S_FTPHOST");

        // 返回Dao数据
        PutToFmlBuf(eFtphost, OutBuf);
    }
	catch(Exception &e)
	{
		OutBuf.SetInt("X_RESULTCODE", e.getCode());
		OutBuf.SetString("X_RESULTINFO", "根据当前登录员工地州编码，得到Ftp参数失败: " + e.toString());
	}

    OutBuf.SetInt("X_RECORDNUM", iCount);

    return;
}

extern "C" void E_GetUpdateModInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 得到输入数据

    // 定义Dao
	UpdatemodDAO   daoUpdatemod ;

    // 初始化Dao
	daoUpdatemod.Clear();

    // 执行Dao
    VUpdatemod     vUpdatemod;
    int iCount = 0;

    try
	{
        iCount = daoUpdatemod.jselect(vUpdatemod, "SEL_BY_MOD_CODE");

        // 返回Dao数据
        convert(vUpdatemod, OutBuf);
    }
	catch(Exception &e)
	{
		OutBuf.SetInt("X_RESULTCODE", e.getCode());
		OutBuf.SetString("X_RESULTINFO", "得到更新模块信息失败: " + e.toString());
	}

    OutBuf.SetInt("X_RECORDNUM", iCount);

    return;
}

extern "C" void E_StaffUpdateModLog(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 得到输入数据
    string sTradeStaffId =  InBuf.GetString("TRADE_STAFF_ID");
    string strDepartId   =  InBuf.GetString("TRADE_DEPART_ID");

    // 定义Dao
    StaffUpdatemodLogDAO daoStaffUpdatemodLog;

    VStaffUpdatemodLog vStaffUpdatemodLog;
    StaffUpdatemodLog objStaffUpdatemodLog;

    convert(InBuf, vStaffUpdatemodLog);

    vector <StaffUpdatemodLog> :: iterator it = vStaffUpdatemodLog.begin();

    int iCount = 0;
    //cout << "LY========[" <<  iCount << "]========LY" << endl;
    for(; it != vStaffUpdatemodLog.end(); it++)
	{
	    //cout << "LY========[" <<  iCount << "]========LY" << endl;
        try
	    {
            objStaffUpdatemodLog = *it ;

            // 执行Dao
        	/**< 为INSERT参数字段传入条件值 */
        	daoStaffUpdatemodLog.SetParam(":VMOD_CODE", objStaffUpdatemodLog.modCode);
        	daoStaffUpdatemodLog.SetParam(":VMOD_VERSION", objStaffUpdatemodLog.modVersion);
        	daoStaffUpdatemodLog.SetParam(":VMOD_BUILDNO", objStaffUpdatemodLog.modBuildno);
            daoStaffUpdatemodLog.SetParam(":VMOD_SIZE", objStaffUpdatemodLog.modSize);

        	daoStaffUpdatemodLog.SetParam(":VMUST_UPDATE_TAG", objStaffUpdatemodLog.mustUpdateTag);
        	daoStaffUpdatemodLog.SetParam(":VREG_UPDATE_TAG", objStaffUpdatemodLog.regUpdateTag);
    	    daoStaffUpdatemodLog.SetParam(":VUPDATE_METHOD", objStaffUpdatemodLog.updateMethod);
            daoStaffUpdatemodLog.SetParam(":VDEL_TAG", objStaffUpdatemodLog.delTag);

        	daoStaffUpdatemodLog.SetParam(":VREMARK", "");
        	daoStaffUpdatemodLog.SetParam(":VUPDATE_STAFF_ID", sTradeStaffId);
        	daoStaffUpdatemodLog.SetParam(":VUPDATE_DEPART_ID", strDepartId);

            // 执行Dao
            iCount += daoStaffUpdatemodLog.jinsert("INSERT");
        }
    	catch(Exception &e)
    	{
    		OutBuf.SetInt("X_RESULTCODE", e.getCode());
    		OutBuf.SetString("X_RESULTINFO", "记录更新模块日志失败: " + e.toString());

    		break;
    	}
    }

    OutBuf.SetInt("X_RECORDNUM", iCount);

    return ;
}
