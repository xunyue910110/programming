#ifndef _BaseTable_H_
#define _BaseTable_H_


#include "RecordConvert.h"
#include "MemManager.h"

class IndexP_Type
{
    public:
        Index* m_pIndex;
        T_INDEXTYPE m_indexType;
    public:
        IndexP_Type()
        {
            m_pIndex = NULL;
        }
        ~IndexP_Type()
        {
            ;
        }
};

typedef map<MyString,IndexP_Type> INDEX_POOL;
typedef INDEX_POOL::iterator INDEX_POOL_ITR;
/**
* BaseTable:表基础类,提供普通表和锁表所需要的基本操作功能。
* 	BaseTable:			构造函数
*	~BaseTable:			析构函数
*	initialize:			表初始化函数
*	create:				表创建函数
*	drop:				表删除函数
*	truncate:			表清空函数
*	createIndex:		表索引创建函数
*	dropIndex:			表索引删除函数
*	getRecordConvert:	获取表记录信息转换类实例的指针
*	isEmpty:			判断表是否为空函数
*   cleanTransDatas:	根据transDatas清空其中对应的内存信息
*/
class BaseTable
{
	public:
		
		/**
		* BaseTable:构造函数
		* @param	tableName	表名称
		* @param	memManager	数据库内存管理实例的指针
		* @return
		*/
		BaseTable(const char* tableName, MemManager* memManager);
		
		/**
		* ~BaseTable:析构函数
		* @param
		* @return
		*/
		virtual ~BaseTable();
		
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
		
		/**
		* getRecordConvert:获取表记录信息转换类实例的指针
		* @param
		* @return
		*/
		RecordConvert* getRecordConvert();
		
		/**
		* getTableName:获取当前表的表名称
		* @param
		* @return 表名称
		*/
		char* getTableName();
		
		/**
		* isEmpty:判断表是否为空函数
		* @param
		* @return
		*/
		bool isEmpty();

		/**
		* cleanTransDatas:根据transDatas清空其中对应的内存信息
		* @param	transDatas		传送数据记录集
		* @return
		*/
		void cleanTransDatas(vector<TransData *>& transDatas);
		
		/**
		* setRowCount:设置select * from table的返回的记录数,可不调用,默认全选
		* @param	iRowCount		返回的记录数
		* @return
		*/
		void setRowCount(int iRowCount)
		{
			m_iRowCount = iRowCount;
		}
		
	protected:
		/**
		* select:根据索引名称＋索引字段值，获取记录信息
		* @param	indexName			索引名称
		* @param	value				索引值
		* @return	vector<void *>		记录结果集
		*/
		void select(const char* indexName, const InputParams *pIndexKeyParams, vector<void *>& recordVector);

		/**
		* select:根据索引名称＋索引字段值，获取记录信息
		* @param	indexName			索引名称
		* @param	value				索引值
		* @return	vector<TransData>	记录结果集
		*/
		void select(const char* indexName, const InputParams *pIndexKeyParams, vector<TransData *>& transDatas);

		
		/**
		* select:根据索引名称＋索引字段值，获取记录信息  留给lockManager使用
		* @param	indexName			索引名称
		* @param	value				索引值
		* @return	vector<TransData>	记录结果集
		*/
		void select(const char* indexName, const void* value, vector<TransData *>& transDatas);


		/** add by chenm 2009-6-3 16:21:06
		* select:根据索引名称＋索引字段值，获取记录信息
		* @param	indexName			索引名称
		* @param	value				索引值
		* @return	vector<void *>		记录结果集
		*/
		void selectRange(const char* indexName
							, const InputParams *pGtParams  //查询条件中的下限索引字段值    
							, const InputParams *pLtParams  //查询条件中的上限索引字段值
							, const int iRangeTag
							, vector<void *>& recordVector);

