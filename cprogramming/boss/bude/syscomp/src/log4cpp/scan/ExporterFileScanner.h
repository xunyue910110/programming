#ifndef _EXPORTERFILESCANNER_H_
#define _EXPORTERFILESCANNER_H_

#include <string>
#include <vector>
#include "config-all.h"

#define DEBUG 0
#define INFO 1
#define WARNING 2
#define MINOR 3
#define MAJOR 4
#define CRITICAL 5  

#define LOG(grade, msg)   _log(false, __FILE__, __LINE__, grade, msg)

#define LOG_P1(grade, msg, p1)  _log(false, __FILE__, __LINE__, grade, msg, p1)
    
#define LOG_P2(grade, msg, p1, p2)  _log(false, __FILE__, __LINE__, grade, msg, p1, p2)

#define LOG_P3(grade, msg, p1, p2, p3)  _log(false, __FILE__, __LINE__, grade, msg, p1, p2, p3)

#define LOG_P4(grade, msg, p1, p2, p3, p4)  _log(false, __FILE__, __LINE__, grade, msg, p1, p2, p3, p4)

#define SLOG(grade, msg)   _log(true, __FILE__, __LINE__, grade, msg)

#define SLOG_P1(grade, msg, p1)  _log(true, __FILE__, __LINE__, grade, msg, p1)
    
#define SLOG_P2(grade, msg, p1, p2)  _log(true, __FILE__, __LINE__, grade, msg, p1, p2)

#define SLOG_P3(grade, msg, p1, p2, p3)  _log(true, __FILE__, __LINE__, grade, msg, p1, p2, p3)

#define SLOG_P4(grade, msg, p1, p2, p3, p4)  _log(true, __FILE__, __LINE__, grade, msg, p1, p2, p3, p4)


USING_NAMESPACE(std);

namespace log4cpp {
	
class LogDir;	

void _log(bool isShort, const char* filename, int line, int grade, const char* msg, ...);

class ExporterFileScanner {
public:
    ExporterFileScanner(const string& host, int port, vector<string>& dirnames, int _batch = 1000);
    virtual ~ExporterFileScanner();
    void scan();

private:
    
    vector<LogDir*> m_logdirs;
};
};

#endif //_EXPORTERFILESCANNER_H_
