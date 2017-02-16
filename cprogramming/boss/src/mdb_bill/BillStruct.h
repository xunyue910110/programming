#ifndef _BILLSTRUCT_H_INCLUDE_20080702_
#define _BILLSTRUCT_H_INCLUDE_20080702_
//定义BILLING系统帐单结构
#include "config-all.h"
#include "FieldsXX.h"

const unsigned char	INIT_FEETYPE = 'Y';	//费用类型默认值Y
typedef struct
{
	string TableName;
	string IndexName;
}TABLE;

//const TABLE UserBillTable[2]      = {{"BILL_USER_0"     ,"IDX_BILL_USER_0"     },{"BILL_USER_1"     ,"IDX_BILL_USER_1"     }};
const TABLE UserBillTable[12]      = {{"BILL_USER_01","IDX_BILL_USER_01"},
	                                   {"BILL_USER_02","IDX_BILL_USER_02"},
	                                   {"BILL_USER_03","IDX_BILL_USER_03"},
	                                   {"BILL_USER_04","IDX_BILL_USER_04"},
	                                   {"BILL_USER_05","IDX_BILL_USER_05"},
	                                   {"BILL_USER_06","IDX_BILL_USER_06"},
	                                   {"BILL_USER_07","IDX_BILL_USER_07"},
	                                   {"BILL_USER_08","IDX_BILL_USER_08"},
	                                   {"BILL_USER_09","IDX_BILL_USER_09"},
	                                   {"BILL_USER_10","IDX_BILL_USER_10"},
	                                   {"BILL_USER_11","IDX_BILL_USER_11"},
	                                   {"BILL_USER_12","IDX_BILL_USER_12"}};
/*const TABLE UserMonfeeBillTable[12] = {{"BILL_USER_MONFEE_01","IDX_BILL_USER_MONFEE_01"},
	                                     {"BILL_USER_MONFEE_02","IDX_BILL_USER_MONFEE_02"},
	                                     {"BILL_USER_MONFEE_03","IDX_BILL_USER_MONFEE_03"},
	                                     {"BILL_USER_MONFEE_04","IDX_BILL_USER_MONFEE_04"},
	                                     {"BILL_USER_MONFEE_05","IDX_BILL_USER_MONFEE_05"},
	                                     {"BILL_USER_MONFEE_06","IDX_BILL_USER_MONFEE_06"},
	                                     {"BILL_USER_MONFEE_07","IDX_BILL_USER_MONFEE_07"},
	                                     {"BILL_USER_MONFEE_08","IDX_BILL_USER_MONFEE_08"},
	                                     {"BILL_USER_MONFEE_09","IDX_BILL_USER_MONFEE_09"},
	                                     {"BILL_USER_MONFEE_10","IDX_BILL_USER_MONFEE_10"},
	                                     {"BILL_USER_MONFEE_11","IDX_BILL_USER_MONFEE_11"},
	                                     {"BILL_USER_MONFEE_12","IDX_BILL_USER_MONFEE_12"}};*/
	                                     
const TABLE UserMonfeeBillTable[2] = {{"BILL_USER_MONFEE_0","IDX_BILL_USER_MONFEE_0"},
	                                   {"BILL_USER_MONFEE_1","IDX_BILL_USER_MONFEE_1"}};	                                     
