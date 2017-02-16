#include "LockManager.h"


int LockManager::judgeAndAddLock(const TransData& inTransData, const int& sessionId, const unsigned char& lockType)
{
	//(1)根据资源值获取等待和占用资源的所有会话;锁记录方式(0:使用原表记录里面的旧值, 1:使用原表记录里面的新值)
	char resValue[MAX_VALUE_LEN] = "\0";
	memset(resValue,0,MAX_VALUE_LEN);
	int resLen = 0;
	RecordConvert *recordConvert = m_table->getRecordConvert();
	//无主键,以相对地址作为锁定资源
	if ((m_table->m_tableDesc->m_tableDef).m_keyClumnNum == 0)
	{
		//旧记录
		if (inTransData.m_addr.m_pos.m_spaceCode != '\0')
		{
			resLen = sizeof(inTransData.m_addr.m_pos);
			memcpy(resValue, &inTransData.m_addr.m_pos, resLen);
		}
		//新记录,返回无锁
		else
		{
			return 0;
		}
	}
	//有主键,以主键作为锁定资源
	else
	{
		if (lockType == 0)
		{
			recordConvert->setRecordValue(inTransData.m_oldRecordValue);
		}
		else
		{
			recordConvert->setRecordValue(inTransData.m_newRecordValue);
		}
		recordConvert->getPkValue(resValue, resLen);
	}
	vector<TransData *> transDatas;
	try
	{
		select(m_indexNameRES, resValue, transDatas);
	}
	catch(Mdb_Exception &e)
	{
		cleanTransDatas(transDatas);
		throw e;
	}
	//(2)数据准备工作
	char maxValue[MAX_VALUE_LEN] = "\0";
	char lockRec[MAX_VALUE_LEN] = "\0";
	//下面部分可能引起内存泄露,上面写法可能引起core.(LINUX一定core,HP、IBM暂时没有,先用上面写法)
	//core原因:TransData的析构函数里,用free释放了m_newRecordValue指向的空间，而m_newRecordValue在lockmanager里，
	//         是指向一个char数组的，在虚拟机上free这个m_newRecordValue的时候，coredump了.
	//char *lockRec = (char *)malloc(MAX_VALUE_LEN);
	m_recordConvert->setRecordValue(lockRec);
	//无主键
	if ((m_table->m_tableDesc->m_tableDef).m_keyClumnNum == 0)
	{
		for(int i=0; i<m_columns.size(); ++i)
		{
			//m_recordConvert->setColumnValue(m_columns[i].c_str(), maxValue);
			m_recordConvert->setColumnValue(m_columns[i].c_str(), resValue);
		}
	}
	//有主键
	else
	{
		for(int i=0; i<m_columns.size(); ++i)
		{
			recordConvert->getColumnValue(m_columns[i].c_str(), maxValue);
			m_recordConvert->setColumnValue(m_columns[i].c_str(), maxValue);
		}
	}
	//(3)根据查询到的结果进行处理
	//(3.1)资源无锁,插入占用锁
	if (transDatas.size() == 0)
	{	
		int  recLen = 0;
		char tmplockTypeValue = '1';
		m_recordConvert->setColumnValue("SESSION_ID", &sessionId);
		m_recordConvert->setColumnValue("LOCK_TYPE", &tmplockTypeValue);
		
		TransData *pTransData;
		pTransData = new TransData();
		
		pTransData->m_recLength = m_tableDesc->m_pageInfo.m_slotSize;
		pTransData->m_oldRecordValue = NULL;
		m_recordConvert->getRecordValue(pTransData->m_newRecordValue, recLen);
		pTransData->m_operType = '1';
		
		try
		{
			insert(pTransData);
		}
		catch(Mdb_Exception &e)
		{
			delete pTransData;
			throw e;
		}
		delete pTransData;
		return 0;
	}
	//(3.2)资源有锁
	else
	{
		char lockTypeValue = '0';
		int	 sessionIdValue = -1;
		vector<TransData *>::iterator iter;
		for (iter=transDatas.begin(); iter!=transDatas.end(); ++iter)
		{
			m_recordConvert->setRecordValue((*iter)->m_oldRecordValue);
			m_recordConvert->getColumnValue("LOCK_TYPE", &lockTypeValue);
			m_recordConvert->getColumnValue("SESSION_ID", &sessionIdValue);
			if (sessionIdValue == sessionId)
			{
				break;
			}
		}
		
		//(3.2.1)锁定等待的信息中该session不存在
		if (iter == transDatas.end())
		{
			bool isDeadLock = true;
			try
			{
				isDeadLock = detectDeadLock(resValue, resLen, sessionId);
			}
			catch(Mdb_Exception &e)
			{
				cleanTransDatas(transDatas);
				throw e;
			}
			//不存在死锁,插入等待锁
			if (!isDeadLock)
			{
				int  recLen = 0;
				char tmplockTypeValue = '0';
				m_recordConvert->setRecordValue(lockRec);
				m_recordConvert->setColumnValue("SESSION_ID", &sessionId);
				m_recordConvert->setColumnValue("LOCK_TYPE", &tmplockTypeValue);
				//m_recordConvert->getRecordValue(maxValue, recLen);
				
				TransData *pTransData;
				pTransData = new TransData();
				
				pTransData->m_recLength = m_tableDesc->m_pageInfo.m_slotSize;
				pTransData->m_oldRecordValue = NULL;
				m_recordConvert->getRecordValue(pTransData->m_newRecordValue, recLen);
				pTransData->m_operType = '1';

				try
				{
					insert(pTransData);
				}
				catch(Mdb_Exception &e)
				{
					cleanTransDatas(transDatas);
					delete pTransData;
					throw e;
				}
				
				cleanTransDatas(transDatas);
				delete pTransData;
				return 3;
			}
			//存在死锁,该会话回滚
			else
			{
				try
				{
					releaseLock(sessionId);
				}
				catch(Mdb_Exception &e)
				{
					cleanTransDatas(transDatas);
					throw e;
				}
				
				cleanTransDatas(transDatas);
				return 4;
			}
		}
		//(3.2.2)锁定等待的信息中该session已经存在
		else
		{
			//等待锁
			if (lockTypeValue == '0')
			{
				cleanTransDatas(transDatas);
				return 2;
			}
			//占用锁
			else
			{
				cleanTransDatas(transDatas);
				return 1;
			}
		}
	}
}


