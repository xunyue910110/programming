#ifndef PM_USERACCOUNT_H_HEADER_INCLUDED_B8827B4B
#define PM_USERACCOUNT_H_HEADER_INCLUDED_B8827B4B

#include "dbparam/ParamManager.h"
#include "dbparam/ParamContainer.h"

#include "include/MonthEndCommon.h"
#include "include/AltibaseIntf.h"
#include "app/MonthEndOutputFile.h"

//#include "pmbilling/PO_UserInfo.h"
#include "PO_SysParamInfo.h"
#include "pmbilling/PO_CycleEparchy.h"
#include "PO_DetailItem.h" //add for round 2009-9-9 13:53:13
#include "pmbilling/IdInfo.h"
#include "PO_FreeInfoDef.h"

class MdbParamManager;

class CAccUser
{
	public:
		bool  operator <(const CAccUser& accUser) const
		{
			if (acct_id==accUser.acct_id)
			{
				return (user_id < accUser.user_id);	
			}
			else
			{
				return (acct_id< accUser.acct_id);
			}	
		}
		 	
	 	long acct_id;
	  long user_id;
	
};

//class PM_Account;

//##ModelId=477DE75C003A
class PM_MonthAccount:public ParamManager
{
  public:
    //##ModelId=477DE75D006F
    PM_MonthAccount();

    //##ModelId=477DE75D0070
    ~PM_MonthAccount();

    //##ModelId=477DE75D0071
    bool initialization();

    //##ModelId=477DE75D0078
    bool end();

    //##ModelId=477DE75D007B
    virtual bool addParamObject();

    //##ModelId=477DE75D007D
    virtual bool getData();


    //##ModelId=477DE75D008D
 //   bool getAcycParaFromBcycId(const int& bcycId, AcycPara& acycPara);

    //##ModelId=477DE75D00A0
    bool getMinRealCycId(const int dbNo, int& acycId);

    //##ModelId=477DE75D00A2
    bool getSysParamInfo(const string& sysParamCode, vector<SysParamInfo>& sysParamInfoVector);

   /**
    * getLocalProvinceCode:从系统参数获取本地省份编码
    * @param  provinceCode 省份代码
    * @return true:成功 false:失败
    */
    bool getLocalProvinceCode(string& provinceCode);

    //##ModelId=4781D5DD0153
    void getZeroDetailBillIntoDBTag( );

    /**
    * getLocalProvinceCodeFromCrm:获取本地地域编码
    * @param  provinceNo 地域编码
    * @return true:成功 false:失败
    */
    bool getLocalProvinceCodeFromCrm(string &provinceNo);

//    bool getCurrBcycIdByAcycId(int & r_cycid); 

   
    // 从应用中得到数据库联接信息
		void setAltiBaseConnInfo(const string &s_userName,const string &s_passWord,
                       			 const string &s_servName,const string &s_port);
		bool reconnect();

    void setCrmConnInfo(const string &s_userName,
                        const string &s_passWord,
                        const string &s_servName);
                        
    bool getTmAbillFromDetailBill(vector <sTmABill> &tmABillVector);  
    
    bool getTmAbillFromAdjustBefore(vector <sTmABill> &tmABillVector); 
    
    bool getRealWriteoffTag(char &realWriteoffTag);
            
    bool parseProcString(const char * procNameStr, char * procName, char Val[10][40],char Sym[10][10],int& Sum);
       
    void setCrmProcInfo(map <string,ImportMode> ProcedureMap);
    
    void setCbsProcInfo(map <string,ImportMode> ProcedureMap); 
    
    void setAltibaseProcInfo(map <string,ImportMode> ProcedureMap); 
    
    void setOutPutFile(MonthEndOutputFile *pMonthEndOutputFile);//2008-3-4 20:11 使用缓冲技术
    
    void setMaxRecordNum(unsigned long lMaxRecordNum);  
      
    bool getOutPutChannelNoFromCrm(vector<channelSt> &channelStVec); 
    
    bool getOutPutBatchNoFromCrm(int &r_batchNo);
       
    bool execMonthTransbillCrm();      
  
    bool execMonthTransBillPreCrm();
    
    void getFeeSum(FEESUM &feeSum);
    
