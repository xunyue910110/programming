#ifndef ACCTATTRIBPROC_H_HEADER_INCLUDED_B59A0520
#define ACCTATTRIBPROC_H_HEADER_INCLUDED_B59A0520

#include "Procedure.h"
#include "ComBill.h"
#include "BillStruct.h"
typedef IdInfo    T_ID_INFO;

// 帐务属性求取过程
//##ModelId=4A5E9C2C0082
class AcctAttribProc : public Procedure
{
  public:
    // initUser
    // getUserInfo
    // getGroupInfoByUserId
    // getUserServState
    // getUserServ
    //##ModelId=4A5E9D300248
    int execute();

    /**
     * initUser 初始化用户
      * @param  r_userId         用户标识
      * @param  r_cycParam       帐期信息
      * @return 无 
     */  
    //##ModelId=4A64378C02D0
    bool initUser(const T_USER_ID &r_userId,const CycParam &r_cycParam);



    /**
    * getUserInfo 根据用户标识获取用户详细信息
    * @param  r_id         用户标识
    * @param  r_idInfo     用户信息结构
    * @return 无  
    */ 
    //##ModelId=4A6437AE006C
    bool getUserInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo);

    // 根据用户标识获取用户异动信息
    //##ModelId=48D6F7290077
    bool getUserServState();

    // 根据用户标识获取用户服务信息
    //##ModelId=48D6F7290077
    bool getUserServ();

        /**
		* getUserGroupInfo 根据群标识获取群详细信息
		* @param  r_id         群标识
		* @param  r_idInfo     群信息结构
		* @return true 成功,false 失败  
		*/ 
    //##ModelId=48D6F7B801DB
    bool getUserGroupInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo);

      /**
		* getAcctInfo 根据账户标识获取账户详细信息
		* @param  r_id         账户标识
		* @param  r_idInfo     账户信息结构
		* @return true 成功,false 失败  
		*/ 

    bool getGroupInfoByUserId();


  private:
	int m_errCode; 
	//##ModelId=4A6436F40209
	ComBill *m_pComBill;
	//资料管理对象指针
	//##ModelId=48DB3FDD032E
	PM_Account *m_pAccountParam;
	//优惠轨迹
	vector<DiscntBill>     *m_pDiscntBillVector;
    
    //附加账单：异常，集合，赠送等账单
	vector <AdditionalBill>  *m_pAddBillVector;
};



#endif /* ACCTATTRIBPROC_H_HEADER_INCLUDED_B59A0520 */
