#include "Table.h"
#include "InstanceFactory.h"
#include "Index.h"

Table::Table(const char * dbName,const char* tableName, MemManager* memManager) : BaseTable(tableName, memManager)
{
    m_lockType = 1;
    m_doubleLock = InstanceFactory::getDoubLockInstance(SYS_SEMAPORE,dbName);
    m_lockManager = NULL;
}


Table::~Table()
{
    if (m_doubleLock != NULL)
    {
        delete m_doubleLock;
    }
    if (m_lockManager != NULL)
    {
        delete m_lockManager;
    }
}


void Table::beginTrans(int lockType)
{
    m_lockType = lockType;
    //如果是直接表锁,对信号灯加锁
    //if (m_lockManager == NULL)
  if(lockType == 0)
    {
        //判断是否有别的进程在行锁
        bool bEmpty = m_lockManager->isEmpty();
        //行锁记录为空，可以加表锁
      if(bEmpty)
      {
          m_doubleLock->getWriteLock();
          return;
      }
      //有行锁记录，不能加表锁
        else
        {
            //循环休眠阻塞，重新尝试是否为空
            while(!bEmpty)
            {
                  sleep(1);
                bEmpty = m_lockManager->isEmpty();
                //为空了，可以加表锁
                if(bEmpty)
                {
                    m_doubleLock->getWriteLock();
                  return;
              }
            }
        }
    }
    else
    {
          return;
    }
}


int Table::select(const char* indexName, const InputParams *pIndexKeyParams, vector<void *>& recordVector)
{
    m_doubleLock->getReadLock();
    
    try
    {
        BaseTable::select(indexName, pIndexKeyParams, recordVector);
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseReadLock();
        throw e;
    }
    
    m_doubleLock->releaseReadLock();
    return recordVector.size();
}


int Table::selectWithTrans(int sessionId, const char* indexName, const InputParams *pIndexKeyParams, vector<TransData *>& transDatas)
{
    //如果是支持记录锁
    //if (m_lockManager != NULL)?
    if (m_lockType == 1)
    {
        m_doubleLock->getWriteLock();
    
        try
        {
            BaseTable::select(indexName, pIndexKeyParams, transDatas);
        
            for (int i=0; i< transDatas.size(); ++i)
            {
                unsigned char lockType = 0;
                int judgeValue = m_lockManager->judgeAndAddLock(*(transDatas[i]), sessionId, lockType);
                //2:有锁,同会话等待锁 3:有锁,不存在同会话锁
                if (judgeValue == 2 || judgeValue == 3)
                {
                    int ii = 0;
                    while(1)
                    {
                        //临时释放写锁，以防止别的进程无法查询或者提交
                        m_doubleLock->releaseWriteLock();
                        
                        //等待1毫秒
                        ii++;
                        waitMicroSeconds(1);
                        
                        m_doubleLock->getWriteLock();
                        //再次进行判断
                        judgeValue = m_lockManager->judgeAndAddLock(*(transDatas[i]), sessionId, lockType);
                        if (judgeValue==0 || judgeValue==1)
                        {
                            break;
                        }
                        //如果等待时间超过1分钟
                        else if (ii>60000)
                        {
                            m_lockManager->releaseLock(sessionId);
                            throw Mdb_Exception(__FILE__, __LINE__, "表资源等待时间超过1分钟, 请加以核查!");
                        }
                    }
                }
                //死锁
                else if (judgeValue == 4)
                {
                    throw Mdb_Exception(__FILE__, __LINE__, "表资源事务处理过程中发现死锁,请核查后再继续进行操作!");
                }
                //0:无锁 1:有锁,同会话占用锁
                else
                {
                    ;
                }
            }
        }
        catch(Mdb_Exception &e)
        {
            cleanTransDatas(transDatas);
            transDatas.clear();
            
            m_doubleLock->releaseWriteLock();
            throw e;
        }
        
        m_doubleLock->releaseWriteLock();
    }
    //如果是支持表锁
    else
    {
        BaseTable::select(indexName, pIndexKeyParams, transDatas);
    }
        
    return transDatas.size();
}


void Table::rollback(const int& sessionId)
{    
    //如果支持记录锁,加写锁并释放锁表中记录;否则已经加锁,无需再加锁.
    //if (m_lockManager != NULL)
    if (m_lockType == 1)
    {
        m_doubleLock->getWriteLock();
        try
        {
            m_lockManager->releaseLock(sessionId);
        }
        catch(Mdb_Exception &e)
        {
            m_doubleLock->releaseWriteLock();
            throw e;
        }
    }
        
    m_doubleLock->releaseWriteLock();
}


