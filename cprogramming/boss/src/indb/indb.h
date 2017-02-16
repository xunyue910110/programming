#ifndef INDB_H_INCLUDED
#define INDB_H_INCLUDED

#include <iostream>
#include </usr/include/pthread.h>
#include "appfrm/Application.h"
#include "base/SimpleConfigure.h"
//#include "uconfig/SimpleConfigure.h"
#include "base/StringUtil.h"
#include "dbi/dbi.h"
#include "sys/param.h"
#include "indbConstDef.h"

using namespace std;

class ImportMode 
{
	public:
	  vector<string> m_filePatterns;
    
    char		m_database[MAXNAMELEN+1];	   
    char    m_executeSql[MAXNAMELEN+1];
    char		m_tableName[MAXNAMELEN+1];
	  bool    m_execProc;
	  bool    m_truncateTable;
	         	         
	  char    m_procedure[MAXBUFFLEN+1];
	  char    m_tableField[MAXLONGBUFFLEN+1];
	  char 	  m_fieldSeperator;
  	int 		m_fieldLength;
    int     m_arrayLine;
    char 	  m_dateField[MAXLONGBUFFLEN+1];
                     
	  bool 	  m_hasHead;
	  char    m_headFlag[MAXBUFFLEN+1];
           
	  bool  	m_hasTail;
	  char    m_tailFlag[MAXBUFFLEN+1];
	         	         
    int 		m_fieldNum;
 	  int     m_dateFieldNum;
            
    char    m_fieldName[MAXBIND][50];
    char    m_dateFieldName[MAXBIND][50];
    char    m_insertStmt[MAXLONGBUFFLEN+1];
            
    char    m_fields[MAXLONGBUFFLEN+1];
    char    m_values[MAXLONGBUFFLEN+1];
 
    ImportMode() 
    {
        m_filePatterns.clear();
      	m_database[0]=0;	 
      	m_executeSql[0]=0;
      	m_tableName[0]=0;
	  	  m_execProc=false;
	  	  m_truncateTable=false;
	  	 	
	  	  m_procedure[0]=0;
	  	  m_tableField[0]=0;
	  	  m_fieldSeperator=',';
  	  	m_fieldLength=0;
      	m_arrayLine=0;
      	m_dateField[0]=0;
      	
	  	  m_hasHead=false;
	  	  m_headFlag[0]=0;
      	 
	  	  m_hasTail=false;
	  	  m_tailFlag[0]=0;
	  
      	m_fieldNum=0;
 	  	  m_dateFieldNum=0;
    }      
 
	  bool isMatchMode(const char *fileName) 
	  {
        string filePattern="*";
	      for(int i=0; i<m_filePatterns.size(); i++) 
	      {
	         filePattern = m_filePatterns.at(i);
	         if (StringUtil::matches(fileName,filePattern.c_str())) 
	         {
	             return true;
	         }
	      }	
	   	 return false;
	  }		   		   
};


class IndbApp : public Application 
{
 public:

 protected:
   virtual bool processopt(int optopt, const char *optarg);
   virtual bool initialization();
   virtual bool recovery();
   virtual bool loopProcess();
   virtual bool end();
   
   typedef struct
   {
     int thrNum;
     IndbApp *m_pSelf;	
   }Thread_indb;
   
 private:	   
   int  writeLogMsg(const char *msg);
   int  writeAlarm(const int ecode,const char *msg);
   int  writeStatus(const char *msg,const char stat);
   
   int  reconnect(int connum); 
   bool getConfig();
   static bool ThreadIndb(const Thread_indb &thr);
   bool process(const char *fileName,int connum);
   
   bool importFile(const ImportMode& importMode,const char *fullFilename, 
                   const char *fileName,int& total,int& success, int& error,
                   int connum);
   
   int  parseProcString(char * procNameStr, char * procName, 
                        char Val[10][40],char Sym[10][10],int& Sum);
   
   string 	m_configFile;
   char   	m_incompleteFlag[20];

	 char 		m_userName[MAXTHREADCONNUM][MAXNAMELEN+1];
   char 		m_passwd[MAXTHREADCONNUM][MAXNAMELEN+1];
   char 		m_server[MAXTHREADCONNUM][MAXNAMELEN+1];
   
   char   	m_sourcePath[MAXTHREADCONNUM][MAXPATHLEN+1];
   char 		m_failedPath[MAXTHREADCONNUM][MAXPATHLEN+1];
   char 		m_errorPath[MAXTHREADCONNUM][MAXPATHLEN+1];
   char 		m_fileType[MAXTHREADCONNUM][MAXPATHLEN+1];
   int      m_formateNum[MAXTHREADCONNUM];
   
   vector<ImportMode>  m_importModes[MAXTHREADCONNUM];
   vector<string>      m_backuppathVector[MAXTHREADCONNUM];
   
   //add for mulitThread_indb by chenxt 20081216  
   int      m_connectNum;                               //入库线程数
   bool     m_abnormal;                                 //入库过程是否发生异常
   pthread_mutex_t m_proMutex;                          //入库全局锁
   bool        m_connected[MAXTHREADCONNUM];
   DbInterface m_dbinterface[MAXTHREADCONNUM];            //入库线程数据库连接对象
   
   char m_msgbuf[MAXTHREADCONNUM][MAXBUFFLEN+1];          
   
   //add for conn_indb by chenxt 20081217
   char     m_connYesNo[MAXTHREADCONNUM];                 //是否启用关联入库功能
   char     m_connFile[MAXTHREADCONNUM][MAXNAMELEN];      //关联文件
   char     m_cmmitYesNo[MAXTHREADCONNUM][2];             //子线程是否已提交事务
   int      m_openFlag;                                 //每一轮入库统一开始开关标志
   int      m_number;                                   //一轮入库已完成处理的线程数目
   int      m_num;                                      //新一轮入库开始前设置统一开始开关标志为
   int      m_count;                                    //已入库的文件数目
   int      m_filetailLen;                              //关联标识长度
	 int      m_filetailBegin;                            //关联标识在文件名中的倒数位置
};

#endif /* INDB_H_INCLUDED */


