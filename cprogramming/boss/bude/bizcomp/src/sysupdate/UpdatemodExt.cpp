#include "UpdatemodExt.h"

/**
 *  \fn void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Updatemod tmpObj, int fldOc)
 *  Updatemod 类的转换函数，从CFmlBuf读取字段值赋给相应成员
 */
void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Updatemod &rUpdatemod, int fldOc)
{
	if(tmpBuf.IsFldExist("MOD_CODE", fldOc)) 
		rUpdatemod.modCode = tmpBuf.GetString("MOD_CODE", fldOc); 
	if(tmpBuf.IsFldExist("MOD_VERSION", fldOc)) 
		rUpdatemod.modVersion = tmpBuf.GetString("MOD_VERSION", fldOc); 
	if(tmpBuf.IsFldExist("MOD_BUILDNO", fldOc)) 
		rUpdatemod.modBuildno = tmpBuf.GetInt("MOD_BUILDNO", fldOc); 
	if(tmpBuf.IsFldExist("MOD_SIZE", fldOc)) 
		rUpdatemod.modSize = tmpBuf.GetString("MOD_SIZE", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_METHOD", fldOc)) 
		rUpdatemod.updateMethod = tmpBuf.GetString("UPDATE_METHOD", fldOc); 
	if(tmpBuf.IsFldExist("MUST_UPDATE_TAG", fldOc)) 
		rUpdatemod.mustUpdateTag = tmpBuf.GetString("MUST_UPDATE_TAG", fldOc); 
	if(tmpBuf.IsFldExist("REG_UPDATE_TAG", fldOc)) 
		rUpdatemod.regUpdateTag = tmpBuf.GetString("REG_UPDATE_TAG", fldOc); 
	if(tmpBuf.IsFldExist("REMARK", fldOc)) 
		rUpdatemod.remark = tmpBuf.GetString("REMARK", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_TIME", fldOc)) 
		rUpdatemod.updateTime = tmpBuf.GetString("UPDATE_TIME", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_STAFF_ID", fldOc)) 
		rUpdatemod.updateStaffId = tmpBuf.GetString("UPDATE_STAFF_ID", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_DEPART_ID", fldOc)) 
		rUpdatemod.updateDepartId = tmpBuf.GetString("UPDATE_DEPART_ID", fldOc); 
	if(tmpBuf.IsFldExist("MOD_PATH", fldOc)) 
		rUpdatemod.modPath = tmpBuf.GetString("MOD_PATH", fldOc); 
	if(tmpBuf.IsFldExist("DEL_TAG", fldOc)) 
		rUpdatemod.delTag = tmpBuf.GetString("DEL_TAG", fldOc); 
	if(tmpBuf.IsFldExist("MOD_NAME", fldOc)) 
		rUpdatemod.modName = tmpBuf.GetString("MOD_NAME", fldOc); 
	if(tmpBuf.IsFldExist("MOD_TYPE", fldOc)) 
		rUpdatemod.modType = tmpBuf.GetString("MOD_TYPE", fldOc); 
}

/**
 *  \fn void ParaDom::PutToFmlBuf(Updatemod tmpObj, CFmlBuf &tmpBuf, int fldOc)
 *  Updatemod 类的转换函数，字段值填入CFmlBuf
 */
void ParaDom::PutToFmlBuf(Updatemod &rUpdatemod, CFmlBuf &tmpBuf, int fldOc)
{
	tmpBuf.SetString("MOD_CODE", rUpdatemod.modCode, fldOc);
	tmpBuf.SetString("MOD_VERSION", rUpdatemod.modVersion, fldOc);
	tmpBuf.SetInt("MOD_BUILDNO", rUpdatemod.modBuildno, fldOc);
	tmpBuf.SetString("MOD_SIZE", rUpdatemod.modSize, fldOc);
	tmpBuf.SetString("UPDATE_METHOD", rUpdatemod.updateMethod, fldOc);
	tmpBuf.SetString("MUST_UPDATE_TAG", rUpdatemod.mustUpdateTag, fldOc);
	tmpBuf.SetString("REG_UPDATE_TAG", rUpdatemod.regUpdateTag, fldOc);
	tmpBuf.SetString("REMARK", rUpdatemod.remark, fldOc);
	tmpBuf.SetString("UPDATE_TIME", rUpdatemod.updateTime, fldOc);
	tmpBuf.SetString("UPDATE_STAFF_ID", rUpdatemod.updateStaffId, fldOc);
	tmpBuf.SetString("UPDATE_DEPART_ID", rUpdatemod.updateDepartId, fldOc);
	tmpBuf.SetString("MOD_PATH", rUpdatemod.modPath, fldOc);
	tmpBuf.SetString("DEL_TAG", rUpdatemod.delTag, fldOc);
	tmpBuf.SetString("MOD_NAME", rUpdatemod.modName, fldOc);
	tmpBuf.SetString("MOD_TYPE", rUpdatemod.modType, fldOc);
}

/** \fn void ParaDom::convert(CFmlBuf &tmpBuf, VUpdatemod &vrUpdatemod)
 *  从CFmlBuf从获取实体对象字段值转换成实体对象vector结果返回
 *  @return
 */
void ParaDom::convert(CFmlBuf &tmpBuf, VUpdatemod &vrUpdatemod)
{
	int RecordCount;

    RecordCount = tmpBuf.GetCount("MOD_CODE");

    for(int i=0; i<RecordCount; i++)
    {
    	Updatemod  tmpUpdatemod;

		GetFromFmlBuf(tmpBuf, tmpUpdatemod, i);
		vrUpdatemod.push_back(tmpUpdatemod);
    }

}

/** \fn void ParaDom::convert(VUpdatemod &vrUpdatemod, CFmlBuf &tmpBuf)
 *  从实体对象vector转换成CFmlBuf返回
 *  @return
 */
void ParaDom::convert(VUpdatemod &vrUpdatemod, CFmlBuf &tmpBuf)
{
	int i=0;

    vector<Updatemod>::iterator it = vrUpdatemod.begin();
    for (i = 0 ; it != vrUpdatemod.end(); it++ , i++)
    {
    	PutToFmlBuf((*it), tmpBuf, i);
    }

    tmpBuf.SetInt("X_RECORDNUM", i);
}
