#include "log4cpp/StdFormatter.h"
#include <sys/time.h>
#include <sys/types.h> 
#include <unistd.h> 

namespace log4cpp {

inline int compLength(int size, int printfResult) {
  if(printfResult>=size)
    return size -1;
  return printfResult;
}

void StdFormatter::init(const map < string, string > & configuration) {
}

int sprintPrefix(char * buffer, int size, const char * dn, const char * type) {
    int offset = 0;
    struct timeval current;
    
    gettimeofday(& current, NULL);
    struct tm * mtm = localtime(& current.tv_sec);
    
    offset += compLength(size, snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d", mtm->tm_year + 1900, mtm->tm_mon + 1, mtm->tm_mday,
       mtm->tm_hour, mtm->tm_min, mtm->tm_sec, current.tv_usec / 1000));
       
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [%s] %d", type, dn, getpid()));
    
    return offset;
}

int StdFormatter::trace(char * buffer, size_t size, const char * dn, const char * file, int line,
   const char * msg, va_list args) {
       int offset = 0;
       offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "TRACE"));
       offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s:%d ", file, line));
       offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, msg, args));
       return offset;
}

int StdFormatter::debug(char * buffer, size_t size, const char * dn, const char * file, int line,
   const char * msg, va_list args) {
       int offset = 0;
       offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "DEBUG"));
       offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s:%d ", file, line));
       offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, msg, args));
       return offset;
}

int StdFormatter::state(char * buffer, size_t size, const char * dn, const NameValueList & states) {
    int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "STATE"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " "));
    for (size_t i = 0; i < states.size(); i++) {
        if (i != 0) offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ";"));
        offset += compLength(size - offset, snprintf(buffer + offset, size - offset, "%s=%s", states[i].name.c_str(), states[i].value.c_str()));
    }
    return offset;
}

int StdFormatter::info(char * buffer, size_t size, const char * dn, const char * msg, va_list args) {
    int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "INFO"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " "));
    offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, msg, args));
    return offset;
}



int StdFormatter::alert(char * buffer, size_t size, const char * dn, Grade::Type grade, int code, const Exception * e,
   const char * msg, va_list args) {
       int offset = 0;
       if (e == 0) {
           offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, code == -1 ? "MAJOR" : Grade::toString(grade)));
           offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %d ", code));
           offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, msg, args));
       } else {
           offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn,
              code == -1 ? Grade::toString(e->getGrade()) : Grade::toString(grade)));
                
           offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %d ", code == -1 ? e->getCode() : code));
           
           if (msg != 0) 
              offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, msg, args));
           else 
              offset += compLength(size - offset, snprintf(buffer + offset, size - offset, "%s", e->what()));
           
           offset += compLength(size - offset, snprintf(buffer + offset, size - offset, "\n%s", e->toString().c_str()));
       }
       return offset;
}



int StdFormatter::ok(char * buffer, size_t size, const char * dn, int code) {
    int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "OK"));
    if (code != -1) offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %d ", code));
    return offset;
}

int StdFormatter::pBegin(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [BEGIN]", pid));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " "));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}

/**
 * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
 */ 
int StdFormatter::pInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [INPUT] fullname=%s,count=%d", pid, fullname, count));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}
// 下面方法分两次调用, 可以记录消耗的时长        
/**
 * 开始读入
 */ 
int StdFormatter::pBeginInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [BEGIN_INPUT] fullname=%s", pid, fullname));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;	
}
/**
 * 完成读入
 */ 
int StdFormatter::pEndInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [END_INPUT] fullname=%s,count=%d", pid, fullname, count));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}

/**
 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
 */ 
int StdFormatter::pOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args)  {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [OUTPUT] fullname=%s,count=%d", pid, fullname, count));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}
//下面方法分两次调用, 可以记录消耗的时长
/**
 * 开始输出
 */ 
int StdFormatter::pBeginOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [BEGIN_OUTPUT] fullname=%s", pid, fullname));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}
/**
 * 完成输出
 */ 
int StdFormatter::pEndOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [END_OUTPUT] fullname=%s,count=%d", pid, fullname, count));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, ","));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;

}
/**
 * 链接
 */ 
int StdFormatter::pLinkOut(char * buffer, size_t size, const char * dn, const char* pid, const char* source, const char* target) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [LINKOUT] source=%s,target=%s", pid, source, target));
    return offset;
}

/**
 * 完成一次执行
 */ 
int StdFormatter::pEnd(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) {
	int offset = 0;
    offset += compLength(size - offset, sprintPrefix(buffer + offset, size - offset, dn, "PROC"));
    offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " %s [END]", pid));
    if(fields != 0) {
    	offset += compLength(size - offset, snprintf(buffer + offset, size - offset, " "));
    	offset += compLength(size - offset, vsnprintf(buffer + offset, size - offset, fields, args));
    }
    return offset;
}



StdFormatter::StdFormatter() {
}

StdFormatter::~StdFormatter() throw() {
}

}
