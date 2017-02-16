#ifndef MONTHENDCOMMON_H
#define MONTHENDCOMMON_H

//#include "accountbase/AccountCommon.h"
#include "config-all.h"

#include <ctime>
#include <cstdio>
#include <string.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <set>

#if defined(STDIOSTREAM_DISABLE)
    #include <fstream.h>
    #include <strstream.h>
    #include <iostream.h>
#else
    #include <fstream>
    #include <strstream>
    #include <iostream>
#endif

#include "base/Types.h"
#include "FieldsXX.h"
#include "include/AltibaseIntf.h"
#include "MdbInfo.h"

//#ifdef __PROV022__
////add by fug 2009-11-20 s
////天津数据业务比例拆分账目常量定义
////端口主端付账目
//const int Driving_side[12]={
//73916,73918,73922,73924,
//73942,73944,73947,73949,
//73901,73903,73909,73911};
////端口从端付账目
//const int Odriving_side[12]={
//73915,73917,73921,73923,
//73941,73943,73946,73948,
//73900,73902,73908,73910};
////PVC主端付账目
//const int Driving_PVC[8]={
//73928,73930,73934,73936,
//73952,73954,73957,73959};	
////PVC对端付账目
//const int Odriving_PVC[8]={
//73927,73929,73933,73935,
//73951,73953,73956,73958};	
////77000040:端口付费模式，77000074：pvc付费模式
//const int Param_id[2]={77000040,77000074};
////add by fug 2009-11-20 e
//#endif

//定义用户、帐户、客户、帐单标识类型
typedef Number USERID;
typedef Number ACCTID;
typedef Number CUSTID;
typedef Number BILLID;

//定义无效的用户、帐户、客户、帐单标识值
const USERID   INVALID_USERID = 0;
const ACCTID   INVALID_ACCTID = 0;
const CUSTID   INVALID_CUSTID = 0;
const BILLID   INVALID_BILLID = 0;

//系统地市标识最大值及相关常量
const int MAX_EPARCHY   = 20;
const int MIN_EPARCHYID = 1;
const int MAX_EPARCHYID = 19;

//add for round 2009-9-9 13:53:13
//系统费用帐目项最大值及相关常量
const int MAX_ITEM   = 100000;
const int MIN_ITEMID = 1;
const int MAX_ITEMID = 99999;

//帐单费用舍取规则
const char CARRY_MODE_ROUND 	= '0';                             //四舍五入
const char CARRY_MODE_CUT 		= '1';                             //去尾数法
const char CARRY_MODE_ADD     = '2';                              //直接进位
const int  PRECISION_FACTOR   = 10;                              //精度
//add for round 2009-9-9 13:53:13

//定义输出文件信息结构
struct OutputFileInfo
{
	 public:	
    string   m_workFileName;
    ofstream *m_workOfStream;
    string   m_outputFileName;
    string   m_backupFileName;
    string   m_logFile;
    bool     m_isBackupValidFile;
    int      m_idNum;
    int64_t  m_recordNum;
    int64_t  m_feeSum;
    char     m_fileStat;
    int64_t  m_balanceSum;
    
};


struct UserImpInfo
{
	 public:	
		  	string	  m_userId;        //用户标识
    		string    m_productId;
    		string    m_brandCode;
    		string    m_msisdn;		  	
    		string	  m_netTypeCode;
		  	string    m_beginTime;    //生效时间
		  	string    m_endTime;      //结束时间
		  			  	
};

//定义帐期信息结构

//class CycParam
//{
//  public:
//  	T_DATE          m_cycleId;  //帐期YYYYMM
//  	T_DATE_TIME     m_beginTime;//起始时间
//  	T_DATE_TIME     m_halfTime; //半月时间点
//  	T_DATE_TIME     m_endTime;  //结束时间
//  	T_FLAG          m_useTag;   //使用标志：代表该帐期全部帐单是否已经生成，可以供用户缴费。
//  	T_FLAG          m_addTag;   //附加标志：系统预留的字段，供月末出帐时使用。
//  	friend ostream& operator<<(ostream& os,const CycParam& po)
//  	{
//  	  os<<" cycleId="<<po.m_cycleId
//  	    <<" beginTime="<<po.m_beginTime
//  	    <<" halfTime="<<po.m_halfTime
//  	    <<" endTime="<<po.m_endTime
//  	    <<" useTag="<<po.m_useTag
//  	    <<" addTag="<<po.m_addTag;
//  	  return os;
//  	}
//};

