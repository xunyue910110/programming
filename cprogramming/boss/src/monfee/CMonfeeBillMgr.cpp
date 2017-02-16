#include "CMonfeeBillMgr.h"
#include "base/Directory.h"


//-------------------------------------------------------------------
//CMonfeeBill
bool operator == (const CMonfeeBill& left,const CMonfeeBill& right)
{
    return ((left.m_spCode == right.m_spCode)&&
            (left.m_spBizCode == right.m_spBizCode)&&
            (left.m_itemCode == right.m_itemCode));
}


bool operator < (const CMonfeeBill& left,const CMonfeeBill& right)
{
	if(left.m_spCode == right.m_spCode)
	{
		if(left.m_spBizCode == right.m_spBizCode)
		{
			return (left.m_itemCode < right.m_itemCode);
		}
		else
		{
			return (left.m_spBizCode < right.m_spBizCode);
		}
	}
	else
	{
    	return (left.m_spCode < right.m_spCode);
    }
}

ostream& operator << (ostream& out, const CMonfeeBill& monfeeBill)
{
    out <<  monfeeBill.m_serialNumber     <<  '\t'    <<
            monfeeBill.m_spCode 		    <<  '\t'    <<
            monfeeBill.m_spBizCode         <<  '\t'    <<
            monfeeBill.m_itemCode         <<  '\t'    <<
            monfeeBill.m_fee              <<  '\t'    <<
            monfeeBill.m_discntFee        <<  '\t'    <<
            monfeeBill.m_displayType      <<  '\t'    <<
 //           monfeeBill.m_validTag;
            monfeeBill.m_validTag      <<  '\t'    <<       
           monfeeBill.m_feeType;//modify by yuxk for MT
    
    return out;
}  

istream& operator >> (istream& in, CMonfeeBill& monfeeBill)
{
	char buf[23];
    in >> buf;
    monfeeBill.m_serialNumber = buf;
    in >> buf;                        
	monfeeBill.m_spCode = buf;
	in >> buf;                        
	monfeeBill.m_spBizCode = buf;

    
    in >> monfeeBill.m_itemCode  >>
          monfeeBill.m_fee       >>
          monfeeBill.m_discntFee >>
          monfeeBill.m_displayType		>>
//          monfeeBill.m_validTag;
              monfeeBill.m_validTag		>>
               monfeeBill.m_feeType;//modify by yuxk for MT
    return in;
}      


//-------------------------------------------------------------------
//CUser
bool operator == (const CUser& left,const CUser& right)
{
    return ((left.m_userId==right.m_userId) && (left.m_dataType==right.m_dataType));
}


bool operator != (const CUser& left,const CUser& right)
{
    return (!(left==right));
}


bool operator < (const CUser& left,const CUser& right)
{
    if (left.m_userId == right.m_userId)
    {
        return (left.m_dataType < right.m_dataType);
    }
    else
    {
        return (left.m_userId < right.m_userId);
    }        
}


ostream& operator << (ostream& out, const CUser& user)
{
    out << user.m_userId << '\t' << user.m_dataType;
    
    return out;    
}
                    

//--------------------------------------------------------------------
//CMonfeeBillManager


int CMonfeeBillManager::initialize(const MonfeeConfig *monfeeConfig, 
                                const int& cycId)
{
    setAcctReserve(100000);
    setHeadReserve(100000);
    setDataReserve(500000);
    
    setTotalBillFilePath(monfeeConfig->m_totalBillFilePath.c_str());
    setBillLogFilePath(monfeeConfig->m_billLogFilePath.c_str());
    
    int rd = 0;    
    
    rd = check(cycId, monfeeConfig->m_channelNo);
    
    if (rd != BILL_OK)
    {
        return rd;
    }    
    
    //rd = -3 帐单初始化错误
    if (!initialization(cycId, monfeeConfig->m_channelNo))
    {
        return -3;
    }
    
    return 0;
}

    
bool CMonfeeBillManager::end()
{
    // 全量帐单内存写入文件
    if (!writeTotalBill(m_totalBillFileName.c_str()))
    {
        return false;
    }
    
    // 清空全量帐单内存和索引区
    m_userBillVector.resize(0);    
    m_userIndex.reset(m_userBillVector.begin(), m_userBillVector.end());
    MonfeeBillList::reset();
    
    // 关闭日志
    if (m_fstreamLog)
    {
        m_fstreamLog.close();
    }    
    m_isFirstOnce = true;  
    
    return true;
}


