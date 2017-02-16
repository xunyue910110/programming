// ############################################
// Source file : MdbSession.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-22
// Update      : 2008-4-22
// Copyright(C): chen min, Linkage.
// ############################################

#include "TransDataMgr.h"
#include "ExpressionMgr.h"
#include "MdbSession.h"
#include "Table.h"
#include "DataBase.h"
#include "InputParams.h"


class Table;
// 自主研发MDB的session管理实现

MdbSession::MdbSession(DataBase *db)
{
	m_isTransaction = false;
	m_table = NULL;
	m_db    = db;
	m_iLockType = 1; // 默认行级锁 by chenm 2009-4-2 
	m_isRowByRowSelect = false; // by chenm 2009-5-18 逐行扫描
}

MdbSession::~MdbSession()
{
	// 清除查询结果集
	this->clearQueryResults();
}

void MdbSession::begin(const char *cTableName,int iLockType) 
{	
	// 事务不能嵌套
	if(m_isTransaction == false )
	{
		m_table = m_db->findTableByName(cTableName);
		m_table->beginTrans(iLockType);
		m_iLockType = iLockType;
		
		// 保险起见 还是把私有数据缓存去清除一下
		m_transDataMgr.clearTransData();
		m_isTransaction = true;
		return;
	}
	else
	{
		throw Mdb_Exception(__FILE__, __LINE__, "不支持嵌套事务!");	
	}
}

bool MdbSession::commit() 
{
	// modified by chenm 2009-4-2 
	//m_table->commit(m_sessionID,m_transDataMgr.getTransDataPool()); 
	try
	{
		m_table->commit(m_sessionID,m_transDataMgr.getTransDataPool());
	} 
	catch(Mdb_Exception &e)
	{
		// 如果是事务是表锁,而非行锁,将m_table置NULL,rollBack函数就不会执行m_table->rollback,
		// 因为表锁事务的m_table->rollback实际上就是释放MutexDoubleLock写锁,而table的commit函数无论是否有异常,这个锁已经释放过了
		// 如果再执行一次m_table->rollback,写锁计数器就会出现负值
		if(m_iLockType == 0)
		{
			m_table = NULL;	
		} 
		throw Mdb_Exception(__FILE__, __LINE__, "commit false!%s",e.GetString());		
	}
	// over 2009-4-2 14:45:23
	
	m_transDataMgr.clearTransData();
	m_table = NULL;
	m_isTransaction = false;	
	
	return true;
}	
	
bool MdbSession::rollBack() 
{
	if(m_isTransaction==true)
	{
		if(m_table!=NULL)
			m_table->rollback(m_sessionID);
		
		m_transDataMgr.clearTransData();
		m_table = NULL;
		m_isTransaction = false;
	}
	
	return true;	
}

bool MdbSession::createTable(const TableDef &theTableDef)
{
	m_db->createTable(theTableDef);
	return true;
}

bool MdbSession::dropTable(const char * cTableName)
{
	m_db->dropTable(cTableName);
	return true;
}

bool MdbSession::truncate(const char * cTableName)
{
	Table *pTable;
		
	pTable = m_db->findTableByName(cTableName);
	pTable->truncate();
	
	return true;	
}

bool MdbSession::createIndex(const IndexDef &theIndexDef)
{
	Table *pTable;
	
	pTable = m_db->findTableByName(theIndexDef.m_tableName);
	pTable->createIndex(theIndexDef);
	
	return true;
}

bool MdbSession::dropIndex(const char * cTableName,const char * cIndexName)
{
	Table *pTable;
		
	pTable = m_db->findTableByName(cTableName);
	pTable->dropIndex(cIndexName);
	
	return true;
}

