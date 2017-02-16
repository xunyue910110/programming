#ifndef _FILTER_ERROR_CODE_H_20080728_
#define _FILTER_ERROR_CODE_H_20080728_

const int DEFAULT_ERROR           = 0;
const int DEFAULT_OFFICECODE      = 0;
const int INVALID_ERROR           = 700;

//致命错误基数
const int FILTER_ERROR_BASE       = 700;
const int FILTER_ERROR_INITIALIZE       = FILTER_ERROR_BASE + 1;   // 初始化错误
const int FILTER_ERROR_SEARCH_MDB       = FILTER_ERROR_BASE + 2;   // 内存数据库链接异常
const int E_GET_INPUT_PARAM             = FILTER_ERROR_BASE + 3;   //根据子通道名获取输入子配置错误
const int FILTER_ERROR_DBUPDATE         = FILTER_ERROR_BASE + 4;   //更新物理数据库失败
const int E_CDR_FIELD_NUMBER            = FILTER_ERROR_BASE + 5;   //话单格式字段个数错误
const int E_MOVE_FILE_ERR               = FILTER_ERROR_BASE + 6;   //移走话单失败
const int E_GET_DIVID_PARAM             = FILTER_ERROR_BASE + 7;   //根据话单内容获取排重配置错误
const int E_GET_CHANNEL_PARAM           = FILTER_ERROR_BASE + 8;   //根据话单内容获取分通道配置错误
const int E_OUT_CHANNEL_DEFINED         = FILTER_ERROR_BASE + 9;   //配置中没有定义输出通道路径

const int FILTER_ERROR_CDR              = 700;
//获取资料错误 内部使用
const int GET_ERROR_USERID              = FILTER_ERROR_CDR + 10; //获取用户标识错误
const int GET_ERROR_USERINFO            = FILTER_ERROR_CDR + 11; //获取用户信息错误

//话单级错误
const int GSM_ERROR_IMSI                = FILTER_ERROR_CDR + 12; //国际IMSI代码错
const int GSM_ERROR_SGSN                = FILTER_ERROR_CDR + 13; //GCDR话单SGSN地址错
const int GET_ERROR_CHANNEL             = FILTER_ERROR_CDR + 14; //根据用户USERID后四位和地市区号获取省内通道号失败
const int GET_ERROR_JUDGE_ID            = FILTER_ERROR_CDR + 15; //根据子条件ID获取简单条件表中的判断对象标识失败
const int GET_ERROR_OBJECT_ID           = FILTER_ERROR_CDR + 16; //根据简单条件表中判断对象标识获取对象定义表中的属性ID失败
const int GET_ERROR_ELEMENT_KEY         = FILTER_ERROR_CDR + 17; //根据对象定义表中的属性ID获取事件元素表中事件元素关键字(话单下标ID)失败
const int GET_ERROR_CDR_ID              = FILTER_ERROR_CDR + 18; //获取的话单下标超范围
const int GET_ERROR_JUDEGMETHOD         = FILTER_ERROR_CDR + 19; //简单条件表中判断方式超范围
const int GET_ERROR_STATEGROUP          = FILTER_ERROR_CDR + 20; //根据用户资料中的状态获取用户状态分组失败
const int GSM_ERROR_FORWARDMTC          = FILTER_ERROR_CDR + 21; //无条件呼转被叫错单
const int ERROR_INVALID_SERVSP          = FILTER_ERROR_CDR + 22; //增值业务无效话单（惩罚性质）
const int GET_ERROR_VALUEMETHOD         = FILTER_ERROR_CDR + 23; //简单条件表中取值方法不在定义范围内（VALUE_METHOD）
const int GET_ERROR_METHOD_COMPUTE      = FILTER_ERROR_CDR + 24; //简单条件表中数值运算定义错误（VALUE_PARAM）