bool CMonfeeBillManager::fileInit(const char *dealDate)
{
    // 清空"帐户增量帐单内存区"
    m_monfeeBillDataMMap.clear();
    
    string dayBillFileNameBak = "";
         
    // 如果是当日首次,判断日帐单是否存在,如果存在,表明非当日首次
    if (m_isFirstOnce)
    {
        strncpy(m_day, dealDate+2 ,DAYLEN);
        m_day[DAYLEN] = 0;
        dayBillFileNameBak = m_dayBillFileNameBak + m_day;
        //文件存在
        if (!access(dayBillFileNameBak.c_str(), F_OK|R_OK))
        {
            m_isFirstOnce = false;
        }    
    }
    
    // "当日首次" 或者 "当前处理时间和传入处理日期不同"
    if (strncmp(m_dealTime, dealDate+2, DAYLEN) || m_isFirstOnce)
    {
        m_sequenceNo = 0;
        strncpy(m_day, dealDate+2 ,DAYLEN);
        m_day[DAYLEN] = 0;
        // 写全量帐单到日帐单文件中
        dayBillFileNameBak = m_dayBillFileNameBak + m_day;
        if (!writeTotalBill(dayBillFileNameBak.c_str()))
        {
            return false;
        }  
        
        // 将日志文件修改成日志备份文件
        m_fstreamLog.close();
        string logBak = m_billLogFile + "@";
        unlink(logBak.c_str());
        rename(m_billLogFile.c_str(), logBak.c_str());
        // 生成全新的日志文件及内容
        m_fstreamLog.clear();
        m_fstreamLog.open(m_billLogFile.c_str(), ios::trunc|ios::out|ios::ate);
        if (m_fstreamLog.rdstate() != ios::goodbit)
        {
            m_fstreamLog.close();
            unlink(m_billLogFile.c_str());
            return false;
        }
        m_fstreamLog << m_cycId << endl;
        m_fstreamLog << dayBillFileNameBak << endl;
        m_fstreamLog.flush();
    }
    
    // 给处理时间赋值
    strncpy(m_dealTime, dealDate + 2, DATETIMELEN);
    m_dealTime[DATETIMELEN] = 0;

    return true;        
}


bool CMonfeeBillManager::fileCommit()
{
    // 如果"帐户增量帐单区"为空,直接返回
    if (m_monfeeBillDataMMap.empty())
    {
        return true;
    }
    
    // 将内存"帐户增量帐单区"部分写入到增量文件中
    char temp[9];
    sprintf(temp,"%08d",m_sequenceNo);
    temp[BILLSEQUENCE_LEN] = 0;
    
    string tempFileName1 = m_increaseBillFileNameBak + m_day + temp;
    
    if (!writeIncreaseBill(tempFileName1.c_str()))
    {
        return false;
    }
    
    // 内存进行同步
    if (!updateTotalBill())
    {
        unlink(tempFileName1.c_str());
        return false;
    }        
    
    // 写日志文件
    m_fstreamLog << tempFileName1 <<endl;
    m_fstreamLog.flush();
    
    // 如果新增加帐户,重建索引
    if ( m_isNewUserAdd )
    {
        m_userIndex.reset(m_userBillVector.begin(), m_userBillVector.end());
    }
    
    //清除增量内存
    m_monfeeBillDataMMap.clear();
    ++m_sequenceNo ;
    
    memset(temp, 0, sizeof(temp));
    
    return true;
}


