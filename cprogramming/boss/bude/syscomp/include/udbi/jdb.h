/** Copyright (c) 2004, 联创科技
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

#ifndef __JDB_H_
#define __JDB_H_

#include <string>
#include <map>

class OCIEnv;
class OCISvcCtx;

namespace UDBI {
using std::string;
using std::map;

const int INIT_NO = 0;
const int INIT_LOCAL_DEFAULT = 1;
const int INIT_XA_DEFAULT = 2;
const int INIT_OCCI_DEFAULT = 3;

class JConnect;

// 静态类，主要用于初始化OCI编程环境
class jdb {
private:
    static OCIEnv* sm_envhp;
    static int sm_InitMode;		/**< OCI编程环境初始化模式，缺省为LOCAL方式OCI_DEFAULT模式 */
	static JConnect* sm_pcon;   /**< 连接实例，用于使用单件连接实例的情况 */
	static map<string, JConnect*> sm_connpool;
	
	static int getOciDefMode();

public:
    
    //! 带创建连接实例的OCI编程环境初始化方法, 非XA方式
    static void jinit(const string& tns, const string& user, const string& password, 
    					int initMode = getOciDefMode());
    
    //! 代创建连接实例的OCI编程环境初始化方法，XA方式
	static void jinitXa(); 

	//! 兼容OCCI编程的静态方法
	static void jinitOCCI(OCIEnv* vpEnv, OCISvcCtx* vpSvc);
				
	//! 取初始化模式
	static int getInitMode() { return sm_InitMode; }

	//! 去初始化
	static void jdeinit();
	
	//! 连接是否建立
	static bool isConnected() { return !(sm_pcon==0); }
	
    //! 获取连接实例单件方法
    static JConnect* getConnect();

    //! OCI编程环境初始化方法，非XA方式
    static void init(int initMode = getOciDefMode());

    //! OCI编程环境初始化方法，XA方式
    static void initXa();

	//! 提供多连接支持的一组静态方法
	//! 创建一个连接，把连接放入连接池，返回连接名字（名字需要调用者自己缓存）
   	static string createConnect(const string& tns, const string& user, const string& password);
   	//! 删除一个连接，断开连接、并从连接池删除连接
	static void deleteConnect(const string& cnName);
	//! 切换连接，指定getConnect()方法返回的连接
	static void setConnect(const string& cnName);
	//! 清除连接池
	static void clearConnect();

    //! 创建JConnect实例，并建立实例的数据库连接，返回该实例，非XA方式
    static JConnect* Connect(const string& tns, const string& user, const string& password);
  
    //! 创建JConnect实例，并建立实例的数据库连接，返回该实例，XA方式
    static JConnect* ConnectXA();

};  

} // namespace UDBI



#endif /* __JDB_H_ */
