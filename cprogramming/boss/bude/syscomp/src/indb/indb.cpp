#include "config-all.h"


#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "indb.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"

#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>
USING_NAMESPACE(std)

DbInterface g_dbinterface;
bool connected = false;
char msgbuf[MAXBUFFLEN+1];

void getFieldsValues(char (*)[50],char (*)[50],int ,int ,char *,char *);
int  getFieldName(char *fieldnamebuf,char fieldname[MAXBIND][50]);
void trim(char *s);
char *truepath(const char *path);
int split(const char *token,const char *cdr,bool end_type,vector<string> & fields);


IndbApp g_application;
Application * const theApp = &g_application;

bool IndbApp::processopt(int optopt, const char *optarg)
{

  bool rc = true;
  switch (optopt)
  {

  case 'c':
  case ':':
  case '?':
  default:
    rc = Application::processopt(optopt, optarg);
  }
  return rc;
}


bool IndbApp::initialization()
{
  setoptstr("c:");
  // normally, call standard initialization first
  if (!FileApplication::initialization())
    return false;

  // set application run method if need, all default is false
#ifdef _DEBUG_
    set_asdaemon(false);
    set_onlyone(false);
    set_runinbkg(false);
#else
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif

  //-----------------------------------------------------------------------//
  // ADD YOUR INITIALIZATION CODE HERE
  //-----------------------------------------------------------------------//

  char channelNo[10];
  sprintf(channelNo,"%d",m_channelNo);
  string section;
  section = SECTDELIM;
  section = section + m_name + SECTDELIM + channelNo;


  if (!m_theSysParam->getValue(section, "configfile", m_configFile))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,
        section + SECTDELIM + "configfile");
    return false;
  }


  string incompleteFlag;
  if (!m_theSysParam->getValue(section, "incompleteflag", incompleteFlag))
  {
    incompleteFlag="";
  }


  //get Real incompelete Flag

  int i,j;

  for(i=0,j=0;i<strlen(incompleteFlag.c_str());i++)
  {
    if(incompleteFlag[i]=='\'')
      continue;
    m_incompleteFlag[j]=incompleteFlag[i];
    j++;
  }
  m_incompleteFlag[j]=0;


  if(!getConfig())
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,
            "configfile incorrect");
    return false;
  }

  if(reconnect())
  {
    sprintf(msgbuf,"Error(%s:%d):database connect failed.",
            __FILE__,__LINE__);
    writeAlarm(ERROR_INDB_DBCONN,msgbuf);
    return false;
  }

  setFileSortMethod(Directory::SF_MODIFYTIME);
  setPathIn(m_sourcePath);

  m_reserved = "";
  m_objectName = "";
  updateStatus();
  return true;
}


bool IndbApp::end()
{

  if(connected)
    g_dbinterface.disconnect();
  connected=0;
  return Application::end();
}


bool IndbApp::recovery()
{

  Application::recovery();
  return true;
}


bool IndbApp::loopProcess()
{

  /* get one File from sourcePath */
  int fileNum;
  char fileName[MAXFILELEN+1];

  //Record the process start time and finish/end time

  if((fileNum=getFile(fileName,sizeof(fileName)))==0)
  {
    // No file need import,sleep 10s ,continue loop
    writeStatus("",PROC_IDLE);
    if(connected)
      g_dbinterface.disconnect();
    connected=0;
    ssleep(10);
    return Application::loopProcess();
  }

  if(strlen(m_incompleteFlag)>0)
  {
      if(!strncmp(fileName,m_incompleteFlag,
                 strlen(m_incompleteFlag)))
      {
        writeStatus("",PROC_IDLE);
        if(connected)
          g_dbinterface.disconnect();
        connected=0;
        ssleep(5);
        return Application::loopProcess();
      }
  }

  if(!connected)
  {
    if(reconnect())
    {
      sprintf(msgbuf,"Error(%s:%d):database connect failed",
              __FILE__,__LINE__);
      writeAlarm(ERROR_INDB_DBCONN,msgbuf);
      writeStatus("",PROC_WARN);
      sleep(60);
      return false;
    }
  }

  writeStatus(fileName,PROC_RUN);

  // Process File
  theLoggerProcessID = theLogger->pBegin();

  if(!process(fileName)) {
	theLogger->pEnd(theLoggerProcessID);
    return false;
	}

  theLogger->pEnd(theLoggerProcessID);

  return Application::loopProcess();
}


