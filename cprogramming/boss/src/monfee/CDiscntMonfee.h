/*
* Copyright (c) 2005,lianchuang
* All rights reserved.
* 
* 文件名称：CDiscntMonfee.h
* 文件标识：
* 摘    要：完成优惠关系处理
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/
#ifndef CDiscntMonfee_H
#define CDiscntMonfee_H

#include "MonfeeCommon.h"
#include "BillingCommon.h"
#include "IdInfo.h"

#include <sstream>
#include <stdlib.h>

class CMonfeeParamMgr;

class CDiscntMonfee 
{
    public:    		
    	~CDiscntMonfee();
    	
    	CDiscntMonfee();
    	
    	void discntMonfee(const IdInfo &idInfo, 
    	                  const CycParam &cycPara,	                      
	                      const char &fileType,
	                      vector<UserSubInfo> *userSubInfoVector,
	                      vector<MonfeeDiscount>    *discountVector,
	                      vector<UserServState> *userSVCStateVector,
	                      vector<IdErrorInfo>  *userErrorInfoVector,
	                      CMonfeeParamMgr *monfeeParamMgr,
	                      SpRelaDiscntMap *spRelaDiscntMap);		             
    
    private:    	
    	vector<UserSubInfo>         * m_userSubInfoVector;    	
    	vector<IdErrorInfo>         * m_userErrorInfoVector;    		
    	vector<MonfeeDiscount>    	* m_monfeeDiscountVector;
    	vector<UserServState>        * m_userSVCStateVector;
    	
			CMonfeeParamMgr          * m_monfeeParamMgr;      
			
			SpRelaDiscntMap				* m_spRelaDiscntMap;  
        
    	char        m_fileType;    	
    	int         m_fee;    	        
    	int         m_displayFee;
      CycParam    	m_cycPara;
    	IdInfo      m_idInfo;
      

      bool dealRelaDiscount(MonfeeDiscount &monfeeDiscount,
														vector<SpRelaDiscntDetail> & spRelaDiscntDetailVector);
      
    	void dealDiscount(MonfeeDiscount &monfeeDiscount);
    	
    	void dealEffect(UserSubInfo &userSubInfo, MonfeeDiscount &monfeeDiscount);
    	                
      bool isValidSubInfo(UserSubInfo &userSubInfo);        
      bool isValidDiscount(const MonfeeDiscount &monfeeDiscount); 
//      int  getfoundFee(UserSubInfo &userSubInfo, const int& itemCode, int &fee);		 	                
        int  getfoundFee(UserSubInfo &userSubInfo, const int& itemCode, int &fee,char &feeType);	//modify by yuxk for monfeediscnt	   	    
};

#endif /* CDiscntMonfee_H_H */
