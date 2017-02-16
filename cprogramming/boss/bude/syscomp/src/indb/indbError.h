#ifndef INDBERROR_H
#define INDBERROR_H

const int EINDBBASE				= 1800;

const int ERROR_INDB_DBCONN		= EINDBBASE + 1;   // 建立数据库连接失败
const int ERROR_INDB_EXECSQL	= EINDBBASE + 2;   // SQL执行错
const int ERROR_INDB_PROCFILE	= EINDBBASE + 3;   // 处理文件错
const int ERROR_INDB_CALLPROC	= EINDBBASE + 4;   // 调用存储过程失败
const int ERROR_INDB_MOVEFILE	= EINDBBASE + 5;   // 移动文件失败
const int ERROR_INDB_DELFILE	= EINDBBASE + 6;   // 删除文件失败


#endif // INDBERROR_H
