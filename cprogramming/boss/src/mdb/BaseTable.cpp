#include "BaseTable.h"
#include "Index.h"
#include "InstanceFactory.h"

BaseTable::BaseTable(const char* tableName, MemManager* memManager)
{
	strcpy(m_tableName, tableName);
	m_memManager = memManager;
	
	m_tableDesc = NULL;
	m_indexs.clear();
	m_recordConvert = NULL;
	m_iRowCount = -1; // 默认select * from table时,全表返回
}


BaseTable::~BaseTable()
{
	strcpy(m_tableName, "\0");
	m_memManager = NULL;
	m_tableDesc = NULL;
	
	for (INDEX_POOL_ITR iter=m_indexs.begin(); iter!=m_indexs.end(); ++iter)
	{
		delete (iter->second).m_pIndex;
	} 
	m_indexs.clear();
	
	if (m_recordConvert != NULL)
	{
		delete m_recordConvert;
		m_recordConvert = NULL;	
	}
}


void BaseTable::initialize()
{
	m_memManager->getTableDescByName(m_tableName, m_tableDesc);
	
	m_recordConvert = new RecordConvert(m_tableDesc);
}


void BaseTable::create(const TableDef& tableDef)
{
	m_memManager->createTable(tableDef, m_tableDesc);

	m_tableDesc->m_recordNum = 0;
	
	m_recordConvert = new RecordConvert(m_tableDesc);
}


void BaseTable::drop()
{
	for (int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		IndexDesc *indexDesc = NULL;
		m_memManager->getIndexDescByName(m_tableDesc->m_indexNameList[i], indexDesc);
		Index *tmpIndex = InstanceFactory::getIndexInstance(((indexDesc->m_indexDef).m_indexType), m_tableName, m_tableDesc->m_indexNameList[i], m_memManager, m_recordConvert);
		try
		{
			tmpIndex->initialization();
			tmpIndex->drop();
		}
		catch(Mdb_Exception &e)
		{
			if (tmpIndex != NULL)
			{
				delete tmpIndex;
			}
			throw e;	
		}
			
		if (tmpIndex != NULL)
		{
			delete tmpIndex;
		}
	}
	
	m_tableDesc->m_recordNum = 0;
	m_memManager->dropTable(m_tableName);
}


void BaseTable::truncate()
{
	for (int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		IndexDesc *indexDesc = NULL;
		m_memManager->getIndexDescByName(m_tableDesc->m_indexNameList[i], indexDesc);
		Index *tmpIndex = InstanceFactory::getIndexInstance((indexDesc->m_indexDef).m_indexType, m_tableName, m_tableDesc->m_indexNameList[i], m_memManager, m_recordConvert);
		try
		{
			tmpIndex->initialization();
			tmpIndex->truncate();
		}
		catch(Mdb_Exception &e)
		{
			if (tmpIndex != NULL)
			{
				delete tmpIndex;
			}
			throw e;	
		}
			
		if (tmpIndex != NULL)
		{
			delete tmpIndex;
		}
	}
	
	m_tableDesc->m_recordNum = 0;
	m_memManager->truncateTable(m_tableName);
}


void BaseTable::createIndex(const IndexDef& indexDef)
{
	//考虑索引删除后又重建的情况.先查询,如果查询到在原来类基础上重建;如果查询不到新建.
	Index *tmpIndex = InstanceFactory::getIndexInstance(indexDef.m_indexType, m_tableName, indexDef.m_indexName, m_memManager, m_recordConvert);
	try
	{
		tmpIndex->create(indexDef);
	}
	catch(Mdb_Exception &e)
	{
		if (tmpIndex != NULL)
		{
			delete tmpIndex;
		}	
		throw e;
	}	
	
	if (tmpIndex != NULL)
	{
		delete tmpIndex;
	}
}


