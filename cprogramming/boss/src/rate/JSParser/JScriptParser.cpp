#include "JScriptParser.h"
#include "js16/jsapi.h"

#include <cstdlib>
#include <cassert>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include "CException.h"

const int ERROR_CODE = -100;
const char * JS_ERROR_CODE ="JS_ERROR_CODE";
const char * JS_ERROR_MESG ="JS_ERROR_MESG";
static void SetJSBufferError(DataBuffer * bufferPtr,int errCode,const char * errMsg)
{
	if (NULL == bufferPtr )
		return;

	if (!bufferPtr->existField(JS_ERROR_CODE))
	{
		bufferPtr->newField(JS_ERROR_CODE,dtInteger,4);
		bufferPtr->newField(JS_ERROR_MESG,dtString,100);
	}
	
	bufferPtr->setValue(JS_ERROR_CODE,errCode);
	bufferPtr->setValue(JS_ERROR_MESG,errMsg);
}

static JSBool Buffer_getProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	return JS_TRUE;
}

static JSBool Buffer_setProperty(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{	
    return JS_TRUE;
}


static JSBool PushValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 2)
	{
		THROW_EXP(ScriptException,"pushValue参数个数不正确!");
	}
		
	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string keyString   = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string valueString = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	const char * key = keyString.c_str();
	const char * value = valueString.c_str();
	DataBuffer::iterator it = (*bufferPtr).findField(key);
	if (it == (*bufferPtr).end())
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -1,string(" 域:" + keyString + "不存在!").c_str());
		return JS_TRUE;
	}
	
	if (it->second.getDataType() == dtString)
		(*bufferPtr).pushBack(key,value);
	else if (it->second.getDataType() == dtInteger)
		(*bufferPtr).pushBack(key,atol(value));
	else if (it->second.getDataType() == dtDouble)
		(*bufferPtr).pushDouble(key,atof(value) );
	else 
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -2,"数据类未被支持!");
	}

    return JS_TRUE;
}


static JSBool SetValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 2)
	{
		THROW_EXP(ScriptException,"setValue参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string keyString   = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string valueString = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	int index = 0;
	if (argc > 2)
	{
		string strIndex = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
		index = atol(strIndex.c_str());
		if (index < 0)
		{
			SetJSBufferError(bufferPtr,ERROR_CODE -3,string(" getValue 参数输入不合法!index="+strIndex).c_str());
			return JS_TRUE;
		}
	}
	
	const char * key = keyString.c_str();
	const char * value = valueString.c_str();
	DataBuffer::iterator it = (*bufferPtr).findField(key);
	if (it == (*bufferPtr).end())
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -4,string(" 域:" + keyString + "不存在!").c_str());
		return JS_TRUE;
	}

	if (it->second.getDataType() == dtString)
		(*bufferPtr).setValue(key,value,index);
	else if (it->second.getDataType() == dtInteger)
		(*bufferPtr).setValue(key,atol(value),index);
	else if (it->second.getDataType() == dtDouble)
		(*bufferPtr).setDouble(key,atof(value),index);
	else 
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -5,"  数据类未被支持!");
	}

	return JS_TRUE;
}

static JSBool ExistValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"existValue参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string keyString   = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	int index = 0;
	if (argc > 1)
	{
		string strIndex = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		index = atol(strIndex.c_str());
		if (index < 0)
		{
			SetJSBufferError(bufferPtr,ERROR_CODE -6,string(" ExistValue 参数输入不合法!index="+strIndex).c_str());
			return JS_TRUE;
		}
	}

	bool ok = bufferPtr->existValue(keyString,index);
	*rval = BOOLEAN_TO_JSVAL (ok);

	return JS_TRUE;
}


static JSBool PopValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"popValue参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string key   = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	DataBuffer::iterator it = (*bufferPtr).findField(key.c_str());
	if (it == (*bufferPtr).end())
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -7,string(" 域:" + key + "不存在!").c_str());
		return JS_TRUE;
	}
	
	it->second.popBack();

    return JS_TRUE;
}

