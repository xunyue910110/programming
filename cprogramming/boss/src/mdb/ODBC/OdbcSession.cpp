// ############################################
// Source file : OdbcSession.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-9-17
// Update      : 2008-9-17
// Copyright(C): chen min, Linkage.
// ############################################

#include "OdbcSession.h"
#include "base/StringUtil.h"
#include "TableDescript.h"
#include "InputParams.h"
#if defined(_TYPE_CMCC)
	#include "uconfig/SysParam.h"
#else
	#include "base/SysParam.h"
#endif
//##ModelId=48D0AB720167
OdbcSession::OdbcSession(T_NAMEDEF hostname
							,T_NAMEDEF dbUserName
							,T_NAMEDEF dbUserPwd
							,int iPortNo
							,int iConnType	)
{
	//m_bufferSize = 1024 * 128;
	//m_pBuffer=(char *)malloc(m_bufferSize);
	m_dbc  = NULL;
	m_env  = NULL;
	
	m_pCurrentStmt=0;
	m_pCurrentStmtInfo=0;
	this->allocHandle();

#ifdef ALTIBASE	
	char connStr[1024];
	sprintf(connStr,"DSN=%s;UID=%s;PWD=%s;CONNTYPE=%d;NLS_USE=US7ASCII;PORT_NO=%d",
        hostname,dbUserName,dbUserPwd,iConnType,iPortNo);
#elif defined TIMESTEN
	unsigned char connStr[1024];
	sprintf((char *)connStr,"DSN=%s;UID=%s;PWD=%s",
        hostname,dbUserName,dbUserPwd,iPortNo);
#endif
	
	SQLRETURN sqlReturn = SQLDriverConnect(m_dbc,NULL,connStr,SQL_NTS,NULL, 0, NULL,SQL_DRIVER_NOPROMPT );
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_SUCCESS_WITH_INFO) )
	{
		this->freeHandle();
		throw Mdb_Exception(__FILE__, __LINE__, "获取 m_dbc失败!");
	}

#if defined TIMESTEN
	SQLSetConnectOption(m_dbc, SQL_AUTOCOMMIT, 0);	
#endif
	memset(m_sql,0,sizeof(m_sql));
	
#ifdef ALTIBASE
	// add by chenm 2009-8-17 使用 excute("commit")提交
	if (SQL_ERROR == SQLAllocStmt(m_dbc, &m_CommitStmt)) 
    {
        this->throwErr(__FILE__, __LINE__);
    }
    if (SQLPrepare(m_CommitStmt, "commit", SQL_NTS) != SQL_SUCCESS)
    {
    	this->throwErr(__FILE__, __LINE__);
    }    
#endif 
}


//##ModelId=48D0AB720177
OdbcSession::~OdbcSession()
{
	//free(m_pBuffer);
	for (INDEX_DEF_MAP_ITR iter=m_indexDefsMap.begin(); iter!=m_indexDefsMap.end(); ++iter)
	{
		delete iter->second;
	} 
	// add by chenm 2009-11-4 16:28:27
	for (TABLE_DEF_MAP_ITR iter=m_tableDefsMap.begin(); iter!=m_tableDefsMap.end(); ++iter)
	{
		delete iter->second;
	} 
	// over 2009-11-4 16:28:34
	m_indexDefsMap.clear();
	freeStmt();
	this->freeHandle();
}

//##ModelId=48D074C602BF
void OdbcSession::freeHandle()
{
    //#ifdef ALTIBASE //add by xuf 2009-8-17 18:36:43
    //    SQLFreeStmt(m_CommitStmt, SQL_DROP); 
    //#endif	
    SQLDisconnect(m_dbc);

    if(m_dbc != NULL)
    {
    	SQLFreeConnect(m_dbc);
    	m_dbc=NULL;
	}
    if(m_env != NULL)
    {
    	SQLFreeEnv(m_env);
    	m_env=NULL;
    }
}

//##ModelId=48D092AF00AB
void OdbcSession::allocHandle()
{
    /* allocate Environment handle */
    if( (SQLAllocEnv(&m_env) != SQL_SUCCESS)
    	||(SQLAllocConnect(m_env, &m_dbc) != SQL_SUCCESS) 
      )
    {
		this->freeHandle();
		this->throwErr(__FILE__, __LINE__);	
    }
}

// 异常处理,获取错误信息抛出
void OdbcSession::throwErr(char * filename, int linenum)
{
	// 错误信息编码 /
    // add by chenm 2009-7-8 17:24:30
   #if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha)) 
    SDWORD iErrNo;
   #else
    SQLINTEGER iErrNo;
   #endif
	// 错误信息长度 
    SQLSMALLINT iMsgLength;
    SQLCHAR cErrMsg[SQL_MAX_MESSAGE_LENGTH + 1];     
	
	// 获取ODBC自身错误失败
	if(SQLError(m_env,m_dbc,*m_pCurrentStmt,NULL,&iErrNo,cErrMsg,SQL_MAX_MESSAGE_LENGTH + 1,&iMsgLength ) != SQL_SUCCESS)
		throw Mdb_Exception(__FILE__, __LINE__, "获取ODBC SQLError信息失败!");
		
   #if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha)) 
	throw Mdb_Exception(filename, linenum, "ErrorNO:%ld,ErrMsg:%s",iErrNo,cErrMsg);
   #else
	throw Mdb_Exception(filename, linenum, "ErrorNO:%d,ErrMsg:%s",iErrNo,cErrMsg);
   #endif
}

