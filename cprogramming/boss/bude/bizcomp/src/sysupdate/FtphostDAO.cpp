/**=!===============SEGMENG1==========================*/
#include "FtphostDAO.h"

/**=~===============SEGMENG5==========================*/

/// 实体类
/**
 *  \fn void init()
 *  Ftphost 类的初始化函数
 */
void ParaDom::Ftphost::init()
{
	eparchyCode.clear();
	ftpHost.clear();
	ftpUsrid.clear();
	ftpPasswd.clear();
	ftpPath.clear();
	ftpMod.clear();
	remark.clear();
	updateTime.clear();
	updateStaffId.clear();
	updateDepartId.clear();
}


/// DAO类
/** \fn int jselect(Ftphost &rFtphost, const string &selBy)
 *  实现TD_S_FTPHOST表的所有结果为单条记录的查询语句的封装，查询结果以实体对象方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::FtphostDAO::jselect(Ftphost& rFtphost, const string &selBy)
{
	try
	{
	    ExecSelect(selBy);
	
	    if (Fetch())
	    {
	        	rFtphost.eparchyCode = GetString("EPARCHY_CODE");
	        	rFtphost.ftpHost = GetString("FTP_HOST");
	        	rFtphost.ftpUsrid = GetString("FTP_USRID");
	        	rFtphost.ftpPasswd = GetString("FTP_PASSWD");
	        	rFtphost.ftpPath = GetString("FTP_PATH");
	        	rFtphost.ftpMod = GetString("FTP_MOD");
	        	rFtphost.remark = GetString("REMARK");
	        	rFtphost.updateTime = GetString("UPDATE_TIME");
	        	rFtphost.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	rFtphost.updateDepartId = GetString("UPDATE_DEPART_ID");

	
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

/** \fn int jselect(VFtphost &vrFtphost, const string &selBy)
 *  实现TD_S_FTPHOST表的所有查询语句的封装，查询结果以实体对象vector方式返回
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::FtphostDAO::jselect(VFtphost &vrFtphost, const string &selBy)
{
    int i=0;

	try
	{
	    ExecSelect(selBy);
	
	    while (Fetch())
	    {
	        Ftphost tmpFtphost;
	
	        	tmpFtphost.eparchyCode = GetString("EPARCHY_CODE");
	        	tmpFtphost.ftpHost = GetString("FTP_HOST");
	        	tmpFtphost.ftpUsrid = GetString("FTP_USRID");
	        	tmpFtphost.ftpPasswd = GetString("FTP_PASSWD");
	        	tmpFtphost.ftpPath = GetString("FTP_PATH");
	        	tmpFtphost.ftpMod = GetString("FTP_MOD");
	        	tmpFtphost.remark = GetString("REMARK");
	        	tmpFtphost.updateTime = GetString("UPDATE_TIME");
	        	tmpFtphost.updateStaffId = GetString("UPDATE_STAFF_ID");
	        	tmpFtphost.updateDepartId = GetString("UPDATE_DEPART_ID");

	
			vrFtphost.push_back(tmpFtphost);
	
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

/** \fn int jinsert(VFtphost &vrFtphost, const string& insBy)
 *  实现TD_S_FTPHOST表的批量数据插入方法
  *  @return 成功返回记录数，失败则抛出例外
 */
int ParaDom::FtphostDAO::jinsert(VFtphost &vrFtphost, const string& insBy)
{
	return -1;
}
