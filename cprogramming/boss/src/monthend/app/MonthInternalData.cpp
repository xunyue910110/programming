#include "MonthInternalData.h"

//##ModelId=477C465501C8
MonthInternalData::MonthInternalData()
{
	m_monthAccountParam = NULL;
//	m_zeroDetailBillIntoIntDbTag = '9';
//	m_intergrateBillVec.clear();
}

//##ModelId=477C465501C9
MonthInternalData::~MonthInternalData()
{
	//m_tmABillVector.clear();
}

//##ModelId=477C465501CA
void MonthInternalData::initialize(const int& minRealCycId,
	                            PM_MonthAccount *monthAccountParam,
	                            const char &zeroDetailBillIntoIntDbTag
	                           )
{

    m_minRealCycId = minRealCycId;
    m_monthAccountParam = monthAccountParam;
//    m_zeroDetailBillIntoIntDbTag=zeroDetailBillIntoIntDbTag;
    	
}

//##ModelId=477C465501EA
void MonthInternalData::checkUserBillInfo()
{
}


//##ModelId=477D97CE014F
void MonthInternalData::getRealWriteoffTag()
{
	//天津已经取消实时销账
}


bool MonthInternalData::execMonthTransbillCrm()
{
	return (*m_monthAccountParam).execMonthTransbillCrm();
}


//账单转换
bool MonthInternalData::transDetailbillToIntegratebill()
{

	try{
	
		//取不开账用户	
		(*m_monthAccountParam).getUnacctUser();

		//add 2009-02-10 zhouq begin
		//取用户,网别
//		(*m_monthAccountParam).getUserNetTypeCode();
		//add 2009-02-10 zhouq begin

  //#ifdef __PROV019__ //山西 取无效用户
	//  (*m_monthAccountParam).getSleepUser();
  //#endif		

		//取流水号
		(*m_monthAccountParam).GetBillId(MAX_USER_NUM);
		//added by pangx on 2010-3-29 begin
		//取一点付账户信息
		(*m_monthAccountParam).getOneClickPayAccountInfo();
		//取VPDN账目顺序
		(*m_monthAccountParam).getVpdnOneClickPayItemPosInfo();
		//取明细账目与外部账目对应信息
		(*m_monthAccountParam).getExternalItemInfo();
		//added by pangx on 2010-3-29 end
		//取综合账单
		(*m_monthAccountParam).getTsAbillFromDetailAdjust();
		//执行存储过程
		execMonthTransbillCrm();

 		}catch(const exception & e)
 		{
 	 		DEBUG_PRINTF("开账失败: %s",e.what());
   		THROW_P1(MonthEndException, "开账失败: %s",e.what());    
 		}
  
  return true;
}


//
