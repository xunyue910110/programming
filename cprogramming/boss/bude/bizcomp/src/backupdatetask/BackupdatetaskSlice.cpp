// TD_M_BACKUPDATETASK	TAB_SET_PARAM
	SetParam(":VUPDATE_ID", objBackupdatetask.updateId); 
	SetParam(":VUPDATE_DLL_NAME", objBackupdatetask.updateDllName); 
	SetParam(":VUPDATE_INTERVAL", objBackupdatetask.updateInterval); 
	SetParam(":VUPDATE_PRI", objBackupdatetask.updatePri); 
	SetParam(":VUPDATE_TAG", objBackupdatetask.updateTag); 
	SetParam(":VREMARK", objBackupdatetask.remark); 
	SetParam(":VUPDATE_TIME", objBackupdatetask.updateTime); 
	SetParam(":VUPDATE_STAFF_ID", objBackupdatetask.updateStaffId); 
	SetParam(":VUPDATE_DEPART_ID", objBackupdatetask.updateDepartId); 
	SetParam(":VEPARCHY_CODE", objBackupdatetask.eparchyCode); 


// TD_M_BACKUPDATETASK	TAB_GET_RESULT
	objBackupdatetask.updateId = GetString("UPDATE_ID");
	objBackupdatetask.updateDllName = GetString("UPDATE_DLL_NAME");
	objBackupdatetask.updateInterval = GetInt("UPDATE_INTERVAL");
	objBackupdatetask.updatePri = GetInt("UPDATE_PRI");
	objBackupdatetask.updateTag = GetString("UPDATE_TAG");
	objBackupdatetask.remark = GetString("REMARK");
	objBackupdatetask.updateTime = GetString("UPDATE_TIME");
	objBackupdatetask.updateStaffId = GetString("UPDATE_STAFF_ID");
	objBackupdatetask.updateDepartId = GetString("UPDATE_DEPART_ID");
	objBackupdatetask.eparchyCode = GetString("EPARCHY_CODE");


// TD_M_BACKUPDATETASK UPD_SET_PARAM


// TD_M_BACKUPDATETASK DEL_SET_PARAM


// TD_M_BACKUPDATETASK INS_SET_PARAM
/**< 为INSERT字段传入VALUES值 */
SetParam(":VUPDATE_ID", objBackupdatetask.updateId); 
SetParam(":VUPDATE_DLL_NAME", objBackupdatetask.updateDllName); 
SetParam(":VUPDATE_INTERVAL", objBackupdatetask.updateInterval); 
SetParam(":VUPDATE_PRI", objBackupdatetask.updatePri); 
SetParam(":VUPDATE_TAG", objBackupdatetask.updateTag); 
SetParam(":VREMARK", objBackupdatetask.remark); 
SetParam(":VUPDATE_STAFF_ID", objBackupdatetask.updateStaffId); 
SetParam(":VUPDATE_DEPART_ID", objBackupdatetask.updateDepartId); 
SetParam(":VEPARCHY_CODE", objBackupdatetask.eparchyCode); 


// TD_M_BACKUPDATETASK SEL_SET_PARAM
if( selBy == "SEL_ALL_BY_UPDATETAG") 
{ 
	/**< 为WHERE条件字段传入条件值 */
	SetParam(":VEPARCHY_CODE", objBackupdatetask.eparchyCode); 
} 	

// TD_M_BACKUPDATETASK SEL_GET_RESULT
if( selBy == "SEL_ALL_BY_UPDATETAG") 
{ 
	tmpBackupdatetask.updateId = GetString("UPDATE_ID");
	tmpBackupdatetask.updateDllName = GetString("UPDATE_DLL_NAME");
	tmpBackupdatetask.updateInterval = GetInt("UPDATE_INTERVAL");
	tmpBackupdatetask.updatePri = GetInt("UPDATE_PRI");
	tmpBackupdatetask.updateTag = GetString("UPDATE_TAG");
	tmpBackupdatetask.remark = GetString("REMARK");
	tmpBackupdatetask.updateTime = GetString("UPDATE_TIME");
	tmpBackupdatetask.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpBackupdatetask.updateDepartId = GetString("UPDATE_DEPART_ID");
	tmpBackupdatetask.eparchyCode = GetString("EPARCHY_CODE");
} 	
else 
{
	tmpBackupdatetask.updateId = GetString("UPDATE_ID");
	tmpBackupdatetask.updateDllName = GetString("UPDATE_DLL_NAME");
	tmpBackupdatetask.updateInterval = GetInt("UPDATE_INTERVAL");
	tmpBackupdatetask.updatePri = GetInt("UPDATE_PRI");
	tmpBackupdatetask.updateTag = GetString("UPDATE_TAG");
	tmpBackupdatetask.remark = GetString("REMARK");
	tmpBackupdatetask.updateTime = GetString("UPDATE_TIME");
	tmpBackupdatetask.updateStaffId = GetString("UPDATE_STAFF_ID");
	tmpBackupdatetask.updateDepartId = GetString("UPDATE_DEPART_ID");
	tmpBackupdatetask.eparchyCode = GetString("EPARCHY_CODE");
}