bool OdbcSession::createTable(const TableDef &theTableDef)
{
	// TO DO
	return true;	
}
bool OdbcSession::dropTable(const char * cTableName)
{
	static string DROP_TABLE("drop table ");
	string sSql;
	
	sSql = DROP_TABLE+cTableName;
	
	SQLHSTMT     stmt = SQL_NULL_HSTMT;
	if (SQL_ERROR == SQLAllocStmt(m_dbc, &stmt)) 
    {
        this->throwErr(__FILE__, __LINE__);
    }
    m_pCurrentStmt=&stmt;
    	
	this->excuteSQL(sSql);

    SQLFreeStmt(stmt, SQL_DROP);
    m_pCurrentStmt=NULL;
    	
	return true;
}

bool OdbcSession::truncate(const char * cTableName)
{
	static string TRUNCATE_TABLE("truncate table ");
	string sSql;
	
	sSql = TRUNCATE_TABLE+cTableName;

	SQLHSTMT     stmt = SQL_NULL_HSTMT;
	if (SQL_ERROR == SQLAllocStmt(m_dbc, &stmt)) 
    {
        this->throwErr(__FILE__, __LINE__);
    }
    m_pCurrentStmt=&stmt;
	    	
	this->excuteSQL(sSql);	
	
    SQLFreeStmt(stmt, SQL_DROP);
    m_pCurrentStmt=NULL;	
	
	return true;
}

bool OdbcSession::createIndex(const IndexDef &theIndexDef)
{
	// TO DO
	return true;
}

bool OdbcSession::dropIndex(const char * cTableName,const char * cIndexName)
{
	static string DROP_INDEX("drop index ");
	string sSql;
	
	sSql = DROP_INDEX+cTableName;
	SQLHSTMT     stmt = SQL_NULL_HSTMT;
	if (SQL_ERROR == SQLAllocStmt(m_dbc, &stmt)) 
    {
        this->throwErr(__FILE__, __LINE__);
    }
    m_pCurrentStmt=&stmt;
    	
	this->excuteSQL(sSql);	
	
    SQLFreeStmt(stmt, SQL_DROP);
    m_pCurrentStmt=NULL;
    	
	return true;		
}

void OdbcSession::begin(const char *cTableName,int iLockType ) 
{
	return;	
}	
	
bool OdbcSession::commit() 
{
#ifdef ALTIBASE //add by xuf 2009-8-17 16:19:52	    	
  SQLRETURN sqlReturn = SQLExecute(m_CommitStmt);
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_NO_DATA_FOUND) )      // over 2009-8-17 
	{
		this->throwErr(__FILE__, __LINE__);
	}	
	return true;
#else	
	if( SQLTransact(m_env, m_dbc, SQL_COMMIT) != SQL_SUCCESS)
	{
		this->throwErr(__FILE__, __LINE__);
	}	
	return true;
#endif	
}

