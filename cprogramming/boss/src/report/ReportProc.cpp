#include "ReportProc.h"

ReportProc::ReportProc(TypdayVector *vday,TypotherVector *vother)//----class ReportProc
{
  m_dayVector = vday;
  m_otherVector = vother;
//  m_theErrorMessages = theErrorMessages;

}

ReportProc::~ReportProc()
{}

int ReportProc::getFileName(int reportFlag,int reportNo,char *thefiledate)
{
  char filename[30],filenamedate[12+1],chDate[8+1],recType[3],orgFileId[255],chFileId[3];
  dayit dayiter;
  otherit otheriter;
  m_reportFlag = reportFlag;
  m_reportNo = reportNo;

  StringVector outvector;
  StringVector errvector;
  vector<string>::iterator it;

  int nTotal,nSuccess,nError,iReturn;
  char selectarray[1000];
  if(reportFlag==1)
  {
    for(dayiter=m_dayVector->begin();dayiter!=m_dayVector->end();dayiter++)
    {
       if( dayiter->reportNo==reportNo )
       {
         memset(filenamedate,0,sizeof(filenamedate));
         m_flag=dayiter->flag;//get flag
         sprintf(m_procedureName,dayiter->procedureName); //get procedurename
         if (strcmp(dayiter->fileNameDate,"YYYYMMDD")==0)//get filename
           sprintf(filenamedate,thefiledate);      
         else if (strcmp(dayiter->fileNameDate,"YYMMDD")==0)
           sprintf(filenamedate,thefiledate+2);
         else if (strcmp(dayiter->fileNameDate,"MMDD")==0)
           sprintf(filenamedate,thefiledate+4);
         else if (strcmp(dayiter->fileNameDate,"YYYYMM")==0)
           strncpy(filenamedate,thefiledate,6);

         if ((strncmp(dayiter->fileNameHead,"S",1) == 0) || (strncmp(dayiter->fileNameHead,"J",1) == 0))
         {
         	    strcpy(recType,dayiter->fileNameHead); 
         	    getcdate(chDate);
         	    connectDb();
            	dbi.dataBind(outvector,errvector);
            	//sprintf(selectarray,"SELECT nvl(max(substr(org_filename,11,2)),'00') from tgl_statisfile where deal_date = '%s' and record_type = '%s'",chDate,recType);
            	memset(selectarray,0,sizeof(selectarray));
            	sprintf(selectarray,"SELECT nvl(max(substr(org_filename,11,2)),'00') from tsl_statisfile where substr(deal_date,1,8) = '%s' and record_type = '%s'",chDate,recType);
              dbi.executeSql(selectarray,nTotal,nSuccess,nError);
              memset(orgFileId,0,sizeof(orgFileId));
              if(nTotal>0)
              {
              	for(it=outvector.begin();it!=outvector.end();it++)
              	{
              		strcpy(orgFileId,(*it).c_str());
              	}
              }
              dbi.disconnect();
              if(strncmp(orgFileId,"00",2)==0)
              {
              	strcpy(chFileId,"01");
              }
              else
              {
                strcpy(chFileId,orgFileId);
			          FileIdAdd(chFileId);
			          if(strcmp(chFileId,MAXFILEID)>0)
			          {
			             return -1;
			          }
		          }
		          sprintf(m_fileName, "%s%s%s.%s", dayiter->fileNameHead,filenamedate,chFileId,dayiter->fileNameEnd);	
         }
         else
         {	                        
        	    sprintf(m_fileName, "%s%s%s.%s", dayiter->fileNameHead,filenamedate,dayiter->fileName3,dayiter->fileNameEnd);
         }
         sprintf(m_fullFileName,dayiter->reportPath);//get fullFileName
         strcat(m_fullFileName,m_fileName);
         sprintf(m_fullBackupName,dayiter->backupPath);//get fullBackupName
         strcat(m_fullBackupName,m_fileName);
         sprintf(m_fileDate,thefiledate);
         return 1;
       }
    }
  }

  if(reportFlag==0)
  {
    for(otheriter=m_otherVector->begin();otheriter!=m_otherVector->end();otheriter++)
    {
       if(otheriter->reportNo==reportNo)
       {          
         m_flag=otheriter->flag;
         sprintf(m_procedureName,otheriter->procedureName);
         sprintf(m_fileName,otheriter->fileName);
         sprintf(m_fullFileName,otheriter->reportPath);
         strcat(m_fullFileName,m_fileName);
         sprintf(m_fullBackupName,otheriter->backupPath);
         strcat(m_fullBackupName,m_fileName);
         if (strcmp(otheriter->lastProcessDate,"M") == 0)
          sprintf(m_fileDate,thefiledate);
         else
          sprintf(m_fileDate,"Y");
         return 1;
       }
     }
   }
  return -1;
}

