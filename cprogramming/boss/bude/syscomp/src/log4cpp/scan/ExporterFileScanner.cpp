#include "ExporterFileScanner.h"
#include <dirent.h>
#include <assert.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <sys/stat.h>
#include <list>
#include "cppsocket/tcpclient.h"
#include "hessian/wrappers.h"
#include "config-all.h"
#include "hessian/hessian_output.h"
#include "hessian/hessian_input.h"
#include "hessian/string_input_stream.h"
#include "hessian/dump.h"
#include "cppsocket/stringbuffer.h"


USING_NAMESPACE(std);
using namespace hessian;
using namespace CPPSocket;

extern volatile bool g_stop;

namespace log4cpp {

const string PROCESS_DIR = "process";
const string ALERT_DIR = "alert";
const string ERROR_DIR = "error";
const char DIR_DELIM = '/';
static string localcode = getenv("LOCODE")? getenv("LOCODE"): "GB18030";
static int log_level = getenv("SCAN_LOG_LEVEL")? atoi(getenv("SCAN_LOG_LEVEL")) : 1;
static int log_count = getenv("SCAN_LOG_COUNT")? atoi(getenv("SCAN_LOG_COUNT")) : 2;
char agent_home[128] = "..";

void _log(bool isShort, const char* filename, int line, int grade, const char* msg, ...) {
  static const char * names[] = {"DEBUG","INFO","WARNING", "MINOR", "MAJOR", "CRITICAL"};
  FILE *fp;
  va_list ap;
  struct tm* tm;
  time_t current;
  char temp1[256],temp2[256], file[256];
  struct stat filestat;

  /*如果级别不够，放弃*/
  if(grade < log_level) return;

  current  = time(NULL);
  sprintf(file, "%s/logs/scan.log", agent_home);
  stat(file, &filestat);
  
  if(filestat.st_size > 2*1024*1024) {
    for(int i=log_count; --i >= 1; ) {
      sprintf(temp1, "%s/logs/scan.log.%d", agent_home, i);
      sprintf(temp2, "%s/logs/scan.log.%d", agent_home, i+1);
      if(i == log_count - 1) {
        if(access(temp1, F_OK) == 0)
          unlink(temp1);    
      }  
      else {
        if(access(temp1, F_OK) == 0)
          rename(temp1, temp2);
      }  
    }
    if(log_count >= 2)
      rename(file, temp1);
    else
      unlink(file);
  }

   tm = localtime(&current);
   va_start(ap, msg);
   fp = fopen(file, "a");
   if(!isShort)
      fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d[%s]%s:%d ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, names[grade], filename, line);
   vfprintf(fp, msg, ap);
   fputc('\n', fp);
   fclose(fp);
   va_end(ap);
}



class file_input_stream : public input_stream {
public:
    /// Constructor.
    file_input_stream(const string& filename) throw(io_exception) {
        pos = 0;        
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
        int c = in.get();
        
        if(c == EOF) {
           throw io_exception("file_input_stream::read(): read past end of stream");
        }
        
        pos ++;
          
        return c;
    }
    
    size_t pos;
    ifstream in;
};

class SocketInputStream : public input_stream {
public:
    SocketInputStream(TCPConnection& con) : m_connnection(con), m_cursor(0), m_eof(false) {}
    /// Read a character from the stream.
    int read() throw(io_exception) {
        try {
            string& str = m_buffer;
            if(m_cursor >= str.size()) {
                m_cursor = 0;
                int n = m_connnection.receive(m_buffer);
                if(n <= 0) {
                    m_eof = true;
                    throw io_exception("socket_input_stram::read(): read past end of stream");
                }
            }
            return str[m_cursor++];
        } catch(CPPSocket::Exception& e) {
            throw io_exception(string("SocketInputStream::read") + e.getMessage());
        }
    }
    /// Test stream for EOF.
    bool eof() throw(io_exception) {
        return m_eof;
    }
    /// Destructor.
    ~SocketInputStream() throw() {
        ;
    }
private:
    TCPConnection& m_connnection;
    StringBuffer m_buffer;
    size_t m_cursor;
    bool m_eof;
};

class LogDir {
    string m_dir;
    string m_errordir;
    string m_method;
    list<Object*> m_tosend;
    size_t m_sendlength;
    map<string, list<Object*> >m_sendfiles;
    string m_remote_host;
    int m_port;
    int m_batch;

public:
    LogDir(const string& host, int port, const string& dir, const string& errordir, const string& method, int batch)
            : m_remote_host(host), m_port(port),m_dir(dir), m_errordir(errordir), m_method(method), m_batch(batch), m_sendlength(0) {}