void Table::commit(const int& sessionId, const TRRANSDATA_VECTOR& transDatas)
{    
    //(1)如果支持记录锁,加写锁并释放锁表中记录;否则已经加锁,无需再加锁.
    //if (m_lockManager != NULL)
    if (m_lockType == 1)
    {
        m_doubleLock->getWriteLock();
        try
        {
            m_lockManager->releaseLock(sessionId);
        }
        catch(Mdb_Exception &e)
        {
            m_doubleLock->releaseWriteLock();
            throw e;
        }
    }
    //(2)将数据根据操作类型进行分类
    //vector<TransData *> insertTransData;
    //vector<TransData *> deleteTransData;
    //vector<TransData *> updateTransData;
    insertTransData.clear();
    deleteTransData.clear();
    updateTransData.clear();

    TRRANSDATA_VECTOR::const_iterator multimapitr;
    
    for(multimapitr = transDatas.begin();
        multimapitr!=transDatas.end();multimapitr++)
    {
        switch((*multimapitr)->m_operType)
        {
        	case '0': //删除
        		deleteTransData.push_back(*multimapitr);
        		break;
        	case '1': //插入
        		insertTransData.push_back(*multimapitr);
        		break;
        	case '2': //更新
        		updateTransData.push_back(*multimapitr);
        		break;	
        }    
        /*
        //删除
        if ( (multimapitr->second)->m_operType == '0')
        {
            deleteTransData.push_back(multimapitr->second);
        }
        //插入
        else if ((multimapitr->second)->m_operType == '1')
        {
            insertTransData.push_back(multimapitr->second);
        }
        //更新
        else if ((multimapitr->second)->m_operType == '2')
        {
            updateTransData.push_back(multimapitr->second);
        }*/
    }
    //(2)将表所有的索引准备好
    try
    {
        BaseTable::prepareAllIndex();
    }
    catch (Mdb_Exception e)
    {
        m_doubleLock->releaseWriteLock();
        throw e;
    }
    //(3)进行删除/插入/更新操作
    //vector<TransData *> unInsertTransData;
    //vector<TransData *> unDeleteTransData;
    //vector<TransData *> unUpdateTransData;
    unInsertTransData.clear();
    unDeleteTransData.clear();
    unUpdateTransData.clear();
    vector<TransData *>::iterator    iter;
    try
    {
        void *pTmpRecordValue;
        
        if (deleteTransData.size() > 0)
        {
            BaseTable::deleteRec(deleteTransData);
            for (iter=deleteTransData.begin(); iter!=deleteTransData.end(); ++iter)
            {
                /* disabled by chenm 2008-5-22 9:12:36 改为使用指针vector
                TransData *transData;
                transData.m_addr = iter->m_addr;
                transData.m_recLength = iter->m_recLength;
                transData.m_oldRecordValue = iter->m_newRecordValue;
                transData.m_newRecordValue = iter->m_oldRecordValue;
                transData.m_operType = '1';
                */
                pTmpRecordValue = (*iter)->m_newRecordValue;
                (*iter)->m_newRecordValue = (*iter)->m_oldRecordValue;
                (*iter)->m_oldRecordValue = pTmpRecordValue;
                unDeleteTransData.push_back(*iter);
            }
        }
        if (insertTransData.size() > 0)
        {
            BaseTable::insert(insertTransData);
            for (iter=insertTransData.begin(); iter!=insertTransData.end(); ++iter)
            {
                /*disabled by chenm 2008-5-22 9:12:36 改为使用指针vector
                TransData transData;
                transData.m_addr = iter->m_addr;
                transData.m_recLength = iter->m_recLength;
                transData.m_oldRecordValue = iter->m_newRecordValue;
                transData.m_newRecordValue = iter->m_oldRecordValue;
                transData.m_operType = '0';
                */
                pTmpRecordValue = (*iter)->m_newRecordValue;
                (*iter)->m_newRecordValue = (*iter)->m_oldRecordValue;
                (*iter)->m_oldRecordValue = pTmpRecordValue;
                unInsertTransData.push_back(*iter);
            }
            
        }
        if (updateTransData.size() > 0)
        {
            BaseTable::update(updateTransData);
            for (iter=updateTransData.begin(); iter!=updateTransData.end(); ++iter)
            {
                /* disabled by chenm 2008-5-22 9:12:36 改为使用指针vector
                TransData transData;
                transData.m_addr = iter->m_addr;
                transData.m_recLength = iter->m_recLength;
                transData.m_oldRecordValue = iter->m_newRecordValue;
                transData.m_newRecordValue = iter->m_oldRecordValue;
                transData.m_operType = '2';
                */
                pTmpRecordValue = (*iter)->m_newRecordValue;
                (*iter)->m_newRecordValue = (*iter)->m_oldRecordValue;
                (*iter)->m_oldRecordValue = pTmpRecordValue;
                unUpdateTransData.push_back(*iter);
            }
        }
    }
    catch(Mdb_Exception e1)
    {
        try
        {
            if (unDeleteTransData.size() > 0)
            {
                BaseTable::insert(unDeleteTransData);
            }
            if (unInsertTransData.size() > 0)
            {
                BaseTable::deleteRec(unInsertTransData);
            }
            if (unUpdateTransData.size() > 0)
            {
                BaseTable::update(unUpdateTransData);
            }
        }
        catch(Mdb_Exception e2)
        {
            m_doubleLock->releaseWriteLock();
            throw e2;
        }
        m_doubleLock->releaseWriteLock();
        throw e1;
    }
    
    m_doubleLock->releaseWriteLock();
}


