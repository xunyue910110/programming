
#include "SoAdapterFactory.h"
#include "base/ClassRegister.h"
#include "base/StringUtil.h"
#include "flowline/FlowLineException.h"
#include "flowline/flbase.h"
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <dlfcn.h>
#include <sstream>
USING_NAMESPACE( std );
using namespace flowline;

SoAdapterFactory::SoAdapterFactory( )  {
}


SoAdapterFactory::~SoAdapterFactory() {
	cout << "close handles " << endl;
    for ( int i = 0; i < m_handles.size(); i++ )
        dlclose( m_handles[ i ] );
}
//typedef map<string, ClassRegister<Adapter>::ClassCreator>&  (*getAdapterCreators)();

void SoAdapterFactory::init(const Properties* env) {
    vector<string> paths;
    
    StringUtil::split( env->getProperty("flowline.adapters"), ":", paths );
    for ( int i = 0; i < paths.size(); i++ ) {
        void* handle = dlopen( paths[ i ].c_str(), RTLD_LAZY );
        if ( handle == 0 ) {
            THROW_P2( ExecutionException, "无效的适配器库名'%s':%s.", paths[ i ].c_str(), dlerror() );
        }
        /*getAdapterCreators handler = (getAdapterCreators)dlsym(handle, "getAdapterCreators");
        if(handler) {
            map<string, ClassRegister<Adapter>::ClassCreator>& creators = handler();
            ClassRegister<Adapter>::creators().insert(creators.begin(), creators.end());
        } else {
        	  THROW_P1( ExecutionException, "无效的适配器库名'%s'的插件列表函数getAdapterCreators(). 必须连接sohlp.cpp, 并放置到所有.cpp的最前面", paths[ i ].c_str());
        }
        m_handles.push_back( handle );*/
    }

    ostringstream out;
    vector<string> classNames;
    out << "可用插件:";
    AdapterClassRegister::getClassNames(classNames);
    
    vector<string>::iterator it = classNames.begin();
    for (int i=0 ; it!=classNames.end(); it++, i++) {
        out << endl << *it;
    }
    g_logger.info( out.str().c_str() );
}

Adapter * SoAdapterFactory::doCreate( const string & className ) {
	return AdapterClassRegister::create(className);
}


void SoAdapterFactory::getClassNames( vector<string>& classNames ) const {
	AdapterClassRegister::getClassNames(classNames);
}
