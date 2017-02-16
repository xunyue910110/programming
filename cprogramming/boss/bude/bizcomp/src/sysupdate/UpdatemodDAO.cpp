/**=!===============SEGMENG1==========================*/
#include "UpdatemodDAO.h"

/**=~===============SEGMENG5==========================*/

/// 实体类
/**
 *  \fn void init()
 *  Updatemod 类的初始化函数
 */
void ParaDom::Updatemod::init()
{
	modCode.clear();
	modVersion.clear();
	modBuildno = -1;
	modSize.clear();
	updateMethod.clear();
	mustUpdateTag.clear();
	regUpdateTag.clear();
	remark.clear();
	updateTime.clear();
	updateStaffId.clear();
	updateDepartId.clear();
	modPath.clear();
	delTag.clear();
	modName.clear();
	modType.clear();
}


/// DAO类
/** \fn int jselect(Updatemod &rUpdatemod, const string &selBy)
 *  实现TD_M_UPDATEMOD表的所有结果为单条记录的查询语句的封装，查询结果以实体对象方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::UpdatemodDAO::jselect(Updatemod& rUpdatemod, const string &selBy)
{
	try
	{
	    ExecSelect(selBy);
	
	    if (Fetch())
	    {
	        if( selBy =="SEL_BY_MOD_CODE") 
	        { 
	        	rUpdatemod.modCode = GetString("MOD_CODE");
	        	rUpdatemod.modVersion = GetString("MOD_VERSION");
	        	rUpdatemod.modName = GetString("MOD_NAME");
	        	rUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	        	rUpdatemod.modType = GetString("MOD_TYPE");
	        	rUpdatemod.modSize = GetString("MOD_SIZE");
	        	rUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	        	rUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	rUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	rUpdatemod.remark = GetString("REMARK");
	        	rUpdatemod.updateTime = GetString("UPDATE_TIME");
	        	rUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	rUpdatemod.modPath = GetString("MOD_PATH");
	        	rUpdatemod.delTag = GetString("DEL_TAG");
	        } 
	        else 
	        {
	        	rUpdatemod.modCode = GetString("MOD_CODE");
	        	rUpdatemod.modVersion = GetString("MOD_VERSION");
	        	rUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	        	rUpdatemod.modSize = GetString("MOD_SIZE");
	        	rUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	        	rUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	rUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	rUpdatemod.remark = GetString("REMARK");
	        	rUpdatemod.updateTime = GetString("UPDATE_TIME");
	        	rUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	rUpdatemod.modPath = GetString("MOD_PATH");
	        	rUpdatemod.delTag = GetString("DEL_TAG");
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

/** \fn int jselect(VUpdatemod &vrUpdatemod, const string &selBy)
 *  实现TD_M_UPDATEMOD表的所有查询语句的封装，查询结果以实体对象vector方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::UpdatemodDAO::jselect(VUpdatemod &vrUpdatemod, const string &selBy)
{
    int i=0;

	try
	{
	    ExecSelect(selBy);
	
	    while (Fetch())
	    {
	        Updatemod tmpUpdatemod;
	
	        if( selBy =="SEL_BY_MOD_CODE") 
	        { 
	        	tmpUpdatemod.modCode = GetString("MOD_CODE");
	        	tmpUpdatemod.modVersion = GetString("MOD_VERSION");
	        	tmpUpdatemod.modName = GetString("MOD_NAME");
	        	tmpUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	        	tmpUpdatemod.modType = GetString("MOD_TYPE");
	        	tmpUpdatemod.modSize = GetString("MOD_SIZE");
	        	tmpUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	        	tmpUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	tmpUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	tmpUpdatemod.remark = GetString("REMARK");
	        	tmpUpdatemod.updateTime = GetString("UPDATE_TIME");
	        	tmpUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	tmpUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	tmpUpdatemod.modPath = GetString("MOD_PATH");
	        	tmpUpdatemod.delTag = GetString("DEL_TAG");
	        } 
	        else 
	        {
	        	tmpUpdatemod.modCode = GetString("MOD_CODE");
	        	tmpUpdatemod.modVersion = GetString("MOD_VERSION");
	        	tmpUpdatemod.modBuildno = GetInt("MOD_BUILDNO");
	        	tmpUpdatemod.modSize = GetString("MOD_SIZE");
	        	tmpUpdatemod.updateMethod = GetString("UPDATE_METHOD");
	        	tmpUpdatemod.mustUpdateTag = GetString("MUST_UPDATE_TAG");
	        	tmpUpdatemod.regUpdateTag = GetString("REG_UPDATE_TAG");
	        	tmpUpdatemod.remark = GetString("REMARK");
	        	tmpUpdatemod.updateTime = GetString("UPDATE_TIME");
	        	tmpUpdatemod.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	tmpUpdatemod.updateDepartId = GetString("UPDATE_DEPART_ID");
	        	tmpUpdatemod.modPath = GetString("MOD_PATH");
	        	tmpUpdatemod.delTag = GetString("DEL_TAG");
	        }
	
			vrUpdatemod.push_back(tmpUpdatemod);
	
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

/** \fn int jinsert(VUpdatemod &vrUpdatemod, const string& insBy)
 *  实现TD_M_UPDATEMOD表的批量数据插入方法
  *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::UpdatemodDAO::jinsert(VUpdatemod &vrUpdatemod, const string& insBy)
{
	return -1;
}