int MdbSession::select(const char *cTableName
			,const char *cIndexName
			,const InputParams *pIndexKeyParams
			,const char *cExpr             
			,const InputParams *pOtherParams
			,const int iRowCount)  
{
	Expression *pExpression;
	
	// 清除上一次的查询结果集
	this->clearQueryResults();
	
	if( cExpr != NULL && strcmp(cExpr,"")!=0)
		pExpression = m_expressionMgr.getExpre(cExpr);
		
	if( m_isTransaction==false )
	{
		m_table = m_db->findTableByName(cTableName);
		
		m_isRowByRowSelect = false;
		if(iRowCount!=-1)
		{
			m_table->setRowCount(iRowCount);
			// add by chenm 2009-5-18 
			if(iRowCount == SELECT_ROW_BY_ROW)
			{
				m_cRowByRowExpr        = cExpr;
				m_pRowByRowOtherParams = pOtherParams;
				m_cRowByRowTableName   = cTableName;
				m_cRowByRowIndexName   = cIndexName;
				m_isRowByRowSelect     = true;					
			}
			// over 2009-5-18 
		}
				
		if( m_table->select(cIndexName,pIndexKeyParams,m_queryResults) == 0 )
			return 0;
			
		// 进行其他查询条件判断
		if( cExpr != NULL && strcmp(cExpr,"")!=0)
		{
			for(int i=0;i<m_queryResults.size();++i)
			{
				if( pExpression->evaluate(m_queryResults[i],m_table->getRecordConvert(),(const void **)pOtherParams->m_pValues) == false)
				{
					free(m_queryResults[i]);
					m_queryResults.erase(m_queryResults.begin()+i);
					--i;
				}
			}	
		}
		return m_queryResults.size();
	}
	else
	{
		// 只针对单表进行事务支持,表对象在MdbSession::begin的时候就得到了
		this->isAcrossTable(cTableName);
		if( m_table->selectWithTrans(m_sessionID,cIndexName,pIndexKeyParams,m_transQueryResults) == 0 )
			return 0;	

		TransData *pTransData = NULL;

		for(int i=0;i<m_transQueryResults.size();++i)
		{
			bool isMatch = false;
			//  如果TransDataMgr缓存中有该条记录,则用缓存中的记录代替内存数据库中查找到的记录
			if( (pTransData = m_transDataMgr.matchTransData(m_transQueryResults[i]) ) != NULL )
			{
				delete m_transQueryResults[i];
				if(pTransData->m_operType == '0')// 如果缓存中的记录,是被删除的记录,则在同一事务里,再select时,不再作为选中记录返回
				{
					m_transQueryResults.erase(m_transQueryResults.begin()+i);
					--i;
					continue;
				}
				else
				{
					m_transQueryResults[i] = pTransData;
					isMatch = true;	
				}
			}
			// 进行其他查询条件判断
			if( (cExpr != NULL && strcmp(cExpr,"")!=0)
				&&(pExpression->evaluate(m_transQueryResults[i]->m_oldRecordValue
				                      ,m_table->getRecordConvert()
				                      ,(const void **)pOtherParams->m_pValues) == false )
			   )
			{
				if( isMatch == false )
					delete m_transQueryResults[i];
				m_transQueryResults.erase(m_transQueryResults.begin()+i);
				--i;
			}
			else 
			{
				// 如果TransDataMgr缓存中没有该数据,则插入
				if( isMatch == false )
				{
					m_transDataMgr.insertTransData(m_transQueryResults[i]);	
				}	
			}	
		}
		return m_transQueryResults.size();
	}
}


int MdbSession::deleteRec(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams
		,const char *cExpr          
		,const InputParams *pOtherParams) 
{
	int iRowCount = this->select(cTableName,cIndexName,pIndexKeyParams,cExpr,pOtherParams);
	if( iRowCount!=0 )
	{
		int i;
		for(i=0;i<m_transQueryResults.size();++i)
		{
			m_transQueryResults[i]->m_operType = '0';				
		}
		return i;
	}
	else
	{
		return 0;
	}
}

