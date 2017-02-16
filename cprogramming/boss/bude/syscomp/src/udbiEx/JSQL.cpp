/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：JSQL.cpp
 *  文件标识：
 *  摘    要：
 * 
 *  当前版本：1.0
 *  作    者：WangFJ
 *  完成日期：2004年12月06日
 *
 *  取代版本： 
 *  原作者  ：
 *  完成日期：
 */        

#include "JSQL.h"
#include "jdb.h"
#include "JConnect.h"
#include "JTable.h"
#include "udbi-inter.h"

#include <fstream>
#include <sys/stat.h>

/**
 *  \class UDBI::JSQL  JSQL.h
 *  \brief SQL语句缓存类
 *
 *  BOSS1.5研发项目定义所有SQL语句配置在数据库中，DAO或业务组件中通过传入
 *  SQL语句引用标识来关联使用的SQL语句。JSQL类即实现了根据指定表名和SQL语
 *  句引用标识查询对应的SQL语句，并实现SQL语句的缓存功能，即只首次查询该
 *  SQL语句时需要从数据库中获取，之后即从缓存中获取。
 */

UDBI::JSQL_MAP UDBI::JSQL::sql_map;
time_t  UDBI::JSQL::lastUpdateTime = 0;
string  UDBI::JSQL::lastUpdateVer = "";

/**
 *  \fn  string UDBI::JSQL::getSQL(string &byStr)
 *  SQL语句获取方法，首次获取时从数据库中查询，并缓存起来，之后即从缓存中获取。
 */
void UDBI::JSQL::getSQL(string& sqlStmt, const string& tabName, const string& sqlRef)
{
	UDBI_DEBUG_P2("JSQL::getSQL(tabName='%s', sqlRef='%s')", tabName.c_str(), sqlRef.c_str());
	
	string  sTabSqlRef(tabName);
	sTabSqlRef += sqlRef;
	
	UDBI::JSQL_MAP::const_iterator iter=sql_map.find(sTabSqlRef);
	if (iter != sql_map.end()) {
		//UDBI_DEBUG("JSQL::getSQL(): from buf ");
		sqlStmt = (*iter).second;
	} else {
		//UDBI_DEBUG("JSQL::getSQL(): from database ");
		//g_jsqlBug = JSQL_BUG_CLOSE;
		setJSqlBug(JSQL_BUG_CLOSE);

		JConnect* tmpcon = jdb::getConnect();
		JTable* pTab = tmpcon->getJTable();
		pTab->Clear();
		pTab->AddSql("select sql_stmt from code_code where tab_name=:vTabName and sql_ref = :vSqlRef");
		pTab->SetParam(":VTABNAME", tabName);
		pTab->SetParam(":VSQLREF", sqlRef);
		pTab->ExecSelect();
		if ( pTab->Fetch() ) {
			sql_map[sTabSqlRef] = pTab->GetString("SQL_STMT");
			sqlStmt = sql_map[sTabSqlRef];
		}
		
		//g_jsqlBug = JSQL_BUG_OPEN;
		setJSqlBug(JSQL_BUG_OPEN);
	}
}

/**
 *  \fn  string UDBI::JSQL::getSQL(string &byStr)
 *  SQL语句获取方法，首次获取时从数据库中查询，并缓存起来，之后即从缓存中获取。
 *  增加了SQL 语句重构支持
 */
void UDBI::JSQL::getSQL(string& sqlStmt, const string& tabName, const string& sqlRef, const string& swhere)
{
	UDBI_DEBUG_P2("JSQL::getSQL(tabName='%s', sqlRef='%s')", tabName.c_str(), sqlRef.c_str());
	
	/// check in-parameters
	if ( swhere.empty() )
		THROW_C(EUdbiException, 9100, "SQL语句重构参数为空！");
		
    string::size_type idx = swhere.find("==");	
    if (idx == string::npos) 
    	THROW_C_P1(EUdbiException, 9100, "SQL语句重构参数格式错误！[swhere = '%s']", swhere.c_str()); 
        
    string findstr = swhere.substr(0, idx);
    string replstr = swhere.substr(idx+2);

	/// get sql statement
	string  sTabSqlRef(tabName);
	sTabSqlRef += sqlRef;
	
	UDBI::JSQL_MAP::const_iterator iter=sql_map.find(sTabSqlRef);
	if (iter != sql_map.end()) {
		sqlStmt = (*iter).second;
	} else {
		//g_jsqlBug = JSQL_BUG_CLOSE;
		setJSqlBug(JSQL_BUG_CLOSE);

		JTable* pTab = jdb::getConnect()->getJTable();
		pTab->Clear();
		pTab->AddSql("select sql_stmt from code_code where tab_name=:vTabName and sql_ref = :vSqlRef");
		pTab->SetParam(":VTABNAME", tabName);
		pTab->SetParam(":VSQLREF", sqlRef);
		pTab->ExecSelect();
		if ( pTab->Fetch() ) {
			sql_map[sTabSqlRef] = pTab->GetString("SQL_STMT");
			sqlStmt = sql_map[sTabSqlRef];
		}
		
		//g_jsqlBug = JSQL_BUG_OPEN;
		setJSqlBug(JSQL_BUG_OPEN);
	}
	
	/// re-spell sql statement
    idx = sqlStmt.find(findstr);
    if (idx == string::npos) 
    	THROW_C_P1(EUdbiException, 9100, "预做重构的SQL语句中没有指定占位符！[swhere='%s']", swhere.c_str());
    	        	
    sqlStmt.replace(idx, findstr.length(), replstr);	
}

/**
 *  SQL 语句缓存刷新
 */
int UDBI::JSQL::update(const string &info)
{
	//获取文件时间戳, 比较时间戳判断是否需要更新
	const char *sqlUpdateFile = getenv("SQL_PATCHINFO");
	if ( (!sqlUpdateFile) || (strlen(sqlUpdateFile)==0) ) {
		return -1;
	}

	struct stat fs;
	stat(sqlUpdateFile, &fs);
	if (lastUpdateTime == fs.st_mtime) {
		return 0;		
    }
    
	//需要更新，则读取TODO内容，循环更新
    lastUpdateTime = fs.st_mtime;
	string tmpVer(lastUpdateVer);
	
	int flag = 0;
    ifstream in(sqlUpdateFile);
    
    if ( !in.good() ) {
    	return -1;
    }
    
    while (true) {
        char line[256] = {0};

        in.getline(line, sizeof(line)-1);
        if (in.fail()) break;

        if (line[0] == '#') continue;

        int len = strlen(line);
        if (len == 0) continue;

        //兼容windows格式
        if (line[len - 1] == '\r') line[len - 1] = '\0';
		
		string tmpstr(line);

		if ((!tmpVer.empty()) && (tmpstr.find(tmpVer) != string::npos)) break;

		if ((flag == 0) && 
			(tmpstr[0] == '!') && 
			(tmpstr.find("!patch") != string::npos)) {
			lastUpdateVer = tmpstr;
			flag = 1;
			
			continue;
		}
		
		if (flag == 1) {
			string::size_type idx = tmpstr.find("::");
			if (idx == string::npos) continue;
			
			string tmpstr2 = tmpstr.substr(idx+2);
			idx = tmpstr2.find("::");
			if (idx == string::npos) continue;
				
			tmpstr2.replace(idx, 2, "");
			sql_map.erase(tmpstr2);
		}

        if (in.eof()) break;
    }

    return 0;
}

