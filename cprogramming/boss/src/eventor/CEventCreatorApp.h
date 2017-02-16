/*
* Copyright (c) 2007,lianchuang
* All rights reserved.
* 
* 文件名称：CEventCreatorApp.h
* 文件标识：
* 摘    要：
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CEventCreatorApp_H
#define CEventCreatorApp_H

#include "appfrm/Application.h"
#include "dbi/dbi.h"
#include "base/SysParam.h"

#include "EventCommon.h"
#include "CEventInfoCreator.h"

class CEventInput;
class CEventOutputFile;
class CEventInfoCreator;

/**
 * CEventCreatorApp：完成用户出帐总体框架类.
 * 含有下面的方法:
 *    initialization:   初始化
 *    loopProcess:      循环处理
 *    recovery:         恢复
 *    end:              结束
 */
class CEventCreatorApp : public FileApplication
{
    public:
        
        CEventCreatorApp();
        
        ~CEventCreatorApp();
   
    protected:
        
        virtual bool initialization();
        
        virtual bool beforeLoop();
        
        virtual bool loopProcess();
        
        virtual bool recovery();
        
        virtual bool end();
    
    private:    
        /**
         * getSysParams：根据配置文件记录初始化系统信息
         * @param  无.
         * @return true:成功    false:失败
         */
        bool getSysParams();

        /**
         * getDealDate：设置处理时间为当前时间
         * @param  无.
         * @return 无
         */        
        void getDealDate();
        
        bool eventRun();
        
        //limeng modified 20090821 清理增量接口表
        bool endDeal(const vector<ReAccountEventInfo>& r_reAccountEventVector);
        bool endDeal(const vector<MonfeeSpEventInfo>& r_spEventInfoVector);
        //limeng modified 20090811 实时模式更新参数表中日期加1
        bool updSysParamCyc();
        //limeng modified 20090811 取实时模式参数表中帐期
		bool getSysParamCyc(string &r_cycDate);        
                void getDuration(const String<14>& endDate, 
                          const String<14>& startDate,
                          int& dayNum);       
        //获取一个月有多少天
        int getMonthDays(int iYMonth) //YYYYMM
        {
            int year = iYMonth/100;
            int month = iYMonth-(iYMonth/100)*100;
            switch (month) 
            {
        	  case 2:
        	    if (TimeUtil::isLeapYear((long)year))
        	      return 29;
        	    else
        	      return 28;
        	    break;
        	  case 4:
        	  case 6:
        	  case 9:
        	  case 11:
        	    return 30;
        	  default:
        	    return 31;
        	}
        }
        
//#ifdef __PROV019__  //如果是山西	   	
       //山西绑定资费    add by xuf 2010-1-28 14:22:10  
       bool bindFeePolicy(string r_createUfileTime);
       //秒转换成小时分钟秒
       void secToTimeString(int sec, string& sTime);
       
       //把HHMMSS转换成秒，sTime小时分钟秒HHMMSS, sec 秒 
       void timeStringToSec(string sTime,int &sec);
       
       //取上次绑定资费的时间YYYYMMDD
       bool getLastBindTpTime(string &r_cycDate);
       
      //更新资费绑定结束时间YYYYMMDD
       bool updateBindTpTime(const char *r_bindTpEndTime);
//#endif
                
    private:

        EventConfig                    m_eventConfig;
        
        T_CYCLE                                m_minRealCycId;
        
        String<14>                            m_dealDate;         
        string                                m_performId;

        
        PM_Billing                         *m_pEventParam;
        CEventInput                         m_eventInputer;
        CEventOutputFile                 m_eventOutputFile;
        CEventOutputFile                 m_ugpEventOutputFile;	//limeng added 20090813 用户群事件
        bool							m_compFlag;	//limeng added 20090820 是否计算
        bool							m_rateReadyFlag;	//limeng added 20090902 批价是否准备好，true:输出到批价输入目录,false:输出到中间目录
        CEventInfoCreator                 m_eventInfoCreator;
        
        //判断用户是否需要出帐
        bool isNeedAccoutUser(const IdInfo& userInfo);
        
        //limeng modified 20090811 判断地市是否需要出帐
        bool isNeedAccoutEparchyCode(const int& cycId,const IdInfo& userInfo);

//#ifdef __PROV019__  //如果是山西       
       int m_bindTpOffsetSecond; //绑定资费偏移秒数 add by xuf 2010-2-3 11:08:24 
       string m_bindTpEndTime; //绑定资费结束时
//#endif	   
};


#endif

