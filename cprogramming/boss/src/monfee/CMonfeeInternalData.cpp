#include "CMonfeeInternalData.h"
#include "param/CMonfeeParamMgr.h"
#include "CRecvMonfee.h"
#include "CDiscntMonfee.h"
#include "base/StringUtil.h"//add by yuxk for MT
CMonfeeInternalData::CMonfeeInternalData() : m_userId(INVALID_USERID)
{
    m_monfeeParamMgr = NULL;
}


CMonfeeInternalData::~CMonfeeInternalData()
{    
    
    m_userSVCStateVector.clear();
    m_discountVector.clear();    
    m_userErrorInfoVector.clear();   
    m_userSubInfoVector.clear(); 
    m_userSubBillVector.clear();
    
    m_detailBillMap.clear();    
    m_detailBillMapOld.clear();    
    m_memBillVector.clear();   
    m_memBillVectorOld.clear();
    m_cdrVector.clear();   
    m_cdrVectorOld.clear();   
}


bool CMonfeeInternalData::initialize(const CycParam& cycPara,                                  
                                  const string& provinceCode,
                                  const string& performId,
                                  const char& fileType,
                                  CMonfeeParamMgr *monfeeParamMgr,
                                  const String<14>& dealDate,
                                  const vector<IdDiscount>& sysDiscountVector,
                           				SpRelaDiscntMap *spRelaDiscntMap,
                           				MonfeeConfig *monfeeConfig)//modify by yuxk for MT
{
    m_userId = INVALID_USERID;

    m_cycPara = cycPara;    
    m_provinceCode = provinceCode;
    m_performId = performId;
    m_fileType = fileType;
    m_monfeeParamMgr = monfeeParamMgr; 
    
    m_dealDate = dealDate;
    m_sysDiscountVector = sysDiscountVector;
    
    m_spRelaDiscntMap = spRelaDiscntMap;

    //add by yuxk for MT	
     m_monfeeConfig = monfeeConfig;
     m_unitfee_M = m_monfeeConfig->m_unitfee_M;
     if (m_unitfee_M==0) m_unitfee_M=unit_fee;
     #ifdef _DEBUG_	 
      cout<<"unitfee_M:"<<m_unitfee_M<<endl;	
     #endif 	 
     m_unitfee_T = m_monfeeConfig->m_unitfee_T;
     if (m_unitfee_T==0) m_unitfee_T=unit_fee;
     #ifdef _DEBUG_	 
      cout<<"unitfee_T:"<<m_unitfee_T<<endl;	
     #endif 	 
     m_iphoneVec.clear();
     m_iphoneVec = StringUtil::split(m_monfeeConfig->m_product_iphone,";",true);
     ::sort(m_iphoneVec.begin(),m_iphoneVec.end()); 
     #ifdef _DEBUG_	   
     for (vector<string>::iterator t_it = m_iphoneVec.begin();
     	 t_it != m_iphoneVec.end();++t_it )
     	{
     	  cout<<"product_iphone:"<<*t_it<<endl;	
     	}	
     #endif 	   	  
    //add by yuxk for MT	   
    m_currentDate = m_dealDate;    
    //获取当前天数 zhaogc add begin
    if (m_currentDate > m_cycPara.m_endTime)
    {
        m_currentDate = m_cycPara.m_endTime;
    }
    
    return true;
    
    //return (*monfeeParamMgr).getSpPunishRule(m_cycPara,m_spPunishMap);   
}


void CMonfeeInternalData::initUser(const USERID& userId,
                                vector<UserSubInfo>& userSubInfoVector)
{
    m_userId = userId;    
     
    m_ifExistUserInfo = false;
    
    m_userSVCStateVector.clear();
    m_discountVector.clear();
    m_userErrorInfoVector.clear();
    m_userSubInfoVector.clear();
    m_userSubBillVector.clear();
    m_monfeeTariffVector.clear();
    
    m_detailBillMap.clear();    
    m_detailBillMapOld.clear();    
    m_memBillVector.clear();   
    m_memBillVectorOld.clear();
    m_cdrVector.clear();   
    m_cdrVectorOld.clear();
    
    m_outBillVector.clear();
    
    m_userSubInfoVector.swap(userSubInfoVector);  
    
//    //将帐单从局部内存提取到全局内存
//    userBillManager.getBill(m_userId, m_fileType,m_memBillVectorOld);     
}


