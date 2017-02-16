#include "SpiderMonkeyHelper.h"
#include "js16/jsapi.h"

#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "CException.h"
class SpiderMonkeyVM
{
    JSRuntime *m_Runtime;
public:
    SpiderMonkeyVM() 
		: m_Runtime(0) 
	{
        m_Runtime = JS_NewRuntime (5L*1024L*1024L);
    }
    ~SpiderMonkeyVM() 
	{
        JS_DestroyRuntime(m_Runtime);
        JS_ShutDown();
    };

    bool good() 
	{
        return m_Runtime != 0;
    };

    static vector<string> & getErrorInfo(JSContext * context);

    static SpiderMonkeyVM * getInstance();

    friend class SpiderMonkeyContext;
};


SpiderMonkeyVM * SpiderMonkeyVM::getInstance()
{
    static SpiderMonkeyVM SpiderMonkeyVM;

    return &SpiderMonkeyVM;
}

vector<string> & SpiderMonkeyVM::getErrorInfo(JSContext * context) 
{
    static map< JSContext*,vector<string> > errorMap;
    return (errorMap[context]);
}
///////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    char*
    Spidermystrdup(const char *str) {
        int len;
        char *res;
        len=strlen(str);
        if ((res=(char *)malloc(len+1)))
            strcpy(res,str);
        return res;
    }

    /* chop trailing \n */
    char*
    Spiderchop(const char *str) {
        char *res;
        int len;
        if (!(res=Spidermystrdup(str)))
            return NULL;
        while ((len=strlen(res))&&(res[len-1]=='\n'))
            res[len-1]=0;
        return res;
    }

    void
    SpiderprintJSException(ostream& oerr, JSContext *cx) {
        jsval val,stack;
        JSObject* obj;
        char* cstr;
        int len;
        JSString *s;

        if (!JS_GetPendingException(cx,&val))
            return;
        if (!JSVAL_IS_OBJECT(val))
            return;
        obj=JSVAL_TO_OBJECT(val);
        if (!JS_GetProperty(cx, obj, "stack", &stack))
            return;

        /* print exception.stack */
        if (!(s=JS_ValueToString(cx, stack))) {
            oerr << "Could not convert exception to string" << endl;
            return;
        }
        /* root the string */
        if (!JS_AddRoot(cx,s)) {
            oerr <<"Could not root string" << endl;
            return;
        }

        /* Todo: we loose unicode information here */
        cstr=JS_GetStringBytes(s);
        if (!cstr) {
            oerr << "Could not get string bytes" << endl;
            if (!JS_RemoveRoot(cx,s))
                oerr <<"Should not happen\n" << endl;
            return;
        }

        string err;
        len=JS_GetStringLength(s);
        for(int i=0; i<len; i++) {
            if(cstr[i] == '\0') {
                err += "\0";
            } else {
                err += (cstr[i]);
            }
        }

        oerr << "Stack:\n" << err << endl;

        if (!JS_RemoveRoot(cx,s))
            oerr << "Should not happen\n" << endl;
    }

    void
    SpiderprintError(JSContext *cx, const char *message, JSErrorReport *report) {

        vector<string> &errorInfos = SpiderMonkeyVM::getErrorInfo(cx);
        if ( errorInfos.size() > 100 ) 
           errorInfos.clear();
        //errorInfos.clear();

        int where;
        char* cstr;
        ostringstream oerr;

        oerr << "JSERROR: "<<(report->filename ? report->filename : "NULL")
             <<":"<<report->lineno<<":    "<<message<<";";
        if (report->linebuf) {
            if (!(cstr=Spiderchop(report->linebuf))) {
                oerr << "out of memory" << endl;
                //logger.alert(oerr.str().c_str());
                //cout << oerr.str().c_str() << endl;
                errorInfos.push_back(oerr.str());
                return;
            }
            oerr << "    \""<<cstr<<"\";";
            free(cstr);

            if (report->tokenptr) {
                where=report->tokenptr - report->linebuf;
                /* Todo: 80 */
                if ((where>=0)&&(where<80)) {
                    where+=6;
                    while (--where>0)
                        oerr << ' ';
                    oerr << "^;";
                }
            }
        }
        oerr <<"    Flags:";
        if (JSREPORT_IS_WARNING(report->flags))
            oerr <<" WARNING";
        if (JSREPORT_IS_EXCEPTION(report->flags))
            oerr <<" EXCEPTION";
        if (JSREPORT_IS_STRICT(report->flags))
            oerr << " STRICT";
        oerr <<" (Error number: "<<report->errorNumber<<");";
        /* print exception object */
        SpiderprintJSException(oerr, cx);

        errorInfos.push_back(oerr.str());
        //cout << oerr.str().c_str() << endl;
    }
}



