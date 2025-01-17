#ifndef UserBillFacOutputFile_H
#define UserBillFacOutputFile_H

#include "UserBillFacCommon.h"

/**
 * UserBillFacOutputFile：客户账单输出文件管理.
 * 含有下面的方法:
 *   init:              根据系统配置参数信息初始化输出文件的目录和文件名称
 *   writeFile:         向输出文件写数据
 *   closeFile:         关闭所有通道输出文件
 *   linkUnlink:        搬运输出文件从工作路径到正式输出路径
 *   openFile:          打开输出文件
 *   computeChannelNo:  计算通道号码  
 *   resetFileTag:      重置文件标志    
 */
class UserBillFacOutputFile
{
	public:
	
	    UserBillFacOutputFile();
	    
	    ~UserBillFacOutputFile();
	    
	    /**
         * init:根据系统配置参数信息初始化输出文件的目录和文件名称
         * @param  performId 流水号.
         * @param  pBillFacConfig 配置信息,用于读取工作路径,正式输出路径.
         * @param  strBcycFromBFileName 从文件名称获取的计费话单时间.
         * @param  strOperateTime 处理时间.
         * @return 成功返回true,失败返回false.
         */
	    bool init(const string& performId,
	              const BillFacConfig*  pBillFacConfig,
                  const string &strBcycFromBFileName, 
                  const string& strOperateTime);
        
        /**
         * writeFile:向输出文件写数据
         * @param  userBillMap 内存账单.
         * @return 成功返回true,失败返回false.
         */            
	    bool writeFile(const USERBILLMAP& userBillMap);
	    
	    /**
         * closeFile:关闭所有通道输出文件
         * @return 无.
         */
	    void closeFile();
	    
	    /**
         * linkUnlink:搬运输出文件从工作路径到正式输出路径
         * @return 无.
         */
	    bool linkUnlink();
	    
    private:
	    /**
         * openFile:打开输出文件
         * @param  channelNo 文件通道号. 
         * @return 成功返回true,失败返回false.
         */
	    bool openFile(const int &channelNo);
	    
	    /**
         * computeChannelNo:计算客户id所属通道号，当返回-1时，表示失败
         * @param  acctId 客户标识.
         * @return 当返回-1时，表示失败.
         */
	    int  computeChannelNo(const ACCTID &acctId);                     
	    
	    /**
         * resetFileTag:重置文件标志
         * @param  i 文件通道号.
         * @return 无.
         */
	    void resetFileTag(const int i);
	    
	   //add by zhouq
	   /**
	    * swichOutputFile:转换输出文件
	    * @param channelNo 文件通道号.
	    * @return 无.
	   */
	   void swichOutputFile(const int &channelNo);
	    
	private:
	    OutputFileInfo          m_outputFileInfo[MAX_USERACCOUNT_CHANNELNUM];
	    string                  m_performId;
	    //add by zhouq begin
		string                  m_outputFile[MAX_USERACCOUNT_CHANNELNUM];
		string                  m_outputFilehead[MAX_USERACCOUNT_CHANNELNUM];
		string                  m_workFile[MAX_USERACCOUNT_CHANNELNUM];
		string                  m_workFilehead[MAX_USERACCOUNT_CHANNELNUM];
		int                     m_seqNo[MAX_USERACCOUNT_CHANNELNUM];
		long                    m_RecordNum;
      //add by zhouq end
};

#endif //UserBillFacOutputFile_H