bool CMonfeeInternalData::syncMemoryBill(CMonfeeBillManager& userBillManager)
{
    //将帐单从局部内存提取到全局内存
    userBillManager.getBill(m_userId, m_fileType,m_memBillVectorOld);    

    //根据传入的用户增量帐单更新局部内存
    userBillManager.updateBill(m_userId, m_fileType, m_memBillVector);    

    //获取用户全新的全量帐单
    
    // Add by Malone 2008-1-26 start
    //zhaogc delete m_memBillVector 
    // Add by Malone 2008-1-26 end
    
    //已经是最新的账单，不用重新获取
    //userBillManager.getBill(m_userId,m_fileType,m_memBillVector);    
    return true;
}


int CMonfeeInternalData::getUserInfo(int& acctTag)
{
    //获取用户基本信息
//    m_ifExistUserInfo = (*m_monfeeParamMgr).getUserInfo(m_userId, 
//                                                              m_userInfo);    
		m_ifExistUserInfo = (*m_monfeeParamMgr).getUserInfo(m_userId, m_idInfo);
    
//    //用户资料存在
//    if (m_ifExistUserInfo)
//    {
//        m_idInfo.m_id=m_userInfo.m_userId;                   
//        m_idInfo.m_idTypeCode='0';                           
//        m_idInfo.m_brandCode=m_userInfo.m_brandCode;     
//        m_idInfo.m_eparchyCode=m_userInfo.m_eparchyCode;
//        m_idInfo.m_cityCode=m_userInfo.m_cityCode;       
//        m_idInfo.m_accountTag=m_userInfo.m_accountTag;
//        m_idInfo.m_removeTag=m_userInfo.m_removeTag;
//        //zhaogc add begin 20070705
//        m_idInfo.m_preDestroyTime=m_userInfo.m_preDestroyTime;
//        m_idInfo.m_destroyTime=m_userInfo.m_destroyTime;
//        //zhaogc add end 20070705
//        m_idInfo.m_serialNumber = m_userInfo.m_serialNumber; 
//    }
//    else
//    {        
//        IdErrorInfo   userErrorInfo;
//        //基本资料有问题
//        userErrorInfo.m_errorType = "10";
//        userErrorInfo.m_fValue = 0;
//        userErrorInfo.m_sValue = 0;
//
//        m_userErrorInfoVector.push_back(userErrorInfo);
//    }  
  
    acctTag = 0;		
    
        //用户资料不存在
    if (m_ifExistUserInfo == 0)
    {
        IdErrorInfo   userErrorInfo;
        //基本资料有问题
        userErrorInfo.m_errorType = "10";
        userErrorInfo.m_fValue = 0;
        userErrorInfo.m_sValue = 0;

        m_userErrorInfoVector.push_back(userErrorInfo);
    }    
    else
    {
    	if ( m_idInfo.m_idAttr[IdInfo::ID_ACCOUNT_TAG] == "2" || 
    			(T_DATE_TIME(m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE]) > m_cycPara.m_endTime)	)
    		acctTag = 2;
    	if (m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE] == "15" || m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE] == "14" || m_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE] == "16")
    		acctTag = 2;
    }
        
    return m_ifExistUserInfo;
}       
        
        
int CMonfeeInternalData::getUserSVCState()
{
    int returnValue = 0;
    //获取用户状态异动信息
    returnValue = (*m_monfeeParamMgr).getUserServState(m_userId, m_userSVCStateVector);
    
    if (returnValue == 0)
    {
        IdErrorInfo   userErrorInfo;
        //用户状态异动信息资料有问题
        userErrorInfo.m_errorType = "11";
        userErrorInfo.m_fValue = 0;
        userErrorInfo.m_sValue = 0;

        m_userErrorInfoVector.push_back(userErrorInfo);
    }   
    return returnValue;
}


int CMonfeeInternalData::getUserDiscount()
{    

	//获取用户优惠(包括帐前调整)信息
  return (*m_monfeeParamMgr).getUserDiscount(m_fileType,
		                                          m_idInfo,                                          
		                                          m_cycPara,
		                                          m_sysDiscountVector,
		                                          m_discountVector);            
    
}

