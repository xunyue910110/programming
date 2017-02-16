#ifndef COMMON_EXCEPTION_HEADER_INCLUDED
#define COMMON_EXCEPTION_HEADER_INCLUDED

//! 异常类，有时候异常可能使用数字等等，所以使用了operator << 的重载，使得抛异常更加容易

#include <string>
#include <string.h>
#include "system.h"

//! 通用异常类， 它重载了<<， 使得抛出更加容易
class common_exception
{
protected:    

    //! 异常发生时的上下文信息
    string                      __Context;
    
public:

    //! 默认构造函数
    common_exception () throw(){__Context = "";};
    
    //! 拷贝构造函数
    common_exception (const common_exception& right) throw(){
        *this = right;
    }
    
    //! 带字符构造函数的异常
    explicit common_exception (const string & right) throw(){
        __Context = right;
    }
    
    //! 带字符构造函数的异常
    explicit common_exception (const char * right) throw(){
        __Context = right;
    }
    
    //! 赋值函数
    common_exception& operator = (const common_exception& right) throw(){
        this->__Context = right.__Context;
        return * this;
    }
    
    //! 获得一个拷贝的异常对象
    common_exception copy(){
        return *this;
    }
    
    //! what接口，目的是为了兼容以前的版本
    virtual string what() const {
        return __Context;
    }
    
    //! 虚拟的析构函数
    virtual ~common_exception () {};
    
    //! 获取上下文信息
    virtual const char * getContext() const{
        return __Context.c_str();
    }
    
    //! 对导入整型值的重载
    common_exception & operator << (int msg){
        char tmp[12];
        sprintf(tmp, "%d", msg);
        __Context += tmp;
        return * this;
    }
    
    //! 对字符串的重载
    common_exception & operator << (const char * msg){
        __Context += msg;
        return * this;
    }
    
    common_exception & operator << (double msg){
        char tmp[12];
        sprintf(tmp, "%f", msg);
        __Context += tmp;
        return * this;
    }
    
    common_exception & operator << (void * msg){
        char tmp[12];
        sprintf(tmp, "%X", msg);
        __Context += tmp;
        return * this;
    }
    
    common_exception & operator << (const string & msg){
        __Context += msg;
        return * this;
    }
    
    common_exception & operator << ( ostream & (* msg)(ostream &)) {
    	char c = '\n';
    	__Context += c;
    	return * this;
    }
    
};

typedef common_exception CException;

//! 带编码信息的异常
class coded_exception : public common_exception
{
protected:

    //! 错误代码
    int                 __ErrorCode;

    //! 该错误代码所对应的通用错误信息
    string              __ErrorCodeMsg;
    
public:
    
    //! 构造函数
    coded_exception(int error_code, string error_code_msg = "", string context = "") throw()
    {
        __ErrorCode = error_code;
        __ErrorCodeMsg = error_code_msg;
        __Context = context;
    }
    
    //! 赋值重载
    coded_exception & operator = (const coded_exception & right){
        this->__ErrorCode = right.__ErrorCode;
        this->__ErrorCodeMsg = right.__ErrorCodeMsg;
        this->__Context = right.__Context;
        return *this;
    }
    
    //! what 重载，兼容以前的版本
    virtual string what() const {
        char tmp[16];
        sprintf(tmp, "%d", __ErrorCode);
        return string(tmp) + "," + __ErrorCodeMsg + "," + __Context;
    }
    
    //! 拷贝构造
    coded_exception(const coded_exception & right){
        *this = right;
    }
    
    //! 获得一个拷贝的对象
    coded_exception copy() const {
        return * this;
    }
    
    //! 获取错误代码
    int getErrorCode() const {
        return __ErrorCode;
    }
    
    //! 获取该错误代码所对应的通用错误信息
    string getErrorCodeMsg() const {
        return __ErrorCodeMsg;
    }
    
    //! 判断两个异常是否相等
    bool operator == (const coded_exception & right ) const {
        return this->__ErrorCode == right.__ErrorCode;
    }
    
    
    //! 对导入整型值的重载
    coded_exception & operator << (int msg){
        char tmp[12];
        sprintf(tmp, "%d", msg);
        __Context += tmp;
        return * this;
    }
    
    //! 对字符串的重载
    coded_exception & operator << (const char * msg){
        __Context += msg;
        return * this;
    }
    
    coded_exception & operator << (double msg){
        char tmp[12];
        sprintf(tmp, "%f", msg);
        __Context += tmp;
        return * this;
    }
    
    coded_exception & operator << (void * msg){
        char tmp[12];
        sprintf(tmp, "%X", msg);
        __Context += tmp;
        return * this;
    }
    
    coded_exception & operator << (const string & msg){
        __Context += msg;
        return * this;
    }
};

#endif /* COMMON_EXCEPTION_HEADER_INCLUDED */
