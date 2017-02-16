#include "log4cpp/Logger.h"
#include "base/SmartPtr.h"
#include "log4cpp/Appender.h"
#include "log4cpp/Formatter.h"
#include "base/StringUtil.h"
#include "base/UUID.h"
#include <stdio.h>
#include <fstream>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
USING_NAMESPACE(std);



namespace log4cpp {

class LoggerImpl;
typedef counted_ptr < LoggerImpl > PLoggerImpl;
typedef map < string, string > Properties;

#define LOGBUF_SZ 4096
#define DEFAULT_WATCH_DELAY 60 //s

/** 日志类型 */
struct LogType {
    enum Type {
        ALL = 0, TRACE = 1, DEBUG = TRACE << 1, STATE = DEBUG << 1, INFO = STATE << 1, WARNING = INFO << 1, MINOR =
           WARNING << 1, MAJOR = MINOR << 1, CRITICAL = MAJOR << 1, INDETERMINATE = CRITICAL << 1, OK = INDETERMINATE << 1, PROC= OK << 1, OFF = PROC << 1
    };

    static Type valueOf(const string & str) {
#define LOGTYPE_CASE(type) if(StringUtil::equalsIgnoreCase(str, #type)) return type
        LOGTYPE_CASE(TRACE);
        LOGTYPE_CASE(DEBUG);
        LOGTYPE_CASE(STATE);
        LOGTYPE_CASE(INFO);
        LOGTYPE_CASE(WARNING);
        LOGTYPE_CASE(MINOR);
        LOGTYPE_CASE(MAJOR);
        LOGTYPE_CASE(CRITICAL);
        LOGTYPE_CASE(INDETERMINATE);
        LOGTYPE_CASE(OK);
        LOGTYPE_CASE(PROC);
        LOGTYPE_CASE(OFF);
        return ALL;
#undef LOGTYPE_CASE
    }

    static Type valueOf(Grade::Type grade) {
        return static_cast < Type > (WARNING << grade);
    }
};


struct AppenderWrapper {
    PAppender appender;

    /** 输出器的过滤条件, (appender_name, filter)每一个日志类型占一个位, 最后合成一个int */
    int filter;
    Formatter * formatter;
};


typedef counted_ptr < AppenderWrapper > PAppenderWrapper;

/** 定义一个域类型 */
struct Category {
    /** 名称 */
    string name;

    /** 调试级别, trace debug off */
    LogType::Type level;

    /** 所有的输出器 */
    map < string, PAppenderWrapper > appenders;
};


typedef counted_ptr < Category > PCategory;

/** 日志内部实现 */
class LoggerImpl : public Logger {
    /** 该logger的dn值 */
    string m_dn;

    /** 域类型 */
    Category * m_category;

public:

    LoggerImpl(const string & dn) : m_dn(dn), m_category(0) {
        //如果log4cpp已配置, 就初始化, 否则等log4cpp配置的时候统一初始化.
        if(g().configured) init();
    }
    
    virtual ~LoggerImpl() {
        m_category = 0;
    }

    void init() {
        m_category = 0;
        size_t matchLength = 0;
        for (map < string, PCategory >::const_iterator it = g().categories.begin(); it != g().categories.end(); it++) {
            const string & name = it->first;
            if (m_dn.find(name) == 0 && name.length() > matchLength) {
                matchLength = name.length();
                m_category = it->second.get();
            }
        }
        if (!m_category) m_category = g().categories["root"].get();
    }

    /** 是否允许调试 */
    virtual bool isDebugEnabled() {
        if(!m_category) return false;
        return m_category->level <= LogType::DEBUG;
    }



    virtual bool isTraceEnabled() {
        if(!m_category) return false;
        return m_category->level <= LogType::TRACE;
    }