void CMonfeeInternalData::getUserBill()
{
    
    vector<UserSubInfo>::iterator        iter;
    vector<UserDetailBill>::iterator        iterS;
    
    DetailBill detailBill;    
    
    CMonfeeBill monfeeBill;    
    OutBill			outBill;         
    
    for (iter=m_userSubBillVector.begin();
         iter!=m_userSubBillVector.end(); ++iter)
    {
        ::partial_sort ((*iter).m_userDetailBill.begin(),
                        (*iter).m_userDetailBill.end(),
                        (*iter).m_userDetailBill.end());

        monfeeBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
        monfeeBill.m_spCode = (*iter).m_spCode;
        monfeeBill.m_spBizCode = (*iter).m_spBizCode;        
        monfeeBill.m_itemCode = 0;
        monfeeBill.m_fee = 0;
        monfeeBill.m_discntFee = 0;
        monfeeBill.m_validTag= INIT_VALID_Tag_2; 
        
        outBill.m_itemCode = 0;
        outBill.m_fee = 0;
        outBill.m_discntFee = 0;
        outBill.m_displayType = '0';
        outBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
        outBill.m_ecSerialNumber = (*iter).m_ecSerialNumber;
        outBill.m_sourceType = (*iter).m_sourceType;
        outBill.m_spCode = (*iter).m_spCode;
        outBill.m_spBizCode = (*iter).m_spBizCode;
        outBill.m_spServCode = (*iter).m_spServCode;
        outBill.m_bizTypeCode = (*iter).m_bizTypeCode;
        outBill.m_validTag= INIT_VALID_Tag_2;  
	 outBill.m_feeType=INIT_FEETYPE;//add by yuxk for MT
        
//        detailBill.m_itemCode = 0;
//        detailBill.m_fee = 0;

        
        int fee = 0;
        int discntFee = 0;
        int itemCodeOld = 0;        
        int itemCodeNew = 0;        
        
        
        for (iterS=(*iter).m_userDetailBill.begin();
             iterS!=(*iter).m_userDetailBill.end(); ++iterS)
        {
            //zhaogc add 200706 begin 过滤掉0账单
            if(iterS->m_fee == 0)
            {
                continue;
            }
            //zhaogc add 200706 end
            itemCodeNew = iterS->m_itemCode;                
            
            if( (itemCodeNew == itemCodeOld) || (itemCodeOld == 0) )
            {                
//                detailBill.m_itemCode = iterS->m_itemCode;
//                detailBill.m_fee += iterS->m_fee;                
                
                if(iterS->m_discntCode == 0)
                {
                    fee += iterS->m_fee;               
                    monfeeBill.m_itemCode = iterS->m_itemCode;
                    monfeeBill.m_fee += iterS->m_fee;
                    monfeeBill.m_displayType = iterS->m_displayType;
                    monfeeBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    outBill.m_itemCode = iterS->m_itemCode;
										outBill.m_fee += iterS->m_fee;
										outBill.m_displayType = iterS->m_displayType;
										outBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    
                }
                else
                {
                    discntFee += iterS->m_fee;
                    monfeeBill.m_itemCode = iterS->m_itemCode;
                    monfeeBill.m_discntFee += iterS->m_fee;
                    monfeeBill.m_displayType = iterS->m_displayType;
                    monfeeBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    outBill.m_itemCode = iterS->m_itemCode;
										outBill.m_discntFee += iterS->m_fee;
										outBill.m_displayType = iterS->m_displayType;
										outBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    
                }   
            }       
            else    
            {    
//            	map<int,DetailBill>::iterator iterMap = 
//            			m_detailBillMap.find(detailBill.m_itemCode);  
//            	if(iterMap == m_detailBillMap.end())
//            	{            		
//            		m_detailBillMap.insert(
//            				make_pair(detailBill.m_itemCode,detailBill));
//            	}
//            	else
//            	{            		
//            		(*iterMap).second.m_fee += detailBill.m_fee;
//            	}                
//                detailBill.m_itemCode = iterS->m_itemCode;
//                detailBill.m_fee = iterS->m_fee;        
                
                m_memBillVector.push_back(monfeeBill);
                
                m_outBillVector.push_back(outBill);
                
                monfeeBill.m_fee = 0;
                monfeeBill.m_discntFee = 0;
                
                outBill.m_fee = 0;
				        outBill.m_discntFee = 0;
				        
                if(iterS->m_discntCode == 0)
                {
                    fee += iterS->m_fee;               
                    monfeeBill.m_itemCode = iterS->m_itemCode;
                    monfeeBill.m_fee = iterS->m_fee;
										monfeeBill.m_displayType = iterS->m_displayType;
										monfeeBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    outBill.m_itemCode = iterS->m_itemCode;
										outBill.m_fee += iterS->m_fee;
										outBill.m_displayType = iterS->m_displayType;
										outBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    
                }
                else
                {
                    discntFee += iterS->m_fee;
                    monfeeBill.m_itemCode = iterS->m_itemCode;
                    monfeeBill.m_discntFee = iterS->m_fee;
										monfeeBill.m_displayType = iterS->m_displayType;
										monfeeBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    outBill.m_itemCode = iterS->m_itemCode;
										outBill.m_discntFee += iterS->m_fee;
										outBill.m_displayType = iterS->m_displayType;
										outBill.m_feeType = iterS->m_feeType;//add by yuxk for MT
                    
                } 
            }               
            itemCodeOld = itemCodeNew;
        }        
        
//        map<int,DetailBill>::iterator iterMapB = 
//            m_detailBillMap.find(detailBill.m_itemCode);  
//        if(iterMapB == m_detailBillMap.end())
//        {            		
//        	m_detailBillMap.insert(
//        			make_pair(detailBill.m_itemCode,detailBill));
//        }
//        else
//        {            		
//        	(*iterMapB).second.m_fee += detailBill.m_fee;
//        }
                                  
        m_memBillVector.push_back(monfeeBill);
        
        m_outBillVector.push_back(outBill);
        
        CdrBill cdrBill;        
        cdrBill.m_fee = fee;                                 
        cdrBill.m_discntFee = fee + discntFee;  
        
        cdrBill.m_validTag = INIT_VALID_Tag_2;                        
        
        cdrBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
        cdrBill.m_ecSerialNumber = (*iter).m_ecSerialNumber;
        cdrBill.m_sourceType = (*iter).m_sourceType;
        cdrBill.m_spCode = (*iter).m_spCode;
        cdrBill.m_spBizCode = (*iter).m_spBizCode;
        cdrBill.m_spServCode = (*iter).m_spServCode;
        cdrBill.m_bizTypeCode = (*iter).m_bizTypeCode;//zhaogc add 20070717
        m_cdrVector.push_back(cdrBill);
                
    }
}

