#ifndef _DAILYFILEAPPENDER_H_
#define _DAILYFILEAPPENDER_H_
#include <fstream>
#include "log4cpp/Appender.h"

namespace log4cpp {
    class DailyFileAppender : public Appender {
    public:
        DailyFileAppender();
        virtual ~DailyFileAppender() throw();
        virtual void init(const map < string, string > & configuration);
        virtual void write(const char * data, size_t size);
    private:
        string m_filename;
        int m_lastday;
        ofstream m_out;        
        void replace(string& source, const char* key, const char* format, int num);
    };
};
#endif //_DAILYFILEAPPENDER_H_
