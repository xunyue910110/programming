/* Copyright (c) 2003,Linkage All rights reserved.*/

#ifndef GLOBALOBJ_H_INCLUDED
#define GLOBALOBJ_H_INCLUDED

class Application;
class ErrorMessages;

namespace log4cpp {
class Logger;
}

extern Application *const theApp;
extern ErrorMessages *const theErrorMessages;
extern log4cpp::Logger *theLogger; 
extern string theLoggerProcessID;

#endif /* GLOBALOBJ_H_INCLUDED */