//定义系统参数信息结构
struct SysParamInfo
{
   string   m_sysParamCode;
   int      m_channelNo;
   int      m_valueN;
   string   m_valueS;
   string   m_valueD;
   int      m_preValueN1;
   int      m_preValueN2;
   int      m_preValueN3;
   string   m_preValueS1;
   string   m_preValueS2;
   string   m_preValueS3;
   string   m_preValueD1;
   string   m_preValueD2;
   string   m_preValueD3;
   char     m_useTag; 
};

//定义地市帐期表结构
struct CycleTimeEparchy
{
   string   m_eparchyCode;
   int      m_cycleId;
   string   m_startDate;
   string   m_endDate;
   char     m_acctTag; 
};



//以下定义程序执行错误编码常量
const int EMonthEndBASE    = 214000;

const int ERROR_OPEN_FILE        = EMonthEndBASE + 1; //文件打开错误 
const int ERROR_LINK_UNLINKE     = EMonthEndBASE + 2; //文件搬迁错误        
const int ERROR_INVALID_FILE     = EMonthEndBASE + 3; //帐单文件无效

const int ERROR_PARAM_FILE       = EMonthEndBASE + 4; //文件系统参数错误 
const int ERROR_PARAM_INITIALIZE = EMonthEndBASE + 5; //资料初始化错误
const int ERROR_PARAM_UPDATING   = EMonthEndBASE + 6; //资料更新错误
const int ERROR_PARAM_DATABASE   = EMonthEndBASE + 7; //数据库系统参数错误

const int ERROR_BILL_RECOVERY_OK = EMonthEndBASE + 8; //帐单恢复错误
const int ERROR_BILL_NO_LOG_FILE = EMonthEndBASE + 9; //帐单日志文件错误
const int ERROR_BILL_FILE        = EMonthEndBASE + 10;//帐单文件错误
const int ERROR_BILL_INITIALIZE  = EMonthEndBASE + 11;//帐单初始化错误
const int ERROR_BILL_SYNC        = EMonthEndBASE + 12;//帐单同步错误
const int ERROR_BILL_END         = EMonthEndBASE + 13;//帐单终止错误


//增加异常输出信息add by xuf 2010-1-7 17:12:09
const int ERROR_USER_EPARCHYCODE  = EMonthEndBASE + 21;//用户地市信息错误

DECLARE_EXCEPTION(MonthEndException, BusinessException);

//用户出帐输出文件类型最大值
const int MAX_OUT_CHANNELNUM = 1010;        //输出通道最多数目

const int MAX_OUT_BATCHNUM =5; //输出批次的最大数

//数据流量作用帐目和默认优惠编码
//extern int DATA_ITEM_CODE;
//extern int DEFAULT_DISCNT_CODE;

//定义文件头类型
const char FILE_TYPE_S = 'S';

//定义工作文件编号
const int WORK_FILE_S = 1;

//文件记录各列分割符
const char FIELD_SPLIT_D = ',';

const unsigned int MAX_RECORD_NUM = 100000;

//const unsigned int MAX_USER_NUM = 700000;
const unsigned int MAX_USER_NUM = 500;

const unsigned int MAX_STR_LEN = 256;

typedef struct  
{
 string     m_procName;
 string     m_procedure;
 string     m_execProc;
}ImportMode;
	
typedef pair<int,int> IntPair;
typedef vector< map<int,IntPair> > VectorPair;



//定义用户出帐系统配置信息                                         
//##ModelId=477DF5530225
struct MonthEndConfig
{
    //##ModelId=477DF5530353
    string m_hostName;

    //##ModelId=477DF553035C
    int m_interval;
    
		bool m_isBackupValidFile;
    
