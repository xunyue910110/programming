
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>  
#include <assert.h>

#include "SessionEx.h"
#include "SessionImpl.h"
#include "Server.h"


void SessionEx::start()
{
    running = true;
    thread.start(&runThread, this);
} 

void thread_proc SessionEx::runThread(void* arg)
{
   ((SessionEx*)arg)->run();
}


SessionEx::SessionEx(Server* sv, SessionImpl* ss)
{
    server=sv;
    session=ss;
    ss->sessionex=this;

    req_queue=NULL;
    resp_queue=NULL;
    
    running=false;
    auto_running=true;
    stop=false;
    memset(client_addr,0,sizeof(client_addr));

}

void SessionEx::run()
{
  //注册当前线程对应会话session的标识
  theSession=server->getSession();
  
  //get peer name
  char* peername=session->socket->get_peer_name();
  sprintf(client_addr,"%s",peername);
  delete[] peername;

  //create two queues for comunication
  char queueName[100];                                      
  sprintf(queueName,"request/%p",session);                
  req_queue  = theSession->createQueue(queueName); 
  
  sprintf(queueName,"response/%p",session);                
  resp_queue  = theSession->createQueue(queueName);

  // thread initialize
  if (!initialize()) {;}

  if (!recovery()){;}
   
  preLoop();

  // thread enter main loop
  mainLoop();

  // thread end
  end();

  delete this;  
} 

bool SessionEx::initialize() {
    return true;
}

bool SessionEx::recovery() {
	return true;
}

void SessionEx::preLoop() {

}

int SessionEx::getRequest(CommPacket& req) {
	Object objDesc(&req,sizeof(req));
	return req_queue->get(objDesc,1000 * 60);
}

void SessionEx::onClientTimeout() {
	;
}

int SessionEx::mainLoop()
{
  while (!stop)	
  {
    CommPacket reqPack;
    if (!getRequest(reqPack)) {
      onClientTimeout();
      continue;
    }

    if (reqPack.opCode < 0) {
      // deal with system level responese
    } else {
       if (!handleRequest(reqPack))
         stop=true;
    }
  }
  
  
  
  return 0;
}

bool SessionEx::handleRequest(CommPacket& req)
{
	return true;
}  

bool SessionEx::end()
{
	return true;
}

Session* SessionEx::getSession()
{
    return (Session*) session;
}

void SessionEx::close() {
    stop=true;
    running=false;
    req_queue->close();
    resp_queue->close();
    delete theSession;
}

SessionEx::~SessionEx() {
    close();
}
