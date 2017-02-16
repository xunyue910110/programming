#ifndef _OBJECTBILL_H_
#define _OBJECTBILL_H_

#include "ObjectBase.h"

const char ATTR_SOURCE_FACT         = '0'; //实际值
const char ATTR_SOURCE_CUR          = '1'; //相对原始值 
const char ATTR_SOURCE_ORIGIN       = '2'; //绝对原始值
const char ATTR_SOURCE_FACT_BUTBFEE = '3'; //实际值,但不包含调整补收的费用
const char ATTR_SOURCE_ITEM_NUM     = '4'; //明细帐目数量
const char ATTR_SOURCE_BIT_NUM      = '5'; //按bit位来统计数量
const char ATTR_SOURCE_BILLUSER     = '6'; //从bill_user_x获取费用，主要是用于参考不下账的费用
//added by wanglu 2011-2-24
const char ATTR_SOURCE_ACCTDEPOSIT  = '7'; //来源于用户存折金额
const char ATTR_SOURCE_ORG_FACT     = '8';//优惠前和优惠后费用之和add by xuzy at 2011.2.7


class ObjectBill :public ObjectBase
{
public:
	ObjectBill();
	virtual ~ObjectBill();
	
	bool operator == (const ObjectBill &right) const
    {
        return (m_id == right.m_id);
    }

    bool operator < (const ObjectBill &right) const
    {
        return (m_id < right.m_id);       
    }
public:
	virtual bool getObjectValue(T_OBJECT_ATTR &value);
	virtual bool getObjectValue(long &value);
	virtual void dump(char *);
	
	void getFactValue(long &value);

	//根据角色获取群内指定用户ID集合
	void geteffectUsers();
	

	//按“用户平摊+账目平摊”方式处理优惠值
	long update_userAveItemAve(long &effectFee,
                               const char &discountType,
                               const char &ifNotShowNegative,
                               const char &ifNegativeBill);
                         
	//按“用户平摊+账目优先”方式处理优惠值
	long update_userAveItemPrior(long &effectFee,
                                 const char &discountType,
                                 const char &ifNotShowNegative,
                                 const char &ifNegativeBill);
                           
    //按“用户优先+账目平摊”方式处理优惠值
	long update_userPriorItemAve(long &effectFee,
                                 const char &discountType,
                                 const char &ifNotShowNegative,
                                 const char &ifNegativeBill);
                         
	//按“用户优先+账目优先”方式处理优惠值
	long update_userPriorItemPrior(long &effectFee,
                                   const char &discountType,
                                   const char &ifNotShowNegative,
                                   const char &ifNegativeBill); 
           
    //按用户优先级最高的作用                                   
	long update_fisrtUser(long &effectFee,
                          const char &discountType,
                          const char &ifNotShowNegative,
                          const char &ifNegativeBill); 

   //指定用户作用                                   
	long update_setUser(const T_USER_ID &userId,
													long &effectFee,
                          const char &discountType,
                          const char &ifNotShowNegative,
                          const char &ifNegativeBill); 

   /*作用:优惠不参考实际费用是否小于优惠费用*/ 
   long  update_userButNEGative(long &effectFee,
                                       const char &discountType, 
                                       const char &ifNotShowNegative, 
                                       const char &ifNegativeBill);
                          					                            
	//更新赠款信息
     void updatePresentMoney(const long &insObjId, const long &value); 
     
     //更新当前费用
     void updateCurFee();                          

	 void splitEnumValue(const char* pEnumValue,
	                    T_MEMBER_TYPE &value, 
	                    vector<T_MEMBER_TYPE> &vValue,
	                    map<T_MEMBER_TYPE, T_MEMBER_TYPE> &mapValue);	
private:
	void getValue(long &value);
	
	//获取群下其他角色用户的费用值
	void getRoleBillValue(T_SERVER_ID &groupId, long &value,ID_TYPE r_idType=USER_ID_TYPE);
	
	//根据角色获取群内指定用户ID集合
	void getRoleUsers(T_SERVER_ID &groupId);
	//根据条件ID获取账户下的用户ID集合
    void getUsers(T_SERVER_ID &acctId);	
	
public:		
	char   m_attrSource; //属性值来源	
	int    m_condId; //条件ID
	char   m_itemType;   //账目类型
	static  RateGlobalInfo *m_pRateInfo;                         //批价全局信息
};

#endif