void CMonfeeInternalData::getUserBillOld()
{    
    ::partial_sort (m_memBillVectorOld.begin(),
                    m_memBillVectorOld.end(),
                    m_memBillVectorOld.end());    
    
    int itemCodeOld = 0;        
    int itemCodeNew = 0;        
    int fee = 0;
        
//    DetailBill detailBill;
//    detailBill.m_itemCode = 0;
//    detailBill.m_fee = 0;    
    
    vector<CMonfeeBill>::iterator iterS;
    vector<CdrBill>::iterator iterC;
    
    
    for (iterS=m_memBillVectorOld.begin();
         iterS!=m_memBillVectorOld.end(); ++iterS)
    {        
//        itemCodeNew = iterS->m_itemCode;                
//        
//        if( (itemCodeNew == itemCodeOld) || (itemCodeOld == 0) )
//        {            
//            detailBill.m_itemCode = iterS->m_itemCode;
//            detailBill.m_fee += iterS->m_fee;                        
//        }       
//        else    
//        {
//        	map<int,DetailBill>::iterator iterMap = 
//            		m_detailBillMapOld.find(detailBill.m_itemCode);  
//            if(iterMap == m_detailBillMapOld.end())
//            {            		
//            	m_detailBillMapOld.insert(
//            			make_pair(detailBill.m_itemCode,detailBill));
//            }
//            else
//            {            		
//            	(*iterMap).second.m_fee += detailBill.m_fee;
//            }                
//            
//            detailBill.m_itemCode = iterS->m_itemCode;
//            detailBill.m_fee = iterS->m_fee;                
//        }               
//        itemCodeOld = itemCodeNew;
        
        
        //获取old cdr        
        for (iterC=m_cdrVectorOld.begin();
              iterC!=m_cdrVectorOld.end(); ++iterC)
        {         
            if( ( (*iterC).m_spCode == (*iterS).m_spCode )&&
                ( (*iterC).m_spBizCode == (*iterS).m_spBizCode ) )
            {                
                (*iterC).m_fee += (*iterS).m_fee;
                (*iterC).m_discntFee += (*iterS).m_fee + (*iterS).m_discntFee;
                break;            
            }
        }
        if(iterC == m_cdrVectorOld.end())
        {            
            CdrBill cdrBill;        
            cdrBill.m_fee = iterS->m_fee;                                 
            cdrBill.m_discntFee = iterS->m_fee + iterS->m_discntFee;
            
            /*cdrBill.m_validTag = '2';*/
            cdrBill.m_validTag = INIT_VALID_Tag_2;                      
                                                
            cdrBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);            
            cdrBill.m_spCode = iterS->m_spCode;
            cdrBill.m_spBizCode = iterS->m_spBizCode;            
            m_cdrVectorOld.push_back(cdrBill);            
        }
              
    }
    