int MdbSession::update(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams
		,vector<string> &sFieldNames
		,const InputParams *pUpdateValues           
		,const char *cExpr             
		,const InputParams *pOtherParams)   
{
	this->isAcrossTable(cTableName);  
	
	if( this->select(cTableName,cIndexName,pIndexKeyParams,cExpr,pOtherParams) != 0)
	{
		int i;
		for(i=0;i<m_transQueryResults.size();++i)
		{
			m_transQueryResults[i]->cloneRecordValue();
			m_table->getRecordConvert()->setRecordValue(m_transQueryResults[i]->m_newRecordValue);
			m_table->getRecordConvert()->setColumnsValue(sFieldNames,pUpdateValues);
			m_transQueryResults[i]->m_operType = '2';				
		}
		return i;
	}
	else
	{
		return 0;
	}
}
int MdbSession::update(const char *cTableName
						,const char *cIndexName
						,const vector<InputParams *>pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams)
{
	int iCount = pIndexKeyParams.size();
	int iUpdateCount = 0;
	for (int i=0;i<iCount;i++)
		iUpdateCount+=update(cTableName,cIndexName,pIndexKeyParams[i],sFieldNames,pUpdateValues,cExpr,pOtherParams);
	return iUpdateCount;
}
//Added 2008-7-8
int MdbSession::update(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams
		,vector<string> &sFieldNames
		,const vector<InputParams *>pUpdateValues           
		,const char *cExpr             
		,const vector<InputParams *>pOtherParams)
{
	Expression *pExpression;
	int total = 0;
	
	if( cExpr != NULL && strcmp(cExpr,"")!=0)
		pExpression = m_expressionMgr.getExpre(cExpr);
		
	this->isAcrossTable(cTableName);  
	
	if( this->select(cTableName,cIndexName,pIndexKeyParams,NULL,NULL) != 0)
	{
		int i;
		for(i=0;i<m_transQueryResults.size();++i)
		{
			for(int j=0; j < pUpdateValues.size(); j ++)
			{
				// 进行其他查询条件判断
				if( (cExpr != NULL && strcmp(cExpr,"")!=0)
					&&(pExpression->evaluate(m_transQueryResults[i]->m_oldRecordValue
					                      ,m_table->getRecordConvert()
					                      ,(const void **)pOtherParams[j]->m_pValues))
				   )
				{
					m_transQueryResults[i]->cloneRecordValue();
					m_table->getRecordConvert()->setRecordValue(m_transQueryResults[i]->m_newRecordValue);
					m_table->getRecordConvert()->setColumnsValue(sFieldNames,pUpdateValues[j]);
					m_transQueryResults[i]->m_operType = '2';
					total ++;		
				}
			}
		}
		return total;
	}
	else
	{
		return 0;
	}
}

//Modified By Sunam 2009/11/4 20:00:44
//bool MdbSession::insert(const char *cTableName,const vector<InputParams *>&pInputParams)
bool MdbSession::insert(const char *cTableName,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	for (int i=0;i<pInputParams.size();i++)
	{
		if (!insert(cTableName,pInputParams[i]))
			return false;
	}
	return true;
}

//Modified By Sunam 2009/11/4 20:00:44
//bool MdbSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams)
bool MdbSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	for (int i=0;i<pInputParams.size();i++)
	{
		if (!insert(cTableName,cFieldNames,pInputParams[i]))
			return false;
	}
	return true;
}
bool MdbSession::insert(const char *cTableName,const InputParams *pInputParams)         
{
	this->isAcrossTable(cTableName);
	
	// add by chenm 2009-5-21 0:04:48
	if(m_table->m_tableDesc->m_tableDef.m_columnNum != pInputParams->m_iNum)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "值个数 %d和表定义字段数 %d不符!",pInputParams->m_iNum,m_table->m_tableDesc->m_tableDef.m_columnNum);	
	}
	// over 2009-5-21 0:04:53
	
	TransData *pTransData = new TransData();
	pTransData->m_newRecordValue = m_table->getRecordConvert()->assembleRec((const void **)pInputParams->m_pValues);  
	pTransData->m_operType       = '1';
	
	m_transDataMgr.insertTransData(pTransData);
	
	return true;
}

