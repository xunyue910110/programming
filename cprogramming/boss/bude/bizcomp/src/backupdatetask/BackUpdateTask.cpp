#include "BackUpdateTask.h"
#include "BackupdatetaskExt.h"

using namespace ParaDom;

extern "C" void E_BackUpdateTask(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 得到输入数据
    string vEparchyCode = InBuf.GetString("TRADE_EPARCHY_CODE");

    // ======================== 定义Dao   ========================
	BackupdatetaskDAO   daoBackupdatetask ;

    // ======================== 初始化Dao ========================
	daoBackupdatetask.Clear();

    // ======================== 执行Dao   ========================
    VBackupdatetask     vBackupdatetask;
    int iCount = 0;

    try
	{
    	daoBackupdatetask.SetParam(":VEPARCHY_CODE", vEparchyCode);

        iCount = daoBackupdatetask.jselect(vBackupdatetask, "SEL_ALL_BY_UPDATETAG");

        convert(vBackupdatetask, OutBuf);
    }
	catch(Exception &e)
	{
		OutBuf.SetInt("X_RESULTCODE", e.getCode());
		OutBuf.SetString("X_RESULTINFO", "得到后台更新任务失败: " + e.toString());
	}

    OutBuf.SetInt("X_RECORDNUM", iCount);

    return;
}
