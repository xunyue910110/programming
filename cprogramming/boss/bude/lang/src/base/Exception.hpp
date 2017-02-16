#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

/** 标准C++异常包装器 */
class __std_exception_wrapper : public exception {
    string m_classname;
    string m_message;

    friend inline ostream & operator << (ostream & out, const __std_exception_wrapper & e) {
        out << e.m_classname << ": " << e.m_message;
        return out;
    }

public:
    __std_exception_wrapper(const exception & e) {
        m_classname = typeid(e).name();
        m_message = e.what();
    }

    const char * what() const throw() {
        return m_message.c_str();
    }

    virtual ~__std_exception_wrapper() throw() {
    }
};


inline ostream & operator << (ostream & out, const exception & e) {
    out << typeid(e).name() << ": " << e.what();
    return out;
}



inline const char* Grade::toString(Grade::Type grade) {
	if(grade == Grade::WARNING)
		return "WARNING";
	else if(grade == Grade::MINOR)
		return "MINOR";
	else if(grade == Grade::MAJOR)
		return "MAJOR";
	else if(grade == Grade::CRITICAL)
		return "CRITICAL";
	else 
		return "INDETERMINATE";	
}

inline ostream & operator << (ostream & out, const Exception & e) {
    out << "[" << Grade::toString(e.getGrade()) << "]" << e.getFile() << ":" << e.getLine() << "," << typeid(e).name() << "-" <<
       e.getCode() << ": " << e.what();
    if (e.getCause() != 0) {
        out << endl << "Caused by: ";
        const Exception * cause = dynamic_cast < const Exception * > (e.getCause());
        if (cause != 0) {
            out << * cause;
        } else {
            const __std_exception_wrapper * cause1 = dynamic_cast < const __std_exception_wrapper * > (e.getCause());
            if (cause1 != 0) {
                out << * cause1;
            } else {
                out << * e.getCause();
            }
        }
    }
    return out;
}

#define ASSIGN_VMESSAGE(m_message, msg) \
    char buf[2048]; \
    va_list ap; \
    va_start(ap, msg); \
    vsnprintf(buf, sizeof(buf), msg, ap); \
    va_end(ap); \
    m_message = buf


inline Exception::Exception(const Exception & e) : m_file(e.m_file), m_line(e.m_line), m_grade(e.m_grade), m_code(e.m_code),
   m_message(e.m_message), m_cause(e.m_cause) {
}

inline Exception::Exception(int code, const string & msg) : m_line(-1), m_grade(Grade::INDETERMINATE), m_code(code), m_message(msg) {
}
    
inline Exception::Exception(int code, const char*  msg, ...) :  m_line(-1), m_grade(Grade::INDETERMINATE), m_code(code)  {
    ASSIGN_VMESSAGE(Exception::m_message, msg);
}  

inline Exception::Exception(const string & file, int line, Grade::Type grade, int code, const string & msg) : m_file(file), m_line(line), m_grade(grade), m_code(code), m_message(msg) {
}

inline Exception::Exception(const string & file, int line, Grade::Type grade, int code, const char*  msg, ...) : m_file(file), m_line(line), m_grade(grade), m_code(code){
    ASSIGN_VMESSAGE(Exception::m_message, msg);    
}

//如果是Exception类型, 利用clone复制原因.
//如果ECAUSE和cause的类型一致, 可以通过copy构造函数复制.
//只能通过包装器, 把必要的信息包装到__std_exception_wrapper中.
#define ASSIGN_CAUSE(m_cause, cause) \
    const Exception * e = dynamic_cast < const Exception * > (& cause); \
    if (e != 0) \
        m_cause = counted_ptr < exception > (e->clone()); \
    else if (typeid(cause) == typeid(const ECAUSE &)) \
        m_cause = counted_ptr < exception > (new ECAUSE(cause)); \
    else \
        m_cause = counted_ptr < exception > (new __std_exception_wrapper(cause))

template < typename ECAUSE >
inline Exception::Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const string & msg) : m_file(file), m_line(line), m_grade(grade), m_code(code), m_message(msg) {
    ASSIGN_CAUSE(Exception::m_cause, cause);
}

template < typename ECAUSE >
inline Exception::Exception(const string & file, int line, const ECAUSE & cause, Grade::Type grade, int code, const char*  msg, ...) : m_file(file), m_line(line), m_grade(grade), m_code(code){
    ASSIGN_CAUSE(Exception::m_cause, cause);
    ASSIGN_VMESSAGE(Exception::m_message, msg);
}

/** 析构函数 */
inline Exception::~Exception() throw() {
}

/** 标准C++异常信息 */
inline const char * Exception::what() const throw() {
    return m_message.c_str();
}

/** 异常代码 */
inline Grade::Type Exception::getGrade() const throw() {
    return m_grade;
}

/** 异常代码 */
inline int Exception::getCode() const throw() {
    return m_code;
}

/** 异常信息 */
inline const string & Exception::getMessage() const throw() {
    return m_message;
}

/** 文件名 */
inline const string & Exception::getFile() const throw() {
    return m_file;
}

/** 行号 */
inline int Exception::getLine() const throw() {
    return m_line;
}

/** 根原因, 如果没有根原因, 则返回 NULL */
inline const exception * Exception::getCause() const throw() {
    return m_cause.get();
}

/** 克隆当前异常对象 */
inline Exception * Exception::clone() const throw() {
    return new Exception(* this);
}

inline string Exception::toString() const {
    ostringstream buffer;
    buffer << * this;
    return buffer.str();
}
#endif //_EXCEPTION_HPP_