void Table::initialize()
{
    m_doubleLock->open(3, m_tableName);
    m_doubleLock->getWriteLock();
    
    try
    {
        //表和索引初始化
        BaseTable::initialize();
        
        //如果不是LOCK_TABLE，需要实例自己的m_lockManager成员  
        if((m_tableDesc->m_tableDef).m_tableType!=LOCK_TABLE)
        {
    
            //支持记录锁,锁表初始化
            //if ((m_tableDesc->m_tableDef).m_lockType == 1)
            //{
            //统一支持记录锁，都要注册LockManager
            T_NAMEDEF      lockTableName;
            sprintf(lockTableName, "LOCK_%s", m_tableName);
            m_lockManager = new LockManager(lockTableName, m_memManager, this);
            m_lockManager->initialize();
            //}
        }
        else
        {
            ;//LOCK_TABLE，无需实例m_lockManager成员
        }
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseWriteLock();
        throw e;
    }
    
    m_doubleLock->releaseWriteLock();
}


void Table::create(const TableDef& tableDef)
{
    m_doubleLock->open(3, m_tableName);
    m_doubleLock->getWriteLock();
    
    try
    {
        //表创建
        BaseTable::create(tableDef);
        
        ////支持记录锁;锁表创建
        //if ((m_tableDesc->m_tableDef).m_lockType == 1)
        //{
        //统一支持记录锁，都要使用LockManager，注释掉if
            //(1)锁表初始化
            T_NAMEDEF      lockTableName;
            sprintf(lockTableName, "LOCK_%s", m_tableName);
            m_lockManager = new LockManager(lockTableName, m_memManager, this);
            
            //(2)锁表创建
            int offset = 0;
            int pos = 0;
            TableDef lockManagerDef;
            sprintf(lockManagerDef.m_tableName, lockTableName);
            lockManagerDef.m_tableType = LOCK_TABLE;
            
            if(tableDef.m_keyClumnNum<0)
            	throw Mdb_Exception(__FILE__, __LINE__,"表定义中的m_keyClumnNum 必须为大于0的整数!");
            	
            lockManagerDef.m_columnNum = ((tableDef.m_keyClumnNum==0) ? 3 : (tableDef.m_keyClumnNum+ 2));        
            for (int i=0; i<lockManagerDef.m_columnNum; ++i)
            {
                if (i == 0)
                {
                    strcpy(lockManagerDef.m_columnList[i].m_name, "SESSION_ID");
                    lockManagerDef.m_columnList[i].m_type = VAR_TYPE_INT;
                    lockManagerDef.m_columnList[i].m_len = 4;
                    lockManagerDef.m_columnList[i].m_offSet = offset;
                    offset += lockManagerDef.m_columnList[i].m_len;
                }
                else if (i == 1) 
                {
                    strcpy(lockManagerDef.m_columnList[i].m_name, "LOCK_TYPE");
                    lockManagerDef.m_columnList[i].m_type = VAR_TYPE_VSTR;
                    lockManagerDef.m_columnList[i].m_len = 1;
                    lockManagerDef.m_columnList[i].m_offSet = offset;
                    offset += lockManagerDef.m_columnList[i].m_len;
                }
                else
                {
                    if (tableDef.m_keyClumnNum==0)
                    {
                        strcpy(lockManagerDef.m_columnList[i].m_name, "RES_ADDR");
                        lockManagerDef.m_columnList[i].m_type = VAR_TYPE_LONG;//VAR_TYPE_VSTR;
                        lockManagerDef.m_columnList[i].m_len = 8;
                        lockManagerDef.m_columnList[i].m_offSet = offset;
                        offset += lockManagerDef.m_columnList[i].m_len;
                    }
                    else
                    {
                        m_recordConvert->getPkColumnPos(i-2, pos);
                        strcpy(lockManagerDef.m_columnList[i].m_name, tableDef.m_columnList[pos].m_name);
                        lockManagerDef.m_columnList[i].m_type = tableDef.m_columnList[pos].m_type;
                        lockManagerDef.m_columnList[i].m_len = tableDef.m_columnList[pos].m_len;
                        lockManagerDef.m_columnList[i].m_offSet = offset;
                        offset += lockManagerDef.m_columnList[i].m_len;
                    }
                }
            }
            lockManagerDef.m_spaceNum = tableDef.m_spaceNum;
            for (int i=0; i<tableDef.m_spaceNum; i++)
            {
                strcpy(lockManagerDef.m_spaceList[i], tableDef.m_spaceList[i]);
            }
            lockManagerDef.m_keyFlag = 0;
            lockManagerDef.m_keyClumnNum = 0;
            //lockManagerDef.m_lockType = 0;
            lockManagerDef.m_logType = 0;
            
            m_lockManager->create(lockManagerDef);
            
            //(3)锁表SESSION索引创建
            IndexDef sesIndexDef;
            sprintf(sesIndexDef.m_indexName, "IDX_%s_SES", lockManagerDef.m_tableName);
            sesIndexDef.m_indexType = HASH_INDEX;
            sesIndexDef.m_hashSize = 10000;
            strcpy(sesIndexDef.m_tableName, lockManagerDef.m_tableName);
            sesIndexDef.m_columnNum = 1;
            strcpy(sesIndexDef.m_columnList[0], "SESSION_ID");
            sesIndexDef.m_spaceNum = tableDef.m_spaceNum;
            for (int i=0; i<tableDef.m_spaceNum; i++)
            {
                strcpy(sesIndexDef.m_spaceList[i], tableDef.m_spaceList[i]);
            }
            sesIndexDef.m_isUnique = false;
            
            m_lockManager->createIndex(sesIndexDef);
            
            //(3)锁表RESOURCE索引创建
            IndexDef resIndexDef;
            sprintf(resIndexDef.m_indexName, "IDX_%s_RES", lockManagerDef.m_tableName);
            resIndexDef.m_indexType = HASH_INDEX;
            resIndexDef.m_hashSize = 10000;
            strcpy(resIndexDef.m_tableName, lockManagerDef.m_tableName);
            if (tableDef.m_keyClumnNum == 0)
            {
                resIndexDef.m_columnNum = 1;
                strcpy(resIndexDef.m_columnList[0], "RES_ADDR");
            }
            else
            {
                resIndexDef.m_columnNum = tableDef.m_keyClumnNum;
                for (int i=0; i<tableDef.m_keyClumnNum; ++i)
                {            
                    strcpy(resIndexDef.m_columnList[i], tableDef.m_keyColumnList[i]);
                }
            }
            resIndexDef.m_spaceNum = tableDef.m_spaceNum;
            for (int i=0; i<tableDef.m_spaceNum; i++)
            {
                strcpy(resIndexDef.m_spaceList[i], tableDef.m_spaceList[i]);
            }
            resIndexDef.m_isUnique = false;
            
            m_lockManager->createIndex(resIndexDef);
        //}//统一支持记录锁，都要使用LockManager，
        //注释掉ifif ((m_tableDesc->m_tableDef).m_lockType == 1)
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseWriteLock();
        m_doubleLock->destroy();
        throw e;
    }
    
    m_doubleLock->releaseWriteLock();
}