void BaseTable::dropIndex(const char* indexName)
{
	IndexDesc *indexDesc = NULL;
	m_memManager->getIndexDescByName(indexName, indexDesc);
	Index *tmpIndex = InstanceFactory::getIndexInstance((indexDesc->m_indexDef).m_indexType, m_tableName, indexName, m_memManager, m_recordConvert);
	try
	{
		tmpIndex->initialization();
		tmpIndex->drop();
	}
	catch(Mdb_Exception &e)
	{
		if (tmpIndex != NULL)
		{
			delete tmpIndex;
		}	
		throw e;
	}
		
	if (tmpIndex != NULL)
	{
		delete tmpIndex;
	}
}


RecordConvert* BaseTable::getRecordConvert()
{
	return m_recordConvert;
}


char* BaseTable::getTableName()
{

	return m_tableName;
}


bool BaseTable::isEmpty()
{
	return (m_tableDesc->m_recordNum == 0);
}


void BaseTable::cleanTransDatas(vector<TransData *>& transDatas)
{
	for (int i=0; i<transDatas.size(); ++i)
	{
		if( transDatas[i] != NULL)
		{
			delete transDatas[i] ;
			transDatas[i] = NULL;
		}
	}
	transDatas.clear();
}


void BaseTable::select(const char* indexName, const InputParams *pIndexKeyParams , vector<void *>& recordVector)
{
	recordVector.clear();
	
	//寻找索引
	Index *tmpIndex;
//	try
//	{
		tmpIndex = getIndexByName(indexName);
	
		//根据索引查询记录
		vector<MdbAddress> addrs;
		addrs.clear();
		tmpIndex->select(pIndexKeyParams, addrs, m_iRowCount);
		m_iRowCount = -1;
		for(vector<MdbAddress>::iterator iterv=addrs.begin(); iterv!=addrs.end(); ++iterv)
		{
			//分配的内存由外部调用者进行释放,比如Session
			void *record = malloc(m_tableDesc->m_pageInfo.m_slotSize);
			memcpy(record, iterv->m_addr, m_tableDesc->m_pageInfo.m_slotSize);
			
			recordVector.push_back(record);
		}
//	}
//	catch(Mdb_Exception &e)
//	{
//		throw e;
//	}
}

void BaseTable::select(const char* indexName, const InputParams *pIndexKeyParams, vector<TransData *>& transDatas)
{	
	transDatas.clear();
		
	//寻找索引
	Index *tmpIndex;
	tmpIndex = getIndexByName(indexName);

	//根据索引查询记录
	vector<MdbAddress> addrs;
	addrs.clear();
	tmpIndex->select(pIndexKeyParams, addrs,m_iRowCount);
	m_iRowCount = -1;
	for(vector<MdbAddress>::iterator iterv=addrs.begin(); iterv!=addrs.end(); ++iterv)
	{
		TransData *pTransData;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData = new TransData();
		
		pTransData->m_addr = *iterv;
		pTransData->m_recLength = m_tableDesc->m_pageInfo.m_slotSize;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData->m_oldRecordValue = malloc(m_tableDesc->m_pageInfo.m_slotSize);
		memcpy(pTransData->m_oldRecordValue, iterv->m_addr, m_tableDesc->m_pageInfo.m_slotSize);
		// 构造的时候已经作了下面的工作
		// pTransData->m_newRecordValue = NULL;
		// pTransData->m_operType = '3';
		
		transDatas.push_back(pTransData);
	}
}
		
void BaseTable::select(const char* indexName, const void* value, vector<TransData *>& transDatas)
{	
	transDatas.clear();
		
	//寻找索引
	Index *tmpIndex;
	tmpIndex = getIndexByName(indexName);

	//根据索引查询记录
	vector<MdbAddress> addrs;
	addrs.clear();
	tmpIndex->select((char *)value, addrs,m_iRowCount);
	m_iRowCount = -1;
	for(vector<MdbAddress>::iterator iterv=addrs.begin(); iterv!=addrs.end(); ++iterv)
	{
		TransData *pTransData;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData = new TransData();
		
		pTransData->m_addr = *iterv;
		pTransData->m_recLength = m_tableDesc->m_pageInfo.m_slotSize;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData->m_oldRecordValue = malloc(m_tableDesc->m_pageInfo.m_slotSize);
		memcpy(pTransData->m_oldRecordValue, iterv->m_addr, m_tableDesc->m_pageInfo.m_slotSize);
		// 构造的时候已经作了下面的工作
		//pTransData->m_newRecordValue = NULL;
		//pTransData->m_operType = '3';
		
		transDatas.push_back(pTransData);
	}
}


