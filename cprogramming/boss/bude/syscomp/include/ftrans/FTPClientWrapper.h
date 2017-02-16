
#ifndef __FTPCLIENTWRAPPER_H__
#define __FTPCLIENTWRAPPER_H__
 

#include <vector>
#include <string>
#include <map>

using namespace std;


const int ERROR_FTP_BASE             =   -2000;
                                     
const int ERROR_FTP_OTHER            =   ERROR_FTP_BASE - 1;
const int ERROR_FTP_CONNECT          =   ERROR_FTP_BASE - 2;
const int ERROR_FTP_CONNECT_TIMEOUT  =   ERROR_FTP_BASE - 3;
const int ERROR_FTP_LOGIN            =   ERROR_FTP_BASE - 4;
const int ERROR_FTP_LISTENSOCKET     =   ERROR_FTP_BASE - 5;
const int ERROR_FTP_DATACONN         =   ERROR_FTP_BASE - 6;
const int ERROR_FTP_FILE             =   ERROR_FTP_BASE - 7;
const int ERROR_FTP_FILENAME         =   ERROR_FTP_BASE - 8;
const int ERROR_FTP_GETFILE          =   ERROR_FTP_BASE - 9;
const int ERROR_FTP_RECEIVE          =   ERROR_FTP_BASE - 10;
const int ERROR_FTP_GETDATA          =   ERROR_FTP_BASE - 11;
const int ERROR_FTP_RESUME           =   ERROR_FTP_BASE - 12;
const int ERROR_FTP_DELETE           =   ERROR_FTP_BASE - 13;
const int ERROR_FTP_RENAME           =   ERROR_FTP_BASE - 14;
const int ERROR_FTP_LIST             =   ERROR_FTP_BASE - 15;
const int ERROR_FTP_IPERROR          =   ERROR_FTP_BASE - 16;
const int ERROR_FTP_INVALID_DATATYPE =   ERROR_FTP_BASE - 17;
const int ERROR_FTP_INVALID_OPTION   =   ERROR_FTP_BASE - 18;
const int ERROR_FTP_STATFILE         =   ERROR_FTP_BASE - 19;
const int ERROR_FTP_SIZE             =   ERROR_FTP_BASE - 20;
const int ERROR_FTP_OPENFILE         =   ERROR_FTP_BASE - 21;
const int ERROR_FTP_GETFILE_TIMEOUT  =   ERROR_FTP_BASE - 22;
const int ERROR_FTP_WRITEFILE        =   ERROR_FTP_BASE - 23;
const int ERROR_FTP_INVALID_CMD      =   ERROR_FTP_BASE - 24;




enum  QUERY{NO = 0, YES = 1, MAYBE = 2};       
enum  FTPOPTION{TIMEOUT, DATATYPE, RESUME, MAXWAITTIME};



/**
 * 服务器的执行命令种类
 */
enum  FTPCMDTYPE{
        CHDIR, 
        CDUP, 
        DEL, 
        LS, 
        PWD, 
        MKDIR, 
        RMDIR, 
        SYSTYPE, 
        SIZE, 
        STATCMD, 
        RENAME
       };


const int BINARY = 1;
const int AUTO   = 1;
const int ASCII  = 1;


/**
 * \class FTPClientWrapper
 * \brief FTP协议,客户端封装
 */



class FTPClientWrapper
{
  public:
    FTPClientWrapper();
    virtual ~FTPClientWrapper();
    
    /**
     * FTP登录
     * @param host 远程FTP服务器地址或名称
     * @param port FTP服务器相应的端口
     * @param user 登录用户
     * @param passwd 口令
     * @return >=0 表示成功,<0 登录失败。
     */ 
    int login(const char *host, const char *port, 
              const char *user, const char *passwd = NULL); 
    
    /**
     * 执行FTP命令 
     * @param cmd 执行的命令类型
     * @param cmdValue 命令参数
     * @param retValue 命令执行后返回的数据
     * @param retMaxSize 命令返回数据的长度
     * @return >=0 表示成功,<0 登录失败。
     */
    int executeCommand(FTPCMDTYPE cmd, const char* cmdValue = NULL, 
                       char *retValue = NULL, int retMaxSize = 0);
    
    /** 
     * 设置FTP环境选项 
     */                   
    int setOption(FTPOPTION option, int value);   
    
    /**
     * 获取列指定目录中的文件列表
     * @param dir 主机中对应的目录
     * @param files 返回相应的文件列表
     * @param pattern 匹配模式
     * @return >=0 表示成功,<0 登录失败。
     */
    int listFile(const string & dir, vector<string> & files,
                 const string & pattern);
    
    /**
     * 从主机上取得指定的文件
     * @remote 远程主机路径
     * @local 对应的本地路径,NULL 表示缺省路径
     * @return >=0 表示成功,<0 登录失败。
     */
    int getFile(const char* remote, const char* local = NULL);
    
    /**
     * 向远程主机上传输文件
     * @local 本地文件名
     * @remote 远程文件名
     * @return >=0 表示成功,<0 登录失败。
     */
    int putFile(const char* local, const char* remote = NULL);  
    
    /**
     * 推出FTP服务,中断连接
     */
    void logout() const;
    
    /**
     * 返回错误信息
     * @code 错误代码
     * @msg 错误描述消息
     */
    void getErrorText(const int code, string & msg) const;

    map<string, int> m_fileInfo;

  private:
    int m_hControlChannel; 
    int m_hListenSocket;      
    int m_hDataSocket;
    char m_replyBuffer[10240];
    /* 主机字节序 */
    int m_isBigOrder;
    /* 超时设置 */
    int m_timeOut;
    int m_isResume;
    int m_maxWaitTime;    
    bool m_226;

  private:
    int isBigOrder() const;
    int createListenSocket();
    int waitResult(int fd);
    int sendCommand(const char* command);
    int acceptSocket(int fd, struct sockaddr* addr, int* len) const;
    int writeData(int fd, char* buff, int *len) const;
    int readData(int fd, char* buff, int *len) const;
    int getLine(char* src, char** nextline) const;
};

#endif // __FTPCLIENTWRAPPER_H__
