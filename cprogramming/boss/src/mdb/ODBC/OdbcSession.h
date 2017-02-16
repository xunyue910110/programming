// ############################################
// Source file : OdbcSession.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-9-17
// Update      : 2008-9-17
// Copyright(C): chen min, Linkage.
// ############################################

// 封装标准ODBC接口,针对支持ODBC标准的第三方内存数据库

#ifndef ODBCSESSION_H_HEADER_INCLUDED_B72F0591
#define ODBCSESSION_H_HEADER_INCLUDED_B72F0591

#include "Session.h"
#include "PublicInfo.h"

#define BATCHARRAYSIZE 2000
#ifdef TIMESTEN
	#include <sql.h>
	#include <sqltypes.h>
	#include <sqlext.h>
	#include <timesten.h>
	#ifdef _WIN32
		#include <windows.h>
	#else
		#include <sqlunix.h>
	#endif
	const int FIRST_COLUMN_NUMBER = 1;    // SQLColumns的字段位置顺序从1开始
	// add by chenm 2009-7-8 17:12:39 TT 中无SQL_C_SBIGINT类型,用SQL_C_CHAR替代
	//const SQLSMALLINT SQL_C_SBIGINT=SQL_C_CHAR;
	// over 2009-7-8 17:12:43
#elif defined ALTIBASE	
	#include <sqlcli.h>
	const int FIRST_COLUMN_NUMBER = 1;    // // SQLColumns的字段位置顺序从0开始
#endif
#define MAX_SQL_CATCH_LEN 4096
typedef struct BINDPARAMINFO
{
	SQLSMALLINT cType;
	SQLSMALLINT sqlType;
	SQLUINTEGER columnSize;
	SQLSMALLINT scale;
	SQLSMALLINT nullAble;
	//add by chenzm@2009-7-28
	char *      data;
	//add end	
} BindParamInfo;
typedef struct RESULTCOLDATA
{
	void * m_columnPtr;	//void *
	#if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha))
		unsigned long m_columnLen;
	#else
		unsigned int m_columnLen;
	#endif
	short m_columnType;
	short m_columnCType;
    // add by chenm 2009-7-8 17:24:30
   #if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha))
    long * m_nullAble;
   #else
	SQLINTEGER* m_nullAble;
   #endif
	// over 2009-7-8 17:25:05

} ResultColData;
class BatchBindParam
{
	public:
		BatchBindParam(const int& paramCount)
		{
			m_paramCount = paramCount;
			m_arraySize = 0;
			void * ptr=NULL;
			int  * pind=NULL;
			for (int i=0;i<m_paramCount;i++){
				m_pColumnPtr.push_back(ptr);
				m_columnSize.push_back(0);
				m_pInd.push_back(pind);
			}
		}
		~BatchBindParam()
		{
			for (int i=0;i<m_paramCount;i++)
			{
				if (m_pColumnPtr[i] != NULL){
					free(m_pColumnPtr[i]);
					m_pColumnPtr[i] = NULL;
				}
				if (m_pInd[i] != NULL){
					free(m_pInd[i]);
					m_pInd[i] = NULL;
				}
			}
		}
		