SpiderMonkeyContext::SpiderMonkeyContext(size_t size)
:m_Context(0), m_Global(0),m_Good(false) 
{
    if(SpiderMonkeyVM::getInstance()->good())
	{
        if(!(m_Context = JS_NewContext (SpiderMonkeyVM::getInstance()->m_Runtime, size))) 
            return;

        if(!(m_Global  = JS_NewObject  (m_Context, NULL, NULL, NULL)))
            return;

        if(!JS_InitStandardClasses(m_Context, m_Global))
            return;

        JS_SetErrorReporter(m_Context, SpiderprintError);

		m_Good = true;
    }
}

SpiderMonkeyContext::~SpiderMonkeyContext()
{
    if(m_Context)
        JS_DestroyContext(m_Context);
}
JSContext *SpiderMonkeyContext::getContext()
{
    return m_Context;
}

JSObject  *SpiderMonkeyContext::getGlobal()
{
    return m_Global;
}

bool SpiderMonkeyContext::good()
{
    return m_Good;
}

void SpiderMonkeyContext::setGlobalObject(const string& name, JSClass* jsclass, void *address)
{
    JSObject* & obj = m_GlobalObjects[name];
    if(!obj)
	{
        obj = JS_DefineObject (m_Context, m_Global, name.c_str(), jsclass, 0,JSPROP_ENUMERATE);
        if(!obj)
			THROW_EXP(ScriptException,this->getErrorInfo().c_str());
	
        if (!JS_SetPrivate(m_Context, obj, address))
            THROW_EXP(ScriptException,this->getErrorInfo().c_str());
    }
	else 
	{
        if (!JS_SetPrivate(m_Context, obj, address))
            THROW_EXP(ScriptException,this->getErrorInfo().c_str());
    }
}

void SpiderMonkeyContext::clearGlobalObjects()
{
    map<string, JSObject*>::iterator it = m_GlobalObjects.begin();
    for(; it!=m_GlobalObjects.end(); ++it) 
	{

        //if(!JS_DeleteProperty (m_Context, m_Global, it->first.c_str()))
        //    THROW_EXP(ScriptException,this->getErrorInfo().c_str());

		if(!JS_DeleteProperty (m_Context, it->second, it->first.c_str()))
			THROW_EXP(ScriptException,this->getErrorInfo().c_str());
    }
    m_GlobalObjects.clear();
}


string SpiderMonkeyContext::getErrorInfo()
{
    vector<string> &errorInfos = SpiderMonkeyVM::getErrorInfo(m_Context);
    string buffer;
    for (size_t i = 0; i < errorInfos.size(); ++i)
	{
        buffer += errorInfos[i];
    }
    errorInfos.clear();
	return buffer;
}

//////////////////////////////////////////////////////////////////////////////////
SpiderMonkeyHelper::SpiderMonkeyHelper(SpiderMonkeyContext* ctx)
	: m_JSScript(0), m_SpiderMonkeyContext(ctx), m_Text(""),m_executed(false)
{

}


JSScript * SpiderMonkeyHelper::compileScript(const char* script)
{
    if(!m_SpiderMonkeyContext->good())
		THROW_EXP(ScriptException,this->getErrorInfo().c_str());
	
	m_Text     = script;
    m_JSScript = 0 ;
    m_JSScript = JS_CompileScript(m_SpiderMonkeyContext->getContext(),
       m_SpiderMonkeyContext->getGlobal(), script, strlen(script), "script", 1);
	
	if (0 == m_JSScript)
		THROW_EXP(ScriptException,this->getErrorInfo().c_str());

	m_executed = false;
    return m_JSScript;
}

