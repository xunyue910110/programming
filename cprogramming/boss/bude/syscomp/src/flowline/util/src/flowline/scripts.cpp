#include "scripts.h"
#include "flowline/flbase.h"
#include "flowline/Context.h"
#include <jsapi.h>
#include <cstdlib>
#include <cassert>
#include <string.h>
#include <iostream>


using  dataset::Row;
using  dataset::Data;
using  dataset::Field;

USING_NAMESPACE(std);
using namespace flowline;
extern "C" {
/////////////////////////////////////////////////////////////////////////////////////////////////////
////  DataSet::Iterator    
    
    static
    JSBool
    DataSet_Iterator_getField
    (JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval);

    static JSBool
    DataSet_Iterator_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        DataSet_Iterator_getField(cx, obj, 1, &id, vp);
        return JS_TRUE;
    }

    static JSClass DataSet_Iterator_class =
        {
            "DataSet_Iterator", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub,  JS_PropertyStub, DataSet_Iterator_getProperty, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub,
            JS_ConvertStub,  JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
        };

static
    JSBool
    DataSet_Iterator_getField
    (JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) {
        if (!JS_InstanceOf(cx, obj, &DataSet_Iterator_class, argv))
            return JS_FALSE;
        
        DataSet::iterator* pit=static_cast<DataSet::iterator *>(JS_GetPrivate(cx,obj));
        if (!pit)
            return jsThrowError(cx,obj,"类型不正确, 不是DataSet::iterator类型");

        dataset::Row row = **pit;
        DataSet* ds = row.getDataSet();

        if (argc != 1)
            return jsThrowError(cx,obj,"参数不对, 只能一个");
         
        int index = -1;
        char errmsg[128];    
		if (JSVAL_IS_INT (argv[0])) {
			index = JSVAL_TO_INT(argv[0]);
			if(index < 0 || index >= ds->fieldCount()) {
		            snprintf(errmsg, sizeof(errmsg), "下标越界%d, 有效范围是0-%d", index, ds->fieldCount()-1);
		            return jsThrowError(cx, obj, errmsg);
		    }
		} else if (JSVAL_IS_STRING(argv[0])) {
	        const char* name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	        index = ds->getFieldIndex(name);
			if(index < 0) {
		            snprintf(errmsg, sizeof(errmsg), "查不到指定的字段名'%s'", name);
		            return jsThrowError(cx, obj, errmsg);
		    }
        } else {
        	return jsThrowError(cx,obj,"参数不是字符串类型");
		}

        const dataset::Field& fld = ds->getField(index);
        switch(fld.getDataType()) {
        case dataset::FTINT:
            return JS_NewNumberValue(cx,(int)row[index],rval);
        case dataset::FTINT64:
            return JS_NewNumberValue(cx,(int64_t)row[index],rval);    
        case dataset::FTDOUBLE:
            return JS_NewDoubleValue(cx,row[index],rval);
        case dataset::FTSTRING: {
                JSString * jss = JS_NewStringCopyZ(cx, row[index].c_str());
                if(jss) {
                    *rval = STRING_TO_JSVAL(jss);
                    return JS_TRUE;
                }
                return jsThrowError(cx, obj, "JSVM不能分配一个字符串");
            }
        default:
            return jsThrowError(cx, obj, "未知的DataSet字段类型");
        }
        return JS_FALSE;
    }

   //static JSFunctionSpec DataSet_Iterator_methods[] = {
     //           {"getField",DataSet_Iterator_getField,1,0,0}, {0,0,0,0,0}
       //     };
    
	JSPropertySpec DataSet_Iterator_properties[] =
	{
        { "", 0, JSPROP_INDEX|JSPROP_READONLY},
        { 0 }
	};
    
/////////////////////////////////////////////////////////////////////////////////////////////////////
////  Context
    
    static
    JSBool
    Context_getAttribute
    (JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval);

    static JSBool
    Context_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        Context_getAttribute(cx, obj, 1, &id, vp);
        return JS_TRUE;
    }

    static JSClass Context_class =
        {
            "Context", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub,  JS_PropertyStub, Context_getProperty, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub,
            JS_ConvertStub,  JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
        };