		int& getArraySize(){return m_arraySize;}
		void* getColumnPtr(const int& i)
		{
			if (m_pColumnPtr[i] == NULL){
				m_pColumnPtr[i] = calloc(BATCHARRAYSIZE,m_columnSize[i]);
			}
			return m_pColumnPtr[i];
		}
		int & getColumnSize(const int& i){return m_columnSize[i];}
		int*  getInd(const int&i)
		{
			if (m_pInd[i] == NULL){
				m_pInd[i] = (int *)calloc(BATCHARRAYSIZE,sizeof(int));
			}
			return m_pInd[i];
		}
		SQLUSMALLINT* getRowStatus(){return &m_rowStatus[0];}
		short& getCType(){return ctype;}
		short& getSqlType(){return sqltype;}
	private:
		int m_paramCount;
		int m_arraySize;//实际数组长度
		//vector大小为字段个数，每个指针为同一字段的连续内存,长度为m_columnSize[i]*BATCHARRAYSIZE
		vector<void *> m_pColumnPtr;
		//vector大小为字段个数
		vector<int>    m_columnSize;
		//vector大小为字段个数,每个指针为连续内存，长度为 sizeof(int) * BATCHARRAYSIZE
		vector<int*>    m_pInd;
		SQLUSMALLINT m_rowStatus[BATCHARRAYSIZE];
		short ctype;
		short sqltype;
};
typedef struct STMTINFO
{
	HSTMT m_stmt;
	vector<BindParamInfo> m_vParamInfo;
   #if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha)) // by chenm 2009-7-9 
    vector<long> m_vInd;
   #else	
	vector<int> m_vInd;
   #endif           // over 2009-7-9 
	short  m_columnCount;
	vector<ResultColData> m_colsData;
	int m_fetchArraySize;
	int m_updateArraySize;
	SQLUINTEGER m_fetchedRowCount;
	SQLUINTEGER m_currentNum;
	SQLUSMALLINT * m_rowStatus;
	BatchBindParam * m_pBatchParam;
	void *getPtr(int iSequnce)
	{
		return (void *)((char *)m_colsData[iSequnce].m_columnPtr+m_currentNum*(m_colsData[iSequnce].m_columnLen));
	}
	SQLINTEGER getNullAble(int iSequnce) 
	{
		return m_colsData[iSequnce].m_nullAble[m_currentNum];
	}
	
}StmtInfo;
class SQLCatchInfo
{
	public:
		SQLCatchInfo(){};
		SQLCatchInfo(const char* r_tname,const char *r_iname,const char *r_sql);
		void setSQL(const char* r_tname,const char *r_iname,const char *r_sql);
		//SQLCatchInfo(const SQLCatchInfo& rhs);
		~SQLCatchInfo();
	    bool operator<(SQLCatchInfo const& right) const;
	    bool operator == (SQLCatchInfo const& right) const;
	    T_NAMEDEF m_tableName;
	    T_NAMEDEF m_indexName;
		string m_sqlStr;
};
//##ModelId=480D9C4D02BF
/*
class ResultColsAttribs
{
	public:
		ResultColsAttribs()
		{
			m_columnPtr = NULL;	//void *
			m_columnLen = NULL; //unsigned int *
			m_columnType= NULL; //short *
			m_nullAble  = NULL; //SQLINTEGER*
			m_columnCount = 0;  //short
		}
		~ResultColsAttribs()
		{
			this->reset();		
		}
		void reset()
		{
			for (int i=0; i<m_columnCount; i++ )
            {
				free( m_columnPtr[i] );
            } 
            if( m_columnPtr!=NULL)
            {
            	free( m_columnPtr );
            	m_columnPtr = NULL;            	
            }
            if( m_columnLen!=NULL)
            {
            	free( m_columnLen );
            	m_columnLen = NULL;
            } 
            if ( m_columnType != NULL )
            {
            	free( m_columnType);
            	m_columnType = NULL;	
            }
            if ( m_nullAble != NULL )
            {
            	free( m_nullAble);
            	m_nullAble = NULL;	
            }
            
            m_columnCount = 0;	
		}
	
	public:	
		void **m_columnPtr;
		short *m_columnType;//add by chenzm@20081217
		SQLINTEGER* m_nullAble;
	#ifdef ALTIBASE
		unsigned int  *m_columnLen;
	#elif defined TIMESTEN
		unsigned long  *m_columnLen;
	#endif 
		
		short  m_columnCount;	
};
*/
class OdbcSession : public Session
{
	public:
		//##ModelId=48D0AB720167
		OdbcSession(T_NAMEDEF hostname
							,T_NAMEDEF dbUserName
							,T_NAMEDEF dbUserPwd
							,int iPortNo	
							,int iConnType=1 // add by chenm 2009-7-21 For altibase 1 : TCP/IP, 2 : UNIXDOMAIN, 3 : IPC
							);
		
		//##ModelId=48D0AB720177
		virtual ~OdbcSession();

		virtual bool createTable(const TableDef &theTableDef);
		virtual bool dropTable(const char * cTableName);
		virtual bool truncate(const char * cTableName);
		
		virtual bool createIndex(const IndexDef &theIndexDef);
		virtual bool dropIndex(const char * cTableName,const char * cIndexName);
		
		virtual void begin(const char *cTableName,int iLockType = 1) ;
		virtual bool commit() ;
		virtual bool rollBack() ;
		
		virtual int select(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams       //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
						,const int iRowCount=-1);
		virtual int  deleteRec(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams);       //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应

