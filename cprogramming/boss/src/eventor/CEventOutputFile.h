/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* 文件名称：CEventOutputFile.h
* 文件标识：
* 摘    要：
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CEventOutputFile_H
#define CEventOutputFile_H


#include "EventCommon.h"
#include "CEventInfoCreator.h"

class CEventInfoCretor;

/**
 * CEventOutputFile：输出文件管理.
 * 含有下面的方法:
 *   initialize:          根据配置信息初始化输出文件目录及名称
 *   dispatch:            输入相关内部数据到输出文件
 *   closeFile:           关闭输出文件
 *   linkUnlink:          将输出文件从工作目录搬迁到输出目录
 */    
class CEventOutputFile
{
    public:
        
        CEventOutputFile();
        
        ~CEventOutputFile();

        void initialize(//const string& performId,
                        EventConfig *eventConfig);
        
        bool dispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator);
        
        //bool writeFile(const int &fileType, 
        //               vector<AccountCheckInfo>& accountCheckInfoVector);

        void closeFile();
        
        bool linkUnlink(const T_CYCLE& cycId, const string& dealDate);
        
        //limeng add 20090814 处理用户群
        bool ugpDispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator);
        
        void ugpCloseFile();
        
        bool ugpLinkUnlink(const T_CYCLE& cycId, const string& dealDate);

        //limeng add 20090901 处理账户
        bool acctDispatch(const int& channelNo, const int& dealCycId, const string& dealDate, CEventInfoCreator &eventInfoCreator);
        
        void acctCloseFile();
        
        bool acctLinkUnlink(const T_CYCLE& cycId, const string& dealDate);

        //limeng add 20090812 复制用户群重出帐事件
        bool linkUgpEvent();
        
        //limeng add 20090812 用户重出帐事件是否处理完
        bool isUserEventFinished();
        
        //limeng add 20090902 复制账户重出帐事件
        bool linkAcctEvent();
        
        //limeng add 20090902 用户群重出帐事件是否处理完
        bool isUgpEventFinished();
        
        //limeng add 20090812 将通道对应的批价目录保存到m_outputFileInfo
        bool setOutFile(const int& r_channelNo,const string& r_subDir);
        
        //limeng add 20090813 文件复制
		int copy(const char *r_srcFile,const char *r_desFile) const;
		
    private:
        
        bool openFile(const int& fileType);
                       
        void closeFile(const int& channelNo);
        
        bool linkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate);
        
        //limeng add 20090814 处理用户群
        bool ugpOpenFile(const int& fileType);
                       
        void ugpCloseFile(const int& channelNo);
        
        bool ugpLinkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate);
        
        //limeng add 20090901 处理账户
        bool acctOpenFile(const int& fileType);
                       
        void acctCloseFile(const int& channelNo);
        
        bool acctLinkUnlink(const int& channelNo,const T_CYCLE& cycId, const string& dealDate);
                    
    private:
        
        OutputFileInfo          m_outputFileInfo[MAX_OUTFILE_CHANNELNO];
        OutputFileInfo          m_ugpOutputFileInfo[MAX_OUTFILE_CHANNELNO];	//limeng add 20090814 用户群文件信息
        OutputFileInfo          m_acctOutputFileInfo[MAX_OUTFILE_CHANNELNO];	//limeng add 20090901 账户事件信息
        vector<string>			m_outputFiles;	//limeng add 20090819 用户事件输出文件
        vector<string>			m_ugpBackupFiles;	//limeng add 20090819 用户群事件中间输出文件
        vector<string>			m_ugpOutputFiles;	//limeng add 20090819 用户群事件输出文件
        vector<string>			m_acctBackupFiles;	//limeng add 20090901 账户事件中间输出文件
        vector<string>			m_acctOutputFiles;	//limeng add 20090901 账户事件输出文件
        //string                  m_performId; 
        
        //AccountLogInfo          m_accountLogInfo;
        EventConfig  *m_eventConfig;
        string        m_fileHead;  //文件头名称（URP或者OTP）根据初始化时候传入的事件类型是重出帐文件还是一次性费用决定
        string        m_ugpFileHead;  //limeng add 20090814 用户群文件头名称
        string        m_acctFileHead;  //limeng add 20090814 账户文件头名称
        bool		  m_rateReadyFlag;	//limeng add 20090902 批价是否准备好，true:输出到批价输入目录,false:输出到中间目录

};

    
#endif //!CEventOutputFile_H