    void scan() {
      m_tosend.clear();
      m_sendfiles.clear();
      m_sendlength = 0;
        do_dir_foreach(".incom", &LogDir::timeout_rollback);
        do_dir_foreach(".r", &LogDir::cut_to_sfiles);
        do_dir_foreach(".s", &LogDir::send);
        do_dir_foreach(".w", &LogDir::rename_to_rfiles);
    }

    void do_dir_foreach(const char* suffix,
                        bool (LogDir::*p)(const string&, bool, struct stat&)) {
    int suffixLen = strlen(suffix);
        DIR* dirp = opendir(m_dir.c_str());
        if(dirp == NULL) {
            LOG_P3(CRITICAL, "do_dir_foreach:cant open process dir %s, code=%d, msg=%s",
              m_dir.c_str(), errno, strerror(errno));
            return;
        }

    REWINDDIR:
    rewinddir(dirp);
        try {
            struct dirent *dp;
            string lastfile;
            struct stat filestat;            
            
            lastfile.clear();
            while((dp = readdir(dirp)) != NULL) {
                int len = strlen(dp->d_name);
                if(len >=suffixLen) {
                    if(strcmp(dp->d_name+len-suffixLen, suffix) == 0) {
                        char buffer[1024];
                        sprintf(buffer, "%s%s", m_dir.c_str(), dp->d_name);
                        if(lastfile.size() ==0) {
                            lastfile = buffer;
                        } else {
                            if(stat(lastfile.c_str(), &filestat) == 0) {
                              if((this->*(p))(lastfile, false, filestat))
                                goto REWINDDIR;                              
                            } 
                            lastfile = buffer;
                        }
                    }
                }
            }
            if(lastfile.size() != 0) {
                if(stat(lastfile.c_str(), &filestat) == 0) {
                  (this->*(p))(lastfile, true, filestat);
                }
            }

            closedir(dirp);
        } catch(...) {
            closedir(dirp);
            throw;
        }
    }

    bool timeout_rollback(const string& file, bool isDirEnd, struct stat& filestat) {
        assert(*file.rbegin() == 'm'); //.incom

        if(time(0) - filestat.st_mtime > 3600 * 24) {

            LOG_P1(DEBUG, "Rollback %s.", file.c_str());

            if(unlink(file.c_str()) != 0)
              throw io_exception(string("rollback: remove ") + file + " fail:" + strerror(errno));
        }

        return false;
    }

    bool cut_to_sfiles(const string& file, bool isDirEnd, struct stat& filestat) {
        list<Object*> frag;
        vector<string> ofiles;
        int filenum = 0;
        bool isEnd = false;

        LOG_P1(DEBUG, "Split %s", file.c_str());

        assert(*file.rbegin() == 'r');
        try {

            struct timeval current;
            gettimeofday(& current, NULL);
      
            //估计一个对象的记录为64字节， 如果文件的大小小于 64*batch, 则直接链接过去。
            if(filestat.st_size < 64 * m_batch) {
              ostringstream ofilename;
                ofilename<<file.substr(0, file.size() -2)<<"-"<<current.tv_sec<<current.tv_usec<<"-0.s";
                ofiles.push_back(ofilename.str());
                
                LOG_P2(DEBUG, "Rename %s to %s", file.c_str(), ofilename.str().c_str());
                if(link(file.c_str(), ofilename.str().c_str()) != 0)
                  throw io_exception(string("cutfile: link ") + file + " to " + ofilename.str() + " fail:" + strerror(errno));

            } else {
              file_input_stream* fis = new file_input_stream(file);
              auto_ptr<input_stream> is (fis);
              hessian_input in(is,localcode.c_str());
              size_t lastpos = 0;
              do {
                  try {
                      Object* obj = in.read_object();
                      log_process(obj);
                      frag.push_back(obj);
                  } catch(io_exception& e) {
                      isEnd = true;
                  }

                  if(frag.size() > 0 && (isEnd || frag.size() >= m_batch || ((size_t)fis->pos) - lastpos > 1024*100)) {
                      LOG_P2(DEBUG, "Position %s:%d", file.c_str(), fis->pos);
                     
                      ostringstream ofilename;
                      ofilename<<file.substr(0, file.size() -2)<<"-"<<current.tv_sec<<current.tv_usec<<"-"<<filenum++<<".s";
                      ofiles.push_back(ofilename.str());

                      //如果文件读完毕, 并且目前只要一个文件的话, 直接改名
                      if(isEnd && filenum ==1) {
                          LOG_P2(DEBUG, "Rename %s to %s", file.c_str(), ofilename.str().c_str());
                          if(link(file.c_str(), ofilename.str().c_str()) != 0) {
                            throw io_exception(string("cutfile: link ") + file + " to " + ofilename.str() + " fail:" + strerror(errno));
                          }
                      } else {
                          LOG_P1(DEBUG, "Write %s", ofilename.str().c_str());
                          write_to_sfile(ofilename.str(), frag);
                      }

                      List destoryer(frag);
                      frag.clear();
                      lastpos = fis->pos;
                  }
              } while(!isEnd);
      }
            LOG_P1(DEBUG, "Unlink %s.", file.c_str());
            if(unlink(file.c_str()) != 0)
              throw io_exception(string("cutfile: unlink ") + file + " fail:" + strerror(errno));
            return true;
        } catch(exception& e) {
            //把分割的文件删除
            for(int i=0; i<ofiles.size(); i++) {
                if(unlink(ofiles[i].c_str()) != 0) {
                  LOG_P2(CRITICAL, "cutfile: unlink %s fail: %s", ofiles[i].c_str(), strerror(errno));
                }
            }
            throw;
        }
    }


