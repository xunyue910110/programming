#ifndef USERBILLFACCOMMON_H
#define USERBILLFACCOMMON_H

#include "appfrm/Application.h"
#include "base/Directory.h"
#include "AccountCommon.h"

const int CARRY_MODE_ROUND=1;             //四舍五入法进位
const int CARRY_MODE_CUT=0;               //去尾法进位
const int MAX_USERACCOUNT_CHANNELNUM=100;  //最多用户通道数目 modifid 20091231
const int MAX_FILENAME_LEN=200;            //最大文件名称长度

//以下定义程序执行错误编码常量
const int ERRUSERBILLFACBASE          = 217000;

const int ERROR_OPEN_FILE           = ERRUSERBILLFACBASE + 1;//文件打开错误 
const int ERROR_LINK_UNLINKE        = ERRUSERBILLFACBASE + 2;//文件搬迁错误        
const int ERROR_INVALID_FILE        = ERRUSERBILLFACBASE + 3;//帐单文件无效

const int ERROR_PARAM_FILE          = ERRUSERBILLFACBASE + 4;//文件系统参数错误 
const int ERROR_PARAM_INITIALIZE    = ERRUSERBILLFACBASE + 5;//资料初始化错误
const int ERROR_PARAM_UPDATING      = ERRUSERBILLFACBASE + 6;//资料更新错误
const int ERROR_PARAM_DATABASE      = ERRUSERBILLFACBASE + 7;//数据库参数错误

const int ERROR_BILL_RECOVERY_OK    = ERRUSERBILLFACBASE + 8; //帐单恢复错误
const int ERROR_BILL_NO_LOG_FILE    = ERRUSERBILLFACBASE + 9; //帐单日志文件错误
const int ERROR_BILL_FILE           = ERRUSERBILLFACBASE + 10;//帐单文件错误
const int ERROR_BILL_INITIALIZE     = ERRUSERBILLFACBASE + 11;//帐单初始化错误
const int ERROR_BILL_SYNC           = ERRUSERBILLFACBASE + 12;//帐单同步错误
const int ERROR_BILL_END            = ERRUSERBILLFACBASE + 13;//帐单终止错误

const int ERROR_SUMBILL_RECOVERY_OK = ERRUSERBILLFACBASE + 14;//综合帐单恢复错误
const int ERROR_SUMBILL_NO_LOG_FILE = ERRUSERBILLFACBASE + 15;//综合帐单日志错误
const int ERROR_SUMBILL_FILE        = ERRUSERBILLFACBASE + 16;//综合帐单文件错误
const int ERROR_SUMBILL_INITIALIZE  = ERRUSERBILLFACBASE + 17;//综合帐单初始错误
const int ERROR_SUMBILL_SYNC        = ERRUSERBILLFACBASE + 18;//综合帐单同步错误
const int ERROR_SUMBILL_END         = ERRUSERBILLFACBASE + 19;//综合帐单终止错误

const int ERROR_GET_USERCHANNELNO   = ERRUSERBILLFACBASE + 20;//获取通道号码失败

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
    long    m_RecordNum ;//文件最大记录数      
};

//外部系统账单结构
struct UserBill
{
	T_SERVER_ID  m_userId;
	int          m_detailItemCode;
	long         m_discountFee;
	int          m_discountId;
	char         m_billTypeCode;
	int          m_priority;
	long         m_adjustInsId;
	long         m_effectId; //优惠作用ID add by xuf 2010-1-23 10:07:15
};

//优惠轨迹
class UserBillTrack
{
	public:
		int          m_detailItemCode;
		int          m_discountId;
		char         m_billTypeCode;
		int          m_priority;
		long         m_adjustInsId;
		long         m_effectId; //优惠作用ID add by xuf 2010-1-23 10:07:15
	public:
	    UserBillTrack(){}
	    ~UserBillTrack(){}
	    
	    friend bool operator==(const UserBillTrack& left, const UserBillTrack& right)
        {
        	#ifdef _DEBUG_
        	cout<<"left:"<<left.m_discountId<<" right:"<<right.m_discountId<<endl;
        	#endif
        	return ((left.m_detailItemCode==right.m_detailItemCode) &&
        	        (left.m_discountId==right.m_discountId) &&
        	        (left.m_billTypeCode==right.m_billTypeCode) &&
        	        (left.m_priority==right.m_priority) &&
        	        (left.m_adjustInsId==right.m_adjustInsId)&& 
        	        (left.m_effectId==right.m_effectId)); //add by xuf 2010-1-23 10:10:57
        }    
        friend ostream& operator<<(ostream& os, const UserBillTrack& userBillTrack)
        {
        	os << userBillTrack.m_detailItemCode       << ',' <<
        	      userBillTrack.m_discountId           << ',' <<
        	      userBillTrack.m_billTypeCode         << ',' <<
        	      userBillTrack.m_priority             << ',' <<
        	      userBillTrack.m_adjustInsId          << ',' <<
        	      userBillTrack.m_effectId          ; //add by xuf 2010-1-23 10:10:51
           return os;
        }
        
