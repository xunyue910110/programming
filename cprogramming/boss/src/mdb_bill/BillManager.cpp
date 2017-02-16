#include "BillManager.h"
#include "base/supdebug.h"
#include "mdb/Mdb_Exception.h"
#include "mdb/InstanceFactory.h"

BillManager::BillManager()
{
  m_db=NULL;
  m_session=NULL;
  m_connected=false;
  m_errno=0;
}

bool BillManager::initialization(const MdbInfo& mdbInfo)
{
	m_mdbInfo = mdbInfo;
	if(connect()==false)
	{
	  #ifdef _DEBUG_
	    cout<<"m_errMsg = "<<m_errMsg<<" "<<__FILE__<<__LINE__<<endl;
	  #endif
	  return false;
	}
 	m_LockUserFields.clear();
 	m_LockUserFields.push_back("UserId");
 	m_LockUserFields.push_back("type");
 	m_LockUserFields.push_back("channelNo");

	m_userBillUpFields.clear();
	m_userBillUpFields.push_back("value");
	m_userBillUpFields.push_back("discntValue");
	//Added By Sunam 2009/8/12 11:34:03
	m_userBillUpFields.push_back("AdjustBefore");
	m_userSum1UpFields.clear();
	m_userSum1UpFields.push_back("value");
	//Added By Sunam 2009/8/8 10:18:57
	m_userSum1UpFields.push_back("maxValue");
	m_userSum2UpFields.clear();
	m_userSum2UpFields.push_back("EndTime");
	m_userSum2UpFields.push_back("Value");
	m_userSum2UpFields.push_back("Type");
	m_userSum2UpFields.push_back("GroupUserId");
	//Added By Sunam 2009/8/8 10:19:41
	m_userSum2UpFields.push_back("maxValue");
	
	m_userSum3UpFields.clear();
	m_userSum3UpFields.push_back("EndTime");
	m_userSum3UpFields.push_back("Value");
		
	m_userMonfeeBillUpFields.clear();
	m_userMonfeeBillUpFields.push_back("DataType");
	m_userMonfeeBillUpFields.push_back("Value");
	m_userMonfeeBillUpFields.push_back("DiscntValue");
	m_userMonfeeBillUpFields.push_back("DisplayType");
	m_userMonfeeBillUpFields.push_back("SerialNumber");
	//m_userMonfeeBillUpFields.push_back("FeeType");
		
	m_accntBillUpFields.clear();
	m_accntBillUpFields.push_back("INIT_FEE");
	m_accntBillUpFields.push_back("FEE");
	m_accntBillUpFields.push_back("A_DISCNT");
	m_accntBillUpFields.push_back("ADJUST_BEFORE");
	m_accntBillUpFields.push_back("ACCT_ADJUST_BEFORE");

	m_rateDiscntUpFields.clear();
	m_rateDiscntUpFields.push_back("Value");

	m_userMonfeeBillInFields.clear();
	m_userMonfeeBillInFields.push_back("UserId");
	m_userMonfeeBillInFields.push_back("SpCode");
	m_userMonfeeBillInFields.push_back("SpBizCode");
	m_userMonfeeBillInFields.push_back("ItemCode");
  m_userMonfeeBillInFields.push_back("DataType");
	m_userMonfeeBillInFields.push_back("Value");
	m_userMonfeeBillInFields.push_back("DiscntValue");
	m_userMonfeeBillInFields.push_back("DisplayType");
	m_userMonfeeBillInFields.push_back("SerialNumber");
	m_userMonfeeBillInFields.push_back("FeeType");
	
	m_userBillInFields.clear();
	m_userBillInFields.push_back("UserId");
	m_userBillInFields.push_back("BillId");
	m_userBillInFields.push_back("value");
	m_userBillInFields.push_back("discntValue");
	//Added By Sunam 2009/8/12 11:34:21
	m_userBillInFields.push_back("AdjustBefore");
	m_userSum1InFields.clear();
	m_userSum1InFields.push_back("UserId");
	m_userSum1InFields.push_back("TpObjId");
	m_userSum1InFields.push_back("BillId");
	m_userSum1InFields.push_back("value");
	//Added By Sunam 2009/8/8 10:19:41
	m_userSum1InFields.push_back("maxValue");
	m_userSum2InFields.clear();
	m_userSum2InFields.push_back("UserId");
	m_userSum2InFields.push_back("TpObjId");
	m_userSum2InFields.push_back("BeginTime");
	m_userSum2InFields.push_back("EndTime");
	m_userSum2InFields.push_back("CycleId");
	m_userSum2InFields.push_back("BillId");
	m_userSum2InFields.push_back("Value");
	m_userSum2InFields.push_back("Type");
	m_userSum2InFields.push_back("GroupUserId");
	//Added By Sunam 2009/8/8 10:19:41
	m_userSum2InFields.push_back("maxValue");
	
	m_userSum3InFields.clear();
	m_userSum3InFields.push_back("UserId");
	m_userSum3InFields.push_back("TpObjId");
	m_userSum3InFields.push_back("SpCode");
	m_userSum3InFields.push_back("SpBizCode");
	m_userSum3InFields.push_back("BeginTime");	
	m_userSum3InFields.push_back("BeginTime");
	m_userSum3InFields.push_back("EndTime");
	m_userSum3InFields.push_back("CycleId");
	m_userSum3InFields.push_back("Value");
	
	m_accntBillInFields.clear();
	m_accntBillInFields.push_back("ACCT_ID");
	m_accntBillInFields.push_back("USER_ID");
	m_accntBillInFields.push_back("EPARCHY_CODE");
	m_accntBillInFields.push_back("DETAIL_ITEM_CODE");
	m_accntBillInFields.push_back("INIT_FEE");
	m_accntBillInFields.push_back("FEE");
	m_accntBillInFields.push_back("A_DISCNT");
	m_accntBillInFields.push_back("ADJUST_BEFORE");
	//add by xuf 2009-8-19 21:44:03
	m_accntBillInFields.push_back("ACCT_ADJUST_BEFORE");
	m_rateDiscntInFields.clear();
	m_rateDiscntInFields.push_back("UserId");
	m_rateDiscntInFields.push_back("AccountId");
	m_rateDiscntInFields.push_back("BillId");
	m_rateDiscntInFields.push_back("Value");

	m_rollBillInFields.clear();
	m_rollBillInFields.push_back("channelNo");
	m_rollBillInFields.push_back("UserId");
	m_rollBillInFields.push_back("billType");
	m_rollBillInFields.push_back("billCycle");
	m_rollBillInFields.push_back("AccountId");
	m_rollBillInFields.push_back("TpObjId");
	m_rollBillInFields.push_back("BeginTime");
	m_rollBillInFields.push_back("EndTime");
	m_rollBillInFields.push_back("BillId");
	m_rollBillInFields.push_back("Value");
	m_rollBillInFields.push_back("discntValue");
	//Added By Sunam 2009/8/12 11:33:30
	m_rollBillInFields.push_back("AdjustBefore");
	m_rollBillInFields.push_back("a_discnt");
	m_rollBillInFields.push_back("b_discnt");
	m_rollBillInFields.push_back("type");
	m_rollBillInFields.push_back("GroupUserId");
	
	m_rollBillInFields.push_back("DataType");
	m_rollBillInFields.push_back("FeeType");
	m_rollBillInFields.push_back("SpCode");
	m_rollBillInFields.push_back("SpBizCode");
	
	
	
	m_rateInfoUpFields.clear();
	m_rateInfoUpFields.push_back("state");
	m_rateInfoUpFields.push_back("fileName");
	m_rateInfoUpFields.push_back("billCycle");
	m_rateInfoUpFields.push_back("rateDate");
	m_rateInfoUpFields.push_back("nextFileNo");
	m_rateInfoUpFields.push_back("remark");
	
	return true;
}

