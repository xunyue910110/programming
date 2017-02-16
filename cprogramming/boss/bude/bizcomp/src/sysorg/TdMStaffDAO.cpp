/**=!===============SEGMENG1==========================*/
#include "TdMStaffDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class Staff 
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::Staff::Staff()
{
}

SysMan::Staff::~Staff()
{
}

void SysMan::Staff::init()
{
	StaffId.clear();	
	DepartId.clear();	
	StaffName.clear();	
	JobInfo.clear();	
	ManagerInfo.clear();	
	Sex.clear();	
	Email.clear();	
	UserPid.clear();	
	SerialNumber.clear();	
	CustId.clear();	
	DimissionTag.clear();	
	Birthday.clear();	
	StaffGroupId = -1;	
	CustHobyy.clear();	
	Remark.clear();	
	Rsvalue1.clear();	
	Rsvalue2.clear();	
	UpdateTime.clear();	
	UpdateStaffId.clear();	
	UpdateDepartId.clear();	
}

/** @class StaffDAO
 *  @brief TD_M_STAFF表的数据访问接口
 *  本对象对数据库TD_M_STAFF表的数据库访问进行封装，提供所有TD_M_STAFF表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::StaffDAO::StaffDAO(): MTable("TD_M_STAFF")
{
	//
}

SysMan::StaffDAO::~StaffDAO ()
{
}

/** \fn SysMan::VPStaff SysMan::StaffDAO::jselect(const string &selBy, Staff &selStaff)
 *  查询方法，实现TD_M_STAFF表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selStaff  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPStaff SysMan::StaffDAO::jselect(const string &selBy, Staff &selStaff)
{
    SysMan::VPStaff  vpRetStaff; 

    if( selBy == SEL_IDX_TD_M_STAFF_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selStaff.DepartId);
    } 	
    else if( selBy == SEL_IDX_TD_M_STAFF_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_GROUP_ID", selStaff.StaffGroupId);
    } 	
    else if( selBy == SEL_PK_TD_M_STAFF) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selStaff.StaffId);
    } 	
    else if( selBy == SEL_STAFF_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selStaff.StaffId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 
    else if( selBy == SEL_STAFF_7) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selStaff.StaffId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Staff *pTmpStaff = new Staff();
        Staff pTmpStaff;
        
        pTmpStaff.StaffId = GetString("STAFF_ID");
        pTmpStaff.DepartId = GetString("DEPART_ID");
        pTmpStaff.StaffName = GetString("STAFF_NAME");
        pTmpStaff.JobInfo = GetString("JOB_INFO");
        pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        pTmpStaff.Sex = GetString("SEX");
        pTmpStaff.Email = GetString("EMAIL");
        pTmpStaff.UserPid = GetString("USER_PID");
        pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        pTmpStaff.CustId = GetString("CUST_ID");
        pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        pTmpStaff.Birthday = GetString("BIRTHDAY");
        pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        pTmpStaff.Remark = GetString("REMARK");
        pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
		
		vpRetStaff.push_back(pTmpStaff);
    }
    
    return vpRetStaff;
}
 
/** \fn SysMan::VPStaff SysMan::StaffDAO::jselect(const string &selBy, Staff &selStaff)
 *  查询方法，实现TD_M_STAFF表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selStaff  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPStaff SysMan::StaffDAO::jselby(const string &selBy, Staff &selStaff)
{
    SysMan::VPStaff  vpRetStaff; 

    if( selBy == SEL_STAFF_0) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selStaff.StaffId);
    } 	
    else if( selBy == SEL_STAFF_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_5) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_6) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selStaff.DimissionTag);
    	SetParam(":VRSVALUE2", selStaff.Rsvalue2);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Staff *pTmpStaff = new Staff();
        Staff pTmpStaff;
        if( selBy ==SEL_STAFF_0) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_STAFF_1) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_STAFF_2) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_STAFF_3) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_STAFF_5) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_STAFF_6) 
        { 
        	pTmpStaff.StaffId = GetString("STAFF_ID");
        	pTmpStaff.DepartId = GetString("DEPART_ID");
        	pTmpStaff.StaffName = GetString("STAFF_NAME");
        	pTmpStaff.JobInfo = GetString("JOB_INFO");
        	pTmpStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpStaff.Sex = GetString("SEX");
        	pTmpStaff.Email = GetString("EMAIL");
        	pTmpStaff.UserPid = GetString("USER_PID");
        	pTmpStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpStaff.CustId = GetString("CUST_ID");
        	pTmpStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpStaff.Birthday = GetString("BIRTHDAY");
        	pTmpStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpStaff.Remark = GetString("REMARK");
        	pTmpStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 
		
		vpRetStaff.push_back(pTmpStaff);
    }
    
    return vpRetStaff;
}

/** \fn int SysMan::StaffDAO::jupdate(const string &updBy, Staff &updStaff) 
 *  更新方法，实现TD_M_STAFF表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updStaff  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::StaffDAO::jupdate(const string &updBy, Staff &updStaff)
{
    if( updBy == UPD_IDX_TD_M_STAFF_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    	SetParam(":VDEPART_ID", updStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updStaff.StaffName);
    	SetParam(":VJOB_INFO", updStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updStaff.ManagerInfo);
    	SetParam(":VSEX", updStaff.Sex);
    	SetParam(":VEMAIL", updStaff.Email);
    	SetParam(":VUSER_PID", updStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updStaff.SerialNumber);
    	SetParam(":VCUST_ID", updStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updStaff.CustHobyy);
    	SetParam(":VREMARK", updStaff.Remark);
    	SetParam(":VRSVALUE1", updStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", updStaff.DepartId);
    } 	
    else if( updBy == UPD_IDX_TD_M_STAFF_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    	SetParam(":VDEPART_ID", updStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updStaff.StaffName);
    	SetParam(":VJOB_INFO", updStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updStaff.ManagerInfo);
    	SetParam(":VSEX", updStaff.Sex);
    	SetParam(":VEMAIL", updStaff.Email);
    	SetParam(":VUSER_PID", updStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updStaff.SerialNumber);
    	SetParam(":VCUST_ID", updStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updStaff.CustHobyy);
    	SetParam(":VREMARK", updStaff.Remark);
    	SetParam(":VRSVALUE1", updStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_GROUP_ID", updStaff.StaffGroupId);
    } 	
    else if( updBy == UPD_PK_TD_M_STAFF) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    	SetParam(":VDEPART_ID", updStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updStaff.StaffName);
    	SetParam(":VJOB_INFO", updStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updStaff.ManagerInfo);
    	SetParam(":VSEX", updStaff.Sex);
    	SetParam(":VEMAIL", updStaff.Email);
    	SetParam(":VUSER_PID", updStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updStaff.SerialNumber);
    	SetParam(":VCUST_ID", updStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updStaff.CustHobyy);
    	SetParam(":VREMARK", updStaff.Remark);
    	SetParam(":VRSVALUE1", updStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    } 	
    else if( updBy == UPD_UPD_STAFF_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updStaff.StaffName);
    	SetParam(":VJOB_INFO", updStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updStaff.ManagerInfo);
    	SetParam(":VSEX", updStaff.Sex);
    	SetParam(":VEMAIL", updStaff.Email);
    	SetParam(":VUSER_PID", updStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updStaff.SerialNumber);
    	SetParam(":VCUST_ID", updStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updStaff.DimissionTag);
    	SetParam(":VCUST_HOBYY", updStaff.CustHobyy);
    	SetParam(":VREMARK", updStaff.Remark);
    	SetParam(":VRSVALUE1", updStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    } 	
    else if( updBy == UPD_DEL_STAFF_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDIMISSION_TAG", updStaff.DimissionTag);
    	SetParam(":VUPDATE_STAFF_ID", updStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updStaff.StaffId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::StaffDAO::jdelete(const string &delBy, Staff &delStaff)
 *  删除方法，实现TD_M_STAFF表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delStaff  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::StaffDAO::jdelete(const string &delBy, Staff &delStaff)
{
	if( delBy == DEL_IDX_TD_M_STAFF_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VDEPART_ID", delStaff.DepartId);
	} 	
	else if( delBy == DEL_IDX_TD_M_STAFF_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VSTAFF_GROUP_ID", delStaff.StaffGroupId);
	} 	
	else if( delBy == DEL_PK_TD_M_STAFF) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VSTAFF_ID", delStaff.StaffId);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::StaffDAO::jinsert(Staff &insStaff)
 *  插入方法，实现TD_M_STAFF表的插入语句的封装
 *  @param insStaff 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::StaffDAO::jinsert(Staff &insStaff)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VSTAFF_ID", insStaff.StaffId);
    SetParam(":VDEPART_ID", insStaff.DepartId);
    SetParam(":VSTAFF_NAME", insStaff.StaffName);
    SetParam(":VJOB_INFO", insStaff.JobInfo);
    SetParam(":VMANAGER_INFO", insStaff.ManagerInfo);
    SetParam(":VSEX", insStaff.Sex);
    SetParam(":VEMAIL", insStaff.Email);
    SetParam(":VUSER_PID", insStaff.UserPid);
    SetParam(":VSERIAL_NUMBER", insStaff.SerialNumber);
    SetParam(":VCUST_ID", insStaff.CustId);
    SetParam(":VDIMISSION_TAG", insStaff.DimissionTag);
    SetParam(":VSTAFF_GROUP_ID", insStaff.StaffGroupId);
    SetParam(":VCUST_HOBYY", insStaff.CustHobyy);
    SetParam(":VREMARK", insStaff.Remark);
    SetParam(":VRSVALUE1", insStaff.Rsvalue1);
    SetParam(":VRSVALUE2", insStaff.Rsvalue2);
    SetParam(":VUPDATE_STAFF_ID", insStaff.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insStaff.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::StaffDAO::jinsert(Staff &insStaff)
 *  插入方法，实现TD_M_STAFF表的插入语句的封装
 *  @param insStaff 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::StaffDAO::jinsby(const string &insBy, Staff &insStaff)
{
    
    
    ExecInsert(insBy);

    return 0;
}

