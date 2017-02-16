#include "MmInterface.h"
#include "mdb/Mdb_Exception.h"
#include "mdb/InstanceFactory.h"
#include "mdb/InputParams.h"
#include "mdb/DataBase.h"
#include "mdb/Session.h"
#include "mdb/TableDefParam.h"

using namespace LINKAGE_MDB;

MmInterface::MmInterface()
{
	m_db = NULL;
	m_session = NULL;
	
	m_columnnum = 0;
	m_transflag = 0;
}


MmInterface::~MmInterface()
{
	//有事务存在
	if (m_transflag == 1)
	{
		rollbackTrans();
	}
	
	if (m_db != NULL)
	{
		if (m_session != NULL)
		{
			m_db->disconnect(m_session);
			m_session = NULL;
		}
		
		delete m_db;
		m_db = NULL;
	}
	
	m_columnnum = 0;
	m_transflag = 0;
}


//modify by chenxt 2008-12-09
//void MmInterface::connect(const T_DATABASETYPE &dbtype, const string &dbname, const string &hostname, const int &port)
void MmInterface::connect(const T_DATABASETYPE &dbtype, const string &dbname, const string &hostname, const int &port, const string &username, const string &password)
{
	//断开数据库连接
	disconnect();
	
	//创建新的连接
	try
	{
		m_db=InstanceFactory::getDBInstance(dbtype, dbname.c_str(), hostname.c_str(), port, username.c_str(), password.c_str());
		
		m_session = m_db->connect();
	}
	catch(Mdb_Exception &e)
	{
		if (m_db != NULL)
		{
  		if (m_session != NULL)
  		{
  			m_db->disconnect(m_session);
  			m_session = NULL;
  		}		  
			delete m_db;
			m_db = NULL;
		}
		//modify by wangfc 2010-05-11
	//	throw CException(__FILE__, __LINE__, "连接内存库失败,信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "连接内存库(%s)失败,信息%s", m_alisname.c_str(),e.GetString());
	}
}


void MmInterface::disconnect()
{
	//有事务存在
	if (m_transflag == 1)
	{
		rollbackTrans();
	}
	
	if (m_db != NULL)
	{
		if (m_session != NULL)
		{
			m_db->disconnect(m_session);
			m_session = NULL;
		}
		
		delete m_db;
		m_db = NULL;
	}
	
	m_columnnum = 0;
	m_transflag = 0;
}


void MmInterface::beginTrans(const string &tablename, const int &locktype)
{
	//如果事务未开启,开启事务
	if (m_transflag == 0)
	{
		try
		{
			m_session->begin(tablename.c_str(), locktype);
		}
		catch(Mdb_Exception &e)
		{
		 // modify by wangfc 2010-05-11
		//	throw CException(__FILE__, __LINE__, "内存库开事务失败, 信息%s", e.GetString());
			throw CException(__FILE__, __LINE__, "内存库(%s)开事务失败, 信息%s",m_alisname.c_str(), e.GetString());
		}
		
		m_transflag = 1;
	}
}


void MmInterface::commitTrans()
{
	//有事务存在
	if (m_transflag == 1)
	{
		try
		{
			m_session->commit();
		}
		catch(Mdb_Exception &e)
		{
			//m_transflag = 0;
			rollbackTrans();
			//modify by  wangfc 2010-05-11
			//throw CException(__FILE__, __LINE__, "内存库提交事务失败, 信息%s", e.GetString());
			throw CException(__FILE__, __LINE__, "内存库(%s)提交事务失败, 信息%s",m_alisname.c_str(), e.GetString());
		}
		
		m_transflag = 0;
	}
}


