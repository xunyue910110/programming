//-------------------------------------------------------------------
// Name      : oradbi.cpp
// Version    : 0.2.8
// Create    : 2001.4.10
// Update    : 2002.3.9
// Auther    : qianyx@lianchuang.com
// Copyright  : All Rights Reserved.
// Description  : General application program interface to copy data
//          in or out from ORACLE database.
//-------------------------------------------------------------------
#include "dbibase.h"
#include "oradbi.h"

int FIELDLENGTH=1500;
int DEFAULTARRAYLINE=1000;

//-------------------------------------------------------------------
DbInterface::DbInterface()
{
  pdatavect=0;
  perrvect=0;
  //为支持多线程而增加
  m_executeSql_fetch=0;
  m_bulkCopy_fetch=0;
  m_dataoutexec_totalrow=0;
  m_dataoutexec_c=0;
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
  if(status=OCIInitialize((ub4)OCI_DEFAULT,(dvoid *)0,
      /*(extern "C" dvoid * (*)(dvoid *,size_t))*/0,
      /*(dvoid * (*)(dvoid *,dvoid *,size_t))*/0,
      /*(void (*)(dvoid *,dvoid *))*/0))
  {
    DEBUG_PRINTF("FAILED: OCIInitialize()\n");
    checkerr();
    return OCI_ERROR;
  }

  if(status=OCIEnvInit((OCIEnv **)&envhp,OCI_DEFAULT,(size_t)0,
      (dvoid **)0))
  {
    DEBUG_PRINTF("FAILED: OCIEnvInit()\n");
    checkerr();
    return OCI_ERROR;
  }

  if(status=OCIHandleAlloc((dvoid *)envhp,(dvoid **)&errhp,
      OCI_HTYPE_ERROR,(size_t)0,(dvoid **)0))
  {
    DEBUG_PRINTF("FAILED: OCIHandleAlloc()\n");
    checkerr();
    return OCI_ERROR;
  }
  if(status=OCIHandleAlloc((dvoid *)envhp,(dvoid **)&stmthp, 
      OCI_HTYPE_STMT,(size_t)0,(dvoid **)0))
  {
    DEBUG_PRINTF("FAILED: OCIHandleAlloc()\n");
    checkerr();
    return OCI_ERROR;
  }

  if(status=OCILogon(envhp,errhp,&svchp,(unsigned char *)username,
      strlen(username),(unsigned char *)password,
      strlen(password),(unsigned char *)connectName,
      strlen(connectName)))
  {
    DEBUG_PRINTF("FAILED: OCILogon()\n");
    checkerr();
    return OCI_ERROR;
  }
  transflag=0;
  return 0;
}

