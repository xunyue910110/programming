//  ############################################
//  Source file : SocketSession.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-4-23
//  Update      : 2008-4-23
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "SocketSession.h"
#include "Socket.h"
#include "Session.h"
#include "DataBase.h"
#include "Mdb_Exception.h"
#include "InputParams.h"
#include "PublicFunc.h"
#include "Table.h"
#include "Logger.h"
#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

SocketSession::SocketSession()
{
	m_session = NULL;
	m_db      = NULL;
	m_isActive= false;
	m_pWorkSocket = NULL;
	m_isInTransction = false;	
	m_pRowByRowOtherParams = NULL; // add by chenm 2009-7-17 17:18:28
	
	m_fucMap.clear();
	m_fucMap[SELECT_CMD      ] = &SocketSession::select;
	m_fucMap[UPDATE_CMD      ] = &SocketSession::update;
	m_fucMap[INSERT_CMD      ] = &SocketSession::insert;
	m_fucMap[DELETE_CMD      ] = &SocketSession::deleteRec;
	m_fucMap[QUIT_CMD        ] = &SocketSession::quit;
	m_fucMap[BEGIN_CMD       ] = &SocketSession::begin;
	m_fucMap[COMMIT_CMD      ] = &SocketSession::commit;
	m_fucMap[ROLLBACK_CMD    ] = &SocketSession::rollBack;
	m_fucMap[GETTABLEDESC_CMD] = &SocketSession::getTableDesc;
	m_fucMap[GETINDEXDESC_CMD] = &SocketSession::getIndexDesc;
	m_fucMap[GETALLTABLENAME_CMD] = &SocketSession::getAllTableName;	
	m_fucMap[GETALLINDEXNAME_CMD] = &SocketSession::getAllIndexName;	
	m_fucMap["selectw"] = &SocketSession::selectw;
	m_fucMap[GETTABLEROWCOUNTNAME_CMD] = &SocketSession::getTableRowCount;
	m_fucMap[GET_ROWBYROW_NEXTREC] = &SocketSession::getRowByRowNextRec; // add by chenm 2009-7-17 15:48:26
		
}

void SocketSession::setSocket(Socket * pSocket)
{
	m_pWorkSocket = pSocket;
	return;	
}

//##ModelId=483BB6C1001F
void SocketSession::run()
{
	char cRecvline[MAX_TRANSMISSION_BYTE_SIZE];  
	int  iReceiveBytes;

	// add by chenm 2009-3-5 增加打印客户端信息
	#ifdef _DEBUG_
	 writeServLog(CONNECT_LOG_TYPE);
	#endif
	// over
	
	while(m_isActive)
	{
		try
		{
			iReceiveBytes = m_pWorkSocket->read(cRecvline,1,MAX_TRANSMISSION_BYTE_SIZE) ; 				// 读查询命令
		    dealRequest(cRecvline,iReceiveBytes,this);
		}
		catch(Mdb_Exception &e)
		{
			// 客户端或服务端出错 均需回滚事务
			if(m_isInTransction == true)
			{
				this->rollBack(NULL,0);	
			}	
				
			m_isActive = false;
		   #ifdef _DEBUG_	
			e.toString();
		   #endif	
			break;
		}
	}
	// 本次socket连接结束,但本地内存数据库连接,没必要删除?
	try
	{
		#ifdef _DEBUG_
		 writeServLog(DISCONNECT_LOG_TYPE);
		#endif
		delete m_pWorkSocket;
	}
	catch(Mdb_Exception &e)
	{
		//theErrorMessages->insert(20010,e.GetString());   
	   #ifdef _DEBUG_	
		e.toString();
	   #endif
	}
	pthread_exit(NULL);
}