bool OdbcSession::rollBack() 
{
	if( SQLTransact(m_env, m_dbc, SQL_ROLLBACK) != SQL_SUCCESS)
	{
		this->throwErr(__FILE__, __LINE__);
	}	
	return true;		
}
bool OdbcSession::getInsertSQL(const char *cTableName
	                          ,const InputParams *pInputParams)
{
	sprintf(m_sql,"insert into %s values(",cTableName);
	
	for(int i=0;i<pInputParams->m_iNum;++i)
	{
		strcat(m_sql,"?");
		if(i != pInputParams->m_iNum-1)
			strcat(m_sql,",");		
	}
	strcat(m_sql,")");
	return true;
}
bool OdbcSession::getInsertSQL(const char *cTableName
							,const vector<string> &cFieldNames
				    		,const InputParams *pInputParams)
{
	sprintf(m_sql,"insert into %s(",cTableName);
	if (cFieldNames.size() != pInputParams->m_iNum)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "指定的字段个数:%d不等于参数字段数:%d",cFieldNames.size(),pInputParams->m_iNum);		
	}
	for(int i=0;i<cFieldNames.size();++i)
	{
		strcat(m_sql,cFieldNames[i].c_str());
		if(i != cFieldNames.size()-1)
			strcat(m_sql,",");	
	}
	strcat(m_sql,") values(");
	
	for(int i=0;i<pInputParams->m_iNum;++i)
	{
		strcat(m_sql,"?");	
		if(i != pInputParams->m_iNum-1)
			strcat(m_sql,",");
	}
	strcat(m_sql,")");
	return true;
}
bool OdbcSession::getUpdateSQL(  const char *cTableName
								,const char *cIndexName
								,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
								,vector<string> &sFieldNames                // 需要更新的字段名
								,const InputParams *pUpdateValues           // 需要更新的字段值
								,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
								,const InputParams *pOtherParams)        //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	sprintf(m_sql,"update %s set ",cTableName);
	
	for(int i=0;i<sFieldNames.size();++i)
	{
		strcat(m_sql,sFieldNames[i].c_str());
		strcat(m_sql,"=?");
		if(i != sFieldNames.size()-1)
			strcat(m_sql,",");
	}

	// 更新条件
	if( pIndexKeyParams != NULL
		&& pIndexKeyParams->m_iNum != 0 )
	{
		IndexDef *pIndexDef = this->getIndexDef(cTableName,cIndexName);
		if (pIndexKeyParams->m_iNum > pIndexDef->m_columnNum)
		{
			throw Mdb_Exception(__FILE__, __LINE__, "输入参数个数:%d大于索引字段数:%d",pIndexKeyParams->m_iNum,pIndexDef->m_columnNum);
		}
		strcat(m_sql," where ");
		
		for (int i=0;i<pIndexKeyParams->m_iNum;i++)
		{
			strcat(m_sql,pIndexDef->m_columnList[i]);
			strcat(m_sql,"=?");
			
			if( i != pIndexKeyParams->m_iNum -1 )
				strcat(m_sql," and ");			
		}
		replaceExprBindParam(cExpr,pOtherParams);
	}
	return true;
}
void OdbcSession::replaceExprBindParam(const char *cExpr,const InputParams *pOtherParams)
{
	if (cExpr == NULL || pOtherParams == NULL || strcmp(cExpr,"") ==0) return;
  
	m_posMap.clear();
	//将表达式拼入sql语句
	strcat(m_sql," and (");
	int iOldLen=strlen(m_sql);//表达式开始之前的长度
	strcat(m_sql,cExpr);
	strcat(m_sql,")");

	//去掉表达式中的:n,替换为?,同时得到绑定变量位置和绑定变量数字之间的关系，保存到map
	char cSequnceNum[5];//记录绑定变量中的数字的临时字符串
	char *cExprPtr=m_sql + iOldLen;//表达式开始位置指针
	int iLen=strlen(cExprPtr);//表达式的长度
	char *cPtr=0;//记录“:”开始的指针位置
	int idx = -1;//记录绑定变量在字符串中出现的次序：0开始
  int i,j,k;
	for (i=0;i<iLen;i++)//轮询整个字符串
	{
		if (cExprPtr[i] == ':')//找到绑定变量的开始
		{
			idx ++;//记录下这个绑定变量出现的位置
			cPtr = &(cExprPtr[i]);
            cPtr[0]='?';//“:”替换为“?”
	        for (j=1;;j++)//从新位置+1处理这个绑定变量，即“:”后面的数字
	        {
                //找到数字结束的位置
	            if (cPtr[j] == ' ' || cPtr[j]==')' || cPtr[j] == '\0' || cPtr[j] == '\t')
	            {
                    //将数字保存到临时字符串
	                memset(cSequnceNum,0,5);
	                strncpy(cSequnceNum,cPtr+1,j);
                    //将绑定变量出现的位置，数字插入map
	                m_posMap.insert(map<int,int>::value_type(idx,atoi(cSequnceNum)));
                    //将数字清空
                    for (k=1;k<j;k++)
                        cPtr[k]=' ';
	                break;//跳出循环，准备找下个“:”
	            }
	        }
		}
	}
}
bool OdbcSession::getDeleteSQL(const char *cTableName
							,const char *cIndexName
							,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
							,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
							,const InputParams *pOtherParams)   //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	sprintf(m_sql,"delete from %s",cTableName);
	
	if( pIndexKeyParams != NULL && pIndexKeyParams->m_iNum != 0 )
	{
		IndexDef *pIndexDef = this->getIndexDef(cTableName,cIndexName);
		if (pIndexKeyParams->m_iNum > pIndexDef->m_columnNum)
		{
			throw Mdb_Exception(__FILE__, __LINE__, "输入参数个数:%d大于索引字段数:%d",pIndexKeyParams->m_iNum,pIndexDef->m_columnNum);
		}
		strcat(m_sql," where ");
		for (int i=0;i<pIndexKeyParams->m_iNum;i++)
		{
			strcat(m_sql,pIndexDef->m_columnList[i]);
			strcat(m_sql,"=?");
			
			if( i != pIndexKeyParams->m_iNum -1 )
				strcat(m_sql," and ");			
		}
		replaceExprBindParam(cExpr,pOtherParams);
	}
	return true;
}
bool OdbcSession::getSelectSQL( const char *cTableName
								,const char *cIndexName
								,const InputParams *pIndexKeyParams
								,const char *cExpr             
								,const InputParams *pOtherParams)
{
	sprintf(m_sql,"select * from %s"	,cTableName	);
	if( pIndexKeyParams != NULL && pIndexKeyParams->m_iNum != 0 )
	{
		IndexDef *pIndexDef = this->getIndexDef(cTableName,cIndexName);
		if (pIndexKeyParams->m_iNum > pIndexDef->m_columnNum)
		{
			throw Mdb_Exception(__FILE__, __LINE__, "输入参数个数:%d大于索引字段数:%d",pIndexKeyParams->m_iNum,pIndexDef->m_columnNum);
		}
		strcat(m_sql," where ");
		for (int i=0;i<pIndexKeyParams->m_iNum;i++)
		{
			strcat(m_sql,pIndexDef->m_columnList[i]);
			strcat(m_sql,"=?");
			
			if( i != pIndexKeyParams->m_iNum -1 )
				strcat(m_sql," and ");			
		}

		replaceExprBindParam(cExpr,pOtherParams);
	}
	else if(cExpr!=NULL && pOtherParams != NULL)
	{
		strcat(m_sql," where 1=1 ");	
		replaceExprBindParam(cExpr,pOtherParams);
	}
	return true;

}
bool OdbcSession::prepareSQL(StmtInfo & r_stmtInfo)
{
    r_stmtInfo.m_columnCount = 0;
    //r_stmtInfo.m_fetchArraySize=m_fetchCount;
    r_stmtInfo.m_fetchedRowCount=0;
    r_stmtInfo.m_rowStatus=NULL;
    r_stmtInfo.m_currentNum = 0;

   #if defined(TIMESTEN) // by chenm 2009-7-8 17:13:40
   	if (SQLPrepare(r_stmtInfo.m_stmt, (unsigned char *)m_sql, SQL_NTS) != SQL_SUCCESS)
   #else
    if (SQLPrepare(r_stmtInfo.m_stmt, m_sql, SQL_NTS) != SQL_SUCCESS)
   #endif // over 2009-7-8 17:13:46
    {
    	this->throwErr(__FILE__, __LINE__);
    }
    
    SQLSMALLINT   NumParams, i;
    SQLNumParams(r_stmtInfo.m_stmt, &NumParams);
    if (NumParams) 
    {
    	for (i = 0; i < NumParams; i++)
    	{
    		int ind;
    		r_stmtInfo.m_vInd.push_back(ind);
			
    		BindParamInfo info;
    		//add by chenzm@2009-7-28
			info.data = NULL;    		
			//add end
    		//SQLDescribeParam(r_stmtInfo.m_stmt
    		//				,i + 1
    		//				,&info.sqlType
    		//				,&info.columnSize
    		//				,&info.scale
    		//				,&info.nullAble);
    		//getCTypeBySqlType(r_stmtInfo.m_vParamInfo[i].sqlType,r_stmtInfo.m_vParamInfo[i].cType);
			r_stmtInfo.m_vParamInfo.push_back(info);
    	}
    }
    return true;
}
bool OdbcSession::bindOneParams(StmtInfo & r_stmtInfo
	                           ,const InputParams *pParams
	                           ,int &iStartPos
	                           ,bool isExpr)
{
	if (pParams == NULL || pParams->m_iNum <= 0)
		return true;
	int paramLen;
   #if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha)) // by chenm 2009-7-9 
	long *pind;
   #else	
	SQLINTEGER *pind;
   #endif           // over 2009-7-9 	
	if (!isExpr)
	{
		for (int i=0;i<pParams->m_iNum;i++)
		{
			//add by chenzm@2009-7-28
			if (r_stmtInfo.m_vParamInfo[i + iStartPos].data != NULL)
			{
				memcpy(r_stmtInfo.m_vParamInfo[i + iStartPos].data
				      ,pParams->m_pValues[i]
				      ,pParams->m_iValueLenths[i]);
				continue;
			}
			//add end			
			if (pParams->m_varTypes[i] == VAR_TYPE_VSTR
			||	pParams->m_varTypes[i] == VAR_TYPE_NUMSTR )
			{
				r_stmtInfo.m_vInd[i + iStartPos]=SQL_NTS;
				pind=&(r_stmtInfo.m_vInd[i + iStartPos]);
				pind=NULL;
				paramLen=getInputParamLen(pParams,i)+1;
				r_stmtInfo.m_vParamInfo[i + iStartPos].columnSize = 100;
			} 
			else 
			{
				//r_stmtInfo.m_vInd[i + iStartPos]=0;
				pind=NULL;
				paramLen=0;
				r_stmtInfo.m_vParamInfo[i + iStartPos].columnSize = pParams->m_iValueLenths[i];
			}
			r_stmtInfo.m_vParamInfo[i + iStartPos].scale = 0;
			//add by chenzm@2009-7-28
			r_stmtInfo.m_vParamInfo[i + iStartPos].data = new char[ r_stmtInfo.m_vParamInfo[i + iStartPos].columnSize ];
			memset( r_stmtInfo.m_vParamInfo[i + iStartPos].data
			      , 0 
			      , r_stmtInfo.m_vParamInfo[i + iStartPos].columnSize);
			memcpy( r_stmtInfo.m_vParamInfo[i + iStartPos].data
			      , pParams->m_pValues[i]
			      , pParams->m_iValueLenths[i]);
			//add end			
		    if (SQLBindParameter(r_stmtInfo.m_stmt,
		                         i + 1 + iStartPos,           /* Parameter number, starting at 1 */
		                         SQL_PARAM_INPUT, /* in, out, inout */
		                         getCTypeByInputParam(pParams,i),        /* C data type of the parameter */
		                         getSQLTypeByInputParam(pParams,i),
		                         //r_stmtInfo.m_vParamInfo[i + iStartPos].sqlType,      /* SQL data type of the parameter : char(8)*/
		                         r_stmtInfo.m_vParamInfo[i + iStartPos].columnSize,       /* size of the column or expression, precision */
		                         r_stmtInfo.m_vParamInfo[i + iStartPos].scale,               /* The decimal digits, scale */
		                         r_stmtInfo.m_vParamInfo[i + iStartPos].data, //pParams->m_pValues[i],/* A pointer to a buffer for the parameter’s data */ //rem by chenzm@2009-7-28
		                         paramLen,   /* Length of the ParameterValuePtr buffer in bytes */
		                         pind        /* indicator */
		                         ) != SQL_SUCCESS)
			{
				throwErr(__FILE__,__LINE__);
			}
		}
		iStartPos += pParams->m_iNum;
		return true;
	}
	int iPos,iCount=r_stmtInfo.m_vParamInfo.size();
	for (int i=iStartPos;i<iCount;i++)
	{
		iPos=m_posMap.find(i - iStartPos)->second;
		//add by chenzm@2009-7-28
		if (r_stmtInfo.m_vParamInfo[i].data != NULL)
		{
			memcpy(r_stmtInfo.m_vParamInfo[i].data
			      ,pParams->m_pValues[iPos]
			      ,pParams->m_iValueLenths[iPos]);
			continue;
		}
		//add end	
		
		if (pParams->m_varTypes[iPos] == VAR_TYPE_VSTR
		||	pParams->m_varTypes[iPos] == VAR_TYPE_NUMSTR )
		{
			r_stmtInfo.m_vInd[i]=SQL_NTS;
			pind=&(r_stmtInfo.m_vInd[i]);
			paramLen=getInputParamLen(pParams,iPos)+1;
			r_stmtInfo.m_vParamInfo[i].columnSize = 100;
		} 
		else 
		{
			//r_stmtInfo.m_vInd[i]=0;
			pind=NULL;
			paramLen=0;
			r_stmtInfo.m_vParamInfo[i].columnSize = pParams->m_iValueLenths[iPos];
		}
    	r_stmtInfo.m_vParamInfo[i].scale = 0;
		//add by chenzm@2009-7-28
		r_stmtInfo.m_vParamInfo[i].data = new char[ r_stmtInfo.m_vParamInfo[i].columnSize ];
		memset( r_stmtInfo.m_vParamInfo[i].data
		      , 0 
		      , r_stmtInfo.m_vParamInfo[i].columnSize);
		memcpy( r_stmtInfo.m_vParamInfo[i].data
		      , pParams->m_pValues[iPos]
		      , pParams->m_iValueLenths[iPos]);
		//add end
	    if (SQLBindParameter(r_stmtInfo.m_stmt,
	                         i + 1,           /* Parameter number, starting at 1 */
	                         SQL_PARAM_INPUT, /* in, out, inout */
	                         getCTypeByInputParam(pParams,iPos),        /* C data type of the parameter */
	                         getSQLTypeByInputParam(pParams,iPos),
	                         //r_stmtInfo.m_vParamInfo[i].sqlType,      /* SQL data type of the parameter : char(8)*/
	                         r_stmtInfo.m_vParamInfo[i].columnSize,       /* size of the column or expression, precision */
	                         r_stmtInfo.m_vParamInfo[i].scale,               /* The decimal digits, scale */
	                         r_stmtInfo.m_vParamInfo[i].data,//pParams->m_pValues[iPos],/* A pointer to a buffer for the parameter’s data */ //rem by chenzm@2009-7-28
	                         paramLen,   /* Length of the ParameterValuePtr buffer in bytes */
	                         pind        /* indicator */
	                         ) != SQL_SUCCESS)
		{
			throwErr(__FILE__,__LINE__);
		}
	}
	return true;
}
bool OdbcSession::bindSelectParam(
				  StmtInfo & r_stmtInfo
				 ,const InputParams *pIndexKeyParams
				 ,const char *cExpr             
				 ,const InputParams *pOtherParams)
{
	int iStartPos=0;
	bindOneParams(r_stmtInfo,pIndexKeyParams,iStartPos);
	if (cExpr != NULL 
	&& strcmp(cExpr,"") != 0 )
	{
		bindOneParams(r_stmtInfo,pOtherParams,iStartPos,true);
	}
	return true;
}
bool OdbcSession::bindInsertParam(StmtInfo & r_stmtInfo
				 ,const InputParams *pInputParams)
{
   #if defined(TIMESTEN) // by chenm 2009-7-10 16:52:16
	//SQLSetStmtOption(*m_pCurrentStmt, , );
   #else
    //SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN,0);
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAMS_PROCESSED_PTR, 0 ,0 );
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAM_STATUS_PTR, (void *)0 ,0 );
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAMSET_SIZE, (void *)1 ,0 );
   #endif
   
	int iStartPos=0;
	bindOneParams(r_stmtInfo,pInputParams,iStartPos);
	return true;
}
bool OdbcSession::bindUpdateParam(StmtInfo & r_stmtInfo
				 ,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
				 ,const InputParams *pUpdateValues           // 需要更新的字段值
				 ,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
				 ,const InputParams *pOtherParams)         //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
   #if defined(TIMESTEN) // by chenm 2009-7-10 16:52:16
   #else 
    //SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN,0);
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAMS_PROCESSED_PTR, 0 ,0 );
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAM_STATUS_PTR, (void *)0 ,0 );
	SQLSetStmtAttr(*m_pCurrentStmt, SQL_ATTR_PARAMSET_SIZE, (void *)1 ,0 );
   #endif
	
	int iStartPos=0;
	bindOneParams(r_stmtInfo,pUpdateValues,iStartPos);
	bindOneParams(r_stmtInfo,pIndexKeyParams,iStartPos);
	if (cExpr != NULL 
	&& strcmp(cExpr,"") != 0 )
	{
		bindOneParams(r_stmtInfo,pOtherParams,iStartPos,true);
	}
	return true;
}
bool OdbcSession::bindDeleteParam(StmtInfo & r_stmtInfo
				 ,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
				 ,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
				 ,const InputParams *pOtherParams)      //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	int iStartPos=0;
	bindOneParams(r_stmtInfo,pIndexKeyParams,iStartPos);
	if (cExpr != NULL 
	&& strcmp(cExpr,"") != 0 )
	{
		bindOneParams(r_stmtInfo,pOtherParams,iStartPos,true);
	}
	return true;
}
int OdbcSession::select(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
		,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
		,const InputParams *pOtherParams         //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
		,const int iRowCount)
{
	FP_BEGIN(getSelectSQL)
	//组装sql语句到m_sql
	getSelectSQL(cTableName,cIndexName,pIndexKeyParams,cExpr,pOtherParams);
	FP_END(getSelectSQL)
	
	FP_BEGIN(getSelectStmt)
	//根据sql语句在缓存中查找，如果找到了返回缓存迭代器
	//如果没有找到则对sql进行prepare,并初始化绑定变量属性
	map<SQLCatchInfo,StmtInfo>::iterator t_it;
	t_it=getStmt(3,cTableName,cIndexName);
	FP_END(getSelectStmt)

   #if defined(TIMESTEN) // add by chenm 2009-7-29 释放游标,有的select操作,并不会fetch结束结果集,如果不close,就有问题
	SQLFreeStmt(t_it->second.m_stmt,SQL_CLOSE);
   #endif
		
	FP_BEGIN(bindSelectParam)
	//对输入参数和宿主进行绑定
	bindSelectParam(t_it->second,pIndexKeyParams,cExpr,pOtherParams);
	FP_END(bindSelectParam)
	
	FP_BEGIN(SQLExecute)
	SQLRETURN sqlReturn = SQLExecute(t_it->second.m_stmt);
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_NO_DATA_FOUND) )
	{
		#ifdef _DEBUG_
		cout<<t_it->first.m_sqlStr<<endl;
		#endif
		this->throwErr(__FILE__, __LINE__);
	}
	FP_END(SQLExecute)
	
	FP_BEGIN(bindResultCols)
	this->bindResultCols(cTableName,&(t_it->second.m_stmt));
	FP_END(bindResultCols)
	
	FP_BEGIN(effectRowCount)
	// 返回选中的记录数
	//int iCount=	this->effectRowCount();
	FP_END(effectRowCount)
	
	return 0;
}
		
