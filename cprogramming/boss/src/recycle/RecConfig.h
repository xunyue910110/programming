#ifndef __REC_CONFIG__
#define __REC_CONFIG__

#include "config-all.h"
#include "MdbInfo.h"

#include <string>
#include <fstream>
#include <vector>

USING_NAMESPACE(std)

#ifndef FILE_FORMAT_DEF
#define FILE_FORMAT_DEF //定义文件组织模式:分割符、定长
    enum  FILE_FORMAT {SEPARATE=1,FIX_LENGTH};
#endif  //FILE_FORMAT_DEF

#ifndef FIELD_MODE_DEF
#define FIELD_MODE_DEF  //定义输出处理模式:左补0、左补空格、右补空格、截空格、不处理
    enum  FIELD_MODE  {
        LEFT_ZEAO=0,
        LEFT_SPACE,
        RIGHT_SPACE,
        TRIM_SPACE,
        NO_DEAL
    };
#endif //FIELD_MODE_DEF

//Added by Sunam 2006-3-14 10:11 FOR DR
//定义排重字段定义格式
struct DividFormat
{
	int m_fieldId;
	int m_pos;
	int m_len;
};

/*前提：所有文件类型定义不重复，不交叉*/
class FileDividFormat
{
  public:
	char                m_fileType[10];//文件类型
	vector<DividFormat> m_rrFormat;
	
	friend int operator<(const FileDividFormat &left,const FileDividFormat& right)
	{
		//按照文件名从大到小排序。
		if (strcmp(left.m_fileType,right.m_fileType)>0)
		{
			return true;
		}
		return false;
    }
	friend int operator==(const FileDividFormat &left,const FileDividFormat &right)
	{
		if (strncmp(left.m_fileType,right.m_fileType,strlen(left.m_fileType)) == 0)
		{
			return true;
		}
		if (strncmp(left.m_fileType,right.m_fileType,strlen(right.m_fileType)) == 0)
		{
			return true;
		}
		return false;
	}
};
//2006-3-14 10:11 end

//定义字段输出规则
class FieldOutputRule {
  public:
    int         m_no;
    int         m_fieldId;
    string      m_fieldName;
    int         m_fieldLen;
    FIELD_MODE  m_mode;

  public:    
    void clear() {
        m_no        = 0;
        m_fieldId   = 0;
        m_fieldName = "";
        m_fieldLen  = 0;
        m_mode      = TRIM_SPACE;
    }
    
    friend bool operator < (FieldOutputRule &l ,FieldOutputRule &r) {
      return (l.m_no < r.m_no);
    };
    
    friend ostream& operator<<(ostream &os,FieldOutputRule &rule) {
        os << rule.m_fieldName <<"="
            <<rule.m_fieldId   <<","
            <<rule.m_fieldLen  <<","
            <<rule.m_mode<<endl;
        return os;
    }
};


//定义文件输出规则
class OutputRule {
  public:
      vector<string>  m_sourceType;     //数据源
      int m_sourceTypeIndex;
      
      string          m_outputFilePath; //正常回收话单输出目录
      string          m_bakFilePath;    //备份目录
//      string          m_billFilePath;    //备份目录
      string          m_fileHead;       //输出文件名前缀
      FILE_FORMAT     m_format;         //输出文件组织方式
      char            m_fieldSeparator; //字段间分割符
      string          m_lineSeparator;  //记录间分割符
      vector<FieldOutputRule>  m_filedOutputRules;//字段输出规则列表

  public:
      void clear() {
         m_filedOutputRules.clear();
      }

      friend ostream& operator<<(ostream &os,OutputRule &rule) {
          os << "--------OutputRule Begin--------"<<endl;
          os << "m_sourceType     :";
          for (vector<string>::iterator itr = rule.m_sourceType.begin();
               itr != rule.m_sourceType.end();itr++)
              os <<*itr<<",";
          os <<endl;
          os << "m_outputFilePath :"<<rule.m_outputFilePath<<endl;
          os << "m_bakFilePath    :"<<rule.m_bakFilePath<<endl;
          os << "m_fileHead       :"<<rule.m_fileHead<<endl;
          os << "m_format         :"<<rule.m_format<<endl;
          os << "m_fieldSeparator :"<<rule.m_fieldSeparator<<endl;
          os << "m_lineSeparator  :"<<rule.m_lineSeparator<<endl;
          os << "m_filedOutputRules:"<<endl;
          vector<FieldOutputRule>::iterator outItr;
          for (outItr = rule.m_filedOutputRules.begin();
               outItr != rule.m_filedOutputRules.end();
               ++outItr)
              os << *outItr;
          os << "--------OutputRule End--------"<<endl;
          return os;
      }
};

