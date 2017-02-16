/**=!===============SEGMENG1==========================*/
#include "OrgTdMDepartDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class ORG_TdMDepart 
 *  @brief TD_M_DEPART表的映射实体
 *  本对象映射数据库TD_M_DEPART表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::ORG_TdMDepart::ORG_TdMDepart()
{
}

SysMan::ORG_TdMDepart::~ORG_TdMDepart()
{
}

void SysMan::ORG_TdMDepart::init()
{
    DepartId.clear();
    DepartCode.clear();
    DepartName.clear();
    DepartKindCode.clear();
    DepartFrame.clear();
    ValidFlag.clear();
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

//add by liufei
    b_DepartKindCode.clear();
    b_ValidFlag.clear();
    b_AreaCode.clear();
    b_ParentDepartId.clear();
    b_ManagerId.clear();
    b_UpdateStaffId.clear();
    b_UpdateDepartId.clear();
} 
  
/** @class ORG_TdMDepartDAO
 *  @brief TD_M_DEPART表的数据访问接口
 *  本对象对数据库TD_M_DEPART表的数据库访问进行封装，提供所有TD_M_DEPART表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::ORG_TdMDepartDAO::ORG_TdMDepartDAO(): DepartDAO()
{
//
}

SysMan::ORG_TdMDepartDAO::~ORG_TdMDepartDAO ()
{
}

/** \fn SysMan::VPORG_TdMDepart SysMan::ORG_TdMDepartDAO::ORG_jselect(const string &selBy, ORG_TdMDepart &selORG_TdMDepart)
 *  查询方法，实现TD_M_DEPART表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy查询条件定义
 *  @param  selORG_TdMDepart  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMDepart SysMan::ORG_TdMDepartDAO::ORG_jselect(const string &selBy, ORG_TdMDepart &selORG_TdMDepart)
{
    SysMan::VPORG_TdMDepart  vpRetORG_TdMDepart; 

    if( selBy == SEL_IDX_TD_M_DEPART_1) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_CODE", selORG_TdMDepart.DepartCode);
    } 
    else if( selBy == SEL_IDX_TD_M_DEPART_2) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VAREA_CODE", selORG_TdMDepart.AreaCode);
    } 
    else if( selBy == SEL_IDX_TD_M_DEPART_3) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_FRAME", selORG_TdMDepart.DepartFrame);
    } 
    else if( selBy == SEL_IDX_TD_M_DEPART_4) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VUSER_DEPART_CODE", selORG_TdMDepart.UserDepartCode);
    } 
    else if( selBy == SEL_PK_TD_M_DEPART) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
    } 
    else if( selBy == SEL_DEPART_0) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_KIND_CODE", selORG_TdMDepart.DepartKindCode);
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    } 
    else if( selBy == SEL_DEPART_1) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    SetParam(":VAREA_CODE", selORG_TdMDepart.AreaCode);
    SetParam(":VDEPART_LEVEL", selORG_TdMDepart.DepartLevel);
    } 
    else if( selBy == SEL_DEPART_3) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    } 
    else if( selBy == SEL_DEPART_4) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    SetParam(":VAREA_CODE", selORG_TdMDepart.AreaCode);
    } 
    else if( selBy == SEL_DEPART_5) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VUSER_DEPART_CODE", selORG_TdMDepart.UserDepartCode);
    } 
    else if( selBy == SEL_DEPART_6) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    SetParam(":VAREA_CODE", selORG_TdMDepart.AreaCode);
    } 
    else if( selBy == SEL_DEPART_7) 
    { 
    /**< 为WHERE条件字段传入条件值 */
    SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
    SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    SetParam(":VAREA_CODE", selORG_TdMDepart.AreaCode);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMDepart *pTmpORG_TdMDepart = new ORG_TdMDepart();
        ORG_TdMDepart pTmpORG_TdMDepart;
        
        pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
        pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
        pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
        pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
        pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
        pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
        pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
        pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
        pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
        pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
        pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
        pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
        pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
        pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
        pTmpORG_TdMDepart.Remark = GetString("REMARK");
        pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
        pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
        pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
        pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
        pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
        pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");

        pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
        pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
        pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
        pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
        pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
        pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");

        vpRetORG_TdMDepart.push_back(pTmpORG_TdMDepart);
    }
    
    return vpRetORG_TdMDepart;
}
 
