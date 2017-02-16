#ifndef __BUDEIPC_H_
#define __BUDEIPC_H_

/*#include <string>
#include <iostream>
#include <exception>

//using namespace std;
using std::string;
using std::ostream;
using std::cout;
using std::endl;
using std::exception;
*/

#include "base/exceptions.h"

/**
 * namespace budeipc
 */
namespace budeipc {
	
	//! DEBUG
	enum IPC_DLEV {
		IPC_FATAL=0, 
		IPC_ERROR=1,
		IPC_DEBUG=2,
		IPC_TRACE=3,
		IPC_INFO =4
	};
	
	const int IPC_DEBUG_LEVEL = IPC_TRACE;
	
	/**
	 *  class IpcException definition.
	 */
	/*class IpcException : public exception {
		
	public:
		
		IpcException(const string msg):__msg(msg) {}
		
		IpcException(const string & file, int line, int code, const string & msg)
		{
			char stmp[1024] = "";
			__errcode = code;
			sprintf(stmp, "[ERROR] <%s:%d> err_code:%d, err_msg:", file.c_str(), line, code);
			__msg = stmp;
			__msg += msg;
		}
			
		const char* what() { return __msg.c_str(); }
		
		int errnum() { return __errcode; }

		friend inline ostream& operator<<(ostream& out, const IpcException& rhs)
		{
			out << rhs.__msg << endl;
			return out;
		}
	
	private:
		
		string __msg;
		int __errcode;
	
	};
	*/
	
	DECLARE_EXCEPTION(IpcException, PrimalException);

} // namespace budeipc

#endif
