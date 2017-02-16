#ifndef INDBCONSTDEF_H
#define INDBCONSTDEF_H


//错误代码常量定义
const int EINDBBASE				= 1800;
const int ERROR_INDB_DBCONN		= EINDBBASE + 1;   // 建立数据库连接失败
const int ERROR_INDB_EXECSQL	= EINDBBASE + 2;   // SQL执行错误
const int ERROR_INDB_PROCFILE	= EINDBBASE + 3;   // 处理文件错误
const int ERROR_INDB_CALLPROC	= EINDBBASE + 4;   // 调用存储过程失败
const int ERROR_INDB_MOVEFILE	= EINDBBASE + 5;   // 移动文件失败
const int ERROR_INDB_DELFILE	= EINDBBASE + 6;   // 删除文件失败


////////////////////////////////////////////////////////////////////
#define MAXNAMELEN      80
#define MAXFILELEN      100                        
#define MAXBUFFLEN      500
#define MAXLONGBUFFLEN  5000
#define MAXTHREADCONNUM   10             //多线程入库最大线程数
#define THREADSTACKSIZE   10*1024*1024   //子线程栈大小
#define MAXPOLLTIME    6                 //子线程轮询查找关联文件次数


///////////////////////////////////////////////////////////////////
inline void DEBUG_PRINTF(const char *fmt,...)
{
  #ifdef DEBUG
  	va_list ap;
  	va_start(ap,fmt);
  	vprintf(fmt,ap);
  	va_end(ap);
  #endif
}

#endif // INDBCONSTDEF_H