//Added By Sunam 2009/8/9 8:52:19批量加锁
bool BillManager::lockUserId(const set<T_SERVER_ID> &r_userId,const set<T_SERVER_ID> &r_acctId,const int &r_channelNo)
{
    vector<InputParams *> pInputParams;
    InputParams * tmpInputParams;
    long *t_userId;
    vector<long *> t_userIdList;
    set<T_SERVER_ID>::const_iterator itr;
    int i;
    int t_type1=0;
    int t_type2=1;
    //Added By Sunam 2009/8/31 22:01:21
    vector<int> errVect;
	try
	{
		if(connect()==false) 
		{
			return false;
		}
		//m_session->dataBind(errVect);
		m_session->begin("USER_LOCK");
		for (itr = r_userId.begin(); itr != r_userId.end(); ++itr)
		{
			t_userId = new long;
			*t_userId = itr->value();
			t_userIdList.push_back(t_userId);
			tmpInputParams = new InputParams(3);
			tmpInputParams->setValue(VAR_TYPE_LONG, (void *)t_userId);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&t_type1);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&r_channelNo);
			pInputParams.push_back(tmpInputParams);
		}
		for (itr = r_acctId.begin(); itr != r_acctId.end(); ++itr)
		{
			t_userId = new long;
			*t_userId = itr->value();
			t_userIdList.push_back(t_userId);
			tmpInputParams = new InputParams(3);
			tmpInputParams->setValue(VAR_TYPE_LONG, (void *)t_userId);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&t_type2);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&r_channelNo);
			pInputParams.push_back(tmpInputParams);
		}
		m_session->insert("USER_LOCK",m_LockUserFields,pInputParams,&errVect);
		m_session->commit();
	    
	    for(i = 0; i < pInputParams.size(); ++i)
	    {
			delete pInputParams[i];
			delete t_userIdList[i];
	    }
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
            e.toString();
            cout<<__FILE__<<__LINE__<<endl;
    	#endif
    	m_errVector.clear();
    	for (int l=0; l < errVect.size(); ++l)
        {
            char t_buffer[20];
            memset(t_buffer,0,sizeof(t_buffer));
            sprintf(t_buffer,"%ld",*t_userIdList[errVect[l]]);
            m_errVector.push_back(t_buffer);
            //用户加锁失败
            if (errVect[l] < r_userId.size())
            {
                m_errVector.push_back("0");
            }
            //账户加锁失败
            else
            {
                m_errVector.push_back("1");
            }
        }
		m_session->rollBack();
		disconnect();
		for(i = 0; i < pInputParams.size(); ++i)
	    {
			delete pInputParams[i];
			delete t_userIdList[i];
	    }
		return false;
	}
	return true;
}

