/**=!===============SEGMENG1==========================*/
#include "TfFCustomerDAO.h"

/**=~===============SEGMENG5==========================*/

/** @class TfFCustomer 
 *  @brief TF_F_CUSTOMER表的映射实体
 *  本对象映射数据库TF_F_CUSTOMER表的所有字段为数据成员，
 *  并对静态数据缓存字段的有意义解释映射进行封装。
 */
CustDom::TfFCustomer::TfFCustomer()
{
}

CustDom::TfFCustomer::~TfFCustomer()
{
}

void CustDom::TfFCustomer::init()
{
	PartitionId = -1;	
	CustId.clear();	
	CustName.clear();	
	CustType.clear();	
	CustState.clear();	
	PsptTypeCode.clear();	
	PsptId.clear();	
	ReopenTag.clear();	
	EparchyCode.clear();	
	CityCode.clear();	
	CustPasswd.clear();	
	ScoreValue.clear();	
	CreditClass = -1;	
	BasicCreditValue.clear();	
	CreditValue.clear();	
	RemoveTag.clear();	
	RemoveDate.clear();	
	DevelopDepartId.clear();	
	DevelopStaffId.clear();	
	IndepartId.clear();	
	InStaffId.clear();	
	InDate.clear();	
	Remark.clear();	
	Prevaluec1.clear();	
	Prevaluec2.clear();	
	Prevaluec3.clear();	
	Prevaluec4.clear();	
	Prevaluec5.clear();	
	Prevaluec6.clear();	
	Prevaluec7.clear();	
	Prevaluec8.clear();	
	Prevaluec9.clear();	
	Prevaluec10.clear();	
}

/** @class TfFCustomerDAO
 *  @brief TF_F_CUSTOMER表的数据访问接口
 *  本对象对数据库TF_F_CUSTOMER表的数据库访问进行封装，提供所有TF_F_CUSTOMER表的SQL操作
 *  方法，方便在业务组件中直接引用。
 */
CustDom::TfFCustomerDAO::TfFCustomerDAO(): MTable("TF_F_CUSTOMER")
{
	//
}

CustDom::TfFCustomerDAO::~TfFCustomerDAO ()
{
}