void CMonfeeBillManager::getBill(const USERID& userId,const char & dataType,
                        vector<CMonfeeBill>& acctOriginalBillVector)
{    
    acctOriginalBillVector.clear();
    
    CMonfeeBill        acctOriginalBill;
    MonfeeBillDataMMapIter    iter;
    CUser                user;
    bool                    isExistIncre = false;
    
    user.m_userId = userId;
    user.m_dataType = dataType;
   
    // 从"帐户增量帐单区"寻找帐户帐单费用    
    iter = m_monfeeBillDataMMap.lower_bound(user);
    for ( ; iter!=m_monfeeBillDataMMap.end(); ++iter)
    {
        
        if ((*iter).first.m_userId != userId ||
            (*iter).first.m_dataType != dataType)
        {
            break;
        }
        else
        {             
            // Modified by Malone start           
            //该条帐单记录有效
            /*if ((*iter).second.m_monfeeBill.m_validTag == 1)*/
            if ((*iter).second.m_monfeeBill.m_validTag == INIT_VALID_Tag_2)
            // Modified by Malone end
            {            	
                acctOriginalBill.m_serialNumber = 
                				  (*iter).second.m_monfeeBill.m_serialNumber;
                acctOriginalBill.m_spBizCode = 
                                  (*iter).second.m_monfeeBill.m_spBizCode;
                acctOriginalBill.m_spCode = 
                				  (*iter).second.m_monfeeBill.m_spCode;
                acctOriginalBill.m_itemCode = 
                                  (*iter).second.m_monfeeBill.m_itemCode;
                acctOriginalBill.m_fee = (*iter).second.m_monfeeBill.m_fee;
                acctOriginalBill.m_discntFee = 
                				  (*iter).second.m_monfeeBill.m_discntFee;
                				  
               	acctOriginalBill.m_displayType = 
                				  (*iter).second.m_monfeeBill.m_displayType;
               	acctOriginalBill.m_feeType = 
                				  (*iter).second.m_monfeeBill.m_feeType; //add by yuxk for MT           
                acctOriginalBillVector.push_back(acctOriginalBill);
                
            }
            isExistIncre = true;
        }   
    }
    
    //增量中存在，返回
    if (isExistIncre)
    {      
        return;  
    }
    
    // 从"帐户全量帐单区"寻找帐户帐单费用,并将费用提取到"帐户增量帐单区"    
    CUserBill userBill;
    userBill.m_user.m_userId = userId;
    userBill.m_user.m_dataType = dataType;
           
    
    UserBillIndexIter ait = m_userIndex.lower_bound(userBill);
    for ( ; ait != m_userIndex.end(); ++ait)
    {
    	
    	if ((*ait).m_user.m_userId != userId ||
            (*ait).m_user.m_dataType != dataType)       
        {            
            break;
        }
        else
        {
            MonfeeBillListIter bIter;
            MonfeeBillListIter eIter;
            bIter = (*ait).m_monfeeBillList.begin();
            eIter = (*ait).m_monfeeBillList.end();
        
            CMonfeeBillData monfeeBillData; 
            monfeeBillData.m_ait = ait;
         
            
            for ( ; bIter!=eIter; ++bIter)
            {                
                //该条帐单记录有效
                if ((*bIter).m_validTag == INIT_VALID_Tag_2)
                {
                    
                    acctOriginalBill.m_serialNumber = (*bIter).m_serialNumber;
                    acctOriginalBill.m_spBizCode = (*bIter).m_spBizCode;
                    acctOriginalBill.m_spCode = (*bIter).m_spCode;
                    acctOriginalBill.m_itemCode = (*bIter).m_itemCode;
                    acctOriginalBill.m_fee = (*bIter).m_fee;
                    acctOriginalBill.m_discntFee = (*bIter).m_discntFee;
                				  
		               	acctOriginalBill.m_displayType = (*bIter).m_displayType;
            		              acctOriginalBill.m_feeType = (*bIter).m_feeType;//add by yuxk for MT

                    
                    acctOriginalBillVector.push_back(acctOriginalBill);
                }
                
                //提取到增量区
                monfeeBillData.m_monfeeBill = (*bIter);           
                m_monfeeBillDataMMap.insert(
                MonfeeBillDataMMap::value_type((*ait).m_user,monfeeBillData));
            }
        }         
    }    
}

void CMonfeeBillManager::updateBill(const USERID& userId,const char& dataType,
                        vector<CMonfeeBill>& monfeeBillVector)
{
	CUser user;
	user.m_userId = userId;
	user.m_dataType = dataType;
    updateBill(user,monfeeBillVector);
}


void CMonfeeBillManager::setAcctReserve(size_t acctSize)
{
    m_acctReserve = acctSize;
}


void CMonfeeBillManager::setHeadReserve(size_t headSize)
{
    m_headReserve = headSize;
}    


void CMonfeeBillManager::setDataReserve(size_t dataSize)
{
    m_dataReserve = dataSize;
}


void CMonfeeBillManager::setBillLogFilePath(const char *filePath)
{
    m_billLogFilePath = filePath;
}


void CMonfeeBillManager::setTotalBillFilePath(const char *filePath)
{
    m_totalBillFilePath = filePath;
}