//-------------------------------------------------------------------
int DbInterface::disconnect()
{
  //if(errhp)(void)OCIServerDetach(srvhp,errhp,OCI_DEFAULT);
  //if(srvhp)checkerr(errhp,OCIHandleFree((dvoid *)srvhp,
  //    OCI_HTYPE_SERVER));
  OCILogoff(svchp,errhp);
  if(stmthp)(void)OCIHandleFree((dvoid *)stmthp,OCI_HTYPE_STMT);
  if(svchp)(void)OCIHandleFree((dvoid *)svchp,OCI_HTYPE_SVCCTX);
  if(errhp)(void)OCIHandleFree((dvoid *)errhp,OCI_HTYPE_ERROR);
  if(envhp)(void)OCIHandleFree((dvoid *)envhp,OCI_HTYPE_ENV);
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
  string sss;
  char *c;
  char sqlstmt[2000]="";
  long pos=0;
  char tablename[50]="";
  //static int fetch=0; //静态变量不支持多线程该为成员变量m_executeSql_fetch 
  //以下该方法中均使用m_executeSql_fetch替换fetch 2005-12-1 gaojf
  int ret;
  int pare=0;
  
  nTotal=0;
  nSuccess=0;
  nError=0;
  strncpy(sqlstmt,sqlStatement,sizeof(sqlstmt));

  for(c=sqlstmt;c[0]==' ';c++);
  if(strncasecmp(c,"SELECT",6)==0)
  {
    if(pdatavect==0)return -1;
    strncpy(tablename,strcasestr(sqlStatement,"FROM")+5,sizeof(tablename));
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
      //if(c[0]=='(')c=(char *)strchr(c,')');
      if(c[0]=='('){pare++;continue;}
      if(c[0]==')'){pare--;continue;}
      if(pare)continue;
      if(c[0]==',')fieldnum++;
    }
    fieldnum++;
    if((fieldbuf=(text *)malloc(DEFAULTARRAYLINE*fieldnum*
        (FIELDLENGTH+1)))==NULL)
    {
      DEBUG_PRINTF("memory alloc failed\n");
      return -1;
    }
    DEBUG_PRINTF("alloc memory size %ld\n",DEFAULTARRAYLINE*
        fieldnum*(FIELDLENGTH+1));
    memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));
    /*
    if(fetch==0)
    {
      fetch=1;
      sqlbind(sqlstmt);
    }
    */
    if(m_executeSql_fetch==0)
    {
      m_executeSql_fetch=1;
      sqlbind(sqlstmt);
    }
    dataoutbind(fieldnum);
    //pdatavect->clear();
    if(pdatavect->size()<DEFAULTARRAYLINE*fieldnum)
        pdatavect->resize(DEFAULTARRAYLINE*fieldnum);
    if(dataoutexec(pos,DEFAULTARRAYLINE,s))
    {
      //fetch=0;
      m_executeSql_fetch=0;
      free(fieldbuf);
      return -1;
    }
    if(s>DEFAULTARRAYLINE)s%=DEFAULTARRAYLINE;
    nTotal+=s;
    for(j=0;j<s*fieldnum;j++)
    {
      //pdatavect->push_back((char *)fieldbuf+
      //    pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
      (*pdatavect)[j]=((char *)fieldbuf+
          pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
    }
    if(nTotal!=DEFAULTARRAYLINE)pdatavect->resize(nTotal*fieldnum);
    DEBUG_PRINTF("nTotal==%d\n",nTotal);
    if(nTotal<DEFAULTARRAYLINE)  {
      //fetch=0;
      m_executeSql_fetch = 0;
    }
    free(fieldbuf);
  }
  else
  {
    sqlbind(sqlstmt);
    ret=execute_cmd();
    if(ret>20000)return -ret;
    else if(ret)return ret;
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
  string sss;
  char *c;
  char sqlstmt[3000]="";
  long pos=0;
  char tablename[50]="";
  //static int fetch=0;//静态变量不支持多线程该为成员变量m_bulkCopy_fetch 
    //以下该方法中均使用m_bulkCopy_fetch替换fetch 2005-12-1 gaojf
  int pare=0;
    
  if(pdatavect==0)return -1;
  nTotal=0;
  nSuccess=0;
  nError=0;
  strncpy(sqlstmt,sqlStatement,sizeof(sqlstmt));

  for(c=sqlstmt;c[0]==' ';c++);
  if(strncasecmp(c,"INSERT",6)==0)
  {
    strncpy(tablename,strcasestr(sqlStatement,"INTO")+5,sizeof(tablename));
    for(i=1;i<50;i++)
    {
      if(tablename[i]==' '||tablename[i]=='('||
          tablename[i]=='\t'||tablename[i]=='\0')
      {
        tablename[i]=0;
        break;
      }
    }
    DEBUG_PRINTF("tablename:%s\n",tablename);

    for(c=(char *)strchr(sqlStatement,'(')+1;c[0]!=')';c++)
    {
      if(c[0]==',')fieldnum++;
    }
    fieldnum++;

    if(strcasestr(sqlStatement,"VALUES")==0)
    {
      strcat(sqlstmt," VALUES(:");
      for(i=1;i<fieldnum;i++)
        sprintf(sqlstmt,"%s%d,:",sqlstmt,i);
      sprintf(sqlstmt,"%s%d)",sqlstmt,i);
    }

    if((fieldbuf=(text *)malloc(DEFAULTARRAYLINE*fieldnum*
        (FIELDLENGTH+1)))==NULL)
    {
      DEBUG_PRINTF("memory alloc failed\n");
      return -1;
    }
    DEBUG_PRINTF("alloc memory size %ld\n",DEFAULTARRAYLINE*
        fieldnum*(FIELDLENGTH+1));
    memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));

    if (sqlbind(sqlstmt) !=0 )
    {
        return -1;
        free(fieldbuf);
    }
    
    if( datainbind(fieldnum) != 0 )
    {
        return -1;
        free(fieldbuf);
    }

    i=0;
    for(it=pdatavect->begin();it!=pdatavect->end();it++)
    {
      sss=*it;
      strncpy((char *)fieldbuf+i*(FIELDLENGTH+1),sss.c_str(),sss.length());
      if(++i>=DEFAULTARRAYLINE*fieldnum)
      {
        nTotal+=DEFAULTARRAYLINE;
        pos=0;
        while(pos<DEFAULTARRAYLINE)
        {
          if(datainexec(pos,DEFAULTARRAYLINE,s))  //增加异常判断gaojf 2005-1-31
          {
            free(fieldbuf);// add by chenm 2005-8-15
            return -1;
          }
          nSuccess+=s;
          pos+=s;
          if(pos<DEFAULTARRAYLINE)
          {
            nError++;
            for(j=0;j<fieldnum;j++)
            perrvect->push_back((char *)fieldbuf+
                pos*fieldnum*(FIELDLENGTH+1)+
                j*(FIELDLENGTH+1));
          }
          pos++;
        }
        i=0;
        memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));
      }
    }
    if(i)
    {
      pos=0;
      nTotal+=i/fieldnum;
      while(pos<(i/fieldnum))
      {
        if(datainexec(pos,i/fieldnum,s))
        {
          free(fieldbuf); // add by chenm 2005-8-15 
          return -1;
        }
        /*
        datainexec(pos,i/fieldnum,s);
        */
        //update end.

        nSuccess+=s;
        pos+=s;
        if(pos<i/fieldnum)
        {
          nError++;
          for(j=0;j<fieldnum;j++)
          perrvect->push_back((char *)fieldbuf+
              pos*fieldnum*(FIELDLENGTH+1)+
              j*(FIELDLENGTH+1));
        }
        pos++;
      }
    }
    free(fieldbuf);
  }
  else if(strncasecmp("SELECT",c,6)==0)
  {
    strncpy(tablename,strcasestr(sqlStatement,"FROM")+5,sizeof(tablename));
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
      //if(c[0]=='(')c=(char *)strchr(c,')');
      if(c[0]=='('){pare++;continue;}
      if(c[0]==')'){pare--;continue;}
      if(pare)continue;
      if(c[0]==',')fieldnum++;
    }
    fieldnum++;
    if((fieldbuf=(text *)malloc(DEFAULTARRAYLINE*fieldnum*
        (FIELDLENGTH+1)))==NULL)
    {
      DEBUG_PRINTF("memory alloc failed\n");
      return -1;
    }
    DEBUG_PRINTF("alloc memory size %ld\n",DEFAULTARRAYLINE*
        fieldnum*(FIELDLENGTH+1));
    memset(fieldbuf,0,DEFAULTARRAYLINE*fieldnum*(FIELDLENGTH+1));
     /*
    if(fetch==0)
    {
      fetch=1;
      sqlbind(sqlstmt);
    }
    */
    if(m_bulkCopy_fetch==0)
    {
      m_bulkCopy_fetch=1;
      sqlbind(sqlstmt);
    }
    dataoutbind(fieldnum);
    //pdatavect->clear();
    if(pdatavect->size()<DEFAULTARRAYLINE*fieldnum)
        pdatavect->resize(DEFAULTARRAYLINE*fieldnum);
    if(dataoutexec(pos,DEFAULTARRAYLINE,s))
    {
      //fetch=0;
      m_bulkCopy_fetch=0;
      free(fieldbuf);
      return -1;  
    }
    if(s>DEFAULTARRAYLINE)s%=DEFAULTARRAYLINE;
    nTotal+=s;
    for(j=0;j<s*fieldnum;j++)
    {
      //pdatavect->push_back((char *)fieldbuf+
      //    pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
      (*pdatavect)[j]=((char *)fieldbuf+
          pos*fieldnum*(FIELDLENGTH+1)+j*(FIELDLENGTH+1));
    }
    if(nTotal!=DEFAULTARRAYLINE)pdatavect->resize(nTotal*fieldnum);
    DEBUG_PRINTF("nTotal==%d\n",nTotal);
    if(nTotal<DEFAULTARRAYLINE)
    {
      //fetch=0;
      m_bulkCopy_fetch=0;
    }
    free(fieldbuf);
  }
  return 0;
}



