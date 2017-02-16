#include "DynamicLibrary.h"
#include "base/SmartPtr.h"
#include "base/StringUtil.h"
#include "log4cpp/Logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <map>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <dlfcn.h>
#include <utility>
#include <time.h>
#include <algorithm>
#include <stdlib.h>


USING_REL_OPS_NAMESPACE;
USING_NAMESPACE(std);
using namespace log4cpp;

static Logger& logger = Logger::getLogger("bude.syscomp.dynlib");
const time_t CHECK_INTERVAL = 0; //6s



template < typename T >
struct counted_ptr_less : public binary_function < counted_ptr < T >, counted_ptr < T >, bool > {
    bool operator() (const counted_ptr < T > & __x, const counted_ptr < T > & __y) const { return * __x.get() < * __y.get(); }
};



/** 定义库文件的类 */
class Library {
public:
    /** 构造一个库的基本信息 */
    Library(const string & filename);

    /** 析构 */
    ~Library();

    /** 打开库 */
    void open();

    /** 关闭库 */
    void close();

    /** 实现比较操作 */
    bool operator < (const Library & that) const;

    /** 库名称 */
    string name;

    /** 文件名称 */
    string filename;

    /** 库的句柄 */
    void * handle;

    /** 版本信息 */
    vector < int > version;
    string strVersion;

    /** 最近一次检查的时间 */
    time_t lastCheckTime;
    
    string toString();
    
    friend ostream& operator << (ostream& out, const Library& lib);
    
    
};


typedef counted_ptr < Library > PLibrary;
//库的名称-库对象
typedef map < string, PLibrary > Holder;

typedef map <string, string> PatchInfo;

/*全局变量数据结构, 所有全局变量放置在此结构中*/
struct DB {
	Holder g_holder;
	time_t lastpatchtime;
	string patchfile;
	PatchInfo patchInfo;	
};

//所有全局静态变量使用函数包装, 以避免模块初始化顺序问题. 
//如果不包装, 则如果本模块还没初始化, 但函数已经被别的模块调用, 就可能发生不可意料的结果.
static DB& _db() {
	static DB db;
	return db;
}


ostream& operator << (ostream& out, const Library& lib) {
	out << "库文件名:" << lib.filename<< ", 版本号:";
	for(int i=0; i<lib.version.size(); i++) {
		if(i!=0)
			out << ".";
		out << lib.version[i];
	}
	return out;
}

string Library::toString() {
	ostringstream out;
	out << *this;
	return out.str();
}

Library::Library(const string & afilename):handle(0) {

    this->filename = afilename;

    int pos = this->filename.find_last_of("/");
    if (pos == -1) pos = this->filename.find_last_of("\\");
    int spos = this->filename.find_last_of(".");

    int vpos = this->filename.find_first_of("-", pos + 1);
    if (vpos == string::npos) vpos = spos;

    //取库的名称
    this->name = this->filename.substr(pos + 1, vpos - (pos + 1));


    //取版本信息
    string verStr = this->filename.substr(vpos + 1, spos - (vpos + 1));
    if (verStr.length()) {        
        const char *p = verStr.c_str();
        string temp;
        for(char ch=*p; ch != 0; ch=*++p) {
          if(ch != '.') {
             temp+=ch;
          } else if(temp.length() != 0){
             version.push_back(atoi(temp.c_str()));
             temp.resize(0);
          }
        }
        if(temp.length() != 0) {
          version.push_back(atoi(temp.c_str()));
        }
    }
    this->strVersion = verStr;

    //检测时间
    this->lastCheckTime = time(0);
}

Library::~Library() {
    close();
}

typedef int(* hook) (void);

void Library::open() {
    if (handle == 0) {
        handle = dlopen(filename.c_str(), RTLD_LAZY);
        if (handle == 0) THROW_P3(LibraryException, "打开动态库(%s)失败: %s\n %s", filename.c_str(), dlerror(), 
        	getenv("LIBPATH")==0? getenv("LD_LIBRARY_PATH"):getenv("LIBPATH"));

    //cout << "open handle-" << handle << endl; 
        hook h = (hook)dlsym(handle, "dyn_create");
        if (h != 0) {
            int c = h();
            if (c != 0) {
                THROW_P1 (LibraryException, "调用动态库(%s)初始化函数(dyn_create)失败", filename.c_str());
            }
        }
    }
}

