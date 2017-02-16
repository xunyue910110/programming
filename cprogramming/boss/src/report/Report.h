
#define ROAM_BASE 		1200
#define ROAM_DB 		0
#define ROAM_FILE_ACCESS 	20
#define ROAM_FILE_ERROR 	40
#define ROAM_RECORD_ERROR 	60
#define ROAM_REPORT_ERROR 	80

//DB error
#define DB_CONNECT 	ROAM_BASE+ROAM_DB+1
#define DB_BIND 	ROAM_BASE+ROAM_DB+2
#define DB_EXECSQL 	ROAM_BASE+ROAM_DB+3
#define DB_EXECPROC 	ROAM_BASE+ROAM_DB+4
#define DB_PROCRETURN 	ROAM_BASE+ROAM_DB+5
#define DB_BEGINTRAN 	ROAM_BASE+ROAM_DB+6
#define DB_COMMIT 	ROAM_BASE+ROAM_DB+7
#define DB_ROLLBACK 	ROAM_BASE+ROAM_DB+8
#define DB_ENDTRAN 	ROAM_BASE+ROAM_DB+9
#define DB_NO_RECORD 	ROAM_BASE+ROAM_DB+10


//file access error
#define SET_FILENAME 	ROAM_BASE+ROAM_FILE_ACCESS+1
#define SET_FILEHEAD 	ROAM_BASE+ROAM_FILE_ACCESS+2
#define SET_FILETAIL 	ROAM_BASE+ROAM_FILE_ACCESS+3
#define OPEN_FILE 	ROAM_BASE+ROAM_FILE_ACCESS+4
#define RENAME_FILE 	ROAM_BASE+ROAM_FILE_ACCESS+5
#define UNLINK_FILE 	ROAM_BASE+ROAM_FILE_ACCESS+6
#define OPEN_PATH 	ROAM_BASE+ROAM_FILE_ACCESS+7
#define EMPTY_FILE 	ROAM_BASE+ROAM_FILE_ACCESS+8


//Roam report error
#define MISS_REPORT 	ROAM_BASE+ROAM_REPORT_ERROR+1
#define REPORT_PARAM_ERROR 	ROAM_BASE+ROAM_REPORT_ERROR+2

#define RN 			0X0D
#define LF 			0X0A

//report parameter table name,fields name
#define DAY_PARAM_TABLE		"td_dayreport"
#define DAY_PARAM_FIELD		"flag,filenamehead,filenamedate,\
filename3,filenameend,procedurename,reportpath,backuppath,\
processtime,reportno,processlimittime,lastprocessdate"

#define OTHER_PARAM_TABLE	"td_otherreport"
#define OTHER_PARAM_FIELD	"flag,filename,procedurename,reportpath,\
backuppath,processtime,reportno,processlimittime,lastprocessdate"

