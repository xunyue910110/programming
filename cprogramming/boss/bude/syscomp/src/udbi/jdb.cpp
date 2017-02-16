/** \file jdb.cpp
 *  Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：jdb.h
 *  文件标识：
 *  摘    要：
 * 
 *  当前版本：1.0
 *  作    者：WangFJ
 *  完成日期：2004年12月06日
 *
 *  取代版本： 
 *  原作者  ：
 *  完成日期：
 */        

#include "jdb.h"
#include "udbi-inter.h"
#include "JConnect.h"
#include "TConnect.h"

using namespace UDBI;

/**
 *  \class UDBI::jdb  jdb.h
 *  \brief 数据库访问组件环境初始化类，静态类
 */

extern "C" {
	typedef dvoid *(*maloc_fp)(dvoid *, size_t);	
	typedef dvoid *(*raloc_fp)(dvoid *, dvoid *, size_t);
	typedef void (*mfree_fp)(dvoid *, dvoid *);
}

int jdb::sm_InitMode = INIT_NO;
OCIEnv* jdb::sm_envhp = (OCIEnv*)0;
JConnect* jdb::sm_pcon = 0;
map<string, JConnect*> jdb::sm_connpool;


int jdb::getOciDefMode()
{
	return OCI_DEFAULT;
}

/**
 *  \fn void jdb::jinit(const string &tns, const string &user, const string &password, ub4 initMode = OCI_DEFAULT)
 *  带创建连接实例的OCI编程环境初始化，用于非XA方式、需要单件连接实例的应用;
 *  初始化的同时也创建一个单件本地连接实例，该连接实例通过\a getConnect()  静态方法获得
 *  @param tns 数据库连接别名
 *  @param user 数据库登录用户名
 *  @param password 数据库登录口令
 */
void jdb::jinit(const string& tns, const string& user, const string& password, int initMode) 
{
	//! 屏蔽JSQL内部DEBUG 信息用
	//g_jsqlBug = JSQL_BUG_OPEN;
	UDBI::setJSqlBug(JSQL_BUG_OPEN);

    (void)OCIInitialize((ub4)initMode, (dvoid *)0, (maloc_fp)0, (raloc_fp)0, (mfree_fp)0 );
    (void)OCIEnvInit( (OCIEnv **)&jdb::sm_envhp, OCI_DEFAULT, (size_t)0, (dvoid **)0 );
	sm_InitMode = INIT_LOCAL_DEFAULT;	
	
	sm_pcon = new TConnect(sm_envhp, tns, user, password);  
}

/**
 *  \fn void jdb::jinitXa()
 *  带创建连接实例的OCI编程环境初始化，用于XA方式、需要单件连接实例的应用
 *  初始化的同时也创建一个单件XA连接实例，该连接实例通过\a getConnect()  静态方法获得
 */
void jdb::jinitXa() 
{
	//! 屏蔽JSQL内部DEBUG 信息用
	//g_jsqlBug = JSQL_BUG_OPEN;
	UDBI::setJSqlBug(JSQL_BUG_OPEN);

    sm_envhp = xaoEnv(NULL);
	sm_InitMode = INIT_XA_DEFAULT;

	sm_pcon = new TConnect(sm_envhp, CONNECTED_XA);
}

/**
 *  \fn void jdb::jinitOCCI(OCIEnv *vpEnv, OCISvcCtx *vpSvc)
 *  用于兼容OCCI应用的静态方法，应用初始化OCI编程环境并用OCCI编程接口创建连接，
 *  获取OCCI的OCIEnv * 与 OCISvcCtx *后调用此静态方法即使得基于UDBI的应用共享
 *  OCCI创建的连接和事务控制
 */
void jdb::jinitOCCI(OCIEnv* vpEnv, OCISvcCtx* vpSvc)
{
	//! 屏蔽JSQL内部DEBUG 信息用
	//g_jsqlBug = JSQL_BUG_OPEN;
	UDBI::setJSqlBug(JSQL_BUG_OPEN);

	sm_envhp = vpEnv;
	sm_InitMode = INIT_OCCI_DEFAULT;
	
	sm_pcon = new TConnect(sm_envhp, vpSvc);
}

void jdb::jdeinit()
{
	if (sm_pcon != 0) 
	{
		delete sm_pcon;
		sm_pcon = 0;	
	}
	
	if (sm_InitMode == INIT_LOCAL_DEFAULT) 
	{
		OCIHandleFree((dvoid *)sm_envhp, OCI_HTYPE_ENV);
	}
	
	sm_InitMode = INIT_NO;
}

