
#ifndef __MANAGER_H__
#define __MANAGER_H__


#include "Config.h"


#include <string>

class Manager{
  public:
    static Manager *GetInstance(const string& mode);
      
	  Manager();
	  virtual ~Manager ();
    
	  virtual int initialize(int debugLevel,const char * host, int port,
                        const char * config_file,const char * section);
	  virtual int handleRequest(const string& reqs, string & result); 
	  
	  virtual int handleRequest(const string &reqs, char *result); 
	  
	  virtual int setDebug(int debugLevel); 
    
	  virtual int connect(char const* host, int port);
	  virtual int disconnect();
	  virtual bool isConnected();
	  virtual void stop();

	  virtual int notify();
	  virtual int update();    

  protected:  
    
    int  m_debug; 
    int  m_errcode;
    char m_errmsg[1024];
};


#endif

