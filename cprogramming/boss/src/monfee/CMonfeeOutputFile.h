/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* 文件名称：MonthFeeAccountOutputFile.h
* 文件标识：
* 摘    要：完成用户出帐输出文件管理
* 
* 当前版本：1.0
* 作    者：zhaogc
* 完成日期：2007年3月29日
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CMonfeeOutputFile_H
#define CMonfeeOutputFile_H


#include "MonfeeCommon.h"


class CMonfeeInternalData;


/**
 * MonthFeeAccountOutputFile：用户出帐输出文件管理.
 * 含有下面的方法:
 *   initialize:          根据用户出帐配置信息初始化输出文件目录及名称
 *   dispatch:            输入用户相关内部数据到输出文件
 *   writeFile:           输入出帐系统核查信息到输出文件
 *   closeFile:           关闭输出文件
 *   linkUnlink:          将输出文件从工作目录搬迁到输出目录
 */    
class CMonfeeOutputFile
{
    public:
        
        CMonfeeOutputFile();
        
        ~CMonfeeOutputFile();

        /**
         * initialize:根据用户出帐配置信息初始化输出文件目录及名称
         * @param   operateId               操作流水号.
         * @param   operateTypeTag          操作类型标志.
         * @param   MonthFeeAccountConfig       用户出帐配置信息.
         * @param   accountLogInfoVector    帐务处理日志信息集.
         * @return  无
         */        
        void initialize(const string& performId,
                        MonfeeConfig *monfeeConfig);
        
        /**
         * dispatch:输入用户相关内部数据到输出文件
         * @param  CMonfeeInternal 用户内部数据类.
         * @return true:成功 false:失败
         */
        bool dispatch(CMonfeeInternalData& monfeeInternalData);
        
        /**
         * writeFile:输入出帐系统核查信息到输出文件
         * @param  fileType                 文件类型.
         * @param  operateId                操作流水号
         * @param  operateTypeTag           操作类型标志
         * @param  accountCheckInfoVector   帐务处理核查信息.
         * @return true:成功 false:失败
         */        
        bool writeFile(const int &fileType, 
                       vector<AccountCheckInfo>& accountCheckInfoVector);

        /**
         * closeFile:关闭输出文件
         * @param  无.
         * @return 无
         */        
        void closeFile();
        
        /**
         * linkUnlink:将输出文件从工作目录搬迁到输出目录
         * @errorTag  错误标志.
         * @return true:成功 false:失败
         */                
        bool linkUnlink(const char &errorTag);
    
    private:
        
        /**
         * openFile:  打开输出文件
         * @param  fileType  文件类型.
         * @return true:成功 false:失败
         */
        bool openFile(const int& fileType);
        
        /**
         * writeFile:将相应的用户内部数据写入输出文件
         * @param  fileType:文件类型.
         * @param  CMonfeeInternalData:用户内部数据类.
         * @return true:成功 false:失败
         */
        bool writeFile(const int& fileType,
                       CMonfeeInternalData& monfeeInternalData);
                    
    private:
        
        OutputFileInfo          m_outputFileInfo[MAX_MONTHFEE_OUTFILETYPE];
        
        int                     m_batchNo;
        string                  m_performId; 
        
        AccountLogInfo          m_accountLogInfo;
        MonfeeConfig  *m_monfeeConfig;

};

    
#endif //!CMonfeeOutputFile_H