//    // Modified by Malone start
//    /*m_detailBillMapOld.push_back(detailBill);*/
//    if (detailBill.m_itemCode != 0)
//    {
//        map<int,DetailBill>::iterator iterMapB = 
//            m_detailBillMapOld.find(detailBill.m_itemCode);  
//        if(iterMapB == m_detailBillMapOld.end())
//        {            		
//        	m_detailBillMapOld.insert(
//        			make_pair(detailBill.m_itemCode,detailBill));
//        }
//        else
//        {            		
//        	(*iterMapB).second.m_fee += detailBill.m_fee;
//        }    
//    }
//    // Modified by Malone start             
}


void CMonfeeInternalData::discntMonfee(CDiscntMonfee& cDiscntMonfee)
{   
    cDiscntMonfee.discntMonfee( m_idInfo,m_cycPara, 
                                    m_fileType,
                                    &m_userSubBillVector,
                                    &m_discountVector,
                                    &m_userSVCStateVector,
                                    &m_userErrorInfoVector,
                                    m_monfeeParamMgr,
                                    m_spRelaDiscntMap);
}

void CMonfeeInternalData::recvMonfee(CRecvMonfee& cRecvMonfee)
{
    cRecvMonfee.recvMonfee( m_idInfo,m_cycPara,
                                    m_fileType,
                                    m_currentDate,
                                    &m_userSubInfoVector,
                                    &m_userSVCStateVector,
                                    &m_userErrorInfoVector,
                                    &m_userSubBillVector,
                                    //&m_spPunishMap,
                                    m_monfeeParamMgr);
}


void CMonfeeInternalData::checkUserBillInfo()
{
    return;
}


void CMonfeeInternalData::addupInfo(AccountAddupInfo& accountAddupInfo)
{
    vector<CMonfeeBill>::iterator iter;    

    //累计用户总数和错误用户总数
    ++accountAddupInfo.m_idNum ;
    if (m_userErrorInfoVector.size() > 0)
    {
        ++accountAddupInfo.m_errorIdNum ;
    }

    //对费用进行累计
    for (iter=m_memBillVector.begin();
         iter!=m_memBillVector.end(); ++iter)
    {        
        accountAddupInfo.m_feeSum += (*iter).m_fee;
        accountAddupInfo.m_afeeSum += (*iter).m_discntFee;        
    }
}


void CMonfeeInternalData::out(const int& fileType, ostream& os, const string& outputFileName,
                           int& recordNum, int& feeSum)
{  
    //输出包月费用明细帐单文件
//    if (fileType == WORK_FILE_D)
//    {
//        outDBill(os, recordNum, feeSum);
//    }
//    //输出SP费用清单文件
//    else if (fileType == WORK_FILE_P)
//    {
//        outPBill(os, recordNum, feeSum);
//    }
//    //ADC费用清单文件
//    else if (fileType == WORK_FILE_I)
//    {
//        outIBill(os, recordNum, feeSum);
//    }
    //包月费用帐单文件
//    else if (fileType == WORK_FILE_B)
		if (fileType == WORK_FILE_B)
    {
        outBBill(os, outputFileName, recordNum, feeSum);
    }
    //包月费用帐单文件
    else if (fileType == WORK_FILE_N)
    {
        outUserErrorInfo(os, recordNum, feeSum);        
    }    
}

