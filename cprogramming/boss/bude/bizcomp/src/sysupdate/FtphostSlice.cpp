// TD_S_FTPHOST	TAB_SET_PARAM
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode); 
	SetParam(":VFTP_HOST", objFtphost.ftpHost); 
	SetParam(":VFTP_USRID", objFtphost.ftpUsrid); 
	SetParam(":VFTP_PASSWD", objFtphost.ftpPasswd); 
	SetParam(":VFTP_PATH", objFtphost.ftpPath); 
	SetParam(":VFTP_MOD", objFtphost.ftpMod); 
	SetParam(":VREMARK", objFtphost.remark); 
	SetParam(":VUPDATE_TIME", objFtphost.updateTime); 
	SetParam(":VUPDATE_STAFF_ID", objFtphost.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objFtphost.updateDepartId); 


// TD_S_FTPHOST	TAB_GET_RESULT
	objFtphost.eparchyCode = GetString("EPARCHY_CODE");
	objFtphost.ftpHost = GetString("FTP_HOST");
	objFtphost.ftpUsrid = GetString("FTP_USRID");
	objFtphost.ftpPasswd = GetString("FTP_PASSWD");
	objFtphost.ftpPath = GetString("FTP_PATH");
	objFtphost.ftpMod = GetString("FTP_MOD");
	objFtphost.remark = GetString("REMARK");
	objFtphost.updateTime = GetString("UPDATE_TIME");
	objFtphost.updateStaffId = GetString("UPDATE_STAFF_ID");
	objFtphost.updateDepartId = GetString("UPDATE_DEPART_ID");


// TD_S_FTPHOST UPD_SET_PARAM
if( updBy == "UPD_PK_TD_S_FTPHOST") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode);

	/**< 为UPDATE字段传入参数值 */
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode); 
	SetParam(":VFTP_HOST", objFtphost.ftpHost); 
	SetParam(":VFTP_USRID", objFtphost.ftpUsrid); 
	SetParam(":VFTP_PASSWD", objFtphost.ftpPasswd); 
	SetParam(":VFTP_PATH", objFtphost.ftpPath); 
	SetParam(":VFTP_MOD", objFtphost.ftpMod); 
	SetParam(":VREMARK", objFtphost.remark); 
	SetParam(":VUPDATE_STAFF_ID", objFtphost.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objFtphost.updateDepartId); 
}

// TD_S_FTPHOST DEL_SET_PARAM
if( delBy == "DEL_PK_TD_S_FTPHOST") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode);
} 	

// TD_S_FTPHOST INS_SET_PARAM
	/**< 为INSERT字段传入VALUES值 */
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode); 
	SetParam(":VFTP_HOST", objFtphost.ftpHost); 
	SetParam(":VFTP_USRID", objFtphost.ftpUsrid); 
	SetParam(":VFTP_PASSWD", objFtphost.ftpPasswd); 
	SetParam(":VFTP_PATH", objFtphost.ftpPath); 
	SetParam(":VFTP_MOD", objFtphost.ftpMod); 
	SetParam(":VREMARK", objFtphost.remark); 
	SetParam(":VUPDATE_STAFF_ID", objFtphost.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objFtphost.updateDepartId); 


// TD_S_FTPHOST SEL_SET_PARAM
if( selBy == "SEL_PK_TD_S_FTPHOST") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VEPARCHY_CODE", objFtphost.eparchyCode);
} 	

// TD_S_FTPHOST SEL_GET_RESULT
	tmpFtphost.eparchyCode = GetString("EPARCHY_CODE");
	tmpFtphost.ftpHost = GetString("FTP_HOST");
	tmpFtphost.ftpUsrid = GetString("FTP_USRID");
	tmpFtphost.ftpPasswd = GetString("FTP_PASSWD");
	tmpFtphost.ftpPath = GetString("FTP_PATH");
	tmpFtphost.ftpMod = GetString("FTP_MOD");
	tmpFtphost.remark = GetString("REMARK");
	tmpFtphost.updateTime = GetString("UPDATE_TIME");
	tmpFtphost.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpFtphost.updateDepartId = GetString("UPDATE_DEPART_ID");


