#ifndef _Table_H_
#define _Table_H_


#include "LockManager.h"
#include "DoubleLock.h"
#include "TransDataMgr.h"


/**
* Table:表类,提供普通表的功能。
* 	Table:				表构造函数
*	~Table:				表析构函数
*	select:				表普通查询函数
*	selectWithTrans:	表事务查询函数
*	rollback:			回滚函数
*	commit:				提交函数
*	initialize:			表初始化函数
*	create:				表创建函数
*	drop:				表删除函数
*	truncate:			表清空函数
*	createIndex:		表索引创建函数
*	dropIndex:			表索引删除函数
*/
class Table : public BaseTable
{
	public:
	
		/**
		* Table:表构造函数
		* @param	tableName	表名称
		* @param	memManager	数据库的内存分配类地址
		* @return
		*/
		Table(const char* dbName, const char* tableName, MemManager* memManager);
		
		/**
		* ~Table:表析构函数
		* @param
		* @return
		*/
		~Table();

		/**
		* beginTrans:表事务开启
		* @param lockType 0表锁,1行锁
		* @return	
		*/
		void beginTrans(int lockType = 1);
		
		/**
		* select:表普通查询函数
		* @param	indexName		查询使用的表索引
		* @param	value			查询的表索引值
		* @param	recordVector	符合条件的记录集
		* @return	符合条件的记录数
		*/
		int select(const char* indexName, const InputParams *pIndexKeyParams, vector<void *>& recordVector);		
		
		/**
		* selectWithTrans:表事务查询函数(正常事务查询、删除、更新使用)
		* @param	sessionId	会话ID
		* @param	indexName	查询使用的表索引
		* @param	value		查询的表索引值
		* @param	transDatas	符合条件的记录集
		* @return	符合条件的记录数
		*/
		int selectWithTrans(int sessionId, const char* indexName, const InputParams *pIndexKeyParams, vector<TransData *>& transDatas);
		
		/**  add by chenm 2009-6-3 处理逻辑和select类似,只是调用BaseTable类的select函数不同
		* select:表普通查询函数
		* @param	indexName		查询使用的表索引
		* @param	value			查询的表索引值
		* @param	recordVector	符合条件的记录集
		* @return	符合条件的记录数
		*/
		int selectRange(const char* indexName
							, const InputParams *pGtParams   //查询条件中的下限索引字段值   
							, const InputParams *pLtParams   //查询条件中的上限索引字段值
							, const int iRangeTag
							, vector<void *>& recordVector);		
		
		/** add by chenm 2009-6-3 处理逻辑和select类似,只是调用BaseTable类的select函数不同
		* selectWithTrans:表事务查询函数(正常事务查询、删除、更新使用)
		* @param	sessionId	会话ID
		* @param	indexName	查询使用的表索引
		* @param	value		查询的表索引值
		* @param	transDatas	符合条件的记录集
		* @return	符合条件的记录数
		*/
		int selectRangeWithTrans(int sessionId
									, const char* indexName
									, const InputParams *pGtParams  //查询条件中的下限索引字段值    
									, const InputParams *pLtParams  //查询条件中的上限索引字段值
									, const int iRangeTag
									, vector<TransData *>& transDatas);

		/**
		* rollback:回滚函数
		* @param	sessionId	会话标识
		* @return
		*/
		void rollback(const int& sessionId);
		
		/**
		* commit:提交函数
		* @param	sessionId	会话标识
		* @param	transDatas	提交的记录集
		* @return
		*/
		void commit(const int& sessionId, const TRRANSDATA_VECTOR& transDatas);
		
		/**
		* initialize:表初始化函数
		* @param
		* @return
		*/
		virtual void initialize();
		
		/**
		* create:表创建函数
		* @param	tableDef	表的定义描述
		* @return
		*/
		virtual void create(const TableDef& tableDef);
		
		/**
		* drop:表删除函数
		* @param
		* @return
		*/
		virtual void drop();
		
		/**
		* truncate:表清空函数
		* @param
		* @return
		*/
		virtual void truncate();
		
		/**
		* createIndex:表索引创建函数
		* @param	indexDef	索引定义描述符
		* @return
		*/
		virtual void createIndex(const IndexDef& indexDef);
		
		/**
		* dropIndex:表索引删除函数
		* @param	indexName	索引名称
		* @return
		*/
		virtual void dropIndex(const char* indexName);
		//add by chenzm@2009-5-11
		//根据srcIndexName对destIndexName重新进行索引
		virtual void reIndex(const char* srcIndexName,const char* destIndexName);
		//add end	
			
	private:
		
		void waitMicroSeconds(int microSeconds);
		
	private:
		
		DoubleLock	*m_doubleLock;		//表信号灯
		LockManager	*m_lockManager;		//表锁表管理类
		int          m_lockType;        //0表锁,1行锁
		
		vector<TransData *> insertTransData;
    	vector<TransData *> deleteTransData;
    	vector<TransData *> updateTransData;

	    vector<TransData *> unInsertTransData;
	    vector<TransData *> unDeleteTransData;
	    vector<TransData *> unUpdateTransData;

};

#endif