int  OdbcSession::deleteRec(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams      //查询条件中的索引字段值
		,const char *cExpr                       //查询表达式,如：(msisdn=:0 and startTime <= :1)
		,const InputParams *pOtherParams)        //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	
	//获得sql到到m_sql
	getDeleteSQL(cTableName,cIndexName,pIndexKeyParams,cExpr,pOtherParams);
	//根据sql语句在缓存中查找，如果找到了返回缓存迭代器
	//如果没有找到则对sql进行prepare,并初始化绑定变量属性
	map<SQLCatchInfo,StmtInfo>::iterator t_it;
	t_it=getStmt(1,cTableName,cIndexName);
	
	//对输入参数和宿主进行绑定
	bindDeleteParam(t_it->second,pIndexKeyParams,cExpr,pOtherParams);

	SQLRETURN sqlReturn = SQLExecute(t_it->second.m_stmt);
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_NO_DATA_FOUND) )
	{
		#ifdef _DEBUG_
		cout<<t_it->first.m_sqlStr<<endl;
		#endif
		this->throwErr(__FILE__, __LINE__);
	}
	// 返回删除掉的记录数
	return this->effectRowCount();
}
bool OdbcSession::insert(const char *cTableName
		,const InputParams *pInputParams)      // 需要插入的记录分字段内容
{
	//获得sql到到m_sql
	getInsertSQL(cTableName,pInputParams);
	//根据sql语句在缓存中查找，如果找到了返回缓存迭代器
	//如果没有找到则对sql进行prepare,并初始化绑定变量属性
	map<SQLCatchInfo,StmtInfo>::iterator t_it;
	t_it=getStmt(0,cTableName,NULL);
	
	//对输入参数和宿主进行绑定
	bindInsertParam(t_it->second,pInputParams);

	SQLRETURN sqlReturn = SQLExecute(t_it->second.m_stmt);
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_NO_DATA_FOUND) )
	{
		#ifdef _DEBUG_
		cout<<t_it->first.m_sqlStr<<endl;
		#endif
		this->throwErr(__FILE__, __LINE__);
	}
	
	return true;
}

