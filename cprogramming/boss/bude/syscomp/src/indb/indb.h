
#ifndef INDB_H_INCLUDED
#define INDB_H_INCLUDED
#include "appfrm/Application.h"
#include "base/SimpleConfigure.h"
#include "base/StringUtil.h"

#include "dbi/dbi.h"

#include "sys/param.h"
#include "indbError.h"

// const define
#define MAXNAMELEN  80
#define MAXFILELEN  100

#define MAXBUFFLEN  200
#define MAXLONGBUFFLEN 3000


class ImportMode {
	public:
	   vector<string> m_filePatterns;
       char		m_database[MAXNAMELEN+1];	   
       char     m_executeSql[MAXNAMELEN+1];
       char		m_tableName[MAXNAMELEN+1];
	   bool     m_execProc;
	   bool     m_truncateTable;
	   
	   
	   char     m_procedure[MAXBUFFLEN+1];
	   char     m_tableField[MAXLONGBUFFLEN+1];
	   char 	m_fieldSeperator;
  	   int 		m_fieldLength;
       int      m_arrayLine;
       char 	m_dateField[MAXLONGBUFFLEN+1];


	   bool 	m_hasHead;
	   char     m_headFlag[MAXBUFFLEN+1];

	   bool 	m_hasTail;
	   char     m_tailFlag[MAXBUFFLEN+1];
	
	
       int 		m_fieldNum;
 	   int      m_dateFieldNum;

       char     m_fieldName[MAXBIND][50];
       char     m_dateFieldName[MAXBIND][50];
       char     m_insertStmt[MAXLONGBUFFLEN+1];
       
       char     m_fields[MAXLONGBUFFLEN+1];
       char     m_values[MAXLONGBUFFLEN+1];
 
       ImportMode() {
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
 
	   bool isMatchMode(const char *fileName) {
	        string filePattern="*";
	        for(int i=0; i<m_filePatterns.size(); i++) {
	           filePattern = m_filePatterns.at(i);
	            
	           if (StringUtil::matches(fileName,filePattern.c_str())) {
	               return true;
	           }
	        }	
	   	 return false;
	   }		   		   
};


class IndbApp : public Application {
 public:

 protected:
  virtual bool processopt(int optopt, const char *optarg);
  virtual bool initialization();
  virtual bool recovery();
  virtual bool loopProcess();
  virtual bool end();
 
 private:
  int reconnect();
  int writeLogMsg(const char *msg);
  int writeAlarm(const int ecode,const char *msg);
  int writeStatus(const char *msg,const char stat);
  bool getConfig();

  bool process(const char *fileName);
  bool importFile(const ImportMode& importMode,const char *fullFilename, 
       const char *fileName,int& total,int& success, int& error);

  int  parseProcString(char * procNameStr, char * procName, 
        char Val[10][40],char Sym[10][10],int& Sum);

  string 	m_configFile;
  char   	m_incompleteFlag[20];

  char 		m_userName[MAXNAMELEN+1];
  char 		m_passwd[MAXNAMELEN+1];
  char 		m_server[MAXNAMELEN+1];

  char   	m_sourcePath[MAXPATHLEN+1];
  char 		m_failedPath[MAXPATHLEN+1];
  char 		m_errorPath[MAXPATHLEN+1];

  vector<string> m_backuppathVector;
  vector<ImportMode>  m_importModes;

};

#endif /* INDB_H_INCLUDED */
