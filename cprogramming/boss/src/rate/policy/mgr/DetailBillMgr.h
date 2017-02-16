#ifndef _DETAILBILLMGR_H_
#define _DETAILBILLMGR_H_

#include <vector>
#include <set>

#include "CompositItemMgr.h"
#include "BillStruct.h"

class RateGlobalInfo;

//优惠类型
const char DISCNT_TYPE_ACCT_DERATE           = '1'; //帐务优惠（减免）
const char DISCNT_TYPE_ACCT_RECEIVE          = '2'; //帐务优惠（补收）
const char DISCNT_TYPE_ADJUST_ASACCT_DERATE  = '3'; //帐前调整体现为优惠（减免）
const char DISCNT_TYPE_ADJUST_ASACCT_RECEIVE = '4'; //帐前调整体现为优惠（补收）
const char DISCNT_TYPE_ADJUST_DERATE         = '5'; //帐前调整（减免）
const char DISCNT_TYPE_ADJUST_RECEIVE        = '6'; //帐前调整（补收）

//优惠作用结果是否可以为负
const char NEGATIVE_BILL_NO     = '0'; //优惠作用结果不可以为负
//const char NEGATIVE_BILL_REMAIN = '1'; //顺延到下账期
const char NEGATIVE_BILL_YES    = '2'; //优惠作用结果可以为负


//定义明细帐目费用信息.
class DetailItemFee
{
public:
    int      m_detailItemCode;//明细账目
	long     m_ofee;          //原始费用
    long     m_nfee;          //实际费用	
    long     m_afee;          //优惠的费用
    long     m_bfee;          //调整的费用    
    long     m_cfee;          //上一个优惠作用后的费用	
    long     m_dfee;          //账户级调帐费用
	long     m_gfee;					//用户群+用户级优惠	add for acctdiscnt 2009-9-11 2:40:07
	long     m_ufee;          //用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
public:
 	DetailItemFee ():m_detailItemCode(0),m_ofee(0),m_nfee(0),m_afee(0),m_bfee(0),m_cfee(0),m_dfee(0),m_gfee(0),m_ufee(0) //add for acctdiscnt 2009-9-11 2:40:45
	{
	}
		
