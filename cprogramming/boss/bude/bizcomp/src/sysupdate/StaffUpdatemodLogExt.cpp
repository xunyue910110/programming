#include "StaffUpdatemodLogExt.h"

/**
 *  \fn void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, StaffUpdatemodLog tmpObj, int fldOc)
 *  StaffUpdatemodLog 类的转换函数，从CFmlBuf读取字段值赋给相应成员
 */
void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, StaffUpdatemodLog &rStaffUpdatemodLog, int fldOc)
{
	if(tmpBuf.IsFldExist("MOD_CODE", fldOc)) 
		rStaffUpdatemodLog.modCode = tmpBuf.GetString("MOD_CODE", fldOc); 
	if(tmpBuf.IsFldExist("MOD_VERSION", fldOc)) 
		rStaffUpdatemodLog.modVersion = tmpBuf.GetString("MOD_VERSION", fldOc); 
	if(tmpBuf.IsFldExist("MOD_BUILDNO", fldOc)) 
		rStaffUpdatemodLog.modBuildno = tmpBuf.GetInt("MOD_BUILDNO", fldOc); 
	if(tmpBuf.IsFldExist("MUST_UPDATE_TAG", fldOc)) 
		rStaffUpdatemodLog.mustUpdateTag = tmpBuf.GetString("MUST_UPDATE_TAG", fldOc); 
	if(tmpBuf.IsFldExist("REG_UPDATE_TAG", fldOc)) 
		rStaffUpdatemodLog.regUpdateTag = tmpBuf.GetString("REG_UPDATE_TAG", fldOc); 
	if(tmpBuf.IsFldExist("REMARK", fldOc)) 
		rStaffUpdatemodLog.remark = tmpBuf.GetString("REMARK", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_TIME", fldOc)) 
		rStaffUpdatemodLog.updateTime = tmpBuf.GetString("UPDATE_TIME", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_STAFF_ID", fldOc)) 
		rStaffUpdatemodLog.updateStaffId = tmpBuf.GetString("UPDATE_STAFF_ID", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_DEPART_ID", fldOc)) 
		rStaffUpdatemodLog.updateDepartId = tmpBuf.GetString("UPDATE_DEPART_ID", fldOc); 
	if(tmpBuf.IsFldExist("MOD_SIZE", fldOc)) 
		rStaffUpdatemodLog.modSize = tmpBuf.GetString("MOD_SIZE", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_METHOD", fldOc)) 
		rStaffUpdatemodLog.updateMethod = tmpBuf.GetString("UPDATE_METHOD", fldOc); 
	if(tmpBuf.IsFldExist("DEL_TAG", fldOc)) 
		rStaffUpdatemodLog.delTag = tmpBuf.GetString("DEL_TAG", fldOc); 
}

/**
 *  \fn void ParaDom::PutToFmlBuf(StaffUpdatemodLog tmpObj, CFmlBuf &tmpBuf, int fldOc)
 *  StaffUpdatemodLog 类的转换函数，字段值填入CFmlBuf
 */
void ParaDom::PutToFmlBuf(StaffUpdatemodLog &rStaffUpdatemodLog, CFmlBuf &tmpBuf, int fldOc)
{
	tmpBuf.SetString("MOD_CODE", rStaffUpdatemodLog.modCode, fldOc);
	tmpBuf.SetString("MOD_VERSION", rStaffUpdatemodLog.modVersion, fldOc);
	tmpBuf.SetInt("MOD_BUILDNO", rStaffUpdatemodLog.modBuildno, fldOc);
	tmpBuf.SetString("MUST_UPDATE_TAG", rStaffUpdatemodLog.mustUpdateTag, fldOc);
	tmpBuf.SetString("REG_UPDATE_TAG", rStaffUpdatemodLog.regUpdateTag, fldOc);
	tmpBuf.SetString("REMARK", rStaffUpdatemodLog.remark, fldOc);
	tmpBuf.SetString("UPDATE_TIME", rStaffUpdatemodLog.updateTime, fldOc);
	tmpBuf.SetString("UPDATE_STAFF_ID", rStaffUpdatemodLog.updateStaffId, fldOc);
	tmpBuf.SetString("UPDATE_DEPART_ID", rStaffUpdatemodLog.updateDepartId, fldOc);
	tmpBuf.SetString("MOD_SIZE", rStaffUpdatemodLog.modSize, fldOc);
	tmpBuf.SetString("UPDATE_METHOD", rStaffUpdatemodLog.updateMethod, fldOc);
	tmpBuf.SetString("DEL_TAG", rStaffUpdatemodLog.delTag, fldOc);
}

/** \fn void ParaDom::convert(CFmlBuf &tmpBuf, VStaffUpdatemodLog &vrStaffUpdatemodLog)
 *  从CFmlBuf从获取实体对象字段值转换成实体对象vector结果返回
 *  @return
 */
void ParaDom::convert(CFmlBuf &tmpBuf, VStaffUpdatemodLog &vrStaffUpdatemodLog)
{
	int RecordCount;

    RecordCount = tmpBuf.GetCount("MOD_CODE");

    for(int i=0; i<RecordCount; i++)
    {
    	StaffUpdatemodLog  tmpStaffUpdatemodLog;

		GetFromFmlBuf(tmpBuf, tmpStaffUpdatemodLog, i);
		vrStaffUpdatemodLog.push_back(tmpStaffUpdatemodLog);
    }

}

/** \fn void ParaDom::convert(VStaffUpdatemodLog &vrStaffUpdatemodLog, CFmlBuf &tmpBuf)
 *  从实体对象vector转换成CFmlBuf返回
 *  @return
 */
void ParaDom::convert(VStaffUpdatemodLog &vrStaffUpdatemodLog, CFmlBuf &tmpBuf)
{
	int i=0;

    vector<StaffUpdatemodLog>::iterator it = vrStaffUpdatemodLog.begin();
    for (i = 0 ; it != vrStaffUpdatemodLog.end(); it++ , i++)
    {
    	PutToFmlBuf((*it), tmpBuf, i);
    }

    tmpBuf.SetInt("X_RECORDNUM", i);
}
