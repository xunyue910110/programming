// ############################################
// Source file : RemoteMdbSession.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

#include "RemoteMdbSession.h"
#include "InputParams.h"
#include "Socket.h"
#include "Mdb_Exception.h"
#include "PublicFunc.h"

RemoteMdbSession::~RemoteMdbSession()
{
	// 这里无须delete,在RemoteDB的disconnect里删除
	//delete m_pWorkSocket;
	TABLE_DEF_MAP_ITR itr;
	for(itr = m_tableDefs.begin(); itr != m_tableDefs.end(); ++itr)
	{
		delete itr->second;
	}
	m_tableDefs.clear();
	
	INDEX_DEF_MAP_ITR itr1;
	for(itr1 = m_indexDefs.begin(); itr1 != m_indexDefs.end(); ++itr1)
	{
		delete itr1->second;
	}
	m_indexDefs.clear();
	clearIndex2TableMap();
}

RemoteMdbSession::RemoteMdbSession(Socket *pSocket)
{
	if(pSocket == NULL)
		throw Mdb_Exception(__FILE__, __LINE__, "传入的套接字指针为空!");
		
	char tmp[MAX_TRANSMISSION_BYTE_SIZE+2];
	memset(tmp,0,sizeof(tmp));                  
	int iReadSize = pSocket->read(tmp,1,MAX_TRANSMISSION_BYTE_SIZE);      // 读取连接状态
	if(iReadSize!=sizeof(SUCCESS))
		throw Mdb_Exception(__FILE__, __LINE__, "%s",tmp);		
		
	m_pWorkSocket = pSocket;
	m_pRecords = NULL;
	m_pColumnValues = NULL;
	m_iValuesLenth  = NULL; // add by chenm 2009-7-9 15:48:52
	m_iColumnCount  = 0;
	m_iResultSequnce = 0;
	m_iResultsCount  = 0;	
	m_isRowByRowSelect = false; // add by chenm 2009-7-17 15:03:56
}


int RemoteMdbSession::select(const char *cTableName
						,const char *cIndexName
						,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
						,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
						,const InputParams *pOtherParams
						,const int iRowCount)     
{
	getTableDef(cTableName);
	
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length,tmpOffSize,tmpOffSize1;
	
	//清除查询结果
	clearQueryResults();
	
	length = strlen(SELECT_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,SELECT_CMD,strlen(SELECT_CMD));
	iOffSize += strlen(SELECT_CMD);
	
	//字段数
	length = 6;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	length = strlen(cIndexName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cIndexName,strlen(cIndexName));
	iOffSize += strlen(cIndexName);
	
	//留下indexKeyParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pIndexKeyParams!=NULL)
	{
		//indexKeyParams叁数个数
		length = pIndexKeyParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pIndexKeyParams->m_iNum;++i)
		{
			//第i个参数长度
			length = pIndexKeyParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pIndexKeyParams->m_pValues[i],pIndexKeyParams->m_iValueLenths[i]);	
			iOffSize += pIndexKeyParams->m_iValueLenths[i];
		}
	}
	else
	{
		//indexKeyParams叁数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//indexKeyParams叁数总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	if(cExpr!=NULL)
	{
		//表达式长度
		length = strlen(cExpr);
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//表达式内容
		memcpy(cmd+iOffSize,cExpr,strlen(cExpr));	
		iOffSize += strlen(cExpr);
	}
	else
	{
		//表达式长度
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//留下pOtherParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pOtherParams!=NULL)
	{
		//pOtherParams参数个数
		length = pOtherParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pOtherParams->m_iNum;++i)
		{
			//第i个值长度
			length = pOtherParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pOtherParams->m_pValues[i],pOtherParams->m_iValueLenths[i]);	
			iOffSize += pOtherParams->m_iValueLenths[i];
		}
	}
	else
	{
		//pOtherParams参数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	}
	
	//pOtherParams总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		
	// add by chenm 2009-7-17 14:53:45 for rowbyrow
	length = sizeof(int);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,&iRowCount,length);
	iOffSize += length;

	if(iRowCount==SELECT_ROW_BY_ROW)
	{
		m_isRowByRowSelect = true;	
	}
	else
	{
		m_isRowByRowSelect = false;
	}
	// over 2009-7-17 14:54:29
	
	excuteCmd(cmd,iOffSize);
	
	m_iResultsCount = PublicFunc::splitStr(m_recLine,m_pRecords);
	
	strcpy(m_queryTableName.m_pStr,cTableName);
	//getTableDef(cTableName);
		
	return m_iResultsCount;
}
				