/** \fn CustDom::VPTfFCustomer CustDom::TfFCustomerDAO::jselect(const string &selBy, TfFCustomer &selTfFCustomer)
 *  查询方法，实现TF_F_CUSTOMER表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selTfFCustomer  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
CustDom::VPTfFCustomer CustDom::TfFCustomerDAO::jselect(const string &selBy, TfFCustomer &selTfFCustomer)
{
    CustDom::VPTfFCustomer  vpRetTfFCustomer; 

    if( selBy == SEL_IDX_TF_F_CUSTOMER_CITYCODE) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCITY_CODE", selTfFCustomer.CityCode);
    	SetParam(":VDEVELOP_DEPART_ID", selTfFCustomer.DevelopDepartId);
    } 	
    else if( selBy == SEL_IDX_TF_F_CUSTOMER_CUSTNAME) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCUST_NAME", selTfFCustomer.CustName);
    } 	
    else if( selBy == SEL_IDX_TF_F_CUSTOMER_PSPTID) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VPSPT_ID", selTfFCustomer.PsptId);
    	SetParam(":VPSPT_TYPE_CODE", selTfFCustomer.PsptTypeCode);
    } 	
    else if( selBy == SEL_PK_TF_F_CUSTOMER) 
    { 
    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCUST_ID", selTfFCustomer.CustId);
    } 
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        TfFCustomer *pTmpTfFCustomer = new TfFCustomer();
        TfFCustomer pTmpTfFCustomer;
        
        pTmpTfFCustomer.PartitionId = GetInt("PARTITION_ID");
        pTmpTfFCustomer.CustId = GetString("CUST_ID");
        pTmpTfFCustomer.CustName = GetString("CUST_NAME");
        pTmpTfFCustomer.CustType = GetString("CUST_TYPE");
        pTmpTfFCustomer.CustState = GetString("CUST_STATE");
        pTmpTfFCustomer.PsptTypeCode = GetString("PSPT_TYPE_CODE");
        pTmpTfFCustomer.PsptId = GetString("PSPT_ID");
        pTmpTfFCustomer.ReopenTag = GetString("REOPEN_TAG");
        pTmpTfFCustomer.EparchyCode = GetString("EPARCHY_CODE");
        pTmpTfFCustomer.CityCode = GetString("CITY_CODE");
        pTmpTfFCustomer.CustPasswd = GetString("CUST_PASSWD");
        pTmpTfFCustomer.ScoreValue = GetString("SCORE_VALUE");
        pTmpTfFCustomer.CreditClass = GetInt("CREDIT_CLASS");
        pTmpTfFCustomer.BasicCreditValue = GetString("BASIC_CREDIT_VALUE");
        pTmpTfFCustomer.CreditValue = GetString("CREDIT_VALUE");
        pTmpTfFCustomer.RemoveTag = GetString("REMOVE_TAG");
        pTmpTfFCustomer.RemoveDate = GetString("REMOVE_DATE");
        pTmpTfFCustomer.DevelopDepartId = GetString("DEVELOP_DEPART_ID");
        pTmpTfFCustomer.DevelopStaffId = GetString("DEVELOP_STAFF_ID");
        pTmpTfFCustomer.IndepartId = GetString("INDEPART_ID");
        pTmpTfFCustomer.InStaffId = GetString("IN_STAFF_ID");
        pTmpTfFCustomer.InDate = GetString("IN_DATE");
        pTmpTfFCustomer.Remark = GetString("REMARK");
        pTmpTfFCustomer.Prevaluec1 = GetString("PREVALUEC1");
        pTmpTfFCustomer.Prevaluec2 = GetString("PREVALUEC2");
        pTmpTfFCustomer.Prevaluec3 = GetString("PREVALUEC3");
        pTmpTfFCustomer.Prevaluec4 = GetString("PREVALUEC4");
        pTmpTfFCustomer.Prevaluec5 = GetString("PREVALUEC5");
        pTmpTfFCustomer.Prevaluec6 = GetString("PREVALUEC6");
        pTmpTfFCustomer.Prevaluec7 = GetString("PREVALUEC7");
        pTmpTfFCustomer.Prevaluec8 = GetString("PREVALUEC8");
        pTmpTfFCustomer.Prevaluec9 = GetString("PREVALUEC9");
        pTmpTfFCustomer.Prevaluec10 = GetString("PREVALUEC10");
		
		vpRetTfFCustomer.push_back(pTmpTfFCustomer);
    }
    
    return vpRetTfFCustomer;
}
 
/** \fn CustDom::VPTfFCustomer CustDom::TfFCustomerDAO::jselect(const string &selBy, TfFCustomer &selTfFCustomer)
 *  查询方法，实现TF_F_CUSTOMER表的所有查询语句的封装，查询返回相同结果集（全表字段）；
 *  每种查询条件对应一个 selBy，传入不同的selBy 执行不同条件的查询；
 *  @param  selBy	查询条件定义
 *  @param  selTfFCustomer  查询条件对象，查询条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
CustDom::VPTfFCustomer CustDom::TfFCustomerDAO::jselby(const string &selBy, TfFCustomer &selTfFCustomer)
{
    CustDom::VPTfFCustomer  vpRetTfFCustomer; 

    
    
    ExecSelect(selBy);
    
    while (Fetch()) {
//        TfFCustomer *pTmpTfFCustomer = new TfFCustomer();
        TfFCustomer pTmpTfFCustomer;
        
		
		vpRetTfFCustomer.push_back(pTmpTfFCustomer);
    }
    
    return vpRetTfFCustomer;
}

/** \fn int CustDom::TfFCustomerDAO::jupdate(const string &updBy, TfFCustomer &updTfFCustomer) 
 *  更新方法，实现TF_F_CUSTOMER表的所有更新语句的封装
 *  @param updBy   更新条件标识
 *  @param updTfFCustomer  更新值和更新条件对象，更新条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外 
 */