static JSBool GetValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"getValue参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string keyString = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	const char * key = keyString.c_str();
	int index = 0;
	if (argc > 1)
	{
		string strIndex = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		index = atol(strIndex.c_str());
		if (index < 0)
		{
			SetJSBufferError(bufferPtr,ERROR_CODE -8,string(" getValue 参数输入不合法!index="+strIndex).c_str());
			return JS_TRUE;
		}
	}

	DataBuffer::iterator it = (*bufferPtr).findField(key);
	if (it == (*bufferPtr).end() || it->second.size() == 0)
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -9,string(" 域:" + keyString + "不存在,或者对应值不存在!").c_str());
		return JS_TRUE;
	}

	if (it->second.getDataType() == dtString)
		*rval = STRING_TO_JSVAL (JS_NewStringCopyZ(cx,(*bufferPtr).getString(key,index)) );
	else if (it->second.getDataType() == dtInteger)
		*rval = INT_TO_JSVAL ((*bufferPtr).getInt(key,index));
	else if (it->second.getDataType() == dtDouble)
		JS_NewNumberValue(cx, (*bufferPtr).getDouble(key,index), rval);
	else 
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -10," 数据类未被支持!");
	}
		
	return JS_TRUE;
}


static JSBool GetValueEx(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"getValue参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string keyString = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	const char * key = keyString.c_str();
	int index = 0;
	if (argc > 1)
	{
		string strIndex = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		index = atol(strIndex.c_str());
		if (index < 0)
		{
			SetJSBufferError(bufferPtr,ERROR_CODE -8,string(" getValue 参数输入不合法!index="+strIndex).c_str());
			return JS_TRUE;
		}
	}

	DataBuffer::iterator it = (*bufferPtr).findField(key);
	if (it == (*bufferPtr).end() || it->second.size() == 0)
	{
		bufferPtr->callback(key);
		it = (*bufferPtr).findField(key);
		if (it == (*bufferPtr).end() || it->second.size() == 0)
		{
			SetJSBufferError(bufferPtr,ERROR_CODE -9,string(" 域:" + keyString + "不存在!").c_str());
			return JS_TRUE;
		}
	}

	if (it->second.getDataType() == dtString)
		*rval = STRING_TO_JSVAL (JS_NewStringCopyZ(cx,(*bufferPtr).getString(key,index)) );
	else if (it->second.getDataType() == dtInteger)
		*rval = INT_TO_JSVAL ((*bufferPtr).getInt(key,index));
	else if (it->second.getDataType() == dtDouble)
		JS_NewNumberValue(cx, (*bufferPtr).getDouble(key,index), rval);
	else 
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -10," 数据类未被支持!");
	}

	return JS_TRUE;
}


inline static JSBool NewField(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 3)
	{
		THROW_EXP(ScriptException,"newField参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string key  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string type = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	string len  = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	if(	(*bufferPtr).existField(key.c_str()))
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -11,string(" 域:" + key + "已经存在!").c_str());
		return JS_TRUE;
	}

	int tmpLen  = atol(len.c_str());
	if (type == "dtInteger")
		(*bufferPtr).newField(key.c_str(),dtInteger,tmpLen);
	else if (type == "dtString")
		(*bufferPtr).newField(key.c_str(),dtString,tmpLen);
	else if (type == "dtDouble")
		(*bufferPtr).newField(key.c_str(),dtDouble,tmpLen);	
	else
	{
		SetJSBufferError(bufferPtr,ERROR_CODE -12,string(" 未定义的数据类型:" + type + "!").c_str());
	}

    return JS_TRUE;
}

inline static JSBool ExistField(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"existField参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string key  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	bool ret = (*bufferPtr).existField(key.c_str());
	
	*rval = BOOLEAN_TO_JSVAL (ret);

	return JS_TRUE;
}

inline static JSBool Count(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc < 1)
	{
		THROW_EXP(ScriptException,"Count 参数个数不正确!");
	}

	DataBuffer * bufferPtr = static_cast<DataBuffer *>(JS_GetPrivate(cx,obj));
	if (NULL == bufferPtr)
		THROW_EXP(ScriptException,"获取buffer对象失败!");

	string key  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

	int iCount = 0;
	if (bufferPtr->existField(key))
		iCount = (*bufferPtr).count(key.c_str());

	*rval = INT_TO_JSVAL(iCount);

	return JS_TRUE;
}