void CMonfeeInternalData::outDBill(ostream& os,int& recordNum,int& feeSum)
{
    recordNum = 0;
    feeSum = 0;

    vector<UserSubInfo>::iterator    iter;
    for (iter=m_userSubBillVector.begin();
         iter!=m_userSubBillVector.end(); ++iter)
    {
        vector<UserDetailBill>::iterator    iterB;
        for(iterB= (*iter).m_userDetailBill.begin();          
            iterB!=(*iter).m_userDetailBill.end(); ++iterB)
        {            
            ++recordNum;
            feeSum += (*iterB).m_fee;
            
            os << m_idInfo.m_idAttr[IdInfo::ID_ATTR_ID] << FIELD_SPLIT <<
                  m_fileType          << FIELD_SPLIT <<
                  (*iter).m_spCode        << FIELD_SPLIT <<
                  (*iter).m_spBizCode   << FIELD_SPLIT <<
                  (*iterB).m_itemCode   << FIELD_SPLIT <<
                  (*iterB).m_fee        << FIELD_SPLIT <<
                  (*iterB).m_discntCode << FIELD_SPLIT <<
                  (*iterB).m_discntType << endl;
           }
    }
}

void CMonfeeInternalData::outPBill(ostream& os,int& recordNum,int& feeSum)
{
    if( (m_fileType ==  FILE_TYPE_IN_M ) ||(m_fileType ==  FILE_TYPE_IN_H ) )
    {   
        outCdr(os,recordNum,feeSum,m_fileType);
    }
}

void CMonfeeInternalData::outIBill(ostream& os,int& recordNum,int& feeSum)
{
    if( (m_fileType ==  FILE_TYPE_IN_A )||(m_fileType ==  FILE_TYPE_IN_B ) )
    {
        outCdr(os,recordNum,feeSum,m_fileType);
    }    
}

void CMonfeeInternalData::outCdr(ostream& os,int& recordNum,
                                 int& feeSum,const char & fileType)
{
    char userType = '1';    
    if(fileType == FILE_TYPE_IN_A)
    {
        userType = '0';
    }
    
    vector<CdrBill> outputCdrVector;
    outputCdrVector.clear();
    vector<CdrBill>::iterator    iter;

    for(iter=m_cdrVectorOld.begin();
        iter!=m_cdrVectorOld.end();++iter)
    {            
        vector<CdrBill>::iterator iterC = 
        ::find(m_cdrVector.begin(),m_cdrVector.end(),(*iter) );        
        if(iterC != m_cdrVector.end())
        {  
            if( ((*iterC).m_fee == (*iter).m_fee) && 
                ((*iterC).m_discntFee == (*iter).m_discntFee) )
            {
                //费用没有发生变化，不输出
                (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
                continue;
            }
            else
            {
                //费用发生了变化
                (*iter).m_validTag = '1';//重做标志
                (*iter).m_fee = - ((*iter).m_fee);
                (*iter).m_discntFee = - ((*iter).m_discntFee);
                (*iter).m_bizTypeCode = (*iterC).m_bizTypeCode;//zhaogc add 20070717
                outputCdrVector.push_back(*iter);
                
                (*iterC).m_validTag = '0';//普通记录
                outputCdrVector.push_back(*iterC); 
                (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
            }
        }
        else
        {
            //老记录被删除了
            (*iter).m_validTag = '1';//重做标志
            (*iter).m_fee = - ((*iter).m_fee);
            (*iter).m_discntFee = - ((*iter).m_discntFee);
            outputCdrVector.push_back(*iter);
        }
    }
    
    for(iter= outputCdrVector.begin();          
        iter!=outputCdrVector.end(); ++iter)
    { 
        ++recordNum;
        feeSum += (*iter).m_fee;
                               
        os << m_cycPara.m_cycleId       << FIELD_SPLIT_OUT <<
              userType                  << FIELD_SPLIT_OUT <<
              m_idInfo.m_idAttr[IdInfo::ID_BRAND_CODE]    << FIELD_SPLIT_OUT <<
              m_idInfo.m_idAttr[IdInfo::ID_MSISDN] << FIELD_SPLIT_OUT <<
              (*iter).m_bizTypeCode       << FIELD_SPLIT_OUT << //zhaogc add 20070717
              (*iter).m_ecSerialNumber    << FIELD_SPLIT_OUT <<                                    
              (*iter).m_sourceType        << FIELD_SPLIT_OUT <<
              (*iter).m_spCode              << FIELD_SPLIT_OUT <<
              (*iter).m_spServCode        << FIELD_SPLIT_OUT <<
              (*iter).m_spBizCode         << FIELD_SPLIT_OUT <<
              (*iter).m_fee            << FIELD_SPLIT_OUT <<
              (*iter).m_discntFee      << FIELD_SPLIT_OUT <<
              m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE] << FIELD_SPLIT_OUT <<
              m_currentDate                   << FIELD_SPLIT_OUT <<
              (*iter).m_validTag        << endl;
    }
    
    for(iter= m_cdrVector.begin();          
        iter!=m_cdrVector.end(); ++iter)
    {           
        if((*iter).m_validTag == INIT_VALID_Tag_2)//没有处理到的记录，一定是新增记录                     
        {
            if((*iter).m_fee == 0 && (*iter).m_discntFee == 0)
            {
                continue;
            }
            ++recordNum;
            feeSum += (*iter).m_fee;
                               
            os << m_cycPara.m_cycleId       << FIELD_SPLIT_OUT <<
                  userType                  << FIELD_SPLIT_OUT <<
                  m_idInfo.m_idAttr[IdInfo::ID_BRAND_CODE]    << FIELD_SPLIT_OUT <<
                  m_idInfo.m_idAttr[IdInfo::ID_MSISDN] << FIELD_SPLIT_OUT <<
                  (*iter).m_bizTypeCode       << FIELD_SPLIT_OUT << //zhaogc add 20070717
                  (*iter).m_ecSerialNumber    << FIELD_SPLIT_OUT <<                                    
                  (*iter).m_sourceType        << FIELD_SPLIT_OUT <<
                  (*iter).m_spCode              << FIELD_SPLIT_OUT <<
                  (*iter).m_spServCode        << FIELD_SPLIT_OUT <<
                  (*iter).m_spBizCode         << FIELD_SPLIT_OUT <<
                  (*iter).m_fee            << FIELD_SPLIT_OUT <<
                  (*iter).m_discntFee      << FIELD_SPLIT_OUT <<
                  m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE]  << FIELD_SPLIT_OUT <<
                  m_currentDate                << FIELD_SPLIT_OUT <<
                  '0'          << endl;            
        }
    }          
}

