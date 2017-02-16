#ifndef USERDISCNT_H_HEADER_INCLUDED_B72F3DF4
#define USERDISCNT_H_HEADER_INCLUDED_B72F3DF4
#include "DiscntBase.h"

// 用户优惠:处理单个用户的优惠
//##ModelId=48B21DB30341
class UserDiscnt : public DiscntBase
{
  public:
    //##ModelId=48B21DB4002C
    UserDiscnt();

    //##ModelId=48B21DB4002D
    ~UserDiscnt();

    //##ModelId=48B21DB4002E
    bool initialize(const T_USER_ID &r_userId,
                            ID_TYPE r_idTypeCode,
                            RateGlobalInfo *r_pRateInfo,
                            PM_Account *r_pAccountParam,
                            ComBill *r_pComBill);
    
   // bool getDiscount();
    
    //获取资费构成
   // bool getPolicyCompByTp();
    
    bool getUserFeeTrans();
    
    //取群上帐务资费
  //  bool getAcctGrpTp();
    
  //  bool getAcctTpInfo();

  private:



};



#endif /* USERDISCNT_H_HEADER_INCLUDED_B72F3DF4 */