//Added By zb 2009/8/12 批量加锁
bool BillManager::lockUserId(const set<T_SERVER_ID> &r_userId,const int &r_type,const int &r_channelNo)
{
  vector<InputParams *> pInputParams;
  InputParams * tmpInputParams;
  long *t_userId;
  vector<long *> t_userIdList;
  set<T_SERVER_ID>::const_iterator itr;
  int i;
	try
	{
		if(connect()==false) 
		{
			return false;
		}
		
		m_session->begin("USER_LOCK");
		for (itr = r_userId.begin(); itr != r_userId.end(); ++itr)
		{
			t_userId = new long;
			*t_userId = itr->value();
			t_userIdList.push_back(t_userId);
			tmpInputParams = new InputParams(3);
			tmpInputParams->setValue(VAR_TYPE_LONG, (void *)t_userId);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&r_type);
			tmpInputParams->setValue(VAR_TYPE_INT,  (void *)&r_channelNo);
			pInputParams.push_back(tmpInputParams);
		}
		m_session->insert("USER_LOCK",m_LockUserFields,pInputParams);
		m_session->commit();
	    
	    for(i = 0; i < pInputParams.size(); ++i)
	    {
			delete pInputParams[i];
			delete t_userIdList[i];
	    }
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();
		  cout<<__FILE__<<__LINE__<<endl;
    	#endif
		m_session->rollBack();
		disconnect();
		for(i = 0; i < pInputParams.size(); ++i)
	    {
			delete pInputParams[i];
			delete t_userIdList[i];
	    }
		return false;
	}
	return true;
}



//用户加解锁
bool BillManager::lockUserId(const T_SERVER_ID &r_userId,const int &r_type,const int &r_channelNo)
{
  InputParams *pInputParams = new InputParams(3);
  long  t_userId = r_userId.value();
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		
		m_session->begin("USER_LOCK");
		pInputParams->setValue(VAR_TYPE_LONG, (void *)&t_userId);
		pInputParams->setValue(VAR_TYPE_INT,  (void *)&r_type);
		pInputParams->setValue(VAR_TYPE_INT,  (void *)&r_channelNo);
		m_session->insert("USER_LOCK",m_LockUserFields,pInputParams);
		m_session->commit();
	  
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif
		m_session->rollBack();
		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}
//增加一个等待超时的lock接口 timeOut为ms
bool BillManager::lockUserId_w(const T_SERVER_ID &r_userId,const int &r_type,const int &r_channelNo,const int &timeOut)
{
  long  t_times=0;
  while(t_times < timeOut*10000/10) //重试时间
  {
    if(lockUserId(r_userId,r_type,r_channelNo)==false)
    {      
      usleep(10); //休息10微秒
      ++t_times;
    }else
    {
      return true;
    }
  }
  return false;
}

