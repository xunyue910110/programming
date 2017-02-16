// Language:    C++
// OS Platform: UNIX
// Authors:     Weiguo Wang <wangwg@lianchuang.com>
// History:
// Copyright (C) 2001 by Weiguo Wang, Linkage. All Rights Reserved.
// Comments:


#include "config-all.h"

#include "ReportApp.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"

#include <assert.h>
#include <unistd.h>

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
USING_NAMESPACE(std);
//using namespace std;


//-------------------------------------------------------------------------//
// application object, which will replaced by your application
//-------------------------------------------------------------------------//
ReportApp g_application;
Application * const theApp = &g_application;


bool ReportApp::loopProcess()
{
    char thefiledate[30],thelimitdate[30];
    char tempstr[30],tempstr1[30],alarmMsg[256];
    char theCurDate[15];  //xiac add a valuable to save the current date 
    long lNowMinutes,lTemp;
    int iReturn,vectorSize,i;
    i = 0;
    vectorSize = timeVector.size();

    if(m_flushNumber == 0)
        FlushTimeVector();
    
    memset(theCurDate,0 ,sizeof(theCurDate));
    getcdate(theCurDate);
    
    getcdate(thefiledate);//初始化 thefiledate 为当天日期
    //处理时跨过一天的24点将满足条件  (m_curday只出现在这)
    if (strcmp(m_curday,thefiledate) !=0)
    {
      sprintf(m_curday,thefiledate);
      FlushTimeVector();//从参数表中读出数据，刷新抽象过的结构(用于放日报，月报参数表的内容)
    }
    
    
    //第一次执行出来后i=0,m_loopNumber=0;以后每次都从1开始，直到最后一条参数
    //timeiter,i  追随m_loopNumber,目的是timeiter每次加1;技巧是:  处理的大段可以不在本for内 
    for(timeiter=timeVector.begin();(timeiter!=timeVector.end())&&(i<m_loopNumber);timeiter++,i++)
      ;
      
    if (timeiter==timeVector.end())
    {  
        //开始新的循环
        m_loopNumber = 0;
        timeiter=timeVector.begin();
    }	

#ifdef _DEBUG_
    cout<<"timeiter->reportNo:"<<timeiter->reportNo<<endl;
    cout<<"timeiter->lastProcessDate:"<<timeiter->lastProcessDate<<endl;
    cout<<"timeiter->reportDate:"<<timeiter->reportDate<<endl;
    cout<<"timeiter->processTime:"<<timeiter->processTime<<endl;
    cout<<"timeiter->processLimitTime:"<<timeiter->processLimitTime<<endl;
    cout<<"timeiter->receiveFlag:"<<timeiter->receiveFlag<<endl;
#endif
    
    //处理主要是 1.给文件名的日期成分thefiledate赋值    2.判是否到了该处理的时间了
    //          3.处理 生成文件 或 接收文件
    
    if (timeVector.begin()!=timeVector.end()) //如果有参数，即记录数不为0
    {
        lNowMinutes = getltime();
        getcdate(thefiledate);//初始化 thefiledate 为当天日期
        dayadd(thelimitdate,thefiledate,-10);//取阀值时间（防止汇总文件缺号）
        
        //取得文件名中的日期信息是关键
        if ((timeiter->reportFlag==1)&&(m_status != PROC_EXIT))//1.日报文件处理
        { 
            //lTemp为现在分钟数与参数处理时间之差
            lTemp = lNowMinutes - timeiter->processTime;
            
            //1.当timeiter->processTime为正值时,thefiledate取当前日前一天日期
            //2.为负则thefiledate取当天的日期(前面已初始化)
            if((lTemp<=timeiter->processLimitTime) &&(lTemp>=0))
                dayadd(thefiledate,thefiledate,-1);

            //(1)已过处理时间，(2)但在时限范围内,即应该处理了
            //timeiter->processTime有可能为负数,lTemp就有可能 >1440
            if(((lTemp<=timeiter->processLimitTime) &&(lTemp>=0))||
               (((lTemp-1440)<=timeiter->processLimitTime) &&((lTemp-1440)>=0)))
            {
            	//3.如果是接收类型，则将上次处理日期的次日 赋值给thefiledate
                sprintf(tempstr,timeiter->lastProcessDate);//上次处理的文件日期
                dayadd(tempstr,tempstr,1);
                if (timeiter->receiveFlag == 0)
                    sprintf(thefiledate,tempstr);
                if (strcmp(thefiledate,thelimitdate) < 0)
                    sprintf(thefiledate,thelimitdate);
                    
                    
            	while(strcmp(thefiledate,theCurDate) <= 0 && strcmp(timeiter->lastProcessDate, theCurDate)<0 )//这样接收日报性质停止处理几天，也可补处理(&& added by cy)
            	{
            	    //tempstr此时放的是 上次处理日期的次日
            	    //(1)上发(2)有几天停发时会满足下面条件
                    if((atol(thefiledate)>atol(tempstr))&&(timeiter->receiveFlag == 1))
                    {
                      int nTotal,nSuccess,nError;
                      char selectarray[1000];
                      //1写告警
                      sprintf(tempstr1,thefiledate);
                      lTemp=timediff(tempstr1,tempstr);
                      sprintf(alarmMsg,"前%d天的日汇总文件未处理,汇总号:%d",lTemp,timeiter->reportNo);
#ifdef _DEBUG_
  cout<<alarmMsg<<endl;
#endif
                      m_theErrorMessages->insert(MISS_REPORT,alarmMsg);
                      //2更新参数表中lastprocessdate 为当前日期的前一天
                      theParam->connectDb();
                      getcdate(tempstr1);
                      dayadd(tempstr1,tempstr1,-1);
                      sprintf(timeiter->lastProcessDate,tempstr1);
                      sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
                                           DAY_PARAM_TABLE,tempstr1,timeiter->reportNo);
                      theParam->dbi.executeSql(selectarray,nTotal,nSuccess,nError);
                      theParam->dbi.disconnect();
                    }
                   
                    //thefiledate此时已确定 
                    //开始真正的处理          
                    m_reportProcess->getFileName(timeiter->reportFlag,timeiter->reportNo,thefiledate);

                    m_status = PROC_RUN;
                    m_objectName = m_reportProcess->m_fileName;
#ifdef _DEBUG_
  cout<<"ReportApp::loopProcess File name:"<<m_objectName<<endl;
#endif
                    updateStatus();//在监控上显示正在处理哪个文件

                    //modify by cy 2003.06 for 新统计报表 正值代表生成的文件名为前一天，负值代表生成的文件名为系统现在日期
                    //正值时，不修改程序将多产生一个文件名为系统当日的文件
                    
                    //if (m_reportProcess->mainProcess() > 0)
                    //    sprintf(timeiter->lastProcessDate,m_reportProcess->m_fileDate);
                    //dayadd(thefiledate,thefiledate,1);

                    
                    iReturn=m_reportProcess->mainProcess();
                    if      (iReturn> 0 && timeiter->processTime <0)
                        sprintf(timeiter->lastProcessDate,m_reportProcess->m_fileDate);
                    else if (iReturn> 0 && timeiter->processTime >=0 &&timeiter->receiveFlag ==1)
                    {
                        char selectarray[1000];
                        int nTotal,nSuccess,nError;
                        dayadd(timeiter->lastProcessDate,m_reportProcess->m_fileDate,1);
                        sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
                                             DAY_PARAM_TABLE,timeiter->lastProcessDate,timeiter->reportNo);
                        theParam->connectDb();
                        theParam->dbi.executeSql(selectarray,nTotal,nSuccess,nError);
                        theParam->dbi.disconnect();
                    }
                       
                       
                         
                    dayadd(thefiledate,thefiledate,1);
                }//while 应处理的文件日期 <=今天  并且  上次处理日期<今天
                
            }//已过处理时间，但在时限范围内,即应该处理了
        }//1.日报
        
        
        else if ((timeiter->reportFlag==0)&&(m_status != PROC_EXIT))//2.其他文件处理
        {
            //processLimitTime没用上 ??
            if( strncmp(timeiter->lastProcessDate,"Y",1) != 0 )
            {
                //那一天，过了时间;  或过了那一天
                if(((lNowMinutes>=timeiter->processTime)&&(atol(thefiledate)==timeiter->reportDate))||
                   (atol(thefiledate)>timeiter->reportDate))
                {
                  m_reportProcess->getFileName(timeiter->reportFlag,timeiter->reportNo,thefiledate);

                  m_status = PROC_RUN;
                  m_objectName = m_reportProcess->m_fileName;
#ifdef _DEBUG_
  cout<<"File name:"<<m_objectName<<endl;
#endif
                  updateStatus();//在监控上显示正在处理哪个文件
                  
                  if (m_reportProcess->mainProcess() > 0)
                      sprintf(timeiter->lastProcessDate,"Y");
                }
            }
        }//1日报，2其他文件处理
    }//if (timeVector.begin()!=timeVector.end()) //如果有参数，即记录数不为0  
        
    //每循环10次读一下参数
    if (m_loopNumber == 0)
    {
      m_flushNumber++;
      m_flushNumber = m_flushNumber%10;
    }

    writeAlert();
    
    m_status = PROC_IDLE;
    m_objectName = "";
    updateStatus();


    sleep(3);      