    const char * getShortFilename(const char * file) {
        for (const char * p = file; * p != 0; p++) {
            if (* p == '/' || * p == '\\') {
                file = p + 1;
            }
        }
        return file;
    }

#define BEGIN_PRINT(logtype) \
   for (map < string, PAppenderWrapper >::iterator it = m_category->appenders.begin(); it != m_category->appenders.end(); it++) { \
   if (it->second->filter == 0 || (it->second->filter & logtype) != 0) { \
   char buffer[LOGBUF_SZ];\
   int size = 0;

#define END_PRINT() \
   it->second->appender->write(buffer, size); \
   } \
   }

#define BEGIN_VPRINT(logtype) \
   va_list ap; \
   va_start(ap, format); \
   for (map < string, PAppenderWrapper >::iterator it = m_category->appenders.begin(); it != m_category->appenders.end(); it++) { \
   if (it->second->filter == 0 || (it->second->filter & logtype) != 0) { \
   char buffer[LOGBUF_SZ];	\
   int size = 0;
   
#define END_VPRINT() \
   it->second->appender->write(buffer, size); \
   } \
   } \
   va_end(ap);

    /** 调试信息 YYYY-mm-dd HH-MM-SS,QQQ DEBUG [dn] FILE:LINE message */
    virtual void trace(const char * file, int line, const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        if (isTraceEnabled()) {
            BEGIN_VPRINT(LogType::TRACE);
            size = it->second->formatter->trace(buffer, LOGBUF_SZ, m_dn.c_str(), getShortFilename(file), line, format, ap);
            END_VPRINT();
        }
    }

    /** 调试信息 YYYY-mm-dd HH-MM-SS,QQQ DEBUG [dn] FILE:LINE message */
    virtual void debug(const char * file, int line, const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        if (isDebugEnabled()) {
            BEGIN_VPRINT(LogType::DEBUG);
            size = it->second->formatter->debug(buffer, LOGBUF_SZ, m_dn.c_str(), getShortFilename(file), line, format, ap);
            END_VPRINT();
        }
    }

    /** 普通信息输出 YYYY-mm-dd HH-MM-SS,QQQ INFO [dn] message */
    virtual void info(const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        BEGIN_VPRINT(LogType::INFO);
        size = it->second->formatter->info(buffer, LOGBUF_SZ, m_dn.c_str(), format, ap);
        END_VPRINT();
    }



    /** 状态信息输出 YYYY-mm-dd HH-MM-SS,QQQ STATE [dn] n1=v1;n2=v2 */
    virtual void state(const NameValueList & states) {
        if(!m_category) return;
        doWatchConfiguration();
        BEGIN_PRINT(LogType::STATE);
        size = it->second->formatter->state(buffer, LOGBUF_SZ, m_dn.c_str(), states);
        END_PRINT();
    }

    /** 告警输出 YYYY-mm-dd HH-MM-SS,QQQ LEVEL [dn] code message */
    virtual void alert(const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        BEGIN_VPRINT(LogType::MAJOR);
        size = it->second->formatter->alert(buffer, LOGBUF_SZ, m_dn.c_str(), Grade::MAJOR, -1, 0, format, ap);
        END_VPRINT();
    }

    virtual void alert(Grade::Type grade, int code, const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        LogType::Type logtype = LogType::valueOf(grade);
        BEGIN_VPRINT(logtype);
        size = it->second->formatter->alert(buffer, LOGBUF_SZ, m_dn.c_str(), grade, code, 0, format, ap);
        END_VPRINT();
    }

    virtual void alert(const Exception & e) {
        if(!m_category) return;
        doWatchConfiguration();
        LogType::Type logtype = LogType::valueOf(e.getGrade());
        BEGIN_PRINT(logtype);
        size = it->second->formatter->alert(buffer, LOGBUF_SZ, m_dn.c_str(), Grade::MAJOR, -1, & e, 0, va_list());
        END_PRINT();
    }

    virtual void alert(Grade::Type grade, int code, const Exception & e, const char * format,...) {
        if(!m_category) return;
        doWatchConfiguration();
        LogType::Type logtype = LogType::valueOf(grade);
        BEGIN_VPRINT(logtype);
        size = it->second->formatter->alert(buffer, LOGBUF_SZ, m_dn.c_str(), grade, code, & e, format, ap);
        END_VPRINT();
    }

    /** 通知该对象(dn)运行全部正常, 所以告警将被清除 YYYY-mm-dd HH-MM-SS,QQQ OK [dn] */
    virtual void ok() {
        if(!m_category) return;
        ok(-1);
    }

