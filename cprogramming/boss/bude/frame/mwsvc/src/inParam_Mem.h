#ifndef  INPARAMMEM_H
/**
 * Connector.cpp v1.0.0
 * Description
 *   the test program for memory leak
 * Author
 *   Nie yanming <nieym@lianchuang.com>
*/
#define  INPARAMMEM_H

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "mwlcu/mwlcu.h"
#include "base/config-all.h"
#include "mwci/CFmlBuf.h"
//#include "base/SmartPtr.h"
using namespace std;

#define MAXCMD_LEN    500

//typedef counted_ptr < CFmlBuf >  PCFmlBuf;

class InParamMem
{
    private:
    	
    		string sTranscode; //流程名   		
    		string sPid;       //进程ID 
    		string sMachineTime; //主机时间
    		string sProcname;   //进程名
    		unsigned long  sUserMemsize; //进程内存大小
    		unsigned long  sUserAllMemsize;//进程总内存大小
    		unsigned long uAllBufSize;//输入BUFFER大小
    		unsigned long uUsedBufSize;//使用BUFFER大小
    		unsigned long uUnUsedBufSize;//未使用的BUFFER大小
    		
    		//IBM
    		unsigned long iHeapsize;
    		unsigned long iStacksize;
    		//HP-UNIX
    		unsigned long uTsize;
    		unsigned long uDsize;
    		unsigned long uSsize;
    		unsigned long uVtsize;
    		unsigned long uVdsize;
    		unsigned long uVssize;    		
    		//string sInbuf; //输入BUFFER
    		string sremark;//备注信息				
        
    public:
        //构造函数
        InParamMem();
        //析构函数
        ~InParamMem();
        
        //赋值      
        string getPid();//获取进程ID
       	void getTime();
				bool init(const PCFmlBuf &buf); 
        int intoFile(const PCFmlBuf &buf);//写文件日志
        int intoFileEnd(const PCFmlBuf &buf);//写文件日志
        int intoFileMiddle(const PCFmlBuf &buf);//写文件日志
        void mem_check_rpt(pid_t pid);
        
};

#endif