//-------------------------------------------------------------------
void DbInterface::checkerr()
{
  text errbuf[512];
  sb4 errcode=0;

  switch (status)
  {
    case OCI_SUCCESS:
      break;
    case OCI_SUCCESS_WITH_INFO:
      DEBUG_PRINTF("Error: OCI_SUCCESS_WITH_INFO\n");
      break;
    case OCI_NEED_DATA:
      DEBUG_PRINTF("Error: OCI_NEED_DATA\n");
      break;
    case OCI_NO_DATA:
      DEBUG_PRINTF("Error: OCI_NODATA\n");
      break;
    case OCI_ERROR:
      OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,
          errbuf,(ub4)sizeof(errbuf),OCI_HTYPE_ERROR);
      m_errorCode=errcode;
      strncpy(m_errorMessage,(char *)errbuf,sizeof(m_errorMessage));
      DEBUG_PRINTF("Error: %.*s  errcode:%d\n", 512, errbuf,errcode);
      break;
    case OCI_INVALID_HANDLE:
      DEBUG_PRINTF("Error: OCI_INVALID_HANDLE\n");
      break;
    case OCI_STILL_EXECUTING:
      DEBUG_PRINTF("Error: OCI_STILL_EXECUTE\n");
      break;
    case OCI_CONTINUE:
      DEBUG_PRINTF("Error: OCI_CONTINUE\n");
      break;
    default:
      break;
  }
}

