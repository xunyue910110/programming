/*
* Copyright (c) 2001,lianchuang
* All rights reserved.
* 
* 文件名称：CMonfeeBillMgr.h
* 文件标识：
* 摘    要：完成帐户帐单内存管理的定义
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CMonfeeBillMgr_H
#define CMonfeeBillMgr_H


#include "MonfeeCommon.h"

#include "base/Types.h"
#include "bill/MatrixVector.h"
#include "bill/ThriftyList.h"
#include "bill/Index.h"

//定义帐单内存初始化返回信息
const int BILL_OK      = 0;                   //  0 - OK
const int BILL_REVOK   = 1;                   //  1 - recovery ok
const int BILL_LOGERR  = -1;                  // -1 - no bill log file
const int BILL_FILEERR = -2;                  // -2 - bill file error

//定义帐单文件的前缀
const string REALTIME_BILLFILE_PREFIX = "R";
const string DAY_BILLFILE_PREFIX = "D";
const string CURRENT_TOTALBILLFILE_PREFIX = "C";
const string TMP_PREFIX = "%";

//定义帐单内存大小初始值相关常量
const size_t ACCT_PRE_VECTSIZE    = 20000; // 1X 12 Bytes
const size_t HEADER_PRE_VECTSIZE  = 20000; // 2-3X 12 Bytes
const size_t BILL_PRE_VECTSIZE    = 50000; // 10-15X  8 Bytes

//系统费用帐目项最大值及相关常量
const int MAX_ITEM   = 10000;
const int MIN_ITEMID = 1;
const int MAX_ITEMID = 9999;

//定义其他系统常量
const int DATETIMELEN = 12;                   //YYMMDDHHMMSS
const int DAYLEN = 6;                         //YYMMDD
const int BILLSEQUENCE_LEN = 8;               //bill sequence length

// Added by Malone start
const unsigned char	INIT_VALID_Tag_2 = '2';	//帐单有效标志初始值，表示有效帐单。
const unsigned char	INIT_VALID_Tag_3 = '3';	//帐单有效标志初始值，表示无效帐单。
// Added by Malone end

//add by yuxk for MT
const unsigned char	INIT_FEETYPE = 'Y';	//费用类型默认值Y

const int unit_fee   = 1000;//1M=1000厘
//add by yuxk for MT
//定义内存分配策略
struct MyAssignPolicy
{
    size_t operator() (unsigned int step)
    {
        switch (step)
        {
            case 0:
                return 5;
            case 1:
                return 5;
            case 2:
                return 5;
            default:
                return 5;
        }
    }
};

/**
 * CMonfeeBill：帐户帐单类. 
 */
 
class CMonfeeBill
{
    public:
        CMonfeeBill():m_serialNumber(""),m_spBizCode(""),m_spCode(""),
        			m_itemCode(0),m_fee(0),
        			m_discntFee(0),m_displayType('0'),m_validTag(0),m_feeType('0'){;}//modify by yuxk for MT
                    
        ~CMonfeeBill() {;}
        
        CMonfeeBill (const CMonfeeBill& monfeeBill)
        {
            m_serialNumber = monfeeBill.m_serialNumber;
            m_spBizCode = monfeeBill.m_spBizCode;            
            m_spCode = monfeeBill.m_spCode;            
            m_itemCode = monfeeBill.m_itemCode;
            m_fee = monfeeBill.m_fee;
            m_discntFee = monfeeBill.m_discntFee;            
            m_validTag= monfeeBill.m_validTag;
            m_displayType= monfeeBill.m_displayType;
            m_feeType= monfeeBill.m_feeType;//add by yuxk for MT
			}    
    
    public:
        CMonfeeBill& operator =(const CMonfeeBill& monfeeBill)
        {
            if (this != &monfeeBill)
            {
              m_serialNumber = monfeeBill.m_serialNumber;
            	m_spBizCode = monfeeBill.m_spBizCode;
            	m_spCode = monfeeBill.m_spCode;            	
            	m_itemCode = monfeeBill.m_itemCode;
            	m_fee = monfeeBill.m_fee;
            	m_discntFee = monfeeBill.m_discntFee;            
            	m_validTag= monfeeBill.m_validTag;
            	m_displayType= monfeeBill.m_displayType;
		m_feeType= monfeeBill.m_feeType;	//add by yuxk for MT	
            }
            return *this;
        }        

        
        
