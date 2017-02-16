
#ifndef __RATEERROR__
#define __RATEERROR__


// 致命错误基数
const int E_DEFAULT  			    = 0               ;
const int E_RATE_BASE               = 900            ;
const int E_INITIALIZE       		= E_RATE_BASE +  1;   // 初始化错误
const int E_PARAM_UPDATING   		= E_RATE_BASE +  2;   // 参数更新错误
const int E_FATAL_ERROR             = E_RATE_BASE +  3;   // 账单更新失败等致命错误

const int E_GET_USER_TP             = E_RATE_BASE + 11;   // 取用户套餐失败
const int E_NOTFOUND_PROCRULE       = E_RATE_BASE + 12;   // 找不到处理规则
const int E_BILLMGR_INIT			= E_RATE_BASE + 13;   // 帐单初始化错误
const int E_CDR_CYCLE               = E_RATE_BASE + 14;   // 话单帐期信息错误
const int E_GET_RATEINFO            = E_RATE_BASE + 15;   // 取批价通道状态信息错误
const int E_PARAM_INI				= E_RATE_BASE + 18;   // 参数s初始化错误
const int E_BILL_CYCLE				= E_RATE_BASE + 19;   // 帐单周期错误
const int E_INVALID_FILE 			= E_RATE_BASE + 20;   // 文件无效、错误
                                
const int E_USER_CELLINFO           = E_RATE_BASE + 21;   // 用户小区资料错误
const int E_USER_SPECIALPHONE       = E_RATE_BASE + 22;   // 用户特殊号码资料错误
const int E_USER_SPECIALVPN         = E_RATE_BASE + 23;   // 用户特殊VPN资料错误
const int E_USER_TPPARAM            = E_RATE_BASE + 24;   // 用户级资费参数没有实例化

const int E_GET_ACCTDBILL           = E_RATE_BASE + 25;   // 取帐务明细账单参数错误
const int E_DATETIME_LONG           = E_RATE_BASE + 26;   // 时间跨度超长
const int E_FEEPOLICY_PARAM         = E_RATE_BASE + 27;   // 资费参数错误
const int E_RATE_FEEPOLICY_PARAM    = E_RATE_BASE + 28;   // 批价资费参数错误
const int E_PRICEPARAM1             = E_RATE_BASE + 29;   // 批价政策未定义
const int E_PRICEPARAM2             = E_RATE_BASE + 30;   // 批价政策未定义
const int E_PRICECOMPPARAM          = E_RATE_BASE + 31;   // 批价作用或条件节点未定义
const int E_FEECOUNTPARAM           = E_RATE_BASE + 32;   // 费用计算错误
const int E_STDFEECOUNTPARAM        = E_RATE_BASE + 33;   // 基准资费错误
const int E_ADDITEM_CYCRULE         = E_RATE_BASE + 34;   // 累计规则周期未定义
const int E_CYCRULE_NOTSUPPORT      = E_RATE_BASE + 35;   // 周期类型不支持
const int E_CYCRULE_OFFSET          = E_RATE_BASE + 36;   // 周期偏移量信息未实例化(个性化)
const int E_CONDITION_PARAM         = E_RATE_BASE + 37;   // 条件参数错误

const int E_LOCKUSER                = E_RATE_BASE + 38;   // 用户加锁
const int E_GET_BILL                = E_RATE_BASE + 39;   // 取帐单失败。
const int E_GET_RATE_BILLITEM       = E_RATE_BASE + 40;   // 取计费明细账单失败。
const int E_UPDATE_BILL             = E_RATE_BASE + 41;   // 更新账单失败

const int E_GPRS_CONTEXTINFO        = E_RATE_BASE + 50;   // GPRS内容计费字段信息错误
const int E_GET_BASETP              = E_RATE_BASE + 51;   // GPRS求取基本政策
const int E_GET_NETUSERTP           = E_RATE_BASE + 52;   // 宽带用户获取套餐错误
const int E_CDR_PRICE               = E_RATE_BASE + 99;   // 批价错误

//以下定义程序执行错误编码常量
const int E_ACCT_DEFAULT  			=  0              ;
const int E_ACCT_BASE               =  1000            ;
const int E_ACCT_INITIALIZE         =  E_ACCT_BASE +  1;   // 初始化错误
const int E_ACCT_PARAM_UPDATING     =  E_ACCT_BASE +  2;   // 参数更新错误
const int E_ACCT_CREATE_OBJECT      =  E_ACCT_BASE +  3;   // 创建对象错误
const int E_ACCT_ITEM               =  E_ACCT_BASE +  4;   // 帐目错误
                                   
const int E_ACCT_USERID_NULL        =  E_ACCT_BASE + 10;   // 用户标识为空错误
const int E_ACCT_GET_MINREALCYC     =  E_ACCT_BASE + 11;   // 获取最小实时帐期错误
const int E_ACCT_GET_USERINFO       =  E_ACCT_BASE + 12;     //取用户资料错误
const int E_ACCT_GET_USERSERV       =  E_ACCT_BASE + 13;     //取用户状态错误
const int E_ACCT_GET_VPNINFO        =  E_ACCT_BASE + 14;     //取用户群资料错误
const int E_ACCT_GET_ACCTINFO       =  E_ACCT_BASE + 15;     //取账户资料错误
const int E_ACCT_GET_USERVPN        =  E_ACCT_BASE + 16;     //取用户群错误
const int E_ACCT_GET_USERMEM        =  E_ACCT_BASE + 17;     //取用户群成员错误
const int E_ACCT_GET_ACCTID         =  E_ACCT_BASE + 18;     //取账户标识错误