const int SM_ERROR_PREPAY               = FILTER_ERROR_CDR + 30; //对于预开用户，下行话单且费用为0的，或者是信息费包月作错单处理
const int ERROR_PREPAY_CDR              = FILTER_ERROR_CDR + 31; //对于预开用户，非计费类别的话单作错单处理
const int ERROR_NOSTATUS                = FILTER_ERROR_CDR + 32; //用户状态库没有手机号
const int ERROR_STOPSTATUS_C            = FILTER_ERROR_CDR + 33; //停机 单停机 包月话单
const int ERROR_STOPSTATUS              = FILTER_ERROR_CDR + 34; //停机 单停机 按条话单
const int ERROR_DESTROYSTATUS           = FILTER_ERROR_CDR + 35; ///销户 预销户
const int GSM_ERROR_GZHEJ               = FILTER_ERROR_CDR + 36; ///江浙沪军网端局话单置错单
const int GSM_ERROR_FREE3S              = FILTER_ERROR_CDR + 37; ///本地主叫长途小于三秒置错单

const int SM_ERROR_PROT                 = FILTER_ERROR_CDR + 51; //无效的端口号
const int SM_ERROR_MSISDN               = FILTER_ERROR_CDR + 52; //无效的手机号
const int SM_ERROR_SPCODE               = FILTER_ERROR_CDR + 53; //无效的SP代码
const int SM_ERROR_SOURCE_TYPE          = FILTER_ERROR_CDR + 54; //无效的SOURCE_TYPE
const int SM_ERROR_SERVICE_CODE         = FILTER_ERROR_CDR + 55; //无效的SERVICE_CODE
const int SM_ERROR_OPERCODE             = FILTER_ERROR_CDR + 56; //无效的业务代码 行业网关话单
const int SM_ERROR_SERVICEOPER          = FILTER_ERROR_CDR + 57; //无效的服务代码和业务代码关系 行业网关话单
const int SM_ERROR_BIZSTATE             = FILTER_ERROR_CDR + 58; //集团业务订购状态错误 行业网关话单
const int SM_ERROR_SPM                  = FILTER_ERROR_CDR + 59; //部分业务包月话单在订购关系出，平台不应该产生的包月话单
const int SM_ERROR_INVALID_SP           = FILTER_ERROR_CDR + 60; //智能网无效SP（智能网的炫铃，要剔除）

const int SM_ERROR_SPTEST               = FILTER_ERROR_CDR + 63; //SP测试话单
const int SM_ERROR_SPOPER               = FILTER_ERROR_CDR + 64; //集团下发无sp资料话单
const int SM_ERROR_CHARGE_TYPE          = FILTER_ERROR_CDR + 65; //费用稽核类型错误
const int SM_ERROR_IFEE                 = FILTER_ERROR_CDR + 66; //信息费检验出错
const int SM_ERROR_MFEE                 = FILTER_ERROR_CDR + 67; //包月费检验出错
const int SM_ERROR_GFEE                 = FILTER_ERROR_CDR + 68; //赠送费检验出错
const int SM_ERROR_ALLFEE               = FILTER_ERROR_CDR + 69; //所有费用累加检验出错

const int SM_ERROR_ORDER1               = FILTER_ERROR_CDR + 70; //定购关系冻结，且不在本月变更
const int SM_ERROR_ORDER2               = FILTER_ERROR_CDR + 71; //定购关系退定，且本月变更
const int SM_ERROR_ORDER3               = FILTER_ERROR_CDR + 72; //没有对应的订购关系	
const int SM_ERROR_ORDER4               = FILTER_ERROR_CDR + 73; //定购关系预退定，且本月变更
const int SM_ERROR_ORDER5               = FILTER_ERROR_CDR + 74; //定购关系入库时间在上月20日24时后
const int SM_ERROR_ORDER6               = FILTER_ERROR_CDR + 75; //数据业务服务开关禁止


