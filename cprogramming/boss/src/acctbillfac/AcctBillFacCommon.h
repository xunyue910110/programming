#ifndef ACCTBILLFACCOMMON_H
#define ACCTBILLFACCOMMON_H

#include "appfrm/Application.h"
#include "base/Directory.h"
#include "AccountCommon.h"

const int CARRY_MODE_ROUND=1;             //四舍五入法进位
const int CARRY_MODE_CUT=0;               //去尾法进位
const int MAX_USERACCOUNT_CHANNELNUM=10;  //最多用户通道数目
const int MAX_FILENAME_LEN=200;            //最大文件名称长度

//以下定义程序执行错误编码常量
const int ERRACCTBILLFACBASE          = 217000;

const int ERROR_OPEN_FILE           = ERRACCTBILLFACBASE + 1;//文件打开错误 
const int ERROR_LINK_UNLINKE        = ERRACCTBILLFACBASE + 2;//文件搬迁错误        
const int ERROR_INVALID_FILE        = ERRACCTBILLFACBASE + 3;//帐单文件无效

const int ERROR_PARAM_FILE          = ERRACCTBILLFACBASE + 4;//文件系统参数错误 
const int ERROR_PARAM_INITIALIZE    = ERRACCTBILLFACBASE + 5;//资料初始化错误
const int ERROR_PARAM_UPDATING      = ERRACCTBILLFACBASE + 6;//资料更新错误
const int ERROR_PARAM_DATABASE      = ERRACCTBILLFACBASE + 7;//数据库参数错误

const int ERROR_BILL_RECOVERY_OK    = ERRACCTBILLFACBASE + 8; //帐单恢复错误
const int ERROR_BILL_NO_LOG_FILE    = ERRACCTBILLFACBASE + 9; //帐单日志文件错误
const int ERROR_BILL_FILE           = ERRACCTBILLFACBASE + 10;//帐单文件错误
const int ERROR_BILL_INITIALIZE     = ERRACCTBILLFACBASE + 11;//帐单初始化错误
const int ERROR_BILL_SYNC           = ERRACCTBILLFACBASE + 12;//帐单同步错误
const int ERROR_BILL_END            = ERRACCTBILLFACBASE + 13;//帐单终止错误

const int ERROR_SUMBILL_RECOVERY_OK = ERRACCTBILLFACBASE + 14;//综合帐单恢复错误
const int ERROR_SUMBILL_NO_LOG_FILE = ERRACCTBILLFACBASE + 15;//综合帐单日志错误
const int ERROR_SUMBILL_FILE        = ERRACCTBILLFACBASE + 16;//综合帐单文件错误
const int ERROR_SUMBILL_INITIALIZE  = ERRACCTBILLFACBASE + 17;//综合帐单初始错误
const int ERROR_SUMBILL_SYNC        = ERRACCTBILLFACBASE + 18;//综合帐单同步错误
const int ERROR_SUMBILL_END         = ERRACCTBILLFACBASE + 19;//综合帐单终止错误

const int ERROR_GET_USERCHANNELNO   = ERRACCTBILLFACBASE + 20;//获取通道号码失败

//账单排重配置结构
struct BillFacConfig
{
    string  m_fileRule;                    //文件命名规则
    bool    m_isNeedJudgeNameDetail;       //是否需要对文件名作详细判断
    bool    m_isNeedBackupValidFile;       //合法文件是否需要备份
    bool    m_isNeedBackupInvalidFile;     //不合法文件是否需要备份
    int     m_dealFileNum;                 //一次最多处理文件的个数
    int     m_channelNum;                  //通道个数
    int     m_sleepTime;                   //休眠时间
    int     m_iCarryMode;                  //进位方式是四舍五入还是去尾
    string  m_inputFilePath;                        
    string  m_outputFileWorkPath;                  
    string  m_outputFilePath[MAX_USERACCOUNT_CHANNELNUM]; 
    string  m_backupPath;                           
    string  m_invalidPath;                          
};

//外部系统账单结构
struct AcctBill
{
    String<4> m_eparchyCode;
	int       m_detailItemCode; 
	long      m_initfee;
	long      m_fee; 
	long      m_adiscount;
	long      m_adjustBefore;
	long      m_acctDay;
};

