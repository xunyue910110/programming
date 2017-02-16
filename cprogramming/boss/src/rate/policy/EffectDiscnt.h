#ifndef _EFFECTDISCNT_H_
#define _EFFECTDISCNT_H_

#include "EffectBase.h"
#include "CompositItemMgr.h"
#include "RateGlobalInfo.h"

class ObjectBase;
class ObjectBill;
class EffectAddup;
class DetailBill;

//费用计算方法
///////added by liugj 20090429 for NX Unicom 之所以放在-1，是因为EffectDiscnt.cpp里updateDiscountFee函数有对if (m_computeMethod < COMP_METHOD_FEE_DIVERT)的判断，所以不能大于COMP_METHOD_FEE_DIVERT
const int COMP_METHOD_SVC          = -1; //依据服务进行收费（需要参考当月已有服务的开始时间） 
////////////////////////////////////////////////////////////////////////////////////////////////
const int COMP_METHOD_COMMON        = 0; //通用（设定/按比例减免或补收/减免或补收固定值）
const int COMP_METHOD_LIMIT_UP      = 1; //上限 or 超过参考值的部分进行打折(补收)
const int COMP_METHOD_LIMIT_DOWN    = 2; //下限
const int COMP_METHOD_DERATE_LIMIT  = 3; //按比例减免或补收但不能超过限定的值
const int COMP_METHOD_DERATE_XY     = 4; //每打X元,减免y元
const int COMP_METHOD_FEE_DIVERT    = 5; //费用转移
const int COMP_METHOD_PRESENT       = 6; //费用赠送到帐户存折
const int COMP_METHOD_PRESENT_LIMIT = 7; //按比例赠送但不能超过指定的值
//added by liugj 20090506
const int COMP_METHOD_SVC_ORDER     = 8; //按当月订购关系的收费  同步广东、湖北功能代码
//add by yuxk begin 2009-9-27 20:48:40
const int COMP_METHOD_ADDUP     = 9; //包使用量套餐退订时 需要根据未使用总量比例进行退费
//add by yuxk end 2009-9-27 20:48:40

//费用调整方法
const char FEE_ADJUST_COMMON       = '0'; //不调整
const char FEE_ADJUST_DAY_MONTH    = '1'; //实际作用值为(作用值×当月实际天数)/全月天数
const char FEE_ADJUST_USER_NEW     = '2'; //实际作用值为(作用值×开户日期起实际天数)/全月天数(针对新开户用户)
const char FEE_ADJUST_USER_STOP    = '3'; //实际作用值为(作用值×月初到最后停机时间的实际天数)/全月天数(针对停机用户)
const char FEE_ADJUST_USER_NEWSTOP = '4'; //实际作用值为(作用值×开户日期起到最后停机时间的实际天数)/全月天数(针对新开户停机用户)
const char FEE_ADJUST_DAY_USERED   = '5'; //套餐实际生效天数
const char FEE_ADJUST_DAY_USERED_2 = '6'; //申请和取消月按实际生效天数收
//added by liugj for Tianj Unicom
const char FEE_ADJUST_USER_NEW_2   = '8'; //实际作用值为(作用值×开户日期起实际天数)/全月天数(针对新开户用户 跟'2'的区别是，'2'按天分摊收取，'8'一次性收取，月末情况一样

//added by taijq begin 2009-12-13
const char FEE_ADJUST_HOLIDAY   = '7'; //山西假期包年
const char FEE_ADJUST_YEAR_MONTH   = '9'; //包年按月收取，首月不满月按天收取，资费结束月满年规整不满年按天
//added by taijq end 2009-12-13

const char FEE_ADJUST_OPEN_DESTROY = 'a'; //实际作用值为(作用值×[开户日期-销户日期]起实际天数)/全月天数
const char FEE_ADJUST_OPEN_FACTDAY = 'x'; //实际作用值为(作用值×[开户日期-月末]起开通(0NAB57)的实际天数)/全月天数
const char FEE_ADJUST_OPEN_FACTDAY1 = 'y';//实际作用值为(作用值×[开户日期-月末]起开通(0NAB)的实际天数)/全月天数
//added by liugj 20090429 for NX Unicom，根据服务开始时间做费用调整（前面计算方式必须为COMP_METHOD_SVC -1，以便配合使用），服务编码填写在baseFee字段上
const char FEE_ADJUST_SVC_STARTEND = 's'; 
const char FEE_ADJUST_SVC_STARTEND2= 't'; 
//add by yuxk begin 2009-9-27 20:48:40
const char FEE_ADJUST_STOPOPEN = 'b'; //实际作用值为(作用值×[复话时间-月末]天数)/全月天数(一次性计算)
const char FEE_ADJUST_OPEN_MONTHEND = 'c'; //实际作用值为(作用值×[开户--月末]天数)/全月天数(一次性计算)
const char FEE_ADJUST_USER_DESTROY = 'd'; //收全月租，拆机时按【拆机时间--月末】天数进行退费
const char FEE_ADJUST_NON_PACKAGE = 'e'; //宽带非包使用量套餐(宽带当月套餐变更：先按旧套餐按天退费，再按新套餐按天收费。新装、停保复话用户按天计算套餐费。拆机时按天退套餐费，但不退包使用量用户的套餐费。一次性收取)
//add by yuxk end 2009-9-27 20:48:40
//add by wanglu 2011-2-15 begin
const char FEE_ADJUST_STOP_RENTFEE = 'f';//日租用户当月欠费停机,要加收3天的日租;如果当月缴费复机,对停机期间的日租要补收
const char FEE_ADJUST_BROADBAND_DEPOSIT_MONEY = 'g';//开始月按照(使用天数/当月实际天数)*(预存金额/使用月数)
//end 2011-2-15