        //for map's sort
        friend bool operator < (const UserBillTrack& left, const UserBillTrack& right)  
        {           
            if	(left.m_detailItemCode  == right.m_detailItemCode )
    				{
    					if (left.m_billTypeCode == right.m_billTypeCode)
    					{
    							if ( left.m_priority == right.m_priority )
    							{
    									if ( left.m_discountId == right.m_discountId )
    									{
    										//add by xuf 2010-1-23 10:10:43
    										if (left.m_effectId == right.m_effectId)
    										  return (left.m_adjustInsId < right.m_adjustInsId);
    										else 
    											return (left.m_effectId < right.m_effectId);
    									}
    									else
    									{
    										return (left.m_discountId < right.m_discountId);
    									}
    							}
    							else
    							{
    								return (left.m_priority < right.m_priority);
    							}
    					}
    					else
    					{
    						return (left.m_billTypeCode < right.m_billTypeCode);
    					}		
						}
						else
						{
							return (left.m_detailItemCode < right.m_detailItemCode);
						}
        }
};

class UserKey
{
	public:
		T_SERVER_ID    m_id;
		char           m_idType;
		//T_SERVER_ID    m_userId;
	public:
	    UserKey(){}
	    ~UserKey(){}
	    
	    friend bool operator==(const UserKey& left, const UserKey& right)
        {
        	/*
        	return ((left.m_id==right.m_id) &&
        	        (left.m_idType==right.m_idType) &&
        	        (left.m_userId==right.m_userId));*/
        	return ((left.m_id==right.m_id) &&
        	        (left.m_idType==right.m_idType));
        }    
        friend ostream& operator<<(ostream& os, const UserKey& userKey)
        {
        	os << userKey.m_id               << ',' <<
        	      userKey.m_idType           << ',' /*<<
        	      userKey.m_userId           << ','*/;
           return os;
        }
        
        //for map's sort
        friend bool operator < (const UserKey& left, const UserKey& right)  
        {           
            if	(left.m_id  == right.m_id )
    				{
    					/*
    					if (left.m_idType == right.m_idType)
    					{
    							return (left.m_userId < right.m_userId);
    					}
    					else
    					{
    						return (left.m_idType < right.m_idType);
    					}		*/
    					return (left.m_idType < right.m_idType);
						}
						else
						{
							return (left.m_id < right.m_id);
						}
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
                    return (m_fileName.substr(m_fileName.length()-14) <= 
                            fileNameInfo.m_fileName.substr(fileNameInfo.m_fileName.length()-14));
                }
                else
                {
                    return (m_bcycDate < fileNameInfo.m_bcycDate);
                }
            }
            else
            {
                return (m_validTag < fileNameInfo.m_validTag);
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
                /*
                else
                {
            	    return ((m_fileName.substr(1,6)+m_fileName.substr(16)) > 
            	            (fileNameInfo.m_fileName.substr(1,6)+
            	             fileNameInfo.m_fileName.substr(16)));
                }*/
                                
                if (m_bcycDate == fileNameInfo.m_bcycDate)
                {
                    return (m_fileName.substr(m_fileName.length()-14) >= 
                            fileNameInfo.m_fileName.substr(fileNameInfo.m_fileName.length()-14));
                }
                else
                {
                    return (m_bcycDate > fileNameInfo.m_bcycDate);
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

typedef map<UserKey, vector<UserBill> >                   USERBILLMAP;
typedef multimap<char, pair<int, vector<FileNameRule> > > FILENAMERULEMMAP;
typedef map<UserKey, vector<UserBill> >::iterator         USERBILLMAPITER;
typedef map<UserKey, vector<UserBill> >::const_iterator   USERBILLMAPCITER;
typedef multimap<char, pair<int, vector<FileNameRule> > >::iterator 
                                                          FILENAMERULEMMAPITER;
                                                          
typedef map<UserBillTrack, long>                             USERBILLFEEMAP;
typedef map<UserBillTrack, long>::iterator                   USERBILLFEEMAPITER;
                                                        
#endif //USERBILLFACCOMMON_H