//-------------------------------------------------------------------
int DbInterface::sqlbind(const char *sqlstmt)
{
  DEBUG_PRINTF("sql:%s\n",sqlstmt);
  text *insertarray=(text *)sqlstmt;
  /*
  if(status=OCIHandleAlloc((dvoid *)envhp,(dvoid **)&stmthp, 
      OCI_HTYPE_STMT,(size_t)0,(dvoid **)0))
  {
    DEBUG_PRINTF("FAILED: OCIHandleAlloc()\n");
    checkerr();
    return OCI_ERROR;
  }
  */
  if(status=OCIStmtPrepare(stmthp,errhp,insertarray,
      (ub4)strlen((char *)insertarray),
      (ub4)OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT))
  {
    DEBUG_PRINTF("FAILED: OCIStmtPrepare()\n");
    checkerr();
    return OCI_ERROR;
  }
  return 0;
}


//-------------------------------------------------------------------
int DbInterface::datainbind(int fieldnum)
{
  int i;
  char v[30];
  
  for(i=0;i<MAXBIND;i++)
  {
    defnp[i]=(OCIDefine *)0;
    bndhp[i]=(OCIBind *)0;
  }
  //
  // bind the input variable
  //
  for(i=0;i<fieldnum;i++)
  {
    // BindByPos
    if(status=OCIBindByPos(stmthp,&bndhp[i],errhp,(ub4)(i+1),
        (dvoid *)(fieldbuf+i*(FIELDLENGTH+1)),
        (sb4)(FIELDLENGTH+1),SQLT_STR,(dvoid *)0,(ub2 *)0,
        (ub2 *)0,(ub4)0, (ub4 *)0,(ub4)OCI_DEFAULT))
    {
      DEBUG_PRINTF("FAILED: OCIBindByPos()\n");
      checkerr();
      return OCI_ERROR;
    }
    // BindByName
    /*
    sprintf(v,":%s",in_field[i]);
    if(status=OCIBindByName(stmthp,&bndhp[i],errhp,(text *)v,
        (sb4) strlen(v),(dvoid *)(fieldbuf+i*
        (FIELDLENGTH+1)),(sb4)(FIELDLENGTH+1),SQLT_STR,
        (dvoid *)0,(ub2 *)0,(ub2 *)0,(ub4)0, (ub4 *)0,
        (ub4)OCI_DEFAULT))
    {
      DEBUG_PRINTF("FAILED: OCIBindByName()\n");
      checkerr();
      return OCI_ERROR;
    }
    */
    if(status=OCIBindArrayOfStruct(bndhp[i],errhp,
        (ub4)(fieldnum*(FIELDLENGTH+1)),0,0,0))
    {
      DEBUG_PRINTF("FAILED: OCIBindArrayOfStruct()\n");
      checkerr();
      return OCI_ERROR;
    }
  }
  return 0;
}


