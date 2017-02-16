/**=!===============SEGMENG1==========================*/
#include "StaffUpdatemodLogDAO.h"

/**=~===============SEGMENG5==========================*/

/// 实体类
/**
 *  \fn void init()
 *  StaffUpdatemodLog 类的初始化函数
 */
void ParaDom::StaffUpdatemodLog::init()
{
	modCode.clear();
	modVersion.clear();
	modBuildno = -1;
	mustUpdateTag.clear();
	regUpdateTag.clear();
	remark.clear();
	updateTime.clear();
	updateStaffId.clear();
	updateDepartId.clear();
	modSize.clear();
	updateMethod.clear();
	delTag.clear();
}


/// DAO类
/** \fn int jselect(StaffUpdatemodLog &rStaffUpdatemodLog, const string &selBy)
 *  实现TD_M_STAFF_UPDATEMOD_LOG表的所有结果为单条记录的查询语句的封装，查询结果以实体对象方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::StaffUpdatemodLogDAO::jselect(StaffUpdatemodLog& rStaffUpdatemodLog, const string &selBy)
{
	try
	{
	    ExecSelect(selBy);
	
	    if (Fetch())
	    {
	        	rStaffUpdatemodLog.modCode = GetString("MOD_CODE");
	        	rStaffUpdatemodLog.modVersion = GetString("MOD_VERSION");
	        	rStaffUpdatemodLog.modBuildno = GetInt("MOD_BUILDNO");
	        	rStaffUpdatemodLog.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	rStaffUpdatemodLog.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	rStaffUpdatemodLog.remark = GetString("REMARK");
	        	rStaffUpdatemodLog.updateTime = GetString("UPDATE_TIME");
	        	rStaffUpdatemodLog.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rStaffUpdatemodLog.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	rStaffUpdatemodLog.modSize = GetString("MOD_SIZE");
	        	rStaffUpdatemodLog.updateMethod = GetString("UPDATE_METHOD");
	        	rStaffUpdatemodLog.delTag = GetString("DEL_TAG");

	
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

/** \fn int jselect(VStaffUpdatemodLog &vrStaffUpdatemodLog, const string &selBy)
 *  实现TD_M_STAFF_UPDATEMOD_LOG表的所有查询语句的封装，查询结果以实体对象vector方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::StaffUpdatemodLogDAO::jselect(VStaffUpdatemodLog &vrStaffUpdatemodLog, const string &selBy)
{
    int i=0;

	try
	{
	    ExecSelect(selBy);
	
	    while (Fetch())
	    {
	        StaffUpdatemodLog tmpStaffUpdatemodLog;
	
	        	tmpStaffUpdatemodLog.modCode = GetString("MOD_CODE");
	        	tmpStaffUpdatemodLog.modVersion = GetString("MOD_VERSION");
	        	tmpStaffUpdatemodLog.modBuildno = GetInt("MOD_BUILDNO");
	        	tmpStaffUpdatemodLog.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	tmpStaffUpdatemodLog.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	tmpStaffUpdatemodLog.remark = GetString("REMARK");
	        	tmpStaffUpdatemodLog.updateTime = GetString("UPDATE_TIME");
	        	tmpStaffUpdatemodLog.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	tmpStaffUpdatemodLog.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	tmpStaffUpdatemodLog.modSize = GetString("MOD_SIZE");
	        	tmpStaffUpdatemodLog.updateMethod = GetString("UPDATE_METHOD");
	        	tmpStaffUpdatemodLog.delTag = GetString("DEL_TAG");

	
			vrStaffUpdatemodLog.push_back(tmpStaffUpdatemodLog);
	
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

/** \fn int jinsert(VStaffUpdatemodLog &vrStaffUpdatemodLog, const string& insBy)
 *  实现TD_M_STAFF_UPDATEMOD_LOG表的批量数据插入方法
  *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::StaffUpdatemodLogDAO::jinsert(VStaffUpdatemodLog &vrStaffUpdatemodLog, const string& insBy)
{
	return -1;
}