void Table::drop()
{
    m_doubleLock->getWriteLock();
    
    if (m_lockManager->isEmpty())
    {
        try
        {
            // 先删除LOCK表
            m_lockManager->drop();
        }
        catch(Mdb_Exception &e)
        {
			try
			{
	        	// 删除LOCK表失败,则尝试删除主表
	        	BaseTable::drop();
			}
			catch(Mdb_Exception &e)
			{
				m_doubleLock->releaseWriteLock();
	            throw Mdb_Exception(__FILE__, __LINE__,"drop table %s 出错:%s",m_tableName,e.GetString());	
			}
            // 删除主表成功,LOCK表失败,则出异常
            m_doubleLock->releaseWriteLock();
            throw Mdb_Exception(__FILE__, __LINE__,"drop lock table LOCK_%s 出错:%s",m_tableName,e.GetString());e;
        }
        
		try
		{
        	BaseTable::drop();
		}
		catch(Mdb_Exception &e)
		{
			m_doubleLock->releaseWriteLock();
            throw Mdb_Exception(__FILE__, __LINE__,"drop table %s 出错:%s",m_tableName,e.GetString());	
		}

        m_doubleLock->releaseWriteLock();
        m_doubleLock->destroy();
    }
    else
    {
        m_doubleLock->releaseWriteLock();
        throw Mdb_Exception(__FILE__, __LINE__, "表资源存在事务处理,请核查后进行删除!");
    }
}


