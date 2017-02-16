#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <stdarg.h>

#include "base/SmartPtr.h"
#include "config-all.h"

USING_NAMESPACE(std);

struct Grade {
    enum Type {
        WARNING, //警告
        MINOR, //次要
        MAJOR, //重大
        CRITICAL, //严重        
        INDETERMINATE //未确定的
    };
    static const char* toString(Type grade);
};
/**
 * 定义异常的基类 包括异常代码, 异常信息, 所在文件, 异常发生的行号, 引发此异常的根异常
 * @author luzl
 */
class Exception : public exception {
public:
    /** 拷贝构造函数 */
    explicit Exception(const Exception & e);
    
    /** 构造函数 */
    Exception(int code, const string & msg);
    
    /** 构造函数 */
    Exception(int code, const char*  msg, ...);

    /** 构造函数 */
    Exception(const string & file, int line, Grade::Type grade, int code, const string & msg);

    /** 构造函数 */
    template < typename ECAUSE >
    Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const string & msg);
    
    /** 构造函数 */
    Exception(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...);

    /** 构造函数 */
    template < typename ECAUSE >
    Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...);

    /** 析构函数 */
    virtual ~Exception() throw();

    /** 标准C++异常信息 */
    virtual const char * what() const throw();

    /** 获取级别 */
    Grade::Type getGrade() const throw();

    /** 异常代码 */
    int getCode() const throw();

    /** 异常信息 */
    const string & getMessage() const throw();

    /** 文件名 */
    const string & getFile() const throw();

    /** 行号 */
    int getLine() const throw();

    /** 根原因, 如果没有根原因, 则返回 NULL */
    const exception * getCause() const throw();

    /** 克隆当前异常对象 */
    virtual Exception * clone() const throw();

    /** 字符串化异常的全部信息, 包括原因的栈信息 */
    string toString() const;

protected:
    Exception() {}
    
    /** 异常出现的文件名 */
    string m_file;

    /** 异常发生的行号 */
    int m_line;

    /** 错误级别 */
    Grade::Type m_grade;

    /** 异常代码 */
    int m_code;

    /** 异常信息 */
    string m_message;

    /**
     * 引起此异常的根异常, 比如一个居于文件的数据库抛出SQLException, 它的根异常有可能是IOException
     * @supplierCardinality 0..1
     * @label cause
     */
    counted_ptr < exception > m_cause;
};

/** 抛出一个异常 */
#define THROW(expclass, msg) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg)
#define THROW_P1(expclass, msg, p1) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1)
#define THROW_P2(expclass, msg, p1, p2) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2)
#define THROW_P3(expclass, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2, p3)
#define THROW_P4(expclass, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, -1, msg, p1, p2, p3, p4)

#define THROW_C(expclass, code, msg) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg)
#define THROW_C_P1(expclass, code, msg, p1) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1)
#define THROW_C_P2(expclass, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2)
#define THROW_C_P3(expclass, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2, p3)
#define THROW_C_P4(expclass, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, Grade::INDETERMINATE, code, msg, p1, p2, p3, p4)

#define THROW_GC(expclass, grade, code, msg) throw expclass(__FILE__, __LINE__, grade, code, msg)
#define THROW_GC_P1(expclass, grade, code, msg, p1) throw expclass(__FILE__, __LINE__, grade, code, msg, p1)
#define THROW_GC_P2(expclass, grade, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2)
#define THROW_GC_P3(expclass, grade, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2, p3)
#define THROW_GC_P4(expclass, grade, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, grade, code, msg, p1, p2, p3, p4)

/** 抛出一个带原因的异常 */
#define THROW_A(expclass, cause, msg) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg)
#define THROW_A_P1(expclass, cause, msg, p1) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1)
#define THROW_A_P2(expclass, cause, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2)
#define THROW_A_P3(expclass, cause, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2, p3)
#define THROW_A_P4(expclass, cause, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, -1, msg, p1, p2, p3, p4)

#define THROW_AC(expclass, cause, code, msg) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg)
#define THROW_AC_P1(expclass, cause, code, msg, p1) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1)
#define THROW_AC_P2(expclass, cause, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2)
#define THROW_AC_P3(expclass, cause, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2, p3)
#define THROW_AC_P4(expclass, cause, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, Grade::INDETERMINATE, code, msg, p1, p2, p3, p4)

#define THROW_AGC(expclass, cause, grade, code, msg) throw expclass(__FILE__, __LINE__, cause, grade, code, msg)
#define THROW_AGC_P1(expclass, cause, grade, code, msg, p1) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1)
#define THROW_AGC_P2(expclass, cause, grade, code, msg, p1, p2) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2)
#define THROW_AGC_P3(expclass, cause, grade, code, msg, p1, p2, p3) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2, p3)
#define THROW_AGC_P4(expclass, cause, grade, code, msg, p1, p2, p3, p4) throw expclass(__FILE__, __LINE__, cause, grade, code, msg, p1, p2, p3, p4)



/** 定义一个新的异常类型 */
#define DECLARE_EXCEPTION(ExpClass, BaseClass)                                                        \
   class ExpClass : public BaseClass {                                                                   \
   public:                                                                                               \
   ExpClass(const ExpClass& e) :                                                                       \
   BaseClass(e) {                                                                                    \
   }                                                                                                   \
   ExpClass(int code, const string& msg) :                                                                       \
   BaseClass(code, msg) {                                                                                    \
   }                                                                                                   \
   \
   ExpClass(int code, const char*  msg, ...)  { \
     m_line = -1; m_grade = Grade::INDETERMINATE; m_code = code; \
     ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                             \
   }                                                                                                    \
   \
   ExpClass(const string& file, int line, Grade::Type grade, int code, const string & msg)                     \
   :BaseClass(file, line, grade, code, msg) {                                                         \
   }                                                                                                 \
   ExpClass(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...) { \
     m_file = file; m_line = line; m_grade = grade; m_code = code; \
     ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                       \
   }                                                                                                \
   \
   template<typename ECAUSE>                                                                           \
   ExpClass(const string& file, int line, const ECAUSE& cause, Grade::Type grade, int code, const string & msg)\
   :BaseClass(file, line, cause, grade, code, msg) {                                                  \
   }                                                                                                    \
   \
   template < typename ECAUSE >                                                                        \
   ExpClass(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...) { \
    BaseClass::m_file = file; BaseClass::m_line = line; BaseClass::m_grade = grade; BaseClass::m_code = code; \
    ASSIGN_CAUSE(BaseClass::m_cause, cause);                                                                            \
    ASSIGN_VMESSAGE(BaseClass::m_message, msg);                                                                           \
   }                                                                                               \
   \
   Exception* clone() const throw() {                                                                  \
   return new ExpClass(*this);                                                                       \
   }                                                                                                   \
   protected:                                                                                          \
    ExpClass() {}                                                                                     \
   }
   
#include "Exception.hpp"
#endif //_EXCEPTION_H_