bool OdbcSession::insert(const char *cTableName
						 ,const vector<string> &cFieldNames
						 ,const InputParams *pInputParams)      // 需要插入的记录分字段内容
{
    //Modified By Sunam 2009/9/29 0:02:17
    ////获得sql到到m_sql
	//getInsertSQL(cTableName,cFieldNames,pInputParams);
	////根据sql语句在缓存中查找，如果找到了返回缓存迭代器
	////如果没有找到则对sql进行prepare,并初始化绑定变量属性
	//map<SQLCatchInfo,StmtInfo>::iterator t_it;
	//t_it=getStmt(0,cTableName,NULL);
	//
	////对输入参数和宿主进行绑定
	//bindInsertParam(t_it->second,pInputParams);
    //
	//SQLRETURN sqlReturn = SQLExecute(t_it->second.m_stmt);
	//if( (sqlReturn != SQL_SUCCESS) 
	//	&&(sqlReturn != SQL_NO_DATA_FOUND) )
	//{
	//	#ifdef _DEBUG_
	//	cout<<t_it->first.m_sqlStr<<endl;
	//	#endif
	//	this->throwErr(__FILE__, __LINE__);
	//}
	vector<InputParams *> pInputParamsNew;
	pInputParamsNew.push_back((InputParams *)pInputParams);

    insert(cTableName,cFieldNames,pInputParamsNew);

	return true;
}

