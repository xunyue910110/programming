/******************************************************************
 *Copyright (c) 2002-2003 南京联创公司联通软件开发中心
 *创建人: 陈忠敏
 *日　期: 2006.11.7
 *描　述:
 *版　本: 1.0
 *****************************************************************/

#ifndef _PRECARDDEAL_H_
#define _PRECARDDEAL_H_



#include "daetools.h"
#include <vector>
#include <occi.h>


using namespace oracle::occi;

extern Environment *env;

extern Connection *conn_1;
/*extern Connection *conn_2;
extern Connection *conn_3;
extern Connection *conn_4;*/

extern Statement *stmt;

typedef vector<string> VFILELIST;

//判断计费文件是否已经生成
bool IsAcctFileExists(VFILELIST &vFileList);
//解析一行数据，得到 业务号码、地市编码、开始通话时间
int ParseSNStarTime(char * src, string &strSN, string &strEparchyCode, string &strStartTime );
//处理一条记录
int DealOneRecord(string &strSN, string &strEparchyCode, string &strStartTime);
//处理一个文件
int DealData(string &AcctFileName);
//获取不同库的oracle连接信息，如 用户/密码/sid
int GetConnectInfo(int ino ,string &user,string &passwd,string &sid);
//根据地市编码获取对应的数据库连接
Connection * GetConnect(string &strEparchyCode);
//连接到oracle 
int ConnectToOracle();
//断开连接
void DisconnectFromOracle();
//获取日志文件名
void GetLogFileName();
int GetConnEparchyCode();
int GetLogSuccessSNFlag();
int GetLogSuccessSNFlag();
int GetSleepInterval();
int GetSysConfig();
int getSubscribeIDAndTradeID(string sEparchyCode,string& strSubscribeID,string& strTradeID);
#endif

