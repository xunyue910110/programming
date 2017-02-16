#include "log4cpp/ExporterAppender.h"
#include "base/StringUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <errno.h>
#include <fstream>
#include <memory>
#include "hessian/hessian_output.h"
#include "hessian/hessian_input.h"
#include "hessian/string_input_stream.h"
#include "hessian/dump.h"

USING_NAMESPACE(std);
using namespace log4cpp;
using namespace hessian;

static mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

class WriteLockVar {
    int m_fd;
    
    int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
        struct flock lock;
    
        lock.l_type = type;
        lock.l_start = offset;
        lock.l_whence = whence;
        lock.l_len = len;
        
        return(fcntl(fd, cmd, &lock));
    }
public:    
    WriteLockVar(int fd): m_fd(fd) {
        if(lock_reg(m_fd, F_SETLKW, F_WRLCK, (off_t)0, SEEK_SET, (off_t)0) == -1)
            cerr << "锁文件失败" << endl;
    }
    
    ~WriteLockVar() {
        lock_reg(m_fd, F_SETLK, F_UNLCK, (off_t)0, SEEK_SET, (off_t)0);
    }
};

class file_input_stream : public input_stream {
public:
    /// Constructor.
    file_input_stream(const string& filename) throw(io_exception) {
        in.open(filename.c_str());
        if(!in) {
            throw io_exception("file_input_stream constructor : no such file" + filename);
        }
    }
    /// Destructor.
    ~file_input_stream() throw() {
        in.close();
    }
    /// EOF test.
    bool eof() throw(io_exception) {
        return in.eof();
    }
    /// Reads next character from stream.
    int read() throw(io_exception) {
        if (eof()) {
            throw io_exception("file_input_stream::read(): read past end of stream");
        }
        return in.get();
    }
private:
    ifstream in;
};


typedef map<string, Object*> hessianMap;
typedef map<string, list<wrappers::Object*>*>::value_type processType;


ExporterAppender::ExporterAppender() {}

void ExporterAppender::init(const map < string , string > & configuration) {
    map < string, string >::const_iterator it = configuration.find("localcode");
    if(it!=configuration.end()) {
        m_localCode = it->second;    
    } else {
        m_localCode = "GB18030";
    }
    
    it = configuration.find("workdir");
    if(it != configuration.end()) {
        m_workdir = (*it).second;
    } else {
        cerr<<"没有设置ExporterAppender的工作目录，使用当前目录"<<endl;
        m_workdir = "./logs";
    }
    
    if(m_workdir.substr(m_workdir.size()-1) != "/") {
        m_workdir += "/";
    }
    
    makeDirectory(m_workdir.c_str());
    
    m_errordir = m_workdir + "error";
    makeDirectory(m_errordir.c_str());
    
    m_processdir = m_workdir + "process";
    makeDirectory(m_processdir.c_str());
    
    m_alertdir = m_workdir + "alert";
    makeDirectory(m_alertdir.c_str());
}

void ExporterAppender::makeDirectory(const string& dir) {
    static mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;    
    if(access(dir.c_str(), F_OK)) {
        mkdir(dir.c_str(),mode);
        chmod(dir.c_str(),mode);
    }
}

//此方法作废, 不再使用. 因为touch和下面的open不是原子操作, 造成两个操作之间如果采集程序在此之间改名
//造成下面的open失败, 导致数据丢失
void ExporterAppender::touchFile(const string& filename) {

    if(access(filename.c_str(), F_OK)) {
    	static int flag = O_CREAT | O_TRUNC | O_EXCL;
    	int fd = open(filename.c_str(), flag , mode);
    	if(fd >=0) {
    		close(fd);
    		chmod(filename.c_str(),mode);
    	}
    }
}

/**
 * 持久化告警信息至本地
 */
void ExporterAppender::persistAlert(const string& dn, const char* data, size_t size) { //TODO
    string alertfile = m_alertdir + "/" + dn + ".alert.w";
    //touchFile(processfile);
    int fd = open(alertfile.c_str(), O_CREAT | O_WRONLY | O_APPEND, mode);

    WriteLockVar lock(fd);
    ::write(fd, data, size);
    close(fd);
}

string ExporterAppender::getIncompletedFile(const string& dn, const string& pid) {
    return m_processdir + "/" + dn + "-" + pid + ".incom";
}

/**
 * 一个批处理结束时的动作，把该批处理的持久文件的后缀从incom改为com,并发送该文件
 * @param pid
 */
void ExporterAppender::endProcess(const string& dn, const string& pid) { //TODO
    string processingfile = getIncompletedFile(dn, pid);
    string processedfile = m_processdir + "/" + dn + ".process.w";
    
	auto_ptr<input_stream> is (new file_input_stream(processingfile.c_str()));
    hessian_input in(is, m_localCode.c_str());
    list<Object*> pList;
    try {
    	while(true) {
      	    pList.push_back(in.read_object());
      	}
    } catch(io_exception& e) {
      	//cout<<"hessian exception in TcpAppender::checkCompletedProcess()"<<e.what()<<endl;
    }
    
    List trans(pList);
    if(pList.size() > 2){
       //如果不是空的事务
       string buffer;
       hessian_output out(m_localCode.c_str());
       out.write_object(buffer,&trans);
       
       //touchFile(processedfile);
       int fd1 = open(processedfile.c_str(), O_CREAT | O_APPEND | O_WRONLY, mode);
       WriteLockVar lock(fd1);		
       ::write(fd1, buffer.c_str(), buffer.size());   
       close(fd1);
    } 
    
    unlink(processingfile.c_str());
}

/**
 * 持久化批处理信息至文件，文件名为${pid}.${tag}的格式, pid为批处理的id,tag为文件后缀(incom表示批处理未完成)
 * @param pid 批处理的id
 * @param content 
 */
void ExporterAppender::persistProcess(const string& dn, const string& pid, const char* data, size_t size) {
    string processfile = getIncompletedFile(dn, pid);
    //touchFile(processfile);
    int fd = open(processfile.c_str(), O_CREAT | O_APPEND | O_WRONLY, mode);

    ::write(fd, data, size);
    close(fd);
}


void ExporterAppender::write(const char* data, size_t size) {
    const string content(data, size);
    auto_ptr<input_stream> is (new string_input_stream(content));

    hessian_input in(is,m_localCode.c_str());

    Map* object = static_cast<Map*>(in.read_object());
    hessianMap& value = object->value();
    auto_ptr<Map> destoryer(object);

    String* type = static_cast<String*>(value["log_type"]);    
    String* dn = static_cast<String*>(value["log_dn"]);
    if(type->value() == "PROC") {
        String* pid = static_cast<String*>(value["log_pid"]);
        String* process = static_cast<String*>(value["log_progress"]);
        persistProcess(dn->value(), pid->value(), data, size);
        if(process->value() == "END") {
            endProcess(dn->value(), pid->value());
        }
    } else if(isAlert(type->value())){
        persistAlert(dn->value(), data, size);
    }
}

bool ExporterAppender::isAlert(const string& type){
	if((type == "WARNING") || (type == "MINOR") || (type == "MAJOR") || (type == "CRITICAL") || (type == "INDETERMINATE"))
		return true;
	return false;
}

ExporterAppender::~ExporterAppender() throw() {
}
