/**
 *  通用参数服务借口 GParamManager。功能相当于BOSS1.0 中 ParamManager
 *    
 *   
 */


#ifndef __GPARMMANAGER_H__
#define __GPARMMANAGER_H__

#include "Config.h"
#include <string>
#include <vector>  
#include <pthread.h>
USING_NAMESPACE(std)


class Manager;
class PConfigureFile;
class API;
class GParamManager {
  public:
	GParamManager();
	virtual ~GParamManager  ();
	  
  	//added for 兼容原来的接口
	void setHostName(const string hostName);
	void setAppName(const string appName);
	void setChannelNo(const int channelNo);
	
    //bool end();
	bool needUpdate();
    bool needUpdateForRate();
    bool initialization();
    
    void setDocalDialog(const string &Dialog);
    
    virtual int run();

	virtual int  initialize(int debugLevel,const char* host, int port,
	                          const char *config_file,const char *section);
    
    virtual int  setApplicationID(const string & appID);
    virtual int  end(); 

	virtual int  handleRequest(const string &reqs, string &result); 
	virtual int  setDebug(int debugLevel); 
	

	virtual int  connect(char const* host, int port);
	virtual int  disconnect();
	virtual bool isConnected();

	virtual int  notify();
	virtual int  update();   
	virtual void  stop();   
	

    virtual int getRecvline();     
    virtual int getRecvline(const string &cmd, vector<string> &fields);
    virtual int getRecvline(const string &cmd, vector<string> &fields,string &recvline);
    
    string getFieldValue(int line,int fieldId);
    string getFieldValue(const vector<string> &fields,int line,int fieldId);
	  
  	vector <string> m_fields;
  	string          m_command;
  	
  	int m_count;
  	int m_fieldCount;
  	

	   

  protected:
	Manager * m_manager; 

    int           m_debug; 
	const char *  m_host; 
	int           m_port;
    const char *  m_config_file;
    const char *  m_section;
    
    PConfigureFile *m_theSysParam;

  	//added for 兼容原来的接口
	string m_hostName; //记日志时用到
	string m_appName;  //记日志及确定源文件用到
	int    m_channelNo;//记日志及确定源文件用到
	bool   m_localDialog;  //是否启动本地交互

  	string          m_recvline;   

};

#endif