    //add 2008-4-23 11:10
    bool IsSpecialAcctId(ACCTID acct_id);
    void getSpecialAcctId();
    
    //2008-5-2 17:35
    void getTsAbillFromDetailAdjust();
    void getUnacctUser();
    void DeleteUnacctUser();
    void UpdateBillId();
    void GetBillId(int billnum);
    long GetNextBillId();
    
    bool getIntergrateBill();
    void outIntergrateBill();

		//add 2009-02-10  zhouq begin
    void getUserNetTypeCode(); 
    bool getNetType(long user_id);
    //add 2009-02-10  zhouq end  
    //add by yangyb 20100830  start
    bool getBizTypeThroughNetTypeCode(const string &r_NetTypeCode,
	 													string &r_bizType);
	 bool getNetTypeCodeSort(const string  &r_strValue, 
	                                  vector<string>    &r_netTypeCodeVector
	                               );
	  string getCityCode(string &eparchy_code);//add by yangyb 2010-08-30
    //add by yangyb 20100830 end 
    string getSubinstanceidByUserid(long &user_id); //add by yangyb 20100917
  //add for round 2009-9-9 13:53:13  
 	/*
 	* 根据帐目用途，获取明细帐目
 	* @param detailItemVector     明细帐目容器
 	* @param r_existFlag          帐目用途
 	* @param r_carryMode          进位方式 	
 	* @return true 找到, false 未找到
 	*/   
	bool getDetailItem(DetailItemInfo  r_detailItem[ ],unsigned char *r_existFlag, unsigned char *r_carryModes);      
  
    /**
    * carry: 进位.
    * @param  帐目.
    * @param  值.
    * @return 进位后的值.
    */    
  long carry(const int &r_itemCode, const long &r_value);
  //add for round 2009-9-9 13:53:13
  
 	/*
 	* 取当前需要开账的地市
 	*/    
 	bool getActEparchyCode(int realCycId,int dbNo,vector<string> &actEparchyCode);
 	
    //设置Mdb接口信息
    bool setMdbInfo(const MdbInfo &r_mdbInfo); 
 
 //#ifdef  __PROV022__   	
   //add by fug 2009-11-20 s
    int getUserParamInfo  (const long &r_userId,const int Param_id);
   //add by fug 2009-11-20 e
 //#endif
   
    int getUserImpInfo  (const long &r_userId,UserImpInfo & userImpInfo);
    void getUserMoreInfo(IntergrateBill & userBill);
    
	int getUserInfo(const T_SERVER_ID &r_userId,IdInfo      &r_userInfo)    ;

//modified by chenyong 2009-10-25--begin    
	void getUnAccInfo(const int &dbNo);

	void string_replace(string &strBig, const string &strsrc, const string &strdst);

	bool isSpecialRec(const long &acctid, const long &userid, const int &itemid);
//modified by chenyong 2009-10-25--end
  	void setUnAcctNetCodes(const set <string> &r_setUnAcctNetCodes);
  	
  //#ifdef __PROV019__ //山西 取无效用户
	//void getSleepUser();
  //#endif		
  void setOneClickPayProvinceCode(string &strOneClickPayProvinceCode); 
        bool getOneClickPayAccountInfo(); 
        bool getVpdnOneClickPayItemPosInfo();                                                                               
	      bool getExternalItemInfo();                                                      
   	    void outVpdnOneClickPayBill(vector<VpdnOneClickPayBill> &vecVpdnOneClickPayBill);
   	    bool judgeOneClickBill(long acct_id);
  public:

     /**
     * getEparchyId:根据省份编码和地市编码获取地市标识
     * @param  provinceCode      省份编码.
     * @param  eparchyCode       地市编码.
     * @param  eparchyId         地市标识.
     * @param  useTag            使用标志.
     * @return true:成功 false:失败
     */
 //   bool getEparchyId(const string& provinceCode,
//                          const string& eparchyCode,
//                          int& eparchyId, char& useTag);
    	     
  private:
    
    MonthEndOutputFile *m_pMonthEndOutputFile; //2008-3-4 20:14
		 
    //##ModelId=477DE75D0064
    bool m_accountParamNeedUpdated;