#ifndef _DEBUG_
    //if (m_loopNumber == 0)
      sleep(3);
#endif
    
    m_loopNumber++;
    
    return Application::loopProcess();
}


bool ReportApp::initialization()
{

    setoptstr("c:");
    if (!Application::initialization())
    	return false;
    
#ifndef _DEBUG_
  set_asdaemon(true);
  set_onlyone(true);
  set_runinbkg(true);
#endif


    theParam        = new ReadParam(&dayVector,&otherVector);//这个参数指表  td_otherreport
    m_reportProcess = new ReportProc(&dayVector,&otherVector);//创建ReportProc对象
    m_loopNumber  = 0;
    m_flushNumber = 0;
    
    if (!getSysParam())
      return false;
    
    return true;
}


bool ReportApp::recovery()
{
    Application::recovery();
    return true;	
}

bool ReportApp::end()
{
    return Application::end();
}



//1.从boss.conf中取参数   2.从参数表中取参数
bool ReportApp::getSysParam()
{
    string path, name, value;
    char channelNo[8];
    string root = m_name;//不含路径也不含参数的命令名
    sprintf(channelNo, "%d", m_channelNo);
    path = SECTDELIM + root + SECTDELIM + channelNo;

// boss.conf::<RoamReport>
// boss.conf::   <0>
// boss.conf::      DB_SERVER                 =  rcpj2
// boss.conf::      DB_USER                   =  roam
// boss.conf::      DB_PASSWORD               =  roam123
// boss.conf::      DB_NAME                   =  roam
// boss.conf::      REPORTTABLE_NAME          =  tgm_collect
// boss.conf::   </0>
// boss.conf::</RoamReport>
    //1.从boss.conf中取参数
    name = "DB_SERVER";//1
    if (!m_theSysParam->getValue(path, name, value)) //m_theSysParam父类的成员,在父类的initialization中实化
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
      return false;
    }
    sprintf(dbServer,value.c_str());
    
    name = "DB_USER";//2
    if (!m_theSysParam->getValue(path, name, value))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
      return false;
    }
    sprintf(dbUser,value.c_str());
    
    name = "DB_PASSWORD";//3
    if (!m_theSysParam->getValue(path, name, value))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
      return false;
    }
    sprintf(dbPassword,value.c_str());
    
    name = "DB_NAME";//4
    if (!m_theSysParam->getValue(path, name, value))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
      return false;
    }
    sprintf(dbName,value.c_str());
    
    name = "REPORTTABLE_NAME";//5
    if (!m_theSysParam->getValue(path, name, value))
    {
      m_theErrorMessages->insert(EAPPFRM_SYSVAL,path + SECTDELIM + name);
      return false;
    }
    sprintf(reportTableName,value.c_str());
    
    
    //2.从参数表中取参数
    FlushTimeVector();//将调theParam(ReadParam)->readParam() 
    
    if (m_status == PROC_EXIT)
      return false;
    return true;
}

