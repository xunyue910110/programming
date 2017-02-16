#include "log4cpp/HessianFormatter.h"

#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>   
#include <arpa/inet.h> 

#include <map>
#include <string>
#include <iostream> 
#include "base/exceptions.h"
#include "hessian/hessian_output.h"
#include "hessian/hessian_input.h"
#include "hessian/string_input_stream.h"
#include "hessian/dump.h"

USING_NAMESPACE(std);
using namespace log4cpp;
using namespace hessian;

typedef map<string,Object*> StringMap;
typedef map<string,Object*>::value_type StringMapValue;
#define HESSIAN_BUFFER_SIZE 1024


void HessianFormatter::init(const map < string, string > & configuration) {     
    char name[128];
    if(gethostname(name,sizeof(name)) == -1){
        m_localaddr = "";
    }else{
        struct hostent* ent = gethostbyname(name);
        m_localaddr = string(inet_ntoa(*(struct in_addr *)*ent->h_addr_list));
    }
    map < string, string >::const_iterator it = configuration.find("localcode");
    if(it!=configuration.end()) {
        m_localCode = it->second;    
    } else {
        m_localCode = "GB18030";
    }
}


int64_t getDateTime(){
    struct timeval current;                       
    gettimeofday(& current, NULL);
    int64_t seconds = (int64_t)current.tv_sec*1000 + current.tv_usec/1000;
    return (int64_t)seconds;
}

int HessianFormatter::trace(char * buffer, size_t size, const char * dn, const char * file, int line,
   const char * msg, va_list args) {
    int offset = 0;
    hessian_output out(m_localCode.c_str());
    
    string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("TRACE")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_file",new String(file)));
    dataMap.insert(StringMapValue("log_line",new Integer(line)));
    
    char message[HESSIAN_BUFFER_SIZE];
    vsnprintf(message,HESSIAN_BUFFER_SIZE,msg,args);
    dataMap.insert(StringMapValue("log_message",new String(message)));
    Map hessianMap(dataMap);
    
    out.write_object(call,&hessianMap); 
    ASSERT(size > call.size());

    memcpy(buffer, call.c_str() ,call.size());
    return call.size();
}

int HessianFormatter::debug(char * buffer, size_t size, const char * dn, const char * file, int line,
   const char * msg, va_list args) {
    int offset = 0;
    hessian_output out(m_localCode.c_str());
    
    string call ;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("DEBUG")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_file",new String(file)));
    dataMap.insert(StringMapValue("log_line",new Integer(line)));
    
    char message[HESSIAN_BUFFER_SIZE];
    vsnprintf(message,HESSIAN_BUFFER_SIZE,msg,args);
    dataMap.insert(StringMapValue("log_message",new String(message)));
    Map hessianMap(dataMap);
    
    out.write_object(call,&hessianMap);
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}                      

int HessianFormatter::state(char * buffer, size_t size, const char * dn, const NameValueList & states) {     
    int offset = 0;        
    hessian_output out(m_localCode.c_str());
    
    string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("STATE")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    for(size_t i = 0 ; i < states.size() ; i ++){ 
        dataMap.insert(StringMapValue(states[i].name, new String(states[i].value)));
    }
    Map hessianMap(dataMap);
    
    out.write_object(call,&hessianMap);
  ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
  return call.size(); 
}

int HessianFormatter::info(char * buffer, size_t size, const char * dn, const char * msg, va_list args) {
    int offset = 0;
    hessian_output out(m_localCode.c_str());
    
    string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("INFO")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    
    char message[HESSIAN_BUFFER_SIZE];
    vsnprintf(message,HESSIAN_BUFFER_SIZE,msg,args);
    dataMap.insert(StringMapValue("log_message",new String(message)));
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);                        
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    return call.size();                       
}

static string& getfirstline(string& msg) {
    size_t lnpos = msg.find('\n');
    if(lnpos != string::npos)
        msg = msg.substr(0, lnpos);
    return msg;      
}

static ostream & exp2str(ostream & out, const Exception & e) {    
    //只取第一行错误信息
    string msg = e.getMessage();
    getfirstline(msg);
    
    //生成改异常信息    
    out << "[" << Grade::toString(e.getGrade()) << "]" << e.getFile() << ":" << e.getLine() << "," << typeid(e).name() << "-" <<
       e.getCode() << ": " << msg;
       
    if (e.getCause() != 0) {
        out << endl << "Caused by: ";
        const Exception * cause = dynamic_cast < const Exception * > (e.getCause());
        if (cause != 0) {
            exp2str(out, * cause);
        } else {
            string strcause(e.getCause()->what());
            getfirstline(strcause);
            out << strcause;
        }
    }
    return out;
}

