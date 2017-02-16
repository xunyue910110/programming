#include "config-all.h"

#include "base/SysParam.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"
#include "file/File.h"
#include "appfrm/ErrorHandle.h"
#include "indb.h"

#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <string>

USING_NAMESPACE(std)

void  getFieldsValues(char (*)[50],char (*)[50],int ,int ,char *,char *);
int   getFieldName(char *fieldnamebuf,char fieldname[MAXBIND][50]);
void  trim(char *s);
char  *truepath(const char *path);
int   split(const char *token,const char *cdr,bool end_type,vector<string> & fields);

string   theIndbLoggerProcessID[MAXTHREADCONNUM];

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
    //if(incompleteFlag[i]=='\'')
    //modified by yinb 2011/2/28 14:16:59 bug fix
    if(incompleteFlag[i]=='\\')
      continue;
    m_incompleteFlag[j]=incompleteFlag[i];
    j++;
  }
  m_incompleteFlag[j]=0;
  
  if(!getConfig())
  { 
    m_theErrorMessages->insert(EAPPFRM_SYSVAL,"configfile incorrect");
    return false;
  }
 
  for(i = 1;i<(m_connectNum + 1);i++) 
  {
     if( reconnect(i) )
     { 
       sprintf(m_msgbuf[i],"Error(%s:%d):database connect failed.",
               __FILE__,__LINE__);            
       writeAlarm(ERROR_INDB_DBCONN,m_msgbuf[i]);
       return false;
     }
  }

  m_abnormal = false;
  m_reserved = "";
  m_objectName = "";
  updateStatus();

  return true;
}


bool IndbApp::end()
{
	for(int i = 1;i<(m_connectNum + 1);i++)
	{
    if(m_connected[i])
      m_dbinterface[i].disconnect();
    m_connected[i] = 0;
  }

  return Application::end();
}


bool IndbApp::recovery()
{
  Application::recovery();
  return true;
}


bool IndbApp::getConfig()
{
   SimpleConfigure cconfig;
   ImportMode im_tmp;
   char       longbuff[MAXLONGBUFFLEN+1];
   char       buff[MAXBUFFLEN+1];
   char       buff2[MAXBUFFLEN+1];
   char       tmp[MAXBUFFLEN+1];
   int        i=0,j=0;

   //initialize
   if(cconfig.initialize(m_configFile.c_str()))
   {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,"cconfig.initialize()");
     return false;
   }
    
   //get DB Configuration  
   if(cconfig.getval("DB","CONNECTNUM",tmp,MAXNAMELEN))
   {
   	  m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:CONNECTNUM not define");
      return false;
   }  
   m_connectNum = atoi(tmp);
   if(m_connectNum > MAXTHREADCONNUM)
   {
   	  m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:CONNECTNUM oversize");
      return false;  	
   }
   
   if(cconfig.getval("DB","CONNYESNO",m_connYesNo,MAXNAMELEN))
   {
   	  m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:CONNYESNO not define");
      return false;
   }
   
   if(!cconfig.getval("DB","CONNWAY",buff,MAXNAMELEN))
   {
     char *result=NULL;
     result=strtok(buff,",");
     m_filetailBegin = atoi(result);
     result=strtok(NULL,",");
     m_filetailLen = atoi(result);
   } 
 
   char path[MAXPATHLEN+1];
   for(i=1;i<(m_connectNum+1);i++)
   {
   	  // get DB Configuration
   	  sprintf(tmp,"USER%d",i);
      if(cconfig.getval("DB",tmp,m_userName[i],MAXNAMELEN))
      {
      	sprintf(tmp,"config error:USER%d not define",i); 	   	 
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);
        return false;
      }
DEBUG_PRINTF("------%d----%s\n",__LINE__, m_userName[i]);      
      sprintf(tmp,"PASSWD%d",i);
      if(cconfig.getval("DB",tmp,m_passwd[i],MAXNAMELEN))
      {
      	 sprintf(tmp,"config error:PASSWD%d not define",i);  	 
         m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);
         return false;
      }