//从参数表中读出数据，刷新抽象过的结构(用于放日报，月报参数表的内容)
void ReportApp::FlushTimeVector()
{   
    char cDay[8+1],cTime[8+1],cTemp[8+1],alarmMsg[256];
    long lTemp;
    int iReturn,nTotal,nSuccess,nError,i;
    char selectarray[1000];
    
    for (i=0;(i<3)&&(theParam->connectDb()<0);i++)//connect to DB for 3 times
      sleep(3);
    if (i >= 3)
      return;
    
    iReturn = theParam->readParam();//全局结构dayVector,otherVector中正常时就有内容了
    if(iReturn<0)
    {
      sprintf(alarmMsg,"Read parameter error.");
      m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
      m_status = PROC_EXIT;
    }
#ifdef _DEBUG_
    cout<<"读取两张参数表完成"<<endl;
#endif
    //初始化目标容器
    timeVector.clear();
    timeVector.resize(dayVector.size()+otherVector.size());
    timeiter=timeVector.begin();
    //下面将给timeiter赋值
   
    //1.日报参数表 
    for(dayiter=dayVector.begin();dayiter!=dayVector.end();dayiter++)
    {       
        if ((dayiter->flag!=0)&&(dayiter->flag!=1))
        {
          sprintf(alarmMsg,"Flag error:value is %d.",dayiter->flag);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        
        sprintf(cTemp,dayiter->fileNameDate);
        if ((strcmp(cTemp,"YYYYMMDD")!=0)&&
            (strcmp(cTemp,"YYMMDD")!=0)&&
            (strcmp(cTemp,"YYYYMM")!=0)&&
            (strcmp(cTemp,"MMDD")!=0))
        {
          sprintf(alarmMsg,"File name date error:value is %s.",dayiter->fileNameDate);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        
        if (abs(dayiter->processTime) > 2400)
        {
          sprintf(alarmMsg,"Process time error:value is %d.",dayiter->processTime);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        
        if (abs(dayiter->processLimitTime) > 2400)
        {
          sprintf(alarmMsg,"Process limit time error:value is %d.",dayiter->processLimitTime);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        //1
        timeiter->reportNo=dayiter->reportNo;
        //2
        lTemp=dayiter->processTime;
        timeiter->processTime=(long)((lTemp/100)*60 + (lTemp%100));//processTime转换为分钟，即0130->90
        //3
        getcdate(cTemp);
        timeiter->reportDate=atol(cTemp);//日报处理中timeiter->reportDate 取值为今天
        //4
        timeiter->reportFlag=1;
        //5
        timeiter->receiveFlag=dayiter->flag;
        //6
        if(strcmp(dayiter->lastProcessDate,"")==0)
        {
            //lastProcessDate 如原为空，则设为当前日的前一天
            dayadd(cTemp,cTemp,-1);
            sprintf(timeiter->lastProcessDate,cTemp);
            sprintf(selectarray,"UPDATE %s SET lastprocessdate='%s' WHERE reportno=%d",
                                 DAY_PARAM_TABLE,timeiter->lastProcessDate,timeiter->reportNo);
            theParam->dbi.executeSql(selectarray,nTotal,nSuccess,nError);
        }
        else
            sprintf(timeiter->lastProcessDate,dayiter->lastProcessDate);
        //7
        timeiter->processLimitTime = dayiter->processLimitTime;
        
        timeiter++;
    }//for 1.日报参数表 
   
    //2.月报及其他参数表
    for(otheriter=otherVector.begin();otheriter!=otherVector.end();otheriter++)
    {  
        if ((otheriter->flag!=0)&&(otheriter->flag!=1))
        {
          sprintf(alarmMsg,"Flag error:value is %d.",otheriter->flag);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        //1处理日的判断
        memset(cDay,0,sizeof(cDay));
        strncpy(cDay,otheriter->processTime,8);//上面的日汇总processTime为小时分钟
        if(CheckDate(atol(cDay))==-1)
        {
          sprintf(alarmMsg,"File name date error:value is %s.",otheriter->processTime);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        //2处理时间的判断
        memset(cTime,0,sizeof(cTime));
        strncpy(cTime,otheriter->processTime+8,4);
        if (atol(cTime) > 2400)
        {
          sprintf(alarmMsg,"Process time error:value is %s.",cTime);
          m_theErrorMessages->insert(REPORT_PARAM_ERROR,alarmMsg);
          m_status = PROC_EXIT;
        }
        
        //1
        timeiter->reportNo=otheriter->reportNo;
        //2
        timeiter->reportDate=atol(cDay);
        //3
        lTemp=atol(cTime);
        timeiter->processTime=(long)((lTemp/100)*60 + (lTemp%100));//processTime转换为分钟，即0130->90
        //4
        timeiter->reportFlag=0;
        //5
        timeiter->receiveFlag=otheriter->flag;
        //6
        if(strcmp(otheriter->lastProcessDate,"")==0)
        {
          sprintf(timeiter->lastProcessDate,"N");
          sprintf(selectarray,"UPDATE %s SET lastprocessdate='N' WHERE reportno=%d",
                               OTHER_PARAM_TABLE,timeiter->reportNo);
          theParam->dbi.executeSql(selectarray,nTotal,nSuccess,nError);
        }
        else
          sprintf(timeiter->lastProcessDate,otheriter->lastProcessDate);
        //7
        timeiter->processLimitTime = otheriter->processLimitTime;
        
        timeiter++;
#ifdef _DEBUG_
    cout<<"timeiter->reportNo:"<<timeiter->reportNo<<endl;
    cout<<"timeiter->reportDate:"<<timeiter->reportDate<<endl;
    cout<<"timeiter->processTime:"<<timeiter->processTime<<endl;
    cout<<"-----------------------------------"<<endl;
#endif
    }//for 2.月报及其他参数表
   
    theParam->dbi.disconnect();

}