void CMonfeeBillManager::getBill(const CUser& user,
                              vector<CMonfeeBill>& monfeeBillVector)
{
    monfeeBillVector.clear();

    
    // 从"帐户增量帐单区"寻找帐户、用户帐单费用
    MonfeeBillDataMMapIter                            iter;
    pair<MonfeeBillDataMMapIter,MonfeeBillDataMMapIter> range;
    range = m_monfeeBillDataMMap.equal_range(user);
    if (range.first != range.second)
    {
        for (iter=range.first; iter!=range.second; ++iter)
        {        	
            monfeeBillVector.push_back((*iter).second.m_monfeeBill);
        }        
        return;
    }
    
    // 从"帐户全量帐单区"寻找帐户、用户帐单费用,并将费用提取到"帐户增量帐单区"    
    CUserBill acct;
    acct.m_user = user;
    UserBillIndexIter ait = m_userIndex.locate(acct);    
    if (ait != m_userIndex.end())
    {
        MonfeeBillListIter bIter;
        MonfeeBillListIter eIter;
        bIter = (*ait).m_monfeeBillList.begin();
        eIter = (*ait).m_monfeeBillList.end();
        
        CMonfeeBillData monfeeBillData; 
        monfeeBillData.m_ait = ait;
         
        for ( ; bIter!=eIter; ++bIter)
        {
            monfeeBillVector.push_back((*bIter)); 
            
            //提取到增量内存中
            monfeeBillData.m_monfeeBill = (*bIter);            
            m_monfeeBillDataMMap.insert(
                    MonfeeBillDataMMap::value_type(user,monfeeBillData));
        }         
    }
}


void CMonfeeBillManager::updateBill(const CUser& user,
                                 vector<CMonfeeBill>& monfeeBillVector)
{    
    MonfeeBillKeyMap monfeeBillKeyMap;
    monfeeBillKeyMap.clear();
    
    CMonfeeBillKey monfeeBillKey;
    vector<CMonfeeBill>::iterator  iter;    
    for (iter=monfeeBillVector.begin(); iter!=monfeeBillVector.end(); ++iter) 
    {
        
        if ((*iter).m_itemCode != 0)
        {
            monfeeBillKey.m_itemCode = (*iter).m_itemCode;
            monfeeBillKey.m_spCode = (*iter).m_spCode;
            monfeeBillKey.m_spBizCode = (*iter).m_spBizCode;
            monfeeBillKeyMap.insert(
            	MonfeeBillKeyMap::value_type(monfeeBillKey,iter));            
        }     
    }

    // 根据帐户、用户标识在"帐户增量内存区"中获取相应记录并更新
    
    pair<MonfeeBillDataMMapIter,MonfeeBillDataMMapIter> range;
    range = m_monfeeBillDataMMap.equal_range(user); 
    
    MonfeeBillKeyMapIter monfeeBillKeyMapIter;   
    
    MonfeeBillDataMMapIter iterS;
    for (iterS=range.first; iterS!=range.second; ++iterS)
    {             
    	monfeeBillKey.m_itemCode = (*iterS).second.m_monfeeBill.m_itemCode;
		monfeeBillKey.m_spCode = (*iterS).second.m_monfeeBill.m_spCode;
		monfeeBillKey.m_spBizCode = (*iterS).second.m_monfeeBill.m_spBizCode;
    	   
        //更新帐单信息
        monfeeBillKeyMapIter = monfeeBillKeyMap.find(monfeeBillKey);        
        if (monfeeBillKeyMapIter != monfeeBillKeyMap.end())
        {
        	//更新费用
            (*iterS).second.m_monfeeBill = 
                                          (*((*monfeeBillKeyMapIter).second));
            //将费用置为已更新
            (*((*monfeeBillKeyMapIter).second)).m_validTag = INIT_VALID_Tag_3;
        }
        else
        {
            //将上次出现但本次没有出现的费用置为失效
            (*iterS).second.m_monfeeBill.m_validTag = INIT_VALID_Tag_3;
            
        }    
    }
   
    // 将新增加的帐目费用插入"帐户增量内存区"中
    CMonfeeBillData monfeeBillData;
      
    if (range.first == range.second)
    {
        monfeeBillData.m_ait = m_userIndex.nit();
    }
    else
    {
        monfeeBillData.m_ait = (*range.first).second.m_ait;
    }
    
    for (iter=monfeeBillVector.begin(); iter!=monfeeBillVector.end(); ++iter)
    {        	    
        if ((*iter).m_validTag == INIT_VALID_Tag_2)
        {
        	if ((*iter).m_itemCode != 0)
        	{
        	
            	monfeeBillData.m_monfeeBill = (*iter);
            
            	m_monfeeBillDataMMap.insert(
                	MonfeeBillDataMMap::value_type(user,monfeeBillData));
            }
        }
        else
        {
        	//取消已更新标志，恢复原来的状态
        	(*iter).m_validTag = INIT_VALID_Tag_2;
        }    
    }
}