//单条
int  OdbcSession::update(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams         //查询条件中的索引字段值
		,vector<string> &sFieldNames                // 需要更新的字段名
		,const InputParams *pUpdateValues           // 需要更新的字段值
		,const char *cExpr                          //查询表达式,如：(msisdn=:0 and startTime <= :1)
		,const InputParams *pOtherParams)           //查询条件中的非索引字段值 pValue[0-9]须和上面表达式中的变量一一对应
{
	getUpdateSQL(cTableName,cIndexName,pIndexKeyParams,sFieldNames,pUpdateValues,cExpr,pOtherParams);
	//根据sql语句在缓存中查找，如果找到了返回缓存迭代器
	//如果没有找到则对sql进行prepare,并初始化绑定变量属性
	map<SQLCatchInfo,StmtInfo>::iterator t_it;
	t_it=getStmt(2,cTableName,cIndexName);	
	
	//对输入参数和宿主进行绑定
	bindUpdateParam(t_it->second,pIndexKeyParams,pUpdateValues,cExpr,pOtherParams);

	SQLRETURN sqlReturn = SQLExecute(t_it->second.m_stmt);
	if( (sqlReturn != SQL_SUCCESS) 
		&&(sqlReturn != SQL_NO_DATA_FOUND) )
	{
		#ifdef _DEBUG_
		cout<<t_it->first.m_sqlStr<<endl;
		#endif
		this->throwErr(__FILE__, __LINE__);
	}
	// 返回更新成功的记录数
	return this->effectRowCount();		
}
// 返回sql操作生效的记录数
int OdbcSession::effectRowCount()
{
#if defined(TIMESTEN) && ( defined(__LP64__) || defined(_LP64) || defined(__64BIT__) || defined(__alpha))
	long iRowCount;
#else
	int iRowCount;
#endif

	if( SQLRowCount(*m_pCurrentStmt,&iRowCount) != SQL_SUCCESS)
	{
		this->throwErr(__FILE__, __LINE__);	
	}
	return iRowCount;
}
/*
// 批量更新						
int OdbcSession::update(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams
		,vector<string> &sFieldNames
		,const vector<InputParams *>pUpdateValues           
		,const char *cExpr             
		,const vector<InputParams *>pOtherParams)
{
	// TODO 可以考虑使用绑定变量的模式实现---比较烦,对大数据量更新操作 有效
	for(int i=0;i<pUpdateValues.size();++i)
	{
		this->update(cTableName,cIndexName,pIndexKeyParams
					  ,sFieldNames,pUpdateValues[i],cExpr,pOtherParams[i]);	
	}
	
	// 返回更新成功的记录数
	return this->effectRowCount();		
}
*/

