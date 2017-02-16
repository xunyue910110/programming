#ifndef READPARAM_H
#define READPARAM_H
#include <string>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include "dbi/dbi.h"
#include "Report.h"
#include "basefunc.h"
#include <iostream>

extern char dbServer[64];
extern char dbUser[64];
extern char dbPassword[64];
extern char dbName[64];
extern char reportTableName[64];

typedef struct DayReportParam
{  
  int flag;//## 接收、发送标记： 0 －接收汇总文件 1 －发送汇总文件
  char fileNameHead[8];//## 文件名头标记。如移动出访汇总头为：CHO
  char fileNameDate[9];//## 文件名中的日期形式。 如YYMMDD、MMDD等
  char fileName3[8];//## 移动中可为999、省代码或空值。联通中可为序列号（NNN）
  char fileNameEnd[8];//## 文件的扩展名
  char procedureName[32];//## 汇总过程名
  char reportPath[80];//## 汇总文件路径
  char backupPath[80];//## 汇总备份路径
  int processTime;//## 日汇总的处理时间：HHMI
  int reportNo;//## 用于标识该汇总文件
  int processLimitTime;//##文件处理有效时间,例 120分钟,有效范围 >=0,<60*24
  char lastProcessDate[8+1];//##上次处理日期:YYYYMMDD
}DayReportParameter;

typedef struct OtherReportParam//## 其它汇总参数。包括月汇总、特殊汇总和临时汇总
{  
  int flag;//## 接收、发送标记：0 －接收汇总文件1 －发送汇总文件
  char fileName[40];//## 汇总文件名
  char procedureName[32];//## 汇总过程名
  char reportPath[80];//## 汇总文件路径
  char backupPath[80];//## 汇总文件备份目录
  char processTime[13];//## 汇总处理时间。形为YYYYMMDDHHMI
  int reportNo;//## 汇总序列号
  int processLimitTime;//##文件处理有效时间,例 120分钟,有效范围 >=0,<60*24
  char lastProcessDate[8+1];//##上次处理日期:YYYYMMDD
}OtherReportParameter;


typedef struct ReportTm//##全部汇总时间参数
{
  int reportNo;//## 汇总序列号
  char lastProcessDate[8+1];//##上次处理日期:YYYYMMDD
  long reportDate;//## 汇总处理日期,形为YYYYMMDD
  long processTime;//## 汇总处理时间：HHMI
  int reportFlag;//## 日报、其他标记：1 －日报文件 0 －其他文件
  int receiveFlag;// 接收、发送标记：0 －接收汇总文件1 －发送汇总文件
  int processLimitTime;//##文件处理有效时间,例 120分钟,有效范围 >=0,<60*24
}ReportTime;
  
typedef vector<DayReportParameter> TypdayVector;
typedef vector<OtherReportParameter> TypotherVector;
typedef vector<ReportTime> TyptimeVector;
 
typedef vector<DayReportParameter>::iterator dayit;
typedef vector<OtherReportParameter>::iterator otherit;
typedef vector<ReportTime>::iterator timeit;

class ReadParam
{
 public:
  ReadParam(TypdayVector *,TypotherVector *);
  TypdayVector *m_dayVector;
  TypotherVector *m_otherVector;
  int readParam();
  int connectDb();
  DbInterface dbi;
 private:
  int readDayParam();
  int readOtherParam();
};
#endif