//费用分配方法
const char DISPATCH_USERAVE_ITEMAVE     = '0'; //按“用户均摊 + 账目均摊”
const char DISPATCH_USERAVE_ITEMPRIOR   = '1'; //按“用户均摊 + 账目优先”
const char DISPATCH_USERPRIOR_ITEMAVE   = '2'; //按“用户优先 + 账目均摊”
const char DISPATCH_USERPRIOR_ITEMPRIOR = '3'; //按“用户优先 + 账目优先”
const char DISPATCH_USERFIRST           = '4'; //按用户优先级最高的作用
const char DISPATCH_ZEROUSER						= '5'; //作用在user_id=0的用户上
const char DISPATCH_DUMMYUSER						= '6'; //作用在实际的虚拟用户上

//add by cuiyw begin for shanxi At 2009-12-12 18:20:00 
const char DISPATCH_BUTNEGATIVE          = '7';//作用:优惠不参考实际费用是否小于优惠费用
//add by cuiye end

//added by dingr begin
const char DISPATCH_LEAST_FIXUSER         = '8';//按一定规则作用在固话用户上
const char DISPATCH_LEAST_MOBILEUSER          = '9';//按一定规则作用在手机用户上
//added by dingr end

//剩余费用处理方法
//const char NEGATIVE_BILL_NO     = '0'; //优惠作用结果不可以为负
const char NEGATIVE_BILL_REMAIN = '1'; //顺延到下账期
//const char NEGATIVE_BILL_YES    = '2'; //优惠作用结果可以为负

class EffectDiscnt :public EffectBase
{
public:
	EffectDiscnt(){};
	~EffectDiscnt(){};
	virtual bool execute();
	virtual void dump(char *);

	bool operator < (const EffectDiscnt &right) const
	{
		if(m_id==right.m_id)
			return (m_orderNo < right.m_orderNo);
		else
			return (m_id < right.m_id);
	}

	bool operator == (const EffectDiscnt &right) const
	{
		return (m_id==right.m_id);
	}

   //added by liugj 广东代码同步
	/**
	* add by geyf
	* getTpTime: 获取用户优惠政策到目前为止的时间生效情况.
	* @param  beginDateTime：开始时间.
	* @param	endDateTime：结束时间.
	* @return int数字,表示本月的优惠有效分布情况.
	*/ 		
	int getTpTime(const T_DATE_TIME  &beginDateTime,const T_DATE_TIME  &endDateTime);                     

protected:
    //获取应该优惠的费用
	void getDiscountFee(long &fee);
	
    //实例化参数值
	void getParamValue(long &childValue,long &parentValue,
				       long &baseFee,   long &discntFee);

	void dealComputeMethod(long &fee);
	void dealAdjustMethod(char &adjustType,long &fee);	
	void addRemainFee(long &fee);

	long updateDiscountFee(long &fee);
	long updateEffectItemFee(long &fee);
	void updateComputeItemFee(long &effectFee);
	
	//根据作用时间计算费用
	void computeFeeByDay(const T_DATE_TIME  &beginDateTime,//起始时间
	                     const T_DATE_TIME  &endDateTime,  //作用结束时间
	                     long &fee);

	/**
	* add by geyf
	* computeFee: 计算用户的固定费用.
	* @param  recvnum: 生效情况的二进制串.
	* @param  childValue: 收取周期类型 1:按天 2:按半月收取 3:整月收取 4:超过15天按全月收取,小于等于15天按半月收取
	* @param  parentValue: 1000:直接作用 1001:移动模式 1002:联通模式
	* @param  discntFee: 计费周期收取的费用
	* @return int数字,表示本月的优惠有效分布情况.
	*/
	int computedFee(const int recvnum,const int discntnum,const long childValue,const long parentValue,const long discntFee,const T_DATE_TIME curDateTime);

	void getOtherParamValue(long& sumToIntFee,int &sumToIntMode,int &displayMode);
	//end of addition
	//added by taijq begin 2009-12-13
	bool getAddupInfo(const long &r_userTpId, const long &r_beginTime, const int &r_addupItem, long &r_addupValue);
	void updateAddupInfo(const long &r_userTpId,const int &r_addupItem, const long &r_beginTime,const long &r_endTime,const long &r_effectFee);
	int getValidTpDays();
	//added by taijq end 2009-12-13
	// added by houyq 2010-03-18 start
	int getValidTpDays(const T_DATE_TIME& judgeTpEndTime);
	// added by houyq 2010-03-18 end

public:		
	char m_adjustMethod;     //优惠值调整方法
	char m_baseAdjustMethod; //优惠基准值调整方法
	char m_dispatchMethod;   //费用分配方法
	char m_remainDealMethod; //剩余费用处理方法
	char m_displayMethod;    //优惠体现方式: 0:体现帐务优惠 1:不体现帐务优惠
	char m_childValue[MIN_STR_LENGTH+1];
	char m_parentValue[MIN_STR_LENGTH+1];
	char m_discntFee[MIN_STR_LENGTH+1];
	char m_baseFee[MIN_STR_LENGTH+1];
	short m_orderNo;
	int  m_computeMethod;    //费用计算方法
	ObjectBase   *m_computeObj;
	ObjectBase   *m_parentObj;//获取分母的对象
	ObjectBase   *m_baseObj;
	ObjectBill   *m_effectObj;
	EffectAddup  *m_addupRule;
	EffectDiscnt *m_next;
	stateMap  m_SvcState;
};

#endif