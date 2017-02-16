/*
* Copyright (c) 2007,lianchuang
* All rights reserved.
* 
* 文件名称：CMonfeeApp.h
* 文件标识：
* 摘    要：完成包月费出帐总体框架类的定义
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CMonfeeApp_H
#define CMonfeeApp_H

#include "appfrm/Application.h"
#include "dbi/dbi.h"

#include "MonfeeCommon.h"
#include "BillingCommon.h"

//Add by Malone 2009-11-1 18:29 start
#include "PM_Billing.h"
//Add by Malone 2009-11-1 18:29 end

class CMonfeeParamMgr;
class CMonfeeInputFile;
class CMonfeeOutputFile;
class CMonfeeBillManager;
class CDiscntMonfee;
class CRecvMonfee;
class CMonfeeInternalData;

/**
 * CMonfeeApp：完成用户出帐总体框架类.
 * 含有下面的方法:
 *    initialization:   初始化
 *    loopProcess:      循环处理
 *    recovery:         恢复
 *    end:              结束
 */
class CMonfeeApp : public FileApplication
{
    public:
        
        CMonfeeApp();
        
        ~CMonfeeApp();
   
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
         * getSysParamsFromDb：根据数据库数据初始化系统信息
         * @param  无.
         * @return true:成功    false:失败
         */
        bool getSysParamsFromDb();
        
        /**
         * shiftNewCyc：根据传入的帐期标识进行帐期切换
         * @param  无.
         * @return true:成功    false:失败
         */
        bool shiftNewCyc();

        /**
         * shiftMemoryBillForNewCyc：根据传入的帐期标识对内存帐单进行帐期切换
         * @param  cycId： 计费帐期标识.
         * @return true:成功    false:失败
         */
        bool shiftMemoryBillForNewCyc(const int& cycId);        

        /**
         * getDealDate：设置处理时间为当前时间
         * @param  无.
         * @return 无
         */        
		void getDealDate();
        
        /**
         * getValidFile：从目录下获取需要处理的有效文件
         * @param  无.
         * @return true:成功    false:失败
         */
        bool getValidFile();
        
        /**
         * checkFileType：核查文件类型是否正确
         * @param  无.
         * @return true:正确    false:错误
         */        
        bool checkFileType();

        /**
         * checkFileName：核查文件名称是否正确
         * @param  无.
         * @return true:正确    false:错误
         */
        bool checkFileName();        
        
        /**
         * addupAccountInfo：对系统累计信息进行累计
         * @param  无.
         * @return 无
         */
        void addupAccountInfo();
        
        /**
         * checkAddupInfo：对系统累计信息进行核查
         * @param  无.
         * @return true:成功    false:失败
         */        
        void checkAddupInfo();

        /**
         * monfeeRun：用户出帐处理
         * @param  无.
         * @return true:成功    false:失败
         */        
        bool monfeeRun();
    
    private:

        MonfeeConfig           m_monfeeConfig;

        string                          m_fileName;
        char                            m_fileType;
        int                             m_fileCycId;
        
        int                             m_minRealCycId;
        int                             m_currentCycId;
        CycParam                        m_cycPara;
        string                          m_provinceCode;
        
        String<14>                          m_dealDate;         
        string                          m_performId;
        char                            m_errorTag;
        
        //Add by Malone 2009-11-1 18:33 start
    	  PM_Billing      *m_pMonfeeParam;
    	  //Add by Malone 2009-11-1 18:34 end

        
        CMonfeeParamMgr                  *m_monfeeParamMgr;
        CMonfeeInputFile            m_monfeeInputFile;
        CMonfeeOutputFile           m_monfeeOutputFile;
        CMonfeeBillManager                 m_userBillManager;        
        CDiscntMonfee                      m_discntMonfee;
        CRecvMonfee                        m_recvMonfee;
        CMonfeeInternalData                m_monfeeInternalData;
                                      
        AccountAddupInfo                m_accountAddupInfo;
        vector<AccountCheckInfo>        m_accountCheckInfoVector;         
        
        SpRelaDiscntMap									m_spRelaDiscntMap;
        vector<IdDiscount>							m_sysDiscountVector;
        
};


#endif