void CMonfeeInternalData::outBBill(ostream& os, const string& outputFileName, int& recordNum,int& feeSum)
{
//    map<int,DetailBill>::iterator    iter;
//    map<int,DetailBill>::iterator    iterS;
//    for(iter=m_detailBillMapOld.begin();
//        iter!=m_detailBillMapOld.end();++iter)
//    {        
//           
//        iterS = m_detailBillMap.find((*iter).first);                           
//                           
//        if(iterS == m_detailBillMap.end())                
//        {
//            (*iter).second.m_fee = 0;
//            m_detailBillMap.insert((*iter));
//        }
//    }
//    
//    for(iter= m_detailBillMap.begin();          
//        iter!=m_detailBillMap.end(); ++iter)
//    {   
//        if((*iter).second.m_itemCode == 0 && (*iter).second.m_fee == 0)
//        {
//            continue;
//        }
//        ++recordNum;
//        feeSum += (*iter).second.m_fee;
//        
//        os << m_userInfo.m_userId << FIELD_SPLIT <<              
//              (*iter).second.m_itemCode   << FIELD_SPLIT <<
//              (*iter).second.m_fee        << endl;
//              
//    } 

    char userType = '1';    
    if(m_fileType == FILE_TYPE_IN_A)
    {
        userType = '0';
    }
    
    OutBill		outputBill;
    vector<OutBill> outputBillVector;
    outputBillVector.clear();
    //add by yuxk for MT
    vector<OutBill> outputBillVectorMT;
    outputBillVectorMT.clear();
    //add by yuxk for MT
    vector<CMonfeeBill>::iterator iter;
    vector<OutBill>::iterator iterC;

    for(iter=m_memBillVectorOld.begin();
        iter!=m_memBillVectorOld.end();++iter)
    {            
        outputBill.m_spCode = (*iter).m_spCode;
        outputBill.m_spBizCode = (*iter).m_spBizCode;
        outputBill.m_itemCode = (*iter).m_itemCode;
        outputBill.m_serialNumber = T_MSISDN(m_idInfo.m_idAttr[IdInfo::ID_MSISDN]);
       	outputBill.m_displayType = (*iter).m_displayType;
              outputBill.m_feeType = (*iter).m_feeType;//add by yuxk for MT
        iterC = ::find(m_outBillVector.begin(),m_outBillVector.end(),outputBill );        
        if(iterC != m_outBillVector.end())//modify by yuxk for MT
        {  
            if( ((*iterC).m_fee == (*iter).m_fee) && 
                ((*iterC).m_discntFee == (*iter).m_discntFee) &&
                ((*iterC).m_feeType == (*iter).m_feeType))
            {
                //费用没有发生变化，不输出
                (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
                continue;
            }
            else
            {
            	if ((*iterC).m_feeType == (*iter).m_feeType)
            		{
            		(*iterC).m_fee = (*iterC).m_fee - (*iter).m_fee;
	              (*iterC).m_discntFee = (*iterC).m_discntFee - (*iter).m_discntFee;
	              
                outputBillVector.push_back(*iterC);
                
                (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
              }
              else
              {
            		(*iterC).m_fee = (*iterC).m_fee;
	              (*iterC).m_discntFee = (*iterC).m_discntFee;
	              (*iterC).m_feeType = (*iterC).m_feeType;
                outputBillVector.push_back(*iterC);
            		(*iterC).m_fee = - ((*iter).m_fee);
	              (*iterC).m_discntFee = - ((*iter).m_discntFee);
	              (*iterC).m_feeType = (*iter).m_feeType;
                outputBillVectorMT.push_back(*iterC);
                
                (*iterC).m_validTag = INIT_VALID_Tag_3;//标志这条详单已经处理
              	
              	}	
            }
        }
        else
        {
            outputBill.m_serialNumber = (*iter).m_serialNumber;
            outputBill.m_spCode = (*iter).m_spCode;
            outputBill.m_spBizCode = (*iter).m_spBizCode;
            outputBill.m_itemCode = (*iter).m_itemCode;
            outputBill.m_fee = - ((*iter).m_fee);
            outputBill.m_discntFee = - ((*iter).m_discntFee);
            outputBill.m_displayType = (*iter).m_displayType;
            outputBill.m_feeType = (*iter).m_feeType;	//add by yuxk for MT		
            
            outputBillVector.push_back(outputBill);
        }
    }
    
    for(iterC= outputBillVector.begin();          
        iterC!=outputBillVector.end(); ++iterC)
    { 
        ++recordNum;
        feeSum += (*iterC).m_fee;
//modify by yuxk for MT
	          bool isiphone=false;
	          int unitfee_MT=1;
	          char feetype=(*iterC).m_feeType;
      			strstream ss;
       			string t_prod;
       			ss << m_idInfo.m_idAttr[IdInfo::ID_PRODUCT_ID] ;
       			ss >> t_prod;		
       	    isiphone=binary_search(m_iphoneVec.begin(), m_iphoneVec.end(),t_prod);
       	    if (isiphone &&  (*iterC).m_feeType!=INIT_FEETYPE)
                	{
                		feetype=INIT_FEETYPE;
                		if ((*iterC).m_feeType=='M')
                		{
                			unitfee_MT=m_unitfee_M;
                		}
                		else
                			{
                				unitfee_MT=m_unitfee_T;
                			}	
                	 }
                		             os << SOURCE_TYPE								<< FIELD_SPLIT_OUT <<
        			BIZ_TYPE								<< FIELD_SPLIT_OUT <<
        			FID								<< FIELD_SPLIT_OUT <<
        			RR_FLAG								<< FIELD_SPLIT_OUT <<
        			m_cycPara.m_cycleId       << FIELD_SPLIT_OUT <<
              userType                  << FIELD_SPLIT_OUT <<
              m_idInfo.m_idAttr[IdInfo::ID_BRAND_CODE]   << FIELD_SPLIT_OUT <<
              m_idInfo.m_idAttr[IdInfo::ID_ATTR_ID]		<< FIELD_SPLIT_OUT <<
              (*iterC).m_serialNumber << FIELD_SPLIT_OUT <<
              (*iterC).m_bizTypeCode       << FIELD_SPLIT_OUT <<
              (*iterC).m_ecSerialNumber    << FIELD_SPLIT_OUT <<                                    
              (*iterC).m_sourceType        << FIELD_SPLIT_OUT <<
              (*iterC).m_spCode              << FIELD_SPLIT_OUT <<
              (*iterC).m_spServCode        << FIELD_SPLIT_OUT <<
              //(*iterC).m_spBizCode         << FIELD_SPLIT_OUT <<