Index* BaseTable::getIndexByName(const char* indexName)
{
    MyString s;
    T_INDEXTYPE indexType;
    
    strcpy(s.m_pStr,indexName);
    INDEX_POOL_ITR itr= m_indexs.find(s);

    if(itr!=m_indexs.end())
    {
        indexType = (((itr->second).m_pIndex)->getIndexDesc()->m_indexDef).m_indexType;
        if((itr->second).m_indexType == indexType)
        {
            return (itr->second).m_pIndex;
        }
        else
        {
            //类型不对，重新获取
            delete (itr->second).m_pIndex;
            (itr->second).m_pIndex = InstanceFactory::getIndexInstance(indexType, m_tableName, indexName, m_memManager, m_recordConvert);;
            (itr->second).m_pIndex->initialization();
            return (itr->second).m_pIndex;
        }
    }
    else
    {
        IndexDesc *indexDesc = NULL;
		m_memManager->getIndexDescByName(indexName, indexDesc);
		indexType = (indexDesc->m_indexDef).m_indexType;

        Index *tmpIndex = InstanceFactory::getIndexInstance(indexType, m_tableName, indexName, m_memManager, m_recordConvert);
        IndexP_Type indexP_Type;
        indexP_Type.m_pIndex = tmpIndex;
        indexP_Type.m_indexType = indexType;
        m_indexs.insert(INDEX_POOL::value_type(s, indexP_Type));
        tmpIndex->initialization();
        return tmpIndex;
    }
}


void BaseTable::prepareAllIndex()
{
	for(int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		getIndexByName(m_tableDesc->m_indexNameList[i]);
	}
}


void BaseTable::deleteRec(const vector<TransData *>& transDatas)
{	
	//(1)删除索引
	vector<MdbAddress> addrs;
	addrs.clear();
	for (int i=0; i<transDatas.size(); ++i)
	{
		//删除记录占用的索引,错误时恢复性插入
		try
		{
FP_BEGIN(del_index)
			deleteAllIndex(*(transDatas[i]), 0);
FP_END(del_index)
		}
		catch(Mdb_Exception &e1)
		{
			for (int j=i-1; j>=0; --j)
			{
				insertAllIndex(*(transDatas[j]), 1);
			}

			throw e1;
		}
		
		//记录需要删除记录的记录地址
		//addrs.push_back(transDatas[i]->m_addr);
	}
			
	//(2)删除记录;记录如果删除不成功,恢复这些记录索引
	int iCount;
	try
	{
FP_BEGIN(del_free)
		for(iCount=0;iCount<transDatas.size(); ++iCount)	
			m_memManager->freeTableMem(m_tableDesc, transDatas[iCount]->m_addr);
FP_END(del_free)
	}
	catch(Mdb_Exception &e2)
	{
		//for (int i=transDatas.size()-1; i>=0; --i)
		for (int i=transDatas.size()-1; i>=iCount; --i)
		{
			insertAllIndex(*(transDatas[i]), 1);
		}
		throw e2;
	}

	//(3)表记录数更新
	m_tableDesc->m_recordNum -= transDatas.size();
}


void BaseTable::insert(vector<TransData *>& transDatas)
{		
	//(1)分配内存并且插入记录
	vector<MdbAddress> addrs;
	addrs.clear();
	m_memManager->allocateTableMem(m_tableDesc, transDatas.size(), addrs);
	for (int i=0; i<transDatas.size(); ++i)
	{
		transDatas[i]->m_addr = addrs[i];
		memcpy(addrs[i].m_addr, transDatas[i]->m_newRecordValue, m_tableDesc->m_pageInfo.m_slotSize);
	}
	

	//(2)插入索引;插入记录对应的索引信息,如果插入失败,进行恢复性删除动作.
	for (int i=0; i<transDatas.size(); ++i)
	{
		try
		{
			insertAllIndex(*(transDatas[i]), 0);
		}
		catch(Mdb_Exception &e1)
		{
			m_memManager->freeTableMem(m_tableDesc, addrs);
			for (int j=i-1; j>=0; --j)
			{
				deleteAllIndex(*(transDatas[j]), 1);
			}
			throw e1;
		}
	}
	
	//(3)表记录数更新
	m_tableDesc->m_recordNum += transDatas.size();
}

