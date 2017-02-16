/**=!===============SEGMENG1==========================*/
#include "OrgTdMStaffDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class ORG_TdMStaff 
 *  @brief TD_M_STAFF表的映射实体
 *  本对象映射数据库TD_M_STAFF表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::ORG_TdMStaff::ORG_TdMStaff()
{
}

SysMan::ORG_TdMStaff::~ORG_TdMStaff()
{
}

void SysMan::ORG_TdMStaff::ORG_init()
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
	//add by liufei
	b_DepartId.clear();
	b_Sex.clear();
	b_DimissionTag.clear();
	b_UpdateStaffId.clear();
	b_UpdateDepartId.clear();
	AreaCode.clear();
	StartStaffId.clear();
	EndStaffId.clear();
	ValidFlag.clear();

}

/** @class ORG_TdMStaffDAO
 *  @brief TD_M_STAFF表的数据访问接口
 *  本对象对数据库TD_M_STAFF表的数据库访问进行封装，提供所有TD_M_STAFF表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::ORG_TdMStaffDAO::ORG_TdMStaffDAO(): StaffDAO()
{
	//
}

SysMan::ORG_TdMStaffDAO::~ORG_TdMStaffDAO ()
{
}

/** \fn SysMan::VPORG_TdMStaff SysMan::ORG_TdMStaffDAO::jselect(const string &selBy, ORG_TdMStaff &selORG_TdMStaff)
 *  查询方法，实现TD_M_STAFF表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selORG_TdMStaff  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMStaff SysMan::ORG_TdMStaffDAO::ORG_jselect(const string &selBy, ORG_TdMStaff &selORG_TdMStaff)
{
    SysMan::VPORG_TdMStaff  vpRetORG_TdMStaff; 

    if( selBy == SEL_IDX_TD_M_STAFF_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selORG_TdMStaff.DepartId);
    } 	
    else if( selBy == SEL_IDX_TD_M_STAFF_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_GROUP_ID", selORG_TdMStaff.StaffGroupId);
    } 	
    else if( selBy == SEL_PK_TD_M_STAFF) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selORG_TdMStaff.StaffId);
    } 	
    else if( selBy == SEL_STAFF_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTARTSTAFF_ID", selORG_TdMStaff.StartStaffId);
    	SetParam(":VENDSTAFF_ID", selORG_TdMStaff.EndStaffId);
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    } 
    else if( selBy == SEL_STAFF_7) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTARTSTAFF_ID", selORG_TdMStaff.StartStaffId);
    	SetParam(":VENDSTAFF_ID", selORG_TdMStaff.EndStaffId);
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    	SetParam(":VAREA_CODE", selORG_TdMStaff.AreaCode);
    	SetParam(":VVALIDFLAG", selORG_TdMStaff.ValidFlag);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMStaff *pTmpORG_TdMStaff = new ORG_TdMStaff();
        ORG_TdMStaff pTmpORG_TdMStaff;
        
        pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        pTmpORG_TdMStaff.Sex = GetString("SEX");
        pTmpORG_TdMStaff.Email = GetString("EMAIL");
        pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        pTmpORG_TdMStaff.Remark = GetString("REMARK");
        pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        //ADD BY LIUFEI
        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
		pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
		pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
		pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
		pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");

		
		vpRetORG_TdMStaff.push_back(pTmpORG_TdMStaff);
    }
    
    return vpRetORG_TdMStaff;
}
 
/** \fn SysMan::VPORG_TdMStaff SysMan::ORG_TdMStaffDAO::jselect(const string &selBy, ORG_TdMStaff &selORG_TdMStaff)
 *  查询方法，实现TD_M_STAFF表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selORG_TdMStaff  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMStaff SysMan::ORG_TdMStaffDAO::ORG_jselby(const string &selBy, ORG_TdMStaff &selORG_TdMStaff)
{
    SysMan::VPORG_TdMStaff  vpRetORG_TdMStaff; 

    if( selBy == SEL_STAFF_0) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", selORG_TdMStaff.StaffId);
    } 	
    else if( selBy == SEL_STAFF_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selORG_TdMStaff.AreaCode);
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selORG_TdMStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    } 	
    else if( selBy == SEL_STAFF_5) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selORG_TdMStaff.DepartId);
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    	SetParam(":VVALIDFLAG", selORG_TdMStaff.ValidFlag);
    } 	
    else if( selBy == SEL_STAFF_6) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDIMISSION_TAG", selORG_TdMStaff.DimissionTag);
    	SetParam(":VRSVALUE2", selORG_TdMStaff.Rsvalue2);
    	SetParam(":VVALIDFLAG", selORG_TdMStaff.ValidFlag);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMStaff *pTmpORG_TdMStaff = new ORG_TdMStaff();
        ORG_TdMStaff pTmpORG_TdMStaff;
        
        if( selBy ==SEL_STAFF_0) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_STAFF_1) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_STAFF_2) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_STAFF_3) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_STAFF_5) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_STAFF_6) 
        { 
        	pTmpORG_TdMStaff.StaffId = GetString("STAFF_ID");
        	pTmpORG_TdMStaff.DepartId = GetString("DEPART_ID");
        	pTmpORG_TdMStaff.StaffName = GetString("STAFF_NAME");
        	pTmpORG_TdMStaff.JobInfo = GetString("JOB_INFO");
        	pTmpORG_TdMStaff.ManagerInfo = GetString("MANAGER_INFO");
        	pTmpORG_TdMStaff.Sex = GetString("SEX");
        	pTmpORG_TdMStaff.Email = GetString("EMAIL");
        	pTmpORG_TdMStaff.UserPid = GetString("USER_PID");
        	pTmpORG_TdMStaff.SerialNumber = GetString("SERIAL_NUMBER");
        	pTmpORG_TdMStaff.CustId = GetString("CUST_ID");
        	pTmpORG_TdMStaff.DimissionTag = GetString("DIMISSION_TAG");
        	pTmpORG_TdMStaff.Birthday = GetString("BIRTHDAY");
        	pTmpORG_TdMStaff.StaffGroupId = GetInt("STAFF_GROUP_ID");
        	pTmpORG_TdMStaff.CustHobyy = GetString("CUST_HOBYY");
        	pTmpORG_TdMStaff.Remark = GetString("REMARK");
        	pTmpORG_TdMStaff.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMStaff.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMStaff.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMStaff.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMStaff.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	//ADD BY LIUFEI
	        pTmpORG_TdMStaff.b_DepartId = GetString("B_DEPARTID");
			pTmpORG_TdMStaff.b_Sex = GetString("B_SEX");
			pTmpORG_TdMStaff.b_DimissionTag = GetString("B_DIMISSIONTAG");
			pTmpORG_TdMStaff.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
			pTmpORG_TdMStaff.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
		
		vpRetORG_TdMStaff.push_back(pTmpORG_TdMStaff);
    }
    
    return vpRetORG_TdMStaff;
}

/** \fn int SysMan::ORG_TdMStaffDAO::jupdate(const string &updBy, ORG_TdMStaff &updORG_TdMStaff) 
 *  更新方法，实现TD_M_STAFF表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updORG_TdMStaff  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::ORG_TdMStaffDAO::ORG_jupdate(const string &updBy, ORG_TdMStaff &updORG_TdMStaff)
{
    if( updBy == UPD_IDX_TD_M_STAFF_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    	SetParam(":VDEPART_ID", updORG_TdMStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updORG_TdMStaff.StaffName);
    	SetParam(":VJOB_INFO", updORG_TdMStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updORG_TdMStaff.ManagerInfo);
    	SetParam(":VSEX", updORG_TdMStaff.Sex);
    	SetParam(":VEMAIL", updORG_TdMStaff.Email);
    	SetParam(":VUSER_PID", updORG_TdMStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updORG_TdMStaff.SerialNumber);
    	SetParam(":VCUST_ID", updORG_TdMStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updORG_TdMStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updORG_TdMStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updORG_TdMStaff.CustHobyy);
    	SetParam(":VREMARK", updORG_TdMStaff.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", updORG_TdMStaff.DepartId);
    } 	
    else if( updBy == UPD_IDX_TD_M_STAFF_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    	SetParam(":VDEPART_ID", updORG_TdMStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updORG_TdMStaff.StaffName);
    	SetParam(":VJOB_INFO", updORG_TdMStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updORG_TdMStaff.ManagerInfo);
    	SetParam(":VSEX", updORG_TdMStaff.Sex);
    	SetParam(":VEMAIL", updORG_TdMStaff.Email);
    	SetParam(":VUSER_PID", updORG_TdMStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updORG_TdMStaff.SerialNumber);
    	SetParam(":VCUST_ID", updORG_TdMStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updORG_TdMStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updORG_TdMStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updORG_TdMStaff.CustHobyy);
    	SetParam(":VREMARK", updORG_TdMStaff.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_GROUP_ID", updORG_TdMStaff.StaffGroupId);
    } 	
    else if( updBy == UPD_PK_TD_M_STAFF) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    	SetParam(":VDEPART_ID", updORG_TdMStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updORG_TdMStaff.StaffName);
    	SetParam(":VJOB_INFO", updORG_TdMStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updORG_TdMStaff.ManagerInfo);
    	SetParam(":VSEX", updORG_TdMStaff.Sex);
    	SetParam(":VEMAIL", updORG_TdMStaff.Email);
    	SetParam(":VUSER_PID", updORG_TdMStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updORG_TdMStaff.SerialNumber);
    	SetParam(":VCUST_ID", updORG_TdMStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updORG_TdMStaff.DimissionTag);
    	SetParam(":VSTAFF_GROUP_ID", updORG_TdMStaff.StaffGroupId);
    	SetParam(":VCUST_HOBYY", updORG_TdMStaff.CustHobyy);
    	SetParam(":VREMARK", updORG_TdMStaff.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    } 	
    else if( updBy == UPD_UPD_STAFF_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updORG_TdMStaff.DepartId);
    	SetParam(":VSTAFF_NAME", updORG_TdMStaff.StaffName);
    	SetParam(":VJOB_INFO", updORG_TdMStaff.JobInfo);
    	SetParam(":VMANAGER_INFO", updORG_TdMStaff.ManagerInfo);
    	SetParam(":VSEX", updORG_TdMStaff.Sex);
    	SetParam(":VEMAIL", updORG_TdMStaff.Email);
    	SetParam(":VUSER_PID", updORG_TdMStaff.UserPid);
    	SetParam(":VSERIAL_NUMBER", updORG_TdMStaff.SerialNumber);
    	SetParam(":VCUST_ID", updORG_TdMStaff.CustId);
    	SetParam(":VDIMISSION_TAG", updORG_TdMStaff.DimissionTag);
    	SetParam(":VCUST_HOBYY", updORG_TdMStaff.CustHobyy);
    	SetParam(":VREMARK", updORG_TdMStaff.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMStaff.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMStaff.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    } 	
    else if( updBy == UPD_DEL_STAFF_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDIMISSION_TAG", updORG_TdMStaff.DimissionTag);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMStaff.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMStaff.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VSTAFF_ID", updORG_TdMStaff.StaffId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::ORG_TdMStaffDAO::jdelete(const string &delBy, ORG_TdMStaff &delORG_TdMStaff)
 *  删除方法，实现TD_M_STAFF表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delORG_TdMStaff  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMStaffDAO::ORG_jdelete(const string &delBy, ORG_TdMStaff &delORG_TdMStaff)
{
	if( delBy == DEL_IDX_TD_M_STAFF_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VDEPART_ID", delORG_TdMStaff.DepartId);
	} 	
	else if( delBy == DEL_IDX_TD_M_STAFF_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VSTAFF_GROUP_ID", delORG_TdMStaff.StaffGroupId);
	} 	
	else if( delBy == DEL_PK_TD_M_STAFF) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VSTAFF_ID", delORG_TdMStaff.StaffId);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::ORG_TdMStaffDAO::jinsert(ORG_TdMStaff &insORG_TdMStaff)
 *  插入方法，实现TD_M_STAFF表的插入语句的封装
 *  @param insORG_TdMStaff 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMStaffDAO::ORG_jinsert(ORG_TdMStaff &insORG_TdMStaff)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VSTAFF_ID", insORG_TdMStaff.StaffId);
    SetParam(":VDEPART_ID", insORG_TdMStaff.DepartId);
    SetParam(":VSTAFF_NAME", insORG_TdMStaff.StaffName);
    SetParam(":VJOB_INFO", insORG_TdMStaff.JobInfo);
    SetParam(":VMANAGER_INFO", insORG_TdMStaff.ManagerInfo);
    SetParam(":VSEX", insORG_TdMStaff.Sex);
    SetParam(":VEMAIL", insORG_TdMStaff.Email);
    SetParam(":VUSER_PID", insORG_TdMStaff.UserPid);
    SetParam(":VSERIAL_NUMBER", insORG_TdMStaff.SerialNumber);
    SetParam(":VCUST_ID", insORG_TdMStaff.CustId);
    SetParam(":VDIMISSION_TAG", insORG_TdMStaff.DimissionTag);
    SetParam(":VSTAFF_GROUP_ID", insORG_TdMStaff.StaffGroupId);
    SetParam(":VCUST_HOBYY", insORG_TdMStaff.CustHobyy);
    SetParam(":VREMARK", insORG_TdMStaff.Remark);
    SetParam(":VRSVALUE1", insORG_TdMStaff.Rsvalue1);
    SetParam(":VRSVALUE2", insORG_TdMStaff.Rsvalue2);
    SetParam(":VUPDATE_STAFF_ID", insORG_TdMStaff.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insORG_TdMStaff.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::ORG_TdMStaffDAO::jinsert(ORG_TdMStaff &insORG_TdMStaff)
 *  插入方法，实现TD_M_STAFF表的插入语句的封装
 *  @param insORG_TdMStaff 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMStaffDAO::ORG_jinsby(const string &insBy, ORG_TdMStaff &insORG_TdMStaff)
{
    
    
    ExecInsert(insBy);

    return 0;
}

