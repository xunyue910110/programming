//-------------------------------------------------------------------
// Name      : oradbi.h
// Version    : 0.2.8
// Create    : 2001.4.10
// Update    : 2002.3.9
// Auther    : qianyx@lianchuang.com
// Copyright  : All Rights Reserved.
// Description  : Should be included in DBI application which use 
//          ORACLE database.
//-------------------------------------------------------------------
#ifndef ORADBI_H
#define ORADBI_H

#include "dbicommon.h"

#ifndef __STDC__
#define __STDC__ 1
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#include <oci.h>
#ifdef __cplusplus
}
#endif

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
  int executeSql(const char *sqlStatement,int &nTotal,
      int &nSuccess,int &nError);
  int bulkCopy(const char *sqlStatement,int &nTotal,int &nSuccess,
      int &nError);
  int useDb(const char *dbName);
  int callProcedure(const char *procedureName,
      const int parameterNum,PROCPARAM *parameters);
  int beginTransaction();
  int endTransaction();
  int commitTransaction();
  int rollbackTransaction();

  int errorCode();
  const char *errorMessage();
  int setFieldLength(int len);
  int setArrayLine(int line);
  
  //为支持多线程而修改 2005-12-1 gaojf
  int m_executeSql_fetch;
  int m_bulkCopy_fetch;
  int m_dataoutexec_totalrow;
  int m_dataoutexec_c;
protected:
  StringVector *perrvect;
  StringVector *pdatavect;
  OCIEnv *envhp;
  OCIError *errhp;
  OCISvcCtx *svchp;
  OCIStmt *stmthp;
  OCIDefine *defnp[MAXBIND];
  OCIBind *bndhp[MAXBIND];
  sb2 indp;
  sword status;
  text *fieldbuf;
  int transflag;

private:
  void checkerr();
  int sqlbind(const char *sqlstmt);
  int datainbind(int fieldnum);
  int dataoutbind(int fieldnum);
  int datainexec(long pos,long count,long &row);
  int dataoutexec(long pos,long count,long &row);
  int execute_cmd();
  
  int m_errorCode;
  char m_errorMessage[1000];
};

#endif //ORADBI_H

//------------------------------ THE END ----------------------------

