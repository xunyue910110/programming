#include "FtphostExt.h"

/**
 *  \fn void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Ftphost tmpObj, int fldOc)
 *  Ftphost 类的转换函数，从CFmlBuf读取字段值赋给相应成员
 */
void ParaDom::GetFromFmlBuf(CFmlBuf &tmpBuf, Ftphost &rFtphost, int fldOc)
{
	if(tmpBuf.IsFldExist("EPARCHY_CODE", fldOc)) 
		rFtphost.eparchyCode = tmpBuf.GetString("EPARCHY_CODE", fldOc); 
	if(tmpBuf.IsFldExist("FTP_HOST", fldOc)) 
		rFtphost.ftpHost = tmpBuf.GetString("FTP_HOST", fldOc); 
	if(tmpBuf.IsFldExist("FTP_USRID", fldOc)) 
		rFtphost.ftpUsrid = tmpBuf.GetString("FTP_USRID", fldOc); 
	if(tmpBuf.IsFldExist("FTP_PASSWD", fldOc)) 
		rFtphost.ftpPasswd = tmpBuf.GetString("FTP_PASSWD", fldOc); 
	if(tmpBuf.IsFldExist("FTP_PATH", fldOc)) 
		rFtphost.ftpPath = tmpBuf.GetString("FTP_PATH", fldOc); 
	if(tmpBuf.IsFldExist("FTP_MOD", fldOc)) 
		rFtphost.ftpMod = tmpBuf.GetString("FTP_MOD", fldOc); 
	if(tmpBuf.IsFldExist("REMARK", fldOc)) 
		rFtphost.remark = tmpBuf.GetString("REMARK", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_TIME", fldOc)) 
		rFtphost.updateTime = tmpBuf.GetString("UPDATE_TIME", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_STAFF_ID", fldOc)) 
		rFtphost.updateStaffId = tmpBuf.GetString("UPDATE_STAFF_ID", fldOc); 
	if(tmpBuf.IsFldExist("UPDATE_DEPART_ID", fldOc)) 
		rFtphost.updateDepartId = tmpBuf.GetString("UPDATE_DEPART_ID", fldOc); 
}

/**
 *  \fn void ParaDom::PutToFmlBuf(Ftphost tmpObj, CFmlBuf &tmpBuf, int fldOc)
 *  Ftphost 类的转换函数，字段值填入CFmlBuf
 */
void ParaDom::PutToFmlBuf(Ftphost &rFtphost, CFmlBuf &tmpBuf, int fldOc)
{
	tmpBuf.SetString("EPARCHY_CODE", rFtphost.eparchyCode, fldOc);
	tmpBuf.SetString("FTP_HOST", rFtphost.ftpHost, fldOc);
	tmpBuf.SetString("FTP_USRID", rFtphost.ftpUsrid, fldOc);
	tmpBuf.SetString("FTP_PASSWD", rFtphost.ftpPasswd, fldOc);
	tmpBuf.SetString("FTP_PATH", rFtphost.ftpPath, fldOc);
	tmpBuf.SetString("FTP_MOD", rFtphost.ftpMod, fldOc);
	tmpBuf.SetString("REMARK", rFtphost.remark, fldOc);
	tmpBuf.SetString("UPDATE_TIME", rFtphost.updateTime, fldOc);
	tmpBuf.SetString("UPDATE_STAFF_ID", rFtphost.updateStaffId, fldOc);
	tmpBuf.SetString("UPDATE_DEPART_ID", rFtphost.updateDepartId, fldOc);
}

/** \fn void ParaDom::convert(CFmlBuf &tmpBuf, VFtphost &vrFtphost)
 *  从CFmlBuf从获取实体对象字段值转换成实体对象vector结果返回
 *  @return
 */
void ParaDom::convert(CFmlBuf &tmpBuf, VFtphost &vrFtphost)
{
	int RecordCount;

    RecordCount = tmpBuf.GetCount("EPARCHY_CODE");

    for(int i=0; i<RecordCount; i++)
    {
    	Ftphost  tmpFtphost;

		GetFromFmlBuf(tmpBuf, tmpFtphost, i);
		vrFtphost.push_back(tmpFtphost);
    }

}

/** \fn void ParaDom::convert(VFtphost &vrFtphost, CFmlBuf &tmpBuf)
 *  从实体对象vector转换成CFmlBuf返回
 *  @return
 */
void ParaDom::convert(VFtphost &vrFtphost, CFmlBuf &tmpBuf)
{
	int i=0;

    vector<Ftphost>::iterator it = vrFtphost.begin();
    for (i = 0 ; it != vrFtphost.end(); it++ , i++)
    {
    	PutToFmlBuf((*it), tmpBuf, i);
    }

    tmpBuf.SetInt("X_RECORDNUM", i);
}
