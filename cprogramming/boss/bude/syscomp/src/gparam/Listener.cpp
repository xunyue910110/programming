

#include "Listener.h"
#include "Socket.h"
#include "Server.h"


Listener::Listener(Server * server)
{
	m_server = server;
	m_running = false; //服务状态
	m_comm = NULL;                 
	m_port = 0;     
	m_debug = 3;
} 

Listener::~Listener()
{
    m_thread.join();//added by cy 2004.03.28
} 


int Listener::setDebug(int & debugLevel)
{
	m_debug = debugLevel;
	return 0;
}


void thread_proc Listener::acceptThread(void* arg)
{
  ((Listener*)arg)->listen();
}

void Listener::setHostName(const string & hostName)
{
    m_hostName = hostName;
}

int Listener::start(const int &port)
{
	m_port=port;
  m_running = true;

  if (m_port>0) {
  	m_server->trace("0. Listener::start listener at %d ..\n",m_port);   
  	m_comm = Socket::create(m_hostName.c_str(), m_port,20);    
  }
  
  if (m_comm == NULL)
    return -1;
    
  m_thread.start(&acceptThread, this);
    
  return 0;
}



void Listener::listen()
{
    int i=0;
    
  while (m_running) { 
    if (m_port>0) 
      m_server->trace("6. Listener::OK startup!!\n");
    
    Socket* s = m_comm->accept();
    if (!m_running) { 
        break;
    }
    
    if (s == NULL) { 
        if (m_debug >= 1) { 
            char errbuf[1024];
            m_comm->get_error_text(errbuf, sizeof errbuf);
            m_server->trace("Accept m_comm error: %s\n" ,errbuf);
            break;
        }
    } else { 
        
        m_server->trace("Listener::OK already accept !!\n");
        m_server->startSession(s);
    }
    i++;
  }
  
  if (m_port>0) 
    m_server->trace("end listener...\n");
  
  if(m_comm != NULL)
    delete m_comm;
    
  m_comm = NULL;  
}


int Listener::stop()
{
  if (m_running ) {
    m_running = false;
    Socket* s = Socket::connect("localhost", m_port);
    delete s;
  } 
  
  if(m_comm != NULL) delete m_comm;
  m_comm = NULL;

  return 0;
}


char* Listener::dump(char* buf) 
{
  buf += sprintf(buf, "listener state:%d ", m_running);
if (m_running)
  buf += sprintf(buf, "at port:%d ", m_port);
  buf += sprintf(buf, "\n");
  return buf;
}

