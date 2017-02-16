// Language:    C++
// OS Platform: UNIX
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// Comments:

#ifndef ReportApp_H_INCLUDED_C4353D06
#define ReportApp_H_INCLUDED_C4353D06
#include "appfrm/Application.h"
//#include "State.h"
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
#include <signal.h>
#include <unistd.h>
#include "Report.h"
#include "ReadParam.h"
#include "ReportProc.h"
#include "basefunc.h"

//#define PROC_EXIT  '2'

//该类是Application的子类，主要是四个虚函数
//     |-- 1[VF] initialization()
//     |          |-- (1)setoptstr("c:") //父类中的保护成员函数,给m_optstring赋值
//     |          |-- (2)Application::initialization()//parse;creat/init;updatestatus
//     |          |-- (3)set_asdaemon(true);set_onlyone(true);set_runinbkg(true);//为基类的preLoop做参数准备
//     |          |-- (4)create/init ReadParam(针对参数表),ReportProc;//loopProcess()中将用到这些类
//     |          |-- (5)getSysParam()//本类的私有成员函数
//                           |-----从boss.conf中得到参数
//     |                     |-----FlushTimeVector();//从参数表中读出数据，刷新抽象过的结构(用于放日报，月报参数表的内容)
//                                   |-----theParam(ReadParam)->readParam()   
//     |--2[VF] recovery()
//     |          |-- Application::recovery();
//（父类中preloop  (主要功能是设置进程环境),
//        mainloop (mainloop中再调loopProcess, while(!m_stop)一直调用loopProcess())
//     |--3[VF] loopProcess()虚函数,该类主要是这个函数
//     +--4[VF] end()
//                |-- Application::end()
//-------------------------------------------------------------------------//

TyptimeVector timeVector;
TypdayVector  dayVector;
TypotherVector otherVector;

dayit   dayiter;
otherit otheriter;
timeit  timeiter;

//void FlushTimeVector();//??下面有,不应该写在这

char dbServer[64];
char dbUser[64];
char dbPassword[64];
char dbName[64];
char reportTableName[64];

//ReadParam *theParam;//也不应写在这 ??

class ReportApp : public Application
{
 public:

 protected:
    virtual bool initialization();
    virtual bool recovery();
    virtual bool loopProcess();
    virtual bool end();
    
    ReportProc *m_reportProcess;
    ReadParam  *theParam;  //added by cy
     
    int  m_loopNumber;
    int  m_flushNumber;
    char m_curday[30];
  
 private:
    bool getSysParam();//既从boss.conf也从参数表 取参数

//  void writeLog();
  
    void FlushTimeVector();
};
#endif /* ReportApp_H_INCLUDED_C4353D06 */