bool CMonfeeBillManager::updateTotalBill()
{
    m_isNewUserAdd = false;

    UserBillVectorIter       avIter;
    UserBillIndexIter        errIter = m_userIndex.nit();
    bool                 isDiffUser = true;    
    CUserBill                 userBill;
    
    userBill.m_user.m_userId = INVALID_USERID;
    userBill.m_user.m_dataType = '0';
    userBill.m_monfeeBillList.clear();
        
    // 从"帐户增量帐单区"读取帐户帐单记录更新"帐户全量帐单区"    
    MonfeeBillDataMMapIter beginIter = m_monfeeBillDataMMap.begin();
    MonfeeBillDataMMapIter endIter = m_monfeeBillDataMMap.end();
    for ( ; beginIter != endIter; ++beginIter)
    {
        if (userBill.m_user != (*beginIter).first)
        {
            userBill.m_user = (*beginIter).first;
            isDiffUser = true;
        }
        else
        {
            isDiffUser = false;
        }
        
        // 该记录中的帐户在"帐户全量帐单区"中不存在
        if ((*beginIter).second.m_ait == errIter)
        {
            if (isDiffUser)
            {
                m_isNewUserAdd = true;
                userBill.m_monfeeBillList.clear();
                userBill.m_monfeeBillList.push_back((*beginIter).second.m_monfeeBill);
                m_userBillVector.push_back(userBill);
                avIter = m_userBillVector.end();
                avIter-- ;
            }
            else
            {
                (*avIter).m_monfeeBillList.push_back(
                                            (*beginIter).second.m_monfeeBill);
            }        
        }
        // 该记录中的帐户在"帐户全量帐单区"中存在
        else
        {
            MonfeeBillListIter bIter;
            MonfeeBillListIter eIter; 
            bIter = (*(*beginIter).second.m_ait).m_monfeeBillList.begin();
            eIter = (*(*beginIter).second.m_ait).m_monfeeBillList.end();
            
            //更新帐户、用户信息
            (*(*beginIter).second.m_ait).m_user = userBill.m_user;
            
            // 循环判断是否存在该帐目，如果存在,更新帐目项及费用
            for ( ; bIter != eIter; ++bIter)
            {
                if ((*bIter) == (*beginIter).second.m_monfeeBill)
                {
                    (*bIter) = (*beginIter).second.m_monfeeBill;
                    
                    break;
                }
            }
            // 如果不存在该帐目项,插入新的帐目项及费用
            if (bIter == eIter)
            {
                (*(*beginIter).second.m_ait).m_monfeeBillList.
                    push_back((*beginIter).second.m_monfeeBill);
            }    
        }
    } // !for循环结束            
    
    return true;
}      