    bool m_isBackupInvalidFile;
    //added by pangx on 2010-3-29 begin
	string m_strOneClickPayProvinceCode;
	//added by pangx on 2010-3-29 end
    //##ModelId=477DF553035D
    string m_userName;

    //##ModelId=477DF553035E
    string m_passWord;

    //##ModelId=477DF5530366
    string m_servName;
    
    int m_dbNo;
    
    string  m_crmUserName;
    string  m_crmPassWord;
    string  m_crmServName;
        
    map <string, ImportMode> m_cbsProcMap;
    map <string ,ImportMode> m_crmProcMap;
    map <string ,ImportMode> m_altiProcMap;
    
    string  m_altiUserName;
    string  m_altiPassWord;
    string  m_altiServName;
    string  m_altiPort;
    
	T_DATABASETYPE			    m_mdbType;
	string          m_mdbserver; 
	string          m_mdbHostName;
	string          m_mdbUserName;
	string          m_mdbPassWord;
	int             m_mdbPort;    

    //##ModelId=477DF5530367
    int m_channelNo;

    //##ModelId=477DF55303AC
    string m_outputFilePath;
    //add by dingr
    string m_outputFilePath_Z;
    
    string m_outputFileWorkPath;
    
		string m_backupFilePath;
	  string m_invalidFilePath;
    //added by pangx on 2010-3-29 begin
	 string m_vpdnOutputFilePath;
	 string m_vpdnBackupFilePath;
	  //added by pangx on 2010-3-29 end
    //##ModelId=477DF55303DF
    int m_sleepTime;
    
    unsigned long m_maxRecordNum;
    
    //不出帐用户网别 2009-11-28 18:26:50
    set <string> m_unAcctNetTypeCodes;

};


struct Area
{
	string m_areaCode;
};

struct Staff
{
	string m_departId;
};

struct Tag
{
	string m_tagChar;
};



typedef struct 
{
	SQLBIGINT   acct_id;
	SQLBIGINT   user_id;
	SQLBIGINT   partition_id;
	SQLINTEGER  integrate_item_code;
	SQLBIGINT   fee;
	SQLBIGINT   b_discnt;
	SQLBIGINT   a_discnt;
	SQLBIGINT   adjust_before;
	SQLBIGINT   bill_id;     
}sTmABill;  
 

const unsigned int MAXLONGBUFFLEN = 2000;


inline void DEBUG_PRINTF(const char *fmt,...)
{
#ifdef DEBUG
	va_list ap;
	va_start(ap,fmt);
	vprintf(fmt,ap);
	va_end(ap);
#endif
}


typedef vector<string> StringVector;
typedef StringVector::iterator StringVectorIterator;
	
	
/**
 * IntergrageBill：定义综合帐单信息.
 */ 
class IntergrateBill
{
    public:
        IntergrateBill() 
        {
        	 bill_id=0;
					 acct_id=0;
			     user_id=0;
			     partition_id=0;
			     cyc_id=0;
			     integrate_item_code=0;
			     fee=0;
			     balance=0;
			     print_fee=0;
			     b_discnt=0;
			     a_discnt=0;
			     adjust_before=0;
			     adjust_after=0;
			     late_fee=0;
			     late_balance=0;
			     latecal_date="0";
			     canpay_tag='0';
			     pay_tag='0';
			     bill_pay_tag='0';
			     update_time="0";
			     update_depart_id="0";
			     update_staff_id="0";
			     version_no="0";
			     eparchy_code="";  	
			     m_ispps = '0';
			     city_code="";
			     serial_number="";
			     net_type_code="";
        }
        ~IntergrateBill() 
        { 
           
        }
        
