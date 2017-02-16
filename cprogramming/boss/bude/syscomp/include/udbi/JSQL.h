/** 
 * @file JSQL.h
 * JSQL静态类，SQL语句缓存
 *
 * @author WangFJ
 * @author &copy; All Rights Reserved By NanJing Linkage Tech. Co., Ltd.
 * @date    2004-2005
 *
 * @sa contact:
 *   http://www.lianchuang.com 
 */

#ifndef __JSQL_H_
#define __JSQL_H_

#include <map>
#include <string>

namespace UDBI {
using std::string;
using std::map;

typedef map<string, string>  JSQL_MAP;

class JSQL {
public:
	//!
	static void getSQL(string& sqlStmt, const string& tabName, const string& sqlRef);

	//!
	static void getSQL(string& sqlStmt, const string& tabName, const string& sqlRef, const string& swhere);
	
	//! 
	static int update(const string &info="");
	
	//!
	static void refresh(const string& tabName, const string& sqlRef)
	{
		string str=tabName;
		str += sqlRef;		
		sql_map.erase(str);	
	}
	
	//!
	static void clear()
	{
		sql_map.clear();
	}

private:

    static JSQL_MAP  sql_map;
    
    static time_t lastUpdateTime;
    
    static string lastUpdateVer;
	
};

} // UDBI

#endif	/* __JSQL_H_  */