	DetailItemFee(const long &right ):m_detailItemCode(0)
	{
		m_ofee           = right;
		m_nfee           = right;
		m_afee           = right;
		m_bfee           = right;
		m_cfee           = right;
		m_dfee           = right;
		m_gfee           = right; //add for acctdiscnt 2009-9-11 2:40:02
		m_ufee           = right; //用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	}
  	
  	DetailItemFee(const DetailItemFee &right )
	{
	    m_detailItemCode = right.m_detailItemCode;
		m_ofee           = right.m_ofee;
  		m_nfee           = right.m_nfee;
  		m_afee           = right.m_afee;
  		m_bfee           = right.m_bfee;
  		m_cfee           = right.m_cfee;
  		m_dfee           = right.m_dfee;
		m_gfee           = right.m_gfee; //add for acctdiscnt 2009-9-11 2:41:22
		m_ufee           = right.m_ufee;//用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	}
  	
	friend bool operator<(const DetailItemFee &left,const DetailItemFee& right)
	{
		return (left.m_detailItemCode < right.m_detailItemCode);
	}
    
    friend bool operator==(const DetailItemFee &left,const DetailItemFee& right)
    {
		return (left.m_detailItemCode == right.m_detailItemCode);	
    }
    DetailItemFee& operator=(const long& right);
    DetailItemFee& operator=(const DetailItemFee& right);
    DetailItemFee& operator +=(const DetailItemFee &right);
    DetailItemFee& operator -=(const DetailItemFee &right);	
    friend ostream & operator << (ostream &os ,const  DetailItemFee& po);
}; 



//定义用户的明细帐单信息.
class UserDetailBill
{
    public:
        UserDetailBill() {}
        ~UserDetailBill() {m_feeVector.clear();}
        
        bool operator < (const UserDetailBill &right) const
        {
            if (m_priority == right.m_priority)
                return (m_subId < right.m_subId); 
            else
                return (m_priority > right.m_priority);
        }         
     friend ostream & operator << (ostream &os , UserDetailBill& po);
    
    friend bool operator==(const UserDetailBill &left,const UserDetailBill& right);
    
    public:
		T_USER_ID                m_subId;      //用户ID    
        int                      m_priority;   //用户在群或账户中的优先级
        vector<DetailItemFee>    m_feeVector;  //用户明细费用
    public:
        //内部数据,存储本次查询的结果,供查询下次使用        
        int m_detailItemNum;//临时存储此组合账目中包含的明细账目个数
        int m_itemCode;     //临时存储组合账目值
        long m_sumFee;      //临时存储某个组合账目的费用值
};

//明细帐单格式(用户/群/客户/账户)
class DetailBill
{
public:
	ID_TYPE                     m_idType;          //0:user 1:group 2:customer 3:account
	T_SERVER_ID                 m_id;              //user_id OR group_id OR customer_id OR acct_id
	vector<UserDetailBill>      m_detailBillVector;//m_id 下的多个用户的用户明细账单
	
	const DetailBill& operator=(const DetailBill& right);
	DetailBill& operator +=(const DetailBill &right);
	friend bool operator==(const DetailBill &left,const DetailBill& right);
	friend bool operator<(const DetailBill &left,const DetailBill& right);
  friend ostream & operator << (ostream &os , DetailBill& po);
  
  int usernum();
};

/*
//定义优惠明细帐单格式（优惠轨迹）
class DiscntBill; //BillStruct.h
*/

//定义优惠明细帐单格式
class ItemBill
{
public:
	DetailItem m_detailItem; //明细账目结构
	vector<DetailItemFee>::iterator m_iter;   //明细账目费用对应的指针    	
};

class PresentMoney;

//明细账单管理器
class DetailBillMgr
{
public:
	DetailBillMgr(){ m_userIdsUnionTag=0;}

	~DetailBillMgr(){}
	bool initialize(CompositItemMgr *pItemMgr,
	                vector<DiscntBill> *discntBillVector,
	                vector<AdditionalBill>  *addBillVector)
	{
		m_compositItemMgr = pItemMgr;
		m_discntBillVector = discntBillVector;	
		m_addBillVector = addBillVector;
		return true;
	}

	void setUserDetailBill(DetailBill *detailBill)
	{
		m_detailBill = detailBill;
		m_discntBill.m_id = detailBill->m_id;
		m_discntBill.m_idType = detailBill->m_idType;	
	}
	
	void setDiscntInfo(T_TP_ID &discntId, long &adjustInsId, int &orderNo)
	{		
		m_discntBill.m_discntId = discntId;
		m_discntBill.m_orderNo = orderNo;
		m_discntBill.m_adjustInsId = adjustInsId;
		m_discntBill.m_effectId = 0; //优惠作用标识 add by xuf 2010-1-23 14:24:01
	}
	
	ID_TYPE getIdType() 
	{
		return m_discntBill.m_idType;
	}
	
	int usernum()
	{
		return	m_detailBill->usernum();
	}
	
//优惠作用标识 add by xuf 2010-1-23 14:24:01
	void setEffectId(T_RULE_ID &r_effectId)
	{
		m_discntBill.m_effectId = r_effectId;
	}

	
public:
    //获取账目实际费用
	void getFactFee(int &itemCode,char &itemType,long &fee);
	//获取账目当前费用(上一个优惠作用后的费用)
	void getCurFee(int &itemCode,char &itemType,long &fee);
	//获取账目原始费用(优惠作用前的费用)
	void getOriginFee(int &itemCode,char &itemType,long &fee);
	//获取账目实际费用，但不包括调整补收的费用
	void getFactFeeButBfee(int &itemCode,char &itemType,long &fee);
	
	//desc:           
    //     获取账目的实际费用及账目个数
    //param:           
    //     itemCode :明细账目或组合账目
    //返回值:           
    //     fee :账目总费用
    //说明:
	//调用此函数主要是为了避免对相同的账目进行多次重复查找
    //在查找账目总费用的时候，将每个用户下的账目费用和账目数保存，
    //在其他需要使用账目费用的地方，就不需要再查了；
    //将每个用户下账目费用累加，得到账目总费用
    //当 itemCode == (*m_billIter).m_itemCode ,说明当前用户下
    //账目费用和账目数已经查好了,可以直接使用,否则需要重新查询
    //约定:在每次更新用户时,令 (*m_billIter).m_itemCode=0,
    //这样下次查询时将重新查询帐目费用
    
    void getFactFee(const int &itemCode, long &fee);    
    //获取账单优惠值
	void getDiscntFee(const int &itemCode, long &fee);
	//获取账目原始费用(优惠作用前的费用)
	void getOriginFee(const int &itemCode,long &fee);
	//获取帐目数量
	//对用户有意义,num 代表确切的帐目数
    //对于群或账户,num>0,代表有此账目，num==0,代码无账目，
	void getItemNum(const int &itemCode, long &num); 
	
	void getFactFee(DetailBill &roleBill, const int &itemCode, long &fee);    
	void getOriginFee(DetailBill &roleBill, const int &itemCode, long &fee); 	
	void getItemNum(DetailBill &roleBill, const int &itemCode, long &num); 
   void getCurFee(DetailBill &roleBill, const int &itemCode, long &fee);//add by xuf 2010-2-1 2:07:59  

	//更新账目费用
	
	//param:
	//itemCode         : 账目编码
	//effectFee        : 优惠的费用
	//ifNotShowNegative: 优惠的值是否在账户账单中显示
	//discountType     : 优惠类型
	//ifNegativeBill   : 是否可以优惠为负值
	//detailItemMap    : 明细账目集合

	//按“用户平摊+账目平摊”方式处理优惠值
	long update_userAveItemAve(const int &itemCode,
                               long &effectFee,
                               const char &ifNotShowNegative,
                               const char &discountType,
                               const char &ifNegativeBill);
                         
	//按“用户平摊+账目优先”方式处理优惠值
	long update_userAveItemPrior(const int &itemCode,
                                 long &effectFee,
                                 const char &ifNotShowNegative,
                           		 const char &discountType,
                                 const char &ifNegativeBill);
                           
    //按“用户优先+账目平摊”方式处理优惠值
	long update_userPriorItemAve(const int &itemCode,
                                 long &effectFee,
                                 const char &ifNotShowNegative,
                                 const char &discountType,
                                 const char &ifNegativeBill);
                                 
	//按“用户优先+账目优先”方式处理优惠值
	long update_userPriorItemPrior(const int &itemCode,
                                   long &effectFee,
                                   const char &ifNotShowNegative,
                           		   const char &discountType,
                                   const char &ifNegativeBill); 
                                   
    //按用户优先级最高的作用                                   
	long update_fisrtUser(const int &itemCode,
                          long &effectFee,
                          const char &ifNotShowNegative,
                          const char &discountType,
                          const char &ifNegativeBill); 
                          
    //指定用户作用                                   
	long update_setUser(const T_USER_ID &userId,
													const int &itemCode,
                          long &effectFee,
                          const char &ifNotShowNegative,
                          const char &discountType,
                          const char &ifNegativeBill);                      
     //更新当前费用
     void updateCurFee();
     
     //更新赠款信息
     void updatePresentMoney(const int &depositCode, 
                 const long &insObjId, const long &value);

    //获取账目的整个实际费用
    void getFactFee(long &fee);
	  
private:
	//获取账目费用
	void getFactFeeComposit(IntMap &detailItemMap,long &fee);
	void getCurFeeComposit(IntMap &detailItemMap,long &fee);
	void getOriginFeeComposit(IntMap &detailItemMap,long &fee);
	void getFactFeeButBfeeComposit(IntMap &detailItemMap,long &fee);	

	void getFactFeeDetail(int &itemCode,long &fee);
	void getCurFeeDetail(int &itemCode,long &fee);
	void getOriginFeeDetail(int &itemCode,long &fee);
	void getFactFeeButBfeeDetail(int &itemCode,long &fee);	
	
	void getFactFeeAll(long &fee);
	void getCurFeeAll(long &fee);
	void getOriginFeeAll(long &fee);
	void getFactFeeButBfeeAll(long &fee);	

	//更新账目费用
	//将费用补收在第一个账目上(针对新增一个补收账目的情况)
	void updateSubIdFeeItemFirst(UserDetailBill &userDetailBill,
					            const long &effectFee,
                                const char &ifNotShowNegative, 
                                const char &discountType,
                                IntMap &detailItemMap);
                                
  //将费用作用在第一个账目上(针对新增一个作用账目的情况) add by  xuf 2010-2-6 17:41:56
	void updateSubIdFeeItemFirstSp(UserDetailBill &userDetailBill,
					            const long &effectFee,
                                const char &ifNotShowNegative, 
                                const char &discountType,
                                IntMap &detailItemMap);
                                
	//按账目平摊方式更新用户费用
	void updateSubIdFeeItemAve(UserDetailBill &userDetailBill,
							   const long &effectFee,							    
                               const char &ifNotShowNegative, 
                               const char &discountType,
                               const char &ifNegativeBill,
                               IntMap &detailItemMap);

	//按账目优先方式更新用户费用
	void updateSubIdFeeItemPrior(UserDetailBill &userDetailBill,
							   const long &effectFee,							    
                               const char &ifNotShowNegative, 
                               const char &discountType,
                               const char &ifNegativeBill,
                               IntMap &detailItemMap);
                
    //获取用户某账目的总费用和包括的明细账目数
    void getItemFactFee(UserDetailBill &detailBill,
                   const int &itemCode, const IntMap &detailItemMap);
    
   //获取用户某账目的上条资费优惠后总费用和包括的明细账目数
    void getItemCurFee(UserDetailBill &detailBill,
                   const int &itemCode, const IntMap &detailItemMap);
                   
    //获取账目的实际费用                   
    void getFactFee(const int &itemCode, 
                    const IntMap &detailItemMap, long &fee);
	         
	//更新明细账单
    void updateSubIdDetailitemFee(UserDetailBill &userDetailBill, 
                                  const int &detailItemCode, 
                                  const long &ofee,
                                  const long &nfee, 
                                  const long &afee,
                                  const long &bfee,
                                  const long &dfee);
                                  
	//更新优惠帐单
	void updateDiscntBill(const T_USER_ID &subId, 
                          const int &detailItemCode, 
                          const long &discountFee, 
                          const char &discountType);    


	//更新帐前调账日志
void updateAdustBeforeBill(const T_SERVER_ID &rId, 
                                   const ID_TYPE &rIdType, 
                                   const long &rInsId,
                                   const long &discountFee, 
                                   const char &discountType,
                                   const int    & detailItemCode);

private:	
    vector<DiscntBill> *m_discntBillVector;              //优惠账单(优惠轨迹)指针
    vector<AdditionalBill> *m_addBillVector;               //赠送现金指针
	DetailBill   *m_detailBill;                          //明细账单指针
	
	CompositItemMgr *m_compositItemMgr;                  //组合账目管理器指针	
	
	vector<DetailItemFee>::iterator m_feeIter;           //临时的明细费用指针
	vector<DetailItemFee>::const_iterator m_constFeeIter;//临时的明细费用指针
	vector<UserDetailBill>::iterator m_billIter;         //临时的用户明细账单指针
	
	DetailItemFee m_detailItemFee;                       //临时的明细费用
public:
     DiscntBill m_discntBill;                            //临时的优惠账单
     set<T_SERVER_ID>   m_userIds;                       //[根据角色或条件ID获取群或账户下的]用户集合
     set<T_SERVER_ID>   m_userIdsUnion;                       //[根据角色或条件ID获取群或账户下的]用户集合
     int m_userIdsUnionTag;  //

     static  RateGlobalInfo *m_pRateInfo;                         //批价全局信息

};

#endif