void MmInterface::rollbackTrans()
{
	//有事务存在
	if (m_transflag == 1)
	{
		try
		{
			m_session->rollBack();
		}
		catch(Mdb_Exception &e)
		{
			m_transflag = 0;
			//modify by wangfc 2010-05-11 m_alisname
			//throw CException(__FILE__, __LINE__, "内存库回滚事务失败, 信息%s", e.GetString());
			throw CException(__FILE__, __LINE__, "内存库(%s)回滚事务失败, 信息%s",m_alisname.c_str(), e.GetString());
		}
		
		m_transflag = 0;
	}
}


int MmInterface::select(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params)
{	
	long lv = 0;
	
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库查询失败, 会话信息不存在!");
	}
	
	//执行查询
	//查询参数大小预备
	int iIndexParamNum = 0;
	int iOthreParamNum = 0;
	//查询参数准备
	for (int i=0; i<params.size(); ++i)
	{
		if (params[i].m_indextag == 1)
		{
			++iIndexParamNum;
		}
		else
		{
			++iOthreParamNum;
		}
	}		
	InputParams *indexparams = new InputParams(iIndexParamNum);
	InputParams *otherparams = new InputParams(iOthreParamNum);
	try
	{
		//获取查询的列数
		const TableDef* tabledef = m_session->getTableDef(tablename.c_str());
		m_columnnum = tabledef->m_columnNum;

		//查询参数准备
		for (int i=0; i<params.size(); ++i)
		{
			if (params[i].m_indextag == 1)
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						indexparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						indexparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						indexparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						indexparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						indexparams->setValue(params[i].m_type, params[i].m_strValue);	
						break;
					default:
						//引起异常
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);
				}
			}
			else
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						otherparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						otherparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						otherparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						otherparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						otherparams->setValue(params[i].m_type, params[i].m_strValue);
						break;
					default:
						//引起异常
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);
				}
			}
		}

		int ires = m_session->select(tablename.c_str(), indexname.c_str(), indexparams, expr.c_str(), otherparams);
		
		
		//数据清理
		delete indexparams;
		delete otherparams;
		
		return ires;
	}
	catch(Mdb_Exception &e)
	{
		//数据清理
		delete indexparams;
		delete otherparams;
		//modify by wangfc 2010-05-11 
		//throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
}


