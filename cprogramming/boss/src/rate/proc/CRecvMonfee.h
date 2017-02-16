#ifndef _CRECVMONFEE_H_20100608
#define _CRECVMONFEE_H_20100608

#include "IdInfo.h"
#include "AccountCommon.h"
#include "BillingCommon.h"
#include "ComBill.h"
#include "RateGlobalInfo.h"
class RateGlobalInfo;
class CRecvMonfee
{
    public:
    	~CRecvMonfee();                    
         CRecvMonfee();
         //初始化
        bool Initialize(UserSubInfoExtra UserSubInfoExtra,UserSubInfo *userSubInfo,
                        char fileType,IdInfo idInfo,RateGlobalInfo  *pRateInfo,char isFileE);
        
        bool recvMonfee();
    private:
     bool dealTariff( UserSubInfo & userSubInfo);
     
     bool dealEffect(MonfeeTariff &monfeeTariff,UserSubInfo &userSubInfo,const int &dayNum);
     
     bool isValidSubInfo(const UserSubInfo &userSubInfo,const MonfeeTariff & monfeeTariff); 
     
     bool adjustRecvFeeByRealDay(const int currentDate,const char recvMode,const int referFee,int &newFee);   	
    private:
    	void getDealDate();//当前处理日期
    	 	     	
    	bool freeInThreeDay(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
	 
	  bool freeInOneMonth(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
	  
	  bool freeInMonthEnd(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
	  
	  bool freeIn24Hours(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
					   
    bool freeInOneMonthHistory(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
   
    bool freeIn72Hours(const UserSubInfo & userSubInfo,const MonfeeTariff & monfeeTariff);
    private:
        UserSubInfoExtra m_UserSubInfoExtra;//扩展信息字段
        UserSubInfo* m_userSubInfo;
        vector<MonfeeTariff>  m_monfeeTariffVector;
        char m_fileType;
        char m_isFileE;
        IdInfo m_idInfo;
        RateGlobalInfo  *m_gRateInfo;
        int m_fee;
        int m_displayFee;
        String<14>  m_currentDate;              
};
#endif