void LockManager::releaseLock(const int& sessionId)
{
	//(1)根据sessionId获取会话占用和等待的所有资源
	vector<TransData *> transDatas;
	try
	{
		select(m_indexNameSES, &sessionId, transDatas);
	}
	catch(Mdb_Exception &e)
	{
		//释放查询分配的内存
		cleanTransDatas(transDatas);
		throw e;
	}
	//(2)如果该会话针对资源是占用锁,唤醒后面等待的会话
	// 如果根据sessionID选出来的记录数,就是LockManager表里的数目,说明此时,没有其他进程在操作,也就无须唤醒
	if( m_tableDesc->m_recordNum != transDatas.size() )
	{
		for (vector<TransData *>::iterator iter=transDatas.begin(); iter!=transDatas.end(); ++iter)
		{
			m_recordConvert->setRecordValue( (*iter)->m_oldRecordValue);
			char lockTypevalue = '0';
			m_recordConvert->getColumnValue("LOCK_TYPE", &lockTypevalue);
			//'0':等待锁 '1':占用锁
			if (lockTypevalue == '1')
			{	
				char maxValue[MAX_VALUE_LEN] = "\0";
				int resLen = 0;
				m_recordConvert->getColumnsValue(m_columns, maxValue, resLen);
				try
				{
					signalFirstWaitSessionByResource(maxValue);
				}
				catch(Mdb_Exception &e)
				{
					//释放查询分配的内存
					cleanTransDatas(transDatas);
					throw e;
				}
			}
			(*iter)->m_operType = '0';
		}
	}
	//(3)释放该会话占用和等待的所有资源锁
	try
	{
		deleteRec(transDatas);
	}
	catch(Mdb_Exception &e)
	{
		//释放查询分配的内存
		cleanTransDatas(transDatas);
		throw e;
	}
	//释放查询分配的内存
	cleanTransDatas(transDatas);
}


void LockManager::initialize()
{
	BaseTable::initialize();
	
	//准备m_columns
	m_columns.clear();
	for (int i=0; i<(m_tableDesc->m_tableDef).m_columnNum; ++i)
	{
		string columnName = (m_tableDesc->m_tableDef).m_columnList[i].m_name;
		if (columnName!="SESSION_ID" && columnName!="LOCK_TYPE")
		{
			m_columns.push_back(columnName);
		}
	} 
	BaseTable::prepareAllIndex();
}


void LockManager::create(const TableDef& tableDef)
{
	BaseTable::create(tableDef);
	
	//准备m_columns
	m_columns.clear();
	for (int i=0; i<(m_tableDesc->m_tableDef).m_columnNum; ++i)
	{
		string columnName = (m_tableDesc->m_tableDef).m_columnList[i].m_name;
		if (columnName!="SESSION_ID" && columnName!="LOCK_TYPE")
		{
			m_columns.push_back(columnName);
		}
	} 
}


void LockManager::drop()
{
	BaseTable::drop();
}


void LockManager::truncate()
{
	BaseTable::truncate();
}


void LockManager::createIndex(const IndexDef& indexDef)
{
	BaseTable::createIndex(indexDef);
	BaseTable::prepareAllIndex();
}