DEBUG_PRINTF("------%d----%s\n",__LINE__, m_passwd[i]);      
      sprintf(tmp,"SERVER%d",i);
      if(cconfig.getval("DB",tmp,m_server[i],MAXNAMELEN))
      {
      	 sprintf(tmp,"config error:SERVER%d not define",i);      	 
         m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);
         return false;
      }
DEBUG_PRINTF("------%d----%s\n",__LINE__, m_server[i]);      
      // get general Configuration                                                              
      sprintf(tmp,"SOURCEPATH%d",i);                                                        
      if(cconfig.getval("GENERAL",tmp,path,MAXPATHLEN))                                         
      {                                                                                         
   	 	  sprintf(tmp,"config error:SOURCEPATH[%d] not define",i); 	 	                            
        m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);                                         
        return false;                                                                           
      }                                                                                         
      strcpy(m_sourcePath[i],truepath(path));                                                   
                                                                                                
      sprintf(tmp,"FAILEDPATH%d",i);                                                        
      if(cconfig.getval("GENERAL",tmp,path,MAXPATHLEN))                                         
      {                                                                                                                                      
        if(cconfig.getval("GENERAL","NOTINPATH",path,MAXPATHLEN))                               
        {                                                                                       
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:NOTINPATH not define");      
           return false;                                                                        
        }                                                                                                                                                                                                                         
      }                                                                                         
      strcpy(m_failedPath[i],truepath(path));                                                   
                                                                                                
      sprintf(tmp,"ERRORPATH%d",i);                                                         
      if(cconfig.getval("GENERAL",tmp,path,MAXPATHLEN))                                         
      {                                                                                         
      	 sprintf(tmp,"config error:ERRORPATH[%d] not define",i);                            
         m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);                                        
         return false;                                                                          
      }                                                                                         
      strcpy(m_errorPath[i],truepath(path));                                                    
                                                                                                
      strcpy(path,"");                                                                          
      sprintf(tmp,"BACKUPPATH%d",i);                                                        
      cconfig.getval("GENERAL",tmp,path,MAXPATHLEN);                                            
      split(",",path,true,m_backuppathVector[i]);                                               
 
      sprintf(tmp,"FILETYPE%d",i);                                                          
      cconfig.getval("GENERAL",tmp,m_fileType[i],MAXPATHLEN); 
 
      int fieldLength=0,arrayLine=0;
      
      sprintf(tmp,"FIELDLENGTH%d",i);
      if(cconfig.getval("GENERAL",tmp,buff,MAXBUFFLEN)==0)
      {
         fieldLength = atoi(buff);
         if(fieldLength>0)
            m_dbinterface[i].setFieldLength(fieldLength);
      }
      
      sprintf(tmp,"ARRAYLINE%d",i);
      if(cconfig.getval("GENERAL",tmp,buff,MAXBUFFLEN)==0)
      {
         arrayLine = atoi(buff);
         if(arrayLine>0)
            m_dbinterface[i].setArrayLine(arrayLine);
      }
                                                                                                 
      sprintf(tmp,"FORMATNUM%d",i);                                                          
      if(cconfig.getval("GENERAL",tmp,path,MAXPATHLEN))                                
      {                                                                                         
       	sprintf(tmp,"config error:FORMATNUM[%d] not define",i);       	                             
         m_theErrorMessages->insert(EAPPFRM_SYSVAL,tmp);                                        
         return false;                                                                          
      }
      m_formateNum[i] = atoi(path);                                                       
   }
 
   // get format Configuration
   for(i=1;i<=m_connectNum;i++)
   { 	
      for(j=1;j<=m_formateNum[i];j++)
      {
      	  sprintf(tmp,"FORMAT%d",j);
      	  sprintf(buff,"FILEPATTERN%d",i);  	  
         if(cconfig.getval(tmp,buff,longbuff,MAXLONGBUFFLEN))
         {
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:DATABASE not define");
           return false;
         }
         im_tmp.m_filePatterns.clear();                    
         split(",",longbuff,true,im_tmp.m_filePatterns);
 
         sprintf(buff,"TABLE%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_tableName,MAXNAMELEN))
         {
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TABLE not define");
           return false;
         }
         
         sprintf(buff,"PROCEDURE%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_procedure,MAXBUFFLEN))
         {
            m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:PROCEDURE not define");
            return false;
         }
         
         sprintf(buff,"TABLEFIELD%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_tableField,MAXLONGBUFFLEN))
         {
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TABLEFIELD not define");
           return false;
         }  
DEBUG_PRINTF("------%d----%s\n",__LINE__, im_tmp.m_tableField);             
         im_tmp.m_fieldNum = getFieldName(im_tmp.m_tableField,
                                             im_tmp.m_fieldName);
         
         sprintf(buff,"DATEFIELD%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_dateField,MAXLONGBUFFLEN)==0)
           im_tmp.m_dateFieldNum = getFieldName(im_tmp.m_dateField,
                                                im_tmp.m_dateFieldName);
       
         getFieldsValues(im_tmp.m_fieldName,im_tmp.m_dateFieldName,
                         im_tmp.m_fieldNum,im_tmp.m_dateFieldNum,
                         im_tmp.m_fields,im_tmp.m_values);
                         
DEBUG_PRINTF("------%d----%d\n",__LINE__, im_tmp.m_fieldNum);  
DEBUG_PRINTF("------%d----%s\n",__LINE__, im_tmp.m_tableField);  
DEBUG_PRINTF("------%d----%s",__LINE__, im_tmp.m_insertStmt);     
                   
         sprintf(im_tmp.m_insertStmt,"INSERT INTO %s(%s) VALUES(%s)",
                 im_tmp.m_tableName,im_tmp.m_fields,im_tmp.m_values); 
DEBUG_PRINTF("------%d----%s\n",__LINE__, im_tmp.m_insertStmt);                     
         sprintf(buff,"DATABASE%d",i);        
         if(cconfig.getval(tmp,buff,im_tmp.m_database,MAXNAMELEN))
         {
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:DATABASE not define");
           return false;
         }
         
         sprintf(buff,"EXECUTESQL%d",i); 
         cconfig.getval(tmp,buff,im_tmp.m_executeSql,MAXNAMELEN);
         
         sprintf(buff,"TRUNCATE%d",i);           
         if(cconfig.getval(tmp,buff,buff2,MAXBUFFLEN))
         {      	
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:TRUNCATE not define");
           return false;
         }     
         if(strncmp(buff2,"YES",3))
           im_tmp.m_truncateTable = false;
         else
           im_tmp.m_truncateTable = true;
         
         sprintf(buff,"FLAGOFEXEPROC%d",i);     
         if(cconfig.getval(tmp,buff,buff2,MAXBUFFLEN))
         {        	
           m_theErrorMessages->insert(EAPPFRM_SYSVAL,"config error:FLAGOFEXEPROC not define");
           return false;
         }
         if(strcmp(buff2,"EXE"))
           im_tmp.m_execProc = false;
         else
           im_tmp.m_execProc = true;
 
         sprintf(buff,"FIELDLENGTH%d",i); 
         if(cconfig.getval(tmp,buff,buff2,MAXBUFFLEN)==0)
         {
           im_tmp.m_fieldLength = atoi(buff2);
         }
         
         sprintf(buff,"ARRAYLINE%d",i);
         if(cconfig.getval(tmp,buff,buff2,MAXBUFFLEN)==0)
         {
           im_tmp.m_arrayLine = atoi(buff2);
         }
         
         sprintf(buff,"FIELDSEP%d",i); 
         if(!cconfig.getval(tmp,buff,buff2,MAXBUFFLEN))
         {
           if(buff2[0]==0x5c)  // "\"
             im_tmp.m_fieldSeperator = atoi(buff2+1);
           else
             im_tmp.m_fieldSeperator = buff2[0];
         }
         else
           im_tmp.m_fieldSeperator  = ',';
         
         sprintf(buff,"HEADFLAG%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_headFlag,MAXBUFFLEN))
         {
           strcpy(im_tmp.m_headFlag,"Head Record");
         }
         
         sprintf(buff,"HAVEHEAD%d",i);
         if(!cconfig.getval(tmp,buff,buff2,MAXBUFFLEN))
         {
            if(strcmp(buff2,"YES")==0)
               im_tmp.m_hasHead = true;
            else
               im_tmp.m_hasHead = false;
         }
         
         sprintf(buff,"HAVETAIL%d",i);
         if(!cconfig.getval(tmp,buff,buff2,MAXBUFFLEN))
         {
            if(strcmp(buff2,"YES")==0)
               im_tmp.m_hasTail = true;
            else
               im_tmp.m_hasTail = false;
         }
         
         sprintf(buff,"TAILFLAG%d",i);
         if(cconfig.getval(tmp,buff,im_tmp.m_tailFlag,MAXBUFFLEN))
         {
           strcpy(im_tmp.m_tailFlag,"Tail Record");
         } 
          
         m_importModes[i].push_back(im_tmp);      
      }
   }
 
   if(cconfig.end())
   {
     m_theErrorMessages->insert(EAPPFRM_SYSVAL,"cconfig.end()");
     return false;
   }
 
// cout<<"IndbApp::getConfig-------------OK!!"<<endl;
 
   return true;
}


int IndbApp::reconnect(int connum)
{ 
  if(m_connected[connum])
    m_dbinterface[connum].disconnect();
 
  m_connected[connum] = 0;
  
  if(m_dbinterface[connum].connect(m_userName[connum],m_passwd[connum],m_server[connum]))
  {
    return -1;
  }
  m_connected[connum] = 1;
  
  return 0;
}


bool IndbApp::ThreadIndb(const Thread_indb &thr)
{   
	int  fileNum = 0;
  char fileName[MAXFILELEN+1];
  int  pollTime = 0;                              //轮询目录6次还未取到文件则本次线程处理结束进行下一循环 
  Directory tDir;

  while(!thr.m_pSelf->m_stop&&!thr.m_pSelf->m_abnormal)
  { 
  	theIndbLoggerProcessID[thr.thrNum].clear();
     	 	
    //捞不到文件时再重刷目录 20090409
    if(fileNum <=0)
   	{
	  	tDir.setPath((thr.m_pSelf->m_sourcePath[thr.thrNum])); 
	    tDir.scanFiles(Directory::SF_MODIFYTIME);
    }
  	 //-------------关联入库文件判断------------------------------------------------	
  	if(!(strncmp(thr.m_pSelf->m_connYesNo,"YES",3)))
  	{ 
  		memset(fileName,'\0',MAXFILELEN+1);
  		pollTime = 0;
  		int i=thr.thrNum;

  		if(thr.m_pSelf->m_count)
  		{			
  		  while(!thr.m_pSelf->m_openFlag&&!thr.m_pSelf->m_stop)
  		  {	 		  		
  		  }  		  
  	  }  	
 
  		while(1)
  		{	
  			 if(pollTime >= MAXPOLLTIME)  
  			 {
  			 	  strcpy(thr.m_pSelf->m_cmmitYesNo[thr.thrNum],"Y");
  			 	  break;
  			 }	  
  			 else if(pollTime < MAXPOLLTIME)
  			 {	  
      			 i=i%(thr.m_pSelf->m_connectNum);
      			 			 
      			 //线程取关联文件前，先判断关联文件数组中是否有关联文件，若有则根据配置信息得到本线程处理的关联文件名
      			 for(int j=1;j<(thr.m_pSelf->m_connectNum+1);j++)
      		   {
      		      if(strlen(thr.m_pSelf->m_connFile[j]))
      		      {
      		      	 int matched=-1;
      		      	 for(int k=0; k<thr.m_pSelf->m_importModes[j].size(); k++)
                   {  
                      if(thr.m_pSelf->m_importModes[j][k].isMatchMode(thr.m_pSelf->m_connFile[j]))
                      {
                         matched = k;
                         break;
                      }                 
                   }
                   
      		      	 if(matched==-1)
      		      	 {
      		      	   continue;	
      		      	 }
	      	 
      		      	 strncpy(fileName,thr.m_pSelf->m_connFile[j],strlen(thr.m_pSelf->m_connFile[j]));
      		      	   		      	               
      		      	 for(int k=0;k<(thr.m_pSelf)->m_filetailLen;k++)
      		      	    fileName[strlen(thr.m_pSelf->m_connFile[j]) - thr.m_pSelf->m_filetailBegin + k] 
      		      	            = thr.m_pSelf->m_fileType[thr.thrNum][k];

      		         break;	
      		      }	
      		   }
      			 
      			 if(!strlen(fileName))
      			 { 			 	
               if(i==1)
      		     {    		     	 
      		     	 strcpy(thr.m_pSelf->m_cmmitYesNo[thr.thrNum],"N");
                 if((fileNum=tDir.getFile(fileName,sizeof(fileName)))==0)
                 {
                   // No file need import,sleep 10s ,continue loop
                   //thr.m_pSelf->writeStatus("",PROC_IDLE);
                   thr.m_pSelf->ssleep(1);
                   strcpy(thr.m_pSelf->m_cmmitYesNo[thr.thrNum],"N");
                   i=i+1;
                   pollTime++;             
                   continue;        
                 }              
      		     }  		     
      		     else
      		     {  		     	     	 
      		     	 thr.m_pSelf->ssleep(1);
                 strcpy(thr.m_pSelf->m_cmmitYesNo[thr.thrNum],"N");
                 i=i+1;
                 pollTime++;             
                 continue;
      		     }
      		   } 		    
      		   else
      		   {
      		     char  fullFileName[MAXPATHLEN+MAXFILELEN+1];
      		     sprintf(fullFileName,"%s/%s",thr.m_pSelf->m_sourcePath[thr.thrNum],fileName);
      		     File fExists(fullFileName);
     
      		     if(!fExists.exists())
      		     {		     	
                  thr.m_pSelf->ssleep(1);
                  strcpy(thr.m_pSelf->m_cmmitYesNo[thr.thrNum],"N");                 
                  i=i+1;
                  pollTime++;   
                  continue; 
      		     }
      		   }
      		   	        
             strcpy(thr.m_pSelf->m_connFile[thr.thrNum],fileName);      
             break;
         } 
  		}   
  	}
  	//--------------------------------------------------------------------------
  	else
    {	
  	  memset(fileName,'\0',MAXFILELEN+1); 		    	
  	  if((fileNum=tDir.getFile(fileName,sizeof(fileName)))==0)
      {
        //No file need import,sleep 10s ,continue loop
        if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
        {
        	 thr.m_pSelf->m_abnormal = true;
        	 break;
        }
        else
        {	
           thr.m_pSelf->writeStatus("",PROC_IDLE);
           pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));        
           if(thr.m_pSelf->m_connected[thr.thrNum])
              (thr.m_pSelf->m_dbinterface[thr.thrNum]).disconnect();
           thr.m_pSelf->m_connected[thr.thrNum] = 0;
           sleep(10);
           //thr.m_pSelf->ssleep(10);          
           continue;
        }        
      }
    }
	
    if(strlen(thr.m_pSelf->m_incompleteFlag)>0)
    {
      if(!strncmp(fileName,thr.m_pSelf->m_incompleteFlag,
                  strlen(thr.m_pSelf->m_incompleteFlag)))
      {        
         if(thr.m_pSelf->m_connected[thr.thrNum])
            (thr.m_pSelf->m_dbinterface[thr.thrNum]).disconnect();
         thr.m_pSelf->m_connected[thr.thrNum]=0;
         
         if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
         {
         	  thr.m_pSelf->m_abnormal = true;
         	  break;
         }
         else
         {	
         	  thr.m_pSelf->writeStatus("",PROC_IDLE);
            pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
            //thr.m_pSelf->ssleep(5);
            sleep(5);
            continue;
         }
      }
    }
      
    if(!thr.m_pSelf->m_connected[thr.thrNum])
    {            
      if(thr.m_pSelf->reconnect(thr.thrNum))
      {          
        sprintf(thr.m_pSelf->m_msgbuf[thr.thrNum],"Error(%s:%d):%s database connect failed",
                __FILE__,__LINE__,thr.m_pSelf->m_server[thr.thrNum]);
        if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
        {
           thr.m_pSelf->m_abnormal = true;
           break;	
        }
        else
        {
           thr.m_pSelf->writeAlarm(ERROR_INDB_DBCONN,thr.m_pSelf->m_msgbuf[thr.thrNum]);
           thr.m_pSelf->writeStatus("",PROC_WARN);
           thr.m_pSelf->m_abnormal = true;
           pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
           sleep(60);
           break;
        }
      }          
    }            
 
    // Process File,write log file     
    if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
    {
        thr.m_pSelf->m_abnormal = true;
        break;  	
    }
    else
    {               
        thr.m_pSelf->writeStatus(fileName,PROC_RUN); 
        theIndbLoggerProcessID[thr.thrNum] = theLogger->pBegin();              
        pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
    }  
              
    //if(pollTime == 6 || !strlen(fileName))  pollTime=0;
    if(pollTime == MAXPOLLTIME )  
    {	
    	 pollTime=0;
    }
    else
    {             	
      if(!thr.m_pSelf->process(fileName,thr.thrNum)) 
      {   	
         if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
         {
            thr.m_pSelf->m_abnormal = true;
            break;  	
         }
         else
         {
            thr.m_pSelf->m_abnormal = true;
            theLogger->pEnd(theIndbLoggerProcessID[thr.thrNum]);
            pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
            break;
         } 
  	  }
  	   	  
	#ifdef _DEBUG_  	   	  
   cout<<" Thread "<<thr.thrNum<<" PROCESS OK -- fileName="<<fileName<<endl<<endl; 
	#endif
    }    
    
    if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
    {
       thr.m_pSelf->m_abnormal = true;
       break;  	
    }
    else
    {   
       theLogger->pEnd(theIndbLoggerProcessID[thr.thrNum]);
       pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
    }                           
        
    if(thr.m_pSelf->m_abnormal)
    {
      break;
    }
    else
    {
       if(!(strncmp(thr.m_pSelf->m_connYesNo,"YES",3)))
       {    
          if(pthread_mutex_lock(&(thr.m_pSelf->m_proMutex)) != 0)
          {
            thr.m_pSelf->m_abnormal = true;
            break;  	
          } 
          else
          {   	    	
             (thr.m_pSelf->m_number)++;
             (thr.m_pSelf->m_count)++;
             (thr.m_pSelf->m_num)++;
            
             if(thr.m_pSelf->m_num == 1)  
             	  thr.m_pSelf->m_openFlag = 0; 
             pthread_mutex_unlock(&(thr.m_pSelf->m_proMutex));
          } 
          
          if((thr.m_pSelf->m_number)==3&&(thr.m_pSelf->m_openFlag)==0)
          {
          	for(int i=1; i<(thr.m_pSelf->m_connectNum + 1);i++)
            {
               memset(thr.m_pSelf->m_connFile[i],'\0',MAXFILELEN+1); 
               memset(thr.m_pSelf->m_cmmitYesNo[i],'\0',2); 
            } 
                               
          	(thr.m_pSelf->m_number) = 0;
          	(thr.m_pSelf->m_num) = 0;
          	(thr.m_pSelf->m_openFlag)=1;      	
          	
          }         
       }
    }

  }
  
  //process over,exit!! 
  pthread_exit(NULL);
	return true;
}


bool IndbApp::loopProcess()
{ 
  int i=0,j=0;
  pthread_t threadId[MAXTHREADCONNUM+1];//xuyb 20090224 使用的时候从1开始，如果使用了5个线程会溢出
  Thread_indb  t_indb[MAXTHREADCONNUM+1];//

  m_openFlag = 0;                                 
  m_number = 0;                                   
  m_num = 0;                                      
  m_count = 0;                                    
  m_abnormal = false;

  pthread_mutex_init(&m_proMutex,NULL);
//  theLoggerProcessID = theLogger->pBegin();

  for( i = 1; i < (m_connectNum+1); i++)
  {  
  	 memset(m_connFile[i],'\0',MAXFILELEN+1); 
     memset(m_cmmitYesNo[i],'\0',2);
  	
     t_indb[i].thrNum  = i;
     t_indb[i].m_pSelf = this;
     
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     pthread_attr_setstacksize(&attr, THREADSTACKSIZE );
    
     if((pthread_create(&(threadId[i]), &attr,
     	                 (void *(*)(void *))ThreadIndb,
     	                 &t_indb[i])) != 0)
     {  
        for (j = 1;j <= i;j++)
            pthread_join(threadId[j],NULL);

       //xuyb 20090224 线程异常的时候，需要销掉线程锁
       pthread_mutex_destroy(&m_proMutex);
        return Application::loopProcess();
     }
  }

  for(i = 1; i < m_connectNum+1; i++)
  {  
     pthread_join(threadId[i],NULL);
  }
  
//  theLogger->pEnd(theLoggerProcessID);  
  pthread_mutex_destroy(&m_proMutex);
  
  if(m_abnormal == true)
  {
    m_stop = true;
    return false;
  }
  else
  {
    return Application::loopProcess();
  }
}


bool IndbApp::process(const char *fileName,int connum) 
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

  //log the process begin time
  time(&s_time);
  gettimeofday(&s_timeval,&tzp);
  ts=localtime(&s_time);
  strftime(s_ts,15,"%Y%m%d%H%M%S",ts);

  month = ts->tm_mon + 1;

  sprintf(fullFileName,"%s/%s",m_sourcePath[connum],fileName);

  for(i=0; i< m_importModes[connum].size(); i++)
  {
     if(m_importModes[connum][i].isMatchMode(fileName))
     {
        matched = i;
        break;
     }
  }

  char failedFileName[MAXPATHLEN+MAXFILELEN+1];

  //if can match import mode,move file to failedPath
  if(matched == -1)
  {
     sprintf(failedFileName,"%s/%s",m_failedPath[connum],fileName);
     if(rename(fullFileName,failedFileName))
     { 
       sprintf(m_msgbuf[connum],"Error(%s:%d):can't rename for (%s) file ",
               __FILE__,__LINE__,fullFileName); 
               
DEBUG_PRINTF("----- %s \n",m_msgbuf[connum]);                   
                           
       if(pthread_mutex_lock(&m_proMutex) != 0)
       {
          return false;
       }
       else
       {        
          writeAlarm(ERROR_INDB_MOVEFILE,m_msgbuf[connum]);
          writeStatus(fileName,PROC_WARN);
          pthread_mutex_unlock(&m_proMutex);      
          sleep(60);                                                    
          return false;
       }
     }
     sprintf(m_msgbuf[connum],"Error(%s:%d):can't match importMode for (%s) file ",
               __FILE__,__LINE__,fullFileName); 

DEBUG_PRINTF("----- %s \n",m_msgbuf[connum]);  
            
     if(pthread_mutex_lock(&m_proMutex) != 0)
     {
        return false;
     }
     else
     {         
        writeAlarm(ERROR_INDB_MOVEFILE,m_msgbuf[connum]);
        theLogger->pInput(theIndbLoggerProcessID[connum],fullFileName,-1,"type='invalid file'");
        theLogger->pOutput(theIndbLoggerProcessID[connum],failedFileName,-1,"type='invalid file'");
        pthread_mutex_unlock(&m_proMutex);
        return false;
     }
  }
  
DEBUG_PRINTF("-----filename = %s \n",fileName);  
   
  //if sybase, call 'useDB'
  if(strlen(m_importModes[connum][matched].m_database)>0)
  {
    if(m_dbinterface[connum].useDb(m_importModes[connum][matched].m_database))
    {
       return false;
    }
  }

  // if needed, truncate table
  // if truncate failed, return false;
  if(m_importModes[connum][matched].m_truncateTable)
  {
     sprintf(sqlcmd,"TRUNCATE TABLE %s",m_importModes[connum][matched].m_tableName);

     if(m_dbinterface[connum].executeSql(sqlcmd,nTotal,nSuccess,nError))
     {
        sprintf(m_msgbuf[connum],"Error(%s:%d):cdbinterface.executeSql(%s) failed",
               __FILE__,__LINE__,sqlcmd);

DEBUG_PRINTF("----- %s \n",m_msgbuf[connum]);  
            
        if(pthread_mutex_lock(&m_proMutex) != 0)
        {
           return false;
        }
        else
        {        
           writeAlarm(ERROR_INDB_EXECSQL,m_msgbuf[connum]);
           writeStatus(fileName,PROC_WARN);
           pthread_mutex_unlock(&m_proMutex);
           sleep(1);
           if(m_connected[connum])
              m_dbinterface[connum].disconnect();
           m_connected[connum]=0;
           return false;
        }
     }
  }

  // start the transaction 
  //-------------new,m_dbinterface的多线程实现
  m_dbinterface[connum].beginTransaction();  

  // if set a configure executesql 
  // execute the statement
  if(strlen(m_importModes[connum][matched].m_executeSql) > 0) 
  {
     sprintf(sqlcmd,"%s",m_importModes[connum][matched].m_executeSql);

     if(m_dbinterface[connum].executeSql(sqlcmd,nTotal,nSuccess,nError))
     {
        m_dbinterface[connum].rollbackTransaction();
        sprintf(m_msgbuf[connum],"Error(%s:%d):cdbinterface.executeSql(%s) failed",
              __FILE__,__LINE__,sqlcmd);             

        if(pthread_mutex_lock(&m_proMutex) != 0)
        {
           return false;
        }
        else
        {      
           writeAlarm(ERROR_INDB_EXECSQL,m_msgbuf[connum]);
           writeStatus(fileName,PROC_WARN); 
           pthread_mutex_unlock(&m_proMutex);
           sleep(20);
           if(m_connected[connum])
              m_dbinterface[connum].disconnect();
           m_connected[connum]=0;       
           return false;
        }
     }
  }  

  // import file to Table
  //--------------new 需要考虑生成的关联文件名不存在的情况，importFile函数的容错性
  nError = 0;
  if(!importFile(m_importModes[connum][matched],fullFileName,fileName,nTotal,nSuccess,nError,connum))
  {
     m_dbinterface[connum].rollbackTransaction();
   
     sprintf(m_msgbuf[connum],"Error(%s:%d):ProcessProc(%s) failed! ErrorMsg = %s", 
             __FILE__, __LINE__, fullFileName, m_dbinterface[connum].errorMessage()); 

DEBUG_PRINTF("----- %s \n",m_msgbuf[connum]);  

      for(int i = 0; i < strlen(m_msgbuf[connum]); i++)
      {
      	 if((int)m_msgbuf[connum][i] == 10||(int)m_msgbuf[connum][i] == 0||m_msgbuf[connum][i] == '\n') 
      	 {
      	 	  m_msgbuf[connum][i] = '&';
            continue;
      	 }
      }
        
      if(pthread_mutex_lock(&m_proMutex) != 0)
      {
         return false;
      }
      else
      {     
         writeAlarm(ERROR_INDB_PROCFILE,m_msgbuf[connum]);
         writeStatus(fileName,PROC_WARN);
         pthread_mutex_unlock(&m_proMutex);
         sleep(60);   
         return false;
      }
  }

  if(nError > 0)
  {
    sprintf(m_msgbuf[connum],"File %s:Error records number is %d", fullFileName, nError);   

DEBUG_PRINTF("----- %s \n",m_msgbuf[connum]);  
  
    if(pthread_mutex_lock(&m_proMutex) != 0)
    {
       return false;
    }
    else
    {
       writeAlarm(ERROR_INDB_PROCFILE, m_msgbuf[co