// 记录服务日志
void SocketSession::writeServLog(const int iLogType)
{
	struct tm* stNowTime;
	char tmp[15];
	time_t tNowTime;
	
	if(iLogType==CONNECT_LOG_TYPE)
	{
		try
		{
			m_pWorkSocket->get_peer_name(m_sClintName);
		}
		catch(Mdb_Exception &e)
		{
			m_sClintName = "error";
			theErrorMessages->insert(20010,e.GetString());   
		   #ifdef _DEBUG_	
			e.toString();
		   #endif
		}
	}
	time(&tNowTime);
	stNowTime = localtime(&tNowTime);
    memset(tmp,0,sizeof(tmp));
	sprintf(tmp,"%04d%02d%02d:%02d:%02d:%02d",stNowTime->tm_year + 1900,stNowTime->tm_mon+1,stNowTime->tm_mday,
									stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
	
	Logger *pLogger=Logger::getInstance();
	pLogger->setMsg(tmp,m_sClintName,pthread_self(),iLogType);

   #ifdef _DEBUG_ 
	cout<<"Thread:"<<pthread_self()<<" is accepting request from:"<<m_sClintName<<" at time:"<<tmp<<endl;
   #endif	

	return;	
}

//##ModelId=483BC1490196
bool SocketSession::end()
{
	// 因为是会话池,所以,只有退出时,才断开和本地mdb的连接
	this->disconnect();
	delete m_db;
	m_isActive = false;
	
	return true;
}

//##ModelId=483BC225031C
bool SocketSession::isActive()
{
	return m_isActive;
}

//##ModelId=483BBC570186
void SocketSession::disconnect()
{
	//add by chenyong --2009/2/15
	//--------------------------------------------------
	rollBack();
	//--------------------------------------------------
	m_db->disconnect(m_session);
	m_isActive = false;
}

//##ModelId=483BBC5E0157
void SocketSession::connect(const char *cDBName)
{
	try
	{
		m_db = new DataBase(cDBName);
		m_session = m_db->connect();
		
		m_isActive = true;
	}
	catch(Mdb_Exception &e)
	{
		if( m_db != NULL )
		{
			m_db->disconnect(m_session);
			delete m_db;
		}
		m_isActive = false;
		throw e;	
	}
}

void SocketSession::setActive()
{
	m_isActive = true;
	return;	
}

// 以下为处理客户端请求的方法
// 前TRANS_FIELDS_LENTH_BYTES个字节记录了命令长度
// TRANS_FIELDS_LENTH_BYTES+TRANS_FIELDS_LENTH_BYTES的值后为数值，格式为
// fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
// 不同的命令其fieldNum不同，各个字段的含义也不一样，对于某些命令某些字段可能多重嵌套
// 由各个命令自己解析。
// 例如:0006(select)0005000B(BILL_USER_0)000F(IDX_BILL_USER_0)000C(00010008(xxxxxxxx))0007(express)000A(00010006param1)
void SocketSession::dealRequest(char *cRecvline,const int iReceiveBytes,SocketSession *pSocketSession)
{
	int length=0;
	char cmd[20];
	memcpy((char*)&length+PublicFunc::m_offSet,cRecvline,TRANS_FIELDS_LENTH_BYTES);
	
	if(length!=iReceiveBytes)
	{
		memset(cmd,0,sizeof(cmd));
		memcpy(cmd,cRecvline+TRANS_FIELDS_LENTH_BYTES,length);

		FUNC_MAP_ITR itr = m_fucMap.find(cmd);
		if( itr!= m_fucMap.end() )
		{
			(pSocketSession->*(itr->second))(cRecvline+TRANS_FIELDS_LENTH_BYTES+length,iReceiveBytes-TRANS_FIELDS_LENTH_BYTES-length);
		}
		else
		{
			write2Client("unknowed command!");
		}	
	}
	else
	{
		write2Client("unknowed command!");	
	}
}

void SocketSession::select(const char *cRecvline,int iStrLenth)
{
	void** pValues;
	int iFieldCount;
	InputParams *pIndexKeyParams=NULL;
	InputParams *pOtherParams=NULL;
	
	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=5 
			&& iFieldCount!=6) // add by chenm 2009-7-17 15:06:28 for rowbyrow select
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		// iRowCount by chenm 2009-7-17 15:07:34
		int iRowCount=-1;
		if(iFieldCount==6)
		{
			memcpy(&iRowCount,pValues[5],sizeof(int));	
		}
		if(iRowCount == SELECT_ROW_BY_ROW)
		{
			strcpy(m_cRowByRowTableName,(char *)pValues[0]);
			if(pValues[1]!=NULL)
			{
				strcpy(m_cRowByRowIndexName,(char *)pValues[1]);			
			}
			else
			{
				memset(m_cRowByRowIndexName,0,sizeof(m_cRowByRowIndexName));	
			}
			if(pValues[3]!=NULL)
			{
				strcpy(m_cRowByRowExpr,(char *)pValues[3]);
			}
			else
			{
				memset(m_cRowByRowExpr,0,sizeof(m_cRowByRowExpr));	
			}
			
			// 条件的字段值
			if(m_pRowByRowOtherParams!=NULL)
			{
				delete m_pRowByRowOtherParams;	
			}
			PublicFunc::splitStr(pValues[4],m_pRowByRowOtherParams);
			m_session->select(m_cRowByRowTableName
			                       ,m_cRowByRowIndexName
			                       ,NULL
			                       ,m_cRowByRowExpr
			                       ,m_pRowByRowOtherParams
			                       ,iRowCount);		
		}
		else
		{
			// 查询条件中的索引字段值
			PublicFunc::splitStr(pValues[2],pIndexKeyParams);

			// 条件的字段值
			PublicFunc::splitStr(pValues[4],pOtherParams);	
			m_session->select((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,(char *)pValues[3],pOtherParams,iRowCount);		
		}
		// over 2009-7-17 15:08:49

		
		int iColumnLenth=0,i,iOffSize=0,recordNum=0,length;	
		char cSentLine[MAX_TRANSMISSION_BUFFER];
		char *pSentLine = cSentLine+10;	
		int offSetRecordNum,offSetRecordLength,offSetColumnNum,offSetColumnLength;
		
		//留下记录条数的位置
		offSetRecordNum = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		if( iRowCount==SELECT_ROW_BY_ROW ) // add by chenm 2009-7-17 15:27:02
		{
			// rowbyrow 逐条返回给客户端
			if( m_session->getNextRec() )
			{
				//留下记录长度的位置
				offSetRecordLength = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				i = 0;
				//留下字段数的位置
				offSetColumnNum = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				//留下字段长段位置
				offSetColumnLength = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				while( m_session->getColumnValue(i,pSentLine+iOffSize,iColumnLenth) )
				{	
					//当前字段长段
					memcpy(pSentLine+offSetColumnLength,(char *)&iColumnLenth + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
					iOffSize += iColumnLenth;
					//留下字段长段位置
					offSetColumnLength = iOffSize;
					iOffSize += TRANS_FIELDS_LENTH_BYTES;
					++i;						
				}
				//减掉多加的一个长度
				iOffSize -= TRANS_FIELDS_LENTH_BYTES;
				//当前录字段数
				memcpy(pSentLine+offSetColumnNum,(char *)&i + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
				
				if(iOffSize > MAX_TRANSMISSION_BUFFER-10)
				{
					throw Mdb_Exception(__FILE__, __LINE__, "transfer data size:%d %s",iOffSize," bigger than the Protocal's defined!");	
				}
				recordNum ++;
				
				//记录长度
				length = iOffSize - offSetRecordLength - TRANS_FIELDS_LENTH_BYTES;
				memcpy(pSentLine+offSetRecordLength,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
			}			
		}
		else
		{ // over 2009-7-17 15:27:07
			while( m_session->getNextRec() )
			{
				//留下记录长度的位置
				offSetRecordLength = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				i = 0;
				//留下字段数的位置
				offSetColumnNum = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				//留下字段长段位置
				offSetColumnLength = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				while( m_session->getColumnValue(i,pSentLine+iOffSize,iColumnLenth) )
				{	
					//当前字段长段
					memcpy(pSentLine+offSetColumnLength,(char *)&iColumnLenth + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
					iOffSize += iColumnLenth;
					//留下字段长段位置
					offSetColumnLength = iOffSize;
					iOffSize += TRANS_FIELDS_LENTH_BYTES;
					++i;						
				}
				//减掉多加的一个长度
				iOffSize -= TRANS_FIELDS_LENTH_BYTES;
				//当前录字段数
				memcpy(pSentLine+offSetColumnNum,(char *)&i + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
				
				if(iOffSize > MAX_TRANSMISSION_BUFFER-10)
				{
					throw Mdb_Exception(__FILE__, __LINE__, "transfer data size:%d %s",iOffSize," bigger than the Protocal's defined!");	
				}
				recordNum ++;
				
				//记录长度
				length = iOffSize - offSetRecordLength - TRANS_FIELDS_LENTH_BYTES;
				memcpy(pSentLine+offSetRecordLength,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
			}			
		}
		
		//记录数
		memcpy(pSentLine+offSetRecordNum,(char *)&recordNum + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
		
		sprintf(cSentLine,"%9d",iOffSize);
		m_pWorkSocket->write(cSentLine,iOffSize+10);		
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());
			// 释放解析过程申请的资源
		for(int i=0;i<iFieldCount;++i)
			free(pValues[i]);
		delete pValues;
		
		if(pIndexKeyParams!=NULL)
			delete pIndexKeyParams;
		if(pOtherParams!=NULL)
			delete pOtherParams;	
	}
	
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
	
	if(pIndexKeyParams!=NULL)
		delete pIndexKeyParams;
	if(pOtherParams!=NULL)
		delete pOtherParams;
	
}

void SocketSession::deleteRec(const char *cRecvline,int iStrLenth)
{
	void ** pValues;
	int iFieldCount,iResult;
	// 假定最多输入的相同数据集只有MAX_COLUMN_NUM个分割
	int iFieldPosition[MAX_COLUMN_NUM];
	InputParams *pIndexKeyParams;
	InputParams *pOtherParams;
	
	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=5 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		// 删除条件中的索引字段值
		PublicFunc::splitStr(pValues[2],pIndexKeyParams);

		// 条件的字段值
		PublicFunc::splitStr(pValues[4],pOtherParams);
			
		iResult = m_session->deleteRec((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,(char *)pValues[3],pOtherParams);		
		
		write2Client( &iResult,sizeof(iResult) );	
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
	
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
	
	delete pIndexKeyParams;
	delete pOtherParams;	
}

void SocketSession::insert(const char *cRecvline,int iStrLenth)
{
	void ** pValues;
	int iFieldCount;
	// 假定最多输入的相同数据集只有MAX_COLUMN_NUM个分割
	int iFieldPosition[MAX_COLUMN_NUM];
	InputParams *pInputParams;

	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=2 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		// 删除条件中的索引字段值
		PublicFunc::splitStr(pValues[1],pInputParams);

		m_session->insert((const char *)pValues[0],pInputParams);		
		write2Client( (void*)SUCCESS,sizeof(SUCCESS) );
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
	
	delete pInputParams;
}

void SocketSession::update(const char *cRecvline,int iStrLenth)
{
	void** pValues;
	InputParams *pIndexKeyParams;
	
	vector<InputParams *> pUpdateValues;
	vector<InputParams *> pOtherParams;
		
	vector<string> sFieldNames;            // 需要更新的字段名
	int iFieldCount,iResult;

	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=7 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			

		// 查询条件中的索引字段值
		PublicFunc::splitStr(pValues[2],pIndexKeyParams);

		// 需要更新的字段名
		PublicFunc::splitStr((char *)pValues[3],sFieldNames);
			
		// 需要更新的字段值
		PublicFunc::splitStr(pValues[4],pUpdateValues);
        
		// 条件的字段值
		PublicFunc::splitStr(pValues[6],pOtherParams);
		
		if (pUpdateValues.size() > 1)
		{
			iResult = m_session->update((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,sFieldNames,pUpdateValues,(char *)pValues[5],pOtherParams);
		}
		else
		{
			iResult = m_session->update((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,sFieldNames,pUpdateValues[0],(char *)pValues[5],pOtherParams[0]);
		}
		write2Client( &iResult,sizeof(iResult) );	
	}
	catch(Mdb_Exception &e)
	{
		write2Client( e.GetString() );	
	}

	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
	delete pIndexKeyParams;
	for(int i=0;i<pUpdateValues.size();++i)
	{
		delete pUpdateValues[i];
		delete pOtherParams[i];
	}
}

void SocketSession::quit(const char *cRecvline,int iStrLenth)
{
	//add by chenyong --2009/2/15
	//-----------------------------------------------------------------------
	rollBack();
	//-----------------------------------------------------------------------
	m_isActive = false;	
	write2Client( (void *)SUCCESS,sizeof(SUCCESS) );
}

void SocketSession::begin(const char *cRecvline,int iStrLenth)
{
	void** pValues;
	// 假定最多输入的相同数据集只有MAX_COLUMN_NUM个分割
	int iFieldPosition[MAX_COLUMN_NUM],iFieldCount;

	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=2 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		m_isInTransction = true;
		m_session->begin((char *)pValues[0],*((int *)pValues[1]));		
		write2Client( (void *)SUCCESS,sizeof(SUCCESS) );
	}
	catch(Mdb_Exception &e)
	{
		m_isInTransction = false;
		write2Client(e.GetString());	
	}
	
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
}

void SocketSession::commit(const char *cRecvline,int iStrLenth)
{
	try
	{
		m_session->commit();	
		write2Client( (void *)SUCCESS,sizeof(SUCCESS) );	
		m_isInTransction = false;
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
}

void SocketSession::getTableDesc(const char *cRecvline,int iStrLenth)
{
	void **pValues;
	int iFieldCount;
	char tableDesc[MAX_TRANSMISSION_BUFFER];
	int length;
	int iOffSize = 0;
	try
	{
		Table *table;
		
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=1 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
		
		table = m_db->findTableByName((char *)pValues[0]);
		
		length = (table->m_tableDesc->m_tableDef).m_columnNum*3;
		memcpy(tableDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for (int i=0;i<(table->m_tableDesc->m_tableDef).m_columnNum;++i)
		{
			// 字段名
			length = strlen((table->m_tableDesc->m_tableDef).m_columnList[i].m_name);
			memcpy(tableDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(tableDesc+iOffSize,&((table->m_tableDesc->m_tableDef).m_columnList[i].m_name),length);
			iOffSize += length;
			
			// 字段数据类型
			length = sizeof((table->m_tableDesc->m_tableDef).m_columnList[i].m_type);
			memcpy(tableDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(tableDesc+iOffSize,&((table->m_tableDesc->m_tableDef).m_columnList[i].m_type),length);
			iOffSize += length;
			
			// 字段数据长度
			length = sizeof((table->m_tableDesc->m_tableDef).m_columnList[i].m_len);
			memcpy(tableDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(tableDesc+iOffSize,&((table->m_tableDesc->m_tableDef).m_columnList[i].m_len),length);
			iOffSize += length;
		}
		write2Client( tableDesc, iOffSize);	
		m_isInTransction = false;
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
}

void SocketSession::getIndexDesc(const char *cRecvline,int iStrLenth)
{
	void **pValues;
	int iFieldCount;
	char indexDesc[MAX_TRANSMISSION_BUFFER];
	int length;
	int iOffSize = 0;
	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=1 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
		
		const IndexDef *pIndexDef = m_session->getIndexDef(NULL,(char *)pValues[0]);
		
		length = pIndexDef->m_columnNum;
		memcpy(indexDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for (int i=0;i<pIndexDef->m_columnNum;++i)
		{
			length = strlen(pIndexDef->m_columnList[i]);
			memcpy(indexDesc+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(indexDesc+iOffSize,pIndexDef->m_columnList[i],length);
			iOffSize += length;
		}
		write2Client( indexDesc, iOffSize);	
		m_isInTransction = false;
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
}

void SocketSession::rollBack(const char *cRecvline,int iStrLenth)
{
	try
	{
		m_session->rollBack();	
		write2Client( (void *)SUCCESS,sizeof(SUCCESS) );	
		m_isInTransction = false;
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
}

//add by chenyong --2009/2/15号
//------------------------------------------------------------------
void SocketSession::rollBack()
{
	try
	{
		//if (m_isInTransction==true && m_session!=NULL) modified by chenm 2009-5-23
		if(m_session!=NULL)
		{
			m_session->rollBack();	
			m_isInTransction = false;
		}
	}
	catch(Mdb_Exception &e)
	{
		e.GetString();
	}
}
//------------------------------------------------------------------

// 为远程客户端管理台而增加 by chenm 2009-1-7 22:11:41
void SocketSession::getAllTableName(const char *cRecvline,int iStrLenth)
{
	char cSentLine[MAX_TRANSMISSION_BUFFER];
	vector<TableDef>    tableDefList;
	int length;
	int iOffSize = 0;
	
	try
	{
		m_db->getMemMgrInstance()->getTableDefList(tableDefList);
		// 传回消息的字段数
		length = tableDefList.size();
		memcpy(cSentLine+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for (int i=0;i<tableDefList.size();++i)
		{
			length = strlen(tableDefList[i].m_tableName);
			memcpy(cSentLine+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(cSentLine+iOffSize,tableDefList[i].m_tableName,length);
			iOffSize += length;
		}
		write2Client( cSentLine, iOffSize);	
		m_isInTransction = false;	
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
}

// 为远程客户端管理台而增加 by chenm 2009-1-7 22:47:50
void SocketSession::getAllIndexName(const char *cRecvline,int iStrLenth)
{
	char cSentLine[MAX_TRANSMISSION_BUFFER];
	vector<IndexDef>    indexDefList,tmpIndexDefList;
	vector<TableDef>    tableDefList;
	int length;
	int iOffSize = 0;
	
	try
	{
		m_db->getMemMgrInstance()->getTableDefList(tableDefList);
		for(vector<TableDef>::iterator itr=tableDefList.begin();
                    itr!=tableDefList.end();itr++)
		{
		    m_db->getMemMgrInstance()->getIndexListByTable(itr->m_tableName, tmpIndexDefList);
		    for(vector<IndexDef>::iterator itr = tmpIndexDefList.begin();
		        itr != tmpIndexDefList.end(); itr++)
		    {
		        indexDefList.push_back(*itr);                    
		    }
		
		}
		// 传回消息的字段数
		length = indexDefList.size()*2;
		memcpy(cSentLine+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for (int i=0;i<indexDefList.size();++i)
		{
			length = strlen(indexDefList[i].m_indexName);
			memcpy(cSentLine+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(cSentLine+iOffSize,indexDefList[i].m_indexName,length);
			iOffSize += length;
			
			length = strlen(indexDefList[i].m_tableName);
			memcpy(cSentLine+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			memcpy(cSentLine+iOffSize,indexDefList[i].m_tableName,length);
			iOffSize += length;
		}
		write2Client( cSentLine, iOffSize);	
		m_isInTransction = false;	
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());	
	}
}
void SocketSession::write2Client(const void *cMsg,size_t iSendSize)
{
	char cSentLine[MAX_TRANSMISSION_BUFFER];

	try
	{
		char cLenth[10+1];
		memset(cLenth,0,11);
		
		// iSendSize为0的，是出错的情况
		if(iSendSize == 0)
		{
			iSendSize = strlen((char *)cMsg);
			// 和客户端约定,如果返回长度为-1,则表示有异常
			sprintf(cLenth,"%ld",-1);
			memcpy(cSentLine,cLenth,10);	
		}
		else
		{
			// 追加长度信息
			sprintf(cLenth,"%ld",iSendSize);
			memcpy(cSentLine,cLenth,10);			
		}	
		memcpy(cSentLine+10,cMsg,iSendSize);
		m_pWorkSocket->write(cSentLine,iSendSize+10);		
	}	
	catch(Mdb_Exception &e) 
	{
		// 如果写入还是错误,则退出线程
		//add by chenyong --2009/2/15
		//------------------------------------------------------------
		rollBack();
		//------------------------------------------------------------
		m_isActive = false;	
		// TODO:如果需要写入server的日志...
	}
}

void SocketSession::selectw(const char *cRecvline,int iStrLenth)
{
	void** pValues;
	int iFieldCount;
	InputParams *pIndexKeyParams;
	InputParams *pOtherParams;
	
	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=5 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		// 查询条件中的索引字段值
		PublicFunc::splitStr(pValues[2],pIndexKeyParams);

		// 条件的字段值
		PublicFunc::splitStr(pValues[4],pOtherParams);

		if(pIndexKeyParams==NULL 
			&&pValues[3]==NULL)
		{
			m_session->select((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,(char *)pValues[3],pOtherParams,50);		
		}
		else
		{
			m_session->select((char *)pValues[0],(char *)pValues[1],pIndexKeyParams,(char *)pValues[3],pOtherParams);
		}		
		
		int iColumnLenth=0,i,iOffSize=0,recordNum=0,length;	
		char cSentLine[MAX_TRANSMISSION_BUFFER];
		char *pSentLine = cSentLine+10;	
		int offSetRecordNum,offSetRecordLength,offSetColumnNum,offSetColumnLength;
		
		//留下记录条数的位置
		offSetRecordNum = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		while( m_session->getNextRec() )
		{
			//留下记录长度的位置
			offSetRecordLength = iOffSize;
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			i = 0;
			//留下字段数的位置
			offSetColumnNum = iOffSize;
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			//留下字段长段位置
			offSetColumnLength = iOffSize;
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			while( m_session->getColumnValue(i,pSentLine+iOffSize,iColumnLenth) )
			{	
				//当前字段长段
				memcpy(pSentLine+offSetColumnLength,(char *)&iColumnLenth + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
				iOffSize += iColumnLenth;
				//留下字段长段位置
				offSetColumnLength = iOffSize;
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				++i;						
			}
			//减掉多加的一个长度
			iOffSize -= TRANS_FIELDS_LENTH_BYTES;
			//当前录字段数
			memcpy(pSentLine+offSetColumnNum,(char *)&i + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
			
			//记录长度
			length = iOffSize - offSetRecordLength - TRANS_FIELDS_LENTH_BYTES;
			memcpy(pSentLine+offSetRecordLength,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 

			if(iOffSize > MAX_TRANSMISSION_BUFFER-200)
			{
				break;	
			}
			recordNum ++;			
		}
		//记录数
		memcpy(pSentLine+offSetRecordNum,(char *)&recordNum + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
		
		sprintf(cSentLine,"%9d",iOffSize);
		m_pWorkSocket->write(cSentLine,iOffSize+10);		
	}
	catch(Mdb_Exception &e)
	{
		write2Client(e.GetString());
			// 释放解析过程申请的资源
		for(int i=0;i<iFieldCount;++i)
			free(pValues[i]);
		delete pValues;
		
		delete pIndexKeyParams;
		delete pOtherParams;	
	}
	
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;
	
	delete pIndexKeyParams;
	delete pOtherParams;
	
}

void SocketSession::getTableRowCount(const char *cRecvline,int iStrLenth)
{
	void** pValues;
	// 假定最多输入的相同数据集只有MAX_COLUMN_NUM个分割
	int iFieldPosition[MAX_COLUMN_NUM],iFieldCount;
	long lResult=0;

	try
	{
		iFieldCount = PublicFunc::splitStr(cRecvline,pValues);
		
		if( iFieldCount!=1 )
			throw Mdb_Exception(__FILE__, __LINE__, "传入字段数有误!");	
			
		lResult = m_session->getTableRowCount((char *)pValues[0]);		
		write2Client( &lResult,sizeof(lResult) );
	}
	catch(Mdb_Exception &e)
	{
		m_isInTransction = false;
		write2Client(e.GetString());	
	}
	
	// 释放解析过程申请的资源
	for(int i=0;i<iFieldCount;++i)
		free(pValues[i]);
	delete pValues;	
}

// add by chenm 2009-7-17 15:49:02
void SocketSession::getRowByRowNextRec(const char *cRecvline,int iStrLenth)
{
	int iColumnLenth=0,i,iOffSize=0,recordNum=0,length;	
	char cSentLine[MAX_TRANSMISSION_BUFFER];
	char *pSentLine = cSentLine+10;	
	int offSetRecordNum,offSetRecordLength,offSetColumnNum,offSetColumnLength;
	
	//留下记录条数的位置
	offSetRecordNum = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;

	if( m_session->getNextRec() )
	{
		//留下记录长度的位置
		offSetRecordLength = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		i = 0;
		//留下字段数的位置
		offSetColumnNum = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		//留下字段长段位置
		offSetColumnLength = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		while( m_session->getColumnValue(i,pSentLine+iOffSize,iColumnLenth) )
		{	
			//当前字段长段
			memcpy(pSentLine+offSetColumnLength,(char *)&iColumnLenth + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += iColumnLenth;
			//留下字段长段位置
			offSetColumnLength = iOffSize;
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			++i;						
		}
		//减掉多加的一个长度
		iOffSize -= TRANS_FIELDS_LENTH_BYTES;
		//当前录字段数
		memcpy(pSentLine+offSetColumnNum,(char *)&i + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
		
		if(iOffSize > MAX_TRANSMISSION_BUFFER-10)
		{
			throw Mdb_Exception(__FILE__, __LINE__, "transfer data size:%d %s",iOffSize," bigger than the Protocal's defined!");	
		}
		recordNum ++;
		
		//记录长度
		length = iOffSize - offSetRecordLength - TRANS_FIELDS_LENTH_BYTES;
		memcpy(pSentLine+offSetRecordLength,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
	}
	//记录数
	memcpy(pSentLine+offSetRecordNum,(char *)&recordNum + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES); 
	
	sprintf(cSentLine,"%9d",iOffSize);
	m_pWorkSocket->write(cSentLine,iOffSize+10);				
}
// over 2009-7-17 15:49:10