int  RemoteMdbSession::deleteRec(const char *cTableName
				,const char *cIndexName
				,const InputParams *pIndexKeyParams
				,const char *cExpr             //查询表达式,如：(msisdn=:1 and startTime <= :2)
				,const InputParams *pOtherParams)   
{
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length,tmpOffSize,tmpOffSize1;
	
	length = strlen(DELETE_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,DELETE_CMD,strlen(DELETE_CMD));
	iOffSize += strlen(DELETE_CMD);
	
	//字段数
	length = 5;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	length = strlen(cIndexName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cIndexName,strlen(cIndexName));
	iOffSize += strlen(cIndexName);
	
	//留下indexKeyParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pIndexKeyParams!=NULL)
	{
		//indexKeyParams叁数个数
		length = pIndexKeyParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pIndexKeyParams->m_iNum;++i)
		{
			//第i个参数长度
			length = pIndexKeyParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pIndexKeyParams->m_pValues[i],pIndexKeyParams->m_iValueLenths[i]);	
			iOffSize += pIndexKeyParams->m_iValueLenths[i];
		}
	}
	else
	{
		//indexKeyParams叁数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//indexKeyParams叁数总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	if(cExpr!=NULL)
	{
		//表达式长度
		length = strlen(cExpr);
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//表达式内容
		memcpy(cmd+iOffSize,cExpr,strlen(cExpr));	
		iOffSize += strlen(cExpr);
	}
	else
	{
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//留下pOtherParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pOtherParams!=NULL)
	{
		//pOtherParams参数个数
		length = pOtherParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pOtherParams->m_iNum;++i)
		{
			//第i个值长度
			length = pOtherParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pOtherParams->m_pValues[i],pOtherParams->m_iValueLenths[i]);	
			iOffSize += pOtherParams->m_iValueLenths[i];
		}
	}
	else
	{
		//pOtherParams参数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//pOtherParams总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	excuteCmd(cmd,iOffSize);

	int iResult;
	memcpy(&iResult,m_recLine,sizeof(iResult));
	
	return iResult;	
}

//Modified By Sunam 2009/11/4 20:02:00
//bool RemoteMdbSession::insert(const char *cTableName,const vector<InputParams *>&pInputParams)
bool RemoteMdbSession::insert(const char *cTableName,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	for (int i=0;i<pInputParams.size();i++)
	{
		if (!insert(cTableName,pInputParams[i]))
			return false;
	}
	return true;
}

//Modified By Sunam 2009/11/4 20:02:00
//bool RemoteMdbSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams)
bool RemoteMdbSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	for (int i=0;i<pInputParams.size();i++)
	{
		if (!insert(cTableName,cFieldNames,pInputParams[i]))
			return false;
	}
	return true;
}

bool RemoteMdbSession::insert(const char *cTableName
	    		,const InputParams *pInputParams)   
{
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length,tmpOffSize,tmpOffSize1;
	
	length = strlen(INSERT_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,INSERT_CMD,strlen(INSERT_CMD));
	iOffSize += strlen(INSERT_CMD);
	
	//字段数
	length = 2;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	//留下pInputParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pInputParams!=NULL)
	{
		//pInputParams叁数个数
		length = pInputParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pInputParams->m_iNum;++i)
		{
			//第i个参数长度
			length = pInputParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pInputParams->m_pValues[i],pInputParams->m_iValueLenths[i]);	
			iOffSize += pInputParams->m_iValueLenths[i];
		}
	}
	else
	{
		//pInputParams叁数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//pInputParams叁数总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		
	excuteCmd(cmd,iOffSize);
	return true;	
}

