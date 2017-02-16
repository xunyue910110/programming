// ############################################
// Source file : DataBase.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#ifndef DATABASE_H_HEADER_INCLUDED_B7E061C8
#define DATABASE_H_HEADER_INCLUDED_B7E061C8

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <unistd.h>
#include <time.h>

#include "base/config-all.h"
#include "MdbConstDef.h"
#include "PublicInfo.h" //add by chenzm for hp-ux

//##ModelId=481027F403EF
class MemManager;
class DoubleLock;
class Table;
class MyString;

class DataBase
{
  public:
    //##ModelId=481027F403F0
    DataBase(const char *dbName);
    DataBase();

    //##ModelId=481027F403F2
    virtual ~DataBase();

    //##ModelId=481027F403F3
    // 创建数据库
    bool create();

    //##ModelId=481027F403F4
    // 删除数据库
    bool drop();

    //##ModelId=481027F403F5
    // 启动数据库
    bool start();

    //##ModelId=481027F403F6
    // 停止数据库
    bool stop();

    //##ModelId=481027F403F7
    // 获取一个DB的一个会话
    virtual Session* connect();

    //##ModelId=481027F403F8
    // 释放一个会话
    virtual bool disconnect(Session *&session);

    //##ModelId=481027F403FA
    // 备份数据库
    bool backup();

    //##ModelId=481027F403FB
    // 创建表
    bool createTable(const TableDef& tableDef);

    //##ModelId=481027F403FD
    // 删除表
    bool dropTable(const char *tableName);

    //##ModelId=481027F403FF
    // 通过表名查找表
    Table* findTableByName(const char *tableName);
    
    MemManager* getMemMgrInstance();

  protected:
    //##ModelId=481027F40401
    // 数据库名
    T_NAMEDEF m_dbName;

    //##ModelId=481027F40402
    // 信号量
    DoubleLock *m_dbLock;

    //##ModelId=481027F40403
    // 内存管理
    MemManager *m_memManager;

    //##ModelId=481027F40404
    // 表描述
    //map<string, TableDesc *> m_tableDescs;

    //##ModelId=481027F40405
    // 表
    typedef map<MyString, Table *> TABLES_POOL;
    typedef TABLES_POOL::iterator TABLES_POOL_ITR;
    TABLES_POOL m_tables;

};



#endif /* DATABASE_H_HEADER_INCLUDED_B7E061C8 */