const int FILTER_ERROR_ATTRIB    = 800;
//AH: UserA homeAreaCode                
//UT: UserType                          
//AV: UserA visitAreaCode               
const int AH_IMSIN_ERROR                = FILTER_ERROR_ATTRIB +  1;  //根据国内imsi查A归属错误
const int AH_MSISDNN_ERROR              = FILTER_ERROR_ATTRIB +  2;  //根据国内号码查A归属错误
const int AH_IMSII_ERROR                = FILTER_ERROR_ATTRIB +  3;  //根据国际imsi查A归属错误
const int AH_UT_IMSIP_ERROR             = FILTER_ERROR_ATTRIB +  4;  //根据省内imsi设置A用户类型错
const int AH_UT_IMSIN_ERROR             = FILTER_ERROR_ATTRIB +  5;  //根据国内imsi设置A用户类型错
const int AH_UT_MSISDNP_ERROR           = FILTER_ERROR_ATTRIB +  6;  //根据省内号码设置A用户类型错
const int AH_UT_MSISDNN_ERROR           = FILTER_ERROR_ATTRIB +  7;  //根据国内号码设置A用户类型错
const int AH_UT_IMSII_ERROR             = FILTER_ERROR_ATTRIB +  8;  //根据国际imsi设置A用户类型错
const int AH_UT_POC_ERROR               = FILTER_ERROR_ATTRIB +  9;  //poc设置A用户类型错
const int AV_MSC_CELL_ERROR             = FILTER_ERROR_ATTRIB +  10; //根据msccell查A到访地错误
const int AV_CELL_ERROR                 = FILTER_ERROR_ATTRIB + 11;  //根据cell查A到访地错误
const int AV_LAC_ERROR                  = FILTER_ERROR_ATTRIB + 12;  //根据lac查A到访地错误
const int AV_MSC_LAC_ERROR              = FILTER_ERROR_ATTRIB + 13;  //根据msc查A到访地错误
const int AV_MSC_LAC_CELL_ERROR         = FILTER_ERROR_ATTRIB + 14;  //根据lac，cellid查A到访地错误
const int AV_LAC_CELLID_ERROR           = FILTER_ERROR_ATTRIB + 15;  //根据lac，cellid查A到访地错误
const int RO_AH_MSISDNP_ERROR           = FILTER_ERROR_ATTRIB + 16;  //出访根据省内号码设置A用户类型错
const int RO_AV_MSC_ERROR               = FILTER_ERROR_ATTRIB + 17;  //出访根据msc查A到访地错误
const int RO_AV_ASP_ERROR               = FILTER_ERROR_ATTRIB + 18;  //出访根据Asp查A到访地错误,国际出访不再根据MSC查到访，而是根据ASP
const int RO_AV_CITY_ERROR              = FILTER_ERROR_ATTRIB + 19;  //国内出访根据漫游所在城市代码查A到访地错误
//BH: UserB homeAreaCode                
//UT: UserType                          
//BV: UserB visitAreaCode               
const int BH_IMSIN_ERROR                = FILTER_ERROR_ATTRIB + 20;  //根据国内imsi查B归属错误
const int BH_M123_ERROR                 = FILTER_ERROR_ATTRIB + 21;  //根据m123查B归属错误
const int BV_M123_ERROR                 = FILTER_ERROR_ATTRIB + 22;  //根据m123查B到访地错误
const int BH_MSISDNO_ERROR              = FILTER_ERROR_ATTRIB + 23;  //根据其他号码段查B归属错误
const int BH_MSISDNP_ERROR              = FILTER_ERROR_ATTRIB + 24;  //根据省内号码段查B归属错误
const int BH_MSISDNN_ERROR              = FILTER_ERROR_ATTRIB + 25;  //根据国内号码段查B归属错误
const int BH_MSISDNI_ERROR              = FILTER_ERROR_ATTRIB + 26;  //根据国际号码段查B归属错误
const int BH_UNKOWN_ERROR               = FILTER_ERROR_ATTRIB + 27;  //B归属未知错误
const int BH_UT_M123_ERROR              = FILTER_ERROR_ATTRIB + 28;  //根据m123设置B归属用户类型错误
const int BH_UT_MSISDNP_IMSIN_ERROR     = FILTER_ERROR_ATTRIB + 29;  //根据国内，省内号码设置B归属用户类型错误
const int BH_UT_MSISDNO_ERROR           = FILTER_ERROR_ATTRIB + 30;  //根据对方号码设置B归属用户类型错误
const int BH_UT_MSISDNI_ERROR           = FILTER_ERROR_ATTRIB + 31;  //根据国际号码设置B归属用户类型错误
const int BH_UT_MSISDNN_ERROR           = FILTER_ERROR_ATTRIB + 32;  //根据国内号码设置B归属用户类型错误
const int BH_UT_SPHONE_ERROR            = FILTER_ERROR_ATTRIB + 33;  //根据特殊号码设置B归属用户类型错误
const int BH_IP_ERROR                   = FILTER_ERROR_ATTRIB + 34;  //IP号码错
const int CH_UT_SPHONE_ERROR            = FILTER_ERROR_ATTRIB + 35;  //根据特殊号码设置C归属用户类型错误
const int TRANS_NUMBER_LEN_OVER         = FILTER_ERROR_ATTRIB + 36;  //转接号码长度超长
const int GET_CITY_CODE_ERROR		        = FILTER_ERROR_ATTRIB + 37;  //城市代码错误
const int USER_ID_ERROR	 		            = FILTER_ERROR_ATTRIB + 38;  //SERVER ID 错误
const int FEE_AUDIT_ERROR               = FILTER_ERROR_ATTRIB + 39;  //费用审计错误
const int INVALID_GPRS_APNNI            = FILTER_ERROR_ATTRIB + 40;  //无效的GPRS APNNI地址
const int B_PHONENUMBER_ERROR_YXHD      = FILTER_ERROR_ATTRIB + 41;  //音信互动不在GROUP表
const int AV_LAC_MSC_ERROR			        = FILTER_ERROR_ATTRIB + 42;  //来访LACMSC非虚拟局不一致错误
const int OTHERPARTY_ERROR              = FILTER_ERROR_ATTRIB + 43;
const int GET_CITYCODE_ERROR            = FILTER_ERROR_ATTRIB + 44;  //根据区号查找省份代码城市代码错误
const int SPECIAL_ERROR_CODE1           = FILTER_ERROR_ATTRIB + 45;

