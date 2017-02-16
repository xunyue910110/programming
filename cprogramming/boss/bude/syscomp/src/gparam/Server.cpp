#define RIPC_IMPLEMENTATION

#include "Server.h"

#include "Session.h"
#include "Listener.h"

#include "DialogTool.h"

#include "util/Util.h" 
#include "util/cs_common.h"
#include "db/ParameterInterface.h"  


#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <vector>


const int MAX_SESSIONS = 100;


Server::Server()
{
	m_listener = NULL;

	int i;
	for (i=0; i<MAX_SESSIONS; ++i)	{
		m_sessions.push_back(new Session(this));
	}

	m_listener  = new Listener(this);	
	
	m_DbManager = new ParameterInterface();

  m_config_file = 0;
  m_section     = 0;

}

Server::~Server()
{
	vector<Session *>::iterator itr; 
	
	for (itr=m_sessions.begin();itr!=m_sessions.end();++itr) {
		delete *itr;
	}
	m_sessions.clear();


  if (m_listener != NULL) 
	  delete m_listener ;

  if (m_DbManager != NULL)
    delete m_DbManager ;
  
}

//因是server,host没有用
int Server::initialize(int debugLevel,const char * host, int port,
                       const char * config_file,const char * section)

{
    string tmp;
	setDebug(debugLevel);
  
  	m_listener_port = port;
	m_config_file   = config_file;
	m_section       = section;

	if( !m_DbManager->initialize(m_section,m_config_file)) {
	    tmp = string("data initialization fail! ") + m_section + "," +m_config_file+"\n";
        this->trace(tmp.c_str());
    	return -1;                                   
  	}
  
	m_listener->setHostName(host); 
	m_DbManager->setBufferSize(m_sessions[0]->getBufferSize());

  if (port > 0) 
    startListener(m_listener_port);
  
  return 1;

}


int Server::setDebug(int debugLevel)
{
	Manager::setDebug(debugLevel);
	m_DbManager->setDebug(debugLevel) ;
	return 1;
} 


int Server::update()
{
	return m_DbManager->updata();
}

int Server::handleRequest(const string &reqs, char *result)
{
	char *p;
	int flag = 0;
	
	p = (char *)reqs.c_str();

    flag = m_DbManager->find(p+5,result) ;
	
	return flag;
}

int Server::handleRequest(const string &reqs, string &result) 
{
	char *p;
	int flag = 0;
	result="";
  
  	p = (char *)reqs.c_str();

  	if (reqs.compare(0,5,"data:") == 0 ) {
               flag = m_DbManager->serverCommand(p+5,result) ;
		return flag ;
  	} 
  	
  	if(reqs.compare(0,11,"server:stop") == 0 ) {
  	    stop();    
  	} else if(reqs.compare(0,11,"server:show") == 0 ) {
    	char server_inf[4*1024];
    	dump(server_inf);
    	result = server_inf;   
  	} else if(reqs.compare(0,21,"server:start_listener") == 0 ) {
    	startListener(m_listener_port);
    
  	} else if(reqs.compare(0,20,"server:stop_listener") == 0 ) {
    	stopListener();
    
	} else {
		result = "unknown server command.";
    	flag = -1;
	}

	return flag;
}


int Server::checkSessions()
{
	return 0;
}
  
char* Server::dump(char* buf) 
{
  buf += sprintf(buf, "<<<--------Active Sessions --------->>>\n");
  vector<Session *>::iterator itr; 
	for (itr=m_sessions.begin();itr!=m_sessions.end();++itr) {
		if ((*itr)->m_running)
		  buf  = (*itr)->dump(buf);
	}

  buf += sprintf(buf, "<<<-------- Otherinfo--------->>>\n");
  buf=m_listener->dump(buf);
  
  return buf;
}


void input(char const* prompt, char* buf, size_t buf_size)
{
    char* p;
    do {
        printf(prompt);
        *buf = '\0';
        fgets(buf, buf_size, stdin);
        p = buf + strlen(buf);
    } while (p <= buf+1);

    if (*(p-1) == '\n') {
        *--p = '\0';
    }
}


/*------------------------------------------------------------------------------
 * follow section for lintener and dialog 
 */
void Server::startListener(const int &port)
{ 
	if (port!=0) 
	  m_listener_port = port;

  if (!m_listener->m_running )
		m_listener->start(port);
		
}

void Server::stopListener()
{  
	if (m_listener != NULL) {
    m_listener->stop();
  }
}

//
void Server::stop() 
{
	stopListener();
	stopAllSessions();
}

//s为传入参数
void Server::startSession(Socket* s) 
{ 
	Session* session=NULL;
	vector<Session *>::iterator itr;
	
  //get a session slot 
    for (itr=m_sessions.begin();itr!=m_sessions.end();++itr) {
		if (!(*itr)->m_running) {   // 空闲的session
			session=*itr;
			break;
		}
	}

	if (session==NULL) {
		this->trace("Too many incoming... \n");
		delete s;
		return ;
	}
	
  if (m_debug >= 0) {
    this->trace("startSession , session = %p\n",session);
  }
  
  session->start(s); //启动新的线程处理连接

}

void Server::stopSession(Session* session) 
{
	session->stop();
}


void Server::stopAllSessions() 
{
  vector<Session *>::iterator itr; 
  for (itr=m_sessions.begin();itr!=m_sessions.end();++itr) {
  	(*itr)->stop();
  }
}

void Server::dialog(const bool & join_thread) 
{ 
	DialogTool dialogTool((Manager*) this);
	dialogTool.start(join_thread); 
	return;
}

void Server::trace(char const* fmt, ...)
{
    //return;
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}