// add by chenm 2008-5-26 插入单条记录 主要给LockManager使用 
void BaseTable::insert(TransData * pTransData)
{		
	//(1)分配内存并且插入记录
	vector<MdbAddress> addrs;
	addrs.clear();
	m_memManager->allocateTableMem(m_tableDesc, 1, addrs);

	pTransData->m_addr = addrs[0];
	memcpy(addrs[0].m_addr, pTransData->m_newRecordValue, m_tableDesc->m_pageInfo.m_slotSize);

	//(2)插入索引;插入记录对应的索引信息,如果插入失败,进行恢复性删除动作.
	try
	{
		insertAllIndex(*pTransData, 0);
	}
	catch(Mdb_Exception &e1)
	{
		m_memManager->freeTableMem(m_tableDesc, addrs);
		deleteAllIndex(*pTransData, 1);
		throw e1;
	}
	
	//(3)表记录数更新
	m_tableDesc->m_recordNum += 1;
}

void BaseTable::update(const vector<TransData *>& transDatas)
{	
	//(1)更新索引;更新记录对应的索引信息,如果更新失败,进行恢复性更新动作.
	for (int i=0; i<transDatas.size(); ++i)
	{
		try
		{
			updateAllIndex(*(transDatas[i]), 0);
		}
		catch(Mdb_Exception &e1)
		{
			for (int j=i-1; j>=0; --j)
			{
				updateAllIndex(*(transDatas[j]), 1);
			}
			throw e1;
		}
	}
	
	//(2)更新数据内容
	for (int i=0; i<transDatas.size(); ++i)
	{
		memcpy(transDatas[i]->m_addr.m_addr, transDatas[i]->m_newRecordValue, m_tableDesc->m_pageInfo.m_slotSize);
	}
}

// add by chenm 2008-6-24 15:08:44 
// 不涉及到索引的更新,目前只给lockManager使用
void BaseTable::update(const TransData * pTransData)
{	
		memcpy(pTransData->m_addr.m_addr, pTransData->m_newRecordValue, m_tableDesc->m_pageInfo.m_slotSize);
}

void BaseTable::deleteAllIndex(const TransData& transData, unsigned char type)
{
	//(0)定义索引操作日志结构
	vector<IndexChangeLog *>	indexChangeLogs;
	indexChangeLogs.clear();
	
	for(int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		try
		{
			//(1)寻找对应的Index类
			Index *tmpIndex = NULL;
			tmpIndex = getIndexByName(m_tableDesc->m_indexNameList[i]);
			
			//(2)获取对应的Index字段名称  
			IndexDesc *indexDesc = tmpIndex->getIndexDesc();
			if (type == 0)
			{
				m_recordConvert->setRecordValue(transData.m_oldRecordValue);
			}
			else
			{
				m_recordConvert->setRecordValue(transData.m_newRecordValue);
			}
			int iColumnLenth,iOffSize = 0;
			char *value = new char[MAX_COLUMN_LEN];
			char columnValue[MAX_COLUMN_LEN];
			for(int i=0;i<(indexDesc->m_indexDef).m_columnNum;++i)
			{
				iColumnLenth = 0;
				memset(columnValue,0,sizeof(columnValue));
				m_recordConvert->getColumnValue( (indexDesc->m_indexDef).m_columnList[i]
				                                  ,columnValue,iColumnLenth);
				memcpy(value+iOffSize,columnValue,iColumnLenth);
				iOffSize += iColumnLenth;
			}
		
			//(4)删除索引信息,并记录删除日志
		FP_BEGIN(deleteIdx)
			tmpIndex->deleteIdx(value, transData.m_addr);
		FP_END(deleteIdx)
			IndexChangeLog *indexChangeLog;
			indexChangeLog = new IndexChangeLog();
			indexChangeLog->m_undoFlag = 0;
			indexChangeLog->m_operType = 0;
			indexChangeLog->m_index = tmpIndex;
			indexChangeLog->m_addr = transData.m_addr;
			indexChangeLog->m_oldValue = value;
			indexChangeLog->m_newValue = NULL;
			indexChangeLogs.push_back(indexChangeLog);
		}
		catch(Mdb_Exception &e1)
		{
			try
			{
				undoIndexChange(indexChangeLogs);
			}
			catch(Mdb_Exception &e2)
			{
				cleanIndexChangeLogs(indexChangeLogs);
				throw e2;
			}
				
			cleanIndexChangeLogs(indexChangeLogs);
			throw e1;
		}
	}
	
	//(5)根据索引操作日志结构,删除动态分配的内存
	cleanIndexChangeLogs(indexChangeLogs);
}	


