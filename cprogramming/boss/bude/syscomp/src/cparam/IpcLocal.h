#ifndef __IPC_LOCAL_H_
#define __IPC_LOCAL_H_

#include <string>
#include <map>
#include <fstream>

#include "budeipc.h"

using std::string;
using std::map;
using std::ifstream;

/**
 * IPC异常抛出宏定义
 */
/*
#define IPCTHROW(expclass, msg) \
		throw expclass(__FILE__, __LINE__, -1, msg)
		
#define IPCTHROW_C(expclass, code, msg) \
		string errmsg(msg); \
		errmsg += "("; \
		errmsg += strerror(errno); \
		errmsg += ")"; \
		throw  expclass(__FILE__, __LINE__, code, errmsg);
*/

#define IPCTHROW(expclass, msg)	THROW(expclass, msg)
#define IPCTHROW_C(expclass, code, msg) THROW_C(expclass, code, msg)

/**
 *  IPC调试信息输出宏定义（简化代码编写）
 */
#ifdef OFF_IPC_DEBUG
#define IPC_DEBUG(msg)
#define IPC_DEBUG_P1(msg, p1)
#define IPC_DEBUG_P2(msg, p1, p2)
#else
#define IPC_DEBUG(msg)	debug2File(IPC_DEBUG, __FILE__, __LINE__, msg);
#define IPC_DEBUG_P1(msg, p1) debug2File(IPC_DEBUG, __FILE__, __LINE__, msg, p1);
#define IPC_DEBUG_P2(msg, p1, p2) debug2File(IPC_DEBUG, __FILE__, __LINE__, msg, p1, p2);
#endif

#ifdef OFF_IPC_TRACE
#define IPC_TRACE(msg) 
#define IPC_TRACE_P1(msg, p1)
#define IPC_TRACE_P2(msg, p1, p2)
#define IPC_INFO(msg)
#else
#define IPC_TRACE(msg) debug2File(IPC_TRACE, __FILE__, __LINE__, msg);
#define IPC_TRACE_P1(msg, p1) debug2File(IPC_TRACE, __FILE__, __LINE__, msg, p1);
#define IPC_TRACE_P2(msg, p1, p2) debug2File(IPC_TRACE, __FILE__, __LINE__, msg, p1, p2);
#define IPC_INFO(msg)  debug2File(IPC_INFO, __FILE__, __LINE__, msg);
#endif


/**
 * namespace budeipc
 */
namespace budeipc {

	void debug2Console(IPC_DLEV level, const char* file, int line, const char* msg, ...);
	
	void debug2File(IPC_DLEV level,const char* file, int line, const char* fmt, ...);

	/**
	 * read from ini-cfg file
	 */
	class IPCINI {
	public:
		//
	    static string strValue(const string& name) 
	    {
	    	string sval = getconf().conf[name];
	    	return sval;
	    }
	    
	    //
	    static int intValue(const string& name) 
	    {
	    	string sval = getconf().conf[name];
	    	if (!sval.empty())
	    		return atol(sval.c_str());
	    	else
	    		return 0;
	    }
			
	private:
		struct _cparamconf {
			map<string, string> conf;
			
			_cparamconf() {
				const char * cfgfile = getenv("CPARAM_CFG");
				ifstream in(cfgfile);
				
				if (!in.good()) {
					THROW_C(budeipc::IpcException, 9999, "未定义初始化文件, 请检查环境变量是否定义($CPARAM_CFG)");
				}
				
				conf.clear();
				
				while (true)
				{
					char line[256] = {0};
	
	                in.getline(line, sizeof(line) - 1);
	                if (in.fail()) break;
	
	                int len = strlen(line);
	                if (len == 0) continue;
	                
	                if (line[0] == '#') continue;
	
	                //兼容windows格式
	                if (line[len - 1] == '\r') line[len - 1] = '\0';
	
					///
					string sl(line);
					size_t ipos = sl.find('=');
					if (ipos != string::npos)
					{
						conf[sl.substr(0, ipos)] = sl.substr(ipos+1);
					}	
					
					if (in.eof()) break;
				}									
			}
		}; // struct _cparamconf
	
		///
	    static _cparamconf& getconf() {
	    	static _cparamconf conf;
	    	return conf;
	    }
	
	};
} // namespace budeipc


#endif