//Modified By Sunam 2009/11/4 20:00:44
//bool OdbcSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams)
bool OdbcSession::insert(const char *cTableName,const vector<string> &cFieldNames,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	int totalSize = pInputParams.size();
	if (totalSize == 0) return true;	
	//获得sql到到m_sql
	getInsertSQL(cTableName,cFieldNames,pInputParams[0]);
	int t_size;
	for (int i = 0;i<totalSize;i += t_size)
	{
		t_size = BATCHARRAYSIZE;
		if (i + t_size > totalSize)
			t_size = totalSize - i;
        //Modified By Sunam 2009/11/4 19:48:43
		//insertBatch(cTableName,pInputParams,i,t_size);
		insertBatch(cTableName,pInputParams,i,t_size,errVector);
	}
	return true;
}

//Modified By Sunam 2009/11/4 20:00:44
//bool OdbcSession::insert(const char *cTableName	,const vector<InputParams *>&pInputParams)
bool OdbcSession::insert(const char *cTableName	,const vector<InputParams *>&pInputParams,vector<int>* errVector)
{
	int totalSize = pInputParams.size();
	if (totalSize == 0) return 0;	
	//获得sql到到m_sql
	getInsertSQL(cTableName,pInputParams[0]);
	int t_size;
	for (int i = 0;i<totalSize;i += t_size)
	{
		t_size = BATCHARRAYSIZE;
		if (i + t_size > totalSize)
			t_size = totalSize - i;
		//Modified By Sunam 2009/11/4 19:48:43
		//insertBatch(cTableName,pInputParams,i,t_size);
		insertBatch(cTableName,pInputParams,i,t_size,errVector);
	}
	return true;
}
int OdbcSession::update(const char *cTableName
						,const char *cIndexName
						,const vector<InputParams *>pIndexKeyParams
						,vector<string> &sFieldNames
						,const vector<InputParams *>pUpdateValues           
						,const char *cExpr             
						,const vector<InputParams *>pOtherParams)
{
	int updateCount = 0;
	int totalSize = pUpdateValues.size();
	if (totalSize == 0) return 0;
	int t_size;
	for (int i = 0;i<totalSize;i += t_size)
	{
		t_size = BATCHARRAYSIZE;
		if (i + t_size > totalSize)
			t_size = totalSize - i;
		updateCount += updateBatch(cTableName
				                  ,cIndexName
				                  ,pIndexKeyParams
				                  ,sFieldNames
				                  ,pUpdateValues
				                  ,cExpr
				                  ,pOtherParams
				                  ,i //fromPos
				                  ,t_size);//arraySize 
	}
	return updateCount;
}

