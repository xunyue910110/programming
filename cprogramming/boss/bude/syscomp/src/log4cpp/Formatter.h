#ifndef _FORMATTER_H_
#define _FORMATTER_H_

#include "base/config-all.h"
#include "base/SmartPtr.h"
#include "log4cpp/Logger.h"
#include <string>
#include <map>
#include <stdarg.h>
USING_NAMESPACE(std);

namespace log4cpp {
    /** 定义日志输出器的接口 */
    class Formatter {
    public:
        /**
         * 初始化
         * @param configuration 初始化参数, 它来自于配置文件, 比如logger.formatter.myformatter.xxxx=yyyy
         * 那么name就是xxxx, value就是yyyyy, 而myformatter是格式器的名称
         */
        virtual void init(const map < string, string > & configuration) = 0;

        virtual int trace(char * buffer, size_t size, const char * dn, const char * file, int line, const char * msg,
           va_list args) = 0;
        virtual int debug(char * buffer, size_t size, const char * dn, const char * file, int line, const char * msg,
           va_list args) = 0;

        virtual int state(char * buffer, size_t size, const char * dn, const NameValueList & states) = 0;
        virtual int info(char * buffer, size_t size, const char * dn, const char * msg, va_list args) = 0;

        virtual int alert(char * buffer, size_t size, const char * dn, Grade::Type grade, int code, const Exception * e,
           const char * msg, va_list args) = 0;

        virtual int ok(char * buffer, size_t size, const char * dn, int code) = 0;
        
        virtual int pBegin(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) = 0;
        
        /**
         * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
         */ 
        virtual int pInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) = 0; 
        // 下面方法分两次调用, 可以记录消耗的时长        
        /**
         * 开始读入
         */ 
        virtual int pBeginInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, const char * fields, va_list args) = 0;
        /**
         * 完成读入
         */ 
        virtual int pEndInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) = 0;

		/**
		 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
		 */ 
		virtual int pOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) = 0;        
		//下面方法分两次调用, 可以记录消耗的时长
		/**
		 * 开始输出
		 */ 
        virtual int pBeginOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, const char * fields, va_list args) = 0;
        /**
         * 完成输出
         */ 
        virtual int pEndOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) = 0;
        
        /**
         * 完成一次执行
         */ 
        virtual int pEnd(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) = 0;
        
        /**
         * 链接
         */ 
        virtual int pLinkOut(char * buffer, size_t size, const char * dn, const char* pid, const char* source, const char* target) = 0;

        virtual ~Formatter() throw() { }
    };

    /**
     * 注册输出器宏
     * @name 输出器类型的全名, 建议使用类似java的命名规则 com.linkage.logger.xxxx
     * @classname 类名
     */
#define REGISTER_FORMATTER(name, classname)  \
   Formatter* classname##Creator() { return new classname(); } \
   int classname##CreatorTemp = log4cpp::LoggerImpl::__registerFormatter(name, classname##Creator)


    /** 定义创建输出器的函数类型 */
    typedef Formatter * (* FormatterCreator) ();

    typedef counted_ptr < Formatter > PFormatter;
}

#endif //_FORMATTER_H_