bool BillManager::unlockUserId(const T_SERVER_ID &r_userId,const int &r_type)
{
  long  t_userId = r_userId.value();
	InputParams *pInputParamsIndexes = new InputParams(2);
	try
	{
		//modified by gaojf on 20090504
	  if(connect()==false) 
		{
		  delete pInputParamsIndexes;
		  return false;
		}
		
		m_session->begin("USER_LOCK");
		pInputParamsIndexes->setValue(VAR_TYPE_LONG, (void *)&t_userId);
		pInputParamsIndexes->setValue(VAR_TYPE_INT , (void *)&r_type);
		m_session->deleteRec("USER_LOCK","IDX_USER_LOCK",pInputParamsIndexes,NULL,NULL);
		m_session->commit();
	  
		delete pInputParamsIndexes;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		m_session->rollBack();
		disconnect();
		delete pInputParamsIndexes;
		return false;
	}
	return true;
}

bool BillManager::clearUserLock(const int &r_channelNo)
{
	InputParams *pInputParamsIndexes = new InputParams(1);
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParamsIndexes;
		  return false;
		}
		
		m_session->begin("USER_LOCK");
		pInputParamsIndexes->setValue(VAR_TYPE_INT, (void *)&r_channelNo);
		m_session->deleteRec("USER_LOCK","IDX_LOCK_CHANNEL",pInputParamsIndexes,NULL,NULL);
		m_session->commit();
	  
		delete pInputParamsIndexes;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		m_session->rollBack();
		disconnect();
		delete pInputParamsIndexes;
		return false;
	}
	return true;
}

bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserBill>     &r_bills)
{
	InputParams *pInputParams = new InputParams(1);
	long   t_userId = r_userId.value();
	
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	UserBill billItem;
	int length;
	//r_bills.clear(); //内部不清空
	try
	{
		//modified by gaojf on 20090504
	  if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}	
		
		m_session->select(UserBillTable[r_cycle%100-1].TableName.c_str(),UserBillTable[r_cycle%100-1].IndexName.c_str(),pInputParams,NULL,NULL);
		billItem.m_userId = r_userId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("billId",&billItem.m_billId,length);
			m_session->getColumnValue("value" ,&billItem.m_value,length);
			m_session->getColumnValue("discntValue" ,&billItem.m_discntValue,length);
			//Added By Sunam 2009/8/12 11:35:13
			m_session->getColumnValue("AdjustBefore" ,&billItem.m_adjustBefore,length);
			r_bills.push_back(billItem);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}
bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserMonfeeBill> &r_bills)
{
	InputParams *pInputParams = new InputParams(1);
	long   t_userId = r_userId.value();
	
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	UserMonfeeBill billItem;
	int length;

	try
	{
	  if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}	
		
		m_session->select(UserMonfeeBillTable[r_cycle%2].TableName.c_str(),UserMonfeeBillTable[r_cycle%2].IndexName.c_str(),pInputParams,NULL,NULL);
		billItem.m_userId = r_userId;
		while(m_session->getNextRec())
		{
	  	m_session->getColumnValue("SpCode" ,&billItem.m_spCode,length);
		  m_session->getColumnValue("SpBizCode" ,&billItem.m_spBizCode,length);
			m_session->getColumnValue("ItemCode",&billItem.m_itemCode,length);
			m_session->getColumnValue("DataType" ,&billItem.m_dateType,length);
			m_session->getColumnValue("Value" ,&billItem.m_fee,length);
			m_session->getColumnValue("DiscntValue" ,&billItem.m_discntFee,length);
			m_session->getColumnValue("DisplayType" ,&billItem.m_displayType,length);
			m_session->getColumnValue("SerialNumber" ,&billItem.m_serialNumber,length);
			m_session->getColumnValue("FeeType" ,&billItem.m_feeType,length);
			r_bills.push_back(billItem);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}
bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill1> &r_bills)
{
  long  t_userId = r_userId.value();
	InputParams *pInputParams = new InputParams(1);
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	UserSumBill1 billItem;
	int length;
	//r_bills.clear(); //内部不清空
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		
		//Modified By Sunam 2009/8/9 8:55:40
		//m_session->select(UserSumBill1Table[r_cycle%2].TableName.c_str(),UserSumBill1Table[r_cycle%2].IndexName.c_str(),pInputParams,NULL,NULL);
		m_session->select(UserSumBill1Table[r_cycle%100-1].TableName.c_str(),UserSumBill1Table[r_cycle%100-1].IndexName.c_str(),pInputParams,NULL,NULL);
		billItem.m_userId = r_userId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("tpObjId" ,&billItem.m_tpObjId,length);
			m_session->getColumnValue("billId"  ,&billItem.m_billId,length);
			m_session->getColumnValue("value"   ,&billItem.m_value,length);
			//Added By Sunam 2009/8/8 10:20:45
			m_session->getColumnValue("maxValue",&billItem.m_maxValue,length);
			r_bills.push_back(billItem);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<UserSumBill2> &r_bills)
{
  long  t_userId = r_userId.value();
  long  t_groupUserId;
  InputParams *pInputParams = new InputParams(1);
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	UserSumBill2 billItem;
	int length;
	//r_bills.clear(); //内部不清空
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		
		//Modified By Sunam 2009/8/10 11:41:30
		//m_session->select(UserSumBill2Table[r_cycle%2].TableName.c_str(),UserSumBill2Table[r_cycle%2].IndexName.c_str(),pInputParams,NULL,NULL);
		m_session->select(UserSumBill2Table.TableName.c_str(),UserSumBill2Table.IndexName.c_str(),pInputParams,NULL,NULL);
		billItem.m_userId = r_userId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("tpObjId"    ,&billItem.m_tpObjId,length);
			m_session->getColumnValue("BeginTime"  ,&billItem.m_beginTime,length);
			m_session->getColumnValue("EndTime"    ,&billItem.m_endTime,length);
			m_session->getColumnValue("CycleId"    ,&billItem.m_cycleId,length);
			m_session->getColumnValue("billId"     ,&billItem.m_billId,length);
			m_session->getColumnValue("value"      ,&billItem.m_value,length);
			m_session->getColumnValue("Type"       ,&billItem.m_type,length);
			m_session->getColumnValue("GroupUserId",&t_groupUserId,length);
			//Added By Sunam 2009/8/8 10:21:24
			m_session->getColumnValue("maxValue"   ,&billItem.m_maxValue,length);
			billItem.m_groupUserId = t_groupUserId;
			r_bills.push_back(billItem);
		}
		delete pInputParams;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,
                              vector<UserSumBill3> &r_bills)
{
  long  t_userId = r_userId.value();
  InputParams *pInputParams = new InputParams(1);
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
	UserSumBill3 billItem;
	int length;
	try
	{
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		m_session->select(UserSumBill3Table.TableName.c_str(),UserSumBill3Table.IndexName.c_str(),pInputParams,NULL,NULL);
		billItem.m_userId = r_userId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("tpObjId"    ,&billItem.m_tpObjId,length);
		       m_session->getColumnValue("spCode"     ,&billItem.m_spCode,length);
			m_session->getColumnValue("spBizCode"  ,&billItem.m_spBizCode,length);
			m_session->getColumnValue("BeginTime"  ,&billItem.m_beginTime,length);
			m_session->getColumnValue("EndTime"    ,&billItem.m_endTime,length);
			m_session->getColumnValue("CycleId"    ,&billItem.m_cycleId,length);
			m_session->getColumnValue("value"      ,&billItem.m_value,length);

			r_bills.push_back(billItem);
		}
		delete pInputParams;
	}
	catch(Mdb_Exception &e)
	{
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_accountId,vector<AccountBill>  &r_bills)
{
  long  t_accountId = r_accountId.value();
  long  t_userId;
  char  t_areaCode[10+1];
  InputParams *pInputParams = new InputParams(1);
	AccountBill billItem;
	int length;
	//r_bills.clear(); //内部不清空
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		
  	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_accountId);
		m_session->select(AccountBillTable[r_cycle%2].TableName.c_str(),
		                  AccountBillTable[r_cycle%2].IndexName.c_str(),
		                  pInputParams,NULL,NULL);
		billItem.m_accountId = r_accountId;
		while(m_session->getNextRec())
		{
		  /*
			m_session->getColumnValue("UserId" ,&t_userId,length);billItem.m_userId=t_userId;
			m_session->getColumnValue("billId" ,&billItem.m_billId,length);
			m_session->getColumnValue("value"  ,&billItem.m_value,length);
			m_session->getColumnValue("discntValue",&billItem.m_discntValue,length);
			m_session->getColumnValue("a_discnt"  ,&billItem.m_a_discnt,length);
			m_session->getColumnValue("b_discnt"  ,&billItem.m_b_discnt,length);
			*/
			m_session->getColumnValue("USER_ID" ,&t_userId,length);billItem.m_userId=t_userId;
			//add by gaojf 2009-2-16 14:44 EPARCHY_CODE
			m_session->getColumnValue("EPARCHY_CODE",t_areaCode,length);billItem.m_areaCode=t_areaCode;
			m_session->getColumnValue("DETAIL_ITEM_CODE" ,&billItem.m_billId,length);
			m_session->getColumnValue("INIT_FEE"  ,&billItem.m_value,length);
			m_session->getColumnValue("FEE",&billItem.m_discntValue,length);
			m_session->getColumnValue("A_DISCNT"  ,&billItem.m_a_discnt,length);
			m_session->getColumnValue("ADJUST_BEFORE"  ,&billItem.m_b_discnt,length);
			m_session->getColumnValue("ACCT_ADJUST_BEFORE"  ,&billItem.m_acct_adjustBefore,length);
			billItem.m_userAdjust    = billItem.m_b_discnt-billItem.m_acct_adjustBefore;
			billItem.m_userGrpDiscnt = billItem.m_a_discnt-(billItem.m_value-billItem.m_discntValue-billItem.m_acct_adjustBefore); 
			r_bills.push_back(billItem);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

bool BillManager::getUserBill(const int &r_cycle,const T_SERVER_ID &r_userId,vector<RateDiscntBill>  &r_bills)
{
  long  t_userId = r_userId.value();
  long  t_accountId;
  InputParams *pInputParams = new InputParams(1);
	RateDiscntBill billItem;
	int length;
	//r_bills.clear(); //内部不清空
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}
		
  	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_userId);
		m_session->select(RateDiscntBillTable[r_cycle%100-1].TableName.c_str(),
		                  RateDiscntBillTable[r_cycle%100-1].IndexName.c_str(),
		                  pInputParams,NULL,NULL);
		billItem.m_userId = t_userId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("AccountId" ,&t_accountId,length);billItem.m_accountId=t_accountId;
			m_session->getColumnValue("billId" ,&billItem.m_billId,length);
			m_session->getColumnValue("value"  ,&billItem.m_value,length);
			r_bills.push_back(billItem);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

