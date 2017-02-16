

#include "Manager.h"

#include "Client.h"
#include "Server.h"
#include "API.h"


Manager::Manager()
{
	//m_debug = 0;
}

Manager::~Manager()
{
    
}


Manager * Manager::GetInstance(const string & mode) 
{
    Manager * manager = 0;
    
	if (mode == "Server") {
	    manager = new Server();		
	} else if (mode == "API") {
	    manager =  new API();
	} else {
	    manager = new Client();
	}

	return manager;
}

 

int Manager::initialize( int debugLevel,const char * host, int port,
                        const char * config_file,const char * section)
{
	return 0;
}

int Manager::handleRequest(const string &reqs, char *result)
{
	return 0;
}  
 

int Manager::handleRequest(const string &reqs, string &result)
{
	return 0;
}  
   

int Manager::setDebug( int debugLevel)
{
 	m_debug = debugLevel;
 	return 0;
} 


int Manager::connect(char const* host, int port)
{
	return 0;
}

int Manager::disconnect()
{ 
	return 0;
}

bool Manager::isConnected()
{
	return true;
}

void  Manager::stop()
{
}

int Manager::notify()
{
	return 0;
}


int Manager::update()
{
	return 0;
}  

