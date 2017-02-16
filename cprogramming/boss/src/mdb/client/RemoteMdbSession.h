// ############################################
// Source file : RemoteMdbSession.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################


#ifndef REMOTEMDBSESSION1_H_HEADER_INCLUDED_B7B9DA1D
#define REMOTEMDBSESSION1_H_HEADER_INCLUDED_B7B9DA1D

#include <vector>
#include "config-all.h"

USING_NAMESPACE(std)

#include "Session.h"
#include "MdbConstDef.h"
#include "PublicInfo.h"
#include "TableDescript.h"

class InputParams;
class Socket;

const int MAX_CMD_LEN = 1024;
//##ModelId=481D6E4300DA
class RemoteMdbSession : public Session
{
	public:
		RemoteMdbSession(Socket *pSocket);
		virtual ~RemoteMdbSession();

		virtual bool createTable(const TableDef &theTableDef) ;
		virtual bool dropTable(const char * cTableName) ;
		virtual bool truncate(const char * cTableName) ;
				
		virtual bool createIndex(const IndexDef &theIndexDef) ;
		virtual bool dropIndex(const char * cTableName,const char * cIndexName) ;
		
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
						,vector<string> &sFieldNames            // 需要更新的字段名
						,const vector<InputParams *> pUpdateValues          // 需要更新的字段值
						,const char *cExpr             //查询表达式,如：(msisdn=:1 and startTime <= :2)
						,const vector<InputParams *> pOtherParams);
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
		virtual bool getAllIndexNameOfTable(const char* cTableName,vector<string>& indexNameList);
		//Added By Sunam 2009/8/31 21:37:17
		//virtual void dataBind(vector<int> &errVect);
	
	private:
		void excuteCmd(const char * cmd,int iSendSize);
		void clearQueryResults();
		void clearColumnValue();
		void clearIndex2TableMap();
	
	private:
		Socket       *m_pWorkSocket;
		
		// server端返回的整体信息
		char          m_recLine[MAX_TRANSMISSION_BUFFER];
		unsigned long m_iRecSize;

		// 解析server端返回信息后的 记录集信息
		void**        m_pRecords;
		int           m_iRecordPositions[MAX_TRANSMISSION_BUFFER/128];// 按一条记录128个字节 估算
		int           m_iResultSequnce;  
		int           m_iResultsCount;
		
		// 解析一条记录后的 字段集信息
		void**        m_pColumnValues;
		// add by chenm 2009-7-9 15:46:10 记录server传过来的字段值的长度
		int *         m_iValuesLenth;
		// over 2009-7-9 15:46:57
		int           m_iColumnPositions[MAX_COLUMN_NUM];
		int           m_iColumnCount;
		
		// 查询表名
		MyString             m_queryTableName;
		TABLE_DEF_MAP        m_tableDefs;
		TABLE_COLOMN_POS_MAP m_tableColumnMap;
		INDEX_DEF_MAP        m_indexDefs;
		
		// add by chenm 2009/7/3 16:10:08
		typedef map<MyString,vector<string> *> INDEX2TABLE_MAP;
		typedef INDEX2TABLE_MAP::iterator INDEX2TABLE_MAP_ITR;
		INDEX2TABLE_MAP      m_index2TableMap;
		
		// add by chenm 2009-7-17 15:03:27
		bool                 m_isRowByRowSelect;  
};

#endif /* REMOTEMDBSESSION1_H_HEADER_INCLUDED_B7B9DA1D */