    void write_to_sfile(const string& filename, list<Object*>& frag) {
        hessian_output out(localcode.c_str());
        string content;

        content.reserve(1024 * frag.size());
        for(list<Object*>::iterator iter = frag.begin(); iter != frag.end(); iter ++) {
            Object* object = *iter;
            out.write_object(content, object);
        }

        touchFile(filename);
        int fd = ::open(filename.c_str(), O_WRONLY | O_APPEND);
        if(fd != -1) {
            ssize_t fsize = ::write(fd, content.c_str(), content.size());
            close(fd);
        } else {
            throw io_exception(string("write_to_sfile:") + strerror(errno));
        }
    }

    void log_process(Object* obj) {
      if(log_level == DEBUG) {
        //打印出处理过程的交易号
        if(obj->classname() == "List") {
          list<Object*> process = static_cast<List*>(obj)->value();                                
          if(process.size() > 0) {           
            Map* rec = static_cast<Map*>(*process.begin());
            map<string, Object*>& value = rec->value();
            
            String* type = static_cast<String*>(value["log_type"]);
            if(type->value() == "PROC") {
              String* pid = static_cast<String*>(value["log_pid"]);
              SLOG(DEBUG, pid->value().c_str());
            }
          }
        }
      }
    }

    bool send(const string& filename, bool isEnd, struct stat& filestat) {
        int current = 0 ;
        struct dirent *dp;
        string buffer;
        string errmsg;                
          
        LOG_P1(DEBUG, "Send %s", filename.c_str());
        
		
		map<string, list<Object*> >::iterator it = m_sendfiles.find(filename);
		//判断文件是否已经存在于发送队列, 如果存在放弃
		if(it != m_sendfiles.end()) {
			return false;
		}
			
        list<Object*>& fileobjects = m_sendfiles[filename];

        file_input_stream* fis = new file_input_stream(filename);
        auto_ptr<input_stream> is (fis);
        hessian_input in(is,localcode.c_str());
        
        int count = 0;
        try {
            while(true) {
                Object* obj = in.read_object();
                log_process(obj);
                m_tosend.push_back(obj);
                fileobjects.push_back(obj);
                count ++;
            }
        } catch(io_exception e) {
            //Ignore
            LOG_P2(DEBUG, "file size:%d, count %d", filestat.st_size, count);
            assert(count != 0);
        }

        m_sendlength += filestat.st_size;

        if(m_tosend.size() >= m_batch || isEnd || m_sendlength > 1024*100) {
          try {
              LOG_P3(DEBUG, "m_tosend.size():%d isEnd:%d m_sendlength:%d", m_tosend.size() , isEnd , m_sendlength);
             
              hessian_output out(localcode.c_str());
              List htosend(m_tosend);

              buffer.reserve(1024 * m_tosend.size());
              buffer = out.start_call(m_method);
              out.write_object(buffer, &htosend);
              out.complete_call(buffer);

              int result = send_tcp_message(buffer, errmsg);

              LOG_P1(DEBUG, "Send message %d", result);
              
              if(result == 0) {
                  remove_files(m_sendfiles);

              } else if(result == 1) {
                //有非法的记录
                map<string, list<Object*> >::iterator it;
                for(it = m_sendfiles.begin(); it != m_sendfiles.end(); it++) {
                  List htosend2(it->second);

                  buffer = out.start_call(m_method);
                  out.write_object(buffer, &htosend2);
                  out.complete_call(buffer);
                  //因为内部的Object对象的释放由， htosend赋值， 故强制释放
                  htosend2.value().clear();
                  result = send_tcp_message(buffer, errmsg);
                  LOG_P2(DEBUG, "Send message %s: %d", it->first.c_str(), result);
                  
                  if(result == 0) {
                    remove_file(it->first);
                  } else {
                    move_to_error_dir(it->first);
                    LOG_P2(MAJOR, "Transfer %s error: %s.", it->first.c_str(), errmsg.c_str());
                  }
                }
              }
              m_tosend.clear();
          m_sendfiles.clear();
          m_sendlength = 0;
          return true;
            }catch(...) {
          m_tosend.clear();
          m_sendfiles.clear();
          m_sendlength = 0;
          throw;
        }
        }

        return false;
    }

