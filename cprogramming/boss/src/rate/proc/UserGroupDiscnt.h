#ifndef USERGROUPDISCNT_H_HEADER_INCLUDED_B72F6E7C
#define USERGROUPDISCNT_H_HEADER_INCLUDED_B72F6E7C
#include "DiscntBase.h"

// 用户群优惠：处理挂在群对象上的优惠（集团级，客户），如总量优惠。
//##ModelId=48B21A0E00BE
class UserGroupDiscnt : public DiscntBase
{
  public:
    //##ModelId=48B21A0E03AD
    UserGroupDiscnt();

    //##ModelId=48B21A0E03AE
    ~UserGroupDiscnt();

    //##ModelId=48B21A0E03AF
    virtual bool initialize(const T_USER_ID &r_id,
                            ID_TYPE r_idTypeCode,
                            RateGlobalInfo *r_pRateInfo,
                            PM_Account *r_pAccountParam ,
                            ComBill *r_pComBill);

   bool soluteDiscount( );

  private:
    


};



#endif /* USERGROUPDISCNT_H_HEADER_INCLUDED_B72F6E7C */