        IntergrateBill& operator =(const IntergrateBill& intergrateBill)
        {
            if ( this != &intergrateBill)
            {
            	bill_id=intergrateBill.bill_id;
						 	acct_id=intergrateBill.acct_id;
			    	 	user_id=intergrateBill.user_id;
			    	 	partition_id=intergrateBill.partition_id;
			    	 	cyc_id=intergrateBill.cyc_id;
			     		integrate_item_code=intergrateBill.integrate_item_code;
			     		fee=intergrateBill.fee;
			    	 	balance=intergrateBill.balance;
			    	 	print_fee=intergrateBill.print_fee;
			     		b_discnt=intergrateBill.b_discnt;
			     		a_discnt=intergrateBill.a_discnt;
			     		adjust_before=intergrateBill.adjust_before;
			     		adjust_after=intergrateBill.adjust_after;
			     		late_fee=intergrateBill.late_fee;
			     		late_balance=intergrateBill.late_balance;
			     		latecal_date=intergrateBill.latecal_date;
			     		canpay_tag=intergrateBill.canpay_tag;
			     		pay_tag=intergrateBill.pay_tag;
			     		bill_pay_tag=intergrateBill.bill_pay_tag;
			     		update_time=intergrateBill.update_time;
			     		update_depart_id=intergrateBill.update_depart_id;
			     		update_staff_id=intergrateBill.update_staff_id;
			     		version_no=intergrateBill.version_no; 
			     		eparchy_code = intergrateBill.eparchy_code;  
			     		city_code = intergrateBill.city_code;
			     		serial_number = intergrateBill.serial_number;
			     		net_type_code = intergrateBill.net_type_code;
            }
            
            return *this;
        }    
              
    public:
			long   bill_id;
			long   acct_id;
			long   user_id;
			long   partition_id;
			int    cyc_id;
			int    integrate_item_code;
			long   fee;
			long   balance;
			long   print_fee;
			long   b_discnt;
			long   a_discnt;
			long   adjust_before;
			long   adjust_after;
			long   late_fee;
			long   late_balance;
			string latecal_date;
			char    canpay_tag;//modified by pangx 2010-5-12
			char   pay_tag;
			char   bill_pay_tag;
			string update_time;
			string update_depart_id;
			string update_staff_id;
			string version_no;
			string eparchy_code;
			//add 2009-02-17 zhouq begin
			char   m_ispps;
			//add 2009-02-17 zhouq end
			string     city_code;
			string     serial_number;
			string     net_type_code;			
};		

//added by pangx on 2010-3-29 begin
//一点支付账目总数
#define ONECLICKPAYBILL_ITEM_TOTAL 66
#define ONECLICKPAYBILL_VPDN_ITEM 2
	
/**
 * VpdnOneClickPayBill：定义VPDN综合帐单信息.
 */ 
class VpdnOneClickPayBill
{
    public:
        VpdnOneClickPayBill() 
        {
        	    // pLocalProvince=NULL;
			pGroupId=NULL;
			//add by yangyb 20100830 start
			pServiceId="";
			pEparchyCode=NULL;
			pBizType = NULL;
			pCitycode="";
			//add by yangyb 20100830 end
      pSubinstanceid="";//add by yangyb 20100917
			fee_date=0;
			memset(&detailfee,0,sizeof(detailfee));
			totalfee=0;
			memset(&vpdnfee,0,sizeof(vpdnfee));
        }
        ~VpdnOneClickPayBill() 
        { 
           
        }
        
        VpdnOneClickPayBill& operator =(const VpdnOneClickPayBill &stVpdnOneClickPayBill)
        {
            if ( this != &stVpdnOneClickPayBill)
            {
				//pLocalProvince=stVpdnOneClickPayBill.pLocalProvince;
				pGroupId=stVpdnOneClickPayBill.pGroupId;
				pServiceId=stVpdnOneClickPayBill.pServiceId;
				fee_date=stVpdnOneClickPayBill.fee_date;
				pCitycode=stVpdnOneClickPayBill.pCitycode;
        pSubinstanceid=stVpdnOneClickPayBill.pSubinstanceid;//add by yangyb 20100917
				memcpy(&detailfee,&stVpdnOneClickPayBill.detailfee,sizeof(detailfee));
				totalfee=stVpdnOneClickPayBill.totalfee;
				memcpy(&vpdnfee,&stVpdnOneClickPayBill.vpdnfee,sizeof(vpdnfee));
            }
            
            return *this;
        }

	//将一点支付账单里各个账目费用清零		
        bool resetFee()
        {
          	memset(&detailfee,0,sizeof(detailfee));
			totalfee=0;
               memset(&vpdnfee,0,sizeof(vpdnfee));
               
               return true;
        }  
		
