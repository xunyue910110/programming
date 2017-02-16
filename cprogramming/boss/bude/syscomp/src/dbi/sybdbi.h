//-------------------------------------------------------------------
// Name			: sybdbi.h
// Version		: 0.2.8
// Create		: 2001.4.10
// Update		: 2002.3.9
// Auther		: qianyx@lianchuang.com
// Copyright	: All Rights Reserved.
// Description	: Should be included in DBI application which use 
//					SYBASE database.
//-------------------------------------------------------------------
#ifndef SYBDBI_H
#define SYBDBI_H

#include "dbicommon.h"
#include <ctpublic.h>
#include <bkpublic.h>
#include "example.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include "exutils.h"
#ifdef __cplusplus
}
#endif
#define MAXCOLUMNS	50
#define COLSTR_LEN	50

class DbInterface
{
public:
	DbInterface();
	~DbInterface();
	int connect(const char *username,const char *password,
			const char *connectName);
	int disconnect();
	int dataBind(StringVector &dataColumnVector,
			StringVector &errorColumnVector);
	int executeSql(const char *sqlStatement,int &nTotal,int &nSuccess,
			int &nError);
	int bulkCopy(const char *sqlStatement,int &nTotal,int &nSuccess,
			int &nError);
	int useDb(const char *dbName);
	int callProcedure(const char *procedureName,const int parameterNum,
			PROCPARAM *parameters);
	int beginTransaction();
	int endTransaction();
	int commitTransaction();
	int rollbackTransaction();

	int errorCode();
	const char *errorMessage();
	int setFieldLength(int len);
	int setArrayLine(int line);

protected:
	StringVector *perrvect;
	StringVector *pdatavect;
	CS_CONTEXT *context;
	CS_CONNECTION *connection;
	CS_RETCODE retcode;
	CS_BLKDESC *blkdesc;
	void *fieldbuf;

private:
	int datainbind(int fieldnum,const char *tablename,long count,long *rows);
	int dataoutbind(int fieldnum,const char *tablename,long count,long *rows);
	int dataoutbind_1(const char *sql,const char *tablename,long count,
			long *rows);
	int datainexec(long pos,long count,long *row);
	int dataoutexec(long pos,long count,long *row);
	int handle_results(CS_COMMAND * cmd_ptr);
	int fetch_data(CS_COMMAND * cmd_ptr, int nStoreType);
	int execute_cmd(const char *sqlstmt);

    char         m_colval[MAXCOLUMNS][COLSTR_LEN];
    unsigned int m_rowscount;
    unsigned int m_colscount;
    CS_DATAFMT   m_colfmts[MAXCOLUMNS];
    CS_INT       m_datalength[MAXCOLUMNS];
    CS_SMALLINT  m_indicator[MAXCOLUMNS];
    char         m_tmpval[MAXCOLUMNS][COLSTR_LEN];
    char         m_paramval[MAXCOLUMNS][COLSTR_LEN];
    char         m_retval[MAXCOLUMNS][COLSTR_LEN];
    CS_SMALLINT  m_msgid;
	int fetch;

	int m_errorCode;
	char m_errorMessage[1000];
};

#endif // SYBDBI_H

//------------------------------ THE END ----------------------------

