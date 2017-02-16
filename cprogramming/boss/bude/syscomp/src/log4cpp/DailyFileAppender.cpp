#include "log4cpp/DailyFileAppender.h"
#include <ctime>
#include <iostream>
#include <cerrno>
using namespace log4cpp;
USING_NAMESPACE(std);



DailyFileAppender::DailyFileAppender() : m_lastday(-1) {
}

void DailyFileAppender::init(const map < string, string > & configuration) {
    map < string, string >::const_iterator it = configuration.find("file");
    if(it != configuration.end()) {
        m_filename = it->second;
    } else {
        cerr << "没有设置日志文件名(file)" << endl; 
    }
}

void DailyFileAppender::replace(string& source, 
    const char* key, const char* format, int num) {       
       size_t pos = string::npos;
        
       size_t idx = source.rfind('/');        
       if(idx == string::npos) {
            pos = source.find(key); 
       } else {            
            pos = source.find(key, idx + 1);
       }
       if(pos != string::npos) { 
            char buffer[5]; 
            sprintf(buffer, format, num); 
            source.replace(pos, strlen(key), buffer); 
       } 
}

void DailyFileAppender::write(const char * data, size_t size) {
    time_t current = time(NULL);
    struct tm* mtm = localtime(&current);
    if(mtm->tm_yday != m_lastday) {
        string todayfile = m_filename;
        replace(todayfile, "yyyy", "%04d", mtm->tm_year+1900);
        replace(todayfile, "YYYY", "%04d", mtm->tm_year+1900);
        replace(todayfile, "yy", "%02d", mtm->tm_year % 100);
        replace(todayfile, "YY", "%02d", mtm->tm_year % 100);
        replace(todayfile, "mm", "%02d", mtm->tm_mon + 1);
        replace(todayfile, "MM", "%02d", mtm->tm_mon + 1);
        replace(todayfile, "dd", "%02d", mtm->tm_mday);
        replace(todayfile, "DD", "%02d", mtm->tm_mday);
       m_lastday = mtm->tm_yday;
       m_out.close();
       m_out.clear();
       
       m_out.open(todayfile.c_str(), ios_base::app);             
    }    
    if(m_out.fail()) {
        cout << "日志文件错误, errno=" << errno <<", msg="<< strerror(errno) << endl; 
    } else {
        //m_out << data << endl;
        m_out.write(data, size);
        m_out<<endl;
    }
}

DailyFileAppender::~DailyFileAppender() throw() {
}