/** \fn SysMan::VPORG_TdMDepart SysMan::ORG_TdMDepartDAO::ORG_jselect(const string &selBy, ORG_TdMDepart &selORG_TdMDepart)
 *  查询方法，实现TD_M_DEPART表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy查询条件定义
 *  @param  selORG_TdMDepart  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMDepart SysMan::ORG_TdMDepartDAO::ORG_jselby(const string &selBy, ORG_TdMDepart &selORG_TdMDepart)
{
    SysMan::VPORG_TdMDepart  vpRetORG_TdMDepart; 

    if( selBy == SEL_DEPART_2) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
    } 
    else if( selBy == SEL_DEPART_8) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", selORG_TdMDepart.DepartId);
        SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    } 
    else if( selBy == SEL_DEPART_9) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_KIND_CODE", selORG_TdMDepart.DepartKindCode);
        SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
    } 
    else if( selBy == SEL_DEPART_10) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VVALIDFLAG", selORG_TdMDepart.ValidFlag);
        SetParam(":VRSVALUE2", selORG_TdMDepart.Rsvalue2);
    } 
    else if( selBy == SEL_DEPART_11) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_CODE", selORG_TdMDepart.DepartCode);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMDepart *pTmpORG_TdMDepart = new ORG_TdMDepart();
        ORG_TdMDepart pTmpORG_TdMDepart;
        if( selBy ==SEL_DEPART_2) 
        { 
            pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
            pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
            pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
            pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
            pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
            pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
            pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
            pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
            pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
            pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
            pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
            pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
            pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
            pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
            pTmpORG_TdMDepart.Remark = GetString("REMARK");
            pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
            pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
            pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
            pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
            pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
            pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
            pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
            
            //ADD BY LIUFEI
            pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
            pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
            pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
            pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
            pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
            pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
            pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
        else if( selBy ==SEL_DEPART_8) 
        { 
            pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
            pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
            pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
            pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
            pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
            pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
            pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
            pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
            pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
            pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
            pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
            pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
            pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
            pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
            pTmpORG_TdMDepart.Remark = GetString("REMARK");
            pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
            pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
            pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
            pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
            pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
            pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
            pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
            //ADD BY LIUFEI
            pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
            pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
            pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
            pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
            pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
            pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
            pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
        else if( selBy ==SEL_DEPART_9) 
        { 
            pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
            pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
            pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
            pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
            pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
            pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
            pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
            pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
            pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
            pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
            pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
            pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
            pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
            pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
            pTmpORG_TdMDepart.Remark = GetString("REMARK");
            pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
            pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
            pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
            pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
            pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
            pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
            pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
            //ADD BY LIUFEI
            pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
            pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
            pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
            pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
            pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
            pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
            pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
        else if( selBy ==SEL_DEPART_10) 
        { 
            pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
            pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
            pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
            pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
            pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
            pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
            pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
            pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
            pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
            pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
            pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
            pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
            pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
            pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
            pTmpORG_TdMDepart.Remark = GetString("REMARK");
            pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
            pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
            pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
            pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
            pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
            pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
            pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
            //ADD BY LIUFEI
            pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
            pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
            pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
            pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
            pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
            pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
            pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
        else if( selBy ==SEL_DEPART_11) 
        { 
            pTmpORG_TdMDepart.DepartId = GetString("DEPART_ID");
            pTmpORG_TdMDepart.DepartCode = GetString("DEPART_CODE");
            pTmpORG_TdMDepart.DepartName = GetString("DEPART_NAME");
            pTmpORG_TdMDepart.DepartKindCode = GetString("DEPART_KIND_CODE");
            pTmpORG_TdMDepart.DepartFrame = GetString("DEPART_FRAME");
            pTmpORG_TdMDepart.ValidFlag = GetString("VALIDFLAG");
            pTmpORG_TdMDepart.AreaCode = GetString("AREA_CODE");
            pTmpORG_TdMDepart.ParentDepartId = GetString("PARENT_DEPART_ID");
            pTmpORG_TdMDepart.ManagerId = GetString("MANAGER_ID");
            pTmpORG_TdMDepart.OrderNo = GetInt("ORDER_NO");
            pTmpORG_TdMDepart.UserDepartCode = GetString("USER_DEPART_CODE");
            pTmpORG_TdMDepart.StartDate = GetString("START_DATE");
            pTmpORG_TdMDepart.EndDate = GetString("END_DATE");
            pTmpORG_TdMDepart.DepartLevel = GetInt("DEPART_LEVEL");
            pTmpORG_TdMDepart.Remark = GetString("REMARK");
            pTmpORG_TdMDepart.Rsvalue1 = GetString("RSVALUE1");
            pTmpORG_TdMDepart.Rsvalue2 = GetString("RSVALUE2");
            pTmpORG_TdMDepart.Rsvalue3 = GetString("RSVALUE3");
            pTmpORG_TdMDepart.Rsvalue4 = GetString("RSVALUE4");
            pTmpORG_TdMDepart.UpdateTime = GetString("UPDATE_TIME");
            pTmpORG_TdMDepart.UpdateStaffId = GetString("UPDATE_STAFF_ID");
            pTmpORG_TdMDepart.UpdateDepartId = GetString("UPDATE_DEPART_ID");
            //ADD BY LIUFEI
            pTmpORG_TdMDepart.b_DepartKindCode = GetString("B_DEPARTKINDCODE");
            pTmpORG_TdMDepart.b_ValidFlag = GetString("B_VALIDFLAG");
            pTmpORG_TdMDepart.b_AreaCode = GetString("B_AREACODE");
            pTmpORG_TdMDepart.b_ParentDepartId = GetString("B_PARENTDEPARTID");
            pTmpORG_TdMDepart.b_ManagerId = GetString("B_MANAGERID");
            pTmpORG_TdMDepart.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
            pTmpORG_TdMDepart.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 

        vpRetORG_TdMDepart.push_back(pTmpORG_TdMDepart);
    }
    
    return vpRetORG_TdMDepart;
}

/** \fn int SysMan::ORG_TdMDepartDAO::ORG_jupdate(const string &updBy, ORG_TdMDepart &updORG_TdMDepart) 
 *  更新方法，实现TD_M_DEPART表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updORG_TdMDepart  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::ORG_TdMDepartDAO::ORG_jupdate(const string &updBy, ORG_TdMDepart &updORG_TdMDepart)
{
    if( updBy == UPD_IDX_TD_M_DEPART_1) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
    } 
    else if( updBy == UPD_IDX_TD_M_DEPART_2) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
    } 
    else if( updBy == UPD_IDX_TD_M_DEPART_3) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
    } 
    else if( updBy == UPD_IDX_TD_M_DEPART_4) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
    } 
    else if( updBy == UPD_PK_TD_M_DEPART) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
    } 
    else if( updBy == UPD_UPD_DEPART_0) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VDEPART_CODE", updORG_TdMDepart.DepartCode);
        SetParam(":VDEPART_NAME", updORG_TdMDepart.DepartName);
        SetParam(":VDEPART_KIND_CODE", updORG_TdMDepart.DepartKindCode);
        SetParam(":VDEPART_FRAME", updORG_TdMDepart.DepartFrame);
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VAREA_CODE", updORG_TdMDepart.AreaCode);
        SetParam(":VPARENT_DEPART_ID", updORG_TdMDepart.ParentDepartId);
        SetParam(":VMANAGER_ID", updORG_TdMDepart.ManagerId);
        SetParam(":VORDER_NO", updORG_TdMDepart.OrderNo);
        SetParam(":VUSER_DEPART_CODE", updORG_TdMDepart.UserDepartCode);
        SetParam(":VDEPART_LEVEL", updORG_TdMDepart.DepartLevel);
        SetParam(":VREMARK", updORG_TdMDepart.Remark);
        SetParam(":VRSVALUE1", updORG_TdMDepart.Rsvalue1);
        SetParam(":VRSVALUE2", updORG_TdMDepart.Rsvalue2);
        SetParam(":VRSVALUE3", updORG_TdMDepart.Rsvalue3);
        SetParam(":VRSVALUE4", updORG_TdMDepart.Rsvalue4);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
    } 
    else if( updBy == UPD_DEL_DEPART_0) 
    { 
        /**< 为UPDATE字段传入更新值 */
        SetParam(":VVALIDFLAG", updORG_TdMDepart.ValidFlag);
        SetParam(":VUPDATE_STAFF_ID", updORG_TdMDepart.UpdateStaffId);
        SetParam(":VUPDATE_DEPART_ID", updORG_TdMDepart.UpdateDepartId);
        
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", updORG_TdMDepart.DepartId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::ORG_TdMDepartDAO::ORG_jdelete(const string &delBy, ORG_TdMDepart &delORG_TdMDepart)
 *  删除方法，实现TD_M_DEPART表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delORG_TdMDepart  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMDepartDAO::ORG_jdelete(const string &delBy, ORG_TdMDepart &delORG_TdMDepart)
{
    if( delBy == DEL_IDX_TD_M_DEPART_1) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_CODE", delORG_TdMDepart.DepartCode);
    } 
    else if( delBy == DEL_IDX_TD_M_DEPART_2) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VAREA_CODE", delORG_TdMDepart.AreaCode);
    } 
    else if( delBy == DEL_IDX_TD_M_DEPART_3) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_FRAME", delORG_TdMDepart.DepartFrame);
    } 
    else if( delBy == DEL_IDX_TD_M_DEPART_4) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VUSER_DEPART_CODE", delORG_TdMDepart.UserDepartCode);
    } 
    else if( delBy == DEL_PK_TD_M_DEPART) 
    { 
        /**< 为WHERE条件字段传入条件值 */
        SetParam(":VDEPART_ID", delORG_TdMDepart.DepartId);
    } 

    ExecDelete(delBy);

    return 0;
}

