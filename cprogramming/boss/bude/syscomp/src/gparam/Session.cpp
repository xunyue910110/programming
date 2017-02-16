#define RIPC_IMPLEMENTATION


#include "Session.h"
#include "Server.h"
#include "errno.h"


pthread_key_t CurrentThread::key;


void Session::reset()
{
	m_running = false;
  m_last_active = 0;
	m_clientID = "";
	m_peer_name = "";
}


Session::Session(Server* server)
{
  this->m_server = server;
  this->m_comm = NULL;
  
  m_bufferSize = 1024 * 32;
  m_buffer = new char[m_bufferSize];
  
  reset();
}

int Session::getBufferSize()
{
	return m_bufferSize;
}

Session::~Session() 
{
	delete [] m_buffer;
}


//comm为传入参数
void Session::start(Socket* comm)
{
    m_running = true;
    m_comm =comm;

    char* p = m_comm->get_peer_name();
    m_peer_name=p;
    delete p;
    
   thread.start(&runThread, this);
}

void thread_proc Session::runThread(void* arg) 
{
    ((Session*)arg)->run();
}


void Session::run() 
{ 
	 char errmsg[256];
	  int n,ret,mm,size;
	  char respSize[11];  
	  char recvline[MAXSIZE];   
	  string resultBuff,tmp;  
	  const char *p;
	  char commandSize[10+1];

  while (m_running) {
    
    // read a line as command string to server
    //----------------------------- old -----------------------------------
	  //if( (n=m_comm->readline(recvline,MAXSIZE)) <= 0 ) {
	  //  if( n < 0 ) {
	  //	    m_comm->get_error_text(errmsg, sizeof(errmsg));
	  //      this->trace("Failed to read a line: errcode%d: %s\n" ,m_comm->errcode,errmsg);
	  //  }
	  //	break;				 
	  //}		  
	  //recvline[n-1]=0; 
	//----------------------------- new -----------------------------------
FP_BEGIN(readline); 
    resultBuff = "";
		memset(recvline,0,sizeof(recvline));  
	   	if (m_comm->read(recvline,1,MAXSIZE) <=0) 				// 读查询命令
	     	break;  

	   	if( strlen(recvline)	!=0 ) 	{       	  	
          int i;
          strncpy(commandSize,recvline,10);
          commandSize[10]=0;
	   	    size  = atoi(commandSize);	   	              
	    }
	   	recvline[size+10]=0;
	   	tmp = recvline+10;
//      sprintf(recvline,tmp.c_str());
      memset(recvline,0,sizeof(recvline));
      strcpy(recvline,tmp.c_str());


FP_END(readline); 	     	     			  
	   //=======================================================================

	  // process a request from the client
      p = resultBuff.c_str();	
      
	  if( strncmp(recvline,"data:find",9) == 0 ) {
	    	if ( (ret=handleRequest(recvline,m_buffer)) < 0) {
	    		sprintf(errmsg,"%d ",ret);
	    		resultBuff = string("error:") + errmsg + resultBuff; 
        		sprintf(errmsg,"Request %s\n",resultBuff.c_str());
	    		this->trace(errmsg);	
	    	    p = resultBuff.c_str();	
	    	} else {
	    	    p = (const char *)  m_buffer;	
	      }
	  } else {  
	    if ( (ret=handleRequest(recvline,resultBuff)) < 0) {
	    	    sprintf(errmsg,"%d ",ret);
	    	    resultBuff = string("error:") + errmsg + resultBuff; 
        	    sprintf(errmsg,"Request %s\n",resultBuff.c_str());
	    	    this->trace(errmsg);
	  	}
	  	p = resultBuff.c_str();	
	  }

      // wirte the result message to pear 
	  sprintf(respSize,"%10d",strlen(p));  
      tmp=respSize;
      tmp=tmp+p;
      p=tmp.c_str();
	  if (! m_comm->write(p,strlen(p))) {
	  	m_comm->get_error_text(errmsg, sizeof(errmsg));
	    this->trace("Failed to write errcode:%d: %s\n", m_comm->errcode, errmsg);	    
	    break;
	  }
  }
	
	delete m_comm;
	m_comm = NULL;

	reset();

}

int Session::handleRequest(const string &reqs, char *result)
{
	return m_server->handleRequest(reqs,result) ;
} 

int Session::handleRequest(const string &reqs, string &result) 
{
	time(&m_last_active);

	int flag  = 0;
	result="";
  
  if (reqs.compare(0,5,"data:") == 0)  {
 		flag =  m_server->handleRequest(reqs,result) ;
 		
 		return flag;
  } 
  
  if (reqs.compare(0,7,"server:") == 0)  {
      // TODO:server level commands 
 		if (flag=m_server-> handleRequest(reqs,result) < 0) {
		  	flag = -1;
		}
		return flag;
  } 
  
  if( reqs.compare(0,8,"session:") == 0 ) {
      // TODO:session level commands
      if (reqs.compare(0,24,"session:setApplicationID") == 0) {
		     m_clientID=reqs.c_str()+25;

      } else if (reqs.compare(0,20,"session:getSessionID") ==0 ){
      	char session_id[256]; sprintf(session_id,"%p",this);
      	result=session_id;
      
      } else if (reqs.compare(0,22,"session:getSessionInfo") ==0 ){
      	char session_inf[2048];
      	dump(session_inf);
      	result = session_inf;
      
      } else if (reqs.compare(0,17,"session:i_am_here") ==0 ){
        result = "i_am_here";
      } else if (reqs.compare(0,13,"session:close") ==0 ){
      	m_running = false;

      } else {
      	result = "unknown session command.";
      	flag = -1;
      }

	} else  {
    	result = "unknown command: "+ reqs;
    	flag = -1;
	}
  
	return flag;
}


void Session::stop()
{
  m_running = false;
  if (m_comm != NULL ) {
  	m_comm->shutdown();
  }
}


char* Session::dump(char* buf)
{
    buf += sprintf(buf, "ID:%p ", this);
    buf += sprintf(buf, "get_peer_name:%s ", m_peer_name.c_str());
    buf += sprintf(buf, "client:%s ", m_clientID.c_str());
    buf += sprintf(buf, "last active:%d ", m_last_active);
    buf += sprintf(buf, "\n");
    return buf;
}



bool Session::isOk()
{
    return m_comm == NULL ? true : m_comm->is_ok();
}

void Session::getErrorText(char* buf, size_t bufSize)
{
    if (m_comm == NULL) { 
        strncpy(buf, "ok", bufSize);
    } else { 
        m_comm->get_error_text(buf, bufSize);
    }
}



void Session::trace(char const* fmt, ...)
{
	m_server->trace("SESSION:%p ",this); 
  m_server->trace(fmt);
}
