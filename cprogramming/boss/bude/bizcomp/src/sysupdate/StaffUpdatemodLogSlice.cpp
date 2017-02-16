// TD_M_STAFF_UPDATEMOD_LOG	TAB_SET_PARAM
	SetParam(":VMOD_CODE", objStaffUpdatemodLog.modCode); 
	SetParam(":VMOD_VERSION", objStaffUpdatemodLog.modVersion); 
	SetParam(":VMOD_BUILDNO", objStaffUpdatemodLog.modBuildno); 
	SetParam(":VMUST_UPDATE_TAG", objStaffUpdatemodLog.mustUpdateTag); 
	SetParam(":VREG_UPDATE_TAG", objStaffUpdatemodLog.regUpdateTag); 
	SetParam(":VREMARK", objStaffUpdatemodLog.remark); 
	SetParam(":VUPDATE_TIME", objStaffUpdatemodLog.updateTime); 
	SetParam(":VUPDATE_STAFF_ID", objStaffUpdatemodLog.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objStaffUpdatemodLog.updateDepartId); 
	SetParam(":VMOD_SIZE", objStaffUpdatemodLog.modSize); 
	SetParam(":VUPDATE_METHOD", objStaffUpdatemodLog.updateMethod); 
	SetParam(":VDEL_TAG", objStaffUpdatemodLog.delTag); 


// TD_M_STAFF_UPDATEMOD_LOG	TAB_GET_RESULT
	objStaffUpdatemodLog.modCode = GetString("MOD_CODE");
	objStaffUpdatemodLog.modVersion = GetString("MOD_VERSION");
	objStaffUpdatemodLog.modBuildno = GetInt("MOD_BUILDNO");
	objStaffUpdatemodLog.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	objStaffUpdatemodLog.regUpdateTag = GetString("REG_UPDATE_TAG");
	objStaffUpdatemodLog.remark = GetString("REMARK");
	objStaffUpdatemodLog.updateTime = GetString("UPDATE_TIME");
	objStaffUpdatemodLog.updateStaffId = GetString("UPDATE_STAFF_ID");
	objStaffUpdatemodLog.updateDepartId = GetString("UPDATE_DEPART_ID");
	objStaffUpdatemodLog.modSize = GetString("MOD_SIZE");
	objStaffUpdatemodLog.updateMethod = GetString("UPDATE_METHOD");
	objStaffUpdatemodLog.delTag = GetString("DEL_TAG");


// TD_M_STAFF_UPDATEMOD_LOG UPD_SET_PARAM


// TD_M_STAFF_UPDATEMOD_LOG DEL_SET_PARAM


// TD_M_STAFF_UPDATEMOD_LOG INS_SET_PARAM
if( insBy == "INSERT") 
{ 
	/**< 为INSERT参数字段传入条件值 */
	SetParam(":VMOD_CODE", objStaffUpdatemodLog.modCode);
	SetParam(":VMOD_VERSION", objStaffUpdatemodLog.modVersion);
	SetParam(":VMOD_BUILDNO", objStaffUpdatemodLog.modBuildno);
	SetParam(":VMUST_UPDATE_TAG", objStaffUpdatemodLog.mustUpdateTag);
	SetParam(":VREG_UPDATE_TAG", objStaffUpdatemodLog.regUpdateTag);
	SetParam(":VREMARK", objStaffUpdatemodLog.remark);
	SetParam(":VUPDATE_STAFF_ID", objStaffUpdatemodLog.updateStaffId);
	SetParam(":VUPDATE_DEPART_ID", objStaffUpdatemodLog.updateDepartId);
	SetParam(":VMOD_SIZE", objStaffUpdatemodLog.modSize);
	SetParam(":VUPDATE_METHOD", objStaffUpdatemodLog.updateMethod);
	SetParam(":VDEL_TAG", objStaffUpdatemodLog.delTag);
} 
else 
{
	/**< 为INSERT字段传入VALUES值 */
	SetParam(":VMOD_CODE", objStaffUpdatemodLog.modCode); 
	SetParam(":VMOD_VERSION", objStaffUpdatemodLog.modVersion); 
	SetParam(":VMOD_BUILDNO", objStaffUpdatemodLog.modBuildno); 
	SetParam(":VMUST_UPDATE_TAG", objStaffUpdatemodLog.mustUpdateTag); 
	SetParam(":VREG_UPDATE_TAG", objStaffUpdatemodLog.regUpdateTag); 
	SetParam(":VREMARK", objStaffUpdatemodLog.remark); 
	SetParam(":VUPDATE_STAFF_ID", objStaffUpdatemodLog.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objStaffUpdatemodLog.updateDepartId); 
	SetParam(":VMOD_SIZE", objStaffUpdatemodLog.modSize); 
	SetParam(":VUPDATE_METHOD", objStaffUpdatemodLog.updateMethod); 
	SetParam(":VDEL_TAG", objStaffUpdatemodLog.delTag); 
}

