// ############################################
// Source file : InstanceFactory.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-5-18
// Update      : 2008-5-18
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef INSTANCEFACTORY_H_HEADER_INCLUDED_B7D34DA8
#define INSTANCEFACTORY_H_HEADER_INCLUDED_B7D34DA8

#include "MdbConstDef.h"

class Session;
class Index;
class RecordConvert;
class DataBase;
class MemManager;
class DoubleLock;
//##ModelId=482CDFE10196
class InstanceFactory
{
  public:
    //##ModelId=482CE0220119
    static Index* getIndexInstance(T_INDEXTYPE indexType 
												,const char *cTableName 
												,const char *cIndexName 
												,MemManager* memMgr 
												,RecordConvert* recordConvert);

    //##ModelId=482CE85A0128
    static DataBase *getDBInstance(T_DATABASETYPE dbType
    								,const char * cDataBaseName
    								,const char *hostName
    								,const int port
    								,const char *cUserName = NULL
    								,const char *cPwd = NULL
    								,const int iConnType=1  // add by chenm 2009-7-21 For altibase 1 : TCP/IP, 2 : UNIXDOMAIN, 3 : IPC
    								);

    //##ModelId=482CE87702AF
    static DoubleLock *getDoubLockInstance(T_SEMAPHORETYPE semaphoreType,const char * cDataBaseName);
    
    static Session *getSessionInstance(T_SESSIONTYPE sessionType,void *point);

};


#endif /* INSTANCEFACTORY_H_HEADER_INCLUDED_B7D34DA8 */
