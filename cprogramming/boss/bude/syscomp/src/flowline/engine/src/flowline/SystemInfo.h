#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_
#include "flowline/flbase.h"

namespace flowline {
struct Host {
	int id;
	string ip;
	string username;
	string password;	
	bool isLocalHost;
};

struct Database {
	Database(const Database& db) 
	: 	id(db.id), name(db.name), 
		host(db.host), username(db.username), password(db.password){}
		
	Database(const Host& host_) : host(host_) {}
	/**
	 * 系统生成的唯一ID
	 */ 
	int id;
	/**
	 * 数据库名
	 */ 
	string name;
	/**
	 * 位于主机的ID
	 */ 
    const Host&  host;	
	string username;
	string password;
};
}
#endif //_SYSTEMINFO_H_
