/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* 文件名称：UserBillFacApp.h
* 文件标识：
* 摘    要：
* 
* 当前版本：1.0
* 作    者：lgj
* 完成日期：2005年3月
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef UserBillFacApp_H
#define UserBillFacApp_H

#include "UserBillFacCommon.h"
#include "UserBillFacInputFile.h"
#include "UserBillFacOutputFile.h"
#include "dbi/dbi.h"

/**
 * UserBillFacApp: 账户账单排重
 * 含有下面的方法:
 * userBillFacRun                运行函数
 * getAllValidFile:              获取所有输入文件
 * dealAllValidFile:             处理所有输入文件
 * writeUserBill:                输出排重后的话单
 * linkUnlink:                   删除或搬移输入文件
 * getSysParams:                 获取配置信息
 * getPayRelationFromDB:         从数据库中获取付费关系
 * getCurrDate:                  从系统获取当前处理时间
 * getBcycDateFromFile:          从文件名称解析出该文件的账期日期
 * judgeFileDateIsValid:         判断文件日期是否处于本次处理的批次
 * judgeFileNameIsValid:         文件名称是否合法
 * judgeFileNameDetailIsValid:   文件名称规则详细判断
 * dealFile:                     处理单个合法文件
 * IsValidPath:                  判断路径是否合法
 * split:                        将一个字符串,按照某种分隔符,分隔成若干子字符串
 */
class UserBillFacApp : public Application
{
	public:
	
	    UserBillFacApp();
	    
	    ~UserBillFacApp();
	    
	protected:
	
	    virtual bool initialization();
	    
		virtual bool loopProcess();
		
		virtual bool recovery();
		
		virtual bool end();
		
    private:
        
        /**
         * userBillFacRun:运行函数
         * @return 正常true,异常false.
         */
        bool userBillFacRun();
    
        /**
         * getAllValidFile:获取所有输入文件
         * @return 返回m_fileNameInfoVector的大小.
         */
	    int getAllValidFile();
	    
	    /**
         * dealAllValidFile:处理所有输入文件
         * @return 成功true,失败false.
         */
	    bool dealAllValidFile();
	    
	    /**
         * writeUserBill:输出排重后的话单
         * @return 成功true,失败false.
         */
	    bool writeUserBill();
	    
	    /**
         * linkUnlink:删除或搬移输入文件
         * @return 成功true,失败false.
         */
	    bool linkUnlink();
    
        /**
         * getSysParams:获取配置信息
         * @return 获得到返回true,未获得到返回false.
         */
        bool getSysParams();
        
        /**
         * getPayRelationFromDB:从数据库中获取付费关系
         * @param  userName 数据库登录用户名.
         * @param  passWord 数据库登录用户密码.
         * @param  servName 数据库服务名
         * @return 获得到返回true,未获得到返回false.
         */
        /*
        bool getPayRelationFromDB(const string& userName,
                          const string& passWord, const string& servName);
        */
        
        /**
         * getCurrDate:从系统获取当前处理时间
         * @return 无.
         */
        void getCurrDate();
        
        /**
         * getBcycDateFromFile:从文件名称解析出该文件的账期日期
         * @param  fileName 文件名称.
         * @param  bcycDate 该文件的账期日期.
         * @return 获得到返回true,未获得到返回false.
         */
        bool getBcycDateFromFile(const string &fileName, string &bcycDate);
        
        /**
         * judgeFileDateIsValid:判断文件日期是否处于本次处理的批次
         * @param  fileName 文件名称.
         * @return 返回值与FileNameInfo结构中m_validTag标志相符.
         */
        int  judgeFileDateIsValid(const string &fileName);                  
        
        /**
         * judgeFileNameIsValid:文件名称是否合法
         * @param  fileName 文件名称.
         * @return 合法true,不合法返回false.
         */
        bool judgeFileNameIsValid(const string &fileName);
        
        /**
         * judgeFileNameDetailIsValid:文件名称规则详细判断
         * @param  fileName 文件名称.
         * @param  vecFileNameRule 文件名称明细规则.
         * @return 合法返回true,不合法返回false.
         */
        bool judgeFileNameDetailIsValid(const string &fileName, 
                          const vector<FileNameRule>& vecFileNameRule);
        
        /**
         * dealFile:处理单个合法文件
         * @param  inputBillFileName 输入文件名称.
         * @return 成功true,失败false.
         */
        bool dealFile(const string &inputBillFileName);	
        
        /**
         * IsValidPath:判断路径是否合法
         * @param  sPath 路径.
         * @return 路径存在合法返回true,路径不存在不合法返回false.
         */
        bool IsValidPath(const string& sPath);                          
        
        /**
         * split:将一个字符串,按照某种分隔符,分隔成若干子字符串
         * @param  source 待解析字符串.
         * @param  seperator 分隔符.
         * @param  destVector 解析出的字符串.
         * @return 返回解析出字符串个数.
         */
        int  split(string& source, const string& seperator, 
                                            vector<string> &destVector); 
    
    private:
        //从首个合法文件的名称中获取的帐期
        string                                         m_dealBcycDate;    
	    string                                         m_performId;
	    
	    BillFacConfig                                  m_billFacConfig;
	    
	    //统一付费账户对应关系集合                                               
	    map<string, string>                            m_mapUnifiedPayRelation;    
	    //文件名称与本地或异地对应关系
	    map<char, char>                                m_mapLocalOrRemote;         
	                               
	    Directory                                      m_directory;
	    vector<FileNameInfo>                           m_fileNameInfoVector;
	    
	    USERBILLMAP                                    m_userBillMap;
	    vector<UserBill>                               m_vecUserBillTotal;
	    
	    UserBillFacInputFile                           m_userBillFacInputFile;
      UserBillFacOutputFile                          m_userBillFacOutputFile;
        
	    //系统日期
	    string                                         m_dealDate;                 
	    //文件名称命名规则定义
	    FILENAMERULEMMAP                               m_mmapFileNameRule;         
	    //日志                                                                           
	    AccountLogInfo                                 m_accountLogInfo; 
	    
	    //数据库登录
	    string                                         m_userName;
	    string                                         m_passWord;
	    string                                         m_servName;          
};

#endif //UserBillFacApp_H

