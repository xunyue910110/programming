/**=!===============SEGMENG1==========================*/
#include "BackupdatetaskDAO.h"

/**=~===============SEGMENG5==========================*/

/// 实体类
/**
 *  \fn void init()
 *  Backupdatetask 类的初始化函数
 */
void ParaDom::Backupdatetask::init()
{
	updateId.clear();
	updateDllName.clear();
	updateInterval = -1;
	updatePri = -1;
	updateTag.clear();
	remark.clear();
	updateTime.clear();
	updateStaffId.clear();
	updateDepartId.clear();
	eparchyCode.clear();
}


/// DAO类
/** \fn int jselect(Backupdatetask &rBackupdatetask, const string &selBy)
 *  实现TD_M_BACKUPDATETASK表的所有结果为单条记录的查询语句的封装，查询结果以实体对象方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::BackupdatetaskDAO::jselect(Backupdatetask& rBackupdatetask, const string &selBy)
{
	try
	{
	    ExecSelect(selBy);
	
	    if (Fetch())
	    {
	        if( selBy == "SEL_ALL_BY_UPDATETAG") 
	        { 
	        	rBackupdatetask.updateId = GetString("UPDATE_ID");
	        	rBackupdatetask.updateDllName = GetString("UPDATE_DLL_NAME");
	        	rBackupdatetask.updateInterval = GetInt("UPDATE_INTERVAL");
	        	rBackupdatetask.updatePri = GetInt("UPDATE_PRI");
	        	rBackupdatetask.updateTag = GetString("UPDATE_TAG");
	        	rBackupdatetask.remark = GetString("REMARK");
	        	rBackupdatetask.updateTime = GetString("UPDATE_TIME");
	        	rBackupdatetask.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rBackupdatetask.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	rBackupdatetask.eparchyCode = GetString("EPARCHY_CODE");
	        } 	
	        else 
	        {
	        	rBackupdatetask.updateId = GetString("UPDATE_ID");
	        	rBackupdatetask.updateDllName = GetString("UPDATE_DLL_NAME");
	        	rBackupdatetask.updateInterval = GetInt("UPDATE_INTERVAL");
	        	rBackupdatetask.updatePri = GetInt("UPDATE_PRI");
	        	rBackupdatetask.updateTag = GetString("UPDATE_TAG");
	        	rBackupdatetask.remark = GetString("REMARK");
	        	rBackupdatetask.updateTime = GetString("UPDATE_TIME");
	        	rBackupdatetask.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rBackupdatetask.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	rBackupdatetask.eparchyCode = GetString("EPARCHY_CODE");
	        }
	
			return 1;
	    }
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName: "); 
		expmsg += ClassName(); 
		expmsg += ", Oper: Select, selBy: "+selBy+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return 0;
}

/** \fn int jselect(VBackupdatetask &vrBackupdatetask, const string &selBy)
 *  实现TD_M_BACKUPDATETASK表的所有查询语句的封装，查询结果以实体对象vector方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::BackupdatetaskDAO::jselect(VBackupdatetask &vrBackupdatetask, const string &selBy)
{
    int i=0;

	try
	{
	    ExecSelect(selBy);
	
	    while (Fetch())
	    {
	        Backupdatetask tmpBackupdatetask;
	
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
	
			vrBackupdatetask.push_back(tmpBackupdatetask);
	
			i++;
	    }
	} catch (EUdbiException& er) 
	{ 
		string expmsg("\nDaoName: "); 
		expmsg += ClassName(); 
		expmsg += ", Oper: Select, selBy: "+selBy+'\n'; 
		expmsg += parToString(); 
		THROW_A(DAOException, er, expmsg.c_str()); 
	}

    return i;
}

/** \fn int jinsert(VBackupdatetask &vrBackupdatetask, const string& insBy)
 *  实现TD_M_BACKUPDATETASK表的批量数据插入方法
  *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::BackupdatetaskDAO::jinsert(VBackupdatetask &vrBackupdatetask, const string& insBy)
{
	return -1;
}