        friend bool operator==(const CMonfeeBill& left,const CMonfeeBill& right);
        friend bool operator<(const CMonfeeBill& left,const CMonfeeBill& right);
        
        friend ostream& operator <<(ostream& out, const CMonfeeBill& monfeeBill);
        friend istream& operator >>(istream& in, CMonfeeBill& monfeeBill);
        
    public:
    	unsigned char      m_validTag;    	
    	int                m_itemCode;
        int                m_fee;
        int                m_discntFee;
        char m_displayType;
        T_MSISDN         m_serialNumber;          
        String<18>         m_spCode;
        String<18>         m_spBizCode;   
        char m_feeType;		//add by yuxk for MT
};
typedef vector<CMonfeeBill>::iterator MonfeeBillIter; 

class CMonfeeBillKey
{
public:	
		bool operator < (const CMonfeeBillKey &monfeeBillKey) const
        {
            if (m_spCode == monfeeBillKey.m_spCode)
            {              
                if(m_spBizCode == monfeeBillKey.m_spBizCode) 
                {
                    return (m_itemCode < monfeeBillKey.m_itemCode);
                }
                else
                {
                    return (m_spBizCode < monfeeBillKey.m_spBizCode);                    
                }
            }
            else
            {
                return (m_spCode < monfeeBillKey.m_spCode);
            }        
        }
        
        bool operator == (const CMonfeeBillKey& right)
        {
            return ((m_spCode == right.m_spCode)&&
                    (m_spBizCode == right.m_spBizCode)&&
                    (m_itemCode == right.m_itemCode));
        }
public:	
	    int                m_itemCode;
        String<18>         m_spCode;
        String<18>         m_spBizCode;        
};

typedef map<CMonfeeBillKey,MonfeeBillIter> MonfeeBillKeyMap;
typedef map<CMonfeeBillKey,MonfeeBillIter>::iterator MonfeeBillKeyMapIter; 

typedef ThriftyList <CMonfeeBill,MyAssignPolicy,
    MatrixVector<CMonfeeBill,BILL_PRE_VECTSIZE>,
    MatrixVector<ThriftyListBlockHead,HEADER_PRE_VECTSIZE> > MonfeeBillList;
        
typedef ThriftyList <CMonfeeBill,MyAssignPolicy,
    MatrixVector<CMonfeeBill,BILL_PRE_VECTSIZE>,
    MatrixVector<ThriftyListBlockHead,HEADER_PRE_VECTSIZE> >::iterator MonfeeBillListIter;

/**
 * User：帐户下用户类.
 */
class CUser
{
    public:

        CUser() {;}
        
        ~CUser() {;}
        
        CUser (const CUser& user)
        {            
            m_userId = user.m_userId;
            m_dataType = user.m_dataType;
        }
        
    public:

        CUser& operator =(const CUser& user)
        {
            if ( this != &user)
            {                
                m_userId = user.m_userId;
                m_dataType = user.m_dataType;  
            }
            
            return *this;
        }    
        
        friend bool operator==(const CUser& left,const CUser& right);
        friend bool operator!=(const CUser& left,const CUser& right);
        friend bool operator<(const CUser& left,const CUser& right);
        
        friend ostream& operator <<(ostream& out, const CUser& user);
            
    public:        
        USERID  m_userId;
        char    m_dataType;
};

/**
 * UserBill：帐户及帐户帐单.
 */
class CUserBill
{
    public:
        CUserBill() {;}
        ~CUserBill() {;}
        
        CUserBill(const CUserBill& acct)       
        {
            m_user = acct.m_user;
            m_monfeeBillList = acct.m_monfeeBillList;
        }
        