bool MdbSession::insert(const char *cTableName
			    		,const vector<string> &cFieldNames     // 指定插入字段顺序
			    		,const InputParams *pInputParams)      // 需要插入的记录分字段内容
{
	this->isAcrossTable(cTableName);
	
	if( cFieldNames.size()!=pInputParams->m_iNum )
		throw Mdb_Exception(__FILE__, __LINE__, "字段名和字段值数目不符!");
	
	TransData *pTransData = new TransData();
	pTransData->m_newRecordValue = m_table->getRecordConvert()->assembleRecByColumnName(cFieldNames,(const void **)pInputParams->m_pValues);  
	pTransData->m_operType       = '1';
	
	m_transDataMgr.insertTransData(pTransData);
	
	return true;	
}

bool MdbSession::isAcrossTable(const char *cTableName)
{
	if( strcasecmp(m_table->getTableName(),cTableName) != 0)
		throw Mdb_Exception(__FILE__, __LINE__, "出现跨表事务!");	
	
	return true;
}

/** 获取查询结果集的下一条记录 */
bool MdbSession::getNextRec()
{
	if( m_isTransaction == true )
	{
		if( m_resultSequnce < m_transQueryResults.size() )
		{
			if( m_transQueryResults[m_resultSequnce]->m_newRecordValue != NULL )
			{
				m_table->getRecordConvert()->setRecordValue(m_transQueryResults[m_resultSequnce]->m_newRecordValue);
			}
			else
			{
				m_table->getRecordConvert()->setRecordValue(m_transQueryResults[m_resultSequnce]->m_oldRecordValue);
			}
			++m_resultSequnce;
						
			return true;	
		}
		else
		{
			return false;	
		}	
	}
	else
	{
		if(m_isRowByRowSelect == false) // add by chenm 2009-5-18 
		{
			if( m_resultSequnce < m_queryResults.size() )
			{
				m_table->getRecordConvert()->setRecordValue(m_queryResults[m_resultSequnce]);
				++m_resultSequnce;
				return true;	
			}
			else
			{
				return false;	
			}
		}
		// add by chenm 2009-5-18 全表扫描
		else if(m_isRowByRowSelect == true)
		{
			if( m_resultSequnce < m_queryResults.size() )
			{
				m_table->getRecordConvert()->setRecordValue(m_queryResults[m_resultSequnce]);
				++m_resultSequnce;
				return true;	
			}
			else
			{
				if(this->selectRowByRow() !=0 )
				{
					m_table->getRecordConvert()->setRecordValue(m_queryResults[m_resultSequnce]);
					++m_resultSequnce;
					return true;	
				}
				else
				{
					return false;
				}	
			}	
		}
		// over 2009-5-18 	
	}
}

// 清除查询结果集
void MdbSession::clearQueryResults()
{
	m_resultSequnce = 0;
	for(int i=0;i<m_queryResults.size();++i)
	{
		if( m_queryResults[i] != NULL )
			free(m_queryResults[i]);
	}
	m_queryResults.clear();
	
	// 带事务查询结果集,只需结果集清空,其维护的指针集在transData生命周期结束时释放
	m_transQueryResults.clear();
	
	return;
}

// 根据字段名,获取字段值
bool MdbSession::getColumnValue(const char* columnName, void * value)
{
	return m_table->getRecordConvert()->getColumnValue(columnName,value);	
}
// 根据字段名,获取字段值
bool MdbSession::getColumnValue(const char* columnName, void * value,int &iColumnLenth)
{
	return m_table->getRecordConvert()->getColumnValue(columnName,value,iColumnLenth);	
}

void MdbSession::setSessionID(int &i)
{
	m_sessionID = i;
	return;	
}

int  MdbSession::getSessionID()
{
	return m_sessionID;	
}

// 根据字段顺序,获取字段值
bool MdbSession::getColumnValue(int iSequnce, void * value,int &iColumnLenth)
{
	return 	m_table->getRecordConvert()->getColumnValue(iSequnce,value,iColumnLenth);
}

