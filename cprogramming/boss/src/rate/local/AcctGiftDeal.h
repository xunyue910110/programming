#ifndef _ACCTGIFTDEAL_H_INCLUDE_20081020_
#define _ACCTGIFTDEAL_H_INCLUDE_20081020_
#include "RateGlobalInfo.h"
#include "Mdb_UserOtherInfo.h"

class AcctGiftDeal
{
	public:
		AcctGiftDeal();
		~AcctGiftDeal();
	            
		void init(const T_SERVER_ID  &r_userId, 
							vector<UserOtherInfo>  *r_pUserOtherInfoVector,
	            vector<AccountBill> 	 *r_pAccountBillVector,
	            PMManager       			 *r_pmManager,
	            RateGlobalInfo  			 *r_pRateInfo);      
	            

		void execute();
	
	private:
		
		bool getGiftItemFee(const int& r_itemCode, long& r_fee);	
		bool getAddupInfo(const long& r_insId, const long &r_startDate, 
										  const int &r_discntItem, long &r_discntValue);
    void updateAddupInfo(const long &r_insId, const long &r_startDate,
    										 const int &r_discntItem,const long &r_endDate,
    										 const long &r_effectFee);
		void getAllFee(long& r_fee);
		void getDetailItemFromGiftItem(const int  & r_ItemCode);
		void updateSubIdDetailitemFee (const int  & r_detailItemCode, 
                                   const long & r_effectfee);
   
		
	private:
		T_SERVER_ID m_userId;
		vector<UserOtherInfo>  *m_pUserOtherInfoVector;
		vector<AccountBill>    *m_pAccountBillVector;
		vector<DetailItem> 			m_detailItemVector;
		unsigned char 					m_detailItem[MAX_ITEM];
		int 										m_existDetailItem[MAX_ITEM];
		PMManager       			 *m_pmManager;
		vector<UserSumBill2>   *m_pUserSumBill2s;
		int 										m_cycleId;
		RateGlobalInfo  			 *m_pRateInfo;
};

#endif //_ACCTGIFTDEAL_H_INCLUDE_20081020_
