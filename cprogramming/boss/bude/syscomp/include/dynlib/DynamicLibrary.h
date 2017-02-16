#ifndef DYNAMICLIBRARY_H
#define DYNAMICLIBRARY_H
#include "base/config-all.h"
#include "base/exceptions.h"
#include <string>

USING_NAMESPACE(std);



//动态库的命名规则 libxxx-10.1.so
class Library;

DECLARE_EXCEPTION(LibraryException, BaseException);

class DynamicLibraryManager {

public:
    DynamicLibraryManager();
    DynamicLibraryManager(const string & path);
    virtual ~DynamicLibraryManager();
    void * getSymbol(const string & libname, const string & symname);
    void setWorkPath(const string & path);
    static void notify();
private:

    Library & getLibrary(const string & libname);
    
    string workPath;
};

#endif // DYNAMICLIBRARY_H