//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//
bool IndbApp::process(const char *fileName)
{
  struct timeval   s_timeval;
  struct timeval   e_timeval;
  struct timezone tzp;
  time_t       s_time;
  time_t       e_time;
  struct tm   *ts;
  char         s_ts[15];
  char         e_ts[15];
  float       dt;
  int         month;
  char         sqlcmd[MAXLONGBUFFLEN+1];
  int         i=0,nTotal=0,nSuccess=0,nError=0;
  char        fullFileName[MAXPATHLEN+MAXFILELEN+1];
  int matched = -1;

  // log the process begin time
  time(&s_time);
  gettimeofday(&s_timeval,&tzp);
  ts=localtime(&s_time);
  strftime(s_ts,15,"%Y%m%d%H%M%S",ts);

  month = ts->tm_mon + 1;
  sprintf(fullFileName,"%s/%s",m_sourcePath,fileName);

  // get the matched importMode
  for(i=0; i< m_importModes.size(); i++)
  {
     if(m_importModes[i].isMatchMode(fileName))
     {
          matched = i;
          break;
     }
  }

  char failedFileName[MAXPATHLEN+MAXFILELEN+1];

  //if can match import mode,move file to failedPath
  if(matched == -1)
  {
    sprintf(failedFileName,"%s/%s",m_failedPath,fileName);
    if(rename(fullFileName,failedFileName))
    {
      sprintf(msgbuf,"Error(%s:%d):can't rename for (%s) file ",
              __FILE__,__LINE__,fullFileName);
      writeAlarm(ERROR_INDB_MOVEFILE,msgbuf);
      writeStatus(fileName,PROC_WARN);
      sleep(60);
      return false;
    }
    sprintf(msgbuf,"Error(%s:%d):can't match importMode for (%s) file ",
              __FILE__,__LINE__,fullFileName);
    writeAlarm(ERROR_INDB_MOVEFILE,msgbuf);
    theLogger->pInput(theLoggerProcessID,fullFileName,-1,"type='invalid file'");
    theLogger->pOutput(theLoggerProcessID,failedFileName,-1,"type='invalid file'");
    return false;
  }

  //if sybase, call 'useDB'
   if(strlen(m_importModes[matched].m_database)>0)
  {
    if(g_dbinterface.useDb(m_importModes[matched].m_database))
    {
      return false;
    }
  }

  // if needed, truncate table
  // if truncate failed, return false;
  if(m_importModes[matched].m_truncateTable)
  {
     sprintf(sqlcmd,"TRUNCATE TABLE %s",m_importModes[matched].m_tableName);
     if(g_dbinterface.executeSql(sqlcmd,nTotal,nSuccess,nError))
        {
          sprintf(msgbuf,"Error(%s:%d):cdbinterface.executeSql(%s) failed",
                __FILE__,__LINE__,sqlcmd);
          writeAlarm(ERROR_INDB_EXECSQL,msgbuf);
          writeStatus(fileName,PROC_WARN);
          sleep(60);
          if(connected)
            g_dbinterface.disconnect();
          connected=0;
          return false;
      }
   }

  // start the transaction 
  g_dbinterface.beginTransaction();

  // if set a configure executesql 
  // execute the statement
  if(strlen(m_importModes[matched].m_executeSql) > 0) {
     sprintf(sqlcmd,"%s",m_importModes[matched].m_executeSql);

     if(g_dbinterface.executeSql(sqlcmd,nTotal,nSuccess,nError)) {
        g_dbinterface.rollbackTransaction();
        sprintf(msgbuf,"Error(%s:%d):cdbinterface.executeSql(%s) failed",
              __FILE__,__LINE__,sqlcmd);
        writeAlarm(ERROR_INDB_EXECSQL,msgbuf);
        writeStatus(fileName,PROC_WARN);
        sleep(20);
        if(connected)
          g_dbinterface.disconnect();
        connected=0;
        return false;
      }
   }  
  
  // import file to Table
  nError = 0;
  if(!importFile(m_importModes[matched],fullFileName,fileName,nTotal,nSuccess,nError))
  {
    g_dbinterface.rollbackTransaction();
    sprintf(msgbuf,"Error(%s:%d):processfile(%s) failed",
          __FILE__,__LINE__,fullFileName);
    writeAlarm(ERROR_INDB_PROCFILE,msgbuf);
    writeStatus(fileName,PROC_WARN);
    sleep(60);
    return false;
  }

  if (nError > 0)
  {
    sprintf(msgbuf,"File %s:Error records number is %d", fullFileName, nError);
    writeAlarm(ERROR_INDB_PROCFILE, msgbuf);
  }

    // if needed,execute procedure after import
  char Val[10][40];
  char Sym[10][10];
  char procName[500];
  int sum;

  int ret=0,err=0;
  memset(procName,0,sizeof(procName));

  if(m_importModes[matched].m_execProc)
  {
    // parse procedure parameter string
    if(parseProcString(m_importModes[matched].m_procedure,procName,Val,Sym,sum))
    {
        sprintf(msgbuf,"Error(%s:%d):ProcessProc(%s) failed",
            __FILE__,__LINE__,m_importModes[matched].m_procedure);
        writeAlarm(ERROR_INDB_PROCFILE,msgbuf);
        writeStatus(fileName,PROC_WARN);
         sleep(60);
        return false;
    }

    // indb procparam ,Hard-Code
     PROCPARAM procParam[3];
    //strcpy(pp[0].name,Val[0]);
    procParam[0].action=PARAM_ACT_IN;
    procParam[0].type=PARAM_TYPE_STR;
    strcpy(procParam[0].strValue,fileName);
    //strcpy(pp[1].name,Val[1]);
    procParam[1].action=PARAM_ACT_OUT;
    procParam[1].type=PARAM_TYPE_INT;
    procParam[1].intValue=m_channelNo;
    procParam[2].action=PARAM_ACT_OUT;
    procParam[2].type=PARAM_TYPE_STR;
    strcpy(procParam[2].strValue,"");

    ret=g_dbinterface.callProcedure(procName,3,procParam);

    err=procParam[1].intValue;
    //printf("return value:%d errno=%d, errtext=%s\n",ret,outerr,pp[2].strValue);

    // call procedure failed!
    if(ret||err)
    {
      sprintf(msgbuf,"Error(%s:%d):file(%s) dbi.callProcedure(%s) failed. return:%d,errno=%d,errtext=%s",
            __FILE__,__LINE__,fileName,procName,ret,err,procParam[2].strValue);
      writeAlarm(ERROR_INDB_CALLPROC,msgbuf);
      writeStatus(fileName,PROC_WARN);
      ssleep(2);

/*        // rename file to failed path
      sprintf(failedFileName,"%s/%s",m_failedPath,fileName);
      if(rename(fullFileName,failedFileName))
      {
        sprintf(msgbuf,"Error(%s:%d):rename(%s,%s) failed",
                __FILE__,__LINE__,fullFileName,failedFileName);
        writeAlarm(ERROR_INDB_MOVEFILE,msgbuf);
        writeStatus(fileName,PROC_WARN);
        sleep(60);
        return false;
      }
      theLogger->pInput(theLoggerProcessID,fullFileName,-1,"type='invalid file'");
      theLogger->pOutput(theLoggerProcessID,failedFileName,-1,"type='invalid file'");
*/
      return false;
    }
  }


  theLogger->pInput(theLoggerProcessID,fullFileName,nTotal);
  if (nSuccess > 0) {
    char dbpath[MAXFILELEN+1];
    sprintf(dbpath,"%s.%s.%s",m_server,m_userName,m_importModes[matched].m_tableName);
    theLogger->pOutput(theLoggerProcessID,dbpath,nSuccess,"type='indb'");
  }
  if (nError >0) {
    char   errorFileName[MAXFILELEN+1];
    sprintf(errorFileName,"%s/%s%s",m_errorPath,"E",fileName);
    theLogger->pOutput(theLoggerProcessID,errorFileName,nError,"type='error'");
  }


  // backup files
  if (m_backuppathVector.size()>0)
  {
    char backupFile[300];
    bool isAllSuccess=true;
    for(i=0;i<m_backuppathVector.size();i++)
    {
        memset(backupFile,0,sizeof(backupFile));
        sprintf(backupFile,"%s/%s",m_backuppathVector[i].c_str(),fileName);
        if (link(fullFileName,backupFile)<0)
        {
            sprintf(msgbuf,"Error(%s:%d):link(%s,%s) failed",
                __FILE__,__LINE__,fullFileName,backupFile);
            writeAlarm(ERROR_INDB_MOVEFILE,msgbuf);
            writeStatus(backupFile,PROC_WARN);
            isAllSuccess=false;
            continue;
        }
        // TODO
        theLogger->pLinkOut(theLoggerProcessID,fullFileName,backupFile);
    }
    if(isAllSuccess==false)
      sleep(60);
  }

  if(remove(fullFileName))
  {
    g_dbinterface.rollbackTransaction();
    sprintf(msgbuf,"Error(%s:%d):remove(%s) failed",
            __FILE__,__LINE__,fullFileName);
    writeAlarm(ERROR_INDB_DELFILE,msgbuf);
    writeStatus(fileName,PROC_WARN);
    sleep(60);
    return false;
  }

  g_dbinterface.commitTransaction();

  // write log
  time(&e_time);
  gettimeofday(&e_timeval,&tzp);
  ts=localtime(&e_time);
  strftime(e_ts,15,"%Y%m%d%H%M%S",ts);
  dt=(double)(e_timeval.tv_sec*1000000+e_timeval.tv_usec-s_timeval.tv_sec*1000000-s_timeval.tv_usec)/1000000;
  sprintf(msgbuf,"%s@%s@%s@%s@%d@%d@%02d",fileName,m_userName,s_ts,e_ts,nSuccess,nError,month);
  writeLogMsg(msgbuf);

  return true;
}



