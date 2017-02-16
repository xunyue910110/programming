/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* CEventInput.h
* 文件标识：
* 摘    要：
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CEventInput_H
#define CEventInput_H


#include "dbi/dbi.h"
#include "EventCommon.h"
#include </usr/include/pthread.h> //multi-thread add by xuf 2010-1-31 22:23:34

class CEventInput
{
    public:
    
        CEventInput();
    
        ~CEventInput();
                
        void initialize(const EventConfig *eventorConfig, PM_Billing *pEventParamMgr); 
        
        bool getEventInfo(vector<ReAccountEventInfo>& reAccountEventVector);
        //limeng modified 20090813 增加reAccountUgpEventVector用户群事件数组
        bool getUgpEventInfo(vector<ReAccountEventInfo>& reAccountUgpEventVector);
        
        //limeng added 20090901 取账户事件
        bool getAcctEventInfo(vector<ReAccountEventInfo>& reAccountAcctEventVector);
        
        //limeng modified 20090821 设置帐期	
        void setCurrCycId(const int currCycId);
        
        bool prePareOtpUserDataFromDb();
        
        bool getEventInfo(vector<OtpEventInfo>& otpEventInfoVector);
        
        bool getEventInfo(vector<MonfeeSpEventInfo>& spEventInfoVector);
        //add 2009-1-30 21:11:15
        bool getNextEventInfo(vector<ReAccountEventInfo>& reAccountEventVector);
        bool getNextEventInfo(vector<MonfeeSpEventInfo>& spEventInfoVector);
				bool getNextUgpEventInfo(vector<ReAccountEventInfo>& reAccountUgpEventVector); 
				//limeng added 20090901 取下一批账户事件       
				bool getNextAcctEventInfo(vector<ReAccountEventInfo>& reAccountAcctEventVector);  
		
//#ifdef __PROV019__  //如果是山西		     
		       typedef struct
          {
             int thrNum;
             CEventInput *m_pSelf;	
          }Thread_bindTp;  
	
				/**
				* callProcedure调用存储过程
				* @param  r_procName  存储过程名
				* @param  r_connum    连接序号
				* @return true：成功 false：失败 
				*/   
				  bool callProcedure(const char *r_procName,const int &r_connum);
				  //绑定资费
				  bool bindFeePolicy();
				  
				  //绑定资费线程
				 static void threadBindTp(const Thread_bindTp &thr);
				 
				 int reconnect(int connum);     
//#endif		  
    private:
        
        
            DbInterface                       m_dbinterface;
            DbInterface                       m_ugpDbinterface;
            DbInterface                       m_acctDbinterface;	//limeng add 20090901 账户
            string                            m_selectInfoSql;
            string                            m_userName;                 //用户名
            string                            m_passWord;                 //口令
            string                            m_servName;                 //数据库名
            string                            m_eventorType;
            string                            m_eparchyCode;			//limeng add 20090817 地市编码
            PM_Billing                       *m_pEventParamMgr;
            
            T_CYCLE                           m_defaultCycId;             //默认帐期（取最小实时帐期）
            char                              m_monthEndTag;              //默认月末标识（取最小实施帐期的月末标志）
            int                               m_currCycId;                //当前月末出帐帐期   add  2009-1-30 21:21:23  
						StringVector					 						recordVector;               // add 2009-1-30 21:21:43
						StringVector                	 		ugpRecordVector;               // limeng add 20090820 用户群
						StringVector                	 	  acctRecordVector;               // limeng add 20090901 账户
						int                               BACTCHNUM;                  //从数据库一次导入到内存的记录数
						string                            m_netTypeCodes;   //不参与出账的网别
						string                            m_fixNetTypeCodes; //参与出账的固网网别  //Added by jlxu@neimeng Dec16,2009
      			  
      			 //add by xuf 2010-1-28 16:24:16
//#ifdef __PROV019__  //如果是山西      			 
						int      													m_fullSign ;//绑定资费的方式 ,0 标识全量 ，其它 标识增量  
						int      													m_connectNum;                               //线程数
						bool     													m_abnormal;                                 //调用过程是否发生异常
						pthread_mutex_t 									m_proMutex;                               //可以过程全局锁
						bool        											m_bConn[MAXTHREADCONNUM];
      			DbInterface                       m_bindTpDbi[MAXTHREADCONNUM+1]; //
      			char m_msgbuf[MAXTHREADCONNUM][MAXBUFFLEN+1];      			
//#endif        
};


#endif

