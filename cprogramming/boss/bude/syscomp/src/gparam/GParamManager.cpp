
#include <stdlib.h>

#include "Config.h"
#include "GParamManager.h"

#include "Manager.h"

#include "util/Util.h"      
#include "util/PConfigureFile.h"
#include "API.h"      

int GParamManager::run()
{
    return 1;
}

GParamManager::GParamManager()
{
	m_manager     = NULL; 
	m_debug       =  0;
  	m_host        =  0; 
	m_port        =  0;
  	m_config_file =  0;
  	m_section     =  0;
  	m_theSysParam	= new PConfigureFile; 
}

GParamManager::~GParamManager()
{
	end(); 
	delete m_manager;
	//delete m_theSysParam;
}

int GParamManager::initialize(int debugLevel,const char * host, int port,
                              const char * config_file,const char * section)
{
	
	m_debug       = debugLevel;
  	m_host        = host; 
	m_port        = port;
  	m_config_file = config_file;
  	m_section     = section;
  
	if ( strcmp(host,"")==0 ) { 
		 m_manager=Manager::GetInstance("API");
	   if (m_manager->initialize(debugLevel,host,port,config_file,section) <0) {
	   	 delete m_manager;
	   	 m_manager = NULL;
	   	 return -1;
	   }
	  
	   if (m_localDialog==true)
	       dynamic_cast<API*>(m_manager)->dialog(true);
	} else {
	  m_manager=Manager::GetInstance("Client");
	  if (m_manager->initialize(debugLevel,host,port,config_file,section) <0) {
	   	 delete m_manager;
	   	 m_manager = NULL;
		    return -1;
	  }
	   
	  if (m_manager->connect(host,port)<0) {
	   	 delete m_manager;
	   	 m_manager = NULL;
	  	 return -1;
	  }
	}    
	return 0;
}

void  GParamManager::stop()
{
    m_manager->stop();
}

int GParamManager::setApplicationID(const string & appID)
{  
	 string result;
	 return handleRequest("session:setApplicationID " + appID,result);
}


//int GParamManager::end()
//{
//	if (m_manager != NULL) {
//	  if (m_manager->isConnected())
//	    m_manager->disconnect();
//	  delete m_manager;
//	  m_manager = NULL;	
//	}
//
//	return 0;
//}

int GParamManager::handleRequest(const string &reqs, string &result)
{
    int flag,status;
    char buffer[1024 * 32];
	
	if (m_manager != NULL) {

		if( reqs.compare(0,9,"data:find") == 0) {
			flag =  m_manager->handleRequest(reqs, buffer);
			result = string(buffer);
		} else {				
	    	flag =  m_manager->handleRequest(reqs, result);
}  
   
	    return flag;
	}
	
	return -1;
}  

int GParamManager::setDebug(int debugLevel)
{
 	m_debug = debugLevel;
 	return -1;
} 

int GParamManager::connect(char const* host, int port)
{
 	if (m_manager != NULL)
 	  return m_manager->connect(host,port);
	
	return -1;
}


int GParamManager::disconnect()
{ 
 	if (m_manager != NULL)
 	  return m_manager->disconnect();
	
	return -1;
}


bool GParamManager::isConnected()
{
 	if (m_manager != NULL)
 	  return m_manager->isConnected();
	
	return false;
}



int GParamManager::notify()
{
 	if (m_manager != NULL)
 	  return m_manager->notify();
	
	return -1;
}



int GParamManager::update()
{
	if (m_manager != NULL)
 	    return m_manager->update();
 	
 	 return -1;
}  


//added for 兼容原来的接口 begin------------------------------------------------

