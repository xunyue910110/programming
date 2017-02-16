#ifndef USERINTERNALDATA_H_HEADER_INCLUDED_B8826EA2
#define USERINTERNALDATA_H_HEADER_INCLUDED_B8826EA2

/*
* Copyright (c) 2008,linkage
* All rights reserved.
* 
* 文件名称：MonthEndDeal.h
* 文件标识：
* 摘    要：开账内部数据类
* 
* 当前版本：1.0
* 作    者：
* 完成日期：2008-1-16 15:18
*
* 取代版本： 
* 原作者  ：
* 完成日期：
*/

#include "param/PM_MonthAccount.h"

//##ModelId=477C4654034A
class MonthInternalData
{
  public:
    //##ModelId=477C465501C8
    MonthInternalData();

    //##ModelId=477C465501C9
    ~MonthInternalData();

    //##ModelId=477C465501CA
    void initialize(const int& minRealCycId, 
                    PM_MonthAccount *monthAccountParam,
                    const char &zeroDetailBillIntoIntDbTag);

    //##ModelId=477C465501EA
    void checkUserBillInfo();

    bool execMonthTransBillPreCrm();

		bool execMonthTransbillCrm(); 
		
//    bool getTmAbill();
    
//    bool getIntergrateBill( );
    
    void getRealWriteoffTag();
    
    bool transDetailbillToIntegratebill();
      
   	
  private:

    //##ModelId=477C46550216
    void outPresentMoney(ostream& os, int& recordNum, int& feeSum);

    //##ModelId=477C4655023D
    int deleteBillType(const int eparchyId, const int billType);

    //##ModelId=477C46550240
    void getNumOfBillType(const int eparchyId, int &callItemNum, int &fixItemNum, int &spItemNum, int &otherItemNum);

    //##ModelId=477C4655024D
    void getNeedDepositTag();

  private:

    //##ModelId=477C46550175
    int m_minRealCycId;

    //##ModelId=477C46550181
//    char m_needDepositTag;

    //##ModelId=477C46550188
//    char m_zeroCallTag;

    //##ModelId=477DE75C01AE
    PM_MonthAccount *m_monthAccountParam;

    //##ModelId=477C4655019D
 //   vector<DetailBill> m_detailBillVector;

    //##ModelId=477C465501A0
//    vector<DetailItemFeeInfo> m_detailItemFeeInfoVector;

    //##ModelId=477C465501A7
//    vector<UserAccountBill> m_userAccountBillVector;

    //##ModelId=477C465501AA
//    vector<AdjustBBill> m_adjustBBillVector;

    //##ModelId=477C465501BD
//    vector<PresentMoney> m_presentMoneyVector;

 //   vector <sTmABill>  m_tmABillVector;
    
 //   vector <IntergrateBill> m_intergrateBillVec;
     
 //   char m_zeroDetailBillIntoIntDbTag;
   // string m_channelNo;
    
};



#endif /* USERINTERNALDATA_H_HEADER_INCLUDED_B8826EA2 */