	   bool specialDeal()
	   {			
			int nSumLi=0;
			int nSumLi1=0;
			//将厘转换为分
			//将去掉的厘平摊到各个帐目
			for(int i=0;i<ONECLICKPAYBILL_ITEM_TOTAL;++i)
			{
				if(detailfee[i] !=0)
				{				    
					nSumLi += detailfee[i]%10;
					detailfee[i] /=10;
					if(nSumLi>=10)
					{
						detailfee[i] +=1;
						nSumLi -=10;					
					}
					else if(nSumLi<=-10)
					{
						detailfee[i] -=1;
						nSumLi +=10;	
					}
					totalfee +=detailfee[i];
				}
			}
			for(int i=0;i<ONECLICKPAYBILL_VPDN_ITEM;++i)
			{
				if(vpdnfee[i] !=0)
				{				    
					nSumLi1 += vpdnfee[i]%10;
					vpdnfee[i] /=10;
					if(nSumLi1>=10)
					{
						vpdnfee[i] +=1;
						nSumLi1 -=10;					
					}
					else if(nSumLi1<=-10)
					{
						vpdnfee[i] -=1;
						nSumLi1+=10;	
					}
					totalfee +=vpdnfee[i];
				}
			}
			if((nSumLi+nSumLi1)>=5)
			{
                  		detailfee[ONECLICKPAYBILL_ITEM_TOTAL-1] +=1;
			 }

			return true;
		}
		
    public:
		     string *pLocalProvince;
			string *pGroupId;
			//add by yangyb 20100830 start
			string pServiceId;
			string *pEparchyCode;
			string  *pBizType;
			string  pCitycode;
      //add by yangyb 20100830 end
      string pSubinstanceid;//add by yangyb 20100917
			int    fee_date;

			long    detailfee[ONECLICKPAYBILL_ITEM_TOTAL];
			long    totalfee;
			long    vpdnfee[ONECLICKPAYBILL_VPDN_ITEM];
};
//added by pangx on 2010-3-29 end
typedef struct  
{
	int m_channelNo;
	int m_startId;
	int m_endId;
	
}channelSt;
const int MAXBUFFLEN = 1024;

class CFeeSum
{
	public:
	
	CFeeSum():m_detailBillFeeSum(0),m_dBillAdjustBFeeSum(0),m_unAcctFeeSum(0),m_IntegrateFeeSum(0),m_tempFeeSum(0),m_noInfoFeeSum(0)
	{
		
	}
	
	CFeeSum(const long &r_value)
	{
	  m_detailBillFeeSum		=	r_value;
		m_dBillAdjustBFeeSum	=	r_value;
		m_unAcctFeeSum				=	r_value;
		m_IntegrateFeeSum			=	r_value;
		m_tempFeeSum					=	r_value;
		m_noInfoFeeSum				=	r_value;
	} 	
		
	CFeeSum& operator=(const CFeeSum& var){ 
		if (this!=&var)
		{
			m_detailBillFeeSum  = var.m_detailBillFeeSum;
			m_dBillAdjustBFeeSum= var.m_dBillAdjustBFeeSum;
			m_unAcctFeeSum      = var.m_unAcctFeeSum;
			m_IntegrateFeeSum   = var.m_IntegrateFeeSum;
			m_tempFeeSum        = var.m_tempFeeSum;
			m_noInfoFeeSum      = var.m_noInfoFeeSum;
	 }
		return  *this;
		}	
		
	 CFeeSum& operator=(const long &r_value){ 
			m_detailBillFeeSum   = r_value;
			m_dBillAdjustBFeeSum = r_value;
			m_unAcctFeeSum       = r_value;
			m_IntegrateFeeSum    = r_value;
			m_tempFeeSum         = r_value;
			m_noInfoFeeSum       = r_value;
			return *this;
		}	
		
	long m_detailBillFeeSum;
	long m_dBillAdjustBFeeSum;
	long m_unAcctFeeSum;
	long m_IntegrateFeeSum;
	long m_tempFeeSum;
  long m_noInfoFeeSum;
};

typedef CFeeSum FEESUM;
	

#endif //!MONTHENDCOMMON_H