bool GParamManager::initialization()
{
	char *env 					= getenv("BOSS_CONFIG");
	
	if(!m_theSysParam->initialization(env)) {
        return false;
    }  
    
    //added by cy for new PM begin
    string root,value;
    string remote_host;
    int    port;
    string db_configfile;
    string db_section;
    int    debugLevel;
    
	root=string("\\") + m_appName;
    //(1)
    if(! m_theSysParam->getValue(root,"remote_host",value) ) 
    {
        //return false;
        value="";  
    }
    remote_host = value; 
    //(2)
    if(! m_theSysParam->getValue(root,"port",value) ) 
    {
        //return false; 
        value=""; 
    }
    port = atoi(value.c_str()); 
    //(3)
    if(! m_theSysParam->getValue(root,"db_configfile",value) ) 
    {
        return false;  
    }
    db_configfile = value; 
    //(4)
    if(! m_theSysParam->getValue(root,"db_section",value) ) 
    {
        return false;  
    }
    db_section = value; 
    ////(5)
    //if(! m_theSysParam->getValue(root,"debugLevel",value) ) 
    //{
    //    return false;  
    //}
    debugLevel = 2 ;//atoi(value.c_str()); 
    //(6)
    if(! m_theSysParam->getValue(root,"local_dialog",value) ) 
    {
        return false;  
    }
    if( value.compare("yes")==0 || value.compare("Yes")==0 ||
        value.compare("YES")==0  ) 
        m_localDialog = true;
    else 
        m_localDialog = false;
    
	if ( initialize(debugLevel,remote_host.c_str(),port,
                         db_configfile.c_str(),db_section.c_str()) != 0 )
    {
        return false;
    }
    //added by cy for new PM end
    
    delete m_theSysParam;
    
    return true;
}

void GParamManager::setDocalDialog(const string &Dialog)
{
    if( Dialog.compare("yes")==0 || Dialog.compare("Yes")==0 ||
        Dialog.compare("YES")==0  ) 
        m_localDialog = true;
    else 
        m_localDialog = false;
}
    

void GParamManager::setHostName(const string hostName)
{
    m_hostName=hostName;
}
void GParamManager::setAppName(const string appName)
{
    m_appName=appName;
}
void GParamManager::setChannelNo(const int channelNo)
{
    m_channelNo=channelNo;
}

int GParamManager::end()
{
	  return  disconnect();
} 
bool GParamManager::needUpdate()
{
	  return update();
}
bool GParamManager::needUpdateForRate()
{
	  return update();
}


//added for 兼容原来的接口 end--------------------------------------------------

string GParamManager::getFieldValue(int line,int fieldId)
{
    return m_fields[line*m_fieldCount + fieldId + 2];
}

string GParamManager::getFieldValue(const vector<string> &fields,int line,int fieldId)
{
    int fieldCount = atoi(fields[1].c_str());
    
    return fields[line*fieldCount + fieldId + 2];
}

int GParamManager::getRecvline()
{
    m_count = 0;
    m_count = 0;
    
    if( handleRequest(m_command.c_str(),m_recvline) < 0 ) {
    	return -1; 
	} 	

	while (true) {
	    if( m_recvline.compare(0,9,"error:-10") == 0 ) {
	    	string str = m_recvline.substr(9);
	    	m_recvline = str;
	    	sleep(1);
            if( handleRequest(m_command.c_str(),m_recvline) < 0 ) {
    	        return -1; 
	        } 	
	    } else {
	        break;
	    } 
    }
	
   	split(",",m_recvline.c_str(),true,m_fields); 
   	if( m_fields.size() >= 2 ) {
   	    m_count         = atoi(m_fields[0].c_str());
   	    m_fieldCount    = atoi(m_fields[1].c_str());
   	}

    return m_count;     // 已经找到
}

int GParamManager::getRecvline(const string &cmd, vector<string> &fields)
{	
    string recvline;
		
    return getRecvline(cmd,fields,recvline);
}	

int GParamManager::getRecvline(const string &cmd, vector<string> &fields,string &recvline)
{	
	char *p;
	
 	if( handleRequest(cmd.c_str(),recvline) < 0 ) {
        return -1; //有错误！	
	} 
    
	if( recvline.compare(0,9,"error:-10") == 0 ) {
		p = (char *)recvline.c_str() + 9;
		recvline = p;
		sleep(1);
        return getRecvline(cmd,fields,recvline);
		
		return -2;	// 正在刷新参数
	} else if( recvline.size() == 2 ) {
		return 0;	// 没有找到
	} 
   	split(",",recvline.c_str(),true,fields);   
 
    return atoi(fields[0].c_str());     // 已经找到
}