class CdrFormat
{
	public :
		void clear()			//清空所有字段
		{
				m_bizType								="";
				m_maxFieldNum						=0;
				m_sqlColumns						="";
        m_sourceTypeIndex				=0;
        m_bizTypeIndex          =0;
        m_userIdIndex           =0;
	 m_billItemIndex          = 0;
        m_msisdnAIndex          =0;
        m_startDateIndex        =0;
        m_startTimeIndex        =0;
        m_callDurationIndex     =0;
        m_roamTypeIndex         =0;
        m_freetimeId1Index      =0;
        m_freetimeValue1Index   =0;
        m_freetimeId2Index      =0;
        m_freetimeValue2Index   =0;
        m_freetimeId3Index      =0;
        m_freetimeValue3Index   =0;
        m_fee1Index             =0;
        m_fee2Index             =0;
        m_fee3Index             =0;
        m_fee4Index             =0;
        m_fee5Index             =0;
        m_discountFee1Index     =0;
        m_discountFee2Index     =0;
        m_discountFee3Index     =0;
        m_discountFee4Index     =0;
        m_discountFee5Index     =0;
        m_gprsDataUp1						=0;
        m_gprsDataDown1         =0;
        m_gprsDataUp2           =0;
        m_gprsDataDown2         =0;
        m_rateTimesIndex        =0;
        
        //Deleted by kongqm 2007-01-21 19:21:00 不再用查找账单明细表，直接利用清单中记录的账单项进行回退
        //m_feeidFactors.clear();
        
        m_endDateIndex					=0;
        m_endTimeIndex          =0;
        m_gprsTariffIndex				=0;
        m_gprsUpDown            =0;
        m_rateIndIndex          =0;
				m_FreeFluxId1Index      =0;
				m_FreeFluxValue1Index   =0;
				m_FreeFluxId2Index      =0;
				m_FreeFluxValue2Index   =0;
				m_FreeFluxId3Index      =0;
				m_FreeFluxValue3Index   =0;
				m_FreeFluxId4Index      =0;
				m_FreeFluxValue4Index   =0;
				m_FreeFluxId5Index      =0;
				m_FreeFluxValue5Index   =0;
				m_feeId1Index						=0;
				m_feeId2Index           =0;
				m_feeId3Index           =0;
				m_feeId4Index           =0;
				m_feeId5Index           =0;

		} 
		  
				string      m_bizType;
				int					m_maxFieldNum;
				string      m_sqlColumns;
        int         m_sourceTypeIndex;
        int         m_bizTypeIndex;
        int         m_userIdIndex;
	 int         m_billItemIndex;
        //Added by Sunam 2005-12-27 A号码的位置为无主回收使用
        int         m_msisdnAIndex;
        int         m_startDateIndex;
        int         m_startTimeIndex;
        int         m_callDurationIndex;
        int         m_roamTypeIndex;
        int         m_freetimeId1Index;
        int         m_freetimeValue1Index;
        int         m_freetimeId2Index;
        int         m_freetimeValue2Index;
        int         m_freetimeId3Index;
        int         m_freetimeValue3Index;
        int         m_fee1Index;
        int         m_fee2Index;
        int         m_fee3Index;
        int         m_fee4Index;
        int         m_fee5Index;
        int         m_discountFee1Index;
        int         m_discountFee2Index;
        int         m_discountFee3Index;
        int         m_discountFee4Index;
        int         m_discountFee5Index;
        //GPRS特殊属性
        int         m_gprsDataUp1;
        int         m_gprsDataDown1;
        int         m_gprsDataUp2;
        int         m_gprsDataDown2;
        int         m_rateTimesIndex;
        
        //Deleted by kongqm 2007-01-21 19:21:00 不再用查找账单明细表，直接利用清单中记录的账单项进行回退
        //vector<int> m_feeidFactors;
        
        int         m_endDateIndex;
        int         m_endTimeIndex;
        //Added by Sunam 2005-12-13
        int         m_gprsTariffIndex;
        int         m_gprsUpDown;
        //Added by Sunam 2005-12-7
				int         m_rateIndIndex;  
	    
				//Added by kongqm 2006-08-09 19:01:00
				int         m_FreeFluxId1Index;
				int         m_FreeFluxValue1Index;
				int         m_FreeFluxId2Index;
				int         m_FreeFluxValue2Index;
				int         m_FreeFluxId3Index;
				int         m_FreeFluxValue3Index;
				int         m_FreeFluxId4Index;
				int         m_FreeFluxValue4Index;
				int         m_FreeFluxId5Index;
				int         m_FreeFluxValue5Index;
	
				//Added by kongqm 2007-01-21 19:21:00 不再用查找账单明细表，直接利用清单中记录的账单项进行回退
				int					m_feeId1Index;
				int					m_feeId2Index;
				int					m_feeId3Index;
				int					m_feeId4Index;
				int					m_feeId5Index;
				
	   			//孔庆明增加 初始化一些参数
   			CdrFormat()
   			{
						clear();
   			}
   			
   			
};

class DataBaseCon
{
	public :
	string   m_bizType;
        bool     m_ifTruncate;   //执行m_truncateSql标记
        string   m_truncateSql;  //truncateSql清除语句
        string   m_cdrTable;
        bool     m_executeProc;  //执行存储过程标记
        string   m_procName;     //过程名
};