void Library::close() {	
    if (handle) {
        LOG_TRACE_P2(logger, "关闭动态库%s,handle=%p", toString().c_str(), handle);
        hook h = (hook)dlsym(handle, "dyn_destroy");
        if (h != 0) {
            int c = h();
            if (c != 0) {
            	logger.alert("调用动态库(%s)初始化函数(dyn_destroy)失败", filename.c_str());
            }
        }

        dlclose(handle);
        handle = 0;
    }
}



bool Library::operator < (const Library & that) const {
    for (int i = 0; i < version.size() && i < that.version.size(); i++) {
        if (version[i] != that.version[i]) return version[i] < that.version[i];
    }
    return version.size() < that.version.size();
}


static bool readPatchInfo(const char * configFilename) {
    //读取配置
    ifstream in(configFilename);
    if (in.good()) {
        _db().patchInfo.clear();
        vector < string > pair;
        while (true) {
            char line[256] = {0};

            in.getline(line, sizeof(line) - 1);
            if (in.fail()) break;

            if (line[0] == '#') continue;

            int len = strlen(line);
            if (len == 0) continue;

            //兼容windows格式
            if (line[len - 1] == '\r') line[len - 1] = '\0';

            pair.clear();
            StringUtil::split(line, "=", pair, true);
            if (pair.size() == 2) {
                //因为最新的在前面, 所有采用不覆盖方式
                if(_db().patchInfo.find(pair[0]) == _db().patchInfo.end()) {                        
                    _db().patchInfo[pair[0]] = pair[1];
                    Holder::iterator it = _db().g_holder.find(pair[0]);
                    if(it != _db().g_holder.end()) {
                        //如果版本不对, 删除
                        if(it->second->strVersion != pair[1]) {                            
                            LOG_TRACE_P1(logger, "版本不一致, 关闭动态库%s.", it->second->toString().c_str());
                            _db().g_holder.erase(it);
                        }
                    }
                }
            }

            if (in.eof()) break;
        }
        return true;
    }
    return false;
}

static void init() {
    if(_db().g_holder.size() == 0) {
        char* pf = getenv("SO_PATCHINFO");
        if(pf) {
            struct stat patchstat;
            if(stat(pf, &patchstat) == 0) {
                _db().patchfile = pf;
                _db().lastpatchtime = patchstat.st_mtime;     
                readPatchInfo(pf);   
            } else {
                logger.alert("检查patch文件失败:%s", strerror(errno));
            }
        }   
    }
}



DynamicLibraryManager::DynamicLibraryManager(const string & path) {
    init();
	setWorkPath(path);
}

DynamicLibraryManager::DynamicLibraryManager() : workPath(".") {    
    init();
}

DynamicLibraryManager::~DynamicLibraryManager() {
}

void DynamicLibraryManager::notify() {
      if(_db().lastpatchtime != 0) {
        struct stat patchstat;
        if(stat(_db().patchfile.c_str(), &patchstat) == 0) {
                if(patchstat.st_mtime != _db().lastpatchtime) {
                    readPatchInfo(_db().patchfile.c_str());     
                    _db().lastpatchtime = patchstat.st_mtime ;
                }
        }  else {
                logger.alert("检查patch文件失败:%s", strerror(errno));
        }        
      }
}

void * DynamicLibraryManager::getSymbol(const string & libname, const string & symname) {
    Library & lib = getLibrary(libname);
    void * sym = dlsym(lib.handle, symname.c_str());
    if (sym == 0) {
        THROW_P4 (LibraryException, "在库%s中(handle=%d), 找不到%s的函数入口, %s", 
        	lib.filename.c_str(), lib.handle, symname.c_str(), dlerror());
    }
    return sym;
}