		/** add by chenm 2009-6-3 16:21:12
		* select:根据索引名称＋索引字段值，获取记录信息
		* @param	indexName			索引名称
		* @param	value				索引值
		* @return	vector<TransData>	记录结果集
		*/
		void selectRange(const char* indexName
							, const InputParams *pGtParams  //查询条件中的下限索引字段值    
							, const InputParams *pLtParams  //查询条件中的上限索引字段值
							, const int iRangeTag
							, vector<TransData *>& transDatas);
		
		
		/**
		* getIndexByName:根据Index的名字获取Index指针
		* @param
		* @return Index指针
		*/
		Index* getIndexByName(const char* indexName);
		
		/**
		* prepareAllIndex:预先准备表下面全部Index类(实例化+初始化)
		* @param
		* @return
		*/
		void prepareAllIndex();
		
		/**
		* deleteRec:根据transDatas删除记录
		* @param	transDatas			记录信息集
		* @return
		*/
		void deleteRec(const vector<TransData *>& transDatas);

		/**
		* insert:根据transDatas插入记录
		* @param	transDatas			记录信息集
		* @return
		*/
		void insert(vector<TransData *>& transDatas);
		
		// add by chenm 2008-5-26 插入单条记录 主要给LockManager使用 
		/**
		* insert:根据pTransData插入记录
		* @param	pTransData			记录信息
		* @return
		*/
		void insert(TransData * pTransData);

		
		/**
		* update:根据transDatas更新记录
		* @param	transDatas			记录信息集
		* @return
		*/
		void update(const vector<TransData *>& transDatas);

		/**
		* update:根据pTransData更新记录,不涉及索引字段的更新
		* @param	transDatas			记录信息集
		* @return
		*/
		void update(const TransData * pTransData);			
	private:

		/**
		* deleteAllIndex:根据一条记录,删除其所有索引(type:0-普通删除 1-插入反向操作,恢复性删除)
		* @param	transData			记录信息
		* @param	type				操作方式(0-普通删除 1-插入反向操作,恢复性删除)
		* @return
		*/
		void deleteAllIndex(const TransData& transData, unsigned char type);

		/**
		* insertAllIndex:根据一条记录,插入其所有索引(type:0-普通插入 1-删除反向操作,恢复性插入)
		* @param	transData			记录信息
		* @param	type				操作方式(0-普通插入 1-删除反向操作,恢复性插入)
		* @return
		*/
		void insertAllIndex(const TransData& transData, unsigned char type);

		/**
		* updateAllIndex:根据一条记录,更新其所有索引(type:0-普通更新 1-更新反向操作,恢复性更新)
		* @param	transData			记录信息
		* @param	type				操作方式(0-普通更新 1-更新反向操作,恢复性更新)
		* @return
		*/
		void updateAllIndex(const TransData& transData, unsigned char type);

		/**
		* undoIndexChange:根据索引操作日志进行反向操作
		* @param	indexChangeLogs		索引修改日志信息
		* @return
		*/
		void undoIndexChange(vector<IndexChangeLog*>& indexChangeLogs);


		/**
		* cleanIndexChangeLogs:根据索引操作日志进行内存清理操作
		* @param	indexChangeLogs		索引修改日志信息
		* @return
		*/		
		void cleanIndexChangeLogs(vector<IndexChangeLog*>& indexChangeLogs);
		
	public:
		TableDesc			*m_tableDesc;						//表描述指针:指向表描述信息内存地址
		
	protected:
		T_NAMEDEF			m_tableName;						//表名称
		INDEX_POOL      	m_indexs;							//Map,存放表下属 索引名称-索引实例指针 对应关系
		RecordConvert		*m_recordConvert;					//记录转换指针：指向表记录转换类的内存地址
		MemManager			*m_memManager;						//内存管理指针：指向数据库内存管理类的内存地址
		int                 m_iRowCount;                        //限定select * from table时返回的记录数,防止大表全选,会撑死客户端。默认值"-1"即全选
};


#endif