int ReportProc::callProcedure()
{
  char cSql[256],alarmMsg[256];
  long iReturn;
  int nTotal,nSuccess,nError;

  memset(cSql,0,sizeof(cSql)); 
  
  #ifdef _FOR_ORACLE_
     sprintf(cSql,"call %s('%s')",m_procedureName,m_fileName);
  #endif
  #ifdef _FOR_SYBASE_
     sprintf(cSql,"exec %s '%s'",m_procedureName,m_fileName);
  #endif
  iReturn = dbi.executeSql(cSql,nTotal,nSuccess,nError);
  if (iReturn<0)
  {
    sprintf(alarmMsg,"执行过程失败:%s",m_procedureName);
    m_theErrorMessages->insert(DB_EXECPROC,alarmMsg);
    return -1;
  }
  return 1;
}

int ReportProc::getDataFromDB()
{
  FILE *fpSend,*fpBackup;
  int i,iReturn;
  char alarmMsg[256];
  StringVector outvector;
  StringVector errvector;
  vector<string>::iterator it;
  int nTotal,nSuccess,nError;
  int total=0;
  char selectarray[1000];
  string recordString;
  
  for (i=0;(i<3)&&(connectDb()<0);i++)//connect to DB for 3 times
    sleep(3);
  if (i >= 3)
  {
    m_theErrorMessages->insert(DB_CONNECT,(char *)"Can not connect to DB on getDataFromDB.");
    return -1;
  }
  dbi.beginTransaction();
  iReturn = callProcedure();
  if (iReturn < 0)
  {
    #ifdef _DEBUG_
      cout<<"Call procedure failed. Return:"<<iReturn<<endl;
    #endif
    dbi.rollbackTransaction();
    dbi.disconnect();
    return iReturn;
  }

  iReturn = dbi.dataBind(outvector,errvector);
  if (iReturn < 0)
  {
    sprintf(alarmMsg,"dbi.dataBind() failed.");
    m_theErrorMessages->insert(DB_BIND,alarmMsg);
    dbi.rollbackTransaction();
    dbi.disconnect();
    return -1;
  }

  if((fpSend=fopen(m_fullFileName, "w"))==NULL)
  {
    sprintf(alarmMsg,"Can not open file: %s.",m_fullFileName);
    #ifdef _DEBUG_
      cout<<alarmMsg<<endl;
    #endif
    m_theErrorMessages->insert(OPEN_FILE,alarmMsg);
    dbi.rollbackTransaction();
    dbi.disconnect();
    return -1;
  }
  if((fpBackup = fopen(m_fullBackupName, "w"))==NULL)
  {
    sprintf(alarmMsg,"Can not open file: %s.",m_fullBackupName);
    #ifdef _DEBUG_
      cout<<alarmMsg<<endl;
    #endif
    m_theErrorMessages->insert(OPEN_FILE,alarmMsg);
    dbi.rollbackTransaction();
    dbi.disconnect();
    fclose(fpSend);
    return -1;
  }
  nTotal=DEFAULTARRAYLINE;
  sprintf(selectarray,"SELECT rec_txt FROM %s order by seq_no",reportTableName);
  while(nTotal==DEFAULTARRAYLINE)
  {
    if(dbi.executeSql(selectarray,nTotal,nSuccess,nError))
    {
      #ifdef _DEBUG_
        cout<<"EXEC SQL failed. SQL:"<<selectarray<<endl;
      #endif
      sprintf(alarmMsg,"dbi.executeSql() failed.");
      m_theErrorMessages->insert(DB_EXECSQL,alarmMsg);
      dbi.rollbackTransaction();
      dbi.disconnect();
      fclose(fpSend);
      fclose(fpBackup);
      return -1;
    }
    if(nTotal>0)
    {
      total+=nTotal;
      for(it=outvector.begin();it!=outvector.end();it++)
      {
        fprintf(fpSend,"%s",(*it).c_str());
        //modi by cy 2002.08.02 局数据部中心要求为换行符
        if(strncmp(m_fileName,"WHI",3)==0)
        {
         fprintf(fpSend,"%c",LF);
        }
        else
        {	
        fprintf(fpSend,"%c",RN);
        fprintf(fpSend,"%c",LF);
        }
        
        fprintf(fpBackup,"%s",(*it).c_str());
        if(strncmp(m_fileName,"WHI",3)==0)
        {
         fprintf(fpBackup,"%c",LF);
        }
        else
        {
        fprintf(fpBackup,"%c",RN);
        fprintf(fpBackup,"%c",LF);
	}
      }
    }
  }
#ifdef _DEBUG_
cout<<"Report record number:"<<total<<endl;
#endif
  if (total == 0)//there are no record on DB
  {
    dbi.rollbackTransaction();
    dbi.disconnect();
    fclose(fpSend);
    fclose(fpBackup);
    sprintf(alarmMsg,"There are no record on %s.",reportTableName);
    m_theErrorMessages->insert(DB_NO_RECORD,alarmMsg);
    unlink(m_fullFileName);
    return 0;
  }
  if(m_reportFlag==0)
    sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
      OTHER_PARAM_TABLE,m_fileDate,m_reportNo);
  else
    sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
      DAY_PARAM_TABLE,m_fileDate,m_reportNo);
  if(dbi.executeSql(selectarray,nTotal,nSuccess,nError))
  {
    #ifdef _DEBUG_
      cout<<"EXEC SQL failed. SQL:"<<selectarray<<endl;
    #endif
    dbi.rollbackTransaction();
    dbi.disconnect();
    fclose(fpSend);
    fclose(fpBackup);
    sprintf(alarmMsg,"Can not write log.Report no:%d.",m_reportNo);
    m_theErrorMessages->insert(DB_NO_RECORD,alarmMsg);
    unlink(m_fullFileName);
    return 0;
  }

  sprintf(selectarray,"DELETE FROM %s",reportTableName);
  dbi.executeSql(selectarray,nTotal,nSuccess,nError);
  dbi.commitTransaction();
  dbi.disconnect();
  fclose(fpSend);
  fclose(fpBackup);

  return 1;
}