JSScript * SpiderMonkeyHelper::compileFile(const char* name)
{
    m_Text = "";
    m_JSScript = 0 ;
    if(!m_SpiderMonkeyContext->good())
		THROW_EXP(ScriptException,this->getErrorInfo().c_str());
	
    m_JSScript = JS_CompileFile(m_SpiderMonkeyContext->getContext(), m_SpiderMonkeyContext->getGlobal(), name);
	if (0 == m_JSScript)
		THROW_EXP(ScriptException,this->getErrorInfo().c_str());

    int indent = 0;
	JSString *script =JS_DecompileScript(m_SpiderMonkeyContext->getContext(), m_JSScript, "script", indent);
	if (0 == script)
		THROW_EXP(ScriptException,this->getErrorInfo().c_str());

	m_Text = JS_GetStringBytes(script);
	m_executed = false;
    return m_JSScript;
}

string SpiderMonkeyHelper::execute()
{
	if(!good())
		THROW_EXP(ScriptException,getSpiderMonkeyContext()->getErrorInfo().c_str());

	JSContext* context =  getSpiderMonkeyContext()->getContext();
	JSObject* global   =  getSpiderMonkeyContext()->getGlobal();
	jsval rval;

	if(!JS_ExecuteScript(context, global, m_JSScript, &rval))
		THROW_EXP(ScriptException,getSpiderMonkeyContext()->getErrorInfo().c_str());

	JSString* jss = JS_ValueToString(context, rval);
	if(0 == jss)
		THROW_EXP(ScriptException,getSpiderMonkeyContext()->getErrorInfo().c_str());

	m_executed = true;
	return JS_GetStringBytes(jss);
}

void SpiderMonkeyHelper::callFunction(const char* name)
{
	if(!good())
		THROW_EXP(ScriptException,getSpiderMonkeyContext()->getErrorInfo().c_str());

	JSContext* context =  getSpiderMonkeyContext()->getContext();
	JSObject* global   =  getSpiderMonkeyContext()->getGlobal();
	jsval rval;
	if(!JS_CallFunctionName(context,global,name,0,0,&rval))
		THROW_EXP(ScriptException,getSpiderMonkeyContext()->getErrorInfo().c_str());
}

JSScript* SpiderMonkeyHelper::getScript()
{
    return m_JSScript;
}

SpiderMonkeyContext* SpiderMonkeyHelper::getSpiderMonkeyContext()
{
    return m_SpiderMonkeyContext;
}

SpiderMonkeyHelper::~SpiderMonkeyHelper()
{
	destroyScript();
}

void SpiderMonkeyHelper::destroyScript()
{
	if(m_JSScript)
		JS_DestroyScript(m_SpiderMonkeyContext->getContext(), m_JSScript);

	m_JSScript = NULL;
}

bool SpiderMonkeyHelper::good()
{
    return m_JSScript != 0;
}

const string& SpiderMonkeyHelper::getText() const
{
    return m_Text;
}

string SpiderMonkeyHelper::getErrorInfo()
{
    return m_SpiderMonkeyContext->getErrorInfo();
}


bool SpiderjsThrowError(JSContext* cx, JSObject* obj, const char* msg)
{
    JSString* jsstr;
    // if we get errors during error reporting we report those
    if ( ((jsstr=JS_NewStringCopyZ(cx, msg)))
            && (JS_AddNamedRoot(cx,&jsstr,"jsstr")) ) {
        jsval dummy;
        // We can't use JS_EvaluateScript since the stack would be wrong
        JSFunction *func;
        JSObject* fobj;
        const char* fbody="throw new Error(msg);";
        const char* argnames[]= {"msg"};
        if ((func=JS_CompileFunction(cx, obj, NULL,
                                     1, argnames,
                                     fbody, strlen(fbody),
                                     NULL, 0))) {
            // root function
            if ( ((fobj = JS_GetFunctionObject(func)))
                    && (JS_AddNamedRoot(cx, &fobj, "fobj")) ) {
                jsval args[]={STRING_TO_JSVAL(jsstr)};
                JS_CallFunction(cx, obj, func, 1, args, &dummy);
                JS_RemoveRoot(cx, &fobj);
            }
        }
        JS_RemoveRoot(cx,&jsstr);
    }

    return JS_FALSE;
}