//added by wanglu 2011-2-24 begin

bool BillManager::getUserAcctDeposit(const T_SERVER_ID &r_acctId,vector<AcctDeposit> &r_acctDeposit)
{
	InputParams *pInputParams = new InputParams(1);//根据acct_id捞取
	long t_acctId = r_acctId.value();
	
	pInputParams->setValue(VAR_TYPE_LONG,(void *)&t_acctId);
	AcctDeposit acctDeposit;
	int length=0;
	try
	{
	  if(connect()==false) 
		{
		  delete pInputParams;
		  return false;
		}	
		
		m_session->select("TF_F_ACCOUNTDEPOSIT","",NULL,"acct_id=:0",pInputParams);
		acctDeposit.m_acctId = r_acctId;
		while(m_session->getNextRec())
		{
			m_session->getColumnValue("acct_balance_id",&acctDeposit.m_acctBalanceId,length);
			m_session->getColumnValue("user_id" ,&acctDeposit.m_userId,length);
			m_session->getColumnValue("deposit_code" ,&acctDeposit.m_depositCode,length);
			m_session->getColumnValue("odd_money",&acctDeposit.m_oddMoney,length);
			m_session->getColumnValue("even_money" ,&acctDeposit.m_evenMoney,length);
			m_session->getColumnValue("limit_money" ,&acctDeposit.m_limitMoney,length);
			m_session->getColumnValue("limit_mode" ,&acctDeposit.m_limitMode,length);
			m_session->getColumnValue("start_cycle_id" ,&acctDeposit.m_startCycleId,length);
			m_session->getColumnValue("end_cycle_id" ,&acctDeposit.m_endCycleId,length);
			m_session->getColumnValue("valid_tag" ,&acctDeposit.m_validTag,length); 			
						
			r_acctDeposit.push_back(acctDeposit);
		}
		delete pInputParams;
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    #endif

		disconnect();
		delete pInputParams;
		return false;
	}
	return true;
}

//end 2011-2-24

