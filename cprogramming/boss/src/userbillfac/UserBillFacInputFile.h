/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* 文件名称：UserBillFacInputFile.h
* 文件标识：
* 摘    要：完成帐户出帐涉及资料、参数内存管理
* 
* 当前版本：1.0
* 作    者：liugj
* 完成日期：2005年4月
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef UserBillFacInputFile_H
#define UserBillFacInputFile_H

#include "UserBillFacCommon.h"

/**
 * UserBillFacInputFile：帐户账单输入文件管理.
 * 含有下面的方法:
 *   init:              根据系统配置参数信息初始化输入文件的目录和文件名称
 *   getUserBill:       从输入文件中读取帐户及帐户帐单信息
 *   openFile:          打开输入文件
 *   closeFile:         关闭输入文件
 *   getDealDate:       从系统获得文件处理时间
 */
class UserBillFacInputFile
{
	public:
	
	    UserBillFacInputFile();
	    
	    ~UserBillFacInputFile();
	    
	    /**
         * init:根据系统配置参数信息初始化输入文件的目录和文件名称
         * @param  strInputFilePath 输入文件路径.
         * @param  mapUnifiedPayRelation 统一付费账户对应关系集合.
         * @return 成功返回true,失败返回false.
         */
	    bool init(const string& strInputFilePath, 
	                        const map<string, string>& mapUnifiedPayRelation);
	                        
	    /**
         * openFile:打开输入文件
         * @param  strFileName 文件名称.
         * @param  localOrRemoteTag 本地还是异地文件.
         * @param  pAccountLogInfo 日志指针用于传回错误的行数.
         * @return 成功返回true,失败返回false.
         */
	    bool openFile(const string& strFileName, const char localOrRemoteTag,
	                                         AccountLogInfo* pAccountLogInfo);
	    
	    /**
         * getUserBill:从输入文件中读取帐户及帐户帐单信息
         * @param  acctKey 客户用户标识.
         * @param  vecUserBill 客户用户帐单.
         * @return 获得到返回true,未获得到返回false.
         */
	    bool getUserBill(UserKey& userKey, vector<UserBill> &vecUserBill);
	    
	    /**
         * closeFile:关闭输入文件
         * @return 无.
         */
	    void closeFile();
	    
	private:
	    string                  m_fileName; 
	    string                  m_filePath;
	    ifstream                m_fileStream;
	    string                  m_seperate;
	    //'0'本地 '1'异地
	    char                    m_localOrRemoteTag;        
	    
	    //统一付费账户对应关系集合
	    map<string, string>     m_mapUnifiedPayRelation;   
	    
	    AccountLogInfo*         m_pAccountLogInfo;
	    
      	T_SERVER_ID				m_lastId;    
		char					m_lastIdType;    
		T_SERVER_ID				m_lastUserId ;
		     
		int						m_lastDetailItemCode; 
		char					m_lastBillTypeCode;
		int						m_lastPriority;
        long					m_lastDiscountFee;
        int						m_lastDiscountId;
        long					m_lastAdjustInsId;
        long          			m_lastEffectId;//add by xuf 2010-1-23 10:16:51
};

#endif //UserBillFacInputFile_H