//Modified By Sunam 2009/8/9 8:48:24
//const TABLE UserSumBill1Table[2]  = {{"BILL_USER_SUM1_0","IDX_BILL_USER_SUM1_0"},{"BILL_USER_SUM1_1","IDX_BILL_USER_SUM1_1"}};
//const TABLE UserSumBill2Table[2]  = {{"BILL_USER_SUM2"  ,"IDX_BILL_USER_SUM2"  },{"BILL_USER_SUM2"  ,"IDX_BILL_USER_SUM2"  }};
const TABLE UserSumBill1Table[12]  = {{"BILL_USER_SUM1_01","IDX_BILL_USER_SUM1_01"},
									  {"BILL_USER_SUM1_02","IDX_BILL_USER_SUM1_02"},
									  {"BILL_USER_SUM1_03","IDX_BILL_USER_SUM1_03"},
									  {"BILL_USER_SUM1_04","IDX_BILL_USER_SUM1_04"},
									  {"BILL_USER_SUM1_05","IDX_BILL_USER_SUM1_05"},
									  {"BILL_USER_SUM1_06","IDX_BILL_USER_SUM1_06"},
									  {"BILL_USER_SUM1_07","IDX_BILL_USER_SUM1_07"},
									  {"BILL_USER_SUM1_08","IDX_BILL_USER_SUM1_08"},
									  {"BILL_USER_SUM1_09","IDX_BILL_USER_SUM1_09"},
									  {"BILL_USER_SUM1_10","IDX_BILL_USER_SUM1_10"},
									  {"BILL_USER_SUM1_11","IDX_BILL_USER_SUM1_11"},
									  {"BILL_USER_SUM1_12","IDX_BILL_USER_SUM1_12"}};
const TABLE UserSumBill2Table     = {"BILL_USER_SUM2"  ,"IDX_BILL_USER_SUM2"  };
const TABLE UserSumBill3Table     = {"BILL_USER_SUM3"  ,"IDX_BILL_USER_SUM3"  };
const TABLE AccountBillTable[2]   = {{"BILL_ACCOUNT_0"  ,"IDX_BILL_ACCOUNT_0"  },{"BILL_ACCOUNT_1"  ,"IDX_BILL_ACCOUNT_1"  }};
/*const TABLE AccountBillTable[12]   = {{"BILL_ACCOUNT_01","IDX_BILL_ACCOUNT_01"},
	                                   {"BILL_ACCOUNT_02","IDX_BILL_ACCOUNT_02"},
	                                   {"BILL_ACCOUNT_03","IDX_BILL_ACCOUNT_03"},
	                                   {"BILL_ACCOUNT_04","IDX_BILL_ACCOUNT_04"},
	                                   {"BILL_ACCOUNT_05","IDX_BILL_ACCOUNT_05"},
	                                   {"BILL_ACCOUNT_06","IDX_BILL_ACCOUNT_06"},
	                                   {"BILL_ACCOUNT_07","IDX_BILL_ACCOUNT_07"},
	                                   {"BILL_ACCOUNT_08","IDX_BILL_ACCOUNT_08"},
	                                   {"BILL_ACCOUNT_09","IDX_BILL_ACCOUNT_09"},
	                                   {"BILL_ACCOUNT_10","IDX_BILL_ACCOUNT_10"},
	                                   {"BILL_ACCOUNT_11","IDX_BILL_ACCOUNT_11"},
	                                   {"BILL_ACCOUNT_12","IDX_BILL_ACCOUNT_12"}};
*/
//const TABLE RateDiscntBillTable[2]= {{"BILL_RATEDICNT_0","IDX_BILL_RATEDICNT_0"},{"BILL_RATEDICNT_1","IDX_BILL_RATEDICNT_1"}};
const TABLE RateDiscntBillTable[12]= {{"BILL_RATEDICNT_01","IDX_BILL_RATEDICNT_01"},
	                                   {"BILL_RATEDICNT_02","IDX_BILL_RATEDICNT_02"},
	                                   {"BILL_RATEDICNT_03","IDX_BILL_RATEDICNT_03"},
	                                   {"BILL_RATEDICNT_04","IDX_BILL_RATEDICNT_04"},
	                                   {"BILL_RATEDICNT_05","IDX_BILL_RATEDICNT_05"},
	                                   {"BILL_RATEDICNT_06","IDX_BILL_RATEDICNT_06"},
	                                   {"BILL_RATEDICNT_07","IDX_BILL_RATEDICNT_07"},
	                                   {"BILL_RATEDICNT_08","IDX_BILL_RATEDICNT_08"},
	                                   {"BILL_RATEDICNT_09","IDX_BILL_RATEDICNT_09"},
	                                   {"BILL_RATEDICNT_10","IDX_BILL_RATEDICNT_10"},
	                                   {"BILL_RATEDICNT_11","IDX_BILL_RATEDICNT_11"},
	                                   {"BILL_RATEDICNT_12","IDX_BILL_RATEDICNT_12"}};