void BaseTable::insertAllIndex(const TransData& transData, unsigned char type)
{
	//(0)定义索引操作日志结构
	vector<IndexChangeLog *>	indexChangeLogs;
	indexChangeLogs.clear();
	
	for(int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		try
		{
			//(1)寻找对应的Index类
			Index *tmpIndex = NULL;
			tmpIndex = getIndexByName(m_tableDesc->m_indexNameList[i]);

			//(2)获取对应的Index字段名称
			IndexDesc *indexDesc = tmpIndex->getIndexDesc();
			//(3)获取字段对应的值内容(type:0-插入,使用新值 1-恢复性插入,使用原值)
			if (type == 0)
			{
				m_recordConvert->setRecordValue(transData.m_newRecordValue);
			}
			else
			{
				m_recordConvert->setRecordValue(transData.m_oldRecordValue);
			}
			int iColumnLenth,iOffSize = 0;
			char *value = new char[MAX_COLUMN_LEN];
			char columnValue[MAX_COLUMN_LEN];
			for(int j=0;j<(indexDesc->m_indexDef).m_columnNum;++j)
			{
				iColumnLenth = 0;
				memset(columnValue,0,sizeof(columnValue));
				m_recordConvert->getColumnValue( (indexDesc->m_indexDef).m_columnList[j]
				                                  ,columnValue,iColumnLenth);
				memcpy(value+iOffSize,columnValue,iColumnLenth);
				iOffSize += iColumnLenth;
			}
			//(4)插入索引信息
			tmpIndex->insert(value, transData.m_addr);

			IndexChangeLog *indexChangeLog;
			indexChangeLog = new IndexChangeLog();
			indexChangeLog->m_undoFlag = 0;
			indexChangeLog->m_operType = 1;
			indexChangeLog->m_index = tmpIndex;
			indexChangeLog->m_addr = transData.m_addr;
			indexChangeLog->m_oldValue = NULL;
			indexChangeLog->m_newValue = value;
			indexChangeLogs.push_back(indexChangeLog);
		}
		catch(Mdb_Exception &e1)
		{
			try
			{
				undoIndexChange(indexChangeLogs);
			}
			catch(Mdb_Exception &e2)
			{
				cleanIndexChangeLogs(indexChangeLogs);
				throw e2;
			}
				
			cleanIndexChangeLogs(indexChangeLogs);
			throw e1;
		}
	}
	//(5)根据索引操作日志结构,删除动态分配的内存
	cleanIndexChangeLogs(indexChangeLogs);
}