const int E_ACCT_GET_BILLBAK        =  E_ACCT_BASE + 20;     //取明细账单镜像错误
const int E_ACCT_GET_SUMBILL        =  E_ACCT_BASE + 21;     //取累计量错误
const int E_ACCT_GET_CYCPARAM       =  E_ACCT_BASE + 22;     //取帐期参数错误     
const int E_ACCT_GET_BILL           =  E_ACCT_BASE + 23;     //取账单错误
const int E_ACCT_UPDATE_BILL        =  E_ACCT_BASE + 24;      //更新账单错误
    
const int E_ACCT_GET_BILLSWAP       =  E_ACCT_BASE + 30;     //取用户间费用转移错误
const int E_ACCT_GET_PAYRELA        =  E_ACCT_BASE + 31;     //取付费关系错误
const int E_ACCT_GET_PAYLIMIT       =  E_ACCT_BASE + 32;     //取付费限额错误
const int E_ACCT_PAY_DETAIL         =  E_ACCT_BASE + 33;     //根据付费帐目获取明细帐目错误
const int E_ACCT_DEAL_PAYRELA       =  E_ACCT_BASE + 34;     //付费关系绑定错误

const int E_ACCT_GET_ADJUSTFEE      =  E_ACCT_BASE + 40;     //获取帐前调整费用错误
const int E_ACCT_GET_ADJUSTMODE     =  E_ACCT_BASE + 41;     //帐前调整方式错误
const int E_ACCT_DEAL_ADJUSTBEFORE  =  E_ACCT_BASE + 42;     //处理帐前调帐错误   
const int E_ACCT_GET_ADJUSTREASON   =  E_ACCT_BASE + 43;     //获取帐期调整原因错误
const int E_ACCT_GET_OBJECTID       =  E_ACCT_BASE + 44;     //根据对象标识获取对象错误
const int E_ACCT_OBJECT_ADJUST      =  E_ACCT_BASE + 45;     //对象表和调帐表配置不一致错误
const int E_ACCT_GET_ADDUPOBJECT    =  E_ACCT_BASE + 46;     //获取累计对象错误
const int E_ACCT_GET_ACCTTP         =  E_ACCT_BASE + 47;     //取账户资费错误
const int E_ACCT_GET_ACCTSYSTP      =  E_ACCT_BASE + 48;     //取账户系统资费错误
const int E_ACCT_GET_ADJUSTBEFORE   =  E_ACCT_BASE + 49;     //取帐前调整信息错误
const int E_ACCT_GET_GROUPTP        =  E_ACCT_BASE + 50;     //取群资费错误

 
const int E_ACCT_TP_DISCNT          =  E_ACCT_BASE + 51;     //资费在优惠表中未定义错误
const int E_ACCT_TP_COMP            =  E_ACCT_BASE + 52;     //资费政策在资费构成表中无定义错误
const int E_ACCT_TP_PARAM           =  E_ACCT_BASE + 53;     //资费参数没有实例化错误
const int E_ACCT_USERTP             =  E_ACCT_BASE + 54;     //用户资费实例既不属于用户也不属于群组
const int E_USER_DISCNT_POLICY      =  E_ACCT_BASE + 55;     //处理用户优惠资费错误
const int E_USER_DISCNT_PARAM       =  E_ACCT_BASE + 56;     //处理用户优惠资料错误 
const int E_ACCT_DISCNT             =  E_ACCT_BASE + 57;     //处理账户优惠错误 
const int E_ACCT_DISCNT_INVAID_COND =  E_ACCT_BASE + 58;     //取优惠失效条件错误 

//固定费批价错误类型
const int E_RENTFEE_DEFAULT                    = E_DEFAULT + 2000; //从2000开始编码
const int E_RENTFEE_GETUSERINFO_ERR            = E_RENTFEE_DEFAULT + 1; //
const int E_RENTFEE_GETCYCINFO_ERR             = E_RENTFEE_DEFAULT + 2; //
const int E_RENTFEE_GETPOLICY_ERR1             = E_RENTFEE_DEFAULT + 3; //
const int E_RENTFEE_GETPOLICY_ERR2             = E_RENTFEE_DEFAULT + 4; //
const int E_RENTFEE_GETPOLICY_ERR3             = E_RENTFEE_DEFAULT + 5; //
const int E_RENTFEE_GET_USERSERV               = E_RENTFEE_DEFAULT + 6; //
const int E_RENTFEE_GET_USERSERVSTATE          = E_RENTFEE_DEFAULT + 7; //
const int E_RENTFEE_GET_STARTOFSERV            = E_RENTFEE_DEFAULT + 8; //
const int E_RENTFEE_GET_USERSERVSTATE2         = E_RENTFEE_DEFAULT + 9; //
const int E_RENTFEE_GET_GETEVENTFROMITEM       = E_RENTFEE_DEFAULT + 10; //
const int E_RENTFEE_GET_HASUSERSERV            = E_RENTFEE_DEFAULT + 11; //

//add for rent new version begin
const int E_RENTFEE_GET_SERVSTATEPRIOR         = E_RENTFEE_DEFAULT + 12;
const int E_RENTFEE_CALCSUBFEE_ERR             = E_RENTFEE_DEFAULT + 13;
//add for rent new version end
#endif // __RATEERROR__