void Table::truncate()
{
    m_doubleLock->getWriteLock();
    
    if (m_lockManager==NULL
    	||m_lockManager->isEmpty())
    {
        try
        {
            BaseTable::truncate();
        }
        catch(Mdb_Exception &e)
        {
            m_doubleLock->releaseWriteLock();
            throw e;
        }
        m_doubleLock->releaseWriteLock();
    }
    else
    {
        m_doubleLock->releaseWriteLock();
        throw Mdb_Exception(__FILE__, __LINE__, "表资源存在事务处理,请核查后进行清空!");
    }
}


void Table::createIndex(const IndexDef& indexDef)
{
    m_doubleLock->getWriteLock();
    
    try
    {
        BaseTable::createIndex(indexDef);
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseWriteLock();
        throw e;
    }
    
    m_doubleLock->releaseWriteLock();
}


void Table::dropIndex(const char* indexName)
{
    m_doubleLock->getWriteLock();
    
    try
    {
        BaseTable::dropIndex(indexName);
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseWriteLock();
        throw e;
    }
    
    m_doubleLock->releaseWriteLock();
}


void Table::waitMicroSeconds(int microSeconds)
{
    struct timeval waitime;
    waitime.tv_sec = microSeconds/1000 ;
    waitime.tv_usec = microSeconds%1000;
    ::select(0 ,NULL, NULL, NULL, &waitime); 
}

void Table::reIndex(const char* srcIndexName,const char* destIndexName)
{
	m_doubleLock->getWriteLock();
	int iSrcPos = -1,iDestPos = -1;
	for (int i=0;i<m_tableDesc->m_indexNum;i++)
	{
		if (strcasecmp(srcIndexName,m_tableDesc->m_indexNameList[i]) == 0)
		{
			iSrcPos = i;
		}
		if (strcasecmp(destIndexName,m_tableDesc->m_indexNameList[i]) == 0)
		{
			iDestPos = i;
		}
	}

	if (iSrcPos == -1){
		m_doubleLock->releaseWriteLock();
		throw Mdb_Exception(__FILE__, __LINE__, "源索引名错误, 请核查!");			
	}
	if (iDestPos == -1){
		m_doubleLock->releaseWriteLock();
		throw Mdb_Exception(__FILE__, __LINE__, "目标索引名错误, 请核查!");			
	}
	
    IndexDesc *srcIndexDesc = NULL;
    IndexDesc *destIndexDesc = NULL;
	m_memManager->getIndexDescByName(srcIndexName, srcIndexDesc);	
	m_memManager->getIndexDescByName(destIndexName, destIndexDesc);	
	
	RecordConvert *pSrcConvert=NULL;
	RecordConvert *pDestConvert=NULL;
	
	Index * pSrcIndex=NULL;
	Index * pDestIndex=NULL;
	
	try
	{
        pSrcConvert=new RecordConvert(m_tableDesc);
		pSrcIndex=InstanceFactory::getIndexInstance(srcIndexDesc->m_indexDef.m_indexType
										   ,m_tableName
		                                   ,m_tableDesc->m_indexNameList[iSrcPos ]
		                                   ,m_memManager
		                                   ,pSrcConvert);	                       
        pDestConvert=new RecordConvert(m_tableDesc);
		pDestIndex=InstanceFactory::getIndexInstance(destIndexDesc->m_indexDef.m_indexType
			                               ,m_tableName
		                                   ,m_tableDesc->m_indexNameList[iDestPos ]
		                                   ,m_memManager
		                                   ,pDestConvert);	
		pSrcIndex->reIndex(pDestIndex);
		if (pSrcConvert != NULL) delete pSrcConvert;
		if (pSrcIndex != NULL) delete pSrcIndex;
		if (pDestConvert!=NULL) delete pDestConvert;
		if (pDestIndex != NULL) delete pDestIndex;
	}
	catch (Mdb_Exception &e)
	{
		if (pSrcConvert != NULL) delete pSrcConvert;
		if (pSrcIndex != NULL) delete pSrcIndex;
		if (pDestConvert!=NULL) delete pDestConvert;
		if (pDestIndex != NULL) delete pDestIndex;

		#ifdef _DEBUG_
		e.toString();
		#endif
		m_doubleLock->releaseWriteLock();
		throw e;
	}
	m_doubleLock->releaseWriteLock();
}

