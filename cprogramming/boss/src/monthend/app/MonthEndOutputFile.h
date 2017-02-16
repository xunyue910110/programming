#ifndef MONTHENDOUTPUTFILE_H
#define MONTHENDOUTPUTFILE_H


#include "include/MonthEndCommon.h"
#include <map>

//#include "MonthInternalData.h"


/**
 * MonthEndOutputFile：用户账单输出文件管理.
 * 含有下面的方法:
 *   init:              根据系统配置参数信息初始化输出文件的目录和文件名称
 *   writeFile:         向输出文件写数据
 *   closeFile:         关闭所有通道输出文件
 *   linkUnlink:        搬运输出文件从工作路径到正式输出路径
 *   openFile:          打开输出文件
 *   computeChannelNo:  计算通道号码  
 *   resetFileTag:      重置文件标志    
 */
class MonthEndOutputFile
{
    public:
    
        MonthEndOutputFile();
        
//        MonthEndOutputFile(const MonthEndOutputFile& monthEndOutputFile);
        
        ~MonthEndOutputFile();
        
//        const MonthEndOutputFile & operator=(const MonthEndOutputFile& monthEndOutputFile);
        
        /**
         * init:根据系统配置参数信息初始化输出文件的目录和文件名称
         * @param  performId 流水号.
         * @param  billFacConfig 配置信息,用于读取工作路径,正式输出路径.
         * @param  channelStVec  通道号
         * @param  r_batch  批次数
         * @param  bcycDate 从文件名称获取的计费帐期时间.
         * @param  dealTime 处理时间.
         * @return 成功返回true,失败返回false.
         */
        bool init(const string& performId,
                  const MonthEndConfig* monthEndConfig,
                  const vector <channelSt> channelStVec,
                  const int &r_batch,
                  const string& bcycDate, 
                  const string& dealTime,
                  const vector<string>& actEparchyCode);
        
        /**
         * writeFile:向输出文件写数据
         * @param  vector <IntergrateBill> &intergrateBillVec 综合账单
         * @return 成功返回true,失败返回false.
         */   
        bool writeFile(vector <IntergrateBill> &intergrateBillVec);

        //added by pangx on 2010-3-29 begin
		bool writeFile(vector <VpdnOneClickPayBill> &vecVpdnOneClickPayBill); 
		//added by pangx on 2010-3-29 end
				//输出不出帐用户账单
				//bool outFreeFeeUserBill(const vector <IntergrateBill> &freeFeeUserBillVec);
				bool outFreeFeeUserBill(const vector <IntergrateBill> &freeFeeUserBillVec,const string strChannelNo);
                                                      
        /**
         * closeFile:关闭所有通道输出文件
         * @return 无.
         */                                               
        void closeFile();
        
        /**
         * linkUnlink:搬运输出文件从工作路径到正式输出路径
         * @return true 成功 ,false 失败.
         */
        bool linkUnlink();
        
        void end();
        
        
   public:
   	     
        int        m_UnacctUserFee;//不出帐用户费用
        long       m_IntegrateFeeSum;//综合账单费用  
        long	   	m_unAcctRecords;  //不出帐记录
        
        //add by xuf 2010-1-7 17:22:28
        long      m_invalidRecords;//无效记录数
        long      m_noInfoFeeSum;// 无效费用
        
        long       m_integrateBillRecords;//综合账单记录数
        string m_outputFileName_Z	   ;  
        
    private:
        /**
         * openFile:打开输出文件
         * @param  channelNo 文件通道号,batchNo 文件批次号.
         * @return 成功返回true,失败返回false.
         */
        bool openFile(OutputFileInfo &outputFileInfo);
        
        /**
         * computeChannelNo:计算账户id所属通道号，当返回-1时，表示失败
         * @param  acctId 账户标识.
         * @return 当返回-1时，表示失败.
         */
        int  computeChannelNo(const string& acctId);                     
        
         /**
         * computeBatchNo:计算账户id所属批次号，当返回-1时，表示失败
         * @param  acctId 账户标识.
         * @return 当返回-1时，表示失败.
         */
        int computeBatchNo(const string& acctId);
        	
        
        /**
         * resetFileTag:重置文件标志
         * @param  i 文件通道号.
         * @return 无.
         */
        void resetFileTag(OutputFileInfo &outputFileInfo);
        
        bool outLog(const OutputFileInfo &outputFileInfo);
        
        bool getPartitionId(const int r_batch);
        
        bool getPartitionIdByNo(const int &r_channNo,const int &r_batchNo,string &r_startPartId,string &r_endPartId); 
        
        bool renameFile(const char *r_srcFullFileName,const char *r_desFullFileName)const ;
        
        int  copy(const char *r_srcFile,const char *r_desFile) const;
        	
        void channelNoCalc(int begId,int endId,int batch,map<int,IntPair>  & mp);
        
        	
    private:
		map<int,OutputFileInfo> 			m_batchMap;
		map<int,map<int,OutputFileInfo> >   m_channelMap;
		map<string ,map<int,map<int,OutputFileInfo> > > m_outputFileInfoMap;  
		map<int ,map<int,IntPair> >  m_chanBacthPartition;
		
	 
        string                  m_performId;
        int                     m_batchNo;
        vector <channelSt>      m_channelStVec;
        vector<string>			m_actEparchyCode;
        //add by xuf 2009-12-16 15:47:03
        OutputFileInfo  m_outputFileInfoEx;//公免用户输出文件  
        //added by pangx on 2010-3-29 begin
	   OutputFileInfo          m_outputVpdnFileInfo;
		//added by pangx on 2010-3-29 end
};

#endif //MONTHENDOUTPUTFILE_H
