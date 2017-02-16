#include "IpcLocal.h"

#include <stdio.h>
#include <stdarg.h>

#include <iostream>

// using namespace std;
using std::cout;
using std::endl;
	

namespace budeipc {
	
	//!
	inline const char* bugToStr(IPC_DLEV bug) {
		static const char* __bug__str[] = {
			"FATAL",
			"ERROR",
			"DEBUG",
			"TRACE",
			"INFO"
		};
		
		return __bug__str[bug];
	}
	
	static void hostTime2Asc(char sTime[])
	{
		time_t tTime;
		struct tm *pTm;
		
		time(&tTime);
		pTm = localtime(&tTime);
		
		sprintf(sTime, "%02d:%02d:%02d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	} 
	
	void debug2Console(IPC_DLEV level, const char* file, int line, const char* msg, ...) {
		if (level<=IPC_DEBUG_LEVEL) {
			va_list ap;
			char sMsg[2048]="";
			char sTime[10]="";
			
			va_start(ap, msg);
			vsprintf(sMsg, msg, ap);
			va_end(ap);
			
			hostTime2Asc(sTime);
			cout << sTime << " " << bugToStr(level) << " [IPC] " << sMsg << "  " << file << ":" << line << endl;
		}
	}
	
	void debug2File(IPC_DLEV level, const char* file, int line, const char* fmt, ...) {
		if (level<=IPC_DEBUG_LEVEL) {
			FILE* fp;
			va_list ap;
			char sMsg[2048]="";
			char sTime[10]="";
	
			fp=fopen("cpar_log.log", "a+");
			
			va_start(ap, fmt);
			vsprintf(sMsg, fmt, ap);
			va_end(ap);
			
			hostTime2Asc(sTime);
			fprintf(fp, "%s %s [IPC] %s %s:%d \n", sTime, bugToStr(level), sMsg, file, line);
			fflush(fp);
			fclose(fp);
		}
	}

} // namespace budeipc