/** \fn int SysMan::ORG_TdMDepartDAO::ORG_jinsert(ORG_TdMDepart &insORG_TdMDepart)
 *  插入方法，实现TD_M_DEPART表的插入语句的封装
 *  @param insORG_TdMDepart 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMDepartDAO::ORG_jinsert(ORG_TdMDepart &insORG_TdMDepart)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VDEPART_ID", insORG_TdMDepart.DepartId);
    SetParam(":VDEPART_CODE", insORG_TdMDepart.DepartCode);
    SetParam(":VDEPART_NAME", insORG_TdMDepart.DepartName);
    SetParam(":VDEPART_KIND_CODE", insORG_TdMDepart.DepartKindCode);
    SetParam(":VDEPART_FRAME", insORG_TdMDepart.DepartFrame);
    SetParam(":VVALIDFLAG", insORG_TdMDepart.ValidFlag);
    SetParam(":VAREA_CODE", insORG_TdMDepart.AreaCode);
    SetParam(":VPARENT_DEPART_ID", insORG_TdMDepart.ParentDepartId);
    SetParam(":VMANAGER_ID", insORG_TdMDepart.ManagerId);
    SetParam(":VORDER_NO", insORG_TdMDepart.OrderNo);
    SetParam(":VUSER_DEPART_CODE", insORG_TdMDepart.UserDepartCode);
    SetParam(":VDEPART_LEVEL", insORG_TdMDepart.DepartLevel);
    SetParam(":VREMARK", insORG_TdMDepart.Remark);
    SetParam(":VRSVALUE1", insORG_TdMDepart.Rsvalue1);
    SetParam(":VRSVALUE2", insORG_TdMDepart.Rsvalue2);
    SetParam(":VRSVALUE3", insORG_TdMDepart.Rsvalue3);
    SetParam(":VRSVALUE4", insORG_TdMDepart.Rsvalue4);
    SetParam(":VUPDATE_STAFF_ID", insORG_TdMDepart.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insORG_TdMDepart.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::ORG_TdMDepartDAO::ORG_jinsert(ORG_TdMDepart &insORG_TdMDepart)
 *  插入方法，实现TD_M_DEPART表的插入语句的封装
 *  @param insORG_TdMDepart 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMDepartDAO::ORG_jinsby(const string &insBy, ORG_TdMDepart &insORG_TdMDepart)
{
    
    ExecInsert(insBy);

    return 0;
}