    public:
        bool operator < (const CUserBill& acct) const
        {
            return (m_user < acct.m_user);
        }

    public:
        CUser        m_user;
        MonfeeBillList    m_monfeeBillList;        
};
                    
typedef MatrixVector<CUserBill,ACCT_PRE_VECTSIZE> UserBillVector;
typedef MatrixVector<CUserBill,ACCT_PRE_VECTSIZE>::iterator UserBillVectorIter;

typedef Bill_Index<CUserBill> UserBillIndex;            
typedef Bill_Index<CUserBill>::iterator UserBillIndexIter;        

/**
 * MonfeeBillData：帐户帐单数据.
 */
class CMonfeeBillData
{
    public:
         CMonfeeBillData() {;}
         ~CMonfeeBillData(){;}

         CMonfeeBillData(const CMonfeeBillData& monfeeBillData)
         {
             m_ait = monfeeBillData.m_ait;
             m_monfeeBill = monfeeBillData.m_monfeeBill;
         }    
    
    public:
        CMonfeeBillData& operator =(const CMonfeeBillData& monfeeBillData)
        {
            if (this != &monfeeBillData)
            {
                m_ait = monfeeBillData.m_ait;
                m_monfeeBill = monfeeBillData.m_monfeeBill;
            }
            return *this;    
        }
            
    public:
        UserBillIndexIter   m_ait;
        CMonfeeBill        m_monfeeBill;         
};    

typedef multimap<CUser,CMonfeeBillData> MonfeeBillDataMMap;        
typedef multimap<CUser,CMonfeeBillData>::iterator MonfeeBillDataMMapIter;    

/**
 * MonfeeBillManager：帐户内存帐单管理.
 * 含有下面的方法:
 *   initialize：帐户帐单内存初始化
 *   end：       帐户帐单内存结束处理
 *   fileInit：  根据处理时间初始化帐户帐单内存
 *   fileCommit：帐户帐单内存同步并写文件
 *   setBill：   根据帐户变更帐单信息更新内存
 *   getBill：   从内存中获取帐户所有帐单信息
 */
class CMonfeeBillManager
{
    public:
        
        CMonfeeBillManager(): m_isNewUserAdd(false), 
                           m_isFirstOnce(true), m_sequenceNo(0) {;}
        
        ~CMonfeeBillManager() 
        {
            memset(m_day,0,sizeof(m_day));
            memset(m_dealTime,0,sizeof(m_dealTime));
        }
        
        /**
         * initialize：帐户帐单内存初始化
         * @param  MonfeeAccountConfig 帐户出帐配置信息.
         * @param  bcycId 计费帐期.
         * @return 1:帐单恢复成功 0:成功 -1:日志文件出错 
         *         -2: 帐单文件出错 -3:帐单初始化出错
         */
        int  initialize(const MonfeeConfig *monfeeConfig, 
                        const int& cycId);
        
        /**
         * end：帐户帐单内存结束处理
         * @param  void空
         * @return true:成功 false:失败
         */        
        bool end();
        
        /**
         * fileInit：根据处理时间初始化帐户帐单内存
         * @param  dealDate 处理时间 格式为yyyymmddhhmiss
         * @return true:成功 false:失败
         */        
        bool fileInit(const char *dealDate);
        
        /**
         * fileCommit：帐户帐单内存同步并写文件
         * @param  void空
         * @return true:成功 false:失败
         */
        bool fileCommit();

        /**
         * getBill：从内存中获取帐户所有帐单信息
         * @param   USERID                   帐户标识
         * @param   acctOriginalBillVector   帐户帐单信息
         * @return  无
         */        
        void getBill(const USERID& userId,const char& dataType,
                     vector<CMonfeeBill>& acctOriginalBillVector);
                     
        /**
         * updateBill：根据帐户变更帐单信息更新内存
         * @param   USERID                   帐户标识
         * @param   acctOriginalBillVector   帐户帐单信息
         * @return  无
         */        
        void updateBill(const USERID& userId,const char& dataType,    
                        vector<CMonfeeBill>& acctOriginalBillVector);

        
    private:
        