//-------------------------------------------------------------------
int DbInterface::dataoutbind(int fieldnum)
{
  int i;

  for(i=0;i<fieldnum;i++)
  {
    if(status=OCIDefineByPos(stmthp,&defnp[i],errhp,(ub4)(i+1),
      (dvoid *)(fieldbuf+i*(FIELDLENGTH+1)),
      (sb4)(FIELDLENGTH+1),SQLT_STR,(dvoid *)&indp,(ub2 *)0,
      (ub2 *)0,(ub4)OCI_DEFAULT))
    {
      DEBUG_PRINTF("FAILED: OCIBindByName()\n");
      checkerr();
         return OCI_ERROR;
       }
      if(status=OCIDefineArrayOfStruct(defnp[i],errhp,
        (ub4)(fieldnum*(FIELDLENGTH+1)),0,0,0))
    {
      DEBUG_PRINTF("FAILED: OCIBindArrayOfStruct()\n");
      checkerr();
      return OCI_ERROR;
    }
  }
  return 0;
}

//-------------------------------------------------------------------
int DbInterface::datainexec(long pos,long count,long &row)
{
  text errbuf[512];
  sb4 errcode=0;
  int rows=0;
  int i;

  if(status=OCIStmtExecute(svchp,stmthp,errhp,(ub4)count,(ub4)pos,
      (CONST OCISnapshot *)NULL,(OCISnapshot *)NULL,
      OCI_DEFAULT))//OCI_COMMIT_ON_SUCCESS))//
     {
    DEBUG_PRINTF("FAILED: OCIStmtExecute()status:%d\n",status);
    checkerr();
    ///*newadd
    OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,
        errbuf,(ub4)sizeof(errbuf),OCI_HTYPE_ERROR);
    m_errorCode=errcode;
    strncpy(m_errorMessage,(char *)errbuf,sizeof(m_errorMessage));
      //update begin for 入库bug修改 by wangk 20050711
      //新增错误类型捕捉:1438错,插入数据的长度超过表字段定义,如将100插入number(2);
    if (m_errorCode != 1400 && m_errorCode != 1401 && m_errorCode != 1438)
      return OCI_ERROR;
     }

  if(status=OCIAttrGet((CONST dvoid *)stmthp,OCI_HTYPE_STMT,(dvoid *)&rows,
         (ub4 *) sizeof(ub4), OCI_ATTR_ROW_COUNT, errhp))
    return OCI_ERROR;

  if (rows < 0)
    return OCI_ERROR;

  row=rows;
  DEBUG_PRINTF("rows: %d\n",rows);
  if(transflag==0)
  {
    if(status=OCITransCommit(svchp,errhp,0))
    {
      DEBUG_PRINTF("FAILED: OCITransCommit()status:%d\n",
          status);
      checkerr();
      return OCI_ERROR;
    }
  }

  return 0;
}

