#ifndef __BUDEIPC_H_
#define __BUDEIPC_H_

#include <string>
#include <iostream>
#include <memory>		// auto_ptr

//! 
#include <sys/types.h>
#include <sys/ipc.h>

using namespace std;

//! DEBUG
enum IPC_DLEV {
	IPC_FATAL=0, 
	IPC_ERROR=1,
	IPC_DEBUG=2,
	IPC_TRACE=3,
	IPC_INFO =4
};

const int IPC_DEBUG_LEVEL=3;

//! EXCEPTION
class EIpcException {
public:
	EIpcException(const string msg):__msg(msg) {}
	const char* what() { return __msg.c_str(); }
	
	EIpcException(const string & file, int line, int code, const string & msg)
	{
		char stmp[1024] = "";
		sprintf(stmp, "[ERROR] <%s:%d> err_code:%d, err_msg:", file.c_str(), line, code);
		__msg = stmp;
		__msg += msg;
	}
		
	friend inline ostream& operator<<(ostream& out, const EIpcException& rhs)
	{
		out << rhs.__msg << endl;
		return out;
	}

private:
	string __msg;

};

#define IPCTHROW(expclass, msg) \
		throw expclass(__FILE__, __LINE__, -1, msg)
		
#define IPCTHROW_C(expclass, code, msg) \
		string errmsg(msg); \
		errmsg += "("; \
		errmsg += strerror(errno); \
		errmsg += ")"; \
		throw  expclass(__FILE__, __LINE__, code, errmsg);


#endif