void BaseTable::updateAllIndex(const TransData& transData, unsigned char type)
{
	//(0)定义索引操作日志结构
	vector<IndexChangeLog *>	indexChangeLogs;
	indexChangeLogs.clear();

	
	for(int i=0; i<m_tableDesc->m_indexNum; ++i)
	{
		try
		{
			//(1)寻找对应的Index类
			Index *tmpIndex = NULL;
			tmpIndex = getIndexByName(m_tableDesc->m_indexNameList[i]);
						
			//(2)获取对应的Index字段名称  disabled by chenm 2008-6-24 9:39:11
			IndexDesc *indexDesc = tmpIndex->getIndexDesc();
			
			int iColumnLenth,iOffSize = 0;
			char columnValue[MAX_COLUMN_LEN];

			char *oldValue = new char[MAX_COLUMN_LEN];
			m_recordConvert->setRecordValue(transData.m_oldRecordValue);
			for(int i=0;i<(indexDesc->m_indexDef).m_columnNum;++i)
			{
				iColumnLenth = 0;
				memset(columnValue,0,sizeof(columnValue));
				m_recordConvert->getColumnValue( (indexDesc->m_indexDef).m_columnList[i]
				                                  ,columnValue,iColumnLenth);
				memcpy(oldValue+iOffSize,columnValue,iColumnLenth);
				iOffSize += iColumnLenth;
			}
			char *newValue = new char[MAX_COLUMN_LEN];
			m_recordConvert->setRecordValue(transData.m_newRecordValue);
			iOffSize = 0;
			for(int i=0;i<(indexDesc->m_indexDef).m_columnNum;++i)
			{
				iColumnLenth = 0;
				memset(columnValue,0,sizeof(columnValue));
				m_recordConvert->getColumnValue( (indexDesc->m_indexDef).m_columnList[i]
				                                  ,columnValue,iColumnLenth);
				memcpy(newValue+iOffSize,columnValue,iColumnLenth);
				iOffSize += iColumnLenth;
			}
			
			if (memcmp(oldValue, newValue, iOffSize) == 0)
			{
				delete[] oldValue;
				delete[] newValue;
				continue;
			}
		
			//(4)更新索引信息(type:0-更新 1-恢复性更新)
			//删除
			IndexChangeLog *indexChangeLog;
			indexChangeLog = new IndexChangeLog();
			if (type == 0)
			{
				tmpIndex->deleteIdx(oldValue, transData.m_addr);
				indexChangeLog->m_oldValue = oldValue;
			}
			else
			{
				tmpIndex->deleteIdx(newValue, transData.m_addr);
				indexChangeLog->m_oldValue = newValue;
			}
			indexChangeLog->m_undoFlag = 0;
			indexChangeLog->m_operType = 0;
			indexChangeLog->m_index = tmpIndex;
			indexChangeLog->m_addr = transData.m_addr;
			indexChangeLog->m_newValue = NULL;
				
			indexChangeLogs.push_back(indexChangeLog);
			
			//插入
			indexChangeLog = new IndexChangeLog();
			if (type == 0)
			{
				tmpIndex->insert(newValue, transData.m_addr);
				
				indexChangeLog->m_newValue = newValue;
			}
			else
			{
				tmpIndex->insert(oldValue, transData.m_addr);
				
				indexChangeLog->m_newValue = oldValue;
			}
			
			indexChangeLog->m_undoFlag = 0;
			indexChangeLog->m_operType = 1;
			indexChangeLog->m_index = tmpIndex;
			indexChangeLog->m_addr = transData.m_addr;
			indexChangeLog->m_oldValue = NULL;
				
			indexChangeLogs.push_back(indexChangeLog);
		}
		catch(Mdb_Exception &e1)
		{
			try
			{
				undoIndexChange(indexChangeLogs);
			}
			catch(Mdb_Exception &e2)
			{
				cleanIndexChangeLogs(indexChangeLogs);
				throw e2;
			}
				
			cleanIndexChangeLogs(indexChangeLogs);
			throw e1;
		}
	}
	
	//(5)根据索引操作日志结构,删除动态分配的内存
	cleanIndexChangeLogs(indexChangeLogs);
}


void BaseTable::undoIndexChange(vector<IndexChangeLog *>& indexChangeLogs)
{
	//for (int i=indexChangeLogs.size()-1; i>=0; i++)
	for (int i=indexChangeLogs.size()-1; i>=0; --i)
	{
		if (indexChangeLogs[i]->m_undoFlag == 0)
		{
			//删除,执行反向插入操作
			if (indexChangeLogs[i]->m_operType == 0)
			{
				(indexChangeLogs[i]->m_index)->insert((char *)indexChangeLogs[i]->m_oldValue, indexChangeLogs[i]->m_addr);
			
				indexChangeLogs[i]->m_undoFlag = 1;
			}
			//插入,执行反向删除操作
			else
			{
				(indexChangeLogs[i]->m_index)->deleteIdx((char *)indexChangeLogs[i]->m_newValue, indexChangeLogs[i]->m_addr);
				
				indexChangeLogs[i]->m_undoFlag = 1;
			}
		}
	}
}