//-------------------------------------------------------------------
int DbInterface::dataoutexec(long pos,long count,long &row)
{
  text errbuf[512];
  sb4 errcode=0;
  int rows=0;
  //static int c=0;     
  //static int totalrow;
  //静态变量不支持多线程该为成员变量m_dataoutexec_totalrow m_dataoutexec_c 
    //以下该方法中均使用m_dataoutexec_totalrow m_dataoutexec_c
    //替换c totalrow 2005-12-1 gaojf

  //if(c==0)
  if(m_dataoutexec_c == 0)
  {
    //c=1;
    m_dataoutexec_c = 1;
    DEBUG_PRINTF("OCIExecute ... ... \n");
    if(status=OCIStmtExecute(svchp,stmthp,errhp,(ub4)count,
        (ub4)pos,(CONST OCISnapshot *)NULL,
        (OCISnapshot *)NULL,OCI_DEFAULT))
       {
      DEBUG_PRINTF("FAILED: OCIStmtExecute(out)status:%d\n",
          status);
      checkerr();
      ///*newadd
      if(status!=OCI_NO_DATA&&status!=OCI_SUCCESS_WITH_INFO)
      {
        OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,
            errbuf,(ub4)sizeof(errbuf),OCI_HTYPE_ERROR);
        m_errorCode=errcode;
        strncpy(m_errorMessage,(char *)errbuf,sizeof(m_errorMessage));
        //c=0;
        m_dataoutexec_c = 0;
        return errcode;
      }//*/
      //return OCI_ERROR;
    }
    //if(status==OCI_NO_DATA)nodata=1;
    OCIAttrGet((CONST dvoid *)stmthp, OCI_HTYPE_STMT,
        (dvoid *)&rows, (ub4 *) sizeof(ub4), 
        OCI_ATTR_ROW_COUNT, errhp);
    DEBUG_PRINTF("outrows: %d\n",rows);
    row=rows;
    //totalrow=row;
    m_dataoutexec_totalrow = row;
    if(rows<count)
    {
      //c=0;
      m_dataoutexec_c = 0;
    }
    return 0;
  }
  if(status=OCIStmtFetch(stmthp,errhp,(ub4)count,
      (ub4)OCI_FETCH_NEXT, OCI_DEFAULT))
  {
    DEBUG_PRINTF("FAILED: OCIStmtFetch()status:%d\n",status);
    checkerr();
    ///*newadd
    if(status!=OCI_NO_DATA&&status!=OCI_SUCCESS_WITH_INFO)
    {
      OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,
          errbuf,(ub4)sizeof(errbuf),OCI_HTYPE_ERROR);
      m_errorCode=errcode;
      strncpy(m_errorMessage,(char *)errbuf,sizeof(m_errorMessage));
      //c=0;
      m_dataoutexec_c = 0;
      return errcode;
    }//*/
    //return OCI_ERROR;
  }
  //if(status==OCI_NO_DATA)nodata=1;
  OCIAttrGet((CONST dvoid *)stmthp,OCI_HTYPE_STMT,(dvoid *)&rows, 
      (ub4 *) sizeof(ub4), OCI_ATTR_ROW_COUNT, errhp);
  DEBUG_PRINTF("outrows: %d\n",rows);
  //if(rows>totalrow)
  if(rows>m_dataoutexec_totalrow)
  {
    //rows-=totalrow;
    //totalrow+=rows;
    rows-=m_dataoutexec_totalrow;
    m_dataoutexec_totalrow +=rows;
  }
  else rows=0;
  row=rows;
  if(rows<count)
  {
    //c=0;
    m_dataoutexec_c=0;
  }
  return 0;
}


