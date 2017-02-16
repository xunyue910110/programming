// TD_M_UPDATEMOD	TAB_SET_PARAM
	SetParam(":VMOD_CODE", objUpdatemod.modCode); 
	SetParam(":VMOD_VERSION", objUpdatemod.modVersion); 
	SetParam(":VMOD_BUILDNO", objUpdatemod.modBuildno); 
	SetParam(":VMOD_SIZE", objUpdatemod.modSize); 
	SetParam(":VUPDATE_METHOD", objUpdatemod.updateMethod); 
	SetParam(":VMUST_UPDATE_TAG", objUpdatemod.mustUpdateTag); 
	SetParam(":VREG_UPDATE_TAG", objUpdatemod.regUpdateTag); 
	SetParam(":VREMARK", objUpdatemod.remark); 
	SetParam(":VUPDATE_TIME", objUpdatemod.updateTime); 
	SetParam(":VUPDATE_STAFF_ID", objUpdatemod.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objUpdatemod.updateDepartId); 
	SetParam(":VMOD_PATH", objUpdatemod.modPath); 
	SetParam(":VDEL_TAG", objUpdatemod.delTag); 


// TD_M_UPDATEMOD	TAB_GET_RESULT
	objUpdatemod.modCode = GetString("MOD_CODE");
	objUpdatemod.modVersion = GetString("MOD_VERSION");
	objUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	objUpdatemod.modSize = GetString("MOD_SIZE");
	objUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	objUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	objUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	objUpdatemod.remark = GetString("REMARK");
	objUpdatemod.updateTime = GetString("UPDATE_TIME");
	objUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	objUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	objUpdatemod.modPath = GetString("MOD_PATH");
	objUpdatemod.delTag = GetString("DEL_TAG");


// TD_M_UPDATEMOD UPD_SET_PARAM
if( updBy == "UPD_PK_TD_M_UPDATEMOD") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VMOD_CODE", objUpdatemod.modCode);

	/**< 为UPDATE字段传入参数值 */
	SetParam(":VMOD_CODE", objUpdatemod.modCode); 
	SetParam(":VMOD_VERSION", objUpdatemod.modVersion); 
	SetParam(":VMOD_BUILDNO", objUpdatemod.modBuildno); 
	SetParam(":VMOD_SIZE", objUpdatemod.modSize); 
	SetParam(":VUPDATE_METHOD", objUpdatemod.updateMethod); 
	SetParam(":VMUST_UPDATE_TAG", objUpdatemod.mustUpdateTag); 
	SetParam(":VREG_UPDATE_TAG", objUpdatemod.regUpdateTag); 
	SetParam(":VREMARK", objUpdatemod.remark); 
	SetParam(":VUPDATE_STAFF_ID", objUpdatemod.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objUpdatemod.updateDepartId); 
	SetParam(":VMOD_PATH", objUpdatemod.modPath); 
	SetParam(":VDEL_TAG", objUpdatemod.delTag); 
}

// TD_M_UPDATEMOD DEL_SET_PARAM
if( delBy == "DEL_PK_TD_M_UPDATEMOD") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VMOD_CODE", objUpdatemod.modCode);
} 	

// TD_M_UPDATEMOD INS_SET_PARAM
	/**< 为INSERT字段传入VALUES值 */
	SetParam(":VMOD_CODE", objUpdatemod.modCode); 
	SetParam(":VMOD_VERSION", objUpdatemod.modVersion); 
	SetParam(":VMOD_BUILDNO", objUpdatemod.modBuildno); 
	SetParam(":VMOD_SIZE", objUpdatemod.modSize); 
	SetParam(":VUPDATE_METHOD", objUpdatemod.updateMethod); 
	SetParam(":VMUST_UPDATE_TAG", objUpdatemod.mustUpdateTag); 
	SetParam(":VREG_UPDATE_TAG", objUpdatemod.regUpdateTag); 
	SetParam(":VREMARK", objUpdatemod.remark); 
	SetParam(":VUPDATE_STAFF_ID", objUpdatemod.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objUpdatemod.updateDepartId); 
	SetParam(":VMOD_PATH", objUpdatemod.modPath); 
	SetParam(":VDEL_TAG", objUpdatemod.delTag); 


// TD_M_UPDATEMOD SEL_SET_PARAM
if( selBy == "SEL_PK_TD_M_UPDATEMOD") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VMOD_CODE", objUpdatemod.modCode);
} 	

// TD_M_UPDATEMOD SEL_GET_RESULT
if( selBy =="SEL_BY_MOD_CODE") 
{ 
	tmpUpdatemod.modCode = GetString("MOD_CODE");
	tmpUpdatemod.modVersion = GetString("MOD_VERSION");
	tmpUpdatemod.modName = GetString("MOD_NAME");
	tmpUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	tmpUpdatemod.modType = GetString("MOD_TYPE");
	tmpUpdatemod.modSize = GetString("MOD_SIZE");
	tmpUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	tmpUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	tmpUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	tmpUpdatemod.remark = GetString("REMARK");
	tmpUpdatemod.updateTime = GetString("UPDATE_TIME");
	tmpUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	tmpUpdatemod.modPath = GetString("MOD_PATH");
	tmpUpdatemod.delTag = GetString("DEL_TAG");
} 
else 
{
	tmpUpdatemod.modCode = GetString("MOD_CODE");
	tmpUpdatemod.modVersion = GetString("MOD_VERSION");
	tmpUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	tmpUpdatemod.modSize = GetString("MOD_SIZE");
	tmpUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	tmpUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	tmpUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	tmpUpdatemod.remark = GetString("REMARK");
	tmpUpdatemod.updateTime = GetString("UPDATE_TIME");
	tmpUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	tmpUpdatemod.modPath = GetString("MOD_PATH");
	tmpUpdatemod.delTag = GetString("DEL_TAG");
}