		virtual bool insert(const char *cTableName
			    		,const InputParams *pInputParams)    ;  // 需要插入的记录分字段内容

		virtual bool insert(const char *cTableName
						,const vector<string> &cFieldNames
			    		,const InputParams *pInputParams)    ;  // 需要插入的记录分字段内容

        //批量插入
		//virtual bool insert(const char *cTableName
		//	    		,const vector<InputParams *>&pInputParams)=0    ;  // 需要插入的记录分字段内容		
		//批量插入
		//virtual bool insert(const char *cTableName
		//				,const vector<string> &cFieldNames
		//	    		,const vector<InputParams *>&pInputParams)=0    ;  // 需要插入的记录分字段内容
		
		//Modified By Suanm 2009/11/4 19:39:52 errVector返回出错字段索引
		// 需要插入的记录分字段内容		
		virtual bool insert(const char *cTableName
			    		,const vector<InputParams *>&pInputParams, vector<int>* errVector=NULL);  

		//批量插入
		// 需要插入的记录分字段内容
		virtual bool insert(const char *cTableName
						,const vector<string> &cFieldNames
			    		,const vector<InputParams *>&pInputParams, vector<int>* errVector=NULL);  

		virtual int  update(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
						,vector<string> &sFieldNames                // 需要更新的字段名
						,const InputParams *pUpdateValues           // 需要更新的字段值
						,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams)   ;     //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
						
		virtual int update(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams);
		//add by chenzm@2009-7-31 批量用户
		virtual int update(const char *cTableName
						,const char *cIndexName
						,const vector<InputParams *>pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams);
		//add end		
		virtual bool getNextRec();
		
		virtual bool getColumnValue(const char* columnName, void * value);
		virtual bool getColumnValue(const char* columnName, void * value,int &iColumnLenth);
		virtual bool getColumnValue(int iSequnce, void * value,int &iColumnLenth) ;

		virtual int getInt(const char* columnName);
		virtual long getLong(const char* columnName);
		virtual double getDouble(const char* columnName);
		virtual string getString(const char* columnName);
		virtual bool getString(const char* columnName,char *r_value);//效率较返回string对象的高
		virtual bool getDate(char* columnName,void *r_value);//程序中使用TIMESTAMP_STRUCT*指针转化

		virtual int getInt(int iSequnce);
		virtual long getLong(int iSequnce);
		virtual double getDouble(int iSequnce);
		virtual string getString(int iSequnce);
		virtual bool getString(int iSequnce,char *r_value);
		virtual bool getDate(int iSequnce,void *r_value);
		
		virtual TableDef* getTableDef(const char* tableName);
		virtual IndexDef * getIndexDef(const char* tableName,const char* indexName);
	
		virtual void setSessionID(int &i){};
		virtual int  getSessionID(){return 0;};
		
		//Added By Sunam 2009/8/31 21:37:17
		//virtual void dataBind(vector<int> &errVect);
	protected:
		// 虚函数,剥离上面各public函数中SQL语句的拼装,今后集成其它第三方内存数据库的时候,可以覆盖此函数,而上面的public函数,可以不变
		// 提高代码的复用度
		virtual void excuteSQL(const string &sSql);
		
		// 释放ODBC申请的连接资源
		//##ModelId=48D074C602BF
		virtual void freeHandle();
		
		// 申请相关句柄
		//##ModelId=48D092AF00AB
		virtual void allocHandle();
		
		virtual void throwErr(char * filename, int linenum);
		
		// 返回sql操作生效的记录数
		virtual int effectRowCount();

		void getSqlFormatColumnValue(const InputParams *&pInputParams
		                             ,const int &iSequnce
		                             ,char *cValue);
		                             
		// 组装sql语句中的where后条件字符串
		void asembleCondition(const char *cTableName
									,const char *cIndexName
									,const InputParams *pIndexKeyParams
									,const char *cExpr             
									,const InputParams *pOtherParams
									,string &sSql);
		// 绑定返回结果集字段变量
		void bindResultCols(const char *cTableName,HSTMT *r_pStmt);
		
