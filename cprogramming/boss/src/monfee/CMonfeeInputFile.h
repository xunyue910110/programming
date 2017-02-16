/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* CMonfeeInputFile.h
* 文件标识：
* 摘    要：完成用户出帐涉及资料、参数内存管理
* 
* 当前版本：1.0
* 作    者：zhaogc
* 完成日期：2007年3月28日
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CMonfeeInputFile_H
#define CMonfeeInputFile_H


#include "MonfeeCommon.h"


/**
 * CMonfeeInputFile：用户出帐输入帐单文件管理.
 * 含有下面的方法:
 *   initialize:        根据系统配置参数信息初始化输入文件的目录和文件名称
 *   getUserBill:       从输入文件中读取用户及用户帐单信息
 *   openFile:          打开输入文件
 *   closeFile:         关闭输入文件
 *   linkUnlink:        将输入帐单文件从输入目录中删除
 */
class CMonfeeInputFile
{
    public:
    
        CMonfeeInputFile();
    
        ~CMonfeeInputFile();
                
        void initialize(const MonfeeConfig *monfeeConfig,
                        const string& performId,
                        AccountAddupInfo *accountAddupInfo);        
        /**
         * getUserBill:从输入文件中读取用户及用户帐单信息
         * @param  userId                   用户标识.
         * @param  userOriginalBillVector   用户原始帐单集.
         * @return true:成功 false:失败
         */        
        bool getUserSubInfo(USERID& userId,
                         vector<UserSubInfo>& userSubInfoVector);

        /**
         * openFile:打开输入文件
         * @param  无.
         * @return true:成功 false:失败
         */                         
        bool openFile();

        /**
         * closeFile:关闭输入文件
         * @param  无.
         * @return 无
         */         
        void closeFile();
        
        /**
         * linkUnlink:将输入帐单文件从输入目录中删除
         * @param  无.
         * @return true:成功 false:失败
         */ 
        bool linkUnlink();
        
    private:
        
        string                  m_fileName;
        string                  m_inputFilePath;
        string                  m_inputFileFullName;
        string                  m_invalidFilepath;
        string                  m_invalidFileFullName;
        
        char                    m_fileType;
        ifstream                m_inputFileStream;
        ofstream                m_invalidFileStream;
        string                  m_seperate;
        string 					m_lastStrUserId;
		UserSubInfo             m_userSubInfo;                                
		
        char                    m_buffer[512];
        
        AccountLogInfo          m_accountLogInfo;
        string                  m_performId;        
        AccountAddupInfo        *m_accountAddupInfo;
};


#endif

