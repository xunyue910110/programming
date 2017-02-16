/**=!===============SEGMENG1==========================*/
#include "TdMDepartDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class Depart 
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::Depart::Depart()
{
}

SysMan::Depart::~Depart()
{
}

void SysMan::Depart::init()
{
	DepartId.clear();	
	DepartCode.clear();	
	DepartName.clear();	
	DepartKindCode.clear();	
	DepartFrame.clear();	
	Validflag.clear();	
	AreaCode.clear();	
	ParentDepartId.clear();	
	ManagerId.clear();	
	OrderNo = -1;	
	UserDepartCode.clear();	
	StartDate.clear();	
	EndDate.clear();	
	DepartLevel = -1;	
	Remark.clear();	
	Rsvalue1.clear();	
	Rsvalue2.clear();	
	Rsvalue3.clear();	
	Rsvalue4.clear();	
	UpdateTime.clear();	
	UpdateStaffId.clear();	
	UpdateDepartId.clear();	
}

/** @class DepartDAO
 *  @brief TD_M_DEPART表的数据访问接口
 *  本对象对数据库TD_M_DEPART表的数据库访问进行封装，提供所有TD_M_DEPART表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::DepartDAO::DepartDAO(): MTable("TD_M_DEPART")
{
	//
}

SysMan::DepartDAO::~DepartDAO ()
{
}

/** \fn SysMan::VPDepart SysMan::DepartDAO::jselect(const string &selBy, Depart &selDepart)
 *  查询方法，实现TD_M_DEPART表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selDepart  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPDepart SysMan::DepartDAO::jselect(const string &selBy, Depart &selDepart)
{
    SysMan::VPDepart  vpRetDepart; 

    if( selBy == SEL_IDX_TD_M_DEPART_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_CODE", selDepart.DepartCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_DEPART_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_DEPART_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_FRAME", selDepart.DepartFrame);
    } 	
    else if( selBy == SEL_IDX_TD_M_DEPART_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_DEPART_CODE", selDepart.UserDepartCode);
    } 	
    else if( selBy == SEL_PK_TD_M_DEPART) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    } 	
    else if( selBy == SEL_DEPART_0) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_KIND_CODE", selDepart.DepartKindCode);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    } 	
    else if( selBy == SEL_DEPART_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    	SetParam(":VDEPART_LEVEL", selDepart.DepartLevel);
    } 	
    else if( selBy == SEL_DEPART_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    	SetParam(":VDEPART_FRAME", selDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    } 	
    else if( selBy == SEL_DEPART_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    } 	
    else if( selBy == SEL_DEPART_5) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_DEPART_CODE", selDepart.UserDepartCode);
    } 	
    else if( selBy == SEL_DEPART_6) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    } 	
    else if( selBy == SEL_DEPART_7) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", selDepart.ParentDepartId);
    } 	
    else if( selBy == SEL_DEPART_8) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VAREA_CODE", selDepart.AreaCode);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Depart *pTmpDepart = new Depart();
        Depart pTmpDepart;
        
        pTmpDepart.DepartId = GetString("DEPART_ID");
        pTmpDepart.DepartCode = GetString("DEPART_CODE");
        pTmpDepart.DepartName = GetString("DEPART_NAME");
        pTmpDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        pTmpDepart.DepartFrame = GetString("DEPART_FRAME");
        pTmpDepart.Validflag = GetString("VALIDFLAG");
        pTmpDepart.AreaCode = GetString("AREA_CODE");
        pTmpDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        pTmpDepart.ManagerId = GetString("MANAGER_ID");
        pTmpDepart.OrderNo = GetInt("ORDER_NO");
        pTmpDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        pTmpDepart.StartDate = GetString("START_DATE");
        pTmpDepart.EndDate = GetString("END_DATE");
        pTmpDepart.DepartLevel = GetInt("DEPART_LEVEL");
        pTmpDepart.Remark = GetString("REMARK");
        pTmpDepart.Rsvalue1 = GetString("RSVALUE1");
        pTmpDepart.Rsvalue2 = GetString("RSVALUE2");
        pTmpDepart.Rsvalue3 = GetString("RSVALUE3");
        pTmpDepart.Rsvalue4 = GetString("RSVALUE4");
        pTmpDepart.UpdateTime = GetString("UPDATE_TIME");
        pTmpDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
		
		vpRetDepart.push_back(pTmpDepart);
    }
    
    return vpRetDepart;
}
 
/** \fn SysMan::VPDepart SysMan::DepartDAO::jselect(const string &selBy, Depart &selDepart)
 *  查询方法，实现TD_M_DEPART表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selDepart  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPDepart SysMan::DepartDAO::jselby(const string &selBy, Depart &selDepart)
{
    SysMan::VPDepart  vpRetDepart; 

    if( selBy == SEL_DEPART_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    } 	
    else if( selBy == SEL_DEPART_9) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_KIND_CODE", selDepart.DepartKindCode);
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    } 	
    else if( selBy == SEL_DEPART_10) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VVALIDFLAG", selDepart.Validflag);
    	SetParam(":VRSVALUE2", selDepart.Rsvalue2);
    } 	
    else if( selBy == SEL_DEPART_11) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", selDepart.DepartId);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Depart *pTmpDepart = new Depart();
        Depart pTmpDepart;
        
        if( selBy ==SEL_DEPART_2) 
        { 
        	pTmpDepart.DepartId = GetString("DEPART_ID");
        	pTmpDepart.DepartCode = GetString("DEPART_CODE");
        	pTmpDepart.DepartName = GetString("DEPART_NAME");
        	pTmpDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        	pTmpDepart.DepartFrame = GetString("DEPART_FRAME");
        	pTmpDepart.Validflag = GetString("VALIDFLAG");
        	pTmpDepart.AreaCode = GetString("AREA_CODE");
        	pTmpDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        	pTmpDepart.ManagerId = GetString("MANAGER_ID");
        	pTmpDepart.OrderNo = GetInt("ORDER_NO");
        	pTmpDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        	pTmpDepart.StartDate = GetString("START_DATE");
        	pTmpDepart.EndDate = GetString("END_DATE");
        	pTmpDepart.DepartLevel = GetInt("DEPART_LEVEL");
        	pTmpDepart.Remark = GetString("REMARK");
        	pTmpDepart.Rsvalue1 = GetString("RSVALUE1");
        	pTmpDepart.Rsvalue2 = GetString("RSVALUE2");
        	pTmpDepart.Rsvalue3 = GetString("RSVALUE3");
        	pTmpDepart.Rsvalue4 = GetString("RSVALUE4");
        	pTmpDepart.UpdateTime = GetString("UPDATE_TIME");
        	pTmpDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_DEPART_9) 
        { 
        	pTmpDepart.DepartId = GetString("DEPART_ID");
        	pTmpDepart.DepartCode = GetString("DEPART_CODE");
        	pTmpDepart.DepartName = GetString("DEPART_NAME");
        	pTmpDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        	pTmpDepart.DepartFrame = GetString("DEPART_FRAME");
        	pTmpDepart.Validflag = GetString("VALIDFLAG");
        	pTmpDepart.AreaCode = GetString("AREA_CODE");
        	pTmpDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        	pTmpDepart.ManagerId = GetString("MANAGER_ID");
        	pTmpDepart.OrderNo = GetInt("ORDER_NO");
        	pTmpDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        	pTmpDepart.StartDate = GetString("START_DATE");
        	pTmpDepart.EndDate = GetString("END_DATE");
        	pTmpDepart.DepartLevel = GetInt("DEPART_LEVEL");
        	pTmpDepart.Remark = GetString("REMARK");
        	pTmpDepart.Rsvalue1 = GetString("RSVALUE1");
        	pTmpDepart.Rsvalue2 = GetString("RSVALUE2");
        	pTmpDepart.Rsvalue3 = GetString("RSVALUE3");
        	pTmpDepart.Rsvalue4 = GetString("RSVALUE4");
        	pTmpDepart.UpdateTime = GetString("UPDATE_TIME");
        	pTmpDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_DEPART_10) 
        { 
        	pTmpDepart.DepartId = GetString("DEPART_ID");
        	pTmpDepart.DepartCode = GetString("DEPART_CODE");
        	pTmpDepart.DepartName = GetString("DEPART_NAME");
        	pTmpDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        	pTmpDepart.DepartFrame = GetString("DEPART_FRAME");
        	pTmpDepart.Validflag = GetString("VALIDFLAG");
        	pTmpDepart.AreaCode = GetString("AREA_CODE");
        	pTmpDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        	pTmpDepart.ManagerId = GetString("MANAGER_ID");
        	pTmpDepart.OrderNo = GetInt("ORDER_NO");
        	pTmpDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        	pTmpDepart.StartDate = GetString("START_DATE");
        	pTmpDepart.EndDate = GetString("END_DATE");
        	pTmpDepart.DepartLevel = GetInt("DEPART_LEVEL");
        	pTmpDepart.Remark = GetString("REMARK");
        	pTmpDepart.Rsvalue1 = GetString("RSVALUE1");
        	pTmpDepart.Rsvalue2 = GetString("RSVALUE2");
        	pTmpDepart.Rsvalue3 = GetString("RSVALUE3");
        	pTmpDepart.Rsvalue4 = GetString("RSVALUE4");
        	pTmpDepart.UpdateTime = GetString("UPDATE_TIME");
        	pTmpDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_DEPART_11) 
        { 
        	pTmpDepart.DepartId = GetString("DEPART_ID");
        	pTmpDepart.DepartCode = GetString("DEPART_CODE");
        	pTmpDepart.DepartName = GetString("DEPART_NAME");
        	pTmpDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        	pTmpDepart.DepartFrame = GetString("DEPART_FRAME");
        	pTmpDepart.Validflag = GetString("VALIDFLAG");
        	pTmpDepart.AreaCode = GetString("AREA_CODE");
        	pTmpDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        	pTmpDepart.ManagerId = GetString("MANAGER_ID");
        	pTmpDepart.OrderNo = GetInt("ORDER_NO");
        	pTmpDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        	pTmpDepart.StartDate = GetString("START_DATE");
        	pTmpDepart.EndDate = GetString("END_DATE");
        	pTmpDepart.DepartLevel = GetInt("DEPART_LEVEL");
        	pTmpDepart.Remark = GetString("REMARK");
        	pTmpDepart.Rsvalue1 = GetString("RSVALUE1");
        	pTmpDepart.Rsvalue2 = GetString("RSVALUE2");
        	pTmpDepart.Rsvalue3 = GetString("RSVALUE3");
        	pTmpDepart.Rsvalue4 = GetString("RSVALUE4");
        	pTmpDepart.UpdateTime = GetString("UPDATE_TIME");
        	pTmpDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 
		
		vpRetDepart.push_back(pTmpDepart);
    }
    
    return vpRetDepart;
}

/** \fn int SysMan::DepartDAO::jupdate(const string &updBy, Depart &updDepart) 
 *  更新方法，实现TD_M_DEPART表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updDepart  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::DepartDAO::jupdate(const string &updBy, Depart &updDepart)
{
    if( updBy == UPD_IDX_TD_M_DEPART_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_DEPART_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_DEPART_3) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    } 	
    else if( updBy == UPD_IDX_TD_M_DEPART_4) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    } 	
    else if( updBy == UPD_PK_TD_M_DEPART) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    } 	
    else if( updBy == UPD_UPD_DEPART_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VDEPART_CODE", updDepart.DepartCode);
    	SetParam(":VDEPART_NAME", updDepart.DepartName);
    	SetParam(":VDEPART_KIND_CODE", updDepart.DepartKindCode);
    	SetParam(":VDEPART_FRAME", updDepart.DepartFrame);
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VAREA_CODE", updDepart.AreaCode);
    	SetParam(":VPARENT_DEPART_ID", updDepart.ParentDepartId);
    	SetParam(":VMANAGER_ID", updDepart.ManagerId);
    	SetParam(":VORDER_NO", updDepart.OrderNo);
    	SetParam(":VUSER_DEPART_CODE", updDepart.UserDepartCode);
    	SetParam(":VDEPART_LEVEL", updDepart.DepartLevel);
    	SetParam(":VREMARK", updDepart.Remark);
    	SetParam(":VRSVALUE1", updDepart.Rsvalue1);
    	SetParam(":VRSVALUE2", updDepart.Rsvalue2);
    	SetParam(":VRSVALUE3", updDepart.Rsvalue3);
    	SetParam(":VRSVALUE4", updDepart.Rsvalue4);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    } 	
    else if( updBy == UPD_DEL_DEPART_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VVALIDFLAG", updDepart.Validflag);
    	SetParam(":VUPDATE_STAFF_ID", updDepart.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updDepart.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VDEPART_ID", updDepart.DepartId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::DepartDAO::jdelete(const string &delBy, Depart &delDepart)
 *  删除方法，实现TD_M_DEPART表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delDepart  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::DepartDAO::jdelete(const string &delBy, Depart &delDepart)
{
	if( delBy == DEL_IDX_TD_M_DEPART_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VDEPART_CODE", delDepart.DepartCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_DEPART_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_CODE", delDepart.AreaCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_DEPART_3) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VDEPART_FRAME", delDepart.DepartFrame);
	} 	
	else if( delBy == DEL_IDX_TD_M_DEPART_4) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VUSER_DEPART_CODE", delDepart.UserDepartCode);
	} 	
	else if( delBy == DEL_PK_TD_M_DEPART) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VDEPART_ID", delDepart.DepartId);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::DepartDAO::jinsert(Depart &insDepart)
 *  插入方法，实现TD_M_DEPART表的插入语句的封装
 *  @param insDepart 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::DepartDAO::jinsert(Depart &insDepart)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VDEPART_ID", insDepart.DepartId);
    SetParam(":VDEPART_CODE", insDepart.DepartCode);
    SetParam(":VDEPART_NAME", insDepart.DepartName);
    SetParam(":VDEPART_KIND_CODE", insDepart.DepartKindCode);
    SetParam(":VDEPART_FRAME", insDepart.DepartFrame);
    SetParam(":VVALIDFLAG", insDepart.Validflag);
    SetParam(":VAREA_CODE", insDepart.AreaCode);
    SetParam(":VPARENT_DEPART_ID", insDepart.ParentDepartId);
    SetParam(":VMANAGER_ID", insDepart.ManagerId);
    SetParam(":VORDER_NO", insDepart.OrderNo);
    SetParam(":VUSER_DEPART_CODE", insDepart.UserDepartCode);
    SetParam(":VDEPART_LEVEL", insDepart.DepartLevel);
    SetParam(":VREMARK", insDepart.Remark);
    SetParam(":VRSVALUE1", insDepart.Rsvalue1);
    SetParam(":VRSVALUE2", insDepart.Rsvalue2);
    SetParam(":VRSVALUE3", insDepart.Rsvalue3);
    SetParam(":VRSVALUE4", insDepart.Rsvalue4);
    SetParam(":VUPDATE_STAFF_ID", insDepart.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insDepart.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::DepartDAO::jinsert(Depart &insDepart)
 *  插入方法，实现TD_M_DEPART表的插入语句的封装
 *  @param insDepart 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::DepartDAO::jinsby(const string &insBy, Depart &insDepart)
{
    ExecInsert(insBy);

    return 0;
}