int CustDom::TfFCustomerDAO::jupdate(const string &updBy, TfFCustomer &updTfFCustomer)
{
    if( updBy == UPD_IDX_TF_F_CUSTOMER_CITYCODE) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VPARTITION_ID", updTfFCustomer.PartitionId);
    	SetParam(":VCUST_ID", updTfFCustomer.CustId);
    	SetParam(":VCUST_NAME", updTfFCustomer.CustName);
    	SetParam(":VCUST_TYPE", updTfFCustomer.CustType);
    	SetParam(":VCUST_STATE", updTfFCustomer.CustState);
    	SetParam(":VPSPT_TYPE_CODE", updTfFCustomer.PsptTypeCode);
    	SetParam(":VPSPT_ID", updTfFCustomer.PsptId);
    	SetParam(":VREOPEN_TAG", updTfFCustomer.ReopenTag);
    	SetParam(":VEPARCHY_CODE", updTfFCustomer.EparchyCode);
    	SetParam(":VCITY_CODE", updTfFCustomer.CityCode);
    	SetParam(":VCUST_PASSWD", updTfFCustomer.CustPasswd);
    	SetParam(":VSCORE_VALUE", updTfFCustomer.ScoreValue);
    	SetParam(":VCREDIT_CLASS", updTfFCustomer.CreditClass);
    	SetParam(":VBASIC_CREDIT_VALUE", updTfFCustomer.BasicCreditValue);
    	SetParam(":VCREDIT_VALUE", updTfFCustomer.CreditValue);
    	SetParam(":VREMOVE_TAG", updTfFCustomer.RemoveTag);
    	SetParam(":VDEVELOP_DEPART_ID", updTfFCustomer.DevelopDepartId);
    	SetParam(":VDEVELOP_STAFF_ID", updTfFCustomer.DevelopStaffId);
    	SetParam(":VINDEPART_ID", updTfFCustomer.IndepartId);
    	SetParam(":VIN_STAFF_ID", updTfFCustomer.InStaffId);
    	SetParam(":VREMARK", updTfFCustomer.Remark);
    	SetParam(":VPREVALUEC1", updTfFCustomer.Prevaluec1);
    	SetParam(":VPREVALUEC2", updTfFCustomer.Prevaluec2);
    	SetParam(":VPREVALUEC3", updTfFCustomer.Prevaluec3);
    	SetParam(":VPREVALUEC4", updTfFCustomer.Prevaluec4);
    	SetParam(":VPREVALUEC5", updTfFCustomer.Prevaluec5);
    	SetParam(":VPREVALUEC6", updTfFCustomer.Prevaluec6);
    	SetParam(":VPREVALUEC7", updTfFCustomer.Prevaluec7);
    	SetParam(":VPREVALUEC8", updTfFCustomer.Prevaluec8);
    	SetParam(":VPREVALUEC9", updTfFCustomer.Prevaluec9);
    	SetParam(":VPREVALUEC10", updTfFCustomer.Prevaluec10);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCITY_CODE", updTfFCustomer.CityCode);
    	SetParam(":VDEVELOP_DEPART_ID", updTfFCustomer.DevelopDepartId);
    } 	
    else if( updBy == UPD_IDX_TF_F_CUSTOMER_CUSTNAME) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VPARTITION_ID", updTfFCustomer.PartitionId);
    	SetParam(":VCUST_ID", updTfFCustomer.CustId);
    	SetParam(":VCUST_NAME", updTfFCustomer.CustName);
    	SetParam(":VCUST_TYPE", updTfFCustomer.CustType);
    	SetParam(":VCUST_STATE", updTfFCustomer.CustState);
    	SetParam(":VPSPT_TYPE_CODE", updTfFCustomer.PsptTypeCode);
    	SetParam(":VPSPT_ID", updTfFCustomer.PsptId);
    	SetParam(":VREOPEN_TAG", updTfFCustomer.ReopenTag);
    	SetParam(":VEPARCHY_CODE", updTfFCustomer.EparchyCode);
    	SetParam(":VCITY_CODE", updTfFCustomer.CityCode);
    	SetParam(":VCUST_PASSWD", updTfFCustomer.CustPasswd);
    	SetParam(":VSCORE_VALUE", updTfFCustomer.ScoreValue);
    	SetParam(":VCREDIT_CLASS", updTfFCustomer.CreditClass);
    	SetParam(":VBASIC_CREDIT_VALUE", updTfFCustomer.BasicCreditValue);
    	SetParam(":VCREDIT_VALUE", updTfFCustomer.CreditValue);
    	SetParam(":VREMOVE_TAG", updTfFCustomer.RemoveTag);
    	SetParam(":VDEVELOP_DEPART_ID", updTfFCustomer.DevelopDepartId);
    	SetParam(":VDEVELOP_STAFF_ID", updTfFCustomer.DevelopStaffId);
    	SetParam(":VINDEPART_ID", updTfFCustomer.IndepartId);
    	SetParam(":VIN_STAFF_ID", updTfFCustomer.InStaffId);
    	SetParam(":VREMARK", updTfFCustomer.Remark);
    	SetParam(":VPREVALUEC1", updTfFCustomer.Prevaluec1);
    	SetParam(":VPREVALUEC2", updTfFCustomer.Prevaluec2);
    	SetParam(":VPREVALUEC3", updTfFCustomer.Prevaluec3);
    	SetParam(":VPREVALUEC4", updTfFCustomer.Prevaluec4);
    	SetParam(":VPREVALUEC5", updTfFCustomer.Prevaluec5);
    	SetParam(":VPREVALUEC6", updTfFCustomer.Prevaluec6);
    	SetParam(":VPREVALUEC7", updTfFCustomer.Prevaluec7);
    	SetParam(":VPREVALUEC8", updTfFCustomer.Prevaluec8);
    	SetParam(":VPREVALUEC9", updTfFCustomer.Prevaluec9);
    	SetParam(":VPREVALUEC10", updTfFCustomer.Prevaluec10);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCUST_NAME", updTfFCustomer.CustName);
    } 	
    else if( updBy == UPD_IDX_TF_F_CUSTOMER_PSPTID) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VPARTITION_ID", updTfFCustomer.PartitionId);
    	SetParam(":VCUST_ID", updTfFCustomer.CustId);
    	SetParam(":VCUST_NAME", updTfFCustomer.CustName);
    	SetParam(":VCUST_TYPE", updTfFCustomer.CustType);
    	SetParam(":VCUST_STATE", updTfFCustomer.CustState);
    	SetParam(":VPSPT_TYPE_CODE", updTfFCustomer.PsptTypeCode);
    	SetParam(":VPSPT_ID", updTfFCustomer.PsptId);
    	SetParam(":VREOPEN_TAG", updTfFCustomer.ReopenTag);
    	SetParam(":VEPARCHY_CODE", updTfFCustomer.EparchyCode);
    	SetParam(":VCITY_CODE", updTfFCustomer.CityCode);
    	SetParam(":VCUST_PASSWD", updTfFCustomer.CustPasswd);
    	SetParam(":VSCORE_VALUE", updTfFCustomer.ScoreValue);
    	SetParam(":VCREDIT_CLASS", updTfFCustomer.CreditClass);
    	SetParam(":VBASIC_CREDIT_VALUE", updTfFCustomer.BasicCreditValue);
    	SetParam(":VCREDIT_VALUE", updTfFCustomer.CreditValue);
    	SetParam(":VREMOVE_TAG", updTfFCustomer.RemoveTag);
    	SetParam(":VDEVELOP_DEPART_ID", updTfFCustomer.DevelopDepartId);
    	SetParam(":VDEVELOP_STAFF_ID", updTfFCustomer.DevelopStaffId);
    	SetParam(":VINDEPART_ID", updTfFCustomer.IndepartId);
    	SetParam(":VIN_STAFF_ID", updTfFCustomer.InStaffId);
    	SetParam(":VREMARK", updTfFCustomer.Remark);
    	SetParam(":VPREVALUEC1", updTfFCustomer.Prevaluec1);
    	SetParam(":VPREVALUEC2", updTfFCustomer.Prevaluec2);
    	SetParam(":VPREVALUEC3", updTfFCustomer.Prevaluec3);
    	SetParam(":VPREVALUEC4", updTfFCustomer.Prevaluec4);
    	SetParam(":VPREVALUEC5", updTfFCustomer.Prevaluec5);
    	SetParam(":VPREVALUEC6", updTfFCustomer.Prevaluec6);
    	SetParam(":VPREVALUEC7", updTfFCustomer.Prevaluec7);
    	SetParam(":VPREVALUEC8", updTfFCustomer.Prevaluec8);
    	SetParam(":VPREVALUEC9", updTfFCustomer.Prevaluec9);
    	SetParam(":VPREVALUEC10", updTfFCustomer.Prevaluec10);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VPSPT_ID", updTfFCustomer.PsptId);
    	SetParam(":VPSPT_TYPE_CODE", updTfFCustomer.PsptTypeCode);
    } 	
    else if( updBy == UPD_PK_TF_F_CUSTOMER) 
    { 
    	/**< 为UPDATE字段传入更新值 */
    	SetParam(":VPARTITION_ID", updTfFCustomer.PartitionId);
    	SetParam(":VCUST_ID", updTfFCustomer.CustId);
    	SetParam(":VCUST_NAME", updTfFCustomer.CustName);
    	SetParam(":VCUST_TYPE", updTfFCustomer.CustType);
    	SetParam(":VCUST_STATE", updTfFCustomer.CustState);
    	SetParam(":VPSPT_TYPE_CODE", updTfFCustomer.PsptTypeCode);
    	SetParam(":VPSPT_ID", updTfFCustomer.PsptId);
    	SetParam(":VREOPEN_TAG", updTfFCustomer.ReopenTag);
    	SetParam(":VEPARCHY_CODE", updTfFCustomer.EparchyCode);
    	SetParam(":VCITY_CODE", updTfFCustomer.CityCode);
    	SetParam(":VCUST_PASSWD", updTfFCustomer.CustPasswd);
    	SetParam(":VSCORE_VALUE", updTfFCustomer.ScoreValue);
    	SetParam(":VCREDIT_CLASS", updTfFCustomer.CreditClass);
    	SetParam(":VBASIC_CREDIT_VALUE", updTfFCustomer.BasicCreditValue);
    	SetParam(":VCREDIT_VALUE", updTfFCustomer.CreditValue);
    	SetParam(":VREMOVE_TAG", updTfFCustomer.RemoveTag);
    	SetParam(":VDEVELOP_DEPART_ID", updTfFCustomer.DevelopDepartId);
    	SetParam(":VDEVELOP_STAFF_ID", updTfFCustomer.DevelopStaffId);
    	SetParam(":VINDEPART_ID", updTfFCustomer.IndepartId);
    	SetParam(":VIN_STAFF_ID", updTfFCustomer.InStaffId);
    	SetParam(":VREMARK", updTfFCustomer.Remark);
    	SetParam(":VPREVALUEC1", updTfFCustomer.Prevaluec1);
    	SetParam(":VPREVALUEC2", updTfFCustomer.Prevaluec2);
    	SetParam(":VPREVALUEC3", updTfFCustomer.Prevaluec3);
    	SetParam(":VPREVALUEC4", updTfFCustomer.Prevaluec4);
    	SetParam(":VPREVALUEC5", updTfFCustomer.Prevaluec5);
    	SetParam(":VPREVALUEC6", updTfFCustomer.Prevaluec6);
    	SetParam(":VPREVALUEC7", updTfFCustomer.Prevaluec7);
    	SetParam(":VPREVALUEC8", updTfFCustomer.Prevaluec8);
    	SetParam(":VPREVALUEC9", updTfFCustomer.Prevaluec9);
    	SetParam(":VPREVALUEC10", updTfFCustomer.Prevaluec10);

    	/**< 为WHERE条件字段传入条件值 */
    	SetParam(":VCUST_ID", updTfFCustomer.CustId);
    } 
    
    ExecUpdate(updBy);

    return 0;
}

