#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "base/config-all.h"
#include "base/Exception.h"
#include <vector>
#include <map>
USING_NAMESPACE(std);

namespace log4cpp {
    /**
     * 定义键值对
     */  
    struct NameValue {
        /**
         * 名称
         */ 
        string name;
        /**
         * 值
         */ 
        string value;

        /**
         * 构造
         */ 
        NameValue(const string & _name, const string & _value) : name(_name), value(_value) {
        }

        NameValue() {
        }

    };

    /**
     * 定义键值对列表
     */ 
    typedef vector < NameValue > NameValueList;

    /**
     * 定义日志器
     * 使用方法: 
     * static Logger& logger = Logger::getLogger("com.linkage.eboss.crm.createuser");
     * //...
     * logger.info("新开用户%s", userid);
     * 
     */ 
    class Logger {
    public:
        virtual ~Logger();
        
        /**
         * 配置Logger, 并定义检测配置文件变更
         * 在不显示调用配置的情况下
         * 系统按如下顺序获取配置文件
         * 1. ./logger.properties
         * 2. $LOGGER_CFG
         * 3. $HOME/.logger.properties
         * 4. 全部输出到标准输出
         * @configFilename 配置文件名
         * @watchDelay 检测的间隔时间, 单位秒
         */
        static void configure(const char* configFilename, time_t watchDelay = -1);
        
        static void configure(const map<string, string>& properties);

        /**
         * @param dn 描述对象的唯一路径, 命名规则使用"."进行分割, 如划价通道一的键 eboss.cbs.billing.rate.ch1 营业开户的键 eboss.crm.createuser
         * @return 返回日志对象
         */
        static Logger & getLogger(const string & dn);

        /** 是否允许调试 */
        virtual bool isDebugEnabled() = 0;
        virtual bool isTraceEnabled() = 0;
        
        /** 跟踪信息 YYYY-mm-dd HH-MM-SS.QQQ TRACE [dn] FILE:LINE message */
        virtual void trace(const char * file, int line, const char * msg,...) = 0;
    
        /** 调试信息 YYYY-mm-dd HH-MM-SS.QQQ DEBUG [dn] FILE:LINE message */
        virtual void debug(const char * file, int line, const char * msg,...) = 0;

        /** 状态信息输出 YYYY-mm-dd HH-MM-SS.QQQ STATE [dn] n1=v1;n2=v2 */
        virtual void state(const NameValueList & states) = 0;
        
        /** 普通信息输出 YYYY-mm-dd HH-MM-SS.QQQ INFO [dn] message */
        virtual void info(const char * msg,...) = 0;

        /** 告警输出 YYYY-mm-dd HH-MM-SS.QQQ GRADE [dn] code message */
        virtual void alert(const char * msg,...) = 0;
        virtual void alert(Grade::Type grade, int code, const char * msg,...) = 0;

        virtual void alert(const Exception & e) = 0;
        virtual void alert(Grade::Type grade, int code, const Exception & e, const char * msg,...) = 0;

        /** 通知该对象(dn)运行全部正常, 所以告警将被清除 YYYY-mm-dd HH-MM-SS.QQQ OK [dn] */
        virtual void ok() = 0;

        /**
         * 通知该对象(dn)某类错误不再存在 比如在网络连不上时, 不停地报网络错误, 正常时可以不停地报网络正常, Logger判断: 如果刚才不正常的话, 会触发一个清除改类型告警的动作, 如果一直正常, 不会有任何动作.
         * YYYY-mm-dd HH-MM-SS.QQQ OK [dn] code
         */
        virtual void ok(int code) = 0;
        