int HessianFormatter::alert(char * buffer, size_t size, const char * dn, Grade::Type grade, int code, const Exception * e,
   const char * msg, va_list args) { 
    int offset = 0;     
    char message[HESSIAN_BUFFER_SIZE];
    hessian_output out(m_localCode.c_str());
    string call;
    StringMap dataMap;
    string type ;
    if(e == 0){
        type = code == -1 ? "MAJOR" : Grade::toString(grade);        
        vsnprintf(message,HESSIAN_BUFFER_SIZE,msg,args);
        dataMap.insert(StringMapValue("log_message",new String(message)));    
    }else{        
        type = code == -1 ? Grade::toString(e->getGrade()) : Grade::toString(grade);
        if(msg != 0){              
            vsnprintf(message,HESSIAN_BUFFER_SIZE,msg,args);                                 
            dataMap.insert(StringMapValue("log_message",new String(message)));
        }else{    
            dataMap.insert(StringMapValue("log_message",new String(e->what())));
        }
        //取各异常的第一行组成一个串
        ostringstream out;
        exp2str(out, *e);
        string exp = out.str();
        
        //如果超过HESSIAN_BUFFER_SIZE, 我们截一下
        if(exp.size() > HESSIAN_BUFFER_SIZE)
          exp = exp.substr(0, HESSIAN_BUFFER_SIZE);
          
        dataMap.insert(StringMapValue("log_exception",new String(exp)));
    }
    dataMap.insert(StringMapValue("log_type",new String(type)));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_code",new Integer(code)));
    dataMap.insert(StringMapValue("log_grade",new Integer(grade)));
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    //msg最大长度是HESSIAN_BUFFER_SIZE, exception的最大长度是HESSIAN_BUFFER_SIZE,
    //也就是HESSIAN_BUFFER_SIZE*2 = 2048, 再加上其他等等, 应该不会超过size(4096) 
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());

    return call.size();
}



int HessianFormatter::ok(char * buffer, size_t size, const char * dn, int code) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("OK")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_code",new Integer(code)));
    
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}

int HessianFormatter::pBegin(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_progress",new String("BEGIN")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}

void HessianFormatter::parse(const char* fullname,string& output){
    output = string(fullname);
    size_t url = output.find("://");
    if(url == string::npos){
        if(output[0] != '/'){
            output = "/" + output;
        }
        output = "file://" + m_localaddr + output;
        url = 7;
    }
    //去除重复的"//"
    string::size_type pos = url;
    while( pos < output.length() && (pos=output.find("//", pos))!=string::npos )
    {
        output.replace(pos, 2, "/");
        pos += 1;
    }
}

/**
 * 执行一次读入, 调用一个方法就完成, 但不记录该次输入的消耗时长
 */ 
int HessianFormatter::pInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_count",new Integer(count)));
    dataMap.insert(StringMapValue("log_progress",new String("INPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}
// 下面方法分两次调用, 可以记录消耗的时长        
/**
 * 开始读入
 */ 
int HessianFormatter::pBeginInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_progress",new String("BEGIN_INPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size(); 
}
/**
 * 完成读入
 */ 
int HessianFormatter::pEndInput(char * buffer, size_t size, const char * dn, const char* pid, const char* fullname, size_t count, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_count",new Integer(count)));
    dataMap.insert(StringMapValue("log_progress",new String("END_INPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}

/**
 * 执行输出, 调用一个方法就完成, 但不记录该次输出的消耗时长
 */ 
int HessianFormatter::pOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args)  {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_count",new Integer(count)));
    dataMap.insert(StringMapValue("log_progress",new String("OUTPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}
//下面方法分两次调用, 可以记录消耗的时长
/**
 * 开始输出
 */ 
int HessianFormatter::pBeginOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_progress",new String("BEGIN_OUTPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}
/**
 * 完成输出
 */ 
int HessianFormatter::pEndOutput(char * buffer, size_t size, const char * dn, const char* pid,  const char* fullname, size_t count, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _fullname;
    parse(fullname, _fullname);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_fullname",new String(_fullname)));
    dataMap.insert(StringMapValue("log_count",new Integer(count)));
    dataMap.insert(StringMapValue("log_progress",new String("END_OUTPUT")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}

int HessianFormatter::pLinkOut(char * buffer, size_t size, const char * dn, const char* pid, const char* source, const char* target) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
    string _source;
    parse(source, _source);
    string _target;
    parse(target, _target);
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_progress",new String("LINKOUT")));
    dataMap.insert(StringMapValue("log_source",new String(_source)));
    dataMap.insert(StringMapValue("log_target",new String(_target)));
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}

/**
 * 完成一次执行
 */ 
int HessianFormatter::pEnd(char * buffer, size_t size, const char * dn, const char* pid, const char * fields, va_list args) {
     int offset = 0;
    hessian_output out(m_localCode.c_str());
     string call;
    StringMap dataMap;
    dataMap.insert(StringMapValue("log_type",new String("PROC")));
    dataMap.insert(StringMapValue("log_time",new Long(getDateTime())));
    dataMap.insert(StringMapValue("log_dn",new String(dn)));
    dataMap.insert(StringMapValue("log_pid",new String(pid)));
    dataMap.insert(StringMapValue("log_progress",new String("END")));
    if(fields !=0){
        char field[HESSIAN_BUFFER_SIZE];
        vsnprintf(field,HESSIAN_BUFFER_SIZE,fields,args);
        dataMap.insert(StringMapValue("log_fields",new String(field)));
    }
    Map hessianMap(dataMap);
    out.write_object(call,&hessianMap);
    
    ASSERT(size > call.size());
    memcpy(buffer, call.c_str() ,call.size());
    
    return call.size();
}



HessianFormatter::HessianFormatter() {
}

HessianFormatter::~HessianFormatter() throw() {
}