// TD_M_BACKUPDATETASK BATCH_INSERT
int ParaDom::BackupdatetaskDAO::jinsert(VBackupdatetask &vrBackupdatetask, const string& insBy)
{
	int isize = vrBackupdatetask.size();
	
	if (isize == 0) return -1;
	
	char* pcUPDATE_ID = new char[isize*21]; 
	char* pcUPDATE_DLL_NAME = new char[isize*51]; 
	int* piUPDATE_INTERVAL = new int[isize]; 
	int* piUPDATE_PRI = new int[isize]; 
	char* pcUPDATE_TAG = new char[isize*2]; 
	char* pcREMARK = new char[isize*101]; 
	char* pcUPDATE_TIME = new char[isize*21]; 
	char* pcUPDATE_STAFF_ID = new char[isize*9]; 
	char* pcUPDATE_DEPART_ID = new char[isize*6]; 
	char* pcEPARCHY_CODE = new char[isize*5]; 
	
	int i = 0;
	vector<Backupdatetask>::const_iterator iter = vrBackupdatetask.begin();
	
	for ( ; iter!=vrBackupdatetask.end(); iter++) 
	{
		strncpy(pcUPDATE_ID+i*21, (*iter).updateId.c_str(), 20);
		*(pcUPDATE_ID+i*21+20) = '\0';
		strncpy(pcUPDATE_DLL_NAME+i*51, (*iter).updateDllName.c_str(), 50);
		*(pcUPDATE_DLL_NAME+i*51+50) = '\0';
		*(piUPDATE_INTERVAL+i) = (*iter).updateInterval; 
		*(piUPDATE_PRI+i) = (*iter).updatePri; 
		strncpy(pcUPDATE_TAG+i*2, (*iter).updateTag.c_str(), 1);
		*(pcUPDATE_TAG+i*2+1) = '\0';
		strncpy(pcREMARK+i*101, (*iter).remark.c_str(), 100);
		*(pcREMARK+i*101+100) = '\0';
		strncpy(pcUPDATE_TIME+i*21, (*iter).updateTime.c_str(), 20);
		*(pcUPDATE_TIME+i*21+20) = '\0';
		strncpy(pcUPDATE_STAFF_ID+i*9, (*iter).updateStaffId.c_str(), 8);
		*(pcUPDATE_STAFF_ID+i*9+8) = '\0';
		strncpy(pcUPDATE_DEPART_ID+i*6, (*iter).updateDepartId.c_str(), 5);
		*(pcUPDATE_DEPART_ID+i*6+5) = '\0';
		strncpy(pcEPARCHY_CODE+i*5, (*iter).eparchyCode.c_str(), 4);
		*(pcEPARCHY_CODE+i*5+4) = '\0';
		
		i++;
	}
	
	SetParam(":VUPDATE_ID", pcUPDATE_ID, 21, isize); 
	SetParam(":VUPDATE_DLL_NAME", pcUPDATE_DLL_NAME, 51, isize); 
	SetParam(":VUPDATE_INTERVAL", piUPDATE_INTERVAL, isize);
	SetParam(":VUPDATE_PRI", piUPDATE_PRI, isize);
	SetParam(":VUPDATE_TAG", pcUPDATE_TAG, 2, isize); 
	SetParam(":VREMARK", pcREMARK, 101, isize); 
	SetParam(":VUPDATE_TIME", pcUPDATE_TIME, 21, isize); 
	SetParam(":VUPDATE_STAFF_ID", pcUPDATE_STAFF_ID, 9, isize); 
	SetParam(":VUPDATE_DEPART_ID", pcUPDATE_DEPART_ID, 6, isize); 
	SetParam(":VEPARCHY_CODE", pcEPARCHY_CODE, 5, isize); 
	
	int iret = 0;
	
	try
	{
		iret = ExecExt(insBy, isize);
	} catch	(EUdbiException& er) 
	{
		delete [] pcUPDATE_ID; 
		delete [] pcUPDATE_DLL_NAME; 
		delete [] piUPDATE_INTERVAL; 
		delete [] piUPDATE_PRI; 
		delete [] pcUPDATE_TAG; 
		delete [] pcREMARK; 
		delete [] pcUPDATE_TIME; 
		delete [] pcUPDATE_STAFF_ID; 
		delete [] pcUPDATE_DEPART_ID; 
		delete [] pcEPARCHY_CODE; 
		
		throw;
	}
	
	delete [] pcUPDATE_ID; 
	delete [] pcUPDATE_DLL_NAME; 
	delete [] piUPDATE_INTERVAL; 
	delete [] piUPDATE_PRI; 
	delete [] pcUPDATE_TAG; 
	delete [] pcREMARK; 
	delete [] pcUPDATE_TIME; 
	delete [] pcUPDATE_STAFF_ID; 
	delete [] pcUPDATE_DEPART_ID; 
	delete [] pcEPARCHY_CODE; 
	
	return iret;
}