const IndexDef * MdbSession::getIndexDef(const char* tableName,const char* indexName)
{
	IndexDesc *pIndesDesc;
	
	(m_db->getMemMgrInstance())->getIndexDescByName(indexName,pIndesDesc);
	
	return &(pIndesDesc->m_indexDef);
}

const TableDef* MdbSession::getTableDef(const char* tableName)
{
	Table *pTable;
	
	pTable = m_db->findTableByName(tableName);
	
	return  &(pTable->m_tableDesc->m_tableDef);		
}

long MdbSession::getTableRowCount(const char* tableName)
{
	Table *pTable;
	
	pTable = m_db->findTableByName(tableName);
	
	return pTable->m_tableDesc->m_recordNum;	
}	

// add by chenm 2009-5-18 全表逐行扫描
int MdbSession::selectRowByRow()
{
	Expression *pExpression;
	// 清除上一次的查询结果集
	this->clearQueryResults();
	
	if( m_cRowByRowExpr != NULL && strcmp(m_cRowByRowExpr,"")!=0)
		pExpression = m_expressionMgr.getExpre(m_cRowByRowExpr);	
		
	m_table = m_db->findTableByName(m_cRowByRowTableName);
	m_table->setRowCount(SELECT_ROW_BY_ROW_INTERNAL);
	while( m_table->select(m_cRowByRowIndexName,NULL,m_queryResults) != 0 )
	{
		// 进行其他查询条件判断
		if( m_cRowByRowExpr != NULL && strcmp(m_cRowByRowExpr,"")!=0)
		{
			for(int i=0;i<m_queryResults.size();++i)
			{
				if( pExpression->evaluate(m_queryResults[i],m_table->getRecordConvert(),(const void **)m_pRowByRowOtherParams->m_pValues) == false)
				{
					free(m_queryResults[i]);
					m_queryResults.erase(m_queryResults.begin()+i);
					--i;
				}
			}	
		}
		if(m_queryResults.size()>0)
		{
			return m_queryResults.size();
		}
		else
		{
			m_table->setRowCount(SELECT_ROW_BY_ROW_INTERNAL);
			this->clearQueryResults();	
		}
	}		
	return 0;
}

// add by chenm 不等值查询类 2009-6-3 15:35:54
int MdbSession::selectRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算 
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
									,const int iRowCount)                
{
	Expression *pExpression;
	
	// 清除上一次的查询结果集
	this->clearQueryResults();
	
	if( cExpr != NULL && strcmp(cExpr,"")!=0)
		pExpression = m_expressionMgr.getExpre(cExpr);
		
	if( m_isTransaction==false )
	{
		m_table = m_db->findTableByName(cTableName);
		
		m_isRowByRowSelect = false;
		if(iRowCount!=-1)
		{
			m_table->setRowCount(iRowCount);
			// add by chenm 2009-5-18 
			if(iRowCount == SELECT_ROW_BY_ROW)
			{
				m_cRowByRowExpr        = cExpr;
				m_pRowByRowOtherParams = pOtherParams;
				m_cRowByRowTableName   = cTableName;
				m_cRowByRowIndexName   = cIndexName;
				m_isRowByRowSelect     = true;					
			}
			// over 2009-5-18 
		}
				
		if( m_table->selectRange(cIndexName,pGtParams,pLtParams,iRangeTag,m_queryResults) == 0 )
			return 0;
			
		// 进行其他查询条件判断
		if( cExpr != NULL && strcmp(cExpr,"")!=0)
		{
			for(int i=0;i<m_queryResults.size();++i)
			{
				if( pExpression->evaluate(m_queryResults[i],m_table->getRecordConvert(),(const void **)pOtherParams->m_pValues) == false)
				{
					free(m_queryResults[i]);
					m_queryResults.erase(m_queryResults.begin()+i);
					--i;
				}
			}	
		}
		return m_queryResults.size();
	}
	else
	{
		// 只针对单表进行事务支持,表对象在MdbSession::begin的时候就得到了
		this->isAcrossTable(cTableName);
		if( m_table->selectRangeWithTrans(m_sessionID,cIndexName,pGtParams,pLtParams,iRangeTag,m_transQueryResults) == 0 )
			return 0;	

		TransData *pTransData = NULL;

		for(int i=0;i<m_transQueryResults.size();++i)
		{
			bool isMatch = false;
			//  如果TransDataMgr缓存中有该条记录,则用缓存中的记录代替内存数据库中查找到的记录
			if( (pTransData = m_transDataMgr.matchTransData(m_transQueryResults[i]) ) != NULL )
			{
				delete m_transQueryResults[i];
				if(pTransData->m_operType == '0')// 如果缓存中的记录,是被删除的记录,则在同一事务里,再select时,不再作为选中记录返回
				{
					m_transQueryResults.erase(m_transQueryResults.begin()+i);
					--i;
					continue;
				}
				else
				{
					m_transQueryResults[i] = pTransData;
					isMatch = true;	
				}
			}
			// 进行其他查询条件判断
			if( (cExpr != NULL && strcmp(cExpr,"")!=0)
				&&(pExpression->evaluate(m_transQueryResults[i]->m_oldRecordValue
				                      ,m_table->getRecordConvert()
				                      ,(const void **)pOtherParams->m_pValues) == false )
			   )
			{
				if( isMatch == false )
					delete m_transQueryResults[i];
				m_transQueryResults.erase(m_transQueryResults.begin()+i);
				--i;
			}
			else 
			{
				// 如果TransDataMgr缓存中没有该数据,则插入
				if( isMatch == false )
				{
					m_transDataMgr.insertTransData(m_transQueryResults[i]);	
				}	
			}	
		}
		return m_transQueryResults.size();
	}
}
					
