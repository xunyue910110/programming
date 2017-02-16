#include <stdio.h>
#include <string.h>


#include "Server.h"

#include "SessionEx.h"
#include "Object.h"

class SessionExImpl :public SessionEx //
{
  public: 
    struct QueueElement {
	int    ivalue;
	char   svalue[64];
    };
    
    SessionExImpl(Server* sv, SessionImpl* ss)
    : SessionEx(sv,ss) { };
    
    ~SessionExImpl(){};

    void run() {
       SessionEx::run();

       char queueName[20];                                      
       sprintf(queueName,"listener/%p",session);                
       Queue* reqQueue  = session->createQueue(queueName); 
       sprintf(queueName,"echo/%p",session);                
       Queue* resQueue  = session->createQueue(queueName)


       QueueElement element;
       while (running)
       {
       	   Object objDesc(element,sizeof(QueueElement));
           clientReqQueue->get(objDesc);    //thread waiting for a new element coming.

              string query(element.svalue;)
              string result;
              if (server->param->find(query,result) > 0)
               {
               	  Object objRDesc(result.c_str(),result.length()); //长度有问题
              	  resQueue->put(objRDesc);  
               }
       }
     }	
};




class MyServer :public Server {
  public: 
    MyServer(int port, int debugLevel)
    :Server(port,debugLevel) {};

    //启动一个
    void startSession(SessionImpl* session) {
      Server::startSession(session);
      SessionExImpl* sessionex = new SessionExImpl(this,session);
      sessionex->auto_running=true;
    }

    //停止一个Session
    void stopSession(SessionImpl* session) {
      Server::stopSession(session);
      delete session->sessionex;
    }

    FunctionInterface  param;
    int id;
};



int main(int argc, char* argv[])
{ 
    int debug = Server::DEFAULT_DEBUG_LEVEL;
    bool isDaemon = false;
    int port = 0;
    for (int i = 1; i < argc; i++) { 
	if (*argv[i] == '-') { 
	    if (strcmp(argv[i] + 1, "d") == 0) { 
		isDaemon = true;
	    } else if (strcmp(argv[i] + 1, "debug") == 0 && i+1 < argc) { 
		if (sscanf(argv[i+1], "%d", &debug) != 1) { 
		    printf("Integer debug level is expected\n");
		    return 1;
		}
		i += 1;
	    } else { 
		printf("Unknown option %s\n", argv[i]);
		return 1;
	    }
	} else if (port != 0) {
	    printf("Syntax: Server [-d] [-debug N] PORT\n");
	    return 1;
	} else { 
	    if (sscanf(argv[i], "%d", &port) != 1 || port <= 0) { 
		printf("Invalid port value: %s\n", argv[i]);
		return 1;
	    }
	}	
    }

    if (port == 0) { 
	printf("Syntax: Server [-d] [-debug N] PORT\n");
	return 1;
    }

    MyServer* server = new MyServer(port, debug);
    Session* localSession =server->getSession();
    localSession->createQueue("listener/0x0000000");
    if (isDaemon) { 
	//server->run();
    } else {	
	server->start();
	server->dialog();
    }
    return 0;
}

