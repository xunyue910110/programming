
#include "Client.h"
#include "Socket.h"
#include "util/Util.h"

#include <stdio.h>

int const MAXSIZE = 2048;


Client::Client()
{
	  m_host=0;
}

Client::~Client()
{
    disconnect();
}

void Client::stop()
{
}


int Client::initialize(int debugLevel,const char * host, int port,
                        const char * config_file,const char * section)
{
	m_debug = debugLevel;
	m_host = host;
	m_port = port;
	return true;
}
	
int Client::connect(const char * host, int port)
{
	m_host = host;
	m_port = port;

	m_comm = Socket::connect(host, port);
	if (m_comm == NULL) {
		//memset(m_errmsg,0,sizeof(m_errmsg));
        //m_errcode = m_comm->errcode;
        //m_comm->get_error_text(m_errmsg, sizeof(m_errmsg));
		return -1;
	}
  
  return 0;
}

int Client::disconnect()
{
    
	if (m_comm != NULL) { 
	  m_comm->close();
		delete m_comm;
		m_comm = NULL;
  }

	return 0;
}


bool Client::isConnected()
{
	if (m_comm == NULL)
	  return false;
	return m_comm->is_ok();
}



int Client::notify()
{
	string i_am_here="session:i_am_here";
	string result;
	handleRequest(i_am_here,result);
	if (result.compare("i_am_here")==0)
		return 0;
    return -1;
}

int Client::handleRequest(const string &reqs, char *result)
{
	char *mychar;

	int flag =0 ;
	int page =0;
	int i =0;
	char commandSize[10+1];
	unsigned long size = 0; 
	bool isRight=true;
	char param[1024];
	char tmp[MAXSIZE+2];

	if (m_comm == NULL) {
		strcpy(result,"Not connected\n");
		return -1;
	} 

	memset(result,0,sizeof(result));
	
	do {
         	     
		//----------------------------- new -----------------------------------
		sprintf(param,"%10d%s",reqs.size(),reqs.c_str());
		if (! m_comm->write(param,reqs.size()+10) ) 		// 写查询命令
	     	break;	    
	        	
		//=======================================================================
	    memset(tmp,0,sizeof(tmp));                  
	   	if (m_comm->read(tmp,1,MAXSIZE) <=0)       // 读查询结果串：长度＋结果
	     break; 
        strncpy(commandSize,tmp,10);
        commandSize[10]=0;
        size  = atoi(commandSize);	   	              

	   	if(size+10<=MAXSIZE)
	   	    tmp[size+10]=0;
	   	else
	   	    tmp[MAXSIZE]=0;
	   	strcpy(result,tmp+10);

	    page  = (size+10)/MAXSIZE;
	    flag  = (size+10)%MAXSIZE;
	    i=page;

       //读整页
       if ( page >= 1 )
       {
    	   while ( page > 1 ) 
    	   {   
    	   	 memset(tmp,0,sizeof(tmp));  
    	   	 if (! m_comm->read(tmp,MAXSIZE))
    	   	 {
    	   	 	 isRight=false;
    	   	   break;
    	   	 } 
    	   	 tmp[MAXSIZE]=0;	
    
    	   	 	strcat(result,tmp);
    	   	 page--;   
    	   }
    
    	   if(isRight==false)
    	       break;  	
    
           //读不足一页 i 是充满的页数
    
    	   if (flag!=0) 
    	   {   
    	   	 memset(tmp,0,sizeof(tmp));  
    	   	 if (! m_comm->read(tmp,(size+10)-i*MAXSIZE))
    	   	   break; 
    
    	   	 tmp[(size+10)-i*MAXSIZE]=0;	
    	   	 	strcat(result,tmp); 
    	   }  	
    	}
    
	   return 0;//成功将返回0
  } while (false);
  
  // 错误情况
  memset(m_errmsg,0,sizeof(m_errmsg));
  m_errcode = m_comm->errcode;
  m_comm->get_error_text(m_errmsg, sizeof(m_errmsg));
  	

  return -1;
} 

int Client::handleRequest(const string &reqs, string &result)
{
	int flag;
	char buffer[1024 * 32];
	
	flag = handleRequest(reqs,buffer);
	
	result = string(buffer);

	return flag;
} 






