#ifndef OUTDBERROR_H
#define OUTDBERROR_H

const int EOUTDBBASE                =1800;

const int ERROR_OUTDB_DBCONN    = EOUTDBBASE + 1;   // 建立数据库连接失败
const int ERROR_OUTDB_EXECSQL    = EOUTDBBASE + 2;   // SQL执行错
const int ERROR_OUTDB_PROCFILE    = EOUTDBBASE + 3;   // 处理文件错
const int ERROR_OUTDB_CALLPROC    = EOUTDBBASE + 4;   // 调用存储过程失败
const int ERROR_OUTDB_MOVEFILE    = EOUTDBBASE + 5;   // 移动文件失败
const int ERROR_OUTDB_PARAM     = EOUTDBBASE + 6;   // 系统参数错


#endif // OUTDBERROR_H