bool BillManager::updateUserBill(const int &r_cycle,const vector<UserBill> &r_bills)
{
    if(r_bills.empty()) return true;
	vector<InputParams *> pInputParamsIndexes;
	vector<InputParams *> pInputParamsOhters ;
	vector<InputParams *> pInputParamsValues ;
	
	InputParams *tmpInputParamsIndexes;
	InputParams *tmpInputParamsOhters ;
	InputParams *tmpInputParamsValues ;
	
	long *t_userId;
    vector<long *> t_userIdList;
	int i;
	vector<UserBill>::const_iterator itr;
	int length;
	try
	{
		if(connect()==false) 
		{
		  return false;
		}
		
		//Commented By Sunam 2009/8/9 9:01:21 事务控制放到应用端控制，以保证事务的完整
		//m_session->begin(UserBillTable[r_cycle%2].TableName.c_str());
		for (itr = r_bills.begin(); itr != r_bills.end(); ++itr)
		{
			//分配内存
		    tmpInputParamsIndexes = new InputParams(1);
		    tmpInputParamsOhters  = new InputParams(1);
		    //Modified By Sunam 2009/8/12 11:36:10
		    //tmpInputParamsValues  = new InputParams(2);
		    tmpInputParamsValues  = new InputParams(3);
		    t_userId = new long;
		    //保存所有已分配的内存
		    pInputParamsIndexes.push_back(tmpInputParamsIndexes);
			pInputParamsOhters.push_back(tmpInputParamsOhters);
			pInputParamsValues.push_back(tmpInputParamsValues);
			t_userIdList.push_back(t_userId);
			
			*t_userId = itr->m_userId.value();
	        tmpInputParamsIndexes->setValue(VAR_TYPE_LONG, (void *)t_userId);
			tmpInputParamsOhters->setValue(VAR_TYPE_INT,   (void *)&itr->m_billId);
			tmpInputParamsValues->setValue(VAR_TYPE_LONG,  (void *)&itr->m_value);
			tmpInputParamsValues->setValue(VAR_TYPE_LONG,  (void *)&itr->m_discntValue);
			//Added By Sunam 2009/8/12 11:36:20
			tmpInputParamsValues->setValue(VAR_TYPE_LONG,  (void *)&itr->m_adjustBefore);
		}
		m_session->update(UserBillTable[r_cycle%100-1].TableName.c_str(),UserBillTable[r_cycle%100-1].IndexName.c_str(),
					pInputParamsIndexes,m_userBillUpFields,pInputParamsValues,"billid=:0",pInputParamsOhters);
		//Commented By Sunam 2009/8/9 9:01:21 事务控制放到应用端控制，以保证事务的完整
		//m_session->commit();
		
		for( i = 0; i < pInputParamsIndexes.size(); ++i)
		{
			delete pInputParamsIndexes[i];
			delete pInputParamsOhters[i];
			delete pInputParamsValues[i];
			delete t_userIdList[i];
		}
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    	#endif

		m_session->rollBack();
		disconnect();
		for( i = 0; i < pInputParamsIndexes.size(); ++i)
		{
			delete pInputParamsIndexes[i];
			delete pInputParamsOhters[i];
			delete pInputParamsValues[i];
			delete t_userIdList[i];
		}
		return false;
	}
	return true;
}

