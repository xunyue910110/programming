#ifndef __DMLSQL_INCLUDE_H__
#define __DMLSQL_INCLUDE_H__
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include "Session.h"
#include "DataBase.h"
#include "Mdb_Exception.h"
#include "SpaceInfo.h"
#include "MemManager.h"
#include "Table.h"

class MdbDmlSql
{
	public:
		enum MDBSQLTYPE{
			SQLTYPE_SELECT,
			SQLTYPE_INSERT,
			SQLTYPE_UPDATE,
			SQLTYPE_DELETE}; 
		MdbDmlSql(DataBase* r_db,Session* r_session,MemManager* r_memMgr,bool r_lineMode);
		~MdbDmlSql();
		void clear();
		bool setSQL(const string& r_sql);
		bool parseSQL();
		
	protected:
		bool parseSelect();
		bool parseInsert();
		bool parseUpdate();
		bool parseDelete();
		bool getInputParams(InputParams *&pIndexParams,vector<string>& cols,vector<string>& values);
		bool getSelectCols(bool& isCount);
		bool getAllParts();
		void getLowerSQL(const string& r_old,string& r_new);
		long getTableRowCount(const string& r_tableName);
		bool getTableDesc();
		bool getIndexDesc();
		bool isValidColumn(const string& col);
		bool getWhereClause();
		bool parseWhereClause(const string& src);
		bool isValidCon(const string& src,string&col,string&val);
		bool doSelect();
		int  getTableColPos(const string& col);
		bool getUpdateCols();
		bool doUpdate();
		bool doDelete();
	protected:
		
		bool isBlank(char c)
		{
			return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
		}
		string toUpper(const string& str);
		string toLower(const string& str);
		bool isNumber(const string& r_src);
		string lTrim(const string& src);
		string rTrim(const string &src);
		
	protected:
		string m_sql;
		string m_lowerSql;
		//vector<string> m_allParts;
		vector<string> m_selectCols;  //select和from之间的字段列表
		vector<string> m_indexCols;   //索引字段名称
		vector<string> m_indexValues; //索引字段取值
		vector<string> m_insertCols;  //插入使用的字段名称
		vector<string> m_insertValues;//插入使用的字段值
		vector<string> m_updateCols;  //更新字段
		vector<string> m_updateValues;//更新值
		string m_expr;                //表达式
		vector<string> m_exprCols;    //表达式使用的字段
		vector<string> m_exprValues;  //表达式字段值
		string m_whereClause;
		string m_tableName;
		string m_indexName;
		MDBSQLTYPE m_sqlType;         //sql类型
		
		int iFromPos,iTablePos,iWherePos,iLimitPos,iSetPos,iIntoPos,iValuesPos;
		int iLimitCount;
		DataBase*   m_db;
		Session*    m_session;
		MemManager* m_memMgr;
		TableDesc*  m_tableDesc;
		IndexDesc*  m_indexDesc;
		vector<IndexDef> m_inndexList;
		bool        m_lineMode;
		
	private:
		
};
#endif