//-----------------------------------------------------------------------------
bool IndbApp::importFile(const ImportMode& importMode,const char *fullFileName,
                         const char *fileName,int& total,int& success, int& error)
{
  StringVector       inVector;
  StringVector       errVector;
  StringVectorIterator   it;
  FILE           *hInputFile=NULL;
  FILE           *hErrorFile=NULL;
  char           errorFileName[MAXFILELEN+1];
  char           buf[5000];
  char           field[200][1500];
  int           recordIndex=0;
  int           totalRecord=0;
  int           errorRecord=0;
  int           charIndex;
  int           fieldIndex;
  int           fieldCharIndex;

  int nTotal,nSuccess,nError;

  int           i;

  // bind the IN data vector,ERROR data vector
  g_dbinterface.dataBind(inVector,errVector);


  sprintf(errorFileName,"%s/%s%s",m_errorPath,"E",fileName);

  if((hInputFile=fopen(fullFileName,"rb"))==NULL)
  {
    sprintf(msgbuf,"inputfile %s open failed",fullFileName);
    //writeAlarm(ALARM_SERIOUS,msgbuf);
    return false;
  }

  while(1)
  {

    if((fgets(buf,5000,hInputFile))==NULL)
        break;
    //增加有头尾记录时的处理

    if(importMode.m_hasHead)
    {
        if(strncmp(buf,importMode.m_headFlag,strlen(importMode.m_headFlag))==0)
           continue;
    }

    if(importMode.m_hasTail)
    {
        if(strncmp(buf,importMode.m_tailFlag,strlen(importMode.m_tailFlag))==0)
           break;
    }
    //END

    totalRecord++;
    for(fieldIndex=0,charIndex=0,fieldCharIndex=0;
        fieldIndex<importMode.m_fieldNum;fieldIndex++)
    {
      //while(buf[charindex]!=','&&buf[charindex]!='\n'&&buf[charindex]!='\r')
    while(buf[charIndex]!=importMode.m_fieldSeperator&&buf[charIndex]!='\n'&&buf[charIndex]!='\r')
      {
        field[fieldIndex][fieldCharIndex]=buf[charIndex];
        charIndex++;
        fieldCharIndex++;
      }

      field[fieldIndex][fieldCharIndex]='\0';
      if(buf[charIndex]=='\n'||buf[charIndex]=='\r')
        break;
      charIndex++;
      fieldCharIndex=0;
    }
    fieldIndex++;
    if(fieldIndex>=importMode.m_fieldNum)
    {
      recordIndex++;
      for(i=0;i<importMode.m_fieldNum;i++)
      {
        inVector.push_back(field[i]);
      }
    }
    else
    {
      errorRecord++;
      if(hErrorFile)
      {
        fprintf(hErrorFile,"%s",buf);
      }
      else
      {
        if((hErrorFile=fopen(errorFileName,"wb"))==NULL)
        {
          sprintf(msgbuf,"badfile %s open failed",errorFileName);
          //writeAlarm(ALARM_SERIOUS,msgbuf);
          if(hInputFile)
            fclose(hInputFile);
          return false;
        }
        fprintf(hErrorFile,"%s",buf);
      }
    }

    if(recordIndex==DEFAULTARRAYLINE)
    {
      if(g_dbinterface.bulkCopy(importMode.m_insertStmt,nTotal,nSuccess,nError))
      {
        //writealarm(ALARM_SERIOUS,"cdbinterface.bulkCopy() failed");
        if(hInputFile)
          fclose(hInputFile);
        if(hErrorFile)
            fclose(hErrorFile);
        if(connected)
          g_dbinterface.disconnect();
        connected=0;
        return false;
      }
      if(nError)
      {
        for(it=errVector.begin(),i=0;it!=errVector.end();it++)
        {
          i++;
          if(hErrorFile)
          {
            fprintf(hErrorFile,"%s",(*it).c_str());
          }
          else
          {
            if((hErrorFile=fopen(errorFileName,"wb"))==NULL)
            {
              sprintf(msgbuf,"badfile %s open failed.\n",errorFileName);
              //writeAlarm(ALARM_SERIOUS,msgbuf);
              if(hInputFile)
                fclose(hInputFile);
              return false;
            }
            fprintf(hErrorFile,"%s",(*it).c_str());
          }
          if(i<importMode.m_fieldNum)
              fprintf(hErrorFile,",");
          if(i==importMode.m_fieldNum)
          {
            fprintf(hErrorFile,"\n");
            i=0;
          }
        }
      }
      total    +=  nTotal;
      success  +=  nSuccess;
      error    +=  nError;
      inVector.clear();
      errVector.clear();
      recordIndex=0;
      nTotal=0;
      nSuccess=0;
      nError=0;
    }
  }

  if(recordIndex)
  {
    //printf("insertarray=%s, recordindex=%d , size=%d\n",insertarray,recordindex,invector.size());
    if(g_dbinterface.bulkCopy(importMode.m_insertStmt,nTotal,nSuccess,nError))
    {
      //writealarm(ALARM_SERIOUS,"cdbinterface.bulkCopy() failed");
      if(hInputFile)
        fclose(hInputFile);
      if(hErrorFile)
        fclose(hErrorFile);
      if(connected)
        g_dbinterface.disconnect();  //add for disc
      connected=0;              //add for disc
      return false;
    }

    if(nError)
    {
      for(it=errVector.begin(),i=0;it!=errVector.end();it++)
      {
        i++;
        if(hErrorFile)
        {
          fprintf(hErrorFile,"%s",(*it).c_str());
        }
        else
        {
          if((hErrorFile=fopen(errorFileName,"wb"))==NULL)
          {
            sprintf(msgbuf,"badfile %s open failed",errorFileName);
            //writealarm(ALARM_SERIOUS,msgbuf);
            if(hInputFile)
              fclose(hInputFile);
            return false;
          }
          fprintf(hErrorFile,"%s",(*it).c_str());
        }
        if(i<importMode.m_fieldNum)
          fprintf(hErrorFile,",");
        if(i==importMode.m_fieldNum)
        {
          fprintf(hErrorFile,"\n");
          i=0;
        }
      }
    }
    total+=nTotal;
    success+=nSuccess;
    error+=nError;
    inVector.clear();
    errVector.clear();
    recordIndex=0;
    nTotal=0;
    nSuccess=0;
    nError=0;
  }
  if(hInputFile)
    fclose(hInputFile);
  if(hErrorFile)
    fclose(hErrorFile);
  return true;
}