void BaseTable::cleanIndexChangeLogs(vector<IndexChangeLog *>& indexChangeLogs)
{
	for (int i=0; i<indexChangeLogs.size(); ++i)
	{
		if (indexChangeLogs[i]->m_oldValue != NULL)
		{
			free(indexChangeLogs[i]->m_oldValue);
		}
		if (indexChangeLogs[i]->m_newValue != NULL)
		{	
			free(indexChangeLogs[i]->m_newValue);
		}
		delete indexChangeLogs[i];
	}
	indexChangeLogs.clear();
}

// add by chenm 2009-6-3 16:24:36
void BaseTable::selectRange(const char* indexName
							, const InputParams *pGtParams  //查询条件中的下限索引字段值    
							, const InputParams *pLtParams  //查询条件中的上限索引字段值
							, const int iRangeTag
							, vector<void *>& recordVector)
{
	recordVector.clear();
	
	//寻找索引
	Index *tmpIndex;

	tmpIndex = getIndexByName(indexName);
	
	if( (tmpIndex->getIndexDesc())->m_indexDef.m_indexType != TREE_INDEX)
	{
		Mdb_Exception (__FILE__, __LINE__, "索引 [%s] 不支持不等值查找,请核实",indexName ); 	
	}
	
	//根据索引查询记录
	vector<MdbAddress> addrs;
	addrs.clear();
	tmpIndex->selectRange(pGtParams,pLtParams,iRangeTag, addrs, m_iRowCount);
	m_iRowCount = -1;
	for(vector<MdbAddress>::iterator iterv=addrs.begin(); iterv!=addrs.end(); ++iterv)
	{
		//分配的内存由外部调用者进行释放,比如Session
		void *record = malloc(m_tableDesc->m_pageInfo.m_slotSize);
		memcpy(record, iterv->m_addr, m_tableDesc->m_pageInfo.m_slotSize);
		
		recordVector.push_back(record);
	}
}

void BaseTable::selectRange(const char* indexName
							, const InputParams *pGtParams  //查询条件中的下限索引字段值    
							, const InputParams *pLtParams  //查询条件中的上限索引字段值
							, const int iRangeTag
							, vector<TransData *>& transDatas)
{
	transDatas.clear();
		
	//寻找索引
	Index *tmpIndex;
	tmpIndex = getIndexByName(indexName);

	if( (tmpIndex->getIndexDesc())->m_indexDef.m_indexType != TREE_INDEX)
	{
		Mdb_Exception (__FILE__, __LINE__, "索引 [%s] 不支持不等值查找,请核实",indexName ); 	
	}

	//根据索引查询记录
	vector<MdbAddress> addrs;
	addrs.clear();
	tmpIndex->selectRange(pGtParams,pLtParams,iRangeTag, addrs,m_iRowCount);
	m_iRowCount = -1;
	for(vector<MdbAddress>::iterator iterv=addrs.begin(); iterv!=addrs.end(); ++iterv)
	{
		TransData *pTransData;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData = new TransData();
		
		pTransData->m_addr = *iterv;
		pTransData->m_recLength = m_tableDesc->m_pageInfo.m_slotSize;
		//分配的内存由外部调用者进行释放,比如Session
		pTransData->m_oldRecordValue = malloc(m_tableDesc->m_pageInfo.m_slotSize);
		memcpy(pTransData->m_oldRecordValue, iterv->m_addr, m_tableDesc->m_pageInfo.m_slotSize);
		// 构造的时候已经作了下面的工作
		// pTransData->m_newRecordValue = NULL;
		// pTransData->m_operType = '3';
		
		transDatas.push_back(pTransData);
	}
		
}
// over 2009-6-3 16:25:21

