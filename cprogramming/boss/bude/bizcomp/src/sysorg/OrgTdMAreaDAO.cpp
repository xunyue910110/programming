/**=!===============SEGMENG1==========================*/
#include "OrgTdMAreaDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class ORG_TdMArea 
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::ORG_TdMArea::ORG_TdMArea()
{
}

SysMan::ORG_TdMArea::~ORG_TdMArea()
{
}

void SysMan::ORG_TdMArea::ORG_init()
{
	AreaCode.clear();	
	AreaName.clear();	
	AreaFrame.clear();	
	ThisTag.clear();	
	UseTag.clear();	
	ExtendTag.clear();	
	OrderNo = -1;	
	UserAreaCode.clear();	
	ParentAreaCode.clear();	
	StartDate.clear();	
	EndDate.clear();	
	AreaLevel = -1;	
	ValidFlag.clear();	
	Remark.clear();	
	Rsvalue1.clear();	
	Rsvalue2.clear();	
	UpdateTime.clear();	
	UpdateStaffId.clear();	
	UpdateDepartId.clear();	
	//add by liufei
	b_ThisTag.clear();
	b_UseTag.clear();
	b_ExtendTag.clear();
	b_ValidFlag.clear();
	b_UpdateStaffId.clear();
	b_UpdateDepartId.clear();

}

