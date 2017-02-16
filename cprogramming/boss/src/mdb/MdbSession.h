// ############################################
// Source file : MdbSession.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-22
// Update      : 2008-4-22
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef MDBSESSION_H_INCLUDED
#define MDBSESSION_H_INCLUDED

#include "Session.h"

class TransDataMgr;
class ExpressionMgr;
class DataBase;
class Table;
// 自主研发MDB的session管理实现
class MdbSession : public Session
{
	public:
		MdbSession(DataBase *db);
		virtual ~MdbSession();
		
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
						,const InputParams *pOtherParams         //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
						,const int iRowCount=-1)   ;
						
		virtual int  deleteRec(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams)   ;  //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应

		virtual bool insert(const char *cTableName
			    		,const InputParams *pInputParams)    ;  // 需要插入的记录分字段内容

		virtual bool insert(const char *cTableName
			    		,const vector<string> &cFieldNames              // 指定插入字段顺序
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
						
		//Added 2008-7-8 批量更新
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
		//从字段名取字段的值
		virtual bool getColumnValue(const char* columnName, void * value);
		virtual bool getColumnValue(const char* columnName, void * value,int &iColumnLenth);
		virtual bool getColumnValue(int iSequnce, void * value,int &iColumnLenth) ;

		//以下尚未实现,请勿使用，add by chenzm@20081217
		virtual int getInt(const char* columnName) {return 0;}
		virtual long getLong(const char* columnName) {return 0;}
		virtual double getDouble(const char* columnName){return 0;}
		virtual string getString(const char* columnName){return "";}
		virtual bool getString(const char* columnName,char *r_value){return r_value;}
		virtual bool getDate(char* columnName,void *r_value){return true;}//程序中使用TIMESTAMP_STRUCT*指针转化

		virtual int getInt(int iSequnce) {return 0;}
		virtual long getLong(int iSequnce){return 0;}
		virtual double getDouble(int iSequnce){return 0;}
		virtual string getString(int iSequnce){return "";}
		virtual bool getString(int iSequnce,char *r_value){return r_value;}
		virtual bool getDate(int iSequnce,void *r_value){return true;}
		//以上get一族函数尚未实现

		virtual void setSessionID(int &i);
		virtual int  getSessionID();
		
		virtual const TableDef* getTableDef(const char* tableName);
		virtual const IndexDef * getIndexDef(const char* tableName,const char* indexName);

		virtual long getTableRowCount(const char* tableName);		

		// add by chenm 不等值查询类 处理逻辑和等值查询类基本一致 只不过调用table类的select 函数不同2009-6-3 15:35:54
		virtual int selectRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算 
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
									,const int iRowCount=-1);               
									
		virtual int  deleteRecRange(const char *cTableName
										,const char *cIndexName
										,const InputParams *pGtParams      //查询条件中的下限索引字段值
										,const InputParams *pLtParams      //查询条件中的上限索引字段值
										,const int iRangeTag               // 标示[]/[)/()/(]运算
										,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
										,const InputParams *pOtherParams);              
		
		virtual int  updateRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算
									,vector<string> &sFieldNames       // 需要更新的字段名
									,const InputParams *pUpdateValues  // 需要更新的字段值
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		 							);              
	 
		// over 2009-6-3 15:36:00
		//Added By Sunam 2009/8/31 21:37:17
		//virtual void dataBind(vector<int> &errVect);
	private:
		// 判断是否为跨表事务
		bool isAcrossTable(const char *cTableName);
		
		// 清除查询结果集
		void clearQueryResults();
		
		// add by chenm 2009-5-18 23:11:41
		int selectRowByRow();
	
	private:
		int                  m_sessionID;
		bool                 m_isTransaction;    // 是否为事务操作,由begin()函数设置,commit/rollBack()函数解除
		TransDataMgr         m_transDataMgr;
		ExpressionMgr        m_expressionMgr;
		Table               *m_table;
		vector<void *>       m_queryResults;        // 普通查询结果集
		vector<TransData *>  m_transQueryResults;   // 带事务查询结果集
		int                  m_resultSequnce;
		DataBase            * m_db;
		int                  m_iLockType;           // 表锁 or 行锁 by chenm 2009-4-2 
		
		const char          *m_cRowByRowExpr ;                // by chenm 2009-5-18 
		const InputParams   *m_pRowByRowOtherParams;		    // by chenm 2009-5-18 
		bool                 m_isRowByRowSelect;               // by chenm 2009-5-18
		const char 			*m_cRowByRowTableName;
		const char 			*m_cRowByRowIndexName;

};

#endif //MDBSESSION_H_INCLUDED