class RollBackBill;

//用于进程内部的锁信息
class LockInfo
{
  public:
    T_SERVER_ID m_id;   //用户、帐户ID
    int         m_type; //ID类型 0用户 1帐户
  public:
    friend int operator< (const LockInfo &r_left,const LockInfo &r_right)
    {
      if(r_left.m_id<r_right.m_id) return true;
      if(r_right.m_id<r_left.m_id) return false;  
      return (r_left.m_type<r_right.m_type);
    }
    friend int operator==(const LockInfo &r_left,const LockInfo &r_right)
    {
      return (r_left.m_id  == r_right.m_id &&
              r_left.m_type== r_right.m_type);
    }
};

class UserBill
{
  public:
    T_SERVER_ID  m_userId;
    int          m_billId;
    long         m_value;
    long         m_discntValue;
    //Added By Sunam 2009/8/12 11:21:31
    long         m_adjustBefore;
  public:
    UserBill(){clear();}
    void clear(){memset(this,0,sizeof(UserBill));}
    //Modified By Sunam 2009/8/12 11:49:50
    //void clearValue(){m_value=m_discntValue=0;}
    void clearValue(){m_value=m_discntValue=m_adjustBefore=0;}
    //Added By Sunam 2009/8/12 21:44:50
    bool equal(const UserBill &left);
    friend ostream& operator<<(ostream& os,const UserBill& po);
    friend int operator<(const UserBill &left,const UserBill& right);
    friend int operator==(const UserBill &left,const UserBill& right);
    void operator=(const RollBackBill& right);
    UserBill& operator +=(const UserBill &r_right);
    UserBill& operator -=(const UserBill &r_right);
    bool isZero() const {return (m_value==0 && m_discntValue==0);}
    //add by xuf 2009-10-22 16:37:08
    UserBill& updatePart(const UserBill &r_right); 
};

class UserMonfeeBill
{
    public:
        UserMonfeeBill() {m_feeType[0]='Y';}                        
        void clear(){memset(this,0,sizeof(UserMonfeeBill));}
        void clearValue(){m_fee=m_discntFee=0;}
        bool equal(const UserMonfeeBill &left);
        
        friend ostream& operator<<(ostream& os,const UserMonfeeBill& po);
        friend int operator<(const UserMonfeeBill &left,const UserMonfeeBill& right);
        friend int operator==(const UserMonfeeBill &left,const UserMonfeeBill& right);          
        UserMonfeeBill& operator +=(const UserMonfeeBill &r_right);
        UserMonfeeBill& operator -=(const UserMonfeeBill &r_right);
        bool isZero() const {return (m_fee==0 && m_discntFee==0);} 
        void operator=(const RollBackBill& right);
    public:
        T_SERVER_ID        m_userId;
        char               m_dateType[2];
    	  int                m_itemCode;
        long               m_fee;
        long               m_discntFee;
        int                m_displayType;
        T_MSISDN           m_serialNumber;          
        String<18>         m_spCode;
        String<18>         m_spBizCode;
        char               m_feeType[2];	        
};

//包月费时间累计表
class UserSumBill3
{
  public:
  	T_SERVER_ID  m_userId;      //用户ID
    long         m_tpObjId;     //资费实例
    String<18>	 m_spCode;      //Sp编码
	  String<18>	 m_spBizCode;   //Sp业务编码
    long         m_beginTime;   //起始时间
    long         m_endTime;     //结束时间
    int          m_cycleId;     //帐期
    long         m_value;       //累计值

  public:
    UserSumBill3(){clear();}
    void clear(){memset(this,0,sizeof(UserSumBill3));}
    void clearValue(){m_value=0;}
    bool equal(const UserSumBill3 &left);
    friend ostream& operator<<(ostream& os,const UserSumBill3& po);
    friend int operator <(const UserSumBill3 &left,const UserSumBill3& right);
    friend int operator==(const UserSumBill3 &left,const UserSumBill3& right);
  void operator=(const RollBackBill& right);//回滚账单处理
  UserSumBill3& operator +=(const UserSumBill3 &r_right);
  UserSumBill3& operator -=(const UserSumBill3 &r_right);
    bool isZero()const {return (m_value==0);}
};