// TD_S_FTPHOST BATCH_INSERT
int ParaDom::FtphostDAO::jinsert(VFtphost &vrFtphost, const string& insBy)
{
	int isize = vrFtphost.size();
	
	if (isize == 0) return -1;
	
	char* pcEPARCHY_CODE = new char[isize*5]; 
	char* pcFTP_HOST = new char[isize*16]; 
	char* pcFTP_USRID = new char[isize*21]; 
	char* pcFTP_PASSWD = new char[isize*21]; 
	char* pcFTP_PATH = new char[isize*51]; 
	char* pcFTP_MOD = new char[isize*2]; 
	char* pcREMARK = new char[isize*101]; 
	char* pcUPDATE_TIME = new char[isize*21]; 
	char* pcUPDATE_STAFF_ID = new char[isize*9]; 
	char* pcUPDATE_DEPART_ID = new char[isize*6]; 
	
	int i = 0;
	vector<Ftphost>::const_iterator iter = vrFtphost.begin();
	
	for ( ; iter!=vrFtphost.end(); iter++) 
	{
		strncpy(pcEPARCHY_CODE+i*5, (*iter).eparchyCode.c_str(), 4);
		*(pcEPARCHY_CODE+i*5+4) = '\0';
		strncpy(pcFTP_HOST+i*16, (*iter).ftpHost.c_str(), 15);
		*(pcFTP_HOST+i*16+15) = '\0';
		strncpy(pcFTP_USRID+i*21, (*iter).ftpUsrid.c_str(), 20);
		*(pcFTP_USRID+i*21+20) = '\0';
		strncpy(pcFTP_PASSWD+i*21, (*iter).ftpPasswd.c_str(), 20);
		*(pcFTP_PASSWD+i*21+20) = '\0';
		strncpy(pcFTP_PATH+i*51, (*iter).ftpPath.c_str(), 50);
		*(pcFTP_PATH+i*51+50) = '\0';
		strncpy(pcFTP_MOD+i*2, (*iter).ftpMod.c_str(), 1);
		*(pcFTP_MOD+i*2+1) = '\0';
		strncpy(pcREMARK+i*101, (*iter).remark.c_str(), 100);
		*(pcREMARK+i*101+100) = '\0';
		strncpy(pcUPDATE_TIME+i*21, (*iter).updateTime.c_str(), 20);
		*(pcUPDATE_TIME+i*21+20) = '\0';
		strncpy(pcUPDATE_STAFF_ID+i*9, (*iter).updateStaffId.c_str(), 8);
		*(pcUPDATE_STAFF_ID+i*9+8) = '\0';
		strncpy(pcUPDATE_DEPART_ID+i*6, (*iter).updateDepartId.c_str(), 5);
		*(pcUPDATE_DEPART_ID+i*6+5) = '\0';
		
		i++;
	}
	
	SetParam(":VEPARCHY_CODE", pcEPARCHY_CODE, 5, isize); 
	SetParam(":VFTP_HOST", pcFTP_HOST, 16, isize); 
	SetParam(":VFTP_USRID", pcFTP_USRID, 21, isize); 
	SetParam(":VFTP_PASSWD", pcFTP_PASSWD, 21, isize); 
	SetParam(":VFTP_PATH", pcFTP_PATH, 51, isize); 
	SetParam(":VFTP_MOD", pcFTP_MOD, 2, isize); 
	SetParam(":VREMARK", pcREMARK, 101, isize); 
	SetParam(":VUPDATE_TIME", pcUPDATE_TIME, 21, isize); 
	SetParam(":VUPDATE_STAFF_ID", pcUPDATE_STAFF_ID, 9, isize); 
	SetParam(":VUPDATE_DEPART_ID", pcUPDATE_DEPART_ID, 6, isize); 
	
	int iret = 0;
	
	try
	{
		iret = ExecExt(insBy, isize);
	} catch	(EUdbiException& er) 
	{
		delete [] pcEPARCHY_CODE; 
		delete [] pcFTP_HOST; 
		delete [] pcFTP_USRID; 
		delete [] pcFTP_PASSWD; 
		delete [] pcFTP_PATH; 
		delete [] pcFTP_MOD; 
		delete [] pcREMARK; 
		delete [] pcUPDATE_TIME; 
		delete [] pcUPDATE_STAFF_ID; 
		delete [] pcUPDATE_DEPART_ID; 
		
		throw;
	}
	
	delete [] pcEPARCHY_CODE; 
	delete [] pcFTP_HOST; 
	delete [] pcFTP_USRID; 
	delete [] pcFTP_PASSWD; 
	delete [] pcFTP_PATH; 
	delete [] pcFTP_MOD; 
	delete [] pcREMARK; 
	delete [] pcUPDATE_TIME; 
	delete [] pcUPDATE_STAFF_ID; 
	delete [] pcUPDATE_DEPART_ID; 
	
	return iret;
}
