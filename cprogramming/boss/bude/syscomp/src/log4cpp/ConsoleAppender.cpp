#include "log4cpp/ConsoleAppender.h"
#include <iostream>
USING_NAMESPACE(std);

using namespace log4cpp;


void ConsoleAppender::init(const map < string, string > & configuration) {
}

void ConsoleAppender::write(const char * data ,size_t size) {
    //cout << data << endl;
    cout.write(data, size);
    cout<<endl;
}

ConsoleAppender::~ConsoleAppender() throw() {
}
