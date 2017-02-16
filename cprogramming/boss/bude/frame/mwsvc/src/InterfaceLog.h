#ifndef  INTERFACELOG_H
#define  INTERFACELOG_H

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
#include "BaseConvert.h"

USING_NAMESPACE(std)

//typedef counted_ptr < CFmlBuf >  PCFmlBuf;

class InterfaceLog
{
    private:
        string sBeginTime;//业务开始时间
        string sEndTime;//业务结束时间
        
        string sTransCode;//交易编码
        string sProvinceCode;//省别编码
        string sInModeCode;//接入方式编码
        string sTradeEparchyCode;//地州编码
        string sTradeCityCode;//业务区编码
        string sTradeDepartId;//部门/渠道标识
        string sTradeStaffId;//员工标识
        string sTradeDepartPasswd;//渠道密码
        
        long lResultCode;//结果代码
        string sResultInfo;//返回信息
        
        //CFmlBuf inBuf;//交易输入FML域
        //CFmlBuf outBuf;//交易输出FML域
        
        string sLogFilePath;//日志文件路径
        string sLogFileName;//日志文件名称
        string sFMLLogFileName;//FML日志文件名称
        
    public:
        //构造函数
        InterfaceLog();
        //析构函数
        ~InterfaceLog();
        
        //赋值
        inline void setBeginTime(const string &beginTime){sBeginTime=beginTime;};
        inline void setEndTime(const string &endTime){sEndTime=endTime;};
              
        string getCurDay();
        string getCurHour();
        
        bool Init(const PCFmlBuf &inBuf, const PCFmlBuf &outBuf);
        
        bool setLogFilePath();//从环境变量中获取
        bool setLogFileName();//根据渠道获取
        
        bool writeLog(const PCFmlBuf &inBuf, const PCFmlBuf &outBuf);//写日志
};

#endif
