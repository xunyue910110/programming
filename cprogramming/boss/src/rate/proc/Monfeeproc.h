#ifndef _Monfeeproc_H_20100608
#define _Monfeeproc_H_20100608

#include "Procedure.h"
#include "AccountCommon.h"
#include "RateGlobalInfo.h"
#include "BillingConstDef.h"
#include "CDiscntMonfee.h"
#include "CRecvMonfee.h"
#include "base/StringUtil.h"
/*问题
 *********************************************************************************************
 *1、账单管理 (回滚账单添加字段，增加monFeebill账单表)                                   完成*
 *2、累积量（时间处理，可能需要添加altibase表）                                          完成*
 *3、表达式（M/T）移植                                                                   完成*
 *4、文件的输出:添加相应的包月费文件生成函数                                             完成*
 *5、异常处理（异常信息的收集m_addBillVector）：融合rate的异常以及错误收集机制           完成*
 *6、E文件特殊处理（生成该用户全量负账单抵消，相应经分稽核文件为生成，入文件输出）       完成*
 *7、包月费优惠（td_b_discnt支持CRM一对多）                                              完成*
 *8、eventor改造                                                                         完成*
 *9、包月费资费fee_type(M/Y)处理配置文件参数的合并                                       完成*
 *10、需要添加系统参数用来满足包月费M/T，以及文件输出目录                                完成*
 *11、包月费M/T依然沿用原版本处理，后续寻求改进措施                                    待处理*
 *12、新增包月费增量刷取订购关系类型(11)以及包月费回退文件E(10)                        待处理*
 *********************************************************************************************
*/

/*增加点
*********************************************************************************************
*1、内存数据库（MDB）添加包月费表3张                                                        *
*2、rate.cfg 新增包月费输出目录                                                             *
*********************************************************************************************
*/
class Monfeeproc:public Procedure 
{
  public:
  	~Monfeeproc();    	
     Monfeeproc();     
    virtual int execute();
    //初始化
    bool Initialize();
  private:
  	//包月费收取流程。
  	bool recvMonfee();
  	//包月费优惠流程。
  	bool discntMonfee(); 	
    //生成各种账单（monfeebill、outbill、cdrBill,userBill）
    bool getAllNewBill();    
    //根据老的monFeeBill获取老的cdrBill
    bool getOldCdrBill();    
    //根据新老账单生成相应的输出账单文件    
    bool getOutBillVector();
    //注册有错误的用户
    void RegisterErrorUser(const T_USER_ID &r_id, const int r_errCode);   
  private:
  	SpRelaDiscntMap m_spRelaDiscntMap;
  	vector<MonfeeDiscount> m_monfeeDiscountVector;

  	PCDR             m_pCdr;
  	CRecvMonfee m_recvmonfee;
  	CDiscntMonfee m_discntmonfee;
  	UserSubInfo m_userSubInfo;
  	char m_fileType;
  	char m_preTemp[512];
  	string m_seperate;
  	T_SERVER_ID m_userId;
  	IdInfo m_idInfo;
  	char m_isFileE;
    bool m_isiphone;
  	int m_unitfee_M; //MONFEE_UNITFEE_M
    int m_unitfee_T; //MONFEE_UNITFEE_T
    vector <string>	 m_iphoneVec; //MONFEE_IPHONE_PRODUCT
  private:
  	
  	//将要输出的内容生成之后更新到m_pRateInfo对应的输出容器中
  	vector<UserBill>			 m_userBillVector;
  	  
    vector<UserMonfeeBill> m_monFeeBillNew;
    vector<UserMonfeeBill> m_monFeeBillOld;
    
    vector<CdrBill>        m_cdrBillVectorNew;
		vector<CdrBill>        m_cdrBillVectorOld;
				
		vector<OutBill>				 m_outBillVector;
		vector<OutBill>        m_outBillVector_inc;
};
#endif