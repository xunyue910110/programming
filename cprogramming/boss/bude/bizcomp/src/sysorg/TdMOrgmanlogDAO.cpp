/**=!===============SEGMENG1==========================*/
#include "TdMOrgmanlogDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class TdMOrgmanlog 
 *  @brief TD_M_ORGMANLOG表的映射实体
 *  本对象映射数据库TD_M_ORGMANLOG表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
SysMan::TdMOrgmanlog::TdMOrgmanlog()
{
}

SysMan::TdMOrgmanlog::~TdMOrgmanlog()
{
}

void SysMan::TdMOrgmanlog::init()
{
	LogId.clear();	
	OperAreaCode.clear();	
	SubsysCode.clear();	
	OpertypeCode.clear();	
	StartValue1.clear();	
	EndValue1.clear();	
	StartValue2.clear();	
	EndValue2.clear();	
	StartValue3.clear();	
	EndValue3.clear();	
	OperInfo.clear();	
	Remark.clear();	
	Rsvalue1.clear();	
	Rsvalue2.clear();	
	OperDate.clear();	
	OperStaffId.clear();	
	OperDepartId.clear();	
}

/** @class TdMOrgmanlogDAO
 *  @brief TD_M_ORGMANLOG表的数据访问接口
 *  本对象对数据库TD_M_ORGMANLOG表的数据库访问进行封装，提供所有TD_M_ORGMANLOG表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
SysMan::TdMOrgmanlogDAO::TdMOrgmanlogDAO(): MTable("TD_M_ORGMANLOG")
{
	//
}

SysMan::TdMOrgmanlogDAO::~TdMOrgmanlogDAO ()
{
}

/** \fn SysMan::VPTdMOrgmanlog SysMan::TdMOrgmanlogDAO::jselect(const string &selBy, TdMOrgmanlog &selTdMOrgmanlog)
 *  查询方法，实现TD_M_ORGMANLOG表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selTdMOrgmanlog  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPTdMOrgmanlog SysMan::TdMOrgmanlogDAO::jselect(const string &selBy, TdMOrgmanlog &selTdMOrgmanlog)
{
    SysMan::VPTdMOrgmanlog  vpRetTdMOrgmanlog; 

    if( selBy == SEL_IDX_TD_M_ORGMANLOG_1) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_AREA_CODE", selTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", selTdMOrgmanlog.SubsysCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_ORGMANLOG_2) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_DATE", selTdMOrgmanlog.OperDate);
    	SetParam(":VOPER_STAFF_ID", selTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPERTYPE_CODE", selTdMOrgmanlog.OpertypeCode);
    } 	
    else if( selBy == SEL_IDX_TD_M_ORGMANLOG_3) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_DATE", selTdMOrgmanlog.OperDate);
    	SetParam(":VSTART_VALUE1", selTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", selTdMOrgmanlog.EndValue1);
    } 	
    else if( selBy == SEL_PK_TD_M_ORGMANLOG) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VLOG_ID", selTdMOrgmanlog.LogId);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        TdMOrgmanlog *pTmpTdMOrgmanlog = new TdMOrgmanlog();
        TdMOrgmanlog pTmpTdMOrgmanlog;
        
        pTmpTdMOrgmanlog.LogId = GetString("LOG_ID");
        pTmpTdMOrgmanlog.OperAreaCode = GetString("OPER_AREA_CODE");
        pTmpTdMOrgmanlog.SubsysCode = GetString("SUBSYS_CODE");
        pTmpTdMOrgmanlog.OpertypeCode = GetString("OPERTYPE_CODE");
        pTmpTdMOrgmanlog.StartValue1 = GetString("START_VALUE1");
        pTmpTdMOrgmanlog.EndValue1 = GetString("END_VALUE1");
        pTmpTdMOrgmanlog.StartValue2 = GetString("START_VALUE2");
        pTmpTdMOrgmanlog.EndValue2 = GetString("END_VALUE2");
        pTmpTdMOrgmanlog.StartValue3 = GetString("START_VALUE3");
        pTmpTdMOrgmanlog.EndValue3 = GetString("END_VALUE3");
        pTmpTdMOrgmanlog.OperInfo = GetString("OPER_INFO");
        pTmpTdMOrgmanlog.Remark = GetString("REMARK");
        pTmpTdMOrgmanlog.Rsvalue1 = GetString("RSVALUE1");
        pTmpTdMOrgmanlog.Rsvalue2 = GetString("RSVALUE2");
        pTmpTdMOrgmanlog.OperDate = GetString("OPER_DATE");
        pTmpTdMOrgmanlog.OperStaffId = GetString("OPER_STAFF_ID");
        pTmpTdMOrgmanlog.OperDepartId = GetString("OPER_DEPART_ID");
		
		vpRetTdMOrgmanlog.push_back(pTmpTdMOrgmanlog);
    }
    
    return vpRetTdMOrgmanlog;
}
 
/** \fn SysMan::VPTdMOrgmanlog SysMan::TdMOrgmanlogDAO::jselect(const string &selBy, TdMOrgmanlog &selTdMOrgmanlog)
 *  查询方法，实现TD_M_ORGMANLOG表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selTdMOrgmanlog  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
SysMan::VPTdMOrgmanlog SysMan::TdMOrgmanlogDAO::jselby(const string &selBy, TdMOrgmanlog &selTdMOrgmanlog)
{
    SysMan::VPTdMOrgmanlog  vpRetTdMOrgmanlog; 

    
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        TdMOrgmanlog *pTmpTdMOrgmanlog = new TdMOrgmanlog();
        TdMOrgmanlog pTmpTdMOrgmanlog;
        
		
		vpRetTdMOrgmanlog.push_back(pTmpTdMOrgmanlog);
    }
    
    return vpRetTdMOrgmanlog;
}

/** \fn int SysMan::TdMOrgmanlogDAO::jupdate(const string &updBy, TdMOrgmanlog &updTdMOrgmanlog) 
 *  更新方法，实现TD_M_ORGMANLOG表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updTdMOrgmanlog  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int SysMan::TdMOrgmanlogDAO::jupdate(const string &updBy, TdMOrgmanlog &updTdMOrgmanlog)
{
    if( updBy == UPD_IDX_TD_M_ORGMANLOG_1) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VLOG_ID", updTdMOrgmanlog.LogId);
    	SetParam(":VOPER_AREA_CODE", updTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", updTdMOrgmanlog.SubsysCode);
    	SetParam(":VOPERTYPE_CODE", updTdMOrgmanlog.OpertypeCode);
    	SetParam(":VSTART_VALUE1", updTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", updTdMOrgmanlog.EndValue1);
    	SetParam(":VSTART_VALUE2", updTdMOrgmanlog.StartValue2);
    	SetParam(":VEND_VALUE2", updTdMOrgmanlog.EndValue2);
    	SetParam(":VSTART_VALUE3", updTdMOrgmanlog.StartValue3);
    	SetParam(":VEND_VALUE3", updTdMOrgmanlog.EndValue3);
    	SetParam(":VOPER_INFO", updTdMOrgmanlog.OperInfo);
    	SetParam(":VREMARK", updTdMOrgmanlog.Remark);
    	SetParam(":VRSVALUE1", updTdMOrgmanlog.Rsvalue1);
    	SetParam(":VRSVALUE2", updTdMOrgmanlog.Rsvalue2);
    	SetParam(":VOPER_STAFF_ID", updTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPER_DEPART_ID", updTdMOrgmanlog.OperDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_AREA_CODE", updTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", updTdMOrgmanlog.SubsysCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_ORGMANLOG_2) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VLOG_ID", updTdMOrgmanlog.LogId);
    	SetParam(":VOPER_AREA_CODE", updTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", updTdMOrgmanlog.SubsysCode);
    	SetParam(":VOPERTYPE_CODE", updTdMOrgmanlog.OpertypeCode);
    	SetParam(":VSTART_VALUE1", updTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", updTdMOrgmanlog.EndValue1);
    	SetParam(":VSTART_VALUE2", updTdMOrgmanlog.StartValue2);
    	SetParam(":VEND_VALUE2", updTdMOrgmanlog.EndValue2);
    	SetParam(":VSTART_VALUE3", updTdMOrgmanlog.StartValue3);
    	SetParam(":VEND_VALUE3", updTdMOrgmanlog.EndValue3);
    	SetParam(":VOPER_INFO", updTdMOrgmanlog.OperInfo);
    	SetParam(":VREMARK", updTdMOrgmanlog.Remark);
    	SetParam(":VRSVALUE1", updTdMOrgmanlog.Rsvalue1);
    	SetParam(":VRSVALUE2", updTdMOrgmanlog.Rsvalue2);
    	SetParam(":VOPER_STAFF_ID", updTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPER_DEPART_ID", updTdMOrgmanlog.OperDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_DATE", updTdMOrgmanlog.OperDate);
    	SetParam(":VOPER_STAFF_ID", updTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPERTYPE_CODE", updTdMOrgmanlog.OpertypeCode);
    } 	
    else if( updBy == UPD_IDX_TD_M_ORGMANLOG_3) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VLOG_ID", updTdMOrgmanlog.LogId);
    	SetParam(":VOPER_AREA_CODE", updTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", updTdMOrgmanlog.SubsysCode);
    	SetParam(":VOPERTYPE_CODE", updTdMOrgmanlog.OpertypeCode);
    	SetParam(":VSTART_VALUE1", updTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", updTdMOrgmanlog.EndValue1);
    	SetParam(":VSTART_VALUE2", updTdMOrgmanlog.StartValue2);
    	SetParam(":VEND_VALUE2", updTdMOrgmanlog.EndValue2);
    	SetParam(":VSTART_VALUE3", updTdMOrgmanlog.StartValue3);
    	SetParam(":VEND_VALUE3", updTdMOrgmanlog.EndValue3);
    	SetParam(":VOPER_INFO", updTdMOrgmanlog.OperInfo);
    	SetParam(":VREMARK", updTdMOrgmanlog.Remark);
    	SetParam(":VRSVALUE1", updTdMOrgmanlog.Rsvalue1);
    	SetParam(":VRSVALUE2", updTdMOrgmanlog.Rsvalue2);
    	SetParam(":VOPER_STAFF_ID", updTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPER_DEPART_ID", updTdMOrgmanlog.OperDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VOPER_DATE", updTdMOrgmanlog.OperDate);
    	SetParam(":VSTART_VALUE1", updTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", updTdMOrgmanlog.EndValue1);
    } 	
    else if( updBy == UPD_PK_TD_M_ORGMANLOG) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VLOG_ID", updTdMOrgmanlog.LogId);
    	SetParam(":VOPER_AREA_CODE", updTdMOrgmanlog.OperAreaCode);
    	SetParam(":VSUBSYS_CODE", updTdMOrgmanlog.SubsysCode);
    	SetParam(":VOPERTYPE_CODE", updTdMOrgmanlog.OpertypeCode);
    	SetParam(":VSTART_VALUE1", updTdMOrgmanlog.StartValue1);
    	SetParam(":VEND_VALUE1", updTdMOrgmanlog.EndValue1);
    	SetParam(":VSTART_VALUE2", updTdMOrgmanlog.StartValue2);
    	SetParam(":VEND_VALUE2", updTdMOrgmanlog.EndValue2);
    	SetParam(":VSTART_VALUE3", updTdMOrgmanlog.StartValue3);
    	SetParam(":VEND_VALUE3", updTdMOrgmanlog.EndValue3);
    	SetParam(":VOPER_INFO", updTdMOrgmanlog.OperInfo);
    	SetParam(":VREMARK", updTdMOrgmanlog.Remark);
    	SetParam(":VRSVALUE1", updTdMOrgmanlog.Rsvalue1);
    	SetParam(":VRSVALUE2", updTdMOrgmanlog.Rsvalue2);
    	SetParam(":VOPER_STAFF_ID", updTdMOrgmanlog.OperStaffId);
    	SetParam(":VOPER_DEPART_ID", updTdMOrgmanlog.OperDepartId);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VLOG_ID", updTdMOrgmanlog.LogId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int SysMan::TdMOrgmanlogDAO::jdelete(const string &delBy, TdMOrgmanlog &delTdMOrgmanlog)
 *  删除方法，实现TD_M_ORGMANLOG表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delTdMOrgmanlog  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::TdMOrgmanlogDAO::jdelete(const string &delBy, TdMOrgmanlog &delTdMOrgmanlog)
{
	if( delBy == DEL_IDX_TD_M_ORGMANLOG_1) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VOPER_AREA_CODE", delTdMOrgmanlog.OperAreaCode);
		SetParam(":VSUBSYS_CODE", delTdMOrgmanlog.SubsysCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_ORGMANLOG_2) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VOPER_DATE", delTdMOrgmanlog.OperDate);
		SetParam(":VOPER_STAFF_ID", delTdMOrgmanlog.OperStaffId);
		SetParam(":VOPERTYPE_CODE", delTdMOrgmanlog.OpertypeCode);
	} 	
	else if( delBy == DEL_IDX_TD_M_ORGMANLOG_3) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VOPER_DATE", delTdMOrgmanlog.OperDate);
		SetParam(":VSTART_VALUE1", delTdMOrgmanlog.StartValue1);
		SetParam(":VEND_VALUE1", delTdMOrgmanlog.EndValue1);
	} 	
	else if( delBy == DEL_PK_TD_M_ORGMANLOG) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VLOG_ID", delTdMOrgmanlog.LogId);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int SysMan::TdMOrgmanlogDAO::jinsert(TdMOrgmanlog &insTdMOrgmanlog)
 *  插入方法，实现TD_M_ORGMANLOG表的插入语句的封装
 *  @param insTdMOrgmanlog 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::TdMOrgmanlogDAO::jinsert(TdMOrgmanlog &insTdMOrgmanlog)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VLOG_ID", insTdMOrgmanlog.LogId);
    SetParam(":VOPER_AREA_CODE", insTdMOrgmanlog.OperAreaCode);
    SetParam(":VSUBSYS_CODE", insTdMOrgmanlog.SubsysCode);
    SetParam(":VOPERTYPE_CODE", insTdMOrgmanlog.OpertypeCode);
    SetParam(":VSTART_VALUE1", insTdMOrgmanlog.StartValue1);
    SetParam(":VEND_VALUE1", insTdMOrgmanlog.EndValue1);
    SetParam(":VSTART_VALUE2", insTdMOrgmanlog.StartValue2);
    SetParam(":VEND_VALUE2", insTdMOrgmanlog.EndValue2);
    SetParam(":VSTART_VALUE3", insTdMOrgmanlog.StartValue3);
    SetParam(":VEND_VALUE3", insTdMOrgmanlog.EndValue3);
    SetParam(":VOPER_INFO", insTdMOrgmanlog.OperInfo);
    SetParam(":VREMARK", insTdMOrgmanlog.Remark);
    SetParam(":VRSVALUE1", insTdMOrgmanlog.Rsvalue1);
    SetParam(":VRSVALUE2", insTdMOrgmanlog.Rsvalue2);
    SetParam(":VOPER_STAFF_ID", insTdMOrgmanlog.OperStaffId);
    SetParam(":VOPER_DEPART_ID", insTdMOrgmanlog.OperDepartId);
    
    ExecInsert();

    return 0;
}

/** \fn int SysMan::TdMOrgmanlogDAO::jinsert(TdMOrgmanlog &insTdMOrgmanlog)
 *  插入方法，实现TD_M_ORGMANLOG表的插入语句的封装
 *  @param insTdMOrgmanlog 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int SysMan::TdMOrgmanlogDAO::jinsby(const string &insBy, TdMOrgmanlog &insTdMOrgmanlog)
{
    
    
    ExecInsert(insBy);

    return 0;
}