const int AH_TRUNK_MSC_ERROR            = FILTER_ERROR_ATTRIB + 46;  //专网话单根据msc查交换机归属地错误
const int RO_AV_IMSI_ERROR              = FILTER_ERROR_ATTRIB + 47;  //国际出访根据漫游所在IMSI号查A到访地错误
const int IMSI2MSISDN_ERROR             = FILTER_ERROR_ATTRIB + 48;  //根据规则imsi转msisdn失败

//GPRS错误类型
const int GPRS_CONTEXTINFO_ERROR        = FILTER_ERROR_ATTRIB + 60; //GPRS内容计费信息错误
const int GPRS_GETAPNNITYPE_ERROR       = FILTER_ERROR_ATTRIB + 61; //GPRSAPNNI大类参数错误
const int AV_GPRS_SGSN_ERROR            = FILTER_ERROR_ATTRIB + 62; //根据SGSN查A到访地错误

const int AH_FIX_MSC_ERROR              = FILTER_ERROR_ATTRIB + 70;  //固网话单根据msc查交换机归属地错误
const int AH_FIX_ROAM_ERROR             = FILTER_ERROR_ATTRIB + 71;  //固网号码计费漫游话单判错
const int AH_FIX_MSISDN_ERROR           = FILTER_ERROR_ATTRIB + 72;  //根据固网号段查A归属错误
const int BH_FIX_MSISDN_ERROR           = FILTER_ERROR_ATTRIB + 73;  //根据固网号段查B归属错误
const int AH_GET_SECTION_ERROR          = FILTER_ERROR_ATTRIB + 74;  //根据A局向查A营业区错误
const int BH_GET_SECTION_ERROR          = FILTER_ERROR_ATTRIB + 75;  //根据B局向查B营业区错误

#endif //_FILTER_ERROR_CODE_H_20080728_                                             