int IndbApp::parseProcString(char * procNameStr, char * procName, char Val[10][40],char Sym[10][10],int& Sum)
{

  int sum;
  char val[10][50];
    if(procNameStr==NULL)
      return -1;

    int len=strlen(procNameStr);
    int i,h,g,loc,j[10];

    i=0;
    for(h=0;;h++)
    {
      for(;i<len;i++)
      {
        if(procNameStr[i]=='(')
        {
            j[h]=i;
            break;
        }
        else if(procNameStr[i]==',')
        {
            j[h]=i;
            break;
        }
        else if(procNameStr[i]==')')
        {
            j[h]=i;
            break;
        }
      }
      if(procNameStr[i]==')')
          break;
      i++;
    }

    sum=++h;

    if(j[0]==0)
      return -1;

    // get procedure name
    for(i=0;i<j[0];i++)
      procName[i]=procNameStr[i];


    // get parameter varible
    for(i=0;i<sum-1;i++)
    {
      if(j[i]+1==j[i+1])
          return -1;

      for(h=0,g=j[i]+1;g<j[i+1];h++,g++)
          val[i][h]=procNameStr[g];
    }


    for(i=0;i<sum-1;i++)
    {
      len=strlen(val[i]);
      loc=len-strlen(strchr(val[i],'$'));
      if((loc==0)||(loc==len-1))
          return -1;

        // parameter name
      for(h=0;h<loc;h++)
          Val[i][h]=val[i][h];

        //  parameter indication
      for(g=0,h=loc+1;h<len;g++,h++)
          Sym[i][g]=val[i][h];
    }
    Sum =  sum-1;
    return 0;
}


