#ifndef _CDISCNTMONFEE_H_20100608
#define _CDISCNTMONFEE_H_20100608

#include "IdInfo.h"
#include "AccountCommon.h"
#include "BillingConstDef.h"
#include "BillingCommon.h"
#include "ComBill.h"
#include "RateGlobalInfo.h"
class CDiscntMonfee
{
    public:    		   	
    	~CDiscntMonfee();
    	CDiscntMonfee();    	
    	//初始化
      bool Initialize(UserSubInfo *userSubInfo,vector<MonfeeDiscount>* m_monfeeDiscountVector,
                      char fileType,IdInfo idInfo,
                      RateGlobalInfo  *pRateInfo,SpRelaDiscntMap* spRelaDiscntMap);
                         
    	bool discntMonfee();
    private:
    	bool dealRelaDiscount(MonfeeDiscount &monfeeDiscount,vector<SpRelaDiscntDetail> & spRelaDiscntDetailVector);      
    	void dealDiscount(MonfeeDiscount &monfeeDiscount);
    	void dealEffect(UserSubInfo &userSubInfo, MonfeeDiscount &monfeeDiscount);    	                
      bool isValidSubInfo(UserSubInfo &userSubInfo);        
      bool isValidDiscount(const MonfeeDiscount &monfeeDiscount); 
      int  getfoundFee(UserSubInfo &userSubInfo, const int& itemCode, int &fee);
      void getDealDate();//当前处理日期
    /* getAddupInfo:      获取累计值
     * @param  r_tpObjId    	资费实例.
     * @param  r_beginTime    起始时间.
     * @param  r_spCode       Spcode.
     * @param  r_spBizCode    SpBizcode.
     */
        bool getAddupInfo(const long &r_tpObjId, 
    	                const long &r_beginTime, 
    	                const string &r_spCode,
    	                const string &r_spBizCode,
    	                long &r_addupValue);
    	//根据资费起始时间获取累计周期起始时间  
    	bool getSumTimeInfo(const T_DATE_TIME &r_tpObjTime,
    	                    long &r_fStartTime,
    	                    const char &r_adduptype); 
    	void updateAddupInfo(const long &r_tpObjId,const string &r_spCode,const string &r_spBizCode, 
    	                     const long &r_beginTime,const long &r_endTime,const long &r_addupvalue) ;
    private:
    	UserSubInfo* m_userSubInfo;  	
    	vector<MonfeeDiscount>* m_monfeeDiscountVector;
    	
      SpRelaDiscntMap *m_spRelaDiscntMap;  
      RateGlobalInfo  *m_RateInfo;  
    	char        m_fileType;    	
    	int         m_fee;    	        
    	int         m_displayFee;
    	IdInfo      m_idInfo;
    	String<14>  m_currentDate; 
};
#endif