/** \fn int CustDom::TfFCustomerDAO::jdelete(const string &delBy, TfFCustomer &delTfFCustomer)
 *  删除方法，实现TF_F_CUSTOMER表的所有删除语句的封装
 *  @param delBy 删除条件标识
 *  @param delTfFCustomer  删除条件对象，删除条件匹配的条件值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int CustDom::TfFCustomerDAO::jdelete(const string &delBy, TfFCustomer &delTfFCustomer)
{
	if( delBy == DEL_IDX_TF_F_CUSTOMER_CITYCODE) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VCITY_CODE", delTfFCustomer.CityCode);
		SetParam(":VDEVELOP_DEPART_ID", delTfFCustomer.DevelopDepartId);
	} 	
	else if( delBy == DEL_IDX_TF_F_CUSTOMER_CUSTNAME) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VCUST_NAME", delTfFCustomer.CustName);
	} 	
	else if( delBy == DEL_IDX_TF_F_CUSTOMER_PSPTID) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VPSPT_ID", delTfFCustomer.PsptId);
		SetParam(":VPSPT_TYPE_CODE", delTfFCustomer.PsptTypeCode);
	} 	
	else if( delBy == DEL_PK_TF_F_CUSTOMER) 
	{ 
		/**< 为WHERE条件字段传入条件值 */
		SetParam(":VCUST_ID", delTfFCustomer.CustId);
	} 
	
	ExecDelete(delBy);
	
	return 0;
}