// TD_M_STAFF_UPDATEMOD_LOG SEL_SET_PARAM


// TD_M_STAFF_UPDATEMOD_LOG SEL_GET_RESULT
	tmpStaffUpdatemodLog.modCode = GetString("MOD_CODE");
	tmpStaffUpdatemodLog.modVersion = GetString("MOD_VERSION");
	tmpStaffUpdatemodLog.modBuildno = GetInt("MOD_BUILDNO");
	tmpStaffUpdatemodLog.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	tmpStaffUpdatemodLog.regUpdateTag = GetString("REG_UPDATE_TAG");
	tmpStaffUpdatemodLog.remark = GetString("REMARK");
	tmpStaffUpdatemodLog.updateTime = GetString("UPDATE_TIME");
	tmpStaffUpdatemodLog.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpStaffUpdatemodLog.updateDepartId = GetString("UPDATE_DEPART_ID");
	tmpStaffUpdatemodLog.modSize = GetString("MOD_SIZE");
	tmpStaffUpdatemodLog.updateMethod = GetString("UPDATE_METHOD");
	tmpStaffUpdatemodLog.delTag = GetString("DEL_TAG");


// TD_M_STAFF_UPDATEMOD_LOG BATCH_INSERT
int ParaDom::StaffUpdatemodLogDAO::jinsert(VStaffUpdatemodLog &vrStaffUpdatemodLog, const string& insBy)
{
	int isize = vrStaffUpdatemodLog.size();
	
	if (isize == 0) return -1;
	
	char* pcMOD_CODE = new char[isize*7]; 
	char* pcMOD_VERSION = new char[isize*11]; 
	int* piMOD_BUILDNO = new int[isize]; 
	char* pcMUST_UPDATE_TAG = new char[isize*2]; 
	char* pcREG_UPDATE_TAG = new char[isize*2]; 
	char* pcREMARK = new char[isize*101]; 
	char* pcUPDATE_TIME = new char[isize*21]; 
	char* pcUPDATE_STAFF_ID = new char[isize*9]; 
	char* pcUPDATE_DEPART_ID = new char[isize*6]; 
	char* pcMOD_SIZE = new char[isize*11]; 
	char* pcUPDATE_METHOD = new char[isize*2]; 
	char* pcDEL_TAG = new char[isize*2]; 
	
	int i = 0;
	vector<StaffUpdatemodLog>::const_iterator iter = vrStaffUpdatemodLog.begin();
	
	for ( ; iter!=vrStaffUpdatemodLog.end(); iter++) 
	{
		strncpy(pcMOD_CODE+i*7, (*iter).modCode.c_str(), 6);
		*(pcMOD_CODE+i*7+6) = '\0';
		strncpy(pcMOD_VERSION+i*11, (*iter).modVersion.c_str(), 10);
		*(pcMOD_VERSION+i*11+10) = '\0';
		*(piMOD_BUILDNO+i) = (*iter).modBuildno; 
		strncpy(pcMUST_UPDATE_TAG+i*2, (*iter).mustUpdateTag.c_str(), 1);
		*(pcMUST_UPDATE_TAG+i*2+1) = '\0';
		strncpy(pcREG_UPDATE_TAG+i*2, (*iter).regUpdateTag.c_str(), 1);
		*(pcREG_UPDATE_TAG+i*2+1) = '\0';
		strncpy(pcREMARK+i*101, (*iter).remark.c_str(), 100);
		*(pcREMARK+i*101+100) = '\0';
		strncpy(pcUPDATE_TIME+i*21, (*iter).updateTime.c_str(), 20);
		*(pcUPDATE_TIME+i*21+20) = '\0';
		strncpy(pcUPDATE_STAFF_ID+i*9, (*iter).updateStaffId.c_str(), 8);
		*(pcUPDATE_STAFF_ID+i*9+8) = '\0';
		strncpy(pcUPDATE_DEPART_ID+i*6, (*iter).updateDepartId.c_str(), 5);
		*(pcUPDATE_DEPART_ID+i*6+5) = '\0';
		strncpy(pcMOD_SIZE+i*11, (*iter).modSize.c_str(), 10);
		*(pcMOD_SIZE+i*11+10) = '\0';
		strncpy(pcUPDATE_METHOD+i*2, (*iter).updateMethod.c_str(), 1);
		*(pcUPDATE_METHOD+i*2+1) = '\0';
		strncpy(pcDEL_TAG+i*2, (*iter).delTag.c_str(), 1);
		*(pcDEL_TAG+i*2+1) = '\0';
		
		i++;
	}
	
	SetParam(":VMOD_CODE", pcMOD_CODE, 7, isize); 
	SetParam(":VMOD_VERSION", pcMOD_VERSION, 11, isize); 
	SetParam(":VMOD_BUILDNO", piMOD_BUILDNO, isize);
	SetParam(":VMUST_UPDATE_TAG", pcMUST_UPDATE_TAG, 2, isize); 
	SetParam(":VREG_UPDATE_TAG", pcREG_UPDATE_TAG, 2, isize); 
	SetParam(":VREMARK", pcREMARK, 101, isize); 
	SetParam(":VUPDATE_TIME", pcUPDATE_TIME, 21, isize); 
	SetParam(":VUPDATE_STAFF_ID", pcUPDATE_STAFF_ID, 9, isize); 
	SetParam(":VUPDATE_DEPART_ID", pcUPDATE_DEPART_ID, 6, isize); 
	SetParam(":VMOD_SIZE", pcMOD_SIZE, 11, isize); 
	SetParam(":VUPDATE_METHOD", pcUPDATE_METHOD, 2, isize); 
	SetParam(":VDEL_TAG", pcDEL_TAG, 2, isize); 
	
	int iret = 0;
	
	try
	{
		iret = ExecExt(insBy, isize);
	} catch	(EUdbiException& er) 
	{
		delete [] pcMOD_CODE; 
		delete [] pcMOD_VERSION; 
		delete [] piMOD_BUILDNO; 
		delete [] pcMUST_UPDATE_TAG; 
		delete [] pcREG_UPDATE_TAG; 
		delete [] pcREMARK; 
		delete [] pcUPDATE_TIME; 
		delete [] pcUPDATE_STAFF_ID; 
		delete [] pcUPDATE_DEPART_ID; 
		delete [] pcMOD_SIZE; 
		delete [] pcUPDATE_METHOD; 
		delete [] pcDEL_TAG; 
		
		throw;
	}
	
	delete [] pcMOD_CODE; 
	delete [] pcMOD_VERSION; 
	delete [] piMOD_BUILDNO; 
	delete [] pcMUST_UPDATE_TAG; 
	delete [] pcREG_UPDATE_TAG; 
	delete [] pcREMARK; 
	delete [] pcUPDATE_TIME; 
	delete [] pcUPDATE_STAFF_ID; 
	delete [] pcUPDATE_DEPART_ID; 
	delete [] pcMOD_SIZE; 
	delete [] pcUPDATE_METHOD; 
	delete [] pcDEL_TAG; 
	
	return iret;
}