//-------------------------------------------------------------------
int DbInterface::execute_cmd()
{
  text errbuf[512];
  sb4 errcode=0;

  if(status=OCIStmtExecute(svchp,stmthp,errhp,(ub4)1,(ub4)0, 
      (CONST OCISnapshot *)NULL,(OCISnapshot *)NULL,
      transflag?OCI_DEFAULT:OCI_COMMIT_ON_SUCCESS))
     {
    DEBUG_PRINTF("FAILED: OCIStmtExecute()status:%d\n",status);
    //checkerr();
    OCIErrorGet((dvoid *)errhp,(ub4)1,(text *)NULL,&errcode,
        errbuf,(ub4)sizeof(errbuf),OCI_HTYPE_ERROR);
    m_errorCode=errcode;
    strncpy(m_errorMessage,(char *)errbuf,sizeof(m_errorMessage));
    //DEBUG_PRINTF("ErrorMessage: %.*s\nErrcode: %d\n",
    //    512, errbuf,errcode);
    return errcode;
     }
     return 0;
}


//-------------------------------------------------------------------
int DbInterface::useDb(const char *dbName)
{
  return 0;
}


//-------------------------------------------------------------------
int DbInterface::callProcedure(const char *procedureName,
    const int parameterNum,PROCPARAM *parameters)
{
  char szParam1[50];
  int Param2;
  char szParamname1[50];
  char szParamname2[50];

  char procedure[200];
  OCIBind *bndp[10]; /* the first bind handle */
  int result;
  dvoid *tmp;
  OCISession *usrhp = (OCISession *)NULL;
  int i,j;

  sprintf(procedure,"BEGIN %s(",procedureName);
  
  for(i=0;i<parameterNum-1;i++)
  {
    sprintf(procedure,"%s:%d,",procedure,i+1);
  }
  sprintf(procedure,"%s:%d); END;",procedure,i+1);

  DEBUG_PRINTF("%s\n",procedure);

  /*
  if(status=OCIHandleAlloc( (dvoid *) envhp, (dvoid **) &stmthp,
      OCI_HTYPE_STMT, 100, (dvoid **) &tmp))
     {
    DEBUG_PRINTF("FAILED: OCIHandleAlloc()status:%d\n",status);
    checkerr();
    //return OCI_ERROR;
    return -1;
     }
     */
  /* prepare the statement request, passing the PL/SQL text
  block as the statement to be prepared */
  if(status=OCIStmtPrepare(stmthp,errhp,(text *)procedure,
      (ub4)strlen(procedure),OCI_NTV_SYNTAX,OCI_DEFAULT))
     {
    DEBUG_PRINTF("FAILED: OCIStmtPrepare()status:%d\n",status);
    checkerr();
    //return OCI_ERROR;
    return -1;
     }
  /*if(status=OCIBindByName(stmthp,&bndhp[i],errhp,(text *)v,
      (sb4) strlen(v),(dvoid *)(fieldbuf+i*(FIELDLENGTH+1)),
      (sb4)(FIELDLENGTH+1),SQLT_STR,(dvoid *)0,(ub2 *)0,
      (ub2 *)0,(ub4)0, (ub4 *)0,(ub4)OCI_DEFAULT))*/
  /* bind each of the placeholders to a program variable */

  for(i=0;i<parameterNum;i++)
  {
    if(parameters[i].type==PARAM_TYPE_STR)
    {
      if(status=OCIBindByPos(stmthp,&bndp[i],errhp,(ub4)(i+1),
          (dvoid *)parameters[i].strValue,(sb4)80,
          SQLT_STR,(dvoid *)0,(ub2 *)0,(ub2 *)0,
          (ub4)0, (ub4 *)0,(ub4)OCI_DEFAULT))
      //if(status=OCIBindByName(stmthp,&bndp[i],errhp,
      //    (text *),(sb4)3,(dvoid *)parameters[i].strValue,
      //    (ub4)50,SQLT_STR,(dvoid *)0,(ub2 *)0,(ub2)0,
      //    (ub4)0,(ub4 *)0,OCI_DEFAULT))
      {
        DEBUG_PRINTF("FAILED: OCIBindByName(1)status:%d\n",
            status);
        checkerr();
        //return OCI_ERROR;
        return -1;
         }
       }
    if(parameters[i].type==PARAM_TYPE_INT)
    {
      // remember that PL/SQL OUT variable are bound, 
      // not defined
      if(status=OCIBindByPos(stmthp,&bndp[i],errhp,(ub4)(i+1),
          (dvoid *)&parameters[i].intValue,
          (sb4)sizeof(int),SQLT_INT,(dvoid *)0,(ub2 *)0,
          (ub2 *)0,(ub4)0, (ub4 *)0,(ub4)OCI_DEFAULT))
      //if(status=OCIBindByName(stmthp,&bndp[i],errhp,
      //    (text *)"",0,(ub1 *)&parameters[i].intValue,
      //    (sword)sizeof(int),SQLT_INT,(dvoid *)0,(ub2 *)0,
      //    (ub2)0,(ub4)0,(ub4 *)0,OCI_DEFAULT))
         {
        DEBUG_PRINTF("FAILED: OCIBindByName(2)status:%d\n",
            status);
        checkerr();
        //return OCI_ERROR;
        return -1;
         }
    }
    if(parameters[i].type==PARAM_TYPE_FLOAT)
    {
      // remember that PL/SQL OUT variable are bound,
      // not defined
      if(status=OCIBindByPos(stmthp,&bndp[i],errhp,(ub4)(i+1),
          (dvoid *)&parameters[i].floatValue,
          (sb4)sizeof(float),SQLT_FLT,(dvoid *)0,(ub2 *)0,
          (ub2 *)0,(ub4)0, (ub4 *)0,(ub4)OCI_DEFAULT))
      //if(status=OCIBindByName(stmthp,&bndp[i],errhp,
      //    (text *)"",0,(ub1 *)&parameters[i].floatValue,
      //    (sword)sizeof(float),SQLT_FLT,(dvoid *)0,
      //    (ub2 *)0,(ub2)0,(ub4)0,(ub4 *)0,OCI_DEFAULT))
         {
        DEBUG_PRINTF("FAILED: OCIBindByName(2)status:%d\n",
            status);
        checkerr();
        //return OCI_ERROR;
        return -1;
         }
    }
  }
  if(status=OCIStmtExecute(svchp,stmthp,errhp,(ub4)1,(ub4)0,
      (OCISnapshot *)NULL,(OCISnapshot *)NULL,OCI_DEFAULT))
     {
    DEBUG_PRINTF("FAILED: OCIStmtExecute()status:%d\n",status);
    checkerr();
    //return OCI_ERROR;
    return -1;
     }
  return 0;
}

//-------------------------------------------------------------------
int DbInterface::beginTransaction()
{

  transflag=1;
  return 0;
}

//-------------------------------------------------------------------
int DbInterface::endTransaction()
{

  transflag=0;
  return 0;
}

//-------------------------------------------------------------------
int DbInterface::commitTransaction()
{
  if(status=OCITransCommit(svchp,errhp,OCI_DEFAULT))
  {
    DEBUG_PRINTF("FAILED: OCITransCommit()status:%d\n",status);
    checkerr();
    return OCI_ERROR;
  }

  return 0;
}


//-------------------------------------------------------------------
int DbInterface::rollbackTransaction()
{
  if(status=OCITransRollback(svchp,errhp,OCI_DEFAULT))
  {
    DEBUG_PRINTF("FAILED: OCITransRollback()status:%d\n",status);
    checkerr();
    return OCI_ERROR;
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