        /* 下面p开头(perform)的函数用于记录一次执行轨迹
         * 扩展参数使用可变参数表示, fields是描述扩展参数的信息.
         * fields的格式必须是, 如果是: 字段名=%d; or 字段名=xxxx, 使用sprintf的规则, 内部会组装成字符串
         * 
         * 使用举例:
         * string pid = pBegin();
         * 
         * pBeginInput(pid, "xxxx1.txt");
         * .....
         * pBeginOutput(pid, "xxxx1.txt");
         * ....
         * pEndOutput(pid, "xxxx.txt", 900, "tag1=90,tag2=%d", 32); //fields中可以常量, 也可以变量
         * 
         * ...
         * pEndInput(pid, "xxxx.txt", 300, "tag1=90,tag2=%d", 32); //fields中可以常量, 也可以变量
         * 
         * 
         * .....
         * 
         * 
         * pBeginOutput(pid, "xxxx2.txt");
         * ....
         * pEndOutput(pid, "xxxx2.txt", 900, "tag1=90,tag2=%d", 32); //fields中可以常量, 也可以变量
         * 
         * pEnd(performId, "tag1=90,tag2=%d", 32); //fields中可以常量, 也可以变量
         */
          
        /**
         * 开始一次执行
         * @return 返回这次执行的执行号
         */  
        virtual string pBegin(const char* fields=0, ...) = 0;
        
        /**
         * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
         */ 
        virtual void pInput(const string& performId, const string& fullname, size_t count, const char* fields=0, ...) = 0; 
        // 下面方法分两次调用, 可以记录消耗的时长        
        /**
         * 开始读入
         */ 
        virtual void pBeginInput(const string& performId, const string& fullname, const char* fields=0, ...) = 0;
        /**
         * 完成读入
         */ 
        virtual void pEndInput(const string& performId, const string& fullname, size_t count, const char* fields=0, ...) = 0;

		/**
		 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
		 */ 
		virtual void pOutput(const string& performId, const string& fullname, size_t count, const char* fields=0, ...) = 0;        
		//下面方法分两次调用, 可以记录消耗的时长
		/**
		 * 开始输出
		 */ 
        virtual void pBeginOutput(const string& performId, const string& fullname, const char* fields=0, ...) = 0;
        /**
         * 完成输出
         */ 
        virtual void pEndOutput(const string& performId, const string& fullname, size_t count, const char* fields=0, ...) = 0;
        /**
         * 完成一次执行
         */ 
        virtual void pEnd(const string& performId, const char* fields=0, ...) = 0;
        /**
         * 链接
         */
        virtual void pLinkOut(const string& performId, const string& source, const string& target) = 0;
    };

};

#ifdef OFF_DEBUG
#define LOG_TRACE(log, msg) 
#define LOG_TRACE_P1(log, msg, p1) 
#define LOG_TRACE_P2(log, msg, p1, p2) 
#define LOG_TRACE_P3(log, msg, p1, p2, p3) 
#define LOG_TRACE_P4(log, msg, p1, p2, p3, p4) 

#define LOG_DEBUG(log, msg) 
#define LOG_DEBUG_P1(log, msg, p1) 
#define LOG_DEBUG_P2(log, msg, p1, p2) 
#define LOG_DEBUG_P3(log, msg, p1, p2, p3) 
#define LOG_DEBUG_P4(log, msg, p1, p2, p3, p4) 
#else //NDEBUG
#define LOG_TRACE(log, msg) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg);}
#define LOG_TRACE_P1(log, msg, p1) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1);}
#define LOG_TRACE_P2(log, msg, p1, p2) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2);}
#define LOG_TRACE_P3(log, msg, p1, p2, p3) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2, p3);}
#define LOG_TRACE_P4(log, msg, p1, p2, p3, p4) {if(log.isTraceEnabled()) log.trace(__FILE__, __LINE__, msg, p1, p2, p3, p4);}

#define LOG_DEBUG(log, msg) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg);}
#define LOG_DEBUG_P1(log, msg, p1) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1);}
#define LOG_DEBUG_P2(log, msg, p1, p2) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2);}
#define LOG_DEBUG_P3(log, msg, p1, p2, p3) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2, p3);}
#define LOG_DEBUG_P4(log, msg, p1, p2, p3, p4) {if(log.isDebugEnabled()) log.debug(__FILE__, __LINE__, msg, p1, p2, p3, p4);}
#endif //NDEBUG
#endif //_LOGGER_H_
