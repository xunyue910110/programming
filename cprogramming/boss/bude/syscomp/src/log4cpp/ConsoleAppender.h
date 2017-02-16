#ifndef _CONSOLEAPPENDER_H_
#define _CONSOLEAPPENDER_H_
#include <fstream>
#include "log4cpp/Appender.h"

namespace log4cpp {
    class ConsoleAppender : public Appender {
    public:
        virtual void init(const map < string, string > & configuration);
        virtual void write(const char * data, size_t size);
        virtual ~ConsoleAppender() throw();
    };
};
#endif //_CONSOLEAPPENDER_H_