static JSFunctionSpec Buffer_methods[] = 
{
    {"pushValue",PushValue, 0, 0, 0},
    {"getValue", GetValue, 0, 0, 0},
	{"setValue", SetValue, 0, 0, 0},
	{"newField", NewField, 0, 0, 0},
	{"popValue", PopValue, 0, 0, 0},
	{"existField", ExistField, 0, 0, 0},
	{"existValue",ExistValue, 0, 0, 0},
	{"count",Count, 0, 0, 0},
	{"getValueEx",GetValueEx, 0, 0, 0},
	{NULL}
};

static JSPropertySpec Buffer_properties[] = 
{
    { "", 0, JSPROP_INDEX },
    { 0 }
};


JSClass* jsclass() 
{
	static JSClass Buffer_class =
	{
		"Buffer", JSCLASS_HAS_PRIVATE,
		JS_PropertyStub,JS_PropertyStub,
		Buffer_getProperty,
		Buffer_setProperty,
		JS_EnumerateStub, JS_ResolveStub,JS_ConvertStub,
		JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
	};
    return &Buffer_class;
}


static SpiderMonkeyContext * GetSpiderMonkeyContext()
{
    static SpiderMonkeyContext ctx;
    return &ctx;
}


JScriptParser::JScriptParser()
	:SpiderMonkeyHelper(GetSpiderMonkeyContext()),m_JSObj(0)
{
	init();
}

void JScriptParser::init()
{
  m_JSObj = JS_InitClass(getSpiderMonkeyContext()->getContext(),
               getSpiderMonkeyContext()->getGlobal(), NULL,jsclass(),
               NULL, 0, Buffer_properties, Buffer_methods, NULL, NULL);
}


bool JScriptParser::good()
{
    return m_JSObj != 0;
}

bool JScriptParser::registerObject(const char * objName,DataBuffer * buf)
{
    if (buf == 0)
        return false;

    GetSpiderMonkeyContext()->setGlobalObject(objName,jsclass(),buf);
    return true;
}

void JScriptParser::clearObjects()
{
	GetSpiderMonkeyContext()->clearGlobalObjects();
}

JScriptParser * JScriptParser::getInstance()
{
	static JScriptParser  s_parser;
	return &s_parser;
}

void ExecuteScript(const char * script,DataBuffer & inBuf,DataBuffer &outBuf)
{
	/*
		if (inBuf.getLong('RECV_FEE') >= 10000 && inBuf.getLong('RECV_FEE') < 30000)
			outBuf.setValue('ACTION_RULE_ID',10001);
		else if ( inBuf.getLong('RECV_FEE') >= 30000)
			outBuf.setValue('ACTION_RULE_ID',10002);
		else
		{
			outBuf.setValue('RESULT_INFO','收费金额不满足条件!');
		}
	
	*/

	JScriptParser  * parser = JScriptParser::getInstance();
	parser->registerObject("inBuf",&inBuf);
	parser->registerObject("outBuf",&outBuf);
	if (parser->getText() != script)
	{
		parser->destroyScript();
		parser->compileScript(script);
	}

	if (outBuf.existField(JS_ERROR_CODE))
	{
		outBuf.clear(JS_ERROR_CODE);
		outBuf.clear(JS_ERROR_MESG);
	}
	parser->execute();
	parser->clearObjects();
	if (GetInt(outBuf,JS_ERROR_CODE,0,0) != 0)
	{
		THROW_EXP(ScriptException,outBuf.getString(JS_ERROR_MESG));
	}
	if (GetInt(inBuf,JS_ERROR_CODE,0,0) != 0)
	{
		THROW_EXP(ScriptException,inBuf.getString(JS_ERROR_MESG));
	}

}

