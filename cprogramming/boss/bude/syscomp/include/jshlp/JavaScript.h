#ifndef _JSUTIL_H_
#define _JSUTIL_H_

//#include "config-all.h"

	// disable annoying template warning under msvc
#pragma warning(disable:4786)
	// disable 'this' pointer used in member initialization message
#pragma warning(disable:4355)
	// disable 'need to have dll interface to be used by clients' message
#pragma warning(disable:4251)

#include <string>
#include <map>
//USING_NAMESPACE(std);

using namespace std;

struct JSObject;
struct JSContext;
struct JSScript;
struct JSClass;

class JSContainer {
	JSContext *context;
	JSObject  *global;	
	int flag;
	std::map<string, JSObject*> globalObjects;

public:	
	JSContainer(size_t size=1024);
	virtual ~JSContainer();
	JSContext *getContext();
	JSObject  *getGlobal();	
	virtual bool good();
	int setGlobalObject(const string& name, JSClass* jsclass, void *address);
	int clearGlobalObjects();
	string getErrorInfo();
};

class JavaScript {
protected:	
	JSScript *jscript;
	JSContainer* container;
	string text;
public:	
    JavaScript(JSContainer* container);
	JavaScript(JSContainer* container, const char* script);
	virtual ~JavaScript();
	
	JSScript * compileScript(const char* script);
	JSScript * compileFile(const char* name);
	
	JSScript* getScript();
	JSContainer* getContainer();
	const string& getText() const;
	virtual bool good();
	string getErrorInfo();
};

/**
 * 向JS中设置一个错误, 返回JS_FALSE
 */ 
extern bool jsThrowError(JSContext* cx, JSObject* obj, const char* msg);

#endif //_JSUTIL_H_