//外部系统账单费用组成结构
struct AcctBillFee
{
    String<4> m_eparchyCode;
	long      m_initfee;
	long      m_fee; 
	long      m_adiscount;
	long      m_adjustBefore;
	long      m_acctDay;	
};

class AcctKey
{
	public:
		T_SERVER_ID m_acctId;
		T_SERVER_ID m_userId;
	public:
	    AcctKey(){}
	    ~AcctKey(){}
	    
	    friend bool operator==(const AcctKey& left, const AcctKey& right)
        {
        	return ((left.m_userId==right.m_userId) &&
        	                             (left.m_acctId==right.m_acctId));
        }    
        friend ostream& operator<<(ostream& os, const AcctKey& acctKey)
        {
        	os << acctKey.m_acctId     << '\t' <<
        	      acctKey.m_userId     << '\t';
           return os;
        }
        
        //for map's sort
        friend bool operator < (const AcctKey& left, const AcctKey& right)  
        {
            return ((left.m_acctId == right.m_acctId)?
                    (left.m_userId < right.m_userId) :
                    (left.m_acctId < right.m_acctId) );
        }
};


const int VALID_FILE = 0;           //0.文件名称合法
const int NOT_ONE_BATCH_FILE = 1;   //1.文件名称合法但不属于此次处理的一批文件中
const int INVALID_FILE = -1;        //-1.文件名称非法无效


//文件名信息结构
class FileNameInfo
{
	public:
	    FileNameInfo(){}
	    
	    ~FileNameInfo(){}
	    
	    //定义小于<方法 for sort operation
	    //规则如下：(1)有效类型大的排在前面
	    //          (2)如果是无效文件，无所谓大小
	    //          (3)帐单时间小的排在前面        
        bool operator < (const FileNameInfo& fileNameInfo) const
        {
            if (m_validTag == fileNameInfo.m_validTag)
            {
                if (m_validTag == INVALID_FILE)
                {
                    return true;
                }
                
                if (m_bcycDate == fileNameInfo.m_bcycDate)
                {
                    return (m_fileName.substr(16) < 
                            fileNameInfo.m_fileName.substr(16));
                }
                else
                {
                    return (m_bcycDate < fileNameInfo.m_bcycDate);
                }
            }
            else
            {
                return (m_validTag > fileNameInfo.m_validTag);
            }
        }
        
	    //定义大于>方法 for greater<>() operation
	    //规则如下：(1)有效类型大的排在前面
	    //          (2)如果是无效文件，无所谓大小
	    //          (3)帐单时间大的排在前面            
	    bool operator > (const FileNameInfo& fileNameInfo) const
        {
            if (m_validTag == fileNameInfo.m_validTag)
            {
                if (m_validTag == INVALID_FILE)
                {
                    return true;
                }
                else
                {
            	    return ((m_fileName.substr(1,6)+m_fileName.substr(16)) > 
            	            (fileNameInfo.m_fileName.substr(1,6)+
            	             fileNameInfo.m_fileName.substr(16)));
                }
            }
            else
            {
                return (m_validTag > fileNameInfo.m_validTag);
            }
        } 
        
    public:    
        string  m_fileName;             //帐单文件名称
        string  m_bcycDate;             //帐单文件归属日期        
        int     m_validTag;             //帐单文件有效类型
};


//文件规则结构
struct FileNameRule 
{
    string::size_type   m_firstPos; 
    string::size_type   m_len;
    string              m_MaxValue; 
    string              m_MinValue;
};

typedef map<AcctKey, vector<AcctBill> >                   ACCTBILLMAP;
typedef multimap<char, pair<int, vector<FileNameRule> > > FILENAMERULEMMAP;
typedef map<AcctKey, vector<AcctBill> >::iterator         ACCTBILLMAPITER;
typedef map<AcctKey, vector<AcctBill> >::const_iterator   ACCTBILLMAPCITER;
typedef multimap<char, pair<int, vector<FileNameRule> > >::iterator 
                                                          FILENAMERULEMMAPITER;
                                                          
typedef map<int, AcctBillFee>                             ACCTBILLFEEMAP;
typedef map<int, AcctBillFee>::iterator                   ACCTBILLFEEMAPITER;
                                                        
#endif //ACCTBILLFACCOMMON_H