    /**
     * 删除文件
     * @param list 一个文件名的list
     */
    void remove_files(map<string, list<Object*> > &files) {
        for(map<string, list<Object*> >::iterator iter = files.begin();
          iter != files.end(); iter++) {
            remove_file(iter->first);
        }
    }

    /**
     * 删除文件
     * @param file 一个文件名
     */
    void remove_file(const string& file) {
        LOG_P1(DEBUG, "Remove %s", file.c_str());    
        if(unlink(file.c_str()) != 0)
          throw io_exception(string("remove ") + file + " fail:" + strerror(errno));            
    }

    /**
     *
     */
    bool rename_to_rfiles(const string& name, bool isEnd, struct stat& filestat) {
        assert(*name.rbegin() == 'w');
        string rname = name.substr(0, name.size() - 2) + ".r";
        
        if(filestat.st_size == 0) {
          //此文件应用还没来得及写数据
          return false;
        }
        
        if(link(name.c_str(), rname.c_str()) != 0 || unlink(name.c_str()) != 0) {
          //可能在读取目录的时候, 前面已经处理过了的文件, 后面应用进程又产生了该文件, 可能引起重名, 故忽略. 
          //throw io_exception(string("rename ") + name + " " + rname + " fail:" + strerror(errno));
          LOG_P1(DEBUG, "%s exist.", rname.c_str());  
          return false;
        } else {
          LOG_P2(DEBUG, "Rename %s to %s", name.c_str(), rname.c_str());  
          return true;
        }  
    }

    void touchFile(const string& filename) {
        static mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

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
     * 把文件移到错误目录
     * @param files
     * @param toDir 错误目录名
     */
    void move_to_error_dir(map <string, list<Object*> > &files) {
        for(map <string, list<Object*> >::iterator iter = files.begin(); iter != files.end(); iter++) {
          move_to_error_dir(iter->first);
        }
    }


    /**
     * 把文件移到错误目录
     * @param files
     * @param toDir 错误目录名
     */
    void move_to_error_dir(const string& filename) {
        int pos = filename.rfind("/");
        string to = m_errordir + filename.substr(pos + 1);
        
        LOG_P2(DEBUG, "Move %s to %s", filename.c_str(), to.c_str());
        
        if(link(filename.c_str(), to.c_str()) != 0 || unlink(filename.c_str()) != 0)
          throw io_exception("move_to_error_dir fail.");            
    }

    /**
     * 发送TCP消息，并接受回复，如果回复不是异常的话，视为一次成功的发送
     * @param data
     * @param size 字符串的大小
     * @return int 成功返回0，对端有异常返回1
     */
    int send_tcp_message(const string& message, string &errmsg) {
        TCPClient client;
        client.connect(Address(m_remote_host, m_port), 2000);
        try {
            client<<message;
            auto_ptr<input_stream> is (new SocketInputStream(client));
            hessian_input in(is, localcode.c_str());

            in.start_reply();
            Object * object = in.read_object();
            in.complete_reply();

            client.disconnect();

            auto_ptr<Object> destoryer(object);

            Fault* fault = dynamic_cast<Fault*>(object);
            if(fault != NULL) {
              errmsg = fault->message();
                return 1;
            }
        } catch (...) {
            client.disconnect();
            throw;
        }
        return 0;
    }
};


ExporterFileScanner::ExporterFileScanner(const string& _host, int _port, vector<string>& dirnames, int _batch) {
    signal(SIGPIPE, SIG_IGN);

    const char* home = getenv("AGENT_HOME");
    if(home != NULL)
        strcpy(agent_home, home);

    for(int i=0; i<dirnames.size(); i++) {
      string& logdir = dirnames[i];
      if(*logdir.rbegin() != DIR_DELIM) {
          logdir += DIR_DELIM;
      }
      string processdir = logdir + PROCESS_DIR + DIR_DELIM;
      string alertdir = logdir  + ALERT_DIR + DIR_DELIM;
      string errordir = logdir  + ERROR_DIR + DIR_DELIM;
      m_logdirs.push_back( new LogDir(_host, _port,alertdir, errordir, "batchLogAlert", _batch));
      m_logdirs.push_back( new LogDir(_host, _port,processdir, errordir, "batchLogProcess", _batch));
    }
}


void ExporterFileScanner::scan() {    
    for(int i=0; i<m_logdirs.size(); i++) {
      m_logdirs[i]->scan();
    }
}


ExporterFileScanner::~ExporterFileScanner() {
    for(int i=0; i<m_logdirs.size(); i++) {
      delete m_logdirs[i];
    }
}

}
