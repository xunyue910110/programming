#ifndef _APPENDER_H_
#define _APPENDER_H_

#include "base/config-all.h"
#include "base/SmartPtr.h"
#include <string>
#include <map>

USING_NAMESPACE(std);

namespace log4cpp {
/**
 * 定义日志输出器的接口
 */ 
class Appender
{
public:
    /**
     * 初始化
     * @param configuration 初始化参数, 它来自于配置文件, 
     *      比如logger.appender.dailyfile.file=CRMYYMMDD.log
     *      那么name就是file, value就是CRMYYMMDD.log, 而dailyfile是输出器的名称
     */ 
	virtual void init(const map<string, string>& configuration) = 0;
    /**
     * 输出一组数据
     */ 
	virtual void write(const char* data, size_t size) = 0;
    virtual ~Appender() throw () {}
};

/**
 * 注册输出器宏
 * @name 输出器类型的全名, 建议使用类似java的命名规则 com.linkage.logger.xxxx
 * @classname 类名
 */
#define REGISTER_APPENDER(name, classname)  \
    Appender* classname##Creator() { return new classname(); } \
    int classname##CreatorTemp = log4cpp::LoggerImpl::__registerAppender(name, classname##Creator)
/**
 * 定义创建输出器的函数类型
 */ 
typedef Appender* (*AppenderCreator)();

typedef counted_ptr<Appender> PAppender;
}

#endif //_APPENDER_H_