class UserSumBill1
{
  public:
    T_SERVER_ID  m_userId;
    long         m_tpObjId;
    int          m_billId;
    long         m_value;
    //Added By Sunam 2009/8/6 16:32:25
    T_TP_ID      m_feePolicyId;
    long         m_maxValue;
  public:
    UserSumBill1(){clear();}
    void clear(){memset(this,0,sizeof(UserSumBill1));}
    void clearValue(){m_value=0;}
    friend ostream& operator<<(ostream& os,const UserSumBill1& po);
    friend int operator< (const UserSumBill1 &left,
    		                  const UserSumBill1& right);
    friend int operator==(const UserSumBill1 &left,
    		                  const UserSumBill1& right);
	//Added By Sunam 2009/8/12 21:44:50
    bool equal(const UserSumBill1 &left);
    void operator=(const RollBackBill& right);
    UserSumBill1& operator +=(const UserSumBill1 &r_right);
    UserSumBill1& operator -=(const UserSumBill1 &r_right);
    bool isZero()const {return (m_value==0);}
};

class UserSumBill2
{
  public:
    T_SERVER_ID  m_userId;      //用户ID
    long         m_tpObjId;     //资费实例
    long         m_beginTime;   //起始时间
    long         m_endTime;     //结束时间
    int          m_cycleId;     //帐期（对帐务累计量有效，计费累计该值为0）
    int          m_billId;      //帐单项
    long         m_value;       //累计值
    int          m_type;        //帐单类型：BILL_TYPE_SUM2个人帐单、
                                //BILL_TYPE_GROUP集团帐单
    T_SERVER_ID  m_groupUserId; //集团用户标识
    //Added By Sunam 2009/8/6 16:32:25
    T_TP_ID      m_feePolicyId;
    long         m_maxValue;
  public:
    UserSumBill2(){clear();}
    void clear(){memset(this,0,sizeof(UserSumBill2));}
    void clearValue(){m_value=0;}
    friend ostream& operator<<(ostream& os,const UserSumBill2& po);
    friend int operator <(const UserSumBill2 &left,const UserSumBill2& right);
    friend int operator==(const UserSumBill2 &left,const UserSumBill2& right);
    //Added By Sunam 2009/8/12 21:44:50
    bool equal(const UserSumBill2 &left);
    void operator=(const RollBackBill& right);
    UserSumBill2& operator +=(const UserSumBill2 &r_right);
    UserSumBill2& operator -=(const UserSumBill2 &r_right);
    bool isZero()const {return (m_value==0);}
};

class AccountBill
{
  public:
    T_SERVER_ID  m_accountId;
    T_SERVER_ID  m_userId;
    int          m_billId;
    long         m_value;
    long         m_discntValue;
    long         m_a_discnt;   //帐务优惠
    long         m_b_discnt;   //帐前调整
    T_AREA_CODE  m_areaCode;   //归属区号
    long         m_acct_adjustBefore;//账户级调帐
	long         m_userGrpDiscnt; //用户+群优惠, 临时变量
	long         m_userAdjust;    //用户级调帐, 临时变量
  public:
    AccountBill(){clear();}
    void clear(){memset(this,0,sizeof(AccountBill));}
    void clearValue(){m_value=m_discntValue=m_a_discnt=m_b_discnt=m_acct_adjustBefore=0;}
    friend ostream& operator<<(ostream& os,const AccountBill& po);
    friend int operator<(const AccountBill &left,const AccountBill& right)
    {
      if(left.m_accountId < right.m_accountId) return true;
      if(left.m_accountId > right.m_accountId) return false;
      if(left.m_userId < right.m_userId) return true;
      if(left.m_userId > right.m_userId) return false;
      if(left.m_billId < right.m_billId) return true;
      if(left.m_billId > right.m_billId) return false;
      return false;
    }
    friend int operator==(const AccountBill &left,const AccountBill& right);
    //Added By Sunam 2009/8/12 21:44:50
    bool equal(const AccountBill &left);
    void operator=(const RollBackBill& right);
    AccountBill& operator +=(const AccountBill &r_right);
    AccountBill& operator -=(const AccountBill &r_right);
    bool isZero()const {return (m_value==0 && m_discntValue==0 &&
                           m_a_discnt==0 && m_b_discnt ==0&&m_acct_adjustBefore==0);}
};