bool CMonfeeBillManager::initFromIncreaseBillFile(const char *fileName)
{
    //清空局部变量数据区
    m_monfeeBillDataMMap.clear();
    
    // 打开增量文件
    m_fstreamIncrease.close();
    m_fstreamIncrease.clear();
    m_fstreamIncrease.open(fileName,ios::in);
    if (m_fstreamIncrease.rdstate() != ios::goodbit)
    {
        m_fstreamIncrease.close();
        return false;
    }
    
    //定义局部变量
    CUser                    user;
    CMonfeeBill                    monfeeBill;
    CUser                    userOld;
    vector<CMonfeeBill>            monfeeBillVector;
    vector<CMonfeeBill>::iterator  iter;
    
    string                      strUserId = "";
    char                        cDataType = '0';
        
    user.m_userId = INVALID_USERID;
    user.m_dataType = '0';
    userOld = user;
    monfeeBillVector.clear();
    
    // 循环读取帐户帐单并进行处理    
    while (m_fstreamIncrease >>  strUserId  >> cDataType >> monfeeBill)
    {
        user.m_userId = strUserId;
        user.m_dataType = cDataType;
        
        if (user == userOld)
        {
            // 判断该帐目项是否存在,如果不存在,插入到monfeeBillVector中
            for (iter=monfeeBillVector.begin(); 
                 iter!=monfeeBillVector.end(); ++iter)
            {
                if (iter->m_itemCode == monfeeBill.m_itemCode && iter->m_spBizCode == monfeeBill.m_spBizCode && iter->m_spCode == monfeeBill.m_spCode)
                {
                    (*iter) = monfeeBill;
                    break;
                }    
            }
            
            if (iter == monfeeBillVector.end())
            {                
                monfeeBillVector.push_back(monfeeBill);
            }      
        }
        else
        {
            if (userOld.m_userId != INVALID_USERID)
            {
                updateBill(userOld,monfeeBillVector);
            }    
            
            userOld = user;
                                   
            getBill(user,monfeeBillVector);
            
            // 判断该帐目项是否存在,如果不存在,插入到monfeeBillVector中
            for (iter=monfeeBillVector.begin(); 
                 iter!=monfeeBillVector.end(); ++iter)
            {
                if (iter->m_itemCode == monfeeBill.m_itemCode && iter->m_spBizCode == monfeeBill.m_spBizCode && iter->m_spCode == monfeeBill.m_spCode)
                {
                    (*iter) = monfeeBill;
                    break;
                }    
            }
            
            if (iter == monfeeBillVector.end())
            {
                monfeeBillVector.push_back(monfeeBill);
            }             
        }    
    } // while循环结束	
    
    if (userOld.m_userId != INVALID_USERID)
    {
        updateBill(userOld,monfeeBillVector);
    }

    m_fstreamIncrease.close();
        
    // 内存帐单同步    
    if (!updateTotalBill())
    {
        return false;
    }
    
    // 如果新增加帐户,重建索引
    if (m_isNewUserAdd)
    {
        m_userIndex.reset(m_userBillVector.begin(), m_userBillVector.end());
    }
    
    // 清除"帐户增量帐单区"
    m_monfeeBillDataMMap.clear();        
    
    return true;        
}


bool CMonfeeBillManager::initFromTotalBillFile(const char *fileName)
{
    // 打开全量文件
    m_fstreamTotal.close();
    m_fstreamTotal.clear();
    m_fstreamTotal.open(fileName,ios::in);
    if ( m_fstreamTotal.rdstate() != ios::goodbit )
    {
        m_fstreamTotal.close();
        return false;
    }
    
    string          strUserId = "";
    char            cDataType = '0';
    CUser        user;
    CMonfeeBill        monfeeBill;
    CUserBill            userBill;  
    
    user.m_userId = INVALID_USERID;
    user.m_dataType = '0';
    userBill.m_user = user;
    userBill.m_monfeeBillList.clear();
    
    // 循环从全量文件中读取帐户及费用信息            
    while (m_fstreamTotal >> strUserId  >> cDataType >> monfeeBill)
    {        
        user.m_userId = strUserId;
        user.m_dataType = cDataType;
        
        if (userBill.m_user == user)
        {
            userBill.m_monfeeBillList.push_back(monfeeBill);
        }
        else
        {
            if (userBill.m_user.m_userId != INVALID_USERID)
            {
                m_userBillVector.push_back(userBill);
            }    
            userBill.m_monfeeBillList.clear();
            userBill.m_user = user;
            userBill.m_monfeeBillList.push_back(monfeeBill);
        }        
    } // while循环结束
    
    if (userBill.m_user.m_userId != INVALID_USERID)
    {
        m_userBillVector.push_back(userBill);
    }
    
    m_fstreamTotal.close();
    
    // 重建索引
    m_userIndex.reset(m_userBillVector.begin(), m_userBillVector.end());

    return true;            
}


bool CMonfeeBillManager::writeIncreaseBill(const char *fileName)
{
    MonfeeBillDataMMapIter    iter;
    string                  tempFileName;
     
    tempFileName = m_totalBillFilePath + TMP_PREFIX + "IBILL";
    //文件存在，先删除文件
    if (access(tempFileName.c_str(), F_OK) == 0)
    {
        unlink(tempFileName.c_str());   
    }
    // 打开临时输出文件
    m_fstreamIncrease.close(); 
    m_fstreamIncrease.clear(); 
    m_fstreamIncrease.open(tempFileName.c_str(), ios::out);
    if (m_fstreamIncrease.rdstate() != ios::goodbit)
    {
        m_fstreamIncrease.close();
        unlink(tempFileName.c_str());
        return false;
    }
    
    // 数据写入临时输出文件中    
    for (iter=m_monfeeBillDataMMap.begin(); 
         iter!=m_monfeeBillDataMMap.end(); ++iter)
    {
        m_fstreamIncrease <<    (*iter).first               << '\t' <<
                                (*iter).second.m_monfeeBill   << endl;                                
    }
    
    // 关闭临时输出文件流
    m_fstreamIncrease.close();
    
    // 临时输出文件搬迁到输出文件
    if (rename(tempFileName.c_str(),fileName))
    {
        unlink(tempFileName.c_str());
        return false;
    }
    return true;           
}