/** \fn int CustDom::TfFCustomerDAO::jinsert(TfFCustomer &insTfFCustomer)
 *  插入方法，实现TF_F_CUSTOMER表的插入语句的封装
 *  @param insTfFCustomer 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int CustDom::TfFCustomerDAO::jinsert(TfFCustomer &insTfFCustomer)
{
    /**< 为INSERT字段传入插入值 */
    SetParam(":VPARTITION_ID", insTfFCustomer.PartitionId);
    SetParam(":VCUST_ID", insTfFCustomer.CustId);
    SetParam(":VCUST_NAME", insTfFCustomer.CustName);
    SetParam(":VCUST_TYPE", insTfFCustomer.CustType);
    SetParam(":VCUST_STATE", insTfFCustomer.CustState);
    SetParam(":VPSPT_TYPE_CODE", insTfFCustomer.PsptTypeCode);
    SetParam(":VPSPT_ID", insTfFCustomer.PsptId);
    SetParam(":VREOPEN_TAG", insTfFCustomer.ReopenTag);
    SetParam(":VEPARCHY_CODE", insTfFCustomer.EparchyCode);
    SetParam(":VCITY_CODE", insTfFCustomer.CityCode);
    SetParam(":VCUST_PASSWD", insTfFCustomer.CustPasswd);
    SetParam(":VSCORE_VALUE", insTfFCustomer.ScoreValue);
    SetParam(":VCREDIT_CLASS", insTfFCustomer.CreditClass);
    SetParam(":VBASIC_CREDIT_VALUE", insTfFCustomer.BasicCreditValue);
    SetParam(":VCREDIT_VALUE", insTfFCustomer.CreditValue);
    SetParam(":VREMOVE_TAG", insTfFCustomer.RemoveTag);
    SetParam(":VDEVELOP_DEPART_ID", insTfFCustomer.DevelopDepartId);
    SetParam(":VDEVELOP_STAFF_ID", insTfFCustomer.DevelopStaffId);
    SetParam(":VINDEPART_ID", insTfFCustomer.IndepartId);
    SetParam(":VIN_STAFF_ID", insTfFCustomer.InStaffId);
    SetParam(":VREMARK", insTfFCustomer.Remark);
    SetParam(":VPREVALUEC1", insTfFCustomer.Prevaluec1);
    SetParam(":VPREVALUEC2", insTfFCustomer.Prevaluec2);
    SetParam(":VPREVALUEC3", insTfFCustomer.Prevaluec3);
    SetParam(":VPREVALUEC4", insTfFCustomer.Prevaluec4);
    SetParam(":VPREVALUEC5", insTfFCustomer.Prevaluec5);
    SetParam(":VPREVALUEC6", insTfFCustomer.Prevaluec6);
    SetParam(":VPREVALUEC7", insTfFCustomer.Prevaluec7);
    SetParam(":VPREVALUEC8", insTfFCustomer.Prevaluec8);
    SetParam(":VPREVALUEC9", insTfFCustomer.Prevaluec9);
    SetParam(":VPREVALUEC10", insTfFCustomer.Prevaluec10);
    
    ExecInsert();

    return 0;
}