bool BillManager::updateUserBill(const int &r_cycle,const vector<UserMonfeeBill> &r_bills)
{
   if(r_bills.empty()) return true;
	vector<InputParams *> pInputParamsIndexes;
	vector<InputParams *> pInputParamsOhters ;
	vector<InputParams *> pInputParamsValues ;
	
	InputParams *tmpInputParamsIndexes;
	InputParams *tmpInputParamsOhters ;
	InputParams *tmpInputParamsValues ;
	
	long *t_userId;
  vector<long *> t_userIdList;
	int i;
	vector<UserMonfeeBill>::const_iterator itr;
	int length;
	try
	{
		if(connect()==false) 
		{
		  return false;
		}
		
		for (itr = r_bills.begin(); itr != r_bills.end(); ++itr)
		{
		    tmpInputParamsIndexes = new InputParams(1);
		    tmpInputParamsOhters  = new InputParams(4);
		    tmpInputParamsValues  = new InputParams(5);
		    t_userId = new long;

		    pInputParamsIndexes.push_back(tmpInputParamsIndexes);
			  pInputParamsOhters.push_back(tmpInputParamsOhters);
			  pInputParamsValues.push_back(tmpInputParamsValues);
			  t_userIdList.push_back(t_userId);
			
			 *t_userId = itr->m_userId.value();
	      tmpInputParamsIndexes->setValue(VAR_TYPE_LONG, (void *)t_userId);
	      tmpInputParamsOhters->setValue(VAR_TYPE_VSTR, (void *)&itr->m_spCode);
			  tmpInputParamsOhters->setValue(VAR_TYPE_VSTR, (void *)&itr->m_spBizCode);			  	      
			  tmpInputParamsOhters->setValue(VAR_TYPE_INT,   (void *)&itr->m_itemCode);
			  tmpInputParamsOhters->setValue(VAR_TYPE_VSTR,  (void *)&itr->m_feeType);
			  			  
			  tmpInputParamsValues->setValue(VAR_TYPE_VSTR,  (void *)&itr->m_dateType);
			  tmpInputParamsValues->setValue(VAR_TYPE_LONG,  (void *)&itr->m_fee);
			  tmpInputParamsValues->setValue(VAR_TYPE_LONG,  (void *)&itr->m_discntFee);
			  tmpInputParamsValues->setValue(VAR_TYPE_INT,  (void *)&itr->m_displayType);
			  tmpInputParamsValues->setValue(VAR_TYPE_VSTR,  (void *)&itr->m_serialNumber);
			  //tmpInputParamsValues->setValue(VAR_TYPE_VSTR,  (void *)&itr->m_feeType);
		} 
		m_session->update(UserMonfeeBillTable[r_cycle%2].TableName.c_str(),UserMonfeeBillTable[r_cycle%2].IndexName.c_str(),
					pInputParamsIndexes,m_userMonfeeBillUpFields,pInputParamsValues,"((SpCode=:0) and (SpBizCode=:1) and (ItemCode=:2))",pInputParamsOhters);

		for( i = 0; i < pInputParamsIndexes.size(); ++i)
		{
			delete pInputParamsIndexes[i];
			delete pInputParamsOhters[i];
			delete pInputParamsValues[i];
			delete t_userIdList[i];
		}
		return true;
	}
	catch(Mdb_Exception &e)
	{
		//异常处理
		m_errMsg = e.GetString();
		#ifdef _DEBUG_		
		  e.toString();cout<<__FILE__<<__LINE__<<endl;
    	#endif

		m_session->rollBack();
		disconnect();
		for( i = 0; i < pInputParamsIndexes.size(); ++i)
		{
			delete pInputParamsIndexes[i];
			delete pInputParamsOhters[i];
			delete pInputParamsValues[i];
			delete t_userIdList[i];
		}
		return false;
	}
	return true;
}

bool BillManager::updateUserBill(const int &r_cycle,const vector<UserSumBill1>  &r_bills)
{
	if(r_bills.empty()) return true;
	vector<InputParams *> pInputParamsIndexes;
	vector<InputParams *> pInputParamsOhters ;
	vector<InputParams *> pInputParamsValues ;
	
	InputParams *tmpInputParamsIndexes;
	InputParams *tmpInputParamsOhters ;
	InputParams *tmpInputParamsValues ;
	
	long *t_userId;
    vector<long *> t_userIdList;
	int i;
	
	vector<UserSumBill1>::const_iterator itr;
	int length;
	try
	{
		//modified by gaojf on 20090504
		if(connect()==false) 
		{
		  return false;
		}
		
		//Commented By Sunam 2009/8/9 9:06:33 事务控制放到应用端去，以保证事务的完整
		//m_session->begin(UserSumBill1Table[r_cycle%2].TableName.c_str());
		for (itr = r_bills.begin(); itr != r_bills.end(); itr++)
		{
			//分配内存
		    tmpInputParamsIndexes = new InputParams(1);
			tmpInputParamsOhters  = new InputParams(2);
			tmpInputParamsValues  = new InputParams(2);
		    t_userId = new long;
		    //保存所有已分配的内存
		    pInputParamsIndexes.push_back(tmpInputParamsIndexes);
			pInputParamsOhters.push_back(tmpInputParamsOhters);
			pInputParamsValues.push_back(tmpInputParamsValues);
			t_userIdList.push_back(t_userId);
			
			*t_userId = itr->m_userId.value();
			tmpInputParamsIndexes->setValue(VAR_TYPE_LONG, (void *)t_userId);
			tmpInputParamsOhters->setValue