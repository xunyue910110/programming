#ifndef ACCTDISCNT_H_HEADER_INCLUDED_B72F1BC2
#define ACCTDISCNT_H_HEADER_INCLUDED_B72F1BC2
#include "DiscntBase.h"

// 功能：账户优惠，账户级的优惠，处理合帐用户优惠
// 日期：2008年8月
//##ModelId=48B21A0E006E
class AcctDiscnt : public DiscntBase
{
  public:
    //##ModelId=48B21A0E0259
    AcctDiscnt();

    //##ModelId=48B21A0E025A
    ~AcctDiscnt();

    //##ModelId=48B21A0E025B
    bool initialize(const T_USER_ID &r_id,
                    ID_TYPE r_idTypeCode,
                    RateGlobalInfo *r_pRateInfo,
                    PM_Account *r_pAccountParam ,
                    ComBill *r_pComBill);
    
    //##ModelId=48B21A0E027A
   // bool getDiscount(vector<Discount>     &r_discountVector);
    
    //获取资费构成
//    bool getPolicyCompByTp();
    
	bool soluteDiscount( );
    
  private:

    //##ModelId=48B21A0E02F1
    void specialDeal();
   
};



#endif /* ACCTDISCNT_H_HEADER_INCLUDED_B72F1BC2 */
