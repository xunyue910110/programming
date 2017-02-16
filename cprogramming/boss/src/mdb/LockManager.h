#ifndef _LockManager_H_
#define _LockManager_H_


#include "BaseTable.h"


/**
* LockManager:锁表类,提供普通表记录锁表所需要的功能。(字段:原表主键字段、SESSION_ID、LOCK_TYPE; 索引:IDX_表名_SES、IDX_表名_RES)
* 	LockManager:		锁表构造函数
*	~LockManager:		锁表析构函数
*	judgeAndAddLock:	判断资源是否锁定并且对资源加锁
*	releaseLock:		根据会话释放锁资源
*	initialize:			表初始化函数
*	create:				表创建函数
*	drop:				表删除函数
*	truncate:			表清空函数
*	createIndex:		表索引创建函数
*	dropIndex:			表索引删除函数
*/
class LockManager : public BaseTable
{
	public:
		
		/**
		* LockManager:锁表构造函数
		* @param	tableName	锁表表名称
		* @param	memManager	锁表数据库的内存分配类地址
		* @param	table		锁表归属表的实例的指针
		* @return
		*/
		LockManager(const char* tableName, MemManager* memManager, BaseTable* table) : BaseTable(tableName, memManager)
		{
			m_table = table;
			sprintf(m_indexNameSES, "IDX_%s_SES", m_tableName);
			sprintf(m_indexNameRES, "IDX_%s_RES", m_tableName);
		}
		
		/**
		* ~LockManager:锁表析构函数
		* @param
		* @return
		*/
		~LockManager() {}
		
		/**
		* judgeAndAddLock:判断记录资源是否锁定并且对资源加锁
		* @param	inTransData	原表记录
		* @param	sessionId	会话Id
		* @param	lockType	锁记录方式(0:使用原表记录里面的旧值, 1:使用原表记录里面的新值)
		* @return	0:无锁 1:有锁,同会话占用锁 2:有锁,同会话等待锁 3:有锁,不存在同会话锁,且没有死锁 4:有锁,不存在同会话锁,且有死锁
		*/
		int judgeAndAddLock(const TransData& inTransData, const int& sessionId, const unsigned char& lockType);
		
		/**
		* releaseLock:根据会话释放锁资源
		* @param	sessionId	会话Id
		* @return	
		*/
		void releaseLock(const int& sessionId);
		
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
	
	private:
		
		/**
		* detectDeadLock:判断该资源针对该会话加锁后是否存在死锁
		* @param	resValue	资源值
		* @param	resLen		资源长度
		* @param	sessionId	会话ID		
		* @return true:存在死锁 false:不存在死锁
		*/
		bool detectDeadLock(const void* resValue, const int& resLen, const int& sessionId);
		
		/**
		* signalFirstWaitSessionByResource:唤醒等待资源锁的第一个会话
		* @param	resValue	资源值
		* @return
		*/
		void signalFirstWaitSessionByResource(const void* resValue);
	
		/**
		* findOwnSessionByResource:根据资源ID查询占用该资源的会话
		* @param	resValue	资源值
		* @param	resLen		资源值长度
		* @param	sessionId	占用资源的会话Id
		* @return	true:有记录  false:无记录
		*/
		bool findOwnSessionByResource(const void* resValue, int& sessionId);
			
		/**
		* findWaitResourceBySession:根据会话Id查询该会话等待的资源ID
		* @param	sessionId	会话Id
		* @param	resValue	资源值
		* @param	resLen		资源值长度
		* @return	true:有记录  false:无记录
		*/
		bool findWaitResourceBySession(const int& sessionId, void* resValue, int& resLen);
	
	private:
		
		BaseTable*		m_table;	//归属的表实例指针
		vector<string>	m_columns;	//表除SESSION_ID、LOCK_TYPE外的列信息(资源字段)
		T_NAMEDEF       m_indexNameSES; // 索引SES的名称
		T_NAMEDEF       m_indexNameRES;
};


#endif