/** @class ORG_TdMAreaDAO
 *  @brief TD_M_AREA表的数据访问接口
 *  本对象对数据库TD_M_AREA表的数据库访问进行封装，提供所有TD_M_AREA表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::ORG_TdMAreaDAO::ORG_TdMAreaDAO(): AreaDAO()
{
	//
}

SysMan::ORG_TdMAreaDAO::~ORG_TdMAreaDAO ()
{
}

/** \fn SysMan::VPORG_TdMArea SysMan::ORG_TdMAreaDAO::jselect(const string &selBy, ORG_TdMArea &selORG_TdMArea)
 *  查询方法，实现TD_M_AREA表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selORG_TdMArea  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMArea SysMan::ORG_TdMAreaDAO::ORG_jselect(const string &selBy, ORG_TdMArea &selORG_TdMArea)
{
    SysMan::VPORG_TdMArea  vpRetORG_TdMArea; 

    if( selBy == SEL_IDX_TD_M_AREA_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_FRAME", selORG_TdMArea.AreaFrame);
    } 	
    else if( selBy == SEL_IDX_TD_M_AREA_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selORG_TdMArea.UserAreaCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_AREA_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", selORG_TdMArea.AreaLevel);
    } 	
    else if( selBy == SEL_PK_TD_M_AREA) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selORG_TdMArea.AreaCode);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMArea *pTmpORG_TdMArea = new ORG_TdMArea();
        ORG_TdMArea pTmpORG_TdMArea;
        
        pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        pTmpORG_TdMArea.Remark = GetString("REMARK");
        pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
		
		vpRetORG_TdMArea.push_back(pTmpORG_TdMArea);
    }
    
    return vpRetORG_TdMArea;
}
 
/** \fn SysMan::VPORG_TdMArea SysMan::ORG_TdMAreaDAO::jselect(const string &selBy, ORG_TdMArea &selORG_TdMArea)
 *  查询方法，实现TD_M_AREA表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selORG_TdMArea  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPORG_TdMArea SysMan::ORG_TdMAreaDAO::ORG_jselby(const string &selBy, ORG_TdMArea &selORG_TdMArea)
{
    SysMan::VPORG_TdMArea  vpRetORG_TdMArea; 

    if( selBy == SEL_AREA_0) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 	
    else if( selBy == SEL_AREA_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", selORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 	
    else if( selBy == SEL_AREA_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selORG_TdMArea.AreaCode);
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 	
    else if( selBy == SEL_AREA_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selORG_TdMArea.UserAreaCode);
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 	
    else if( selBy == SEL_AREA_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selORG_TdMArea.AreaCode);
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 	
    else if( selBy == SEL_AREA_5) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selORG_TdMArea.UserAreaCode);
    	SetParam(":VVALIDFLAG", selORG_TdMArea.ValidFlag);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        ORG_TdMArea *pTmpORG_TdMArea = new ORG_TdMArea();
        ORG_TdMArea pTmpORG_TdMArea;
        
        if( selBy ==SEL_AREA_0) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        	
        } 	
        else if( selBy ==SEL_AREA_1) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_AREA_2) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_AREA_4) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_AREA_3) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 	
        else if( selBy ==SEL_AREA_5) 
        { 
        	pTmpORG_TdMArea.AreaCode = GetString("AREA_CODE");
        	pTmpORG_TdMArea.AreaName = GetString("AREA_NAME");
        	pTmpORG_TdMArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpORG_TdMArea.ThisTag = GetString("THIS_TAG");
        	pTmpORG_TdMArea.UseTag = GetString("USE_TAG");
        	pTmpORG_TdMArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpORG_TdMArea.OrderNo = GetInt("ORDER_NO");
        	pTmpORG_TdMArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpORG_TdMArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpORG_TdMArea.StartDate = GetString("START_DATE");
        	pTmpORG_TdMArea.EndDate = GetString("END_DATE");
        	pTmpORG_TdMArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpORG_TdMArea.ValidFlag = GetString("VALIDFLAG");
        	pTmpORG_TdMArea.Remark = GetString("REMARK");
        	pTmpORG_TdMArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpORG_TdMArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpORG_TdMArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpORG_TdMArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpORG_TdMArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        	
        	//add by liufei
        	pTmpORG_TdMArea.b_ThisTag = GetString("B_THISTAG");
        	pTmpORG_TdMArea.b_UseTag = GetString("B_USETAG");
        	pTmpORG_TdMArea.b_ExtendTag = GetString("B_EXTENDTAG");
        	pTmpORG_TdMArea.b_ValidFlag = GetString("B_VALIDFLAG");
        	pTmpORG_TdMArea.b_UpdateStaffId = GetString("B_UPDATESTAFFID");
        	pTmpORG_TdMArea.b_UpdateDepartId = GetString("B_UPDATEDEPARTID");
        } 
		
		vpRetORG_TdMArea.push_back(pTmpORG_TdMArea);
    }
    
    return vpRetORG_TdMArea;
}

/** \fn int SysMan::ORG_TdMAreaDAO::jupdate(const string &updBy, ORG_TdMArea &updORG_TdMArea) 
 *  更新方法，实现TD_M_AREA表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updORG_TdMArea  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::ORG_TdMAreaDAO::ORG_jupdate(const string &updBy, ORG_TdMArea &updORG_TdMArea)
{
    if( updBy == UPD_IDX_TD_M_AREA_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    	SetParam(":VAREA_NAME", updORG_TdMArea.AreaName);
    	SetParam(":VAREA_FRAME", updORG_TdMArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updORG_TdMArea.ThisTag);
    	SetParam(":VUSE_TAG", updORG_TdMArea.UseTag);
    	SetParam(":VEXTEND_TAG", updORG_TdMArea.ExtendTag);
    	SetParam(":VORDER_NO", updORG_TdMArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updORG_TdMArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VREMARK", updORG_TdMArea.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_FRAME", updORG_TdMArea.AreaFrame);
    } 	
    else if( updBy == UPD_IDX_TD_M_AREA_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    	SetParam(":VAREA_NAME", updORG_TdMArea.AreaName);
    	SetParam(":VAREA_FRAME", updORG_TdMArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updORG_TdMArea.ThisTag);
    	SetParam(":VUSE_TAG", updORG_TdMArea.UseTag);
    	SetParam(":VEXTEND_TAG", updORG_TdMArea.ExtendTag);
    	SetParam(":VORDER_NO", updORG_TdMArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updORG_TdMArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VREMARK", updORG_TdMArea.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_AREA_3) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    	SetParam(":VAREA_NAME", updORG_TdMArea.AreaName);
    	SetParam(":VAREA_FRAME", updORG_TdMArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updORG_TdMArea.ThisTag);
    	SetParam(":VUSE_TAG", updORG_TdMArea.UseTag);
    	SetParam(":VEXTEND_TAG", updORG_TdMArea.ExtendTag);
    	SetParam(":VORDER_NO", updORG_TdMArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updORG_TdMArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VREMARK", updORG_TdMArea.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    } 	
    else if( updBy == UPD_PK_TD_M_AREA) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    	SetParam(":VAREA_NAME", updORG_TdMArea.AreaName);
    	SetParam(":VAREA_FRAME", updORG_TdMArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updORG_TdMArea.ThisTag);
    	SetParam(":VUSE_TAG", updORG_TdMArea.UseTag);
    	SetParam(":VEXTEND_TAG", updORG_TdMArea.ExtendTag);
    	SetParam(":VORDER_NO", updORG_TdMArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updORG_TdMArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VREMARK", updORG_TdMArea.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    } 	
    else if( updBy == UPD_UPD_AREA_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_NAME", updORG_TdMArea.AreaName);
    	SetParam(":VTHIS_TAG", updORG_TdMArea.ThisTag);
    	SetParam(":VUSE_TAG", updORG_TdMArea.UseTag);
    	SetParam(":VEXTEND_TAG", updORG_TdMArea.ExtendTag);
    	SetParam(":VORDER_NO", updORG_TdMArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updORG_TdMArea.UserAreaCode);
    	SetParam(":VAREA_LEVEL", updORG_TdMArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VREMARK", updORG_TdMArea.Remark);
    	SetParam(":VRSVALUE1", updORG_TdMArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updORG_TdMArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    } 	
    else if( updBy == UPD_DEL_AREA_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VVALIDFLAG", updORG_TdMArea.ValidFlag);
    	SetParam(":VUPDATE_STAFF_ID", updORG_TdMArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updORG_TdMArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updORG_TdMArea.AreaCode);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::ORG_TdMAreaDAO::jdelete(const string &delBy, ORG_TdMArea &delORG_TdMArea)
 *  删除方法，实现TD_M_AREA表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delORG_TdMArea  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMAreaDAO::ORG_jdelete(const string &delBy, ORG_TdMArea &delORG_TdMArea)
{
	if( delBy == DEL_IDX_TD_M_AREA_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_FRAME", delORG_TdMArea.AreaFrame);
	} 	
	else if( delBy == DEL_IDX_TD_M_AREA_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VUSER_AREA_CODE", delORG_TdMArea.UserAreaCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_AREA_3) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_LEVEL", delORG_TdMArea.AreaLevel);
	} 	
	else if( delBy == DEL_PK_TD_M_AREA) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_CODE", delORG_TdMArea.AreaCode);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::ORG_TdMAreaDAO::jinsert(ORG_TdMArea &insORG_TdMArea)
 *  插入方法，实现TD_M_AREA表的插入语句的封装
 *  @param insORG_TdMArea 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMAreaDAO::ORG_jinsert(ORG_TdMArea &insORG_TdMArea)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VAREA_CODE", insORG_TdMArea.AreaCode);
    SetParam(":VAREA_NAME", insORG_TdMArea.AreaName);
    SetParam(":VAREA_FRAME", insORG_TdMArea.AreaFrame);
    SetParam(":VTHIS_TAG", insORG_TdMArea.ThisTag);
    SetParam(":VUSE_TAG", insORG_TdMArea.UseTag);
    SetParam(":VEXTEND_TAG", insORG_TdMArea.ExtendTag);
    SetParam(":VORDER_NO", insORG_TdMArea.OrderNo);
    SetParam(":VUSER_AREA_CODE", insORG_TdMArea.UserAreaCode);
    SetParam(":VPARENT_AREA_CODE", insORG_TdMArea.ParentAreaCode);
    SetParam(":VAREA_LEVEL", insORG_TdMArea.AreaLevel);
    SetParam(":VVALIDFLAG", insORG_TdMArea.ValidFlag);
    SetParam(":VREMARK", insORG_TdMArea.Remark);
    SetParam(":VRSVALUE1", insORG_TdMArea.Rsvalue1);
    SetParam(":VRSVALUE2", insORG_TdMArea.Rsvalue2);
    SetParam(":VUPDATE_STAFF_ID", insORG_TdMArea.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insORG_TdMArea.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::ORG_TdMAreaDAO::jinsert(ORG_TdMArea &insORG_TdMArea)
 *  插入方法，实现TD_M_AREA表的插入语句的封装
 *  @param insORG_TdMArea 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::ORG_TdMAreaDAO::ORG_jinsby(const string &insBy, ORG_TdMArea &insORG_TdMArea)
{
 
     
    ExecInsert(insBy);

    return 0;
}

