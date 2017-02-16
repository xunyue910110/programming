
#include "NAMEJScript.h"

#include "js16/jsapi.h"
#include "base/TimeUtil.h"
#include <cstdlib>
#include <cassert>
#include <string.h>
#include <iostream>


static JSBool
NAME_getProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{

    JSString *idstr;
    const char *name;

    int index;

    if (!JSVAL_IS_INT(id)) {
       idstr = JS_ValueToString(cx, id);
       if (!idstr )
           return JS_FALSE;
       name = JS_GetStringBytes(idstr);
       index = atoi(name + 1);
    } else {
       index = JSVAL_TO_INT(id);
    }

	string *pname;
    pname=static_cast<string *>(JS_GetPrivate(cx,obj));
    if (!pname)
      return jsThrowError(cx,obj,"类型不正确");

    if (index < 0 || index >= 400)
      return jsThrowError(cx, obj, "属性不存在");

    JSString * jss = JS_NewStringCopyZ(cx, (pname)[index].c_str());
    if(jss) {
        *vp = STRING_TO_JSVAL(jss);
        return JS_TRUE;
    }
    return jsThrowError(cx, obj, "JSVM不能分配一个字符串");
}



static JSBool
NAME_setProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
    JSString *idstr, *valstr;
    const char *name, *value;
    int index;

    if (!JSVAL_IS_INT(id)) {
       idstr = JS_ValueToString(cx, id);
       if (!idstr )
           return JS_FALSE;
       name = JS_GetStringBytes(idstr);
       index = atoi(name + 1);
    } else {
       index = JSVAL_TO_INT(id);
    }

    valstr = JS_ValueToString(cx, *vp);
    if ( !valstr)
        return JS_FALSE;
    value = JS_GetStringBytes(valstr);


	string *pname;
    pname=static_cast<string *>(JS_GetPrivate(cx,obj));
    if (!pname)
      return jsThrowError(cx,obj,"类型不正确");

    if (index < 0 || index >= 400)
      return jsThrowError(cx, obj, "属性不存在");

    (pname)[index] = value;
    return JS_TRUE;
}


static JSClass NAME_class = {
    "NAME", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,JS_PropertyStub,
    NAME_getProperty,
    NAME_setProperty,
    JS_EnumerateStub, JS_ResolveStub,JS_ConvertStub,
    JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSFunctionSpec NAME_methods[] = {
    {0,0,0,0,0}
};


JSPropertySpec NAME_properties[] = {
    { "", 0, JSPROP_INDEX },
    { 0 }
};


JSClass* NAMEJScript::jsclass() {
    return &NAME_class;
}



NAMEJScript::NAMEJScript()
            :JavaScript(GetContainer()) , jsObj(0) {
  if(JavaScript::good())
       jsObj = JS_InitClass(getContainer()->getContext(),
               getContainer()->getGlobal(), NULL, &NAME_class,
               NULL, 0, NAME_properties, NAME_methods, NULL, NULL);
}



NAMEJScript::NAMEJScript(const char* script)
                   :JavaScript(GetContainer(), script) , jsObj(0) {
  if(JavaScript::good())
       jsObj = JS_InitClass(getContainer()->getContext(),
               getContainer()->getGlobal(), NULL, &NAME_class,
               NULL, 0, NULL, NAME_methods, NULL, NULL);
}


int NAMEJScript::callFunction(const char* name) {
    if(!good())
      return -1;   //JavaScript不可用

    JSContext* context =  getContainer()->getContext();
    JSObject* global =   getContainer()->getGlobal();
    jsval rval;
    string result;
    int rt;
    if (!m_executed) { // 第一次装入并调用以保证声明的函数有效
       rt = execute(result);
       if (rt !=0)
         return rt;
    }

    if(!JS_CallFunctionName(context,global,name,0,0,&rval))
        return -2; //执行脚步失败

    return 0;
}

bool NAMEJScript::matches() {
   string isOK("true");
     string result;
   if (execute(result)>=0) {
        return (result == isOK) ;
   }
   return false;
}


int NAMEJScript::execute(string &result) {
    if(!good())
      return -1;   //JavaScript不可用

   JSContext* context =  getContainer()->getContext();
   JSObject* global =   getContainer()->getGlobal();
   jsval rval;

   m_executed = true;
   if(!JS_ExecuteScript(context, global, jscript, &rval))
        return -2; //执行脚步失败

   JSString* jss = JS_ValueToString(context, rval);
   if(jss == 0)
       return -3;

   result = JS_GetStringBytes(jss);

   return 0;
}

bool NAMEJScript::good() {
    return jsObj != 0;
}

JSContainer *NAMEJScript::GetContainer() {
    static JSContainer container;
    return &container;
}

bool NAMEJScript::Initialize(string  name[]) {
    if (name == 0)
        return false;
    GetContainer()->setGlobalObject("name",NAMEJScript::jsclass(),name);
    return true;
}


// int main() {
//     string result;
//     string m_name[400];
//     m_name[0] = "sunhk1";
//     m_name[2] = "sunhk1";
//   char ctime[17+1];
//   TimeUtil::now(ctime);
//   m_name[3]=ctime;
//
//     NAMEJScript rjs(" name[2]='rr';name[2]=name[0]+name[3].substr(4,6); ");
//
//     NAMEJScript::Initialize(m_name);
//     cout<<"ctime---"<<ctime<<endl;
//     cout << "script:" << rjs.getText() << endl;
//     cout << rjs.execute(result) << result << endl;
//     cout << rjs.matches() << endl;
//     cout << result << endl;
//     cout << "----------------------------" << endl;
//     return 0;
// }

