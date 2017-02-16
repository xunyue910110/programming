/*
* Copyright (c) 2005,lianchuang
* All rights reserved.
* 
* 文件名称：CRecvMonfee.h
* 文件标识：
* 摘    要：完成基本资费收取处理
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CRecvMonfee_H
#define CRecvMonfee_H

#include "MonfeeCommon.h"
#include "BillingCommon.h"
#include "IdInfo.h"

#include <sstream>
#include <stdlib.h>

typedef vector<UserSubInfo>::iterator UserSubInfoIter;

class CMonfeeParamMgr;

class CRecvMonfee 
{
    public:            
        ~CRecvMonfee();
        
        CRecvMonfee();
        
        void recvMonfee(const IdInfo &idInfo, 
                          const CycParam &cycPara,                           
                          const char   &dataType,
                          const String<14> &currentDate,
                          vector<UserSubInfo> *userSubInfoVector,
                          vector<UserServState> *userSVCStateVector,
                          vector<IdErrorInfo>  *userErrorInfoVector,
                          vector<UserSubInfo> *userSubBillVector, 
                          //SpPunishMap * spPunishMap,
                          CMonfeeParamMgr *monfeeParamMgr);
    
    private:        
        vector<UserSubInfo>         * m_userSubInfoVector;
        vector<UserSubInfo>         * m_userSubBillVector;//zhaogc add
        vector<UserSubInfoIter>     m_validSubInfoVector;//zhaogc add
        vector<UserServState>        * m_userSVCStateVector; 
        vector<IdErrorInfo>         * m_userErrorInfoVector;            
        vector<MonfeeTariff>      m_monfeeTariffVector;
       // SpPunishMap                 *m_spPunishMap;
        CMonfeeParamMgr          * m_monfeeParamMgr;
        
        
        CycParam    m_cycPara;
        IdInfo      m_idInfo;        
        String<14>      m_currentDate;              
        T_FLAG        m_isEndMonth;        
        char        m_dataType;                
        int         m_fee;                
        int         m_displayFee; 
        //zhaogc add begin
        string      m_tempSpCode;
        string      m_tempSpBizCode;   
        UserSubInfoIter m_userSubInfoIter;         
        
        //zhaogc add end
      private:  
        void dealTariff(vector<UserSubInfoIter>  &validSubInfoVector);        
        
        void dealEffect(MonfeeTariff &monfeeTariff,
                        UserSubInfo &userSubInfo,const int &dayNum);
        
        bool isValidSubInfo(UserSubInfo &userSubInfo);                           
        
        bool getAllSubInfo(vector<UserSubInfo> &userSubInfoVector);
        
        bool getValidSubInfo(const vector<UserSubInfoIter> &userSubInfoVector,
                             const MonfeeTariff & monfeeTariff);
        //add by yangyb 手机报包年、包半年
        bool getValidSubInfo(
                          UserSubInfo & userSubInfo,
                          const MonfeeTariff & monfeeTariff);
        void getDuration(const String<14>& endDate, const String<14>& startDate,
                        int& dayNum); 
		
		void getSecond(const String<14>& endDate, const String<14>& startDate,                        
		               unsigned int& secondNum);
		                
		bool freeInThreeDay(const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff);
		bool freeInOneMonth(const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff);
		bool freeInMonthEnd(const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff);
		bool freeIn24Hours(const vector<UserSubInfoIter> &userSubInfoVector,
						   const MonfeeTariff & monfeeTariff);
						   
		// Add by Malone 2008-5-28 start
	  bool freeInOneMonthHistory(const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff);
	  // Add by Malone 2008-5-28 end
	  
	  // Add by Malone 2008-6-27 start
	  bool freeIn72Hours(const vector<UserSubInfoIter> &userSubInfoVector,
							const MonfeeTariff & monfeeTariff);
	  // Add by Malone 2008-6-27 end
						   
		//add by yih 2009-01-14 start
		bool adjustRecvFeeByRealDay(const CycParam &cycPara,
		                            const T_DATE_TIME &svcStartTime,//yyyymmddhh24miss
		                            const int currentDate,//yyyymmdd
		                            const char recvMode,
		                            const int referFee,
		                            int &newFee);
		//add by yih 2009-01-14 end

    // Add by Malone 2009-07-16 start
		bool monthDiff(const char *start_Date,const char *end_Date,int &cycNum);
		int dayDiff(const vector<UserSubInfoIter> &userSubInfoVector);	
    // Add by Malone 2009-07-16 end			   
};

#endif /* CRecvMonfee_H */