bool IndbApp::getConfig()
{
   SimpleConfigure cconfig;
   ImportMode im_tmp;
   char       longbuff[MAXLONGBUFFLEN+1];
   char       buff[MAXBUFFLEN+1];
   int j=0,k=0;


  //initialize
  if(cconfig.initialize(m_configFile.c_str()))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"cconfig.initialize()");
    return false;
  }

  // get DB Configuration
  if(cconfig.getval("DB","USER",m_userName,MAXNAMELEN))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:USER not define");
    return false;
  }
  if(cconfig.getval("DB","PASSWD",m_passwd,MAXNAMELEN))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:PASSWD not define");
    return false;
  }
  if(cconfig.getval("DB","SERVER",m_server,MAXNAMELEN))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:SERVER not define");
    return false;
  }

  // get general Configuration
   char path[MAXPATHLEN+1];

  if(cconfig.getval("GENERAL","SOURCEPATH",path,MAXPATHLEN))
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:SOURCEPATH not define");
    return false;
  }

  strcpy(m_sourcePath,truepath(path));

   if(cconfig.getval("GENERAL","FAILEDPATH",path,MAXPATHLEN))
   {
       if(cconfig.getval("GENERAL","NOTINPATH",path,MAXPATHLEN))
       {
          m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:NOTINPATH not define");
          return false;
        }
   }
   strcpy(m_failedPath,truepath(path));

   if(cconfig.getval("GENERAL","ERRORPATH",path,MAXPATHLEN))
   {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:ERRORPATH not define");
      return false;
   }
   strcpy(m_errorPath,truepath(path));

   strcpy(path,"");
   cconfig.getval("GENERAL","BACKUPPATH",path,MAXPATHLEN);
   split(",",path,true,m_backuppathVector);

   int fieldLength=0,arrayLine=0;

    if(cconfig.getval("GENERAL","FIELDLENGTH",buff,MAXBUFFLEN)==0)
    {
        fieldLength = atoi(buff);
      if(fieldLength>0)
      g_dbinterface.setFieldLength(fieldLength);
    }

    if(cconfig.getval("GENERAL","ARRAYLINE",buff,MAXBUFFLEN)==0)
    {
      arrayLine = atoi(buff);
      if(arrayLine>0)
      g_dbinterface.setArrayLine(arrayLine);
    }

   int formatNum=0;
   char tmp[MAXBUFFLEN+1];

   if(cconfig.getval("GENERAL","FORMATNUM",tmp,MAXBUFFLEN))
   {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:ERRORPATH not define");
      return false;
   }
   formatNum = atoi(tmp);

  // 不同的格式配置信息
  for(int i=1;i<formatNum+1;i++)
  {
    sprintf(tmp,"FORMAT%d",i);

    if(cconfig.getval(tmp,"FILEPATTERN",longbuff,MAXLONGBUFFLEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:DATABASE not define");
      return false;
    }

    im_tmp.m_filePatterns.clear();
    split(",",longbuff,true,im_tmp.m_filePatterns);


    if(cconfig.getval(tmp,"DATABASE",im_tmp.m_database,MAXNAMELEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:DATABASE not define");
      return false;
    }

    cconfig.getval(tmp,"EXECUTESQL",im_tmp.m_executeSql,MAXNAMELEN);

    if(cconfig.getval(tmp,"TABLE",im_tmp.m_tableName,MAXNAMELEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TABLE not define");
      return false;
    }

    if(cconfig.getval(tmp,"FLAGOFEXEPROC",buff,MAXBUFFLEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:FLAGOFEXEPROC not define");
      return false;
    }
    if(strcmp(buff,"EXE"))
      im_tmp.m_execProc = false;
    else
      im_tmp.m_execProc = true;

    if(cconfig.getval(tmp,"PROCEDURE",im_tmp.m_procedure,MAXBUFFLEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:PROCEDURE not define");
      return false;
    }

    if(cconfig.getval(tmp,"TRUNCATE",buff,MAXBUFFLEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TRUNCATE not define");
      return false;
    }

    if(strcmp(buff,"YES"))
      im_tmp.m_truncateTable = false;
    else
      im_tmp.m_truncateTable = true;


    if(cconfig.getval(tmp,"TABLEFIELD",im_tmp.m_tableField,MAXLONGBUFFLEN))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TABLEFIELD not define");
      return false;
    }

    im_tmp.m_fieldNum = getFieldName(im_tmp.m_tableField,im_tmp.m_fieldName);


    if(cconfig.getval(tmp,"DATEFIELD",im_tmp.m_dateField,MAXLONGBUFFLEN)==0)
      im_tmp.m_dateFieldNum = getFieldName(im_tmp.m_dateField,im_tmp.m_dateFieldName);

    if(cconfig.getval(tmp,"FIELDLENGTH",buff,MAXBUFFLEN)==0)
    {
      im_tmp.m_fieldLength = atoi(buff);
    }

    if(cconfig.getval(tmp,"ARRAYLINE",buff,MAXBUFFLEN)==0)
    {
      im_tmp.m_arrayLine = atoi(buff);
    }

    if(!cconfig.getval(tmp,"FIELDSEP",buff,MAXBUFFLEN))
    {
      if(buff[0]==0x5c)  // "\"
        im_tmp.m_fieldSeperator = atoi(buff+1);
      else
        im_tmp.m_fieldSeperator = buff[0];
    }
    else
      im_tmp.m_fieldSeperator  = ',';


    if(cconfig.getval(tmp,"HEADFLAG",im_tmp.m_headFlag,MAXBUFFLEN))
    {
      strcpy(im_tmp.m_headFlag,"Head Record");
    }

    if(!cconfig.getval(tmp,"HAVEHEAD",buff,MAXBUFFLEN))
    {
       if(strcmp(buff,"YES")==0)
          im_tmp.m_hasHead = true;
       else
          im_tmp.m_hasHead = false;
    }

    if(!cconfig.getval(tmp,"HAVETAIL",buff,MAXBUFFLEN))
    {
       if(strcmp(buff,"YES")==0)
          im_tmp.m_hasTail = true;
       else
          im_tmp.m_hasTail = false;
    }

    if(cconfig.getval(tmp,"TAILFLAG",im_tmp.m_tailFlag,MAXBUFFLEN))
    {
      strcpy(im_tmp.m_tailFlag,"Tail Record");
    }

    getFieldsValues(im_tmp.m_fieldName,im_tmp.m_dateFieldName,
              im_tmp.m_dateFieldNum,im_tmp.m_fieldNum,im_tmp.m_fields,im_tmp.m_values);
    sprintf(im_tmp.m_insertStmt,"INSERT INTO %s(%s) VALUES(%s)",im_tmp.m_tableName,im_tmp.m_fields,im_tmp.m_values);
    m_importModes.push_back(im_tmp);
  }

  if(cconfig.end())
  {
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"cconfig.end()");
    return false;
  }

  return true;
}




//-----------------------------------------------------------------------------
int IndbApp::writeLogMsg(const char *msg)
{
  //m_logstr=msg;
  //writeLog();
  return 0;
}

//-----------------------------------------------------------------------------
int IndbApp::writeAlarm(const int ecode,const char *msg)
{
  m_theErrorMessages->insert(ecode,msg);
  writeAlert();
  return 0;
}

//-----------------------------------------------------------------------------
int IndbApp::writeStatus(const char *msg,const char stat)
{
  m_objectName = msg;
  m_status = stat;
  updateStatus();
  return 0;
}

//-----------------------------------------------------------------------------


int IndbApp::reconnect()
{
  if(connected)
    g_dbinterface.disconnect();

  connected=0;
  if(g_dbinterface.connect(m_userName,m_passwd,m_server))
  {
    return -1;
  }

  connected=1;
  return 0;
}




/* main */
/* main() in application.cpp */

//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//
/* some C function API */
char * truepath(const char *path)
{
  static char truepath[500];
  char env[100];
  char *c;

  if(*path=='~')
  {
    strcpy(truepath,getenv("HOME"));
    strcat(truepath,path+1);
  }
  else if(*path=='$')
  {
    c=(char *)strchr(path,'/');
    if(c==NULL)
    {
      strcpy(truepath,getenv(path+1));
    }
    else
    {
      strncpy(env,path+1,c-path);
      env[c-path-1]=0;
      strcpy(truepath,getenv(env));
      strcat(truepath,c);
    }
  }
  else
  {
    strcpy(truepath,path);
  }

  return truepath;
}



void getFieldsValues(char fieldname[MAXBIND][50],char todatefieldbuf[MAXBIND][50],
                  int todatefieldnum,int fieldnum,char *fields,char *values)
{
  int i,j,dateflag=0;
  for(i=0;i<fieldnum;i++)
  {
    for(j=0;j<todatefieldnum;j++)
    {
      if(strcmp(fieldname[i],todatefieldbuf[j])==0)
        dateflag=1;
    }

    if(dateflag==0)
    {
      if(i==0)
      {
        strcpy(fields,fieldname[i]);
        sprintf(values,":%s",fieldname[i]);
      }
      else
      {
        sprintf(fields,"%s,%s",fields,fieldname[i]);
        sprintf(values,"%s,:%s",values,fieldname[i]);
      }
    }
    else
    {
      if(i==0)
      {
        sprintf(values,"to_date(:%s,'yyyymmdd HH24:MI:SS')",fieldname[i]);
        sprintf(fields,"%s",fieldname[i]);
      }
      else
      {
        sprintf(values,"%s,to_date(:%s,'yyyymmdd HH24:MI:SS')",values,fieldname[i]);
        sprintf(fields,"%s,%s",fields,fieldname[i]);
      }
      dateflag=0;
    }
  }
}


//-----------------------------------------------------------------------------
int getFieldName(char *fieldnamebuf,char fieldname[MAXBIND][50])
{
  int charindex,fieldindex,i;
  for(i=0,charindex=0,fieldindex=0;;i++,fieldindex++)
  {
    while((fieldnamebuf[i]!=',')&&(fieldnamebuf[i]!='\0'))
    {
      fieldname[fieldindex][charindex] = fieldnamebuf[i];
      i++;
      charindex++;
    }
    fieldname[fieldindex][charindex] = '\0';
    charindex = 0;
    trim(fieldname[fieldindex]);
    if(fieldnamebuf[i]=='\0' )
      break;
  }
  return ++fieldindex;
}

//-----------------------------------------------------------------------------
void trim(char *s)
{
  int l,i,k;
  l=strlen(s);
  for(i=l-1;i>=0;i--)
  {
    if(isspace(s[i]))
      s[i]=0;
    else
      break;
  }
  l=strlen(s);
  for(i=0;i<l;i++)
  {
    if(isspace(s[i]))
      continue;
    else
      break;
  }
  if(i!=0)
  {
    for(k=0;k<l-i;k++)
    {
      s[k]=s[k+i];
    }
    s[k]=0;
  }
}
//--------------------------
int split(const char *token,const char *cdr,bool end_type,vector<string> & fields)
{
  char *next,*p;
  char field[1024];
  int i,len,size,token_len;

  fields.clear();
  if((size=strlen(cdr))<=0)
    return 0;

  token_len=strlen(token);
  if(end_type) {
    size-=token_len;
  }
  i=0;
  p=(char *)cdr;
  do {
    next  = p + strcspn(p,token);
    len   = next - p;
    strncpy(field,p,len);
    field[len]=0;
    trim(field);
    fields.push_back(field);
    p= next+token_len;
    i++;
  } while(p-cdr<=size);

  return i;
}