bool CMonfeeBillManager::writeTotalBill(const char *fileName)
{
    UserBillVectorIter        iter;
    MonfeeBillListIter      lIter;
    string                tempFileName;
     
    tempFileName = m_totalBillFilePath + TMP_PREFIX + "TBILL";
    //文件存在，先删除文件
    if (access(tempFileName.c_str(), F_OK) == 0)
    {
        unlink(tempFileName.c_str());   
    }  

    // 打开临时输出文件
    m_fstreamTotal.clear(); 
    m_fstreamTotal.open(tempFileName.c_str(), ios::out);
    if ( m_fstreamTotal.rdstate() != ios::goodbit )
    {
        m_fstreamTotal.close();
        unlink(tempFileName.c_str());
        return false;
    }
    
    // 数据写入临时输出文件中    
    for (iter=m_userBillVector.begin(); iter!=m_userBillVector.end(); ++iter)
    {
        for (lIter=(*iter).m_monfeeBillList.begin(); 
             lIter!=(*iter).m_monfeeBillList.end(); ++lIter)
        {
            m_fstreamTotal << (*iter).m_user << '\t' <<
                              (*lIter)           << endl;
        }                                                    
    }
    
    // 关闭临时输出文件流
    m_fstreamTotal.close();
    
    // 临时输出文件搬迁到输出文件
    if (rename(tempFileName.c_str(),fileName))
    {
        unlink(tempFileName.c_str());
        return false;
    }
    return true;           
}


int CMonfeeBillManager::check(const int& cycId, const int& channelNo)
{
    // 初始化日志文件和帐单文件名称
    char temp[10];
    sprintf(temp,"%03d",channelNo);
    temp[3] = 0;
    m_channelNo = temp;
    sprintf(temp,"%06d",cycId);
    temp[6] = 0;
    m_cycId = temp + m_channelNo;
    memset(temp,0,sizeof(temp));
        
    // Modified by Malone start
    /*m_billLogFile = m_billLogFilePath + "BILL.LOG";*/
    m_billLogFile = m_billLogFilePath + m_cycId + "BILL.LOG";
    // Modified by Malone end
    
    m_totalBillFileName = m_totalBillFilePath + CURRENT_TOTALBILLFILE_PREFIX
                        + m_cycId + "TOTAL.BILL";
    m_dayBillFileNameBak = m_totalBillFilePath + DAY_BILLFILE_PREFIX
                        + m_cycId;
    m_increaseBillFileNameBak = m_totalBillFilePath + REALTIME_BILLFILE_PREFIX
                        + m_cycId;

    // 在日志文件不存在情况进行处理
    if (access(m_billLogFile.c_str(), F_OK|R_OK))
    {
        string logBak = m_billLogFile + "@";
        // 日志备份文件存在,使用日志备份文件
        if (!access(logBak.c_str(), F_OK|R_OK))
        {
            if (rename(logBak.c_str(), m_billLogFile.c_str()))
            {
                return BILL_LOGERR;
            }        
        }
        // 日志备份文件不存在,生成新的日志文件
        else
        {
            m_fstreamLog.close();  
            m_fstreamLog.clear();
            m_fstreamLog.open(m_billLogFile.c_str(), 
                                ios::trunc|ios::out|ios::ate);
            if (m_fstreamLog.rdstate() != ios::goodbit)
            {
                m_fstreamLog.close();        
                return BILL_LOGERR;
            }  
            m_fstreamLog << cycId << endl ;
            m_fstreamLog.flush();
            m_fstreamLog.close();
        }         
    }
    
    // 打开日志文件
    m_fstreamLog.close();
    m_fstreamLog.clear();
    m_fstreamLog.open(m_billLogFile.c_str(), ios::in);
    if ( m_fstreamLog.rdstate() != ios::goodbit )
    {
        m_fstreamLog.close();
        return BIL