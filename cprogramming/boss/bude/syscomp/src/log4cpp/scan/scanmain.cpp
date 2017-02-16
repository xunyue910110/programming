#include "ExporterFileScanner.h"
#include "base/StringUtil.h"
#include <string>
#include <iostream>
#include <signal.h>
extern "C" {
#include <unistd.h>
}
#include "config-all.h"
#include "base/supdebug.h"


USING_NAMESPACE(std);
using namespace log4cpp;

volatile bool g_stop = false;

extern "C" void term_proc(int sig)
{
  g_stop = true;
  // reset signal process with myself
  signal(sig, term_proc);
  
  cout << "terminating ..." << endl;
}

int main(int argc,char* *argv) {
    if(argc < 3) {
        cout<<"Usage: [ipaddress] [port] [batch] [dirname:dirname.....]"<<endl;
        exit(-1);
    }
    string ip(argv[1]);
    int port = atoi(argv[2]);
    int batch = atoi(argv[3]);
    
    string dirnames(argv[4]);
    vector<string> dirs;
    cout << dirnames << endl;
    StringUtil::split(dirnames, ":", dirs, true);
    ExporterFileScanner scanner(ip, port, dirs, batch);

	signal(SIGTERM, term_proc);
	signal(SIGINT,  term_proc);
	
    while(!g_stop) {
    	try {
            sleep(5);            
            if(g_stop)	break;
            	
            FP_BEGIN(scan);
            scanner.scan();
            FP_END(scan);            
            
        } catch(exception& e) {
        	LOG_P1(CRITICAL, "There is a exception %s.", e.what());
	    } catch(...) {
	        LOG(CRITICAL, "Unknow error.");	        
	    }
    }
        
    return 0;
}