bool RemoteMdbSession::insert(const char *cTableName
			    		,const vector<string> &cFieldNames     // 指定插入字段顺序
			    		,const InputParams *pInputParams)      // 需要插入的记录分字段内容
{
	MyString strTableName;
	TABLE_COLOMN_POS_MAP_ITR tableColumnMapItr;
	
	// 查找表描述相关信息
	strcpy(strTableName.m_pStr,cTableName);
	tableColumnMapItr = m_tableColumnMap.find(strTableName);
	
	if( tableColumnMapItr==m_tableColumnMap.end() )
	{
		this->getTableDef(cTableName);	
		tableColumnMapItr = m_tableColumnMap.find(strTableName);
	}
	
	InputParams *pInParams = new InputParams(pInputParams->m_iNum);
	
	try
	{
		COLOMN_POS_MAP_ITR itr;
		for(int i=0;i<cFieldNames.size();++i)
		{
			MyString strColumnName;	
			strcpy(strColumnName.m_pStr,cFieldNames[i].c_str());
			
			itr = (tableColumnMapItr->second).find(strColumnName);
			
			// 按照表里的字段顺序,构造新的传入字段值
			if(itr != (tableColumnMapItr->second).end() )
			{
				pInParams->m_iValueLenths[itr->second] = pInputParams->m_iValueLenths[i];
				pInParams->m_pValues[itr->second] = malloc(pInputParams->m_iValueLenths[i]);
				memset(pInParams->m_pValues[itr->second],0,pInputParams->m_iValueLenths[i]);
				memcpy(pInParams->m_pValues[itr->second],pInputParams->m_pValues[i],pInputParams->m_iValueLenths[i]);
			}
			else
			{
				throw Mdb_Exception(__FILE__, __LINE__,"传入的字段名:%s 在表:%s 里没有定义!",cFieldNames[i].c_str(),cTableName);	
			}
		}
	
		this->insert(cTableName,pInParams);
	}
	catch(Mdb_Exception e)
	{
		delete 	pInParams;
		throw Mdb_Exception(__FILE__, __LINE__,e.GetString());
	} 
	delete 	pInParams;
	return true;
}