void ExecuteScriptFile(const char * scriptFile,DataBuffer & inBuf,DataBuffer &outBuf)
{
	JScriptParser  * parser = JScriptParser::getInstance();
	parser->registerObject("inBuf",&inBuf);
	parser->registerObject("outBuf",&outBuf);
	parser->compileFile(scriptFile);

	if (outBuf.existField(JS_ERROR_CODE))
	{
		outBuf.clear(JS_ERROR_CODE);
		outBuf.clear(JS_ERROR_MESG);
	}
	parser->execute();
	parser->destroyScript();
	parser->clearObjects();
	if (GetInt(outBuf,JS_ERROR_CODE,0,0) != 0)
	{
		THROW_EXP(ScriptException,outBuf.getString(JS_ERROR_MESG));
	}

}
/*

class A
{
public:
	bool operator <(const A & right) const
	{
		if ( x <= right.x)
			return true;
		else
			return false;
	}
	int x;

};
#include <vector>
#include <algorithm>
using namespace std;
 int main() 
 {
     
	vector<A> vec;
	for (int i = 0; i < 100; ++i)
	{
		A a;
		vec.push_back(a);
	}
	std::sort(vec.begin(),vec.end());

	 Buffer inBuf;
	 Buffer outBuf;
	try
	{
		inBuf.newField("f1",dtString,10);
		outBuf.newField("f2",dtString,100);
		outBuf.newField("f3",dtInteger,100);
		
		outBuf.pushBack("f3",345);
		outBuf.pushBack("f3",566);
		outBuf.pushBack("f3",-1);
		cout << outBuf <<endl;
		outBuf.setValue("f3",8891,0);
		outBuf.setValue("f3",8892,1);
		outBuf.setValue("f3",8893,2);
		//outBuf.setValue("f3",7893,3);
		cout << outBuf <<endl;

//inBuf.setValue("f1","abdfdfdfd",0);

		time_t t1 = time(NULL);
		for (int i = 0; i< 100000; ++i)
		{
			
			inBuf.clear();
			outBuf.clear();
			inBuf.pushBack("f1","songzj1"); 
			inBuf.pushBack("f1","ok");
			outBuf.pushBack("f2","具体的问题具体解决!"); 
			//outBuf.push_back("f4",(BigInt)3467890099232);
			ExecuteScript("if (inBuf.getValue('f1',1) == 'ok'){ \
						  outBuf.pushValue('f2','songzj ok');\
						  outBuf.pushValue('f3',456);\
						  var i = outBuf.getValue('f3');\
						  i *=2; \
						  outBuf.pushValue('f3',i);\
						  }",inBuf,outBuf);
			
		}

		//parser->destroyScript();
		//parser->clearObjects();

		time_t t2 = time(NULL);
		cout << "pushTimes=" << pushTimes << ",getTimes=" << getTimes <<endl;
		cout << "t2-t1=" << t2-t1;

	
		JScriptParser parser;
		parser->registerObject("inBuf",&inBuf);
		parser->registerObject("outBuf",&outBuf);

		
		time_t t1 = time(NULL);
		for (int i = 0; i< 10000; ++i)
		{
			inBuf.clear();
			outBuf.clear();
			parser->compileScript("inBuf.pushValue('f1','songzj1'); inBuf.pushValue('f1','ok'); \
								 outBuf.pushValue('f2','具体的问题具体解决!'); if (inBuf.getValue('f1',1) == 'ok'){ \
								 outBuf.pushValue('f2','songzj ok');outBuf.newField('f3','dtInteger',4);\
								 outBuf.pushValue('f3',456);var i = outBuf.getValue('f3');i *=2; \
								 outBuf.pushValue('f3',i);if (outBuf.getValue('f4') == 3467890099232323223) outBuf.pushValue('f4',outBuf.getValue('f4')+1); \
								 }"); 
			parser->execute();
		}

		parser->getSpiderMonkeyContext()->clearGlobalObjects();

		time_t t2 = time(NULL);
		cout << "pushTimes=" << pushTimes << ",getTimes=" << getTimes <<endl;
        cout << "t2-t1=" << t2-t1;

	}catch(CException & ex)
	{
		cout << ex.displayText() <<endl;
		return -1;
	}
     cout << "--------InBuf--------------------" << endl;
	 cout << inBuf;
	 cout << "--------outBuf--------------------" << endl;
	 cout << outBuf;

     return 0;
}

*/
