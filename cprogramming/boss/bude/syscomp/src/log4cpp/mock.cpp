#include "Logger.h"

namespace log4cpp {
class LoggerImpl : public Logger {
public:
        virtual ~LoggerImpl() {
        }
        
        bool isDebugEnabled() {
        	return true;
        }
        
        bool isTraceEnabled() {
        	return true;
        }
        
        void trace(const char * file, int line, const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }
    
        void debug(const char * file, int line, const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }

        void state(const NameValueList & states) {
        }
        
        void info(const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }

        void alert(const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }
        
        void alert(Grade::Type grade, int code, const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }

        void alert(const Exception & e) {
	   cout << e << endl;
        }
        
        void alert(Grade::Type grade, int code, const Exception & e, const char * msg,...) {
	   va_list ap; 
   	   va_start(ap, msg); 
	   vfprintf(stdout, msg, ap);
   	   va_end(ap);
        }

        void ok() {
        }

        void ok(int code) {
        }
        
        string pBegin(const char* fields, ...){
        	return string();
        }
        
        void pInput(const string& performId, const string& fullname, size_t count, const char* fields, ...) {
        }
        
        void pBeginInput(const string& performId, const string& fullname, const char* fields, ...) {
        }
                 
        void pEndInput(const string& performId, const string& fullname, size_t count, const char* fields, ...) {
        }

	void pOutput(const string& performId, const string& fullname, size_t count, const char* fields, ...) {
	 }
        
        void pBeginOutput(const string& performId, const string& fullname, const char* fields, ...) {
        }
        
        void pEndOutput(const string& performId, const string& fullname, size_t count, const char* fields, ...){
        }
        
        void pEnd(const string& performId, const char* fields, ...){
        }
        
        void pLinkOut(const string& performId, const string& source, const string& target){
        }
};

        void Logger::configure(const char* configFilename, time_t watchDelay) {        
        }
        
        void Logger::configure(const map<string, string>& properties) {
        }
        
        Logger & Logger::getLogger(const string & dn) {
        	static LoggerImpl logger;
        	return logger;
        }

	Logger::~Logger() {
	}
}