// add by chenm 2009-6-3 15:56:00
int Table::selectRange(const char* indexName
							, const InputParams *pGtParams      
							, const InputParams *pLtParams
							, const int iRangeTag
							, vector<void *>& recordVector)
{
    m_doubleLock->getReadLock();
    
    try
    {
        BaseTable::selectRange(indexName, pGtParams, pLtParams, iRangeTag, recordVector);
    }
    catch(Mdb_Exception &e)
    {
        m_doubleLock->releaseReadLock();
        throw e;
    }
    
    m_doubleLock->releaseReadLock();
    return recordVector.size();		
}	

int Table::selectRangeWithTrans(int sessionId
									, const char* indexName
									, const InputParams *pGtParams      
									, const InputParams *pLtParams
									, const int iRangeTag
									, vector<TransData *>& transDatas)
{
    //如果是支持记录锁
    //if (m_lockManager != NULL)?
    if (m_lockType == 1)
    {
        m_doubleLock->getWriteLock();
    
        try
        {
            BaseTable::selectRange(indexName, pGtParams , pLtParams, iRangeTag, transDatas);
        
            for (int i=0; i< transDatas.size(); ++i)
            {
                unsigned char lockType = 0;
                int judgeValue = m_lockManager->judgeAndAddLock(*(transDatas[i]), sessionId, lockType);
                //2:有锁,同会话等待锁 3:有锁,不存在同会话锁
                if (judgeValue == 2 || judgeValue == 3)
                {
                    int ii = 0;
                    while(1)
                    {
                        //临时释放写锁，以防止别的进程无法查询或者提交
                        m_doubleLock->releaseWriteLock();
                        
                        //等待1毫秒
                        ii++;
                        waitMicroSeconds(1);
                        
                        m_doubleLock->getWriteLock();
                        //再次进行判断
                        judgeValue = m_lockManager->judgeAndAddLock(*(transDatas[i]), sessionId, lockType);
                        if (judgeValue==0 || judgeValue==1)
                        {
                            break;
                        }
                        //如果等待时间超过1分钟
                        else if (ii>60000)
                        {
                            m_lockManager->releaseLock(sessionId);
                            throw Mdb_Exception(__FILE__, __LINE__, "表资源等待时间超过1分钟, 请加以核查!");
                        }
                    }
                }
                //死锁
                else if (judgeValue == 4)
                {
                    throw Mdb_Exception(__FILE__, __LINE__, "表资源事务处理过程中发现死锁,请核查后再继续进行操作!");
                }
                //0:无锁 1:有锁,同会话占用锁
                else
                {
                    ;
                }
            }
        }
        catch(Mdb_Exception &e)
        {
            cleanTransDatas(transDatas);
            transDatas.clear();
            
            m_doubleLock->releaseWriteLock();
            throw e;
        }
        
        m_doubleLock->releaseWriteLock();
    }
    //如果是支持表锁
    else
    {
        BaseTable::selectRange(indexName, pGtParams , pLtParams, iRangeTag, transDatas);
    }
        
    return transDatas.size();		
}
// over 2009-6-3 15:56:09