int ReportProc::getDataFromFile()
{
  FILE *fpReceive,*fpBackup;
  char cRecord[2000],cSql[2000],alarmMsg[256];
  char cTemp[64] = "INSERT INTO %s VALUES (%ld,'%s')";
  char selectarray[1000];
  int i,nTotal,nSuccess,nError;
  long iReturn;
  long lTemp = 1;
  if((fpReceive=fopen(m_fullFileName,"r"))==NULL)
  {
  #ifdef _DEBUG_
    cout<<"fopen  failed"<<m_fullFileName<<endl;
  #endif

    return -1;
   }
  if((fpBackup=fopen(m_fullBackupName,"w"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<alarmMsg<<endl;
    #endif
    sprintf(alarmMsg,"Can not open file: %s.",m_fullBackupName);
    m_theErrorMessages->insert(OPEN_FILE,alarmMsg);
    fclose(fpReceive);
    return -1;
  }
  
  for (i=0;(i<3)&&(connectDb()<0);i++)//connect to DB for 3 times
    sleep(3);
  if (i >= 3)
  {
    m_theErrorMessages->insert(DB_CONNECT,(char *)"Can not connect to DB on getDataFromDB.");
    fclose(fpReceive);
    fclose(fpBackup);
    return -1;
  }
  sprintf(selectarray,"DELETE FROM %s",reportTableName);
  if(dbi.executeSql(selectarray,nTotal,nSuccess,nError))
  {
    #ifdef _DEBUG_
      cout<<"EXEC SQL failed. SQL:"<<selectarray<<endl;
    #endif
    m_theErrorMessages->insert(DB_EXECSQL,(char *)"Can not delete record from report table.");
    dbi.disconnect();
    fclose(fpReceive);
    fclose(fpBackup);
    return -1;
  }

  dbi.beginTransaction();
  while(1)
  {
    if(fgets(cRecord,2000,fpReceive)==NULL)
      break;
    if(strlen(cRecord)<3)
      continue;
    fprintf(fpBackup,cRecord);
    sprintf(cSql,cTemp,reportTableName,lTemp,cRecord);
      #ifdef _DEBUG_
        cout<<"cSql:"<<cSql<<endl;
      #endif    
    iReturn = dbi.executeSql(cSql,nTotal,nSuccess,nError);
    if (iReturn < 0)
    {
      #ifdef _DEBUG_
        cout<<"EXEC SQL failed. SQL:"<<selectarray<<endl;
      #endif
      sprintf(alarmMsg,"Can not insert record to DB,file name: %s.",m_fullFileName);
      m_theErrorMessages->insert(DB_EXECSQL,alarmMsg);
      dbi.rollbackTransaction();
      dbi.disconnect();
      fclose(fpReceive);
      fclose(fpBackup);
      return -1;
    }
    //dbi.commitTransaction();
    lTemp++;
  }
  //dbi.commitTransaction();
  unlink(m_fullFileName);
  #ifdef _FOR_ORACLE_
     sprintf(cSql,"call %s('%s')",m_procedureName,m_fileName);
  #endif
  #ifdef _FOR_SYBASE_
     sprintf(cSql,"exec %s '%s'",m_procedureName,m_fileName);
  #endif
  iReturn = dbi.executeSql(cSql,nTotal,nSuccess,nError);
  #ifdef _DEBUG_
    cout<<"cSql:"<<cSql<<".return:"<<iReturn<<endl;
    dbi.commitTransaction();
  #endif
  
  if (iReturn<0)
  {
    sprintf(alarmMsg,"执行过程失败:%s",m_procedureName);
    m_theErrorMessages->insert(DB_EXECPROC,alarmMsg);
    dbi.rollbackTransaction();
    dbi.disconnect();
    fclose(fpReceive);
    fclose(fpBackup);
    return -1;
  }

/*  
  if(iReturn = 999)
  {
    sprintf(alarmMsg,"发生文件级错误:%s",m_procedureName);
    m_theErrorMessages->insert(DB_EXECPROC,alarmMsg);
  }
*/
  
  if(m_reportFlag==0)
    sprintf(selectarray,"UPDATE %s SET lastprocessdate='Y' WHERE reportno=%d",
      OTHER_PARAM_TABLE,m_reportNo);
  else
    sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
      DAY_PARAM_TABLE,m_fileDate,m_reportNo);
  if(dbi.executeSql(selectarray,nTotal,nSuccess,nError))
  {
    dbi.rollbackTransaction();
    dbi.disconnect();
    fclose(fpReceive);
    fclose(fpBackup);
    sprintf(alarmMsg,"Can not write log.Report no:%d.",m_reportNo);
    m_theErrorMessages->insert(DB_NO_RECORD,alarmMsg);
    unlink(m_fullFileName);
    return 0;
  }

  dbi.commitTransaction();
  dbi.disconnect();
  fclose(fpReceive);
  fclose(fpBackup);
  
  return 1;
}

int ReportProc::connectDb()
{
  int iReturn = dbi.connect(dbUser,dbPassword,dbServer);
  #ifdef _FOR_SYBASE_
    if (iReturn >= 0)
    if (strlen(dbName) != 0)
      iReturn = dbi.useDb(dbName);
  #endif
  return iReturn;
}


int ReportProc::mainProcess()
{
  int iReturn;
  if (m_flag==1)
  {     
	iReturn = getDataFromDB(); 
  #ifdef _DEBUG_
    cout<<"getDataFromDB"<<m_fileName<<". m_flag:"<<m_flag<<"iReturn:"<<iReturn<<endl;
  #endif
  }     
  else
     iReturn = getDataFromFile();
  #ifdef _DEBUG_
    cout<<"Processed file name: "<<m_fileName<<". m_flag:"<<m_flag<<"iReturn:"<<iReturn<<endl;
  #endif
  return iReturn;
}

int ReportProc::FileIdAdd(char *chFileId)
{
	int iFileId;
	if((chFileId[0]>='0')&&(chFileId[0] <= '9')&&(chFileId[1]>='0')&&(chFileId[1] <= '9'))
	{
		iFileId = atoi(chFileId);
	}
	else
	{
		iFileId = (chFileId[1] - 'A') + (chFileId[0] - 'A' )*26 + 100;
	}
	iFileId++;
	memset(chFileId,0,sizeof(chFileId));
	if (iFileId < 100)
	{
		sprintf(chFileId,"%.2d",iFileId);
		return 0;
	}
	sprintf(chFileId,"%c%c",'A'+(iFileId-100)/26,'A'+((iFileId-100) % 26));
	return 0;
}
