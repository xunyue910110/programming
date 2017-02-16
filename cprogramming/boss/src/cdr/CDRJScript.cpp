
#include "CDRJScript.h"

#include "js16/jsapi.h"
#include <cstdlib>
#include <cassert>
#include <string.h>
#include <iostream>

#include "CdrBase.h"

extern "C" {
static JSBool
CDR_getProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
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

    CdrBase* pcdr=static_cast<CdrBase *>(JS_GetPrivate(cx,obj));
    if (!pcdr)
      return jsThrowError(cx,obj,"类型不正确");

    if (index < 0 || index >= pcdr->getMaxIndex())
      return jsThrowError(cx, obj, "属性不存在");

    JSString * jss = JS_NewStringCopyZ(cx, (*pcdr)[index].c_str());
    if(jss) {
        *vp = STRING_TO_JSVAL(jss);
        return JS_TRUE;
    }
    return jsThrowError(cx, obj, "JSVM不能分配一个字符串");
}



static JSBool
CDR_setProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
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


    CdrBase* pcdr=static_cast<CdrBase *>(JS_GetPrivate(cx,obj));
    if (!pcdr)
      return jsThrowError(cx,obj,"类型不正确");

    if (index < 0 || index >= pcdr->getMaxIndex())
      return jsThrowError(cx, obj, "属性不存在");

    (*pcdr)[index] = value;
    return JS_TRUE;
}
}

static JSClass CDR_class = {
    "CdrBase", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,JS_PropertyStub,
    CDR_getProperty,
    CDR_setProperty,
    JS_EnumerateStub, JS_ResolveStub,JS_ConvertStub,
    JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSFunctionSpec CDR_methods[] = {
    {0,0,0,0,0}
};


JSPropertySpec CDR_properties[] = {
    { "", 0, JSPROP_INDEX },
    { 0 }
};


JSClass* CDRJScript::jsclass() {
    return &CDR_class;
}



CDRJScript::CDRJScript()
            :JavaScript(GetContainer()) , jsObj(0) {
  if(JavaScript::good())
       jsObj = JS_InitClass(getContainer()->getContext(),
               getContainer()->getGlobal(), NULL, &CDR_class,
               NULL, 0, CDR_properties, CDR_methods, NULL, NULL);
}



CDRJScript::CDRJScript(const char* script)
                   :JavaScript(GetContainer(), script) , jsObj(0) {
  if(JavaScript::good())
       jsObj = JS_InitClass(getContainer()->getContext(),
               getContainer()->getGlobal(), NULL, &CDR_class,
               NULL, 0, NULL, CDR_methods, NULL, NULL);
}


int CDRJScript::callFunction(const char* name) {
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

bool CDRJScript::matches() 
{
   string isOK("true");
   string result;
   if (execute(result)>=0) 
   {
        return (result == isOK) ;
   }
   return false;
}


int CDRJScript::execute(string &result) {
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

bool CDRJScript::good() {
    return jsObj != 0;
}

JSContainer *CDRJScript::GetContainer() {
    static JSContainer container;
    return &container;
}

bool CDRJScript::Initialize(CdrBase * cdr) {
    if (cdr == 0)
        return false;
    GetContainer()->setGlobalObject("cdr",CDRJScript::jsclass(),cdr);
    return true;
}


// int main() {
//     string result;
//     CdrBase cdr;
//     cdr.Attributes[0] = "sunhk";
//     cdr.Attributes[2] = "sunhk1";
//
//     CDRJScript rjs(" cdr[2]==cdr[2]; ");
//     CDRJScript::Initialize(&cdr);
//     cout << "script:" << rjs.getText() << endl;
//     cout << rjs.execute(result) << result << endl;
//     cout << rjs.matches() << endl;
//     cout << result << endl;
//     cout << "----------------------------" << endl;
//     return 0;
// }