void DynamicLibraryManager::setWorkPath(const string & path) {    
    workPath = path;
}


Library & DynamicLibraryManager::getLibrary(const string & libname) {
    char temp[256];
    Holder::iterator old = _db().g_holder.find(libname);
    
    //使用布丁文件取库
    if(_db().lastpatchtime != 0) {
        if(old != _db().g_holder.end())
            return *old->second;
        PatchInfo::iterator p = _db().patchInfo.find(libname);
        if(p != _db().patchInfo.end()) {
            string libfile = workPath + "/lib" + libname + (p->second.size()? string("-") + p->second : string());
            string libso = libfile + ".so";
            PLibrary lib;
            if(!access(libso.c_str(), F_OK)) {
                 lib = PLibrary(new Library(libso));
            }
            string libsl = libfile + ".sl";
            if(!access(libsl.c_str(), F_OK)) {
                 lib = PLibrary(new Library(libsl));
            }
            string libdll = libfile + ".dll";
            if(!access(libdll.c_str(), F_OK)) {
                lib = PLibrary(new Library(libdll));
            }
            if(lib) {
            	lib->open();
            	_db().g_holder[libname] = lib;            	
            	return *lib;
            }
            THROW_P1(LibraryException, "共享库(%s.*)在在目录中不存在", libfile.c_str());
        }
        else {
            THROW_P1(LibraryException, "共享库(%s)在patch的清单中不存在", libname.c_str());
        }
    }
    
    
    //自动使用最新版本
    if (old != _db().g_holder.end()) {
        
        if (time(0) - old->second->lastCheckTime < CHECK_INTERVAL) {
            return * old->second;            
        }
    }
    
    vector < PLibrary > libvers;

    DIR* dirp = opendir(workPath.c_str());
    
    while (dirp) {
        errno = 0;
        dirent* dp = readdir(dirp);
        if (dp != NULL) {
            if (strncmp(dp->d_name, "lib", 3) || 
                strncmp(dp->d_name + 3, libname.c_str(), libname.length())) {
                continue;            
            }
            
            //库名后面必须时. or -
            int sep = 3+libname.length();
            if (dp->d_name[sep] != '-' &&
               dp->d_name[sep] != '.') {    
                continue;
            }
            
            char* suffix = 0;
            if (dp->d_name[sep] == '.') {
                suffix = dp->d_name + sep;    
            } else {
                suffix = strrchr(dp->d_name,  '.');
                if(suffix == NULL) {
                    continue;
                }    
            }
            
            if (strcmp(suffix, ".so") == 0 || 
                strcmp(suffix, ".sl") == 0 ||
                strcmp(suffix, ".dll") == 0) {
                    PLibrary lib = PLibrary(new Library(workPath + "/" + dp->d_name));
                    libvers.push_back(lib);   
            }                
        } else {
            if (errno == 0) {            
                closedir(dirp);
                break;
            } else {
            	string strerr = strerror(errno);
                closedir(dirp);
                THROW(LibraryException, strerr.c_str());
            }
        }
    }

    if (libvers.size() > 0) {
        sort(libvers.begin(), libvers.end(), counted_ptr_less < Library > ());
        PLibrary & nlib = * libvers.rbegin();
        if (old == _db().g_holder.end() || *old->second.get() < *nlib.get()) {
	    	if(old != _db().g_holder.end() )
        		LOG_TRACE_P2(logger, "废弃旧库%s, 启用新库%s", old->second->toString().c_str(), nlib->toString().c_str());        	
	    	if(old == _db().g_holder.end() )
        		LOG_TRACE_P1(logger, "启用库%s", nlib->toString().c_str());        
	
            //如果没有打开过，或者目前找到的比以前打开的新，则打开。原来的会自动释放。            
            nlib->open();
            _db().g_holder[libname] = nlib;
            return * nlib;
        }
        old->second->lastCheckTime = time(0);
        return * old->second;
    } else {
        THROW_P2(LibraryException, "共享库(%s), 在目录(%s)不存在", libname.c_str(), workPath.c_str());
    }
}

