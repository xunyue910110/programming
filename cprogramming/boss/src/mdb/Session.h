// ############################################
// Source file : Session.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-22
// Update      : 2008-4-22
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#include <vector>

#include "base/config-all.h"

USING_NAMESPACE(std)

class InputParams;
class IndexDef;
class TableDef;

// Session 抽象类
class Session
{
	public:
		Session(){}
		virtual ~Session(){}
		
		virtual bool createTable(const TableDef &theTableDef) =0;
		virtual bool dropTable(const char * cTableName) =0;
		virtual bool truncate(const char * cTableName) =0;
				
		virtual bool createIndex(const IndexDef &theIndexDef) =0;
		virtual bool dropIndex(const char * cTableName,const char * cIndexName) =0;
		
		virtual void begin(const char *cTableName,int iLockType = 1) =0;
		virtual bool commit() =0;
		virtual bool rollBack() =0;
		
		virtual int select(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams         //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
						,const int iRowCount=-1)   =0;  
						
		virtual int  deleteRec(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams)   =0;  //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应

		virtual bool insert(const char *cTableName
			    		,const InputParams *pInputParams)    =0;  // 需要插入的记录分字段内容
		
		virtual bool insert(const char *cTableName
			    		,const vector<string> &cFieldNames              // 指定插入字段顺序
			    		,const InputParams *pInputParams)    =0;  // 需要插入的记录分字段内容

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
			    		,const vector<InputParams *>&pInputParams, vector<int>* errVector=NULL)=0    ;  
		//批量插入
		// 需要插入的记录分字段内容
		virtual bool insert(const char *cTableName
						,const vector<string> &cFieldNames
			    		,const vector<InputParams *>&pInputParams, vector<int>* errVector=NULL)=0    ;  

		virtual int  update(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
						,vector<string> &sFieldNames                // 需要更新的字段名
						,const InputParams *pUpdateValues           // 需要更新的字段值
						,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams)   =0;     //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
						
		//Added 2008-7-8 批量更新
		virtual int update(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams) =0;
		//add by chenzm@2009-7-31 批量用户
		virtual int update(const char *cTableName
						,const char *cIndexName
						,const vector<InputParams *>pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams) =0;
		//add end
		virtual bool getNextRec() =0;
		
		virtual bool getColumnValue(const char* columnName, void * value) =0;
		virtual bool getColumnValue(const char* columnName, void * value,int &iColumnLenth)=0;
		virtual bool getColumnValue(int iSequnce, void * value,int &iColumnLenth) =0;
		//以下get系列函数，仅对odbcSession有效，其他的都没有实现。add by chenzm@20081217
		virtual int getInt(const char* columnName) = 0;
		virtual long getLong(const char* columnName) = 0;
		virtual double getDouble(const char* columnName) = 0;
		virtual string getString(const char* columnName) = 0;
		virtual bool getString(const char* columnName,char *r_value) = 0;//效率较返回string对象的高
		
		virtual bool getDate(char* columnName,void *r_value) = 0;//程序中使用TIMESTAMP_STRUCT*指针转化

		virtual int getInt(int iSequnce) = 0;
		virtual long getLong(int iSequnce) = 0;
		virtual double getDouble(int iSequnce) = 0;
		virtual string getString(int iSequnce) = 0;
		virtual bool getString(int iSequnce,char *r_value) = 0;
		virtual bool getDate(int iSequnce,void *r_value) = 0;
		//add end		
		virtual void setSessionID(int &i) =0;
		virtual int  getSessionID() =0;
		
		virtual const TableDef* getTableDef(const char* tableName)=0;
		virtual const IndexDef * getIndexDef(const char* tableName,const char* indexName)=0;
		virtual long getTableRowCount(const char* tableName){return 0;}
		
		// add by chenm 不等值查询类 2009-6-3 15:35:54
		virtual int selectRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算 
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
									,const int iRowCount=-1)									
				{return 0;}
									
		virtual int  deleteRecRange(const char *cTableName
										,const char *cIndexName
										,const InputParams *pGtParams      //查询条件中的下限索引字段值
										,const InputParams *pLtParams      //查询条件中的上限索引字段值
										,const int iRangeTag               // 标示[]/[)/()/(]运算
										,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
										,const InputParams *pOtherParams)										
				{return 0;}
		
		virtual int  updateRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算
									,vector<string> &sFieldNames       // 需要更新的字段名
									,const InputParams *pUpdateValues  // 需要更新的字段值
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams)   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		 		{return 0;}
		 
		// over 2009-6-3 15:36:00
		// add by chenm 2009/7/3 17:18:11
		virtual bool getAllIndexNameOfTable(const char* cTableName,vector<string>& indexNameList){return true;}
		// over 2009/7/3 17:18:16
        //Added By Sunam 2009/8/31 21:37:17
	    //virtual void dataBind(vector<int> &errVect) = 0;
		//Added By Sunam 2009/8/31 21:37:17存放处理出错的记录index
		//vector<int> *m_errVect;

};

#endif //SESSION_H_INCLUDED
