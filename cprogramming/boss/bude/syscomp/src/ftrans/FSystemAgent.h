
#ifndef __TRANSFER_H__
#define __TRANSFER_H__

/**
 * \class FSystemAgent FSystemAgent.h
 * \brief 文件系统代理(File System Agent).
 * 
 * 该文件系统代理是一个抽象类.通过不同的具体实现来完成不同协议文件传输功能。
 *  
 * 该系统代理向外界提供一个静态工厂方法，通过该方法调用者得到相应协议的实现。
 * 其中定义的各个方法根据不同的实现对应的功能含义有所差异。 
 */



#include <vector>
#include <string>
#include <map>

using namespace std;


enum FILEIMPL_OPTION{MAXTRANSFILE};

class FSystemAgent {
  public:
    virtual ~FSystemAgent(){}
    
    /**
     * 初始化
     */
    virtual int initialize() = 0;

    /**
     * 建立远程连接,如果FILE该方法无实际意义
     * @param host 远程服务器地址或名称
     * @param port 远程P服务器相应的端口
     * @param user 登录用户
     * @param passwd 口令
     * @return >=0 表示成功,<0 登录失败。
     */	     
    virtual int connect(const string & host, const string & port,
                        const string & user, const string & passwd) = 0;
    
    /**
     * 断开远程连接,,如果FILE该方法无实际意义
     */                        
    virtual int disconnect() = 0;
    
    /**
     * 取得文件
     * @param dest 目标文件名
     * @param souce 源文件名
     * @return >=0 成功,<0 失败。
     */
    virtual int getFile(const string & source, const string & dest) = 0; 
    
    /**
     * 上传文件
     * @param dest 目标文件名
     * @param souce 源文件名
     * @return >=0 成功,<0 失败。
     */
    virtual int putFile(const string & dest, const string & source) = 0;
    
    /**
     * 设置传输模式,0
     * @param  mode 表示二进制,1 ASCII 方式 
     * @return >=0 成功,<0 失败。
     */
    virtual int setMode(const int & mode) = 0;

    /**
     * 设置环境选项或模式
     * @param type 选项类型
     * @param value 选项值
     * @return >=0 成功,<0 失败。
     */
    virtual int setOption(const int & type, const int & value) = 0;

    /**
     * 改变当前目录
     * @param dir 目标目录
     */
    virtual int changeDirectory(const string & dir) = 0;
    
    /**
		 * 获取列指定目录中的文件列表
		 * @param dir 主机中对应的目录
		 * @param files 返回相应的文件列表
		 * @param pattern 匹配模式
		 * @return >=0 表示成功,<0 失败。
		 */
    virtual int listDirectory(const string & dir, vector<string> & files,
                              const string & pattern) = 0;
    
    /**
     * 取得文件信息
     * @param file 文件名
     * @param size 文件大小
     * @return >=0 成功,<0 失败。
     */                              
    virtual int getFileInfo(const string & file, unsigned long & size) = 0;
    /**
     * 取得文件时间  //新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
     * @param file 文件名
     * @param time 文件大小
     * @return >=0 成功,<0 失败。
     */                              
    virtual int getFileTime(const string & file, time_t & time) = 0;    
    /**
     * 删除的文件
     * @param file 文件名
     * @return >=0 成功,<0 失败。
     */

    virtual int delFile(const string & file) = 0;
    
    /**
     * 文件改名
     * @param old_file 原来文件名
     * @param new_file 新文件名
     */
    virtual int rename(const string & old_file, const string & new_file) = 0;         

    /**
     * 结束
     */
    virtual int end() = 0;
    
		/**
		 * 返回错误信息
		 * @code 错误代码
		 * @msg 错误描述消息
		 */
    virtual void getErrInfo(int & code, string & msg) = 0;  
    
    /**
     * 工厂方法得到不同协议实现
     * @param type 协议类型 FTP,FILE,FTAM,DTA等。
     * @return 返回FSsystemAgent 指针.
     */
    static FSystemAgent* getInstance(const string & type);

    int m_errorCode;
  protected:
    int m_maxFileLimit; 

};

#endif //__TRANSFER_H__
