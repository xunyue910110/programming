#ifndef _SCRIPTS_H_
#define _SCRIPTS_H_

#include "jshlp/JavaScript.h"

#include <string>
#include <map>

using namespace std;

class NAMEJScript : public JavaScript  {
    JSObject* jsObj;
    bool m_executed;
public:
    static bool Initialize(string name[]);
    static JSContainer *GetContainer();
    static JSClass* jsclass();

    NAMEJScript();
    NAMEJScript(const char* script);

    // 评估脚本的值.主要用于逻辑表达式
    bool matches();

    // 执行一段脚本.用于执行一段匿名脚本
    int execute(string& result);

    // 调用脚本中某一段函数
    int callFunction(const char* name);

    virtual bool good();
};

#endif //_SCRIPTS_
