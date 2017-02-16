#ifndef __JSCRIPTSPARSER_H_
#define __JSCRIPTSPARSER_H_

/*======================================================================
FileName :  JSCriptParser.h     
Copyright:  (C) 2009 by Linkage. All Rights Reserved. 
Owner    :                    
Describe :  JS 解析器包装 ,结合Buffer类                                                   
Version  :  1.0                 
   
remark:            
        1.0  2009-7-22 20:15:13  songzj    create                      
                             
===================================================================*/
#include "SpiderMonkeyHelper.h"
#include "DataBuffer.h"
#include <string>
#include <map>

//////////////////JS 解析器包装 ,结合Buffer类////////////////
class JScriptParser : public SpiderMonkeyHelper 
{
public:
	JScriptParser();
	~JScriptParser(){};

    bool registerObject(const char * objName,DataBuffer * buffer);

    //static JSClass* jsclass();

	void clearObjects();

	static JScriptParser * getInstance();

private:

	void init();

	virtual bool good();

	JSObject * m_JSObj;

   
};

void ExecuteScript(const char * script,DataBuffer & inBuf,DataBuffer &outBuf);
void ExecuteScriptFile(const char * scriptFile,DataBuffer & inBuf,DataBuffer &outBuf);


#endif //__JSCRIPTSPARSER_H_