/**
 *  \fn JConnect *jdb::getConnect()
 *  获取JConnect 的单件实例，要求之前必须已经调用jdb::jinit()或jdb::jinitXa()初始化
 *  如果是有jdb::jinit初始化，则获取的是本地连接；如果是jdb::jinitXa初始化，则获取的是一个XA连接
 */
JConnect* jdb::getConnect() 
{
	if (sm_pcon == 0) {
		THROW_C(EUdbiException, 90000, "jdb::getConnect(): 未建立数据库连接！");
	}
	
	return sm_pcon;
}
		
/**
 *  \fn void jdb::init(ub4 initMode = OCI_DEFAULT)
 *  OCI编程环境初始化, 用于非XA方式
 *  @param initMode 初始化模式，可选值有：OCI_DEFAULT
 *  @return void
 */
void jdb::init(int initMode) 
{
    (void)OCIInitialize((ub4)initMode, (dvoid *)0, (maloc_fp)0, (raloc_fp)0, (mfree_fp)0 );
    (void)OCIEnvInit( (OCIEnv **)&sm_envhp, OCI_DEFAULT, (size_t)0, (dvoid **)0 );
    
    sm_InitMode = initMode;
}

/**
 *  \fn void jdb::initXa() 
 *  OCI编程环境初始化方法, 用于XA方式下
 *  @param initMode 初始化模式，此处无意义
 *  @return void
 */
void jdb::initXa() 
{
    sm_envhp = xaoEnv(NULL);
    
    sm_InitMode = INIT_XA_DEFAULT;
}

/**
 *  \fn string jdb::createConnect(const string& tns, const string& user, const string& password)
 *  多连接支持方法：创建新的连接，并把连接放入连接池
 *  返回连接名字，该名字可以用于下面的连接访问或操作，如删除、设置当前连接等
 */
string jdb::createConnect(const string& tns, const string& user, const string& password)
{
	string tmpStr(user);
	tmpStr += "@";
	tmpStr += tns;
	
	sm_connpool[tmpStr] = new TConnect(sm_envhp, tns, user, password);
	return tmpStr;
}

/**
 *  \fn void jdb::deleteConnect(const string& cnName)
 *  多连接支持方法：删除指定名字的连接（断开连接并从连接池中清除）
 */
void jdb::deleteConnect(const string& cnName)
{
	JConnect* lc = sm_connpool[cnName];
	sm_connpool.erase(cnName);
	delete lc;
}

/**
 *  \fn void jdb::setConnect(const string& cnName)
 *  多连接支持方法：设置当前连接，输入连接名
 *  本方法执行后，当前连接被指向输入名字匹配的连接上，getConnect()方法即可返回指定的连接
 */
void jdb::setConnect(const string& cnName)
{
	sm_pcon = sm_connpool[cnName];
}

/**
 *  \fn void jdb::clearConnect()
 *  多连接支持方法：清除连接池
 */
void jdb::clearConnect()
{
	map<string, JConnect*>::iterator iter;
	JConnect* lc;
	for (iter=sm_connpool.begin(); iter!=sm_connpool.end(); iter++)
	{
		lc = (*iter).second;
		delete lc;
	}
	sm_connpool.clear();
}

/**
 *  \fn JConnect *jdb::Connect(const string &tns, const string &user, const string &password)
 *  创建JConnect 连接实例，并使用 \a tns, \a user, \a password  参数建立本地数据库连接，用于非XA方式
 *  @param tns 数据库连接别名
 *  @param user 数据库登录用户名
 *  @param password 数据库登录口令
 *  @param JConnect 数据库连接引用
 *  @return JConnect *
 *  @see JConnect *ConnectXA()
 */
JConnect* jdb::Connect(const string& tns, const string& user, const string& password) 
{
    return new TConnect(sm_envhp, tns, user, password);
}

/**
 *  \fn JConnect *jdb::ConnectXA()
 *  创建JConnect 实例，并建立实例的XA接口的数据库连接，用于XA方式下
 *  @return JConnect *
 *  @see JConnect *Connect(const string &tns, const string &user, const string &password)
 */
JConnect* jdb::ConnectXA() 
{
    return new TConnect(sm_envhp, CONNECTED_XA);
}

