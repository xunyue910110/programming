#include "COcciAPI.h"

OCCIConnect::OCCIConnect(const string&user, const string &passwd, const string &db,bool multiThreadsSupport)
	:env(NULL),conn(NULL)
{
	connect(user,passwd,db,multiThreadsSupport);
}
OCCIConnect::~OCCIConnect()
{
	disConnect();
}
bool OCCIConnect::connect(const string&user, const string &passwd, const string &db,bool multiThreadsSupport)
{
	disConnect();
	
	if (multiThreadsSupport)
		env = Environment::createEnvironment(Environment::THREADED_MUTEXED);
	else
		env = Environment::createEnvironment(Environment::DEFAULT);
	conn = env->createConnection(user, passwd, db);
	return true;
}
bool OCCIConnect::disConnect()
{
    if (conn != NULL)
    {
    	env->terminateConnection(conn);
    	conn = NULL;
    }
    if (env != NULL)
    {
    	Environment::terminateEnvironment(env);
    	env = NULL;
	}
	return true;
}
bool OCCIConnect::getTableColumnAttributes(const string& r_tableName, map<string,ColumnAttribute> & r_mAttrs)
{
	if (conn == NULL) return false;
	r_mAttrs.clear();
	MetaData emptab_metaData = conn->getMetaData(r_tableName, MetaData::PTYPE_TABLE);
	//int columnCount=emptab_metaData.getInt(MetaData::ATTR_NUM_COLS);
	vector<MetaData>listOfColumns;
	listOfColumns=emptab_metaData.getVector(MetaData::ATTR_LIST_COLUMNS);
	ColumnAttribute t_attr;
	for (int i=0;i<listOfColumns.size();i++)
	{
	   MetaData columnObj=listOfColumns[i];
	   t_attr.m_pos             = i;
	   t_attr.m_columnName      = columnObj.getString(MetaData::ATTR_NAME);
	   t_attr.m_columnType      = columnObj.getInt(MetaData::ATTR_DATA_TYPE);
	   t_attr.m_columnSize      = columnObj.getInt(MetaData::ATTR_DATA_SIZE);
	   t_attr.m_columnPrecision = columnObj.getInt(MetaData::ATTR_PRECISION);
	   t_attr.m_columnScale     = columnObj.getInt(MetaData::ATTR_SCALE);
       t_attr.m_nullAble        = columnObj.getBoolean(MetaData::ATTR_IS_NULL);
       r_mAttrs.insert(map<string,ColumnAttribute>::value_type(t_attr.m_columnName,t_attr));
	}
	return true;
}
bool OCCIConnect::getTableColumnAttributes(const string& r_tableName, vector<ColumnAttribute> & r_vAttrs)
{
	if (conn == NULL) return false;
	r_vAttrs.clear();
	MetaData emptab_metaData = conn->getMetaData(r_tableName, MetaData::PTYPE_TABLE);
	//int columnCount=emptab_metaData.getInt(MetaData::ATTR_NUM_COLS);
	vector<MetaData>listOfColumns;
	listOfColumns=emptab_metaData.getVector(MetaData::ATTR_LIST_COLUMNS);
	ColumnAttribute t_attr;
	for (int i=0;i<listOfColumns.size();i++)
	{
	   MetaData columnObj=listOfColumns[i];
	   t_attr.m_pos             = i;
	   t_attr.m_columnName      = columnObj.getString(MetaData::ATTR_NAME);
	   t_attr.m_columnType      = columnObj.getInt(MetaData::ATTR_DATA_TYPE);
	   t_attr.m_columnSize      = columnObj.getInt(MetaData::ATTR_DATA_SIZE);
	   t_attr.m_columnPrecision = columnObj.getInt(MetaData::ATTR_PRECISION);
	   t_attr.m_columnScale     = columnObj.getInt(MetaData::ATTR_SCALE);
       t_attr.m_nullAble        = columnObj.getBoolean(MetaData::ATTR_IS_NULL);
       r_vAttrs.push_back(t_attr);
	}
	return true;	
}
bool OCCISQL::getColumnAttributes(vector<ColumnAttribute> & r_vAttrs)
{
	if (rset == NULL) return false;
	r_vAttrs.clear();
	ColumnAttribute t_attr;
	vector<MetaData>cmd = rset->getColumnListMetaData();
	for (int i=0;i<cmd.size();i++)
	{
	   t_attr.m_pos             = i;
	   t_attr.m_columnName      = cmd[i].getString(MetaData::ATTR_NAME);
	   t_attr.m_columnType      = cmd[i].getInt(MetaData::ATTR_DATA_TYPE);
	   t_attr.m_columnSize      = cmd[i].getInt(MetaData::ATTR_DATA_SIZE);
	   t_attr.m_columnPrecision = cmd[i].getInt(MetaData::ATTR_PRECISION);
	   t_attr.m_columnScale     = cmd[i].getInt(MetaData::ATTR_SCALE);
       t_attr.m_nullAble        = cmd[i].getBoolean(MetaData::ATTR_IS_NULL);
       r_vAttrs.push_back(t_attr);
	}
	return true;
}
bool OCCISQL::getColumnAttributes(map<string,ColumnAttribute> & r_vAttrs)
{
	if (rset == NULL) return false;
	r_vAttrs.clear();
	ColumnAttribute t_attr;
	vector<MetaData>cmd = rset->getColumnListMetaData();
	for (int i=0;i<cmd.size();i++)
	{
	   t_attr.m_pos             = i;
	   t_attr.m_columnName      = cmd[i].getString(MetaData::ATTR_NAME);
	   t_attr.m_columnType      = cmd[i].getInt(MetaData::ATTR_DATA_TYPE);
	   t_attr.m_columnSize      = cmd[i].getInt(MetaData::ATTR_DATA_SIZE);
	   t_attr.m_columnPrecision = cmd[i].getInt(MetaData::ATTR_PRECISION);
	   t_attr.m_columnScale     = cmd[i].getInt(MetaData::ATTR_SCALE);
       t_attr.m_nullAble        = cmd[i].getBoolean(MetaData::ATTR_IS_NULL);
       r_vAttrs.insert(map<string,ColumnAttribute>::value_type(t_attr.m_columnName,t_attr));
	}
	return true;
}