int MdbSession::deleteRecRange(const char *cTableName
										,const char *cIndexName
										,const InputParams *pGtParams      //查询条件中的下限索引字段值
										,const InputParams *pLtParams      //查询条件中的上限索引字段值
										,const int iRangeTag               // 标示[]/[)/()/(]运算
										,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
										,const InputParams *pOtherParams)
{
	int iRowCount = this->selectRange(cTableName,cIndexName,pGtParams,pLtParams,iRangeTag,cExpr,pOtherParams);
	if( iRowCount!=0 )
	{
		int i;
		for(i=0;i<m_transQueryResults.size();++i)
		{
			m_transQueryResults[i]->m_operType = '0';				
		}
		return i;
	}
	else
	{
		return 0;
	}		
}

int MdbSession::updateRange(const char *cTableName
									,const char *cIndexName
									,const InputParams *pGtParams      //查询条件中的下限索引字段值
									,const InputParams *pLtParams      //查询条件中的上限索引字段值
									,const int iRangeTag               // 标示[]/[)/()/(]运算
									,vector<string> &sFieldNames       // 需要更新的字段名
									,const InputParams *pUpdateValues  // 需要更新的字段值
									,const char *cExpr                 //查询表达式,如：(msisdn=:0 and startTime <= :1)
									,const InputParams *pOtherParams)   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	this->isAcrossTable(cTableName);  
	
	if( this->selectRange(cTableName,cIndexName,pGtParams,pLtParams,iRangeTag,cExpr,pOtherParams) != 0)
	{
		int i;
		for(i=0;i<m_transQueryResults.size();++i)
		{
			m_transQueryResults[i]->cloneRecordValue();
			m_table->getRecordConvert()->setRecordValue(m_transQueryResults[i]->m_newRecordValue);
			m_table->getRecordConvert()->setColumnsValue(sFieldNames,pUpdateValues);
			m_transQueryResults[i]->m_operType = '2';				
		}
		return i;
	}
	else
	{
		return 0;
	}
}
 
// over 2009-6-3 15:36:00
//Added By Sunam 2009/8/31 21:37:17
//void MdbSession::dataBind(vector<int> &errVect)
//{
//    m_errVect = &errVect;
//	return;
//}
