//-------------------------------------------------------------------
// Name			: sybdbi.cpp
// Version		: 0.2.8
// Create		: 2001.4.10
// Update		: 2002.3.9
// Auther		: qianyx@lianchuang.com
// Copyright	: All Rights Reserved.
// Description	: General application program interface to copy data
//					in or out from SYBASE database.
//-------------------------------------------------------------------
#include "dbibase.h"
#include "sybdbi.h"

int FIELDLENGTH=1500;
int DEFAULTARRAYLINE=1000;

//-------------------------------------------------------------------
DbInterface::DbInterface()
{
	context = NULL;
	connection = NULL;
	pdatavect=0;
	perrvect=0;
	fetch=0;
	return;
}

//-------------------------------------------------------------------
DbInterface::~DbInterface()
{
	return;
}


//-------------------------------------------------------------------
int DbInterface::connect(const char *username,const char *password,
		const char *connectName)
{
	CS_INT len;
	CS_BOOL bool1;
	CS_CHAR *Ex_appname=(CS_CHAR *)"sybdbi";
	CS_CHAR *Ex_server=(CS_CHAR *)connectName;
	CS_CHAR *Ex_username=(CS_CHAR *)username;
	CS_CHAR *Ex_password=(CS_CHAR *)password;

	retcode = ex_init(&context);
	if (retcode != CS_SUCCEED)
	{
		DEBUG_PRINTF("ex_init() failed.\n");
		return -1;
	}
	/* 
	** Allocate a connection structure. 
	*/
	retcode=ct_con_alloc(context,&connection);
	if (retcode != CS_SUCCEED)
	{
		DEBUG_PRINTF("ct_con_alloc() failed.\n");
		return -1;
	}

	/*	
	** If a username is defined in example.h, set the CS_USERNAME 
	** property.
	*/
	if(retcode==CS_SUCCEED&&Ex_username!=NULL)
	{
		if((retcode=ct_con_props(connection,CS_SET,CS_USERNAME,
				Ex_username,CS_NULLTERM,NULL))!=CS_SUCCEED)
		{
			DEBUG_PRINTF("ct_con_props(username) failed.\n");
		}
	}

	/*	
	** If a password is defined in example.h, set the CS_PASSWORD 
	** property.
	*/
	if(retcode==CS_SUCCEED&&Ex_password!=NULL)
	{
		if((retcode=ct_con_props(connection,CS_SET,CS_PASSWORD,
				Ex_password,CS_NULLTERM,NULL))!=CS_SUCCEED)
		{
			DEBUG_PRINTF("ct_con_props(passwd) failed.\n");
		}
	}

	/*	
	** Set the CS_APPNAME property.
	*/
	if (retcode == CS_SUCCEED && Ex_appname != NULL)
	{
		if ((retcode = ct_con_props(connection, CS_SET, CS_APPNAME,
				Ex_appname, CS_NULLTERM, NULL)) != CS_SUCCEED)
		{
			DEBUG_PRINTF("ct_con_props(appname) failed.\n");
		}
	}

	/*
	** Enable the bulk login property
	*/
	if (retcode == CS_SUCCEED)
	{
		bool1 = CS_TRUE;
		retcode = ct_con_props(connection, CS_SET, CS_BULK_LOGIN,
				(CS_VOID *)&bool1, CS_UNUSED, NULL);
		if (retcode != CS_SUCCEED)
		{
			 DEBUG_PRINTF("ct_con_props(bulk_login) failed.\n");
		}
	}

	/*	
	** Open a Server connection.
	*/
	if (retcode == CS_SUCCEED)
	{
		len = (Ex_server == NULL) ? 0 : CS_NULLTERM;
		retcode = ct_connect(connection, Ex_server, len);
		if (retcode != CS_SUCCEED)
		{
			DEBUG_PRINTF("ct_connect failed.\n");
		}
	}

	if (retcode != CS_SUCCEED)
	{
		ct_con_drop(connection);
		connection = NULL;
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::disconnect()
{
	if (connection != NULL)
	{
		retcode = ex_con_cleanup(connection, retcode);
		if (retcode != CS_SUCCEED)return -1;
	}
	if (context != NULL)
	{
		retcode = ex_ctx_cleanup(context, retcode);
		if (retcode != CS_SUCCEED)return -1;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::dataBind(StringVector &dataColumnVector,
		StringVector &errorColumnVector)
{
	pdatavect=&dataColumnVector;
	perrvect=&errorColumnVector;
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::executeSql(const char *sqlStatement,int &nTotal,
		int &nSuccess,int &nError)
{
	StringVectorIterator it;
	long i=0;
	long j;
	long s=0;
	int fieldnum=0;
	char *c;
	char sqlstmt[1000]="";
	long pos=0;
	char tablename[50]="";
	int ret;
	
	nTotal=0;
	nSuccess=0;
	nError=0;
	strncpy(sqlstmt,sqlStatement,sizeof(sqlstmt));

	if(strncasecmp(sqlStatement,"SELECT",6)==0)
	{
		if(pdatavect==0)return -1;
		/*
		strncpy(tablename,strcasestr(sqlStatement,"FROM")+5,50);
		for(i=1;i<50;i++)
		{
			if(tablename[i]==' '||tablename[i]=='('||
					tablename[i]=='\t'||tablename[i]=='\0')
			{
				tablename[i]=0;
				break;
			}
		}
		c=strcasestr(sqlstmt,"SELECT");
		while(c<strcasestr(sqlstmt,"FROM"))
		{
			c++;
			if(c[0]=='(')c=strchr(c,')');
			if(c[0]==',')fieldnum++;
		}
		fieldnum++;
		*/
		//pdatavect->clear();
		if(dataoutbind_1(sqlStatement,tablename,DEFAULTARRAYLINE,&s))
				return -1;
		nTotal=m_rowscount;
		DEBUG_PRINTF("nTotal==%d\n",nTotal);
	}
	else
	{
		if(execute_cmd(sqlstmt))
		{
			DEBUG_PRINTF("sql:%s execute failed.\n",sqlstmt);
			return -1;
		}
		ret=atoi(m_retval[0]);
		if(ret<-20000)return ret;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::bulkCopy(const char *sqlStatement,int &nTotal,
		int &nSuccess,int &nError)
{
	StringVectorIterator it;
	long i=0;
	long j;
	long s=0;
	int fieldnum=0;
	char *c;
	char sqlstmt[1000]="";
	long pos=0;
	char tablename[50]="";
	
	if(pdatavect==0)return -1;
	nTotal=0;
	nSuccess=0;
	nError=0;
	strncpy(sqlstmt,sqlStatement,sizeof(sqlstmt));

	if(strncasecmp(sqlStatement,"INSERT",6)==0)
	{
		strncpy(tablename,strcasestr(sqlStatement,"INTO")+5,50);
		for(i=1;i<50;i++)
		{
			if(tablename[i]==' '||tablename[i]=='('||
					tablename[i]=='\t'||tablename[i]=='\0')
			{
				tablename[i]=0;
				break;
			}
		}

		for(c=strchr(sqlStatement,'(')+1;c[0]!=')';c++)
		{
			if(c[0]==',')fieldnum++;
		}
		fieldnum++;
		
		if((fieldbuf=(void *)malloc(DEFAULTARRAYLINE*fieldnum*
				(FIELDLENGTH+1)))==NULL)
		{
			DEBUG_PRINTF("memory alloc failed\n");
			return -1;
		}
		memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));
		i=0;
		for(it=pdatavect->begin();it!=pdatavect->end();it++)
		{
			strcpy((char *)fieldbuf+i*(FIELDLENGTH+1),(*it).c_str());
			if(++i>=DEFAULTARRAYLINE*fieldnum)
			{
				nTotal+=DEFAULTARRAYLINE;
				pos=0;
				if(pos<DEFAULTARRAYLINE)
				{
					datainbind(fieldnum,tablename,DEFAULTARRAYLINE,
							&s);
					nSuccess+=s;
					nError+=DEFAULTARRAYLINE-s;
					pos+=s;
					/*
					while(pos<DEFAULTARRAYLINE)
					{
						nError++;
						for(j=0;j<fieldnum;j++)
						perrvect->push_back((char *)fieldbuf+
								pos*fieldnum*(FIELDLENGTH+1)+
								j*(FIELDLENGTH+1));
						pos++;
					}*/
				}
				i=0;
			}
		}
		if(i)
		{
			pos=0;
			nTotal+=i/fieldnum;
			if(pos<(i/fieldnum))
			{
				datainbind(fieldnum,tablename,i/fieldnum,&s);
				//oracle_datainexec(pos,i/fieldnum,s);
				nSuccess+=s;
				nError+=i/fieldnum-s;
				pos+=s;
				/*
				while(pos<i/fieldnum)
				{
					nError++;
					for(j=0;j<fieldnum;j++)
					perrvect->push_back((char *)fieldbuf+
							pos*fieldnum*(FIELDLENGTH+1)+
							j*(FIELDLENGTH+1));
					pos++;
				}*/
			}
		}
		free(fieldbuf);
	}
	else if(strncmp("SELECT",sqlStatement,6)==0)
	{
		strncpy(tablename,strcasestr(sqlStatement,"FROM")+5,50);
		for(i=1;i<50;i++)
		{
			if(tablename[i]==' '||tablename[i]=='('||
					tablename[i]=='\t'||tablename[i]=='\0')
			{
				tablename[i]=0;
				break;
			}
		}
		c=strcasestr(sqlstmt,"SELECT");
		while(c<strcasestr(sqlstmt,"FROM"))
		{
			c++;
			if(c[0]=='(')c=strchr(c,')');
			if(c[0]==',')fieldnum++;
		}
		fieldnum++;

		if((fieldbuf=(void *)malloc(DEFAULTARRAYLINE*fieldnum*
				(FIELDLENGTH+1)))==NULL)
		{
			DEBUG_PRINTF("memory alloc failed\n");
			return -1;
		}
		memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));

		//pdatavect->clear();
		if(pdatavect->size()<DEFAULTARRAYLINE*fieldnum)
				pdatavect->resize(DEFAULTARRAYLINE*fieldnum);
		//dataoutbind_1(sqlStatement,tablename,DEFAULTARRAYLINE,&s);
		if(dataoutbind(fieldnum,tablename,DEFAULTARRAYLINE,&s))
		{
			free(fieldbuf);
			return -1;
		};
		if(s>DEFAULTARRAYLINE)s%=DEFAULTARRAYLINE;
		nTotal+=s;
		
		for(j=0;j<s*fieldnum;j++)
		{
			//pdatavect->push_back((char *)fieldbuf+
			//		pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
			(*pdatavect)[j]=((char *)fieldbuf+
					pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
		}
		if(nTotal!=DEFAULTARRAYLINE)pdatavect->resize(nTotal*fieldnum);
		DEBUG_PRINTF("nTotal==%d\n",nTotal);
		free(fieldbuf);
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::datainbind(int fieldnum,const char *tablename,
		long count,long *rows)
{
	char *field;
	int i,j;
	CS_INT numrows=0;
	CS_INT datalen[MAXBIND];
	CS_DATAFMT datafmt; /* hold the variable descriptions */
	CS_CHAR *Ex_tabname=(CS_CHAR *)tablename; // table on the server
	long errrows=0;
	int policy=0;
	long policyrows[3]={5000,100,1};
	long policysuccesstimes=0;
	long batchrows=5000;
	long donerows=0;

	*rows=0;
	if(blk_alloc(connection,BLK_VERSION_100,&blkdesc)!=CS_SUCCEED)
	{
		DEBUG_PRINTF("BulkCopyIn: blk_alloc() failed.\n");
		return -1;
	}
	if(blk_init(blkdesc,CS_BLK_IN,Ex_tabname,strlen(Ex_tabname))==
			CS_FAIL)
	{
		DEBUG_PRINTF("BulkCopyIn: blk_init() failed.\n");
		return -1;
	}
	datafmt.locale = 0;
	datafmt.count = 1;
	datafmt.maxlength = 20;
	datafmt.datatype = CS_CHAR_TYPE;
	//datafmt.format = CS_FMT_NULLTERM;
	DEBUG_PRINTF("count:%d fieldnum:%d\n",count,fieldnum);

	while(donerows<count)
	{
		//batchrows=policyrows[policy];
		batchrows=count<donerows+batchrows?count-donerows:batchrows;
		DEBUG_PRINTF("batchrows=%ld\n",batchrows);
		errrows=0;
		for(j=donerows;j<donerows+batchrows;j++)
		{
			for(i=0;i<fieldnum;i++)
			{
				field=(char *)fieldbuf+j*fieldnum*(FIELDLENGTH+1)+
						i*(FIELDLENGTH+1);
				datalen[i] = strlen(field);
				if (blk_bind(blkdesc,i+1,&datafmt,(CS_VOID *)field,
						 &datalen[i],NULL)!=CS_SUCCEED)
				{
					DEBUG_PRINTF("fieldno:%d bind error\n",i);
					DEBUG_PRINTF("BulkCopyIn:blk_bind(2) failed.\n");
					//if(blk_bind(blkdesc,CS_UNUSED,NULL,NULL,NULL,
					//NULL)==CS_FAIL)
					//{
					//	DEBUG_PRINTF("BulkCopyIn: blk_bind(clear) failed.\n");
					//	return -1;
					//}
					if (blk_done(blkdesc,CS_BLK_CANCEL,&numrows)==
							CS_FAIL)
					{
						DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
						return -1;
					}
					if (blk_drop(blkdesc) == CS_FAIL)
					{
						DEBUG_PRINTF("BulkCopyIn: blk_drop() failed.\n");
						return -1;
					}
					return -1;
				}
			}
			if (blk_rowxfer (blkdesc) == CS_FAIL)
			{
				for(i=0;i<fieldnum;i++)
				{
					perrvect->push_back((char *)fieldbuf+
							j*fieldnum*(FIELDLENGTH+1)+
							i*(FIELDLENGTH+1));
					errrows++;
				}
				DEBUG_PRINTF("BulkCopyIn: blk_rowxfer() failed.\n");
				continue;
			}
		}
		if (blk_done(blkdesc, CS_BLK_BATCH, &numrows) == CS_FAIL)
		{
			DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
			//return -1;
		}
		if(numrows==0)
		{
			policysuccesstimes=0;
			/*
			if(policy==2)
			{
				for(i=0;i<fieldnum;i++)
				{
					perrvect->push_back((char *)fieldbuf+
							donerows*fieldnum*(FIELDLENGTH+1)+
							i*(FIELDLENGTH+1));
				}
				donerows++;
			}
			if(policy<2)policy++;
			*/
			if(batchrows==1)
			{
				for(i=0;i<fieldnum;i++)
				{
					perrvect->push_back((char *)fieldbuf+
							donerows*fieldnum*(FIELDLENGTH+1)+
							i*(FIELDLENGTH+1));
				}
				batchrows++;
				donerows++;
			}
			batchrows/=2;
		}
		else
		{
			policysuccesstimes++;
			/*
			if(policysuccesstimes>5&&policy>0)
			{
				policysuccesstimes=0;
				policy--;
			}
			*/
			batchrows*=2;
			if(batchrows>5000)batchrows=5000;
	 		donerows+=numrows+errrows;
			*rows+=numrows;
		}
	}
	if (blk_done(blkdesc, CS_BLK_ALL, &numrows) == CS_FAIL)
	{
		DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
		return -1;
	}
	if (blk_drop(blkdesc) == CS_FAIL)
	{
		DEBUG_PRINTF("BulkCopyIn: blk_drop() failed.\n");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::dataoutbind_1(const char *sql,const char *tablename,
		long count,long *rows)
{
	CS_RETCODE retcode;
	CS_INT restype;
	static CS_COMMAND *cmd;

	DEBUG_PRINTF("fetch=%d\n",fetch);	
	if(fetch==0)
	{
		if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
		{
			ex_error("ex_execute_cmd: ct_cmd_alloc() failed");
			return retcode;
		}

		if ((retcode = ct_command(cmd,CS_LANG_CMD,(CS_CHAR *)sql,
				CS_NULLTERM,CS_UNUSED)) != CS_SUCCEED)
		{
			ex_error("ex_execute_cmd: ct_command() failed");
			(void)ct_cmd_drop(cmd);
			return retcode;
		}
		if ((retcode = ct_send(cmd)) != CS_SUCCEED)
		{
			ex_error("ex_execute_cmd: ct_send() failed");
			(void)ct_cmd_drop(cmd);
			return retcode;
		}
		retcode = ct_results(cmd, &restype);
	}
	fetch_data(cmd,1);
	if(m_rowscount>=DEFAULTARRAYLINE)fetch=1;
	else fetch=0;
	DEBUG_PRINTF("fetch=%d\n",fetch);	
	if(fetch==0)handle_results(cmd);
	//DEBUG_PRINTF("----end----\n");
	return 0;
}


//-------------------------------------------------------------------
int DbInterface::dataoutbind(int fieldnum,const char *tablename,
		long count,long *rows)
{
	static int c=0;
	char *field;
	int i,j;
	CS_INT ret;
	CS_INT numrows=0;
	CS_INT datalen[MAXBIND];
	CS_DATAFMT	datafmt;	/* hold the variable descriptions */
	CS_CHAR	*Ex_tabname= (CS_CHAR *)tablename; // table on the server
	
	*rows=0;

	if(c==0)
	{
		c=1;
		if(blk_alloc(connection,BLK_VERSION_100,&blkdesc)!=
				CS_SUCCEED)
		{
			DEBUG_PRINTF("BulkCopyIn: blk_alloc() failed.\n");
			return -1;
		}

		if(blk_init(blkdesc,CS_BLK_OUT,Ex_tabname,
				strlen(Ex_tabname))==CS_FAIL)
		{
			DEBUG_PRINTF("BulkCopyIn: blk_init() failed.\n");
			return -1;
		}
	}
	datafmt.locale = 0;
	datafmt.count = 1;
	datafmt.maxlength = FIELDLENGTH+1;
	datafmt.datatype = CS_CHAR_TYPE;
	datafmt.format = CS_FMT_NULLTERM;
	for(j=0;j<count;j++)
	{
		for(i=0;i<fieldnum;i++)
		{
			field=(char *)fieldbuf+j*fieldnum*(FIELDLENGTH+1)+
					i*(FIELDLENGTH+1);
			datalen[i] = FIELDLENGTH+1;
			if (blk_bind(blkdesc, i+1, &datafmt, (CS_VOID *)field,
					 &datalen[i], NULL) != CS_SUCCEED)
			{
				DEBUG_PRINTF("fieldno:%d bind error\n",i);
				DEBUG_PRINTF("BulkCopyIn: blk_bind(out) failed.\n");
				//if(blk_bind(blkdesc,CS_UNUSED,NULL,NULL,NULL,
				//		NULL)==CS_FAIL)
				//{
				//	DEBUG_PRINTF("BulkCopyIn: blk_bind(clear) failed.\n");
				//	return -1;
				//}
				if(blk_done(blkdesc,CS_BLK_CANCEL,&numrows)==
						CS_FAIL)
				{
					DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
					return -1;
				}
				if (blk_drop(blkdesc) == CS_FAIL)
				{
					DEBUG_PRINTF("BulkCopyIn: blk_drop() failed.\n");
					return -1;
				}
				return -1;
			}
		}
		if ((ret=blk_rowxfer (blkdesc)) == CS_FAIL)
		{
			DEBUG_PRINTF("BulkCopyIn: blk_rowxfer() failed.\n");
			//if (blk_bind(blkdesc,CS_UNUSED,NULL,NULL,NULL,NULL)==
			//		CS_FAIL)
			//{
			//	DEBUG_PRINTF("BulkCopyIn: blk_bind(clear) failed.\n");
			//	return -1;
			//}
			if(blk_done(blkdesc,CS_BLK_CANCEL,&numrows)==CS_FAIL)
			{
				DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
				return -1;
			}
			if (blk_drop(blkdesc) == CS_FAIL)
			{
				DEBUG_PRINTF("BulkCopyIn: blk_drop() failed.\n");
				return -1;
			}
			return -1;
		}
		if(ret==CS_END_DATA)break;
	}
	if(ret==CS_END_DATA)
	{
		if(j)
		{
			if(blk_done(blkdesc,CS_BLK_ALL,&numrows)==CS_FAIL)
			{
				DEBUG_PRINTF("BulkCopyIn: blk_done(1) failed.\n");
				return -1;
			}
			*rows=numrows;
		}
		else
		{
			if(blk_done(blkdesc,CS_BLK_CANCEL,&numrows)==CS_FAIL)
			{
				DEBUG_PRINTF("BulkCopyIn: blk_done() failed.\n");
				return -1;
			}
		}
		if (blk_drop(blkdesc) == CS_FAIL)
		{
			DEBUG_PRINTF("BulkCopyIn: blk_drop() failed.\n");
			return -1;
		}
		c=0;
	}
	else
	{
		numrows=DEFAULTARRAYLINE;
	}

	*rows=numrows;

	return 0;
}

//-------------------------------------------------------------------
int DbInterface::callProcedure(const char *procedureName,
		const int parameterNum,PROCPARAM *parameters)
{
	CS_RETCODE retcode;
	CS_DATAFMT datafmt;
	CS_COMMAND * cmd_ptr;
	char rpc_name[100];
	int i,j;

	strncpy(rpc_name, procedureName,sizeof(rpc_name));
	if ((retcode = ct_cmd_alloc(connection, &cmd_ptr)) != CS_SUCCEED)
	{
		DEBUG_PRINTF("EXECSQL: ct_cmd_alloc() failed\n");
		return -1;
	}
	// Send the RPC command for our stored procedure.
	if((retcode=ct_command(cmd_ptr,CS_RPC_CMD,rpc_name,CS_NULLTERM,
					CS_NO_RECOMPILE)) != CS_SUCCEED)
	{
		DEBUG_PRINTF("EXECSQL: ct_command() failed\n");
		return -1;
	}

	for(i=0;i<parameterNum;i++)
	{
		if(parameters[i].type==PARAM_TYPE_STR)
		{
			// Clear and setup the CS_DATAFMT structure, then pass
			// each of the parameters for the RPC.
			memset(&datafmt, 0, CS_SIZEOF(datafmt));
			//sprintf(datafmt.name,"@%s",parameters[i].name);
			datafmt.namelen = CS_NULLTERM;			// Length of name
			if(parameters[i].action==PARAM_ACT_IN)
					datafmt.status = CS_INPUTVALUE;	// Status symbols
			if(parameters[i].action==PARAM_ACT_OUT)
					datafmt.status = CS_RETURN;		// Status symbols
			datafmt.datatype = CS_CHAR_TYPE;		// Data’s datatype
			datafmt.maxlength = CS_UNUSED;			// Data max length
			datafmt.precision = CS_UNUSED;			// Data precision
			datafmt.locale = NULL;					// Locale information
			// datafmt.scale;						// Scale of data
			// datafmt.format = CS_FMT_NULLTERM;	// Format symbols
			// datafmt.count;
			// datafmt.usertype;					// Svr user-def’d type

			if ((retcode = ct_param(cmd_ptr,&datafmt,
					(CS_VOID *)parameters[i].strValue,
					80, CS_UNUSED)) != CS_SUCCEED)	//strlen(pl[i].value.c
			{
				DEBUG_PRINTF("EXECSQL: ct_param(char) failed\n");
				return -1;
			}
		}
		if(parameters[i].type==PARAM_TYPE_INT)
		{
			// Clear and setup the CS_DATAFMT structure, then pass
			// each of the parameters for the RPC.
			memset(&datafmt, 0, CS_SIZEOF(datafmt));
			//sprintf(datafmt.name,"@%s",parameters[i].name);
			datafmt.namelen = CS_NULLTERM;			// Length of name
			if(parameters[i].action==PARAM_ACT_IN)
					datafmt.status = CS_INPUTVALUE; // Status symbols
			if(parameters[i].action==PARAM_ACT_OUT)
					datafmt.status = CS_RETURN;		// Status symbols
			datafmt.datatype = CS_INT_TYPE;			// Data’s datatype
			datafmt.maxlength = CS_UNUSED;			// Data max length
			datafmt.precision = CS_UNUSED;			// Data precision
			datafmt.locale = NULL;					// Locale information
			// datafmt.scale;						// Scale of data
			// datafmt.format = CS_FMT_NULLTERM;	// Format symbols
			// datafmt.count;
			// datafmt.usertype;					// Svr user-def’d type

			if ((retcode=ct_param(cmd_ptr,&datafmt,
					(CS_VOID *)&parameters[i].intValue,
					sizeof(CS_INT_TYPE), CS_UNUSED)) != CS_SUCCEED)
			{
				DEBUG_PRINTF("EXECSQL: ct_param(char) failed\n");
				return -1;
			}
		}
		if(parameters[i].type==PARAM_TYPE_FLOAT)
		{
			// Clear and setup the CS_DATAFMT structure, then pass
			// each of the parameters for the RPC.
			memset(&datafmt, 0, CS_SIZEOF(datafmt));
			//sprintf(datafmt.name,"@%s",parameters[i].name);
			datafmt.namelen = CS_NULLTERM;			// Length of name
			if(parameters[i].action==PARAM_ACT_IN)
					datafmt.status = CS_INPUTVALUE; // Status symbols
			if(parameters[i].action==PARAM_ACT_OUT)
					datafmt.status = CS_RETURN;		// Status symbols
			datafmt.datatype = CS_FLOAT_TYPE;		// Data’s datatype
			datafmt.maxlength = CS_UNUSED;			// Data max length
			datafmt.precision = CS_UNUSED;			// Data precision
			datafmt.locale = NULL;					// Locale information
			// datafmt.scale;						// Scale of data
			// datafmt.format = CS_FMT_NULLTERM;	// Format symbols
			// datafmt.count;
			// datafmt.usertype;					// Svr user-def’d type

			if ((retcode=ct_param(cmd_ptr,&datafmt,
					(CS_VOID *)&parameters[i].floatValue,
					sizeof(CS_FLOAT_TYPE),CS_UNUSED))!=CS_SUCCEED)
			{
				DEBUG_PRINTF("EXECSQL: ct_param(char) failed\n");
				return -1;
			}
		}
	}
	if (ct_send(cmd_ptr) != CS_SUCCEED)
	{
		DEBUG_PRINTF("EXECSQL: ct_send() failed\n");
		return -1;
	}

	if (handle_results(cmd_ptr))
		return -1;

	for(i=0,j=0;i<parameterNum;i++)
	{
		if(parameters[i].action==PARAM_ACT_OUT)
		{
		 	if(parameters[i].type==PARAM_TYPE_STR)
			{
				strncpy(parameters[i].strValue,m_paramval[j],sizeof(parameters[i].strValue));
			}
		 	if(parameters[i].type==PARAM_TYPE_INT)
			{
				parameters[i].intValue=atoi(m_paramval[j]);
			}
		 	if(parameters[i].type==PARAM_TYPE_FLOAT)
			{
				parameters[i].floatValue=atof(m_paramval[j]);
			}
			j++;
		}
	}

	retcode = ct_cmd_drop(cmd_ptr);
	if (retcode != CS_SUCCEED)return -1;
	return 0;

}


//-------------------------------------------------------------------
int DbInterface::handle_results(CS_COMMAND * cmd_ptr)
{
	CS_INT result_type;
	CS_RETCODE retcode;
	CS_RETCODE results_ret;
	int i;
	
	while ((results_ret = ct_results(cmd_ptr, &result_type)) ==
			CS_SUCCEED)
	// OPTIONAL:ct_res_info to get current command number.
	{
		switch ((int)result_type)
		{
			// Values of result_type that indicate fetchable results:
			// for each column:
			//	ct_bind
			//	end for
			//		while ct_fetch is returning rows
			//			process each row
			//		end while
			//	check ct_fetch’s final return code
			// end case row results
			case CS_COMPUTE_RESULT:
				if (fetch_data(cmd_ptr,0))
				;
				break;

			case CS_CURSOR_RESULT:
				if (fetch_data(cmd_ptr,0))
				;
				break;

			case CS_PARAM_RESULT:
				if (fetch_data(cmd_ptr,0))
				;
				for (i = 0; i < m_colscount; i++)
					strcpy(m_paramval[i], m_tmpval[i]);
				break;

			case CS_ROW_RESULT:
				if (fetch_data(cmd_ptr, 1))
				;
				for (i = 0; i < m_colscount; i++)
					strcpy(m_colval[i], m_tmpval[i]);
				break;

			case CS_STATUS_RESULT:
				if (fetch_data(cmd_ptr,0))
				;
				for (i = 0; i < m_colscount; i++)
					strcpy(m_retval[i], m_tmpval[i]);
				break;

			// Values of result_type that indicate non-fetchable 
			// results:
			case CS_MSG_RESULT:
				retcode = ct_res_info(cmd_ptr, CS_MSGTYPE,
						(CS_VOID *)&m_msgid, CS_UNUSED, NULL);
				if (retcode != CS_SUCCEED)
				{
					DEBUG_PRINTF("handle_results: ct_res_info(msgtype) failed");
					return -1;
				}
				DEBUG_PRINTF("ct_result returned CS_MSG_RESULT where msg id = %d",
						m_msgid);
				break;

			case CS_COMPUTEFMT_RESULT:
			case CS_ROWFMT_RESULT:
			case CS_DESCRIBE_RESULT:
				break;
			
			// Other values of result_type:
			case CS_CMD_DONE:
				// the logical command has been completely processed.
				// OPTIONAL: ct_res_info to get the number of rows 
				// affected by the current command.
				break;
				
			case CS_CMD_SUCCEED:
				// We executed a command that never returns rows.
				break;

			case CS_CMD_FAIL:
				// server encountered an error while processing 
				// command.
				if (fetch_data(cmd_ptr,0))
				;
				for (i = 0; i < m_colscount; i++)
					strcpy(m_retval[i], m_tmpval[i]);
				break;
			default:				// we got something unexpected.
				return -1;
				break;
		}
	}

	// finished processing results.
	// check the return value of ct_results() to see if everything 
	// went okay.
	switch ((int)results_ret)
	{
		case CS_SUCCEED:	//A result set is available for processing.
		case CS_END_RESULTS:// All results have been completely processed.
			break;
		
		case CS_FAIL:
			// The routine failed; any remaining results are no 
			// longer available. must call ct_cancel with type as 
			// CS_CANCEL_ALL before using the affected command 
			// structure to send another command. If ct_cancel 
			// returns CS_FAIL, must call ct_close(CS_FORCE_CLOSE) 
			// to force the connection closed. Terminate results 
			// processing and break out of the results loop
			retcode = ct_cancel(NULL, cmd_ptr, CS_CANCEL_ALL);
			if (retcode != CS_SUCCEED)
			{
				DEBUG_PRINTF("execute_cmd: ct_cancel() failed");
			}
			if (retcode == CS_FAIL)
				return -1;
			else
				return -1;
			break;
		
		case CS_CANCELED:
			// Results have been canceled.
			break;

		// Asynchronous operation. 
		case CS_PENDING:// Asynchronous network I/O is in effect. 
		case CS_BUSY:
			// An asynchronous operation is already pending for this 
			// connection. 
		default:				
			return -1;
			break;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::fetch_data(CS_COMMAND * cmd_ptr, int nStoreType)
{
	static CS_INT num_cols=0;
	CS_RETCODE retcode;
	CS_INT row_count = 0;
	CS_INT rows_read;
	int		 i;

	m_rowscount = row_count;
	if(fetch==0)
	{
		// Find out how many columns there are in this result set.
		retcode = ct_res_info(cmd_ptr, CS_NUMDATA, &num_cols,
				CS_UNUSED,NULL);
		if (retcode != CS_SUCCEED)
		{
			DEBUG_PRINTF("fetch_data: ct_res_info() failed");
			return -1;
		}

		// Make sure we have at least one column
		if (num_cols <= 0)
		{
			DEBUG_PRINTF("fetch_data: ct_res_info() returned zero columns");
			return -1;
		}
		m_colscount = num_cols;

		// Loop through the columns getting a description of each one
		// and binding each one to a program variable.
		for (i = 0; i < num_cols; i++)
		{
			// Get the column description.
			retcode = ct_describe(cmd_ptr, (i + 1), &m_colfmts[i]);
			if (retcode != CS_SUCCEED)
			{
				DEBUG_PRINTF("fetch_data: ct_describe() failed");
				break;
			}

			// update the datafmt structure to indicate that we want 
			// the results in a null terminated character string.
			m_colfmts[i].maxlength = COLSTR_LEN;
			m_colfmts[i].datatype = CS_CHAR_TYPE;
			m_colfmts[i].format= CS_FMT_NULLTERM;

			// Now bind.
			retcode = ct_bind(cmd_ptr, (i + 1), &m_colfmts[i],
					m_tmpval[i], &m_datalength[i], &m_indicator[i]);
			if (retcode != CS_SUCCEED)
			{
				DEBUG_PRINTF("fetch_data: ct_bind() failed");
				break;
			}
		}

		if (retcode != CS_SUCCEED)
			return -1;
	}
	if(pdatavect->size()<DEFAULTARRAYLINE*num_cols)
			pdatavect->resize(DEFAULTARRAYLINE*num_cols);
	// Fetch the rows. Loop while ct_fetch() returns CS_SUCCEED or 
	// CS_ROW_FAIL
	while(((retcode=ct_fetch(cmd_ptr,CS_UNUSED,CS_UNUSED,CS_UNUSED,
			&rows_read))==CS_SUCCEED)||(retcode==CS_ROW_FAIL))
	{
		// Increment our row count by the number of rows just 
		// fetched.

		// Check if we hit a recoverable error.
		if (retcode == CS_ROW_FAIL)
		{
			DEBUG_PRINTF("Error on row %d.\n", row_count);
		}

		// we have a row, store and let's print it.
		if (nStoreType)
		{
			for(i=0;i<num_cols;i++)
			{
				//pdatavect->push_back(m_tmpval[i]);
				(*pdatavect)[row_count*num_cols+i]=m_tmpval[i];
			}
		}
		row_count = row_count + rows_read;
		m_rowscount = row_count;
		if(m_rowscount>=DEFAULTARRAYLINE)return 0;
	}
	pdatavect->resize(row_count*num_cols);

	// We're done processing rows. Let's check the final return 
	// value of ct_fetch().
	switch ((int)retcode)
	{
		case CS_END_DATA:
			return 0;
			break;

		case CS_FAIL:
			DEBUG_PRINTF("fetch_data: ct_fetch() failed");
			return -1;
			break;

		default:
			DEBUG_PRINTF("fetch_data: ct_fetch() returned an expected retcode");
			return -1;
			break;
	}
	return 0;
}

//-------------------------------------------------------------------
int DbInterface::useDb(const char *dbName)
{
	CS_COMMAND * cmd_ptr;
	char szStatement[80];

	CS_RETCODE retcode;

	strcpy(szStatement, "use ");
	strcat(szStatement, dbName);

	if (ct_cmd_alloc(connection, &cmd_ptr) != CS_SUCCEED)
		return -1;

	if (ct_command(cmd_ptr, CS_LANG_CMD, szStatement, CS_NULLTERM,
			CS_UNUSED)!=CS_SUCCEED)
		return -1;

	if (ct_send(cmd_ptr) != CS_SUCCEED)
		return -1;

	if (handle_results(cmd_ptr))
		return -1;

	if ((retcode = ct_cmd_drop(cmd_ptr)) != CS_SUCCEED)
		return -1;
	
	return 0;	 
}

//-------------------------------------------------------------------
int DbInterface::execute_cmd(const char *sqlstmt)
{
	CS_COMMAND * cmd_ptr;
	CS_RETCODE retcode;
	char szStatement[80];

	strncpy(szStatement,sqlstmt,sizeof(szStatement));

	if (ct_cmd_alloc(connection, &cmd_ptr) != CS_SUCCEED)
		return -1;

	if (ct_command(cmd_ptr, CS_LANG_CMD, szStatement, CS_NULLTERM,
			CS_UNUSED)!=CS_SUCCEED)
		return -1;

	if (ct_send(cmd_ptr) != CS_SUCCEED)
		return -1;

	if (handle_results(cmd_ptr))
		return -1;

	if ((retcode = ct_cmd_drop(cmd_ptr)) != CS_SUCCEED)
		return -1;

	return 0;
}

//-------------------------------------------------------------------
int DbInterface::beginTransaction()
{
	CS_BOOL paramvalue;
	paramvalue = CS_TRUE;
	ct_options(connection, CS_SET, CS_OPT_CHAINXACTS, &paramvalue,
			CS_UNUSED, NULL);

	if(ex_execute_cmd(connection,"begin transaction")!=CS_SUCCEED)
	{
		DEBUG_PRINTF("sql:%s execute failed.\n","begin transaction");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------
int DbInterface::endTransaction()
{
	CS_BOOL paramvalue;
	paramvalue = CS_FALSE;
	ct_options(connection, CS_SET, CS_OPT_CHAINXACTS, &paramvalue,
			CS_UNUSED, NULL);

	return 0;
}

//-------------------------------------------------------------------
int DbInterface::commitTransaction()
{
	if(ex_execute_cmd(connection,"commit transaction")!=CS_SUCCEED)
	{
		DEBUG_PRINTF("sql:%s execute failed.\n",
				"commit transaction");
		return -1;
	}

	return 0;
}


//-------------------------------------------------------------------
int DbInterface::rollbackTransaction()
{
	if(ex_execute_cmd(connection,"rollback transaction")!=CS_SUCCEED)
	{
		DEBUG_PRINTF("sql:%s execute failed.\n",
				"rollback transaction");
		return -1;
	}

	return 0;
}


int DbInterface::errorCode()
{
	return m_errorCode;
}

const char * DbInterface::errorMessage()
{
	return m_errorMessage;
}

int DbInterface::setFieldLength(int len)
{
	if(len<=0)return -1;
	*(int *)&FIELDLENGTH=len;
	return 0;
}


int DbInterface::setArrayLine(int line)
{
	if(line<=0)return -1;
	*(int *)&DEFAULTARRAYLINE=line;
	return 0;
}

//------------------------------ THE END ----------------------------