// TD_M_UPDATEMOD BATCH_INSERT
int ParaDom::UpdatemodDAO::jinsert(VUpdatemod &vrUpdatemod, const string& insBy)
{
	int isize = vrUpdatemod.size();
	
	if (isize == 0) return -1;
	
	char* pcMOD_CODE = new char[isize*7]; 
	char* pcMOD_VERSION = new char[isize*11]; 
	int* piMOD_BUILDNO = new int[isize]; 
	char* pcMOD_SIZE = new char[isize*11]; 
	char* pcUPDATE_METHOD = new char[isize*2]; 
	char* pcMUST_UPDATE_TAG = new char[isize*2]; 
	char* pcREG_UPDATE_TAG = new char[isize*2]; 
	char* pcREMARK = new char[isize*101]; 
	char* pcUPDATE_TIME = new char[isize*21]; 
	char* pcUPDATE_STAFF_ID = new char[isize*9]; 
	char* pcUPDATE_DEPART_ID = new char[isize*6]; 
	char* pcMOD_PATH = new char[isize*51]; 
	char* pcDEL_TAG = new char[isize*2]; 
	
	int i = 0;
	vector<Updatemod>::const_iterator iter = vrUpdatemod.begin();
	
	for ( ; iter!=vrUpdatemod.end(); iter++) 
	{
		strncpy(pcMOD_CODE+i*7, (*iter).modCode.c_str(), 6);
		*(pcMOD_CODE+i*7+6) = '\0';
		strncpy(pcMOD_VERSION+i*11, (*iter).modVersion.c_str(), 10);
		*(pcMOD_VERSION+i*11+10) = '\0';
		*(piMOD_BUILDNO+i) = (*iter).modBuildno; 
		strncpy(pcMOD_SIZE+i*11, (*iter).modSize.c_str(), 10);
		*(pcMOD_SIZE+i*11+10) = '\0';
		strncpy(pcUPDATE_METHOD+i*2, (*iter).updateMethod.c_str(), 1);
		*(pcUPDATE_METHOD+i*2+1) = '\0';
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
		strncpy(pcMOD_PATH+i*51, (*iter).modPath.c_str(), 50);
		*(pcMOD_PATH+i*51+50) = '\0';
		strncpy(pcDEL_TAG+i*2, (*iter).delTag.c_str(), 1);
		*(pcDEL_TAG+i*2+1) = '\0';
		
		i++;
	}
	
	SetParam(":VMOD_CODE", pcMOD_CODE, 7, isize); 
	SetParam(":VMOD_VERSION", pcMOD_VERSION, 11, isize); 
	SetParam(":VMOD_BUILDNO", piMOD_BUILDNO, isize);
	SetParam(":VMOD_SIZE", pcMOD_SIZE, 11, isize); 
	SetParam(":VUPDATE_METHOD", pcUPDATE_METHOD, 2, isize); 
	SetParam(":VMUST_UPDATE_TAG", pcMUST_UPDATE_TAG, 2, isize); 
	SetParam(":VREG_UPDATE_TAG", pcREG_UPDATE_TAG, 2, isize); 
	SetParam(":VREMARK", pcREMARK, 101, isize); 
	SetParam(":VUPDATE_TIME", pcUPDATE_TIME, 21, isize); 
	SetParam(":VUPDATE_STAFF_ID", pcUPDATE_STAFF_ID, 9, isize); 
	SetParam(":VUPDATE_DEPART_ID", pcUPDATE_DEPART_ID, 6, isize); 
	SetParam(":VMOD_PATH", pcMOD_PATH, 51, isize); 
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
		delete [] pcMOD_SIZE; 
		delete [] pcUPDATE_METHOD; 
		delete [] pcMUST_UPDATE_TAG; 
		delete [] pcREG_UPDATE_TAG; 
		delete [] pcREMARK; 
		delete [] pcUPDATE_TIME; 
		delete [] pcUPDATE_STAFF_ID; 
		delete [] pcUPDATE_DEPART_ID; 
		delete [] pcMOD_PATH; 
		delete [] pcDEL_TAG; 
		
		throw;
	}
	
	delete [] pcMOD_CODE; 
	delete [] pcMOD_VERSION; 
	delete [] piMOD_BUILDNO; 
	delete [] pcMOD_SIZE; 
	delete [] pcUPDATE_METHOD; 
	delete [] pcMUST_UPDATE_TAG; 
	delete [] pcREG_UPDATE_TAG; 
	delete [] pcREMARK; 
	delete [] pcUPDATE_TIME; 
	delete [] pcUPDATE_STAFF_ID; 
	delete [] pcUPDATE_DEPART_ID; 
	delete [] pcMOD_PATH; 
	delete [] pcDEL_TAG; 
	
	return iret;
}
