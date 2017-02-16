#include "JavaScript.h"
#include "js16/jsapi.h"

#include <sstream>
#include <iostream>
#include <map>
#include <vector>


class JSVM {
    JSRuntime *runtime;
public:
    JSVM() : runtime(0) {
        runtime = JS_NewRuntime (5L*1024L*1024L);
    }
    ~JSVM() {
        JS_DestroyRuntime(runtime);
        JS_ShutDown();
    };

    bool good() {
        return runtime != 0;
    };

    static vector<string> & GetErrorInfo(JSContext * context);
    static JSVM * GetInstance();

    friend class JSContainer;
};


//static JSVM jsvm;

JSVM * JSVM::GetInstance(){
    static JSVM jsvm;
    return &jsvm;
}

vector<string> & JSVM::GetErrorInfo(JSContext * context) {
    static map< JSContext*,vector<string> > errorMap;
    return (errorMap[context]);
}


extern "C" {
    char*
    mystrdup(const char *str) {
        int len;
        char *res;
        len=strlen(str);
        if ((res=(char *)malloc(len+1)))
            strcpy(res,str);
        return res;
    }

    /* chop trailing \n */
    char*
    chop(const char *str) {
        char *res;
        int len;
        if (!(res=mystrdup(str)))
            return NULL;
        while ((len=strlen(res))&&(res[len-1]=='\n'))
            res[len-1]=0;
        return res;
    }

    void
    printJSException(ostream& oerr, JSContext *cx) {
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
    printError(JSContext *cx, const char *message, JSErrorReport *report) {

        vector<string> &errorInfos = JSVM::GetErrorInfo(cx);
        if ( errorInfos.size() > 100 ) 
           errorInfos.clear();
        //errorInfos.clear();

        int where;
        char* cstr;
        ostringstream oerr;

        oerr << "JSERROR: "<<(report->filename ? report->filename : "NULL")
             <<":"<<report->lineno<<":    "<<message<<";";
        if (report->linebuf) {
            if (!(cstr=chop(report->linebuf))) {
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
        printJSException(oerr, cx);

        errorInfos.push_back(oerr.str());
        //cout << oerr.str().c_str() << endl;
    }
}



JSContainer::JSContainer(size_t size):context(0), global(0),flag(0) {
    if(JSVM::GetInstance()->good()) {
        if(!(context = JS_NewContext (JSVM::GetInstance()->runtime, size))) {
            flag = 1;
            return;
        }
        if(!(global  = JS_NewObject  (context, NULL, NULL, NULL))) {
            flag = 1;
            return;
        }
        if(!JS_InitStandardClasses(context, global)) {
            flag = 1;
            return;
        }
        JS_SetErrorReporter(context, printError);
    } else {
        flag = 1;
    }
}

JSContainer::~JSContainer() {
    if(context)
        JS_DestroyContext(context);
}
JSContext *JSContainer::getContext() {
    return context;
}

JSObject  *JSContainer::getGlobal() {
    return global;
}

bool JSContainer::good() {
    return flag == 0;
}

int JSContainer::setGlobalObject(const string& name, JSClass* jsclass, void *address) {
    JSObject* & obj = globalObjects[name];
    if(!obj) {
        obj = JS_DefineObject (context, global, name.c_str(), jsclass, 0,JSPROP_ENUMERATE);
        if(!obj)
            return -1;
        if (!JS_SetPrivate(context, obj, address))
            return -1;
    } else {
        if (!JS_SetPrivate(context, obj, address))
            return -1;
    }
    return 0;
}

int JSContainer::clearGlobalObjects() {
    map<string, JSObject*>::iterator it = globalObjects.begin();
    for(; it!=globalObjects.end(); it++) {
        if(!JS_DeleteProperty (context, global, it->first.c_str()))
            return -1;
    }
    globalObjects.clear();
    return 0;
}


string JSContainer::getErrorInfo() {
    vector<string> &errorInfos = JSVM::GetErrorInfo(context);
    int i;
    string buffer;
    for (i=0; i<errorInfos.size(); ++i) {
        buffer += errorInfos[i];
    }
    errorInfos.clear();
	return buffer;
}

JavaScript::JavaScript(JSContainer* container_)
           : jscript(0), container(container_), text("") {
    if(container->good()) {
        jscript = JS_CompileScript(container->getContext(),
           container->getGlobal(), text.c_str(), strlen(text.c_str()), "script", 1);
    }
}

JavaScript::JavaScript(JSContainer* container_, const char* script)
            : jscript(0), container(container_), text(script) {
    if(container->good()) {
        jscript = JS_CompileScript(container->getContext(),
           container->getGlobal(), script, strlen(script), "script", 1);
    }
}

JSScript * JavaScript::compileScript(const char* script) {
    text = script;
    jscript = 0 ;
    if(container->good()) {
        jscript = JS_CompileScript(container->getContext(),
           container->getGlobal(), script, strlen(script), "script", 1);
    }
    return jscript;
}

JSScript * JavaScript::compileFile(const char* name) {
    text = "";
    jscript = 0 ;
    if(container->good()) {
        jscript = JS_CompileFile(container->getContext(), container->getGlobal(), name);
        int indent = 0;
        if (jscript !=0) {
           JSString *script =JS_DecompileScript(container->getContext(), jscript, "script", indent);
           if (script !=0 )
             text = JS_GetStringBytes(script);
        }

    }
    return jscript;
}


JSScript* JavaScript::getScript() {
    return jscript;
}

JSContainer* JavaScript::getContainer() {
    return container;
}

JavaScript::~JavaScript() {
    if(jscript)
        JS_DestroyScript(container->getContext(), jscript);
}

bool JavaScript::good() {
    return jscript != 0;
}

const string& JavaScript::getText() const {
    return text;
}

string JavaScript::getErrorInfo() {
    return container->getErrorInfo();
}


bool jsThrowError(JSContext* cx, JSObject* obj, const char* msg) {


    JSString* jsstr;
    // if we get errors during error reporting we report those
    if ( ((jsstr=JS_NewStringCopyZ(cx, msg)))
            && (JS_AddNamedRoot(cx,&jsstr,"jsstr")) ) {
        jsval dummy;
        // We can't use JS_EvaluateScript since the stack would be wrong
        JSFunction *func;
        JSObject* fobj;
        const char* fbody="throw new Error(msg);";
        const char* argnames[]= {"msg"
                                };
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