		bool getDeleteSQL(const char *cTableName
						 ,const char *cIndexName
						 ,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						 ,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						 ,const InputParams *pOtherParams)   ;  //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		bool bindDeleteParam(StmtInfo & r_stmtInfo
						 ,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						 ,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						 ,const InputParams *pOtherParams)   ;  //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		
		bool getUpdateSQL(const char *cTableName
						 ,const char *cIndexName
						 ,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
						 ,vector<string> &sFieldNames                // 需要更新的字段名
						 ,const InputParams *pUpdateValues           // 需要更新的字段值
						 ,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
						 ,const InputParams *pOtherParams)   ;     //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		bool bindUpdateParam(StmtInfo & r_stmtInfo
						 ,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
						 ,const InputParams *pUpdateValues           // 需要更新的字段值
						 ,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
						 ,const InputParams *pOtherParams)   ;     //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		
		bool getSelectSQL(const char *cTableName
						 ,const char *cIndexName
						 ,const InputParams *pIndexKeyParams
						 ,const char *cExpr             
						 ,const InputParams *pOtherParams);
		bool bindSelectParam(StmtInfo & r_stmtInfo
						 ,const InputParams *pIndexKeyParams
						 ,const char *cExpr             
						 ,const InputParams *pOtherParams);

		bool bindOneParams(StmtInfo & r_stmtInfo,const InputParams *pParams,int &iStartPos,bool isExpr=false);
		bool getInsertSQL(const char *cTableName,const InputParams *pInputParams);
		bool bindInsertParam(StmtInfo & r_stmtInfo
						 ,const InputParams *pInputParams);
		bool getInsertSQL(const char *cTableName
		                 ,const vector<string> &cFieldNames
			    		 ,const InputParams *pInputParams);
		//r_type:0-insert,1-delete,2-update,3-select
		map<SQLCatchInfo,StmtInfo>::iterator getStmt(int r_type
			                                     ,const char* r_tname
			                                     ,const char *r_iname);
		bool prepareSQL(StmtInfo & r_stmtInfo);
		bool getCTypeBySqlType(short r_sqlType,short & r_cType);
		short getCTypeByInputParam(const InputParams *&pInputParams
								,const int &iSequnce);
		short getSQLTypeByInputParam(const InputParams *&pInputParams
										,const int &iSequnce);
		int getInputParamLen(const InputParams *&pInputParams
		                  ,const int &iSequnce);
		void freeStmt();
		void freeOneStmt(map<SQLCatchInfo,StmtInfo> *t_pMap);
		void replaceExprBindParam(const char *cExpr,const InputParams *pOtherParams);
		void replaceExprBindParam(const char *cExpr,const vector<InputParams *> &OtherParams);
		bool fetchRec();
		int updateBatch(const char *cTableName
						,const char *cIndexName
						,const vector<InputParams *>pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams
						,int fromPos,int arraySize);

        //Modified By Sunam 2009/11/4 19:45:50
		//bool insertBatch(const char *cTableName,const vector<InputParams *>&pInputParams,int fromPos,int arraySize);		
		bool insertBatch(const char *cTableName,const vector<InputParams *>&pInputParams,int fromPos,int arraySize, vector<int>* errVector=NULL);
	private:	
		// 环境句柄
		//##ModelId=48D0754903B9
		HENV m_env;
		// 连接句柄
		//##ModelId=48D075C701A5
		HDBC m_dbc;
		// SQL语句句柄
		//##ModelId=48D0743902CE
		HSTMT *m_pCurrentStmt;
		StmtInfo * m_pCurrentStmtInfo;
		// 索引--索引字段对应
		INDEX_DEF_MAP m_indexDefsMap;
		TABLE_DEF_MAP        m_tableDefsMap;
		// 字段顺序和表名的对应关系map
		TABLE_COLOMN_POS_MAP m_tableColumnMap;		
		TABLE_COLOMN_POS_MAP_ITR m_tableColumnMapItr;
		
		//ResultColsAttribs m_rs;
		map<SQLCatchInfo,StmtInfo> m_insertCatch;
		map<SQLCatchInfo,StmtInfo> m_deleteCatch;
		map<SQLCatchInfo,StmtInfo> m_updateCatch;
		map<SQLCatchInfo,StmtInfo> m_selectCatch;
		char m_sql[32*1024];
		SQLCatchInfo m_catchInfo;
		//宿主变量的位置,InputParams中的次序
		map<int,int> m_posMap;
		//int m_bufferSize;
		//char *m_pBuffer;
	#ifdef ALTIBASE		
		HSTMT m_CommitStmt;
	#endif
};



#endif /* ODBCSESSION_H_HEADER_INCLUDED_B72F0591 */
