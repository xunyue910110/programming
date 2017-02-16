#include "flowline/flbase.h"
#include "OcciFlowLineDao.h"
#include "SoAdapterFactory.h"
#include "flowline/FlowLineException.h"
#include "flowline/Engine.h"
#include "base/Properties.h"
#include <iostream>
#include <string>
#include <fstream>


using namespace flowline;

USING_NAMESPACE( std );


///////////////////////////////////////////////////////////////////////////
// Application
///////////////////////////////////////////////////////////////////////////




class Application {
    FlowLineDao* m_dao;
    AdapterFactory* m_factory;
    map<string, string> m_args;
    Properties m_env;
    string m_main;
public:

    Application() : m_dao( 0 ), m_factory( 0 ) {}

    ~Application() {
        delete m_dao;
        delete m_factory;
    }

    int parseArgs( int argc, char* argv[] ) {
        string key;         
              
        for ( int i = 1; i < argc; i++ ) {
            if ( argv[ i ][ 0 ] == '-' ) {
                key = argv[ i ] + 1 ;
                m_args[ key ] = argv[ i + 1 ];
                cout << "argv: key=" << key << " value=" << m_args[ key ] <<endl;
            } else {
                if ( key.empty() ) {
                    if ( i == argc - 1 ) {
                        //最后一个是流程的名
                        m_main = argv[ i ];
                    } else {
                        g_logger.alert( "无效的参数 %s", argv[ i ] );
                        return -1;
                    }
                } else {
                    m_args[ key ] = argv[ i ];
                    key.clear();
                }
            }
        }
        return 0;
    }

    void initFactory() {
    	FL_TRACE("初始化工厂");        
        m_factory = new SoAdapterFactory();
        m_factory->init(&m_env);
    }

    void initDao() {
    	FL_TRACE("初始化DAO");
        m_dao = new OcciFlowLineDao();
        m_dao->setAdapterFactory( m_factory );
        m_dao->init(&m_env);
    }
    
    void loadEnv() {    	
    	ifstream in;
    	if ( m_args.find("p") != m_args.end() ){
    		const char * cfg = m_args["p"].c_str();
    		if(cfg) {
    			in.open(cfg);
    		}
    		if(!in.good())
    		THROW_P1(ExecutionException, 
    			"加载-p %s配置失败, 可以在当前目录下放置flowline.properties, 或者配置FLOWLINE_CFG指向配置文件",cfg);
        
      }else{
	    	in.open("flowline.properties");
	    	if(!in.good()) {
	    		const char * cfg = getenv("FLOWLINE_CFG");
	    		if(cfg) {
	    			in.open(cfg);
	    		}
	    	}
	    	if(!in.good())
	    		THROW(ExecutionException, 
	    			"加载配置失败, 可以在当前目录下放置flowline.properties, 或者配置FLOWLINE_CFG指向配置文件");
    	}		
    	m_env.load(in);
    }

    int run( int argc, char * argv[] ) {
        //初始化
        if ( parseArgs( argc, argv ) == 0 ) {
            if ( m_args.find( "notry" ) != m_args.end()) {
            	loadEnv();
                initFactory();
                initDao();
                Engine engine(&m_env, m_dao, m_factory, m_main );
                engine.run();
                return 0;
            } else {
                try {
                	loadEnv();
                    initFactory();
                    initDao();
                    Engine engine(&m_env, m_dao, m_factory, m_main );
                    engine.run();
                    return 0;
                } catch ( const Exception & e ) {
		    g_logger.alert("告警:[%s]%s", m_main.c_str(),e.what() );
                    cout << e << endl;
                } catch ( const std::exception & e ) {
		    g_logger.alert("告警:[%s]%s", m_main.c_str(),e.what() );
                    cout << e.what() << endl;
                } catch ( int & e ) {
		    g_logger.alert("告警:[%s]%d", m_main.c_str(),e );
                    cout << "error: " << e << endl;
                } catch ( ... ) {
                    cout << "unknown" << endl;
                }
            }
        }
        return -1;
    }
};



int main( int argc, char *argv[] ) {
    Application app;
    return app.run( argc, argv );

    return 0;
}