class RedoFlagConfig
{
	public:
		RedoFlagConfig()
		{
			m_execResult = "FAILURE";
		}
	public:
		string  m_bizType;
		int     m_billingCycle;
		int     m_channelNo;
		string  m_status;
		string  m_rowid;
		string  m_execResult;
};

class RecConfig {
    public:
    	string		        m_channelNo;
    	
    	//Added by Xiel 2006-01-15
    	string				m_homeAreaCode;
    	//Added by Sunam 2005-12-27 重做对应的批价通道类型
		string              m_channelType;
        string              m_serverName;   //数据库服务器名
        string              m_userName;     //用户名
        string              m_passwd;       //口令
        long                m_maxCdrNum;    //一个文件中包含的最大话单数
        string              m_logFile;      //日志文件名（带全路径）
        vector<DataBaseCon> m_dataBaseConfig;
        int                 m_dataBaseConfigIndex;
        string              m_selectSql;    //SQL出库语句
        string              m_flagTable;
        string		        m_rawDataPath;	//原始数据
        string		        m_rawDataPathTemp;	//原始数据临时目录
        //Added by Sunam 2005-12-7 省代码
        string              m_provinceCode;
        //Added by Sunam 2005-12-13 GPRS流量处理
        string              m_gprsFlowDeal;
        // 
        // 清单格式相关 (CDR_FORMAT)--------------------------------------
        vector<CdrFormat>  m_cdrFormat;
        //Added by Sunam 容灾 排重相关 2006-3-14 10:15
        vector<FileDividFormat> m_rrFormatList;
        
        int         m_cdrFormatIndex;
        //帐单文件相关-------------------------------------
        bool        m_needRollbackBill; 
	 string      m_tmpminusBillPath;
	 string      m_minusBillPath;
        string      m_totalBillPath;
        string      m_bakBillPath;
        string      m_billLogPath;
        
        //Added by Sunam 2006-3-14 14:56 
        string      m_drOutPutPath;

        //string      m_cycleSql;             //帐期SQL
        int         m_cycleDate;            //从相应通道中取得相应的帐期
        int         m_billingChannalNo;     //对应通道
        //int         m_discountFeeId;
        int         m_interProvRoamFeeId;
        int         m_interNationalRoamFeeId;
        time_t      m_timeStamp;
        
        //Added by Sunam 2006-3-14 11:32 FOR DR
        bool        m_drOutPut;
	int         m_sequenceNo;

	MdbInfo            m_mdbInfo;         //Mdb信息

        // 参数表相关 -------------------------------------
        string      m_dbParamUser;
        string      m_dbParamPassword;
        string      m_dbParamServName;
        
    public:
        friend ostream& operator<<(ostream &os,RecConfig &rule) {
            os << "m_serverName = "<<rule.m_serverName<<endl;
            os << "m_userName   = "<<rule.m_userName<<endl;
            os << "m_passwd     = "<<rule.m_passwd<<endl;
            os << "m_maxCdrNum  = "<<rule.m_maxCdrNum<<endl;
            os << "m_logFile    = "<<rule.m_logFile<<endl;
            return os;
        }
        
        int copy(const char *r_srcFile,const char *r_desFile) const
        {
          FILE *t_srcFile,*t_desFile;
          int t_readSize;
          const int BUFFLEN=1024;
          char t_msgBuff[BUFFLEN+1];
          char t_tmpFile[512+1];
          int  t_sLen,t_tLen;
          
          if(strrchr(r_desFile,'/')==NULL)
          {
            sprintf(t_tmpFile,"%%%s",r_desFile);
          }else
          {
            sprintf(t_tmpFile,"%s",r_desFile);
            t_tLen= strlen(r_desFile);
            t_sLen = strlen(strrchr(r_desFile,'/')+1);
            sprintf(t_tmpFile+t_tLen-t_sLen,"%%%s",strrchr(r_desFile,'/')+1);
          } 
          if((t_srcFile=fopen(r_srcFile,"rb"))==NULL)
          {
            #ifdef _DEBUG_
              cout<<"open file:"<<r_srcFile<<" error!"<<__FILE__<<__LINE__<<endl;
            #endif
            return -1;
          }
          if((t_desFile=fopen(t_tmpFile,"wb"))==NULL)
          {
            #ifdef _DEBUG_
              cout<<"open file:"<<t_tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
            #endif
            fclose(t_srcFile);
            return -1;
          }
          while(!feof(t_srcFile))
          {
            t_readSize=fread(t_msgBuff,1,BUFFLEN,t_srcFile);
            if(t_readSize == 0) break;
            if(fwrite(t_msgBuff,1,t_readSize,t_desFile)!=t_readSize)
            {
              fclose(t_srcFile);
              fclose(t_desFile);
              return -1;
            }
          }
          fclose(t_srcFile);
          fclose(t_desFile);
          if(rename(t_tmpFile,r_desFile)!=0)
          {
            #ifdef _DEBUG_
              cout<<"rename file:"<<t_tmpFile<<" "<<r_desFile<<" error!"<<__FILE__<<__LINE__<<endl;
            #endif
            return -1;
          }
          return 0;
        }
   			
};




#endif //REC_CONFIG_DEF_200407