static
    JSBool
    Context_getAttribute
    (JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) {
        if (!JS_InstanceOf(cx, obj, &Context_class, argv))
            return JS_FALSE;
        
        Context* context=static_cast<Context *>(JS_GetPrivate(cx,obj));
        if (!context)
            return jsThrowError(cx,obj,"类型不正确, 不是Context类型");


        if (JSVAL_IS_STRING(argv[0])) {
            const char* name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
            string v = context->getAttribute(name);
            JSString * jss = JS_NewStringCopyZ(cx, v.c_str());
                if(jss) {
                    *rval = STRING_TO_JSVAL(jss);
                    return JS_TRUE;
                }
                return jsThrowError(cx, obj, "JSVM不能分配一个字符串");
        } else {
            return jsThrowError(cx,obj,"参数不是字符串类型");
        }
        return JS_FALSE;
    }

   // static JSFunctionSpec Context_methods[] = {
     //   {"getAttribute",Context_getAttribute,1,0,0}, {0,0,0,0,0}
    //};
    
    JSPropertySpec Context_properties[] =
    {
        { "", 0, JSPROP_READONLY},
        { 0 }
    };    
}
namespace flowline {
JSClass* RowJavaScript::rowClass() {
    return &DataSet_Iterator_class;
}

JSClass* RowJavaScript::ctxClass() {
    return &Context_class;
}

RowJavaScript::RowJavaScript(JSContainer* container, const char* script) : JavaScript(container, script) {    
}

void RowJavaScript::execute(string& data) {
    if(!good())
        THROW_P1(DefinitionException, "JavaScript不可用:\n, %s", getText().c_str());

    JSContext* context =  getContainer()->getContext();
    JSObject* global =   getContainer()->getGlobal();
    jsval rval;
    if(!JS_ExecuteScript(context, global, jscript, &rval))
    THROW_P2(DefinitionException, "执行脚步失败 \n%s\n%s", getText().c_str(), getErrorInfo().c_str());
    JSString* jss = JS_ValueToString(context, rval);
   
    if(jss == 0)
        THROW_P1(DefinitionException, "执行结果转换到字符串失败\n%s", getText().c_str());
    data = JS_GetStringBytes(jss);

}

void RowJavaScript::execute(dataset::Data data) {
    if(!good())
        THROW_P1(DefinitionException, "JavaScript不可用:\n, %s", getText().c_str());
    JSContext* context =  getContainer()->getContext();
    JSObject* global =   getContainer()->getGlobal();

    jsval rval;
    if(!JS_ExecuteScript(context, global, jscript, &rval))
        THROW_P2(DefinitionException, "执行脚步失败 \n%s\n%s", getText().c_str(), getErrorInfo().c_str());
        

    int dataType = data.getDataType();
    if(dataType == dataset::FTINT) {
        int32 i;
        if(!JS_ValueToInt32(context, rval, &i)) {
            THROW_P1(DefinitionException, "执行结果转换到Int32失败\n%s", getText().c_str());
        }
        data = i;
    } else if(dataType == dataset::FTINT64) {
        double d;
        if(!JS_ValueToNumber(context, rval, &d)) {
            THROW_P1(DefinitionException, "执行结果转换到Double失败\n%s", getText().c_str());
        }
        data = (int64_t)d;
    } else if(dataType == dataset::FTDOUBLE) {
        double d;
        if(!JS_ValueToNumber(context, rval, &d)) {
            THROW_P1(DefinitionException, "执行结果转换到Double失败\n%s", getText().c_str());
        }
        data = d;
    } else if(dataType == dataset::FTSTRING) {
        JSString* jss = JS_ValueToString(context, rval);
        if(jss == 0)
            THROW_P1(DefinitionException, "执行结果转换到字符串失败\n%s", getText().c_str());
        data = JS_GetStringBytes(jss);
    } else {
        THROW_P1(DefinitionException, "未知的DataSet字段类型%d", dataType);
    }
}


RowJavaScript& RowJSContainer::get(const string& script){    
    PRowJavaScript& js = m_javascripts[script];
    if(!js) 
        js = PRowJavaScript(new RowJavaScript(this, script.c_str()));    
    if(!js->good()) {
        THROW_P2(DefinitionException, "javascript无效js=%s\n%s", script.c_str(), js->getErrorInfo().c_str());
    }
    return *js.get();
}

RowJSContainer::RowJSContainer() {
    assert(JS_InitClass(getContext(), getGlobal(), NULL, &DataSet_Iterator_class,
                                NULL, 0, DataSet_Iterator_properties, NULL, NULL, NULL) != 0);
                                
    assert(JS_InitClass(getContext(), getGlobal(), NULL, &Context_class,
                                NULL, 0, Context_properties, NULL, NULL, NULL) != 0);
                                
    cout << "+++++++ ok +++++" << endl;                                                          
}                                

}
/*
using namespace flowline;
int main() {
	try {
	DataSet ds, ds2;
	ds.addField(Field("f1", FTINT,8));
	ds.addField(Field("f2", FTDOUBLE,10));
	ds.addField(Field("f3", FTSTRING, 32));
	ds2.addField(Field("f1", FTINT,8));
	ds2.addField(Field("f2", FTDOUBLE,10));
	ds2.addField(Field("f3", FTSTRING, 32));
	
	DataSet::iterator it = ds.pushBack();
	(*it)[0] = 1;
	(*it)[1] = 2.01;
	(*it)[2] = "adsf"; 
	char *s="abcd ";
	DataSet::iterator it2 = ds2.pushBack();
	(*it2)[0] = 1;
	(*it2)[1] = 2.01;
	(*it2)[2] = "adsf"; 
	JSContainer container;
	//RowJavaScript rjs(&container, "src.getField('f3')+(src.getField('f2')+src.getField('f1'))");
	
	string sourceName = "aa001_T_aa.txt";
	string ss = "sourceName.find(001)!= -1 ?(sourceName.find(aa001)!=-1 ? 'dst3' :'dst1'):'dst2'";
	int temp = -1;
	string jj;
	for(; ;){
		temp = ss.find("sourceName");
		if(temp==-1)break;
		jj = ss.substr(ss.find("sourceName.find(")+16,ss.find(")") - ss.find("sourceName.find(")-16);
		cout<<" jj "<<jj.c_str()<<endl;
		
		int ii = -1; 
		ii = sourceName.find(jj.c_str());
		
		char tchar[10];			
				     
		sprintf(tchar,"%d",ii);
		string bb = tchar;
		cout<<"bb value is :  "<<bb<<endl;
		ss.replace(ss.find("sourceName.find("),( ss.find(")") - ss.find("sourceName.find(") + 1 ), bb.c_str());
		cout<<"ss value is :224 row :  "<<ss.c_str()<<endl;
	    bb.clear();
	    jj.clear();
	    }
	cout<<"after ss is .. "<<ss.c_str()<<endl;
	
	//RowJavaScript rjs(&container, " 2!= -1 ?(2!=1?(2>1 ?'dst4' :'dst5') :'dst3'):'dst2'");
      RowJavaScript rjs(&container, ss.c_str());
	cout <<"rjs.good()"<< rjs.good() << endl;
	map<string, dataset::Row> params;
	
	//params.insert(pair<string, dataset::Row>("src", *it));
	container.setGlobalObject("src", RowJavaScript::jsclass() ,&it);
    string name =" ";
//	FP_BEGIN(A);
	for(int i=0; i<20; i++) {
		cout<<"before value is : "<<(*it2)[2]<<endl;
		//rjs.execute((*it2)[2]);
		 rjs.execute(name);
		cout<<"after value is : "<<(*it2)[2]<<"/"<<name<<endl;
	}
//	FP_END(A);	
	
	cout << (*ds2.begin())[2] << endl;
	return 0;
	}catch(FlowLineException& e) {		
		cout << e << endl;
		return 1;
	}
}
*/