/** \fn int CustDom::TfFCustomerDAO::jinsert(TfFCustomer &insTfFCustomer)
 *  插入方法，实现TF_F_CUSTOMER表的插入语句的封装
 *  @param insTfFCustomer 插入值对象，插入字段的匹配值由该对象传入
 *  @return 成功返回0，失败则抛出例外
 */
int CustDom::TfFCustomerDAO::jinsby(const string &insBy, TfFCustomer &insTfFCustomer)
{
    if( insBy == INS_STAFF_0) 
    { 
    	/**< 为INSERT参数字段传入条件值 */
//    	SetParam(":VPARTITION_ID", insTfFCustomer.PartitionId);
    	SetParam(":VCUST_ID", insTfFCustomer.CustId);
    	SetParam(":VCUST_NAME", insTfFCustomer.CustName);
    	SetParam(":VCUST_TYPE", insTfFCustomer.CustType);
    	SetParam(":VCUST_STATE", insTfFCustomer.CustState);
    	SetParam(":VPSPT_TYPE_CODE", insTfFCustomer.PsptTypeCode);
    	SetParam(":VPSPT_ID", insTfFCustomer.PsptId);
    	SetParam(":VREOPEN_TAG", insTfFCustomer.ReopenTag);
    	SetParam(":VEPARCHY_CODE", insTfFCustomer.EparchyCode);
    	SetParam(":VCITY_CODE", insTfFCustomer.CityCode);
    	SetParam(":VREMOVE_TAG", insTfFCustomer.RemoveTag);
    	SetParam(":VINDEPART_ID", insTfFCustomer.IndepartId);
    	SetParam(":VIN_STAFF_ID", insTfFCustomer.InStaffId);
    	SetParam(":VREMARK", insTfFCustomer.Remark);
    
    ExecInsert(insBy);

	}
	return 0;
}