int RemoteMdbSession::update(const char *cTableName
				,const char *cIndexName
				,const InputParams *pIndexKeyParams
				,vector<string> &sFieldNames            // 需要更新的字段名
				,const InputParams *pUpdateValues          // 需要更新的字段值
				,const char *cExpr             //查询表达式,如：(msisdn=:1 and startTime <= :2)
				,const InputParams *pOtherParams)   
{
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length,tmpOffSize,tmpOffSize1;
	
	length = strlen(UPDATE_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,UPDATE_CMD,strlen(UPDATE_CMD));
	iOffSize += strlen(UPDATE_CMD);
	
	//字段数
	length = 7;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	length = strlen(cIndexName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cIndexName,strlen(cIndexName));
	iOffSize += strlen(cIndexName);
	
	//留下indexKeyParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pIndexKeyParams!=NULL)
	{
		//indexKeyParams叁数个数
		length = pIndexKeyParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pIndexKeyParams->m_iNum;++i)
		{
			//第i个参数长度
			length = pIndexKeyParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pIndexKeyParams->m_pValues[i],pIndexKeyParams->m_iValueLenths[i]);	
			iOffSize += pIndexKeyParams->m_iValueLenths[i];
		}
	}
	else
	{
		//indexKeyParams叁数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//indexKeyParams叁数总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	//留下fieldNames总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//需要更新的字段个数
	length = sFieldNames.size();
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	for(int i=0;i<sFieldNames.size();++i)
	{
		//第i个字段长度
		length = sFieldNames[i].length();
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//第i个字段值
		memcpy(cmd+iOffSize,sFieldNames[i].c_str(),sFieldNames[i].length());	
		iOffSize += sFieldNames[i].length();
	}
	//fieldNames总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);

	//留下updateValues总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//更新单条记录
	length = 1;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//记录总长度的位置
	tmpOffSize1 = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pUpdateValues!=NULL)
	{
		//记录的字段数
		length = pUpdateValues->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pUpdateValues->m_iNum;++i)
		{
			//记录第i个值长度
			length = pUpdateValues->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pUpdateValues->m_pValues[i],pUpdateValues->m_iValueLenths[i]);	
			iOffSize += pUpdateValues->m_iValueLenths[i];
		}
	}
	else
	{
		//记录的字段数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//记录总长度
	length = iOffSize - tmpOffSize1 - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize1,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);

	//updateValues总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		
	if(cExpr!=NULL)
	{
		//表达式长度
		length = strlen(cExpr);
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//表达式内容
		memcpy(cmd+iOffSize,cExpr,strlen(cExpr));	
		iOffSize += strlen(cExpr);
	}
	else
	{
		//表达式长度
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//留下pOtherParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//更新单条记录
	length = 1;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//留下记录总长度的位置
	tmpOffSize1 = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pOtherParams!=NULL)
	{
		//记录的字段数
		length = pOtherParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pOtherParams->m_iNum;++i)
		{
			//记录第i个值长度
			length = pOtherParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pOtherParams->m_pValues[i],pOtherParams->m_iValueLenths[i]);	
			iOffSize += pOtherParams->m_iValueLenths[i];
		}
	}
	else
	{
		//记录的字段数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	//记录总长度
	length = iOffSize - tmpOffSize1 - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize1,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);

	//pOtherParams总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	excuteCmd(cmd,iOffSize);

	int iResult;
	memcpy(&iResult,m_recLine,sizeof(iResult));
	
	return iResult;	
}
int RemoteMdbSession::update(const char *cTableName
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
//Added 2008-7-8 批量更新
int RemoteMdbSession::update(const char *cTableName
				,const char *cIndexName
				,const InputParams *pIndexKeyParams
				,vector<string> &sFieldNames            // 需要更新的字段名
				,const vector<InputParams *> pUpdateValues          // 需要更新的字段值
				,const char *cExpr             //查询表达式,如：(msisdn=:1 and startTime <= :2)
				,const vector<InputParams *> pOtherParams)   
{
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length,tmpOffSize,tmpOffSize1;
	
	length = strlen(UPDATE_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,UPDATE_CMD,strlen(UPDATE_CMD));
	iOffSize += strlen(UPDATE_CMD);
	
	//字段数
	length = 7;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	length = strlen(cIndexName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cIndexName,strlen(cIndexName));
	iOffSize += strlen(cIndexName);
	
	//留下indexKeyParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	if( pIndexKeyParams!=NULL)
	{
		//indexKeyParams叁数个数
		length = pIndexKeyParams->m_iNum;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		for(int i=0;i<pIndexKeyParams->m_iNum;++i)
		{
			//第i个参数长度
			length = pIndexKeyParams->m_iValueLenths[i];
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			//第i个参数值
			memcpy(cmd+iOffSize,pIndexKeyParams->m_pValues[i],pIndexKeyParams->m_iValueLenths[i]);	
			iOffSize += pIndexKeyParams->m_iValueLenths[i];
		}
	}
	else
	{
		//indexKeyParams叁数个数
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;	
	}
	//indexKeyParams叁数总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	//留下fieldNames总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//需要更新的字段个数
	length = sFieldNames.size();
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	for(int i=0;i<sFieldNames.size();++i)
	{
		//第i个字段长度
		length = sFieldNames[i].length();
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//第i个字段值
		memcpy(cmd+iOffSize,sFieldNames[i].c_str(),sFieldNames[i].length());	
		iOffSize += sFieldNames[i].length();
	}
	//fieldNames总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);

	//留下updateValues总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//pUpdateValues记录数
	length = pUpdateValues.size();
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	for (int j=0;j<pUpdateValues.size();++j)
	{
		//留下第j条记录总长度的位置
		tmpOffSize1 = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		if( pUpdateValues[j]!=NULL)
		{
			//第j条记录的字段数
			length = pUpdateValues[j]->m_iNum;
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			for(int i=0;i<pUpdateValues[j]->m_iNum;++i)
			{
				//第j条记录第i个值长度
				length = pUpdateValues[j]->m_iValueLenths[i];
				memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				
				//第i个参数值
				memcpy(cmd+iOffSize,pUpdateValues[j]->m_pValues[i],pUpdateValues[j]->m_iValueLenths[i]);	
				iOffSize += pUpdateValues[j]->m_iValueLenths[i];
			}
		}
		else
		{
			//第j条记录的字段数
			length = 0;
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
		}
		//第j条记录总长度
		length = iOffSize - tmpOffSize1 - TRANS_FIELDS_LENTH_BYTES;
		memcpy(cmd+tmpOffSize1,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	}
	//updateValues总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	
	if(cExpr!=NULL)
	{
		//表达式长度
		length = strlen(cExpr);
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		//表达式内容
		memcpy(cmd+iOffSize,cExpr,strlen(cExpr));	
		iOffSize += strlen(cExpr);
	}
	else
	{
		//表达式长度
		length = 0;
		memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
	}
	
	//留下pOtherParams总长度的位置
	tmpOffSize = iOffSize;
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	//pOtherParams记录数
	length = pOtherParams.size();
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	for (int j=0;j<pOtherParams.size();++j)
	{
		//留下第j条记录总长度的位置
		tmpOffSize1 = iOffSize;
		iOffSize += TRANS_FIELDS_LENTH_BYTES;
		
		if( pOtherParams[j]!=NULL)
		{
			//第j条记录的字段数
			length = pOtherParams[j]->m_iNum;
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
			
			for(int i=0;i<pOtherParams[j]->m_iNum;++i)
			{
				//第j条记录第i个值长度
				length = pOtherParams[j]->m_iValueLenths[i];
				memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
				iOffSize += TRANS_FIELDS_LENTH_BYTES;
				
				//第i个参数值
				memcpy(cmd+iOffSize,pOtherParams[j]->m_pValues[i],pOtherParams[j]->m_iValueLenths[i]);	
				iOffSize += pOtherParams[j]->m_iValueLenths[i];
			}
		}
		else
		{
			//第j条记录的字段数
			length = 0;
			memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
			iOffSize += TRANS_FIELDS_LENTH_BYTES;
		}
		//第j条记录总长度
		length = iOffSize - tmpOffSize1 - TRANS_FIELDS_LENTH_BYTES;
		memcpy(cmd+tmpOffSize1,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	}
	//pOtherParams总长度
	length = iOffSize - tmpOffSize - TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+tmpOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);

	excuteCmd(cmd,iOffSize);

	int iResult;
	memcpy(&iResult,m_recLine,sizeof(iResult));
	
	return iResult;	
}

void RemoteMdbSession::excuteCmd(const char * cmd,int iSendSize)
{
	int iFlag =0,iPages =0,iReadSize;
	char cRecSize[10+1];
	char tmp[MAX_TRANSMISSION_BYTE_SIZE+2];
	int offSet;

	// 发送命令
	m_pWorkSocket->write(cmd,iSendSize);

	memset(m_recLine,0,sizeof(m_recLine));
	
	// 获取返回值
	//memset(tmp,0,sizeof(tmp));                  
	//iReadSize = m_pWorkSocket->read(tmp,1,MAX_TRANSMISSION_BYTE_SIZE);      // 读查询结果串：长度＋结果
	//
	//strncpy(cRecSize,tmp,10);
	//cRecSize[10]=0;
	//m_iRecSize  = atol(cRecSize);	  
	//
	//memcpy(m_recLine,tmp+10,iReadSize-10);
	//offSet = iReadSize-10;
	//
	//if( m_iRecSize == -1)
	//{
	//	throw Mdb_Exception(__FILE__, __LINE__, "服务端执行指令:[%s]%s[%s]",cmd," 发生:",m_recLine); 	              
	//}
    //
	//if( (m_iRecSize+10)>MAX_TRANSMISSION_BYTE_SIZE )
	//{
	//	iPages  = (m_iRecSize+10)/MAX_TRANSMISSION_BYTE_SIZE;
	//	iFlag  = (m_iRecSize+10)%MAX_TRANSMISSION_BYTE_SIZE;
	//	
	//	// 读整页
	//	for(int i=iPages;i>1;--i)
	//	{   
	//		memset(tmp,0,sizeof(tmp));  
	//		iReadSize = m_pWorkSocket->read(tmp,1,MAX_TRANSMISSION_BYTE_SIZE);
	//		memcpy(m_recLine + offSet,tmp,iReadSize);
	//		offSet += MAX_TRANSMISSION_BYTE_SIZE;
	//	}
	//	// 读不足一页 iPages 是充满的页数
	//	if (iFlag!=0) 
	//	{   
	//		memset(tmp,0,sizeof(tmp));  
	//		iReadSize = m_pWorkSocket->read(tmp,1,(m_iRecSize+10)-iPages*MAX_TRANSMISSION_BYTE_SIZE);
	//		memcpy(m_recLine + offSet,tmp,iReadSize);
	//	}  	
	//} 
	int iFirstReadCount = m_pWorkSocket->read(tmp,1,MAX_TRANSMISSION_BYTE_SIZE);
	if ( iFirstReadCount  > 0) 
	{
		strncpy(cRecSize,tmp,10);
		cRecSize[10]=0;
		m_iRecSize  = atol(cRecSize);	  
		
		memcpy(m_recLine,tmp+10,iFirstReadCount-10);
		
		if( m_iRecSize == -1)
		{
			throw Mdb_Exception(__FILE__, __LINE__, "服务端执行指令:[%s]%s[%s]",cmd," 发生:",m_recLine); 	              
		}

		if ((m_iRecSize + 10) > iFirstReadCount)//MAX_TRANSMISSION_BYTE_SIZE) 
		{
			int iReadCount = 0;
			while (true) 
			{
				memset(tmp,0,sizeof(tmp)); 
				iReadSize = m_pWorkSocket->read(tmp,1,MAX_TRANSMISSION_BYTE_SIZE);
				memcpy(m_recLine + iReadCount + iFirstReadCount-10,tmp,iReadSize);
				iReadCount += iReadSize;
				if (iReadCount == (m_iRecSize + 10) - iFirstReadCount) 
				{
					break;
				}
			}
		}
	}
} 
    
void RemoteMdbSession::setSessionID(int &i)
{   
	return ;
}
    
int  RemoteMdbSession::getSessionID()
{   
	return 0;
}
    
void RemoteMdbSession::begin(const char *cTableName,int iLockType)
{   
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length;
	
	length = strlen(BEGIN_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,BEGIN_CMD,strlen(BEGIN_CMD));
	iOffSize += strlen(BEGIN_CMD);
	
	//字段数
	length = 2;
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	
	length = strlen(cTableName);
	memcpy(cmd+iOffSize,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,cTableName,strlen(cTableName));
	iOffSize += strlen(cTableName);
	
	length = sizeof(iionID()
{   
	return 0;
}
    
void RemoteMdbSession::begin(const char *cTableName,int iLockType)
{   
	char cmd[MAX_CMD_LEN];
	int iOffSize = 0;
	int length;
	
	length = strlen(BEGIN_CMD);
	memcpy(cmd,(char *)&length + PublicFunc::m_offSet,TRANS_FIELDS_LENTH_BYTES);
	iOffSize += TRANS_FIELDS_LENTH_BYTES;
	memcpy(cmd+iOffSize,BEGIN_CMD,strlen(BEGIN_CMD));
	iOffSize += strlen(BEGIN_CMD);
	
	//瀛楁