#ifndef _STDFORMATER_H_
#define _STDFORMATER_H_
#include "log4cpp/Formatter.h"

namespace log4cpp {
    class StdFormatter : public Formatter {
    public:
        /**
         * 初始化
         * @param configuration 初始化参数, 它来自于配置文件, 比如logger.formatter.myformatter.xxxx=yyyy
         * 那么name就是xxxx, value就是yyyyy, 而myformatter是格式器的名称
         */
        virtual void init(const map < string, string > & configuration);

        virtual int trace(char * buffer, size_t size, const char * dn, const char * file, int line,
           const char * msg, va_list args);
        virtual int debug(char * buffer, size_t size, const char * dn, const char * file, int line,
           const char * msg, va_list args);

        virtual int state(char * buffer, size_t size, const char * dn, const NameValueList & states);
        virtual int info(char * buffer, size_t size, const char * dn, const char * msg, va_list args);

        virtual int alert(char * buffer, size_t size, const char * dn, Grade::Type grade, int code, const Exception * e,
           const char * msg, va_list args);

        /**  */
        virtual int ok(char * buffer, size_t size, const char * dn, int code);
        
        virtual int pBegin(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) ;
        
        /**
         * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
         */ 
        virtual int pInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) ; 
        // 下面方法分两次调用, 可以记录消耗的时长        
        /**
         * 开始读入
         */ 
        virtual int pBeginInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, const char * fields, va_list args) ;
        /**
         * 完成读入
         */ 
        virtual int pEndInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) ;

		/**
		 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
		 */ 
		virtual int pOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) ;        
		//下面方法分两次调用, 可以记录消耗的时长
		/**
		 * 开始输出
		 */ 
        virtual int pBeginOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, const char * fields, va_list args) ;
        /**
         * 完成输出
         */ 
        virtual int pEndOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) ;
       /**
         * 链接
         */ 
        virtual int pLinkOut(char * buffer, size_t size, const char * dn, const char* pid, const char* source, const char* target);
        
        /**
         * 完成一次执行
         */ 
        virtual int pEnd(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) ;
        
        
        StdFormatter(); 
        virtual ~StdFormatter() throw();
    };
};
#endif //_STDFORMATER_H_
