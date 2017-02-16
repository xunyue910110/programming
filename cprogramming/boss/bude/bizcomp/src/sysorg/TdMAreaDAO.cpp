/**=!===============SEGMENG1==========================*/
#include "TdMAreaDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class Area 
 *  @brief TD_M_AREA表的映射实体
 *  本对象映射数据库TD_M_AREA表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::Area::Area()
{
}

SysMan::Area::~Area()
{
}

void SysMan::Area::init()
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
	Validflag.clear();	
	Remark.clear();	
	Rsvalue1.clear();	
	Rsvalue2.clear();	
	UpdateTime.clear();	
	UpdateStaffId.clear();	
	UpdateDepartId.clear();	
}

/** @class AreaDAO
 *  @brief TD_M_AREA表的数据访问接口
 *  本对象对数据库TD_M_AREA表的数据库访问进行封装，提供所有TD_M_AREA表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::AreaDAO::AreaDAO(): MTable("TD_M_AREA")
{
	//
}

SysMan::AreaDAO::~AreaDAO ()
{
}

/** \fn SysMan::VPArea SysMan::AreaDAO::jselect(const string &selBy, Area &selArea)
 *  查询方法，实现TD_M_AREA表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selArea  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPArea SysMan::AreaDAO::jselect(const string &selBy, Area &selArea)
{
    SysMan::VPArea  vpRetArea; 

    if( selBy == SEL_IDX_TD_M_AREA_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_FRAME", selArea.AreaFrame);
    } 	
    else if( selBy == SEL_IDX_TD_M_AREA_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selArea.UserAreaCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_AREA_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", selArea.AreaLevel);
    } 	
    else if( selBy == SEL_PK_TD_M_AREA) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selArea.AreaCode);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Area *pTmpArea = new Area();
        Area pTmpArea;
        
        pTmpArea.AreaCode = GetString("AREA_CODE");
        pTmpArea.AreaName = GetString("AREA_NAME");
        pTmpArea.AreaFrame = GetString("AREA_FRAME");
        pTmpArea.ThisTag = GetString("THIS_TAG");
        pTmpArea.UseTag = GetString("USE_TAG");
        pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        pTmpArea.OrderNo = GetInt("ORDER_NO");
        pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        pTmpArea.StartDate = GetString("START_DATE");
        pTmpArea.EndDate = GetString("END_DATE");
        pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        pTmpArea.Validflag = GetString("VALIDFLAG");
        pTmpArea.Remark = GetString("REMARK");
        pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
		
		vpRetArea.push_back(pTmpArea);
    }
    
    return vpRetArea;
}
 
/** \fn SysMan::VPArea SysMan::AreaDAO::jselect(const string &selBy, Area &selArea)
 *  查询方法，实现TD_M_AREA表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selArea  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPArea SysMan::AreaDAO::jselby(const string &selBy, Area &selArea)
{
    SysMan::VPArea  vpRetArea; 

    if( selBy == SEL_AREA_0) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 	
    else if( selBy == SEL_AREA_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", selArea.AreaLevel);
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 	
    else if( selBy == SEL_AREA_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selArea.AreaCode);
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 	
    else if( selBy == SEL_AREA_4) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selArea.UserAreaCode);
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 	
    else if( selBy == SEL_AREA_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", selArea.AreaCode);
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 	
    else if( selBy == SEL_AREA_5) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", selArea.UserAreaCode);
    	SetParam(":VVALIDFLAG", selArea.Validflag);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        Area *pTmpArea = new Area();
        Area pTmpArea;
        if( selBy ==SEL_AREA_0) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_AREA_1) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_AREA_2) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_AREA_4) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_AREA_3) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 	
        else if( selBy ==SEL_AREA_5) 
        { 
        	pTmpArea.AreaCode = GetString("AREA_CODE");
        	pTmpArea.AreaName = GetString("AREA_NAME");
        	pTmpArea.AreaFrame = GetString("AREA_FRAME");
        	pTmpArea.ThisTag = GetString("THIS_TAG");
        	pTmpArea.UseTag = GetString("USE_TAG");
        	pTmpArea.ExtendTag = GetString("EXTEND_TAG");
        	pTmpArea.OrderNo = GetInt("ORDER_NO");
        	pTmpArea.UserAreaCode = GetString("USER_AREA_CODE");
        	pTmpArea.ParentAreaCode = GetString("PARENT_AREA_CODE");
        	pTmpArea.StartDate = GetString("START_DATE");
        	pTmpArea.EndDate = GetString("END_DATE");
        	pTmpArea.AreaLevel = GetInt("AREA_LEVEL");
        	pTmpArea.Validflag = GetString("VALIDFLAG");
        	pTmpArea.Remark = GetString("REMARK");
        	pTmpArea.Rsvalue1 = GetString("RSVALUE1");
        	pTmpArea.Rsvalue2 = GetString("RSVALUE2");
        	pTmpArea.UpdateTime = GetString("UPDATE_TIME");
        	pTmpArea.UpdateStaffId = GetString("UPDATE_STAFF_ID");
        	pTmpArea.UpdateDepartId = GetString("UPDATE_DEPART_ID");
        } 
		
		vpRetArea.push_back(pTmpArea);
    }
    
    return vpRetArea;
}

/** \fn int SysMan::AreaDAO::jupdate(const string &updBy, Area &updArea) 
 *  更新方法，实现TD_M_AREA表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updArea  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::AreaDAO::jupdate(const string &updBy, Area &updArea)
{
    if( updBy == UPD_IDX_TD_M_AREA_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    	SetParam(":VAREA_NAME", updArea.AreaName);
    	SetParam(":VAREA_FRAME", updArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updArea.ThisTag);
    	SetParam(":VUSE_TAG", updArea.UseTag);
    	SetParam(":VEXTEND_TAG", updArea.ExtendTag);
    	SetParam(":VORDER_NO", updArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VREMARK", updArea.Remark);
    	SetParam(":VRSVALUE1", updArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_FRAME", updArea.AreaFrame);
    } 	
    else if( updBy == UPD_IDX_TD_M_AREA_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    	SetParam(":VAREA_NAME", updArea.AreaName);
    	SetParam(":VAREA_FRAME", updArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updArea.ThisTag);
    	SetParam(":VUSE_TAG", updArea.UseTag);
    	SetParam(":VEXTEND_TAG", updArea.ExtendTag);
    	SetParam(":VORDER_NO", updArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VREMARK", updArea.Remark);
    	SetParam(":VRSVALUE1", updArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_AREA_3) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    	SetParam(":VAREA_NAME", updArea.AreaName);
    	SetParam(":VAREA_FRAME", updArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updArea.ThisTag);
    	SetParam(":VUSE_TAG", updArea.UseTag);
    	SetParam(":VEXTEND_TAG", updArea.ExtendTag);
    	SetParam(":VORDER_NO", updArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VREMARK", updArea.Remark);
    	SetParam(":VRSVALUE1", updArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    } 	
    else if( updBy == UPD_PK_TD_M_AREA) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    	SetParam(":VAREA_NAME", updArea.AreaName);
    	SetParam(":VAREA_FRAME", updArea.AreaFrame);
    	SetParam(":VTHIS_TAG", updArea.ThisTag);
    	SetParam(":VUSE_TAG", updArea.UseTag);
    	SetParam(":VEXTEND_TAG", updArea.ExtendTag);
    	SetParam(":VORDER_NO", updArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    	SetParam(":VPARENT_AREA_CODE", updArea.ParentAreaCode);
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VREMARK", updArea.Remark);
    	SetParam(":VRSVALUE1", updArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    } 	
    else if( updBy == UPD_UPD_AREA_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VAREA_NAME", updArea.AreaName);
    	SetParam(":VTHIS_TAG", updArea.ThisTag);
    	SetParam(":VUSE_TAG", updArea.UseTag);
    	SetParam(":VEXTEND_TAG", updArea.ExtendTag);
    	SetParam(":VORDER_NO", updArea.OrderNo);
    	SetParam(":VUSER_AREA_CODE", updArea.UserAreaCode);
    	SetParam(":VAREA_LEVEL", updArea.AreaLevel);
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VREMARK", updArea.Remark);
    	SetParam(":VRSVALUE1", updArea.Rsvalue1);
    	SetParam(":VRSVALUE2", updArea.Rsvalue2);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    } 	
    else if( updBy == UPD_DEL_AREA_0) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VVALIDFLAG", updArea.Validflag);
    	SetParam(":VUPDATE_STAFF_ID", updArea.UpdateStaffId);
    	SetParam(":VUPDATE_DEPART_ID", updArea.UpdateDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VAREA_CODE", updArea.AreaCode);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::AreaDAO::jdelete(const string &delBy, Area &delArea)
 *  删除方法，实现TD_M_AREA表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delArea  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::AreaDAO::jdelete(const string &delBy, Area &delArea)
{
	if( delBy == DEL_IDX_TD_M_AREA_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_FRAME", delArea.AreaFrame);
	} 	
	else if( delBy == DEL_IDX_TD_M_AREA_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VUSER_AREA_CODE", delArea.UserAreaCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_AREA_3) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_LEVEL", delArea.AreaLevel);
	} 	
	else if( delBy == DEL_PK_TD_M_AREA) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VAREA_CODE", delArea.AreaCode);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::AreaDAO::jinsert(Area &insArea)
 *  插入方法，实现TD_M_AREA表的插入语句的封装
 *  @param insArea 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::AreaDAO::jinsert(Area &insArea)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VAREA_CODE", insArea.AreaCode);
    SetParam(":VAREA_NAME", insArea.AreaName);
    SetParam(":VAREA_FRAME", insArea.AreaFrame);
    SetParam(":VTHIS_TAG", insArea.ThisTag);
    SetParam(":VUSE_TAG", insArea.UseTag);
    SetParam(":VEXTEND_TAG", insArea.ExtendTag);
    SetParam(":VORDER_NO", insArea.OrderNo);
    SetParam(":VUSER_AREA_CODE", insArea.UserAreaCode);
    SetParam(":VPARENT_AREA_CODE", insArea.ParentAreaCode);
    SetParam(":VAREA_LEVEL", insArea.AreaLevel);
    SetParam(":VVALIDFLAG", insArea.Validflag);
    SetParam(":VREMARK", insArea.Remark);
    SetParam(":VRSVALUE1", insArea.Rsvalue1);
    SetParam(":VRSVALUE2", insArea.Rsvalue2);
    SetParam(":VUPDATE_STAFF_ID", insArea.UpdateStaffId);
    SetParam(":VUPDATE_DEPART_ID", insArea.UpdateDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::AreaDAO::jinsert(Area &insArea)
 *  插入方法，实现TD_M_AREA表的插入语句的封装
 *  @param insArea 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::AreaDAO::jinsby(const string &insBy, Area &insArea)
{

    ExecInsert(insBy);

    return 0;
}

