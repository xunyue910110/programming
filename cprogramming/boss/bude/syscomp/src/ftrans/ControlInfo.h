#ifndef __CONTROLINFO_H__
#define __CONTROLINFO_H__


/**
 * \class ControlInfo
 * \brief 传输控制器配置描述信息

@code
# 从末某个服务取文件的配置取信息描述例子
 protocal_type        = FTP #FTP,DTA,FTAM,FILE
 action               = get
 compress             = gzip
 uncompress           = 
 connect_remote       = 192.168.131.129
 connect_port         = 21
 connect_user         = budp
 connect_passwd       = udp1234
 swap_path            = /app/budp/test/swap/
 
 source_count         = 2   
          
 <source0>                  
    source_path        = /app/budp/test/remote/
    backup_path        = /app/budp/test/back/
    list_in_source     = no  #yes,no
    finish_tag_file    = finish.awk     #新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    pattern_count      = 2              
    <pattern0>
       pattern         = 
       check_interval  = 100
       target_path     = /app/budp/test/local/      
       distribute_path = /app/budp/test/dist/
    </pattern0>              
    <pattern1>
       pattern         = D200*
       check_interval  = 100
       target_path     = /app/budp/test/target/      
       distribute_path = /app/budp/test/dist_new/
    </pattern1>     
 </source0>
 
 <source1>
    source_path        = /app/budp/test/remote/
    backup_path        = /app/budp/test/back/
    list_in_source     = no  #yes,no
    pattern_count      = 1
    <pattern0>
       pattern         = M200*
       check_interval  = 100
       target_path     = /app/budp/test/dist_old/      
       distribute_path = /app/budp/test/dist_old/
    </pattern0>
 </source1>

@endcode

@code

#上传文件的配置信息描述例子
protocal_type       = FTP   #FTP,DTA,FTAM,FILE
action              = put
compress            = gzip
uncompress          = gunzip
connect_remote      = 192.168.131.129
connect_port        = 21      
connect_user        = budp
connect_passwd      = udp1234        
swap_path           = /app/budp/test/swap/

source_count        = 1

<source0>
   source_path      = /app/budp/test/remote/
   backup_path      = /app/budp/test/back/
   check_interval   = 120
   target_path      = /app/budp/test/target3/
</source0>

@endcode
  
 */

#include <time.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "NAMEJScript.h"

using namespace std;

class SysParam;


class ControlInfo_get_source_pattern {
  public:
    string pattern;
    int check_interval;
    string target_path;
    string distribute_path;

  public:
    time_t m_lastTime;
    vector<string> m_queue;
    map<string,int> m_fileInfo;

  public:
    ControlInfo_get_source_pattern();
    bool checkTimeOut();
    void resetTime();
    char* dump(char* buf) const;
};


class ControlInfo_get_source {
  public:
    string source_path;
    string backup_path;
    string list_in_source;
    string finish_tag_file; //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    string change_name;
    string m_name[8];
    NAMEJScript *m_js;
    int pattern_count;
    vector<ControlInfo_get_source_pattern *> patterns;
    
  public:
    vector<string> m_queue;
    map<string,int> m_fileInfo;
    
  public:
    ControlInfo_get_source();
    ~ControlInfo_get_source();
    
    char* dump(char* buf) const;    
};


class ControlInfo_put_source {
  public:
    int check_interval;
    string source_path;
    string backup_path;
    string target_path;

  public:
    vector<string> m_queue;
    map<string,int> m_fileInfo;
    int m_lastTime;
    
  public:
    ControlInfo_put_source();
    bool checkTimeOut();  
    void resetTime(); 
    char* dump(char* buf) const;    
};





class ControlInfo {
  public:
    
    ///远程文件传输服务协议类型 FTP,FILE,FTAM,DTA
    string protocal_type;
    ///任务动作 get,put
    string action;            
    ///连接类型:long(缺省);short
    string link_type;            
    ///压缩命令
    string compress;
    ///解压缩命令
    string uncompress;
    ///远程服务主机
    string connect_remote;
    ///远程服务端口
    string connect_port;
    ///登录远程服务使用的用户
    string connect_user;
    ///登录远程服务使用的口令
    string connect_passwd;
  
    ///交换文件路径
    string swap_path;

    int source_count;
    
    ///获取去远程文件源描述。action=get时有效。
    vector<ControlInfo_get_source *> get_sources;
    ///上传文件是本地源描述。action=put时有效。
    vector<ControlInfo_put_source *> put_sources;



  public: 
    ControlInfo();
    ~ControlInfo();
    
    /**
     * 初始化配置信息
     * @param conf_file 配置文件
     * @param section_path 配置信息位置描述
     * @return >=0 表示成功,<0 表示初始化失败
     */
    int initialize(const string & conf_file, const string & section_path);

    /**
     * 转储配置信息
     */
    char* dump(char* buf) const;
  
  private:
    SysParam *m_controlSysParam;
    map<string, string> m_param;
  private:
    bool getSysParam(const string & section_path); 
    bool getGetSubSysParam(const string & section_path); 
    bool getPutSubSysParam(const string & section_path); 
    bool getGetPatternSubSysParam(const string & section_path, 
                                  ControlInfo_get_source * get_source); 
    
};


#ifdef _DEBUG_ 
#define ________________________________________________________________________________  cout << "RUNTO:" << __FILE__ << ":" <<__LINE__ <<endl;cout.flush(); 
#else
#define ________________________________________________________________________________  ; 
#endif


#endif 
