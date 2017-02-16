#include "BackupdatetaskExt.h"

/**
 *  \fn void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Backupdatetask tmpObj, int fldOc)
 *  Backupdatetask 类的转换函数，从CFmlBuf读取字段值赋给相应成员
 */
void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Backupdatetask &rBackupdatetask, int fldOc)
{
	if(tmpBuf.IsFldExist("UPDATE_ID", fldOc)) 
		rBackupdatetask.updateId = tmpBuf.GetString("UPDATE_ID", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_DLL_NAME", fldOc)) 
		rBackupdatetask.updateDllName = tmpBuf.GetString("UPDATE_DLL_NAME", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_INTERVAL", fldOc)) 
		rBackupdatetask.updateInterval = tmpBuf.GetInt("UPDATE_INTERVAL", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_PRI", fldOc)) 
		rBackupdatetask.updatePri = tmpBuf.GetInt("UPDATE_PRI", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_TAG", fldOc)) 
		rBackupdatetask.updateTag = tmpBuf.GetString("UPDATE_TAG", fldOc); 
	if(tmpBuf.IsFldExist("REMARK", fldOc)) 
		rBackupdatetask.remark = tmpBuf.GetString("REMARK", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_TIME", fldOc)) 
		rBackupdatetask.updateTime = tmpBuf.GetString("UPDATE_TIME", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_STAFF_ID", fldOc)) 
		rBackupdatetask.updateStaffId = tmpBuf.GetString("UPDATE_STAFF_ID", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_DEPART_ID", fldOc)) 
		rBackupdatetask.updateDepartId = tmpBuf.GetString("UPDATE_DEPART_ID", fldOc); 
	if(tmpBuf.IsFldExist("EPARCHY_CODE", fldOc)) 
		rBackupdatetask.eparchyCode = tmpBuf.GetString("EPARCHY_CODE", fldOc); 
}

/**
 *  \fn void ParaDom::PutToFmlBuf(Backupdatetask tmpObj, CFmlBuf &tmpBuf, int fldOc)
 *  Backupdatetask 类的转换函数，字段值填入CFmlBuf
 */
void ParaDom::PutToFmlBuf(Backupdatetask &rBackupdatetask, CFmlBuf &tmpBuf, int fldOc)
{
	tmpBuf.SetString("UPDATE_ID", rBackupdatetask.updateId, fldOc);
	tmpBuf.SetString("UPDATE_DLL_NAME", rBackupdatetask.updateDllName, fldOc);
	tmpBuf.SetInt("UPDATE_INTERVAL", rBackupdatetask.updateInterval, fldOc);
	tmpBuf.SetInt("UPDATE_PRI", rBackupdatetask.updatePri, fldOc);
	tmpBuf.SetString("UPDATE_TAG", rBackupdatetask.updateTag, fldOc);
	tmpBuf.SetString("REMARK", rBackupdatetask.remark, fldOc);
	tmpBuf.SetString("UPDATE_TIME", rBackupdatetask.updateTime, fldOc);
	tmpBuf.SetString("UPDATE_STAFF_ID", rBackupdatetask.updateStaffId, fldOc);
	tmpBuf.SetString("UPDATE_DEPART_ID", rBackupdatetask.updateDepartId, fldOc);
	tmpBuf.SetString("EPARCHY_CODE", rBackupdatetask.eparchyCode, fldOc);
}

/** \fn void ParaDom::convert(CFmlBuf &tmpBuf, VBackupdatetask &vrBackupdatetask)
 *  从CFmlBuf从获取实体对象字段值转换成实体对象vector结果返回
 *  @return
 */
void ParaDom::convert(CFmlBuf &tmpBuf, VBackupdatetask &vrBackupdatetask)
{
	int RecordCount;

    RecordCount = tmpBuf.GetCount("UPDATE_ID");

    for(int i=0; i<RecordCount; i++)
    {
    	Backupdatetask  tmpBackupdatetask;

		GetFromFmlBuf(tmpBuf, tmpBackupdatetask, i);
		vrBackupdatetask.push_back(tmpBackupdatetask);
    }

}

/** \fn void ParaDom::convert(VBackupdatetask &vrBackupdatetask, CFmlBuf &tmpBuf)
 *  从实体对象vector转换成CFmlBuf返回
 *  @return
 */
void ParaDom::convert(VBackupdatetask &vrBackupdatetask, CFmlBuf &tmpBuf)
{
	int i=0;

    vector<Backupdatetask>::iterator it = vrBackupdatetask.begin();
    for (i = 0 ; it != vrBackupdatetask.end(); it++ , i++)
    {
    	PutToFmlBuf((*it), tmpBuf, i);
    }

	int count = 0;
	if (tmpBuf.IsFldExist("X_RECORDNUM"))
		count = tmpBuf.GetInt("X_RECORDNUM");
		
    tmpBuf.SetInt("X_RECORDNUM", (i > count ? i:count));
}
