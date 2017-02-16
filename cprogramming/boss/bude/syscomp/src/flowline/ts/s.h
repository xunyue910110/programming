#ifndef _SCRIPTS_H_
#define _SCRIPTS_H_
#include "base/SmartPtr.h"
#include "jshlp/JavaScript.h"
#include "flowline/DataSet.h"

#include <string>
#include <map>
using namespace std;

namespace flowline {


class RowJavaScript : public JavaScript{
	//  JSObject* rowproto;
  //  JSObject* ctxproto;
public:
	RowJavaScript(JSContainer* container, const char* script);
	
	void execute(dataset::Data data);
	void execute(string& data);	
	
	static JSClass* rowClass();
  static JSClass* ctxClass();
};

typedef counted_ptr<RowJavaScript> PRowJavaScript;

class RowJSContainer : public JSContainer {
    map<string, PRowJavaScript> m_javascripts;
public:
    RowJSContainer();
    RowJavaScript& get(const string& script);    
};

}
#endif //_SCRIPTS_H_