        void setAcctReserve(size_t acctSize);
        void setHeadReserve(size_t headSize);
        void setDataReserve(size_t dataSize);

        void setBillLogFilePath(const char *filePath);
        void setTotalBillFilePath(const char *filePath);
        
        /**
         * getBill：获取帐户＋用户的帐单集
         * @param   user        帐户用户信息类
         * @param   monfeeBillVector  帐户帐单集
         * @return  无
         */        
        void getBill(const CUser& user, 
                     vector<CMonfeeBill>& monfeeBillVector);
        
        /**
         * updateBill：更新帐户＋用户的帐单集
         * @param  user        帐户用户信息类
         * @param  monfeeBillVector  帐户帐单集
         * @return 无
         */        
        void updateBill(const CUser& user, 
                        vector<CMonfeeBill>& monfeeBillVector);
        
        /**
         * updateTotalBill：帐户帐单内存同步,不更新索引
         * @param  void空
         * @return true:成功 false:失败
         */
        bool updateTotalBill();

        /**
         * initFromIncreaseBillFile：根据增量帐单初始化帐单内存
         * @param  fileName 增量帐单文件名称
         * @return true:成功 false:失败
         */    
        bool initFromIncreaseBillFile(const char *fileName);
        
        /**
         * initFromTotalBillFile：根据全量帐单初始化帐单内存
         * @param  fileName 全量帐单文件名称
         * @return true:成功 false:失败
         */        
        bool initFromTotalBillFile(const char *fileName);

        /**
         * writeIncreaseBill：将增量帐单文件写入文件
         * @param  fileName 文件名称
         * @return true:成功 false:失败
         */    
        bool writeIncreaseBill(const char *fileName);
        
        /**
         * writeTotalBill：将全量帐单文件写入文件
         * @param  fileName 文件名称
         * @return true:成功 false:失败
         */    
        bool writeTotalBill(const char *fileName);

        /**
         * check：根据帐期,通道号进行内存核查
         * @param  bcycId    计费帐期
         * @param  channelNo 通道号
         * @return 1:帐单恢复成功 0:成功 
         *         -1:日志文件出错 -2: 帐单文件出错
         */
        int  check(const int& cycId, const int& channelNo);

        /**
         * check：根据帐期,通道号进行内存初始化
         * @param  bcycId    计费帐期
         * @param  channelNo 通道号
         * @return true:成功 false:失败
         */        
        bool initialization(const int& cycId, const int& channelNo);
        
    private:
  
        size_t           m_acctReserve;
        size_t           m_headReserve;
        size_t           m_dataReserve;

        UserBillVector       m_userBillVector;              /** 帐户全量帐单区. */
        UserBillIndex        m_userIndex;               /** 帐户全量帐单索引区. */ 
        MonfeeBillDataMMap m_monfeeBillDataMMap;        /** 帐户增量帐单区. */        

        string           m_totalBillFilePath;       /** 全量帐单文件目录. */
        string           m_totalBillFileName;       /** 全量帐单文件名称. */
        string           m_increaseBillFileNameBak; /** 增量帐单备份文件名称. */
        string           m_dayBillFileNameBak;      /** 日帐单备份文件名称. */        

        string           m_billLogFilePath;         /** 帐单日志文件路径. */
        string           m_billLogFile;             /** 帐单日志文件. */
        
        fstream          m_fstreamIncrease;
        fstream          m_fstreamTotal;
        fstream          m_fstreamLog;

        bool             m_isNewUserAdd;        /** 是否有新帐户、用户. */

        string           m_channelNo;               /** 通道号. */
        string           m_cycId;                  /** 计费帐期＋通道号. */
        char             m_day[DAYLEN+1];           /** 当前处理日期. */
        char             m_dealTime[DATETIMELEN+1]; /** 当前处理时间. */
        bool             m_isFirstOnce;             /** 是否首次运行. */
        int              m_sequenceNo;              /** 文件名称序列. */
};        
#endif //!ACCTBILLMANAGER_H