class RateDiscntBill
{
  public:
    T_SERVER_ID  m_accountId;
    T_SERVER_ID  m_userId;
    int          m_billId;
    long         m_value;
  public:
    RateDiscntBill(){clear();}
    void clear(){memset(this,0,sizeof(RateDiscntBill));}
    void clearValue(){m_value=0;}
    friend ostream& operator<<(ostream& os,const RateDiscntBill& po);
    friend int operator<(const RateDiscntBill &left,const RateDiscntBill& right)
    {
      if(left.m_userId < right.m_userId) return true;
      if(left.m_userId > right.m_userId) return false;
      if(left.m_accountId < right.m_accountId) return true;
      if(left.m_accountId > right.m_accountId) return false;
      if(left.m_billId < right.m_billId) return true;
      if(left.m_billId > right.m_billId) return false;
      return false;
    }
    friend int operator==(const RateDiscntBill &left,const RateDiscntBill& right);
    //Added By Sunam 2009/8/12 21:44:50
    bool equal(const RateDiscntBill &left);
    void operator=(const RollBackBill& right);
    RateDiscntBill& operator +=(const RateDiscntBill &r_right);
    RateDiscntBill& operator -=(const RateDiscntBill &r_right);
    bool isZero() const{return (m_value==0 );}
};

//m_channelNo：为索引
//             无主键方式
//增量回滚帐单：用于异常回滚
class RollBackBill
{
  public:
    int          m_channelNo;     //批价通道号
    T_SERVER_ID  m_userId;        //用户表示
    int          m_billType;      //帐单类型（用于区分以上4类帐单）
                                  //0 UserBill     , 1 UserSumBill1
                                  //2 UserSumBill2 , 3 AccountBill
                                  //4 计费优惠累计
    int          m_billCycle;     //所在帐期(有可能一个文件的话单落在两个帐期)
    //以下为帐单具体信息：根据帐单类型来区分
    T_SERVER_ID  m_accountId;     //账户，只有帐单类型为3时有效,其它填0
    long         m_tpObjId;       //政策实例：只有帐单类型为1或2时有效,其它填0
    long         m_beginTime;     //起始时间：只有帐单类型为2时有效,其它填0
    long         m_endTime;       //结束时间：只有帐单类型为2时有效,其它填0
    int          m_billId;        //帐单项
    long         m_value;         //帐单值
    long         m_discntValue;   //账单优惠后值
    long         m_a_discnt;      //帐务优惠:帐户账单有效
    long         m_b_discnt;      //帐前调整:帐户账单有效
    int          m_type       ;   //账单类型（集团、用户）
    T_SERVER_ID  m_groupUserId;   //集团用户标识
    int          m_operFlag   ;   //0 更新、1 新增
    T_AREA_CODE  m_areaCode;   //归属区号
    //Added By Sunam 2009/8/6 16:32:25
    T_TP_ID      m_feePolicyId;
    long         m_maxValue;
    //Added By Sunam 2009/8/12 11:25:36
    long         m_adjustBefore;
  	long 				 m_userGrpDiscnt; //用户+群优惠
  	long 				 m_userAdjust;    //用户级调账
  	//Sp 包月费新增
    //int        m_displayType;   //调整类型
    char        m_dateType[2];      //Sp类型   
    char        m_feeType[2];       //fee类型（M/Y）
    String<18>    m_spCode;        //Spcode,包月费运营商编码
    String<18>    m_spBizCode;     //Sp话单编码
    T_MSISDN     m_serialNumber;  //手机号码
  public:
    RollBackBill(){clear();}
    void   clear(){memset(this,0,sizeof(RollBackBill));}
    friend ostream& operator<<(ostream& os,const RollBackBill& po);
    bool isZero()const {return (m_value==0 && m_discntValue==0 &&
                           m_a_discnt==0 && m_b_discnt ==0);}
};