void LockManager::dropIndex(const char* indexName)
{
	BaseTable::dropIndex(indexName);
}


bool LockManager::detectDeadLock(const void* resValue, const int& resLen, const int& sessionId)
{
	//原理:	
	//根据transData获取当前占用的事务A 如果A == sessionId 死锁
	//根据A获取A等待的资源transDataW
	//根据transDataW获取当前占用的事务C 如果C == sessionId  死锁
	//直到事务没有等待的资源为止。
	char value[MAX_VALUE_LEN] = "\0";
	memcpy(value, resValue, resLen);
	int  len = resLen;
	int  newSessionId = -1;
	
	while(1)
	{
		if (!findOwnSessionByResource(value, newSessionId))
		{
			break;
		}
				
		if (newSessionId == sessionId)
		{
			return true;
		}
		else
		{
			if (!findWaitResourceBySession(newSessionId, value, len))
			{
				break;
			}
		}
	}
	
	return false;
}


void LockManager::signalFirstWaitSessionByResource(const void* resValue)
{
	//根据资源值获取等待和占用资源的所有会话
	vector<TransData *> transDatas;
	try
	{
		select(m_indexNameRES, resValue, transDatas);
	}
	catch(Mdb_Exception &e)
	{
		//释放查询分配的内存
		cleanTransDatas(transDatas);
		throw e;
	}
		
	//记录选取时，最前面的记录是最新插入的.
	for (vector<TransData *>::iterator iter=transDatas.begin(); iter!=transDatas.end(); ++iter)
	{
		m_recordConvert->setRecordValue((*iter)->m_oldRecordValue);
		char lockTypevalue = '0';
		m_recordConvert->getColumnValue("LOCK_TYPE", &lockTypevalue);
		//'0':等待锁 '1':占用锁
		if (lockTypevalue == '0')
		{
			(*iter)->cloneRecordValue();
			m_recordConvert->setRecordValue((*iter)->m_newRecordValue);
			char tmpLockTypeValue = '1';
			m_recordConvert->setColumnValue("LOCK_TYPE", &tmpLockTypeValue);
			(*iter)->m_operType = '2';
			
			try
			{
				//更新(包括索引更新)
				update(*iter);
				
				//释放查询分配的内存
				cleanTransDatas(transDatas);
			}
			catch(Mdb_Exception &e)
			{
				//释放查询分配的内存
				cleanTransDatas(transDatas);
				throw e;
			}
			break;
		}
	}
	
	//释放查询分配的内存
	cleanTransDatas(transDatas);
}


bool LockManager::findOwnSessionByResource(const void* resValue, int& sessionId)
{
	//根据资源值获取等待和占用资源的所有会话
	vector<TransData *> transDatas;
	try
	{
		select(m_indexNameRES, resValue, transDatas);
	}
	catch(Mdb_Exception &e)
	{
		//释放查询分配的内存
		cleanTransDatas(transDatas);
		throw e;
	}
	
	//从其中查询占用会话的资源信息
	for (vector<TransData *>::iterator iter=transDatas.begin(); iter!=transDatas.end(); ++iter)
	{
		m_recordConvert->setRecordValue((*iter)->m_oldRecordValue);
		char lockTypevalue = '0';
		m_recordConvert->getColumnValue("LOCK_TYPE", &lockTypevalue);
		//'0':等待锁 '1':占用锁
		if (lockTypevalue == '1')
		{
			m_recordConvert->getColumnValue("SESSION_ID", &sessionId);
			
			//释放查询分配的内存
			cleanTransDatas(transDatas);
			return true;
		}
	}
	
	//释放查询分配的内存
	cleanTransDatas(transDatas);
	return false;
}	


bool LockManager::findWaitResourceBySession(const int& sessionId, void* resValue, int& resLen)
{
	//根据sessionId获取会话占用和等待的所有资源
	vector<TransData *> transDatas;
	try
	{
		select(m_indexNameSES, &sessionId, transDatas);
	}
	catch(Mdb_Exception &e)
	{		
		//释放查询分配的内存
		cleanTransDatas(transDatas);
		throw e;
	}
	
	//从其中查询等待的资源信息
	for (vector<TransData *>::iterator iter=transDatas.begin(); iter!=transDatas.end(); ++iter)
	{
		m_recordConvert->setRecordValue((*iter)->m_oldRecordValue);
		char lockTypevalue = '0';
		m_recordConvert->getColumnValue("LOCK_TYPE", &lockTypevalue);
		//'0':等待锁 '1':占用锁
		if (lockTypevalue == '0')
		{
			m_recordConvert->getColumnsValue(m_columns, resValue, resLen);
			
			//释放查询分配的内存
			cleanTransDatas(transDatas);
			return true;
		}
	}
	
	//释放查询分配的内存
	cleanTransDatas(transDatas);
	return false;
}	

