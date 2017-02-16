#include "base/exceptions.h"
#include "log4cpp/Logger.h"
#include <cerrno>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include "base/supdebug.h"

USING_NAMESPACE(std);
using namespace log4cpp;
DECLARE_EXCEPTION(TDAOException, Exception);

DECLARE_EXCEPTION(TCBSException, Exception);
Logger& logger = Logger::getLogger("eboss.crm.demo");
const int EUNXFILEOPEN = 10110;

class DAO {

public:
    void op1() {
        try {
            vector < int > v;
            //v.at(1) = 1;
        } catch (const exception & e) {
            THROW_AGC(TDAOException, e, Grade::WARNING, 100, "查询失败");
            //THROW_AC_P2(TCBSException, e, 100, "文件%s, 第%d字段", "file1", 3);
        }
        //
        //自行构造一个异常(cause)作为原因, 添加的当前需要抛出的异常中, 可用于包装系统原始异常.(PrimalException)
        //THROW_AC_P1(TCBSException, NetException(errno, strerror(errno)), 10, "打开端口%d失败", 23);         
    }
};


class B {
public:
    int op2() {
        DAO a;
        try {
          a.op1();
        } catch (const Exception & e) {
	    logger.alert(e);
            THROW_AC_P1(TCBSException, e, EUNXFILEOPEN, "执行XXX失败%d!", 100);
            //throw TCBSException(__FILE__, __LINE__, Grade::WARNING, EUNXFILEOPEN, "执行XXX失败%d!", 100);
        }
        return 100;
    }
};


class C {
    B b;
public:
    void op3() {
        LOG_TRACE(logger, "进入op3函数");
        try {
        	//while(true){
        	//while(true){
        	FP_BEGIN(info);
            logger.info("info");
            FP_END(info);
            
            //sleep(1);
          //}
            NameValueList list;
            list.push_back(NameValue("n1","v"));
            list.push_back(NameValue("n2","v"));
            FP_BEGIN(state); 
            logger.state(list);
            FP_END(state);
            int j = 0 ;
            while(j < 0){
            	FP_BEGIN(alert);
            ostringstream buf;
            buf<<"alert"<<j;
            logger.alert(buf.str().c_str());
            logger.alert(Grade::WARNING, 100, buf.str().c_str()/*"alert2"*/);
            	j++;
            	FP_END(alert);
            }
            logger.ok();
            logger.ok(100);
          int i = 100;
          while(i> 0){
    FP_BEGIN(proc);      	
	  string pid = logger.pBegin();
	  logger.pBeginInput(pid, "file1");
	  logger.pEndInput(pid, "file1", 100, "f1=2,f2=3");
	  logger.pBeginOutput(pid, "file2");
	  logger.pEndOutput(pid, "file2", 101);
	  logger.pInput(pid, "file3", 103);
	  logger.pOutput(pid, "file4", 105, "f3=3");
	  logger.pLinkOut(pid, "file2", "file3");
	  logger.pEnd(pid);
	  FP_END(proc);
	   i--;
	 }
	  //  for(int i=0; i<100; i++) {
		//LOG_TRACE(logger, "...");
		//sleep(1);
	  //  }
            LOG_TRACE(logger, "调用op2");            
            int r = 
			b.op2();
            LOG_DEBUG_P1(logger, "op2结果为%d", r);
            sleep(1);
         // }
        } catch (const Exception & e) {
            logger.alert(e);
        }
        LOG_TRACE(logger, "退出op3函数");
    }
};


int main() {
    C c;
    try {
        c.op3();
    } catch (const std::exception & e) {
        cout << e.what() << endl;
    } catch (...) {
    }
}