class RateRunInfo
{
  public:
    int          m_channelNo;     //批价通道号
    int          m_state;         //处理状态：0 空闲,1 忙(文件处理中)
    char         m_fileName[64];  //正在处理的文件名(状态忙时有效)
    int          m_billCycle;     //计费帐期:YYYYMM
    int          m_rateDate;      //处理日期:处理日期 YYYYMMDD
    long         m_nextFileNo;    //下个文件序号
    char         m_remark[64];    //备注    
  public:
    friend ostream& operator<<(ostream& os,const RateRunInfo& r_obj);
};

//优惠明细信息结构
class DiscntBill
{
  public:
  	ID_TYPE       m_idType;      //ID类型，具体见ID_TYPE定义
  	char          m_discntType;  //'0':原始费用, >0: 见"优惠类型" 定义
  	T_SERVER_ID   m_id;          //user_id OR group_id OR customer_id OR acct_id
  	T_SERVER_ID   m_subId;	     //用户标识
  	int           m_detailItem;  //明细账目编码
  	long          m_discntFee;   //费用值
  	int           m_discntId;    //优惠编码(或调账原因编号)
  	int           m_orderNo;     //作用顺序号
  	long          m_adjustInsId; //调账编号
  	T_RULE_ID 		m_effectId;    //优惠作用标识
};

//附加账单信息结构（异常信息，赠送账单，稽核信息等)
class  AdditionalBill
{
public:
	AdditionalBill():m_billType("0"),m_InsId(0),m_id(""),m_idType(USER_ID_TYPE),m_operateType("0"),m_itemCode(0),m_factValue(0),m_fValue(0),m_sValue(0)
	{
		
	}
	
	T_FLAG       m_billType;			//账单类型标志: "0":异常信息；"1"：用户/账户/群赠送输出账单；"2"：稽核信息
	long         m_InsId;         //流水号:       异常信息和稽核信息: 操作流水号；赠送账单：资费实例号/资费号
	T_SERVER_ID  m_id;            //三户标识：    用户/账户/群标识符
	ID_TYPE      m_idType;        //标识类型：    '0':用户标志 '1':帐户标志  '2':客户标志  '3':群组标志
	T_FLAG       m_operateType;   //操作类型标志: 异常信息为操作类型标志：0:未处理   1:已处理；赠送账单：赠送类型（0：存折  1:优惠）
	int          m_itemCode;      //类型编码:     稽核信息:内容编码,异常信息：异常原因编码；赠送账单：赠送对象标识
	long         m_factValue;     //实际值:       稽核信息：实际值；赠送输出：赠送数量
	long         m_fValue;        //辅助数值1:    稽核信息：预期最小值；异常信息：辅助数值1；赠送账单：开始帐期	
	long         m_sValue;        //辅助数值2:    稽核信息：预期最大值；异常信息：辅助数组2；赠送账单：结束帐期

	AdditionalBill &operator=(const AdditionalBill& right)
	{	
		m_billType		=  right.m_billType;		
 		m_InsId				=	 right.m_InsId;     
 		m_id					=  right.m_id;         
 		m_idType			=  right.m_idType;     
 		m_operateType	=  right.m_operateType;
 		m_itemCode		=  right.m_itemCode;   
 		m_factValue		=  right.m_factValue;  
 		m_fValue			=  right.m_fValue;     
 		m_sValue			=  right.m_sValue;     
	
		return *this;
	}
};

//added by wanglu 2011-2-24 begin
//定义账本信息 
class AcctDeposit
{
  public:
  	long            m_acctBalanceId; //账本标识
    T_SERVER_ID  		m_acctId;        //帐户ID
    T_SERVER_ID     m_userId;        //用户ID
    int             m_depositCode;   //账本类型标识
    long         		m_oddMoney;      //奇数月账本费用
    long         		m_evenMoney;     //偶数月账本费用
    long            m_limitMoney;    //限定可销金额
    long            m_limitMode;     //限定可销类型
    long            m_startCycleId;  //起始帐期
    long            m_endCycleId;    //结束帐期
    char            m_validTag;      //是否有效标识
};
//end 2011-2-24
#endif //_BILLSTRUCT_H_INCLUDE_20080702_