    //##ModelId=477DE75D005C
 //   PM_Account *m_accountParam;
    
    abdb::AltibaseIntf *m_altiDbi; 
    	
		string m_altibaseUserName;
		string m_altibasePasswd;
		string m_altibaseServer;
		string m_altibasePort;
		bool m_altibaseConnected;
		
		DbInterface  m_crmDbi;
		
    //内存数据库管理接口类
	MdbParamManager   *m_pMdbManager;		
		
	  string m_crmUserName;
		string m_crmPasswd;
		string m_crmServer;
		bool m_crmConnected;
		
		char m_msgbuf[MAXLONGBUFFLEN+1];
				
	  int m_minRealCycId;
	  //added by pangx on 2010-3-29 begin
	  T_DATE_TIME m_beginTime;
	  T_DATE_TIME m_endTime;
	  //added by pangx on 2010-3-29 end
	  T_DATE_TIME m_cycBeginTime;
	  T_DATE_TIME m_cycEndTime;	  
	  
	  map <string,ImportMode> m_crmProcedureMap;
	  map <string,ImportMode> m_cbsProcedureMap;
	  map <string,ImportMode> m_altibaseProcedureMap;
	  
	  string m_provinceCode;
	  string m_localProvinceCode;

	  char m_zeroDetailBillIntoIntDbTag;
	  
	  int m_nTotal;
		int m_nSuccess;
		int m_nError;
		
		unsigned long m_lMaxRecordNum;
		unsigned long m_lMaxUserNum;
		
		FEESUM m_feeSum;//统计费用
		
		//特殊账户，如测试卡账户
		vector <long> m_specialAcctIdVec;
		
		//bill_id
		map<CAccUser,long> m_tmabillidMap;
		vector <long> m_billidvec;
		vector <long>:: iterator m_billidvecit;
			
		set <long> m_unacctUser;

		//add 2009-02-10  zhouq begin
		set<long> m_userNetTypecode;
		//add 2009-02-10  zhouq end

    set<string> m_unAcctNetTypeCodes;
    
		long m_acctNum;
//		long m_recordNum;
	 
	  vector<IntergrateBill> m_intergrateBillVector;
//and by dingr 2009-12-05
 	  vector<IntergrateBill> m_freeFeeUserBillVector;
	 
	 	long m_acctidlast ;
	  long m_useridlast;
	  
//	  ParamContainer<PO_CycleTime>                m_PO_CycleTimeContainer;
    ParamContainer<PO_SysParamInfo>             m_PO_SysParamInfo;
//add for round 2009-9-9 13:53:13    
    ParamContainer<PO_DetailItem>          			m_PO_DetailItem;
    ParamContainer<PO_CycleEparchy>          	m_PO_CycleTimeEparchy;

		//对应明细帐目是否存在标识         
    unsigned char     m_existFlag[MAX_ITEM];
    
    //明细帐目数组
    DetailItemInfo    m_detailItem[MAX_ITEM];
    
    //对应明细帐目是否存在标识         
    unsigned char     m_carryModes[MAX_ITEM];
    
    int m_divisor;

//modified by chenyong 2009-10-25--begin    
		ParamContainer<PO_FreeInfoDef>	m_PO_FreeInfoDef;
		//map<CAccUser,int>								m_unaccAccttUserMap;
		multimap<CAccUser,int>								m_unaccAccttUserMap; //modified by xuf 2009-12-21 16:30:44
//modified by chenyong 2009-10-25--end
  //#ifdef __PROV019__ //山西 取无效用户   
	//	set <long> m_sleepUserVector; 
	//#endif	
	//added by pangx 2010-5-12 begin
    string m_strOneClickPayProvinceCode;
	//long 需要是64位的
	map<long,string> m_mapOneClickPayAccountInfo;
	map<int,int>     m_mapVpdnOneClickPayItemPosInfo;
	map<int,int>     m_mapExternalItemInfo;
	//added by pangx 2010-5-12 end
public:
 		long m_unAcctNetFee;
		long m_unAcctOtherFee;
		long m_unAcctRecords; 
		long m_recordNum; 
};




#endif /* PM_USERACCOUNT_H_HEADER_INCLUDED_B8827B4B */