    /**
     * 通知该对象(dn)某类错误不再存在 比如在网络连不上时, 不停地报网络错误, 正常时可以不停地报网络正常, Logger判断: 如果刚才不正常的话, 会触发一个清除改类型告警的动作, 如果一直正常, 不会有任何动作.
     * YYYY-mm-dd HH-MM-SS,QQQ OK [dn] code
     */
    virtual void ok(int code) {
        if(!m_category) return;
        doWatchConfiguration();
        BEGIN_PRINT(LogType::OK);
        size = it->second->formatter->ok(buffer, LOGBUF_SZ, m_dn.c_str(), code);
        END_PRINT();
    }
    
    virtual string pBegin(const char* format, ...) {
        if(!m_category) return string();
    	doWatchConfiguration();
    	UUID uuid = UUID::generate();
        string pid = uuid.ascii();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pBegin(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), format, ap);
        END_VPRINT();
        return pid;
    }
        
    /**
     * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
     */ 
    virtual void pInput(const string& pid, const string& fullname, size_t count, const char* format=0, ...) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pInput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), count, format, ap);
        END_VPRINT();
    }
    // 下面方法分两次调用, 可以记录消耗的时长        
    /**
     * 开始读入
     */ 
    virtual void pBeginInput(const string& pid, const string& fullname, const char* format=0, ...) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pBeginInput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), format, ap);
        END_VPRINT();
    }
    /**
     * 完成读入
     */ 
    virtual void pEndInput(const string& pid, const string& fullname, size_t count, const char* format=0, ...) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pEndInput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), count, format, ap);
        END_VPRINT();
    }

	/**
	 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
	 */ 
	virtual void pOutput(const string& pid, const string& fullname, size_t count, const char* format=0, ...) {
	    if(!m_category) return;
		doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pOutput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), count, format, ap);
        END_VPRINT();
	}       
	//下面方法分两次调用, 可以记录消耗的时长
	/**
	 * 开始输出
	 */ 
    virtual void pBeginOutput(const string& pid, const string& fullname, const char* format=0, ...) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pBeginOutput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), format, ap);
        END_VPRINT();
    }
    /**
     * 完成输出
     */ 
    virtual void pEndOutput(const string& pid, const string& fullname, size_t count, const char* format=0, ...) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pEndOutput(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), fullname.c_str(), count, format, ap);
        END_VPRINT();
    }
    
    /**
     * 链接
     */ 
    virtual void pLinkOut(const string& pid, const string& source, const string& target) {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_PRINT(LogType::PROC);
        size = it->second->formatter->pLinkOut(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), source.c_str(), target.c_str());
        END_PRINT();
    }
    
    
    /**
     * 完成一次执行
     */ 
    virtual void pEnd(const string& pid, const char* format=0, ...)  {
        if(!m_category) return;
    	doWatchConfiguration();
        BEGIN_VPRINT(LogType::PROC);
        size = it->second->formatter->pEnd(buffer, LOGBUF_SZ, m_dn.c_str(), pid.c_str(), format, ap);
        END_VPRINT();
    }

 

    ///////////////////////////////////////////////////////////////////////////////
    ////静态方法
    ///////////////////////////////////////////////////////////////////////////////
    static Logger & getLoggerImpl(const string & dn) {
        //LoggerImpl::checkConfiguration(); 
        //使用全局变量初始化, 可保证log4cpp的所有cpp模块都初始化之后被调用.
        PLoggerImpl & logger = g().loggers[dn];
        if (!logger) {
            logger = PLoggerImpl(new LoggerImpl(dn));
        }
        
        return * logger;
    }

    static bool isConfigured() {
        return g().configured;
    }

    static void doWatchConfiguration() {
        static time_t lastWatchTime;
        static time_t lastCfgTime;

        //判断是否需要检测
        if (g().watchDelay == 0) return;

        //如果没有文件配置, 则返回
        if (g().configFilename.length() == 0) return;

        //检查是否到检测时间
        time_t current = time(0);
        if ((current - lastWatchTime) < g().watchDelay) return;
        lastWatchTime = current;

        //开始检查
        struct stat fs;
        stat(g().configFilename.c_str(), & fs);

        //判断是否是第一次检查
        if (lastCfgTime == 0) {
            lastCfgTime = fs.st_mtime;
            return;
        }

        //如果变化了
        if (lastCfgTime != fs.st_mtime) {
            lastCfgTime = fs.st_mtime;
            refresh();
        }
    }

    static void refresh() {
        Logger & logger = getLoggerImpl("com.linkage.logger");
        logger.info("配置文件%s被更改, 刷新配置, 注:%d秒钟检测一次配置.", g().configFilename.c_str(), g().watchDelay);
        readConfigure(g().configFilename.c_str());
        doConfigure();
        
    }

    static int checkConfiguration() {
        if (!LoggerImpl::isConfigured()) {
            LoggerImpl::configureImpl("logger.properties", -1);
        }
        if (!LoggerImpl::isConfigured()) {
            const char * cfgfile = getenv("LOGGER_CFG");
            if (cfgfile) {
                LoggerImpl::configureImpl(cfgfile, -1);
            }
        }
        if (!LoggerImpl::isConfigured()) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s/.logger.properties", getenv("HOME"));
            LoggerImpl::configureImpl(buffer, -1);
        }
        if (!LoggerImpl::isConfigured()) {
            //构造Appenders
            doConfigure();
            g().configured = true;
        }
        return 0;
    }



    static bool readConfigure(const char * configFilename) {
        //读取配置
        ifstream in(configFilename);
        if (in.good()) {
            g().properties.clear();
            vector < string > pair;
            while (true) {
                char line[256] = {0};

                in.getline(line, sizeof(line) - 1);
                if (in.fail()) break;


                int len = strlen(line);
                if (len == 0) continue;
                
                if (line[0] == '#') continue;

                //兼容windows格式
                if (line[len - 1] == '\r') line[len - 1] = '\0';

                pair.clear();
                StringUtil::split(line, "=", pair, true);
                if (pair.size() == 2) {
            		size_t begin = pair[1].find("${", 0);
            		if(begin != string::npos) {
            			size_t end = pair[1].find("}", begin+2);
            			if(end != string::npos) {
            				string envKey = pair[1].substr(begin+2, end - (begin + 2));
            				char* envValue = getenv(envKey.c_str());
            				if(envValue == NULL) {
            					cerr<<"无法找到"<<pair[0]<<"的值:名为"<<pair[1]<<"的环境变量"<<endl;
            				} else {
            					pair[1].replace(begin, end - begin + 1, envValue);	
            				}	
            			}
            		}                		
                	g().properties[pair[0]] = pair[1];
                 }

                if (in.eof()) break;
            }


            return true;
        }
        return false;
    }

    static void doConfigure() {
        if (g().properties["logger.category.root"].length() == 0) {
            g().properties["logger.category.root"] = "trace,__console";
            g().properties["logger.appender.__console"] = "com.linkage.logger.ConsoleAppender";
        }
        g().properties[g().FORMATTER_PROP_PREFIX + g().DEFAULT_FORMATTER_ID] = g().DEFAULT_FORMATTER_CLASS;
        g().appenders.clear();
        g().formatters.clear();
        g().categories.clear();


        for (Properties::iterator it = g().properties.begin(); it != g().properties.end(); it++) {
            const string & key = it->first;
            if (key.find(g().FORMATTER_PROP_PREFIX) != string::npos &&
               key.find(".", g().FORMATTER_PROP_PREFIX.length()) == string::npos) /*忽略格式器的参数*/ {
                   makeFormatter(key);
            }
        }
        for (Properties::iterator it = g().properties.begin(); it != g().properties.end(); it++) {
            const string & key = it->first;
            if (key.find(g().APPENDER_PROP_PREFIX) != string::npos &&
               key.find(".", g().APPENDER_PROP_PREFIX.length()) == string::npos) /*忽略输出器的参数*/ {
                   makeAppender(key);
            }
        }
        for (Properties::iterator it = g().properties.begin(); it != g().properties.end(); it++) {
            const string & key = it->first;
            if (key.find(g().CATEGORY_PROP_PREFIX) != string::npos) {
                   makeCategory(key);
            }
        }
        
        //初始化所有的dn的logger句柄, 
        for (map < string, PLoggerImpl >::iterator it = g().loggers.begin(); it != g().loggers.end(); it++) {
            it->second->init();
        }
    }

    static void configureImpl(const map < string, string > & properties) {
        g().properties = properties;
        doConfigure();
        g().configured = true;
    }

    static void configureImpl(const char * configFilename, time_t watchDelay) {
        if (readConfigure(configFilename)) {
            doConfigure();
            g().configured = true;
            g().configFilename = configFilename;
            g().watchDelay = watchDelay < 0 ? DEFAULT_WATCH_DELAY : watchDelay;
        }
    }

    static void makeAppender(const string & key) {
        string name = key.substr(g().APPENDER_PROP_PREFIX.length());
        //得到Appender插件
        PAppender app = PAppender(createAppender(g().properties[key]));
        if (!app) {
            cerr << "无效的Appender类名:" << g().properties[key] << endl;
            return;
        }
        map < string, string > prop;
        loadSubProperties(key, prop);
        app->init(prop);

        //计算过滤器
        vector < string > types;
        StringUtil::split(prop["filter"], ",", types, true);
        int filter = 0;
        for (size_t i = 0; i < types.size(); i++) {
            filter |= LogType::valueOf(types[i]);
        }

        //得到Formatter
        Formatter * formatter = 0;
        const string & formatterId = prop["formatter"];
        if (formatterId.length() != 0) {
            formatter = g().formatters[formatterId].get();
            if (!formatter) {
                cerr << "无效的Formatter实例名:" << formatterId << endl;
            }
        }
        if (!formatter) {
            formatter = g().formatters[g().DEFAULT_FORMATTER_ID].get();
        }
        //构造wrapper
        PAppenderWrapper wrapper(new AppenderWrapper());
        wrapper->appender = app;
        wrapper->filter = filter;
        wrapper->formatter = formatter;

        g().appenders[name] = wrapper;
    }


    static void loadSubProperties(const string & key, map < string, string > & prop) {
        Properties::const_iterator it = g().properties.begin();
        for ( ; it != g().properties.end(); it++) {
            const string & akey = it->first;
            if (akey.find(key) == 0 && akey != key) {
                string attname = akey.substr(key.length() + 1);
                prop[attname] = it->second;
            }
        }
    }

    static void makeFormatter(const string & key) {
        string name = key.substr(g().FORMATTER_PROP_PREFIX.length());
        PFormatter formatter = PFormatter(createFormatter(g().properties[key]));
        if (!formatter) {
            cerr << "无效的Formatter类名:" << g().properties[key] << endl;
            return;
        }

        map < string, string > prop;
        loadSubProperties(key, prop);

        formatter->init(prop);
        g().formatters[name] = formatter;
    }


    static void makeCategory(const string & key) {
        string name = key.substr(g().CATEGORY_PROP_PREFIX.length());
        const string & value = g().properties[key];
        PCategory category(new Category());
        category->name = name;
        category->level = LogType::OFF;

        vector < string > params;
        StringUtil::split(value, ",", params, true);
        if (params.size() > 0) {

#ifndef OFF_DEBUG
            category->level = LogType::valueOf(params[0]);
#endif

            for (size_t i = 1; i < params.size(); i++) {
                map < string, PAppenderWrapper >::iterator it = g().appenders.find(params[i]);
                if (it != g().appenders.end()) {
                    category->appenders[it->first] = it->second;
                } else {
                    cerr << "无效的appender标识:" << params[i] << endl;
                }
            }
        }
        g().categories[name] = category;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    /////格式化的管理函数
    ///////////////////////////////////////////////////////////////////////////////
    static FormatterCreator & getFormatterCreator(const string & name) {        
        return g().formatterCreators[name];
    }

    static int __registerFormatter(const string & name, FormatterCreator creator) {
        getFormatterCreator(name) = creator;
        return 0;
    }

    static Formatter * createFormatter(const string & name) {
        FormatterCreator c = getFormatterCreator(name);    
        return c == 0? 0 : c();
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    /////输出器的管理函数
    ///////////////////////////////////////////////////////////////////////////////
    static AppenderCreator &getAppenderCreator(const string & name) {       
        return g().appenderCreators[name];
    }
    
    static int __registerAppender(const string & name, AppenderCreator creator) {
        getAppenderCreator(name) = creator;
        return 0;
    }
    
    static Appender * createAppender(const string & name) {
        AppenderCreator c = getAppenderCreator(name);    
        return c == 0? 0 : c();
    }


    ////////////////////////////////////////////////////////////////////////////////
    /////静态变量
    ////////////////////////////////////////////////////////////////////////////////

private:
	struct GlobalConf {	    

    	/** 从配置文件导入的配置 */    
        Properties properties;
    
        /** 所有的域类型 (name, category) */
        map < string, PCategory > categories;
    
        /** 所有的输出器 (name, appender) */
        map < string, PAppenderWrapper > appenders;
    
        /** 所有的格式器 (name, formatter) */
        map < string, PFormatter > formatters;
    
        /** 所有的日志器, (dn,logger) */
        map < string, PLoggerImpl > loggers;
    
        /** 配置的文件名 */
        string configFilename;
    
        /** 是否已经配置 */
        bool configured;
    
        /** 检查配置文件变化的间隔的时间 */
        time_t watchDelay;
        
        /*登记的所有的格式化的工厂函数*/
        map < string, FormatterCreator > formatterCreators;
        
        /*登记所有的输出器工厂函数*/
        map < string, AppenderCreator > appenderCreators;
        
        
        string CATEGORY_PROP_PREFIX;
        string APPENDER_PROP_PREFIX;
        string FORMATTER_PROP_PREFIX;
        string DEFAULT_FORMATTER_ID;
        string DEFAULT_FORMATTER_CLASS;
    
        GlobalConf() : 
            CATEGORY_PROP_PREFIX("logger.category."),        
            APPENDER_PROP_PREFIX("logger.appender."),
            FORMATTER_PROP_PREFIX("logger.formatter."),
            DEFAULT_FORMATTER_ID("__std_formatter"),
            DEFAULT_FORMATTER_CLASS("com.linkage.logger.StdFormatter") {}
	};
    /**
     * 通过函数静态变量, 保证所有的全局变量, 在被调用的时候初始化
     * 编译器保证先初始化后释放全局变量的原则
     *
     * 1.在logger.cpp模块没有被初始化的时候, 被调用输出日志, 将被忽略.
     *
     * 2.目前的机制在一般情况下, GlobalConf的释放都在其他对象的后面,
     *   即使其他对象所在的cpp模块先初始化, 但由于初始化需要调用getLogger, 
     *   会触发 static GlobalConf& g() 的调用, 效果就是GlobalConf conf;
     *   反而先被初始化, 也就保证最后析构GlobalConf conf;
     *
     * 3.在logger模块没有初始化时, 如果调用输出, 程序通过m_category==0, 而忽略输出
     *
     * 4.目前维有一个陷阱, 但无法避免的情况是
     *   在一个全局对象的析构里调用log4cpp, 其他地方都没有使用, 这样就有可能
     *   在该对象调用log4cpp的时候, 已经析构, 会产生不可意料的结果,
     *   不过一般不可能发生这种情况, 另外一般建议的写法是, 定义全局的引用
     *   static Logger& logger = Logger::getLogger("x.x.x.x");
     *   可能出错, 但一般不可能发生的case如下:
     *   
     *   //tl.cpp
     *   struct TL {                                            
     *       ~TL()  {                                        
     *           Logger::getLogger("x,x,x").alert("destroy");
     *       }                                               
     *   };                                                  
     *   static TL tl;
     *
     *   //Makefile    
     *   xlC -o tl tl.cpp -llog4cpp
     *
     * 
     */  
    static GlobalConf& g() {
    	static GlobalConf conf;
    	return conf;
    }
};

///////////////////////////////////////////////////////////////////////////////
void Logger::configure(const char * configFilename, time_t delay) {
    LoggerImpl::configureImpl(configFilename, delay);
}

void Logger::configure(const map < string, string > & properties) {
    LoggerImpl::configureImpl(properties);
}

Logger & Logger::getLogger(const string & dn) {
    return LoggerImpl::getLoggerImpl(dn);
}

Logger::~Logger() {
}

}
//////////////////////////////////////////////////////////////////////////////////
//初始化插件
#include "plugins.def"

//调用log4cpp的配置
int checked = log4cpp::LoggerImpl::checkConfiguration();