int MmInterface::select(const string &tablename, const string &indexname, const string &expr, vector<string> &params)
{
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库查询失败, 会话信息不存在!");
	}
	
	//获取查询的表列数和索引信息
	const TableDef* tabledef = NULL;
	const IndexDef* indexdef = NULL;
	try
	{
		tabledef = m_session->getTableDef(tablename.c_str());
		indexdef = m_session->getIndexDef(tablename.c_str(), indexname.c_str());
	}
	catch(Mdb_Exception &e)
	{
	  //modify by wangfc 2010-05-11 
		//throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
	

	//获取表达式里面的列名称
	vector<string> columnNames;
	getColumnName(expr.c_str(), columnNames);
	//执行查询
	vector<MmParam> mmparams;
	mmparams.clear();
	//查询参数准备
	for (int i=0; i<params.size(); ++i)
	{
		MmParam mmparam;
		if (i < indexdef->m_columnNum)
		{
			mmparam.m_indextag = 1;
			mmparam.m_type = getColumnType(tabledef, indexdef->m_columnList[i]);
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, params[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(params[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(params[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, params[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");				
			}
		}
		else
		{
			mmparam.m_indextag = 0;
			mmparam.m_type = getColumnType(tabledef, columnNames[i-indexdef->m_columnNum].c_str());
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, params[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(params[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(params[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, params[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");		
			}
		}
		
		mmparams.push_back(mmparam);
	}
	
	//执行查询
	return select(tablename, indexname, expr, mmparams);
}


int MmInterface::deleteRec(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params)
{
	long lv = 0;
	
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库删除失败, 会话信息不存在!");
	}
	
	//删除参数准备
	//删除参数大小预备
	int iIndexParamNum = 0;
	int iOthreParamNum = 0;
	//删除参数准备
	for (int i=0; i<params.size(); ++i)
	{
		if (params[i].m_indextag == 1)
		{
			++iIndexParamNum;
		}
		else
		{
			++iOthreParamNum;
		}
	}		
	InputParams *indexparams = new InputParams(iIndexParamNum);
	InputParams *otherparams = new InputParams(iOthreParamNum);
	//执行删除
	try
	{
		//删除参数准备
		for (int i=0; i<params.size(); ++i)
		{
			if (params[i].m_indextag == 1)
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						indexparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						indexparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						indexparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						indexparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						indexparams->setValue(params[i].m_type, params[i].m_strValue);	
						break;
					default:
						//引起异常
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);
				}
			}
			else
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						otherparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						otherparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						otherparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						otherparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						otherparams->setValue(params[i].m_type, params[i].m_strValue);	
						break;
					default:
						//引起异常
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);	
				}
			}	
		}
	
		int ires = m_session->deleteRec(tablename.c_str(), indexname.c_str(), indexparams, expr.c_str(), otherparams);
		
		//数据清理
		delete indexparams;
		delete otherparams;
		
		return ires;
	}
	catch(Mdb_Exception &e)
	{
		//数据清理
		delete indexparams;
		delete otherparams;
		//modify by wangfc 2010-05-11 
		// throw CException(__FILE__, __LINE__, "内存库删除失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)删除失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
}


int MmInterface::deleteRec(const string &tablename, const string &indexname, const string &expr, vector<string> &params)
{
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库删除失败, 会话信息不存在!");
	}
	
	//获取查询的表列数和索引信息
	const TableDef* tabledef = NULL;
	const IndexDef* indexdef = NULL;
	try
	{
		tabledef = m_session->getTableDef(tablename.c_str());
		indexdef = m_session->getIndexDef(tablename.c_str(), indexname.c_str());
	}
	catch(Mdb_Exception &e)
	{
	  //modify by wangfc 2010-05-11 
	 //	throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
	 throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s", m_alisname.c_str(),e.GetString());
	}
	

	//获取表达式里面的列名称
	vector<string> columnNames;
	getColumnName(expr.c_str(), columnNames);
	//执行查询
	vector<MmParam> mmparams;
	mmparams.clear();
	//查询参数准备
	for (int i=0; i<params.size(); ++i)
	{
		MmParam mmparam;
		if (i < indexdef->m_columnNum)
		{
			mmparam.m_indextag = 1;
			mmparam.m_type = getColumnType(tabledef, indexdef->m_columnList[i]);
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, params[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(params[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(params[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, params[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");				
			}
		}
		else
		{
			mmparam.m_indextag = 0;
			mmparam.m_type = getColumnType(tabledef, columnNames[i-indexdef->m_columnNum].c_str());
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(params[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, params[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(params[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(params[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, params[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");		
			}
		}
		
		mmparams.push_back(mmparam);
	}
	
	return deleteRec(tablename, indexname, expr, mmparams);
}


int MmInterface::insert(const string &tablename, vector<MmParam> &params)
{
	long lv = 0;
	
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库插入失败, 会话信息不存在!");
	}
	
	//输入参数准备
	InputParams *inputparams = new InputParams(params.size());
	//执行插入
	try
	{
		for (int i=0; i<params.size(); ++i)
		{
			switch(params[i].m_type)
			{
				case VAR_TYPE_INT2:
					inputparams->setValue(params[i].m_type, &params[i].m_shortValue);
					break;
				case VAR_TYPE_INT:
					inputparams->setValue(params[i].m_type, &params[i].m_intValue);
					break;
				case VAR_TYPE_DATE:
					//日期型从字符型转换成日期型
					lv = atol(params[i].m_dateValue);
					inputparams->setValue(params[i].m_type, &lv);
					break;
				case VAR_TYPE_LONG:
					inputparams->setValue(params[i].m_type, &params[i].m_longValue);
					break;
				case VAR_TYPE_REAL:
					inputparams->setValue(params[i].m_type, &params[i].m_floatValue);
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					inputparams->setValue(params[i].m_type, params[i].m_strValue);	
					break;
				default:
					//引起异常
					inputparams->setValue(params[i].m_type, &params[i].m_intValue);
			}
		}
	
		int ires = m_session->insert(tablename.c_str(), inputparams);
		//清理数据
		delete inputparams;
		return ires;
	}
	catch(Mdb_Exception &e)
	{
		//清理数据
		delete inputparams;
		//modify by wangfc 2010-05-11
		//throw CException(__FILE__, __LINE__, "内存库删除失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)删除失败, 信息%s", m_alisname.c_str(),e.GetString());
	}
}


int MmInterface::insert(const string &tablename, vector<string> &params)
{
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库插入失败, 会话信息不存在!");
	}
	
	const TableDef* tabledef = NULL;
	try
	{
		//获取插入的列数
		tabledef = m_session->getTableDef(tablename.c_str());
	}
	catch(Mdb_Exception &e)
	{
	  //modify by wangfc 2010-05-11
		//throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
	
	//输入参数准备
	vector<MmParam> mmparams;
	mmparams.clear();
	for (int i=0; i<params.size(); ++i)
	{
		MmParam mmparam;
		mmparam.m_indextag = 0;
		mmparam.m_type = tabledef->m_columnList[i].m_type;
//cout<<"MmInterface::insert----------mmparam.m_type = "<<mmparam.m_type<<endl;
		switch(mmparam.m_type)
		{
			case VAR_TYPE_INT2:
				mmparam.m_shortValue = atoi(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_shortValue = "<<mmparam.m_shortValue<<endl;
				break;
			case VAR_TYPE_INT:
				mmparam.m_intValue = atoi(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_intValue = "<<mmparam.m_intValue<<endl;
				break;
			case VAR_TYPE_DATE:
				strcpy(mmparam.m_dateValue, params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_dateValue = "<<mmparam.m_dateValue<<endl;
				break;
			case VAR_TYPE_LONG:
				mmparam.m_longValue = atol(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_longValue = "<<mmparam.m_longValue<<endl;
				break;
			case VAR_TYPE_REAL:
				mmparam.m_floatValue = atof(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_floatValue = "<<mmparam.m_floatValue<<endl;
				break;
			case VAR_TYPE_NUMSTR:
			case VAR_TYPE_VSTR:
				strcpy(mmparam.m_strValue, params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_strValue = "<<mmparam.m_strValue<<endl;
				break;
			default:
				throw CException(__FILE__, __LINE__, "未知数据类型!");				
		}
		
		mmparams.push_back(mmparam);
	}
	
	//执行插入
	return insert(tablename, mmparams);

}


int MmInterface::insert(const string &tablename, vector<string> &colnames, vector<MmParam> &params)
{
	long lv = 0;
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库插入失败, 会话信息不存在!");
	}
	
	//输入参数准备
	InputParams *inputparams = new InputParams(params.size());
	//执行插入
	try
	{
		for (int i=0; i<params.size(); ++i)
		{
			switch(params[i].m_type)
			{
				case VAR_TYPE_INT2:
					inputparams->setValue(params[i].m_type, &params[i].m_shortValue);
					break;
				case VAR_TYPE_INT:
					inputparams->setValue(params[i].m_type, &params[i].m_intValue);
					break;
				case VAR_TYPE_DATE:
					//日期型从字符型转换成日期型
					lv = atol(params[i].m_dateValue);
					inputparams->setValue(params[i].m_type, &lv);
					break;
				case VAR_TYPE_LONG:
					inputparams->setValue(params[i].m_type, &params[i].m_longValue);
					break;
				case VAR_TYPE_REAL:
					inputparams->setValue(params[i].m_type, &params[i].m_floatValue);
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					inputparams->setValue(params[i].m_type, params[i].m_strValue);	
					break;
				default:
					//引起异常
					inputparams->setValue(params[i].m_type, &params[i].m_intValue);
			}
		}
	
		int ires = m_session->insert(tablename.c_str(), colnames, inputparams);
		//清理数据
		delete inputparams;
		return ires;
	}
	catch(Mdb_Exception &e)
	{
		//清理数据
		delete inputparams;
	  // modify by wangfc 2010-05-11 	
		//throw CException(__FILE__, __LINE__, "内存库插入失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)插入失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
}


int MmInterface::insert(const string &tablename, vector<string> &colnames, vector<string> &params)
{
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库插入失败, 会话信息不存在!");
	}
	
	const TableDef* tabledef = NULL;
	try
	{
		//获取插入的列数
		tabledef = m_session->getTableDef(tablename.c_str());
	}
	catch(Mdb_Exception &e)
	{
	  // modify by wangfc 2010-05-11
		// throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s",m_alisname.c_str(), e.GetString());
	}
	
	//输入参数准备
	vector<MmParam> mmparams;
	mmparams.clear();
	for (int i=0; i<params.size(); ++i)
	{
		MmParam mmparam;
		mmparam.m_indextag = 0;
		mmparam.m_type = tabledef->m_columnList[i].m_type;
//cout<<"MmInterface::insert----------mmparam.m_type = "<<mmparam.m_type<<endl;
		switch(mmparam.m_type)
		{
			case VAR_TYPE_INT2:
				mmparam.m_shortValue = atoi(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_shortValue = "<<mmparam.m_shortValue<<endl;
				break;
			case VAR_TYPE_INT:
				mmparam.m_intValue = atoi(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_intValue = "<<mmparam.m_intValue<<endl;
				break;
			case VAR_TYPE_DATE:
				strcpy(mmparam.m_dateValue, params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_dateValue = "<<mmparam.m_dateValue<<endl;
				break;
			case VAR_TYPE_LONG:
				mmparam.m_longValue = atol(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_longValue = "<<mmparam.m_longValue<<endl;
				break;
			case VAR_TYPE_REAL:
				mmparam.m_floatValue = atof(params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_floatValue = "<<mmparam.m_floatValue<<endl;
				break;
			case VAR_TYPE_NUMSTR:
			case VAR_TYPE_VSTR:
				strcpy(mmparam.m_strValue, params[i].c_str());
//cout<<"MmInterface::insert----------mmparam.m_strValue = "<<mmparam.m_strValue<<endl;
				break;
			default:
				throw CException(__FILE__, __LINE__, "未知数据类型!");				
		}
		
		mmparams.push_back(mmparam);
	}
	
	//执行插入
	return insert(tablename, colnames, mmparams);

}


int MmInterface::update(const string &tablename, const string &indexname, const string &expr, vector<MmParam> &params,
				   		vector<string> &fieldnames, vector<MmParam> &values)
{
	long lv = 0;
	
	//如果没有会话
	if (m_session == NULL)
	{
		throw CException(__FILE__, __LINE__, "内存库删除失败, 会话信息不存在!");
	}
	
	//更新条件参数准备
	//查询参数准备
	int iIndexParamNum = 0;
	int iOthreParamNum = 0;
	for (int i=0; i<params.size(); ++i)
	{
		if (params[i].m_indextag == 1)
		{
			++iIndexParamNum;
		}
		else
		{
			++iOthreParamNum;
		}
	}		
	InputParams *indexparams = new InputParams(iIndexParamNum);
	InputParams *otherparams = new InputParams(iOthreParamNum);
	InputParams *updatevalues = new InputParams(values.size());
	//执行更新
	try
	{
		for (int i=0; i<params.size(); ++i)
		{
			if (params[i].m_indextag == 1)
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						indexparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						indexparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						indexparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						indexparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						indexparams->setValue(params[i].m_type, params[i].m_strValue);	
						break;
					default:
						//引起异常
						indexparams->setValue(params[i].m_type, &params[i].m_intValue);				
				}
			}
			else
			{
				switch(params[i].m_type)
				{
					case VAR_TYPE_INT2:
						otherparams->setValue(params[i].m_type, &params[i].m_shortValue);
						break;
					case VAR_TYPE_INT:
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);
						break;
					case VAR_TYPE_DATE:
						//日期型从字符型转换成日期型
						lv = atol(params[i].m_dateValue);
						otherparams->setValue(params[i].m_type, &lv);
						break;
					case VAR_TYPE_LONG:
						otherparams->setValue(params[i].m_type, &params[i].m_longValue);
						break;
					case VAR_TYPE_REAL:
						otherparams->setValue(params[i].m_type, &params[i].m_floatValue);
						break;
					case VAR_TYPE_NUMSTR:
					case VAR_TYPE_VSTR:
						otherparams->setValue(params[i].m_type, params[i].m_strValue);	
						break;
					default:
						//引起异常
						otherparams->setValue(params[i].m_type, &params[i].m_intValue);
				}
			}
		}
		
		//更新字段参数准备
		for (int i=0; i<values.size(); ++i)
		{
			switch(values[i].m_type)
			{
				case VAR_TYPE_INT2:
					updatevalues->setValue(values[i].m_type, &values[i].m_shortValue);
					break;
				case VAR_TYPE_INT:
					updatevalues->setValue(values[i].m_type, &values[i].m_intValue);
					break;
				case VAR_TYPE_DATE:
					//日期型从字符型转换成日期型
					lv = atol(params[i].m_dateValue);
					updatevalues->setValue(params[i].m_type, &lv);
					break;
				case VAR_TYPE_LONG:
					updatevalues->setValue(values[i].m_type, &values[i].m_longValue);
					break;
				case VAR_TYPE_REAL:
					updatevalues->setValue(values[i].m_type, &values[i].m_floatValue);
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					updatevalues->setValue(values[i].m_type, values[i].m_strValue);	
					break;
				default:
					//引起异常
					updatevalues->setValue(values[i].m_type, &values[i].m_intValue);
			}
		}
	
		int ires = m_session->update(tablename.c_str(), indexname.c_str(), indexparams, fieldnames, updatevalues, expr.c_str(), otherparams);
		
		//执行清理
		delete indexparams;
		delete otherparams;
		delete updatevalues;
		
		return ires;
	}
	catch(Mdb_Exception &e)
	{
		//执行清理
		delete indexparams;
		delete otherparams;
		delete updatevalues;
		//modify by wangfc 2010-05-11
		//throw CException(__FILE__, __LINE__, "内存库插入更新, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)插入更新, 信息%s",m_alisname.c_str(), e.GetString());
	}
}


int MmInterface::update(const string &tablename, const string &updatesql, vector<string> &values)
{
	//indexname|((column1>=:0) and .. )and (columnN>:N)|(column1;column2;…;columnN)注意中间不允许有空格
	//分析updatesql,获取其中的indexname, expr, updateparam

	vector<string> sVector;
	StringUtil::split(updatesql, "|", sVector, false);
	string indexname = sVector[0];
	string expr = sVector[1];
	string updateparam = sVector[2];
	
	//获取更新的表达式列名称集和更新列名称集
	vector<string> exprColumnNames;
	vector<string> updateColumnNames;
	getColumnName(expr.c_str(), exprColumnNames);
	getColumnName(updateparam.c_str(), updateColumnNames);
	//获取表和索引描述
	const TableDef* tabledef = NULL;
	const IndexDef* indexdef = NULL;
	try
	{
		tabledef = m_session->getTableDef(tablename.c_str());
		indexdef = m_session->getIndexDef(tablename.c_str(), indexname.c_str());
	}
	catch(Mdb_Exception &e)
	{
	  //modify by wangfc 2010-05-11
		//throw CException(__FILE__, __LINE__, "内存库查询失败, 信息%s", e.GetString());
		throw CException(__FILE__, __LINE__, "内存库(%s)查询失败, 信息%s", m_alisname.c_str(),e.GetString());
	}
	
	//更新参数准备
	vector<MmParam> mmparams;
	vector<MmParam> mmvalues;
	for (int i=0; i<values.size(); ++i)
	{
		MmParam mmparam;
		if (i < indexdef->m_columnNum)
		{	
			mmparam.m_indextag = 1;
			mmparam.m_type = getColumnType(tabledef, indexdef->m_columnList[i]);
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, values[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(values[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(values[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, values[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");				
			}
			
			mmparams.push_back(mmparam);
		}
		else if (i < indexdef->m_columnNum+exprColumnNames.size())
		{
			mmparam.m_indextag = 0;
			mmparam.m_type = getColumnType(tabledef, exprColumnNames[i-indexdef->m_columnNum].c_str());
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, values[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(values[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(values[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, values[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");				
			}
			
			mmparams.push_back(mmparam);
		}
		else
		{
			mmparam.m_indextag = 0;
			mmparam.m_type = getColumnType(tabledef, updateColumnNames[i-indexdef->m_columnNum-exprColumnNames.size()].c_str());
			switch(mmparam.m_type)
			{
				case VAR_TYPE_INT2:
					mmparam.m_shortValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_INT:
					mmparam.m_intValue = atoi(values[i].c_str());
					break;
				case VAR_TYPE_DATE:
					strcpy(mmparam.m_dateValue, values[i].c_str());
					break;
				case VAR_TYPE_LONG:
					mmparam.m_longValue = atol(values[i].c_str());
					break;
				case VAR_TYPE_REAL:
					mmparam.m_floatValue = atof(values[i].c_str());
					break;
				case VAR_TYPE_NUMSTR:
				case VAR_TYPE_VSTR:
					strcpy(mmparam.m_strValue, values[i].c_str());
					break;
				default:
					throw CException(__FILE__, __LINE__, "未知数据类型!");				
			}
			
			mmvalues.push_back(mmparam);
		}
	}
	
	//执行更新
	return update(tablename, indexname, expr, mmparams, updateColumnNames, mmvalues);
	
}


bool MmInterface::getNext()
{
	return m_session->getNextRec();
}


int MmInterface::getColumnnum()
{
	return m_columnnum;
}


short MmInterface::getColumnShortValue(const unsigned int &pos)
{
	int len = 0;
	short value = 0;
	m_session->getColumnValue(pos-1, &value, len);

	return value;
}


int MmInterface::getColumnIntValue(const unsigned int &pos)
{
	int  len = 0;
	int  value = 0;
	m_session->getColumnValue(pos-1, &value, len);

	return value;
}


long MmInterface::getColumnLongValue(const unsigned int &pos)
{
	int  len = 0;
	long value = 0;
	m_session->getColumnValue(pos-1, &value, len);

	return value;
}


float MmInterface::getColumnFloatValue(const unsigned int &pos)
{
	int   len = 0;
	float value = 0.0;
	m_session->getColumnValue(pos-1, &value, len);

	return value;
}


string MmInterface::getColumnDateValue(const unsigned int &pos)
{
	int    len = 0;
	long   dateValue = 0;
	m_session->getColumnValue(pos-1, &dateValue, len);
	
	char tmpc[15];
	sprintf(tmpc, "%ld", dateValue);
	string value = tmpc;
	return value;
}


string MmInterface::getColumnStringValue(const unsigned int &pos)
{
	int  len = 0;
	char stringValue[MAX_MMSTRVALUE_LEN];
	memset(stringValue, 0x0, MAX_MMSTRVALUE_LEN);
	m_session->getColumnValue(pos-1, stringValue, len);
	
	string value = stringValue;
	return value;
}


void MmInterface::tr