#ifndef __SPIDERMONKEYHELPER_H
#define __SPIDERMONKEYHELPER_H
/*======================================================================
FileName :  SpiderMonkeyHelper.h     
Copyright:  (C) 2009 by Linkage. All Rights Reserved. 
Owner    :                    
Describe :  SpiderMonkey 辅助类                                                   
Version  :  1.0                 

remark:            
	1.0  2009-7-22 20:15:13  songzj    create                      

===================================================================*/
#include <string>
#include <map>

using namespace std;

struct JSObject;
struct JSContext;
struct JSScript;
struct JSClass;
///////////////SpiderMonkeyContext 运行上下文/////////////////
const size_t PAGE_UNIT = sizeof(void*);
class SpiderMonkeyContext 
{
public:	
	SpiderMonkeyContext(size_t size = 8192L);

	virtual ~SpiderMonkeyContext();

	JSContext *getContext();

	JSObject  *getGlobal();	

	virtual bool good();

	void setGlobalObject(const string& name, JSClass* jsclass, void *address);

	void clearGlobalObjects();

	string getErrorInfo();

private:
	JSContext * m_Context;
	
	JSObject  * m_Global;
	
	map<string, JSObject*> m_GlobalObjects;

	bool m_Good;
	
};

///////////////SpiderMonkeyHelper 辅助类/////////////////
class SpiderMonkeyHelper 
{
public:	
    SpiderMonkeyHelper(SpiderMonkeyContext* ctx);

	virtual ~SpiderMonkeyHelper();
	
	JSScript * compileScript(const char* script);
	
	void destroyScript();

	JSScript * compileFile(const char* name);

	string execute();
	
	void callFunction(const char* name);


	JSScript* getScript();
	
	SpiderMonkeyContext* getSpiderMonkeyContext();
	
	const string& getText() const;
	
	virtual bool good();
	
	string getErrorInfo();

protected:


	JSScript * m_JSScript;

	SpiderMonkeyContext * m_SpiderMonkeyContext;

	string m_Text;

	bool m_executed;
};

/**
 * 向JS中设置一个错误, 返回JS_FALSE
 */ 
extern bool SpiderjsThrowError(JSContext* cx, JSObject* obj, const char* msg);

#endif //__SPIDERMONKEYHELPER_H