//批量
int OdbcSession::update(const char *cTableName
		,const char *cIndexName
		,const InputParams *pIndexKeyParams
		,vector<string> &sFieldNames
		,const vector<InputParams *>pUpdateValues           
		,const char *cExpr             
		,const vector<InputParams *>pOtherParams)
{
	int iCount = pUpdateValues.size();
	vector<InputParams *> t_vIndexKeyParam;
	if (pIndexKeyParams != NULL && pIndexKeyParams->m_iNum > 0)
	{
		//t_vIndexKeyParam.push_back(pIndexKeyParams);
		for (int i=0;i<iCount;i++)
		{
			InputParams * t_param = new InputParams(pIndexKeyParams->m_iNum);
			for (int j=0;j<pIndexKeyParams->m_iNum;j++)
			{
				t_param->setValue(pIndexKeyParams->m_varTypes[j],pIndexKeyParams->m_pValues[j]);
			}
			t_vIndexKeyParam.push_back(t_param);
		}
	}
	int updateCount = update(cTableName,cIndexName,t_vIndexKeyParam,sFieldNames,pUpdateValues,cExpr,pOtherParams);
	if (pIndexKeyParams != NULL && pIndexKeyParams->m_iNum > 0)
	{
		for (int i=0;i<iCount;i++)
		{
			delete t_vIndexKeyParam[i];
		}
	}
	return updateCount;
}

bool OdbcSession::fetchRec()
{
	SQLRETURN sqlReturn;
	//FP_BEGIN(SQLFetch)
	//sqlReturn = SQLFetch(m_stmt);
	sqlReturn = SQLFetch(*m_pCurrentStmt);
	//FP_END(SQLFetch)
	
	if( sqlReturn == SQL_NO_DATA_FOUND )  // 查询数据集已经fetch空了
	{
	   #if defined(TIMESTEN) // by chenm 2009-7-13 22:15:01
		SQLFreeStmt(*m_pCurrentStmt,SQL_CLOSE);
	   #endif 	
		return false;	
	}		
	else if( sqlReturn == SQL_ERROR)
	{
		this->throwErr(__FILE__, __LINE__);	
	}
	return true;
}
bool OdbcSession::getNextRec()
{
	//m_pCurrentStmtInfo->m_currentNum = m_pCurrentStmtInfo->m_fetchedRowCount;
	//第一次
	if (m_pCurrentStmtInfo->m_fetchedRowCount pIndexKeyParams->m_iNum);
			for (int j=0;j<pIndexKeyParams->m_iNum;j++)
			{
				t_param->setValue(pIndexKeyParams->m_varTypes[j],pIndexKeyParams->m_pValues[j]);
			}
			t_vIndexKeyParam.push_back(t_param);
		}
	}
	int updateCount = update(cTableName,cIndexName,t_vIndexKeyParam,sFieldNames,pUpdateValues,cExpr,pOtherParams);
	if (pIndexKeyParams != NULL && pIndexKeyParams->m_iNum > 0)
	{
		for (int i=0;i<iCount;i++)
		{
			delete t_vIndexKeyParam[i];
		}
	}
	return updateCount;
}

bool OdbcSession::fetchRec()
{
	SQLRETURN sqlReturn;
	//FP_BEGIN(SQLFetch)
	//sqlReturn = SQLFetch(m_stmt);
	sqlReturn = SQLFetch(*m_pCurrentStmt);
	//FP_END(SQLFetch)
	
	if( sqlReturn == SQL_NO_DATA_FOUND )  // 鏌ヨ