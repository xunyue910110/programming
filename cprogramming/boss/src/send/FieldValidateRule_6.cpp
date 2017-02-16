// ##########################################
// Source file : FieldValidateRule_6.cpp
// System      : Mobile Billing System
// Version     : 0.1.9
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020422
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_6.h"
#include "PreProcApplication.h"

//##ModelId=3BAAA7DB0386
bool FieldValidateRule_6::initialize(string ruleText)
{
	vector<string> tmpStrs,tmpStrs1,tmpStrs2;

	m_sequnce = 0;
	m_vectFieldMaps.clear();
	m_localCdrErrLog.clear();
	
	// rule_6;fields;01;AE;FCI=00000003,FREE_FORMAT!=800130    # 爱立信vpmn通话的主叫单
    splitStrByToken(ruleText,";",tmpStrs);

	if( tmpStrs.size() != 5 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}	
	
	m_callType  = tmpStrs[2];    // 跳过"rule_1","fields",得到"01"
	m_callType1 = tmpStrs[3]; // is "AE"
	
	// 解析"FCI=00000003,FREE_FORMAT!=800130"
	splitStrByToken(tmpStrs[4],",",tmpStrs1);
	if( tmpStrs1.size() == 2 )
	{
		// 解析"FCI=00000003"
		splitStrByToken(tmpStrs1[0],"=",tmpStrs2);
		if( tmpStrs2.size() == 2 )
		{
			m_fciFieldName  = tmpStrs2[0];   // get "FCI"
			m_fciFieldValue = tmpStrs2[1];   // get "00000003"
		}
		else
		{
			return false;	
		} 
		
		// 解析FREE_FORMAT!=800130
		splitStrByToken(tmpStrs1[1],"!=",tmpStrs2);
		if( tmpStrs2.size() == 2 )
		{
			m_freeFieldName  = tmpStrs2[0];   // get "FREE_FORMAT"
			m_freeFieldValue = tmpStrs2[1];   // get "800130"
		}
		else
		{
			return false;	
		} 	
	}
	else
	{
		return false;	
	}
	
	return true;
}

//##ModelId=3BAAAA25008F
int FieldValidateRule_6::validField(vector<FieldInfo> &theVectFieldMap)
{
    FieldInfo *aFieldInfo=0,*tmpFieldInfo=0;
    int len,fieldValue,errorCode=0;
    static string callType("CALL_TYPE");
    static string MSRN("msrn");
    static string OTHER_PARTY("other_party");

    if( (aFieldInfo=getFieldMap(theVectFieldMap,callType))==0 )  // edit by zhangy in 2002.4.22
	 	return 0;

    // 如果是主叫话单
    if( aFieldInfo->m_FieldDesValue == m_callType )            
    {
	 	m_sequnce = 0;
    	if( (tmpFieldInfo = getFieldMap(theVectFieldMap,m_fciFieldName))==0 )  //找到 FCI 字段
	 		return 0;
	 	
	 	//判断当FCI为配置值时,将该话单置为无效话单,并用m_vectFieldMap保存话单信息
	 	if( tmpFieldInfo->m_FieldDesValue == m_fciFieldValue
	 	    || tmpFieldInfo->m_FieldDesValue == "00000029" )  
	 	{
		 	// add by chenm 2004-12-9 在每一条智能网主叫话单中把变量初始化
		 	m_isVpmnCall = false;
		 	m_vpmnLongNum = "";

	 		// 如果m_vectFieldMap中的值不为空(即上一条复合话单没有正常结束),则把它输出
	 		// by chenm 2004-1-8
	 		if( m_vectFieldMaps.size() != 0 )
	 		{
	 			vector<FieldInfo> tmpVectFieldMap ;
	 		
	 			tmpVectFieldMap.clear();
	 			tmpVectFieldMap = m_vectFieldMaps;
	 			
	 			m_vectFieldMaps.clear();
	 			m_vectFieldMaps = theVectFieldMap;
	 			
	 			theVectFieldMap.clear();
	 			theVectFieldMap = tmpVectFieldMap;
	 			
	 			// 把缓存中的错误代码信息付给当前cdr的错误代码
	 			getCdrFlag(m_localCdrErrLog);
	 		}
	 		else
	 		{	
		 		m_vectFieldMaps.clear();
	 			m_vectFieldMaps = theVectFieldMap;
	 			// 记录当前的错误信息
	 			setCdrFlag(m_localCdrErrLog); 			
	 			
		 		aFieldInfo->m_FieldDesValue = "VP";
	 		}
	 		m_sequnce = 1;
	 	}	
    }
    // 如果是"AE"话单
    else if ( (aFieldInfo->m_FieldDesValue == m_callType1)
    		   && (m_sequnce != 0) )
    {
    	if( (tmpFieldInfo = getFieldMap(theVectFieldMap,m_freeFieldName))==0 )  //找到 free_format 字段
	 		return 0;
	 	
	 	//判断当free_format不是配置值时,而且"AE"在"AD"话单之后,已经取得"AD"话单中的msrn时
	 	//用m_vectFieldMap中保存话单信息代替"AE"话单输出
	 	//m_sequnce 代表该"AE"话单在复合话单中出现的顺序,1:在"AD"之前,2:在"AD"之后
	 	if( m_sequnce == 2)
		{
			if( tmpFieldInfo->m_FieldDesValue != m_freeFieldValue
	 			&& m_vectFieldMaps.size() != 0 )  
			{
				theVectFieldMap.clear();
				theVectFieldMap = m_vectFieldMaps;
				// 把缓存中的错误代码信息付给当前cdr的错误代码
	 			getCdrFlag(m_localCdrErrLog);
				m_vectFieldMaps.clear();
			}
			else
			{
				// disabled by chenm 2004-11-24 for vpmn 改造
				//m_vectFieldMaps.clear();   //如果是vpmn通话,则直接将内存中的话单清除	
				// add by chenm 2004-11-24
				// 如果是vpmn网内通话,则在m_vectFieldMap里打上vpmn标志正常输出,供后续进程识别vpmn话单
				this->setVpmnFlag();
				theVectFieldMap.clear();
				theVectFieldMap = m_vectFieldMaps;
				m_vectFieldMaps.clear();
			}
			m_sequnce = 0;
		}
		else if( m_sequnce == 1 )
		{
			if( !(tmpFieldInfo->m_FieldDesValue != m_freeFieldValue
	 			  && m_vectFieldMaps.size() != 0) )  
			{
				// disabled by chenm 2004-11-24 for vpmn 改造
				//m_vectFieldMaps.clear();   //如果是vpmn通话,则直接将内存中的话单清除	
				m_isVpmnCall = true;
			}
			++m_sequnce;	
		}  		
    }
    //如果是"AD"话单
    else if ( (aFieldInfo->m_FieldDesValue.compare("AD") == 0)
    		  && (m_sequnce != 0) ) 
    {
    	FieldInfo *msrnFieldInfo=0;
    	
	 	if( (msrnFieldInfo = getFieldMap(m_vectFieldMaps,MSRN )) != 0 )  //找到 msrn 字段    	 	
		{                                                                                  	 	
	    	//在"AD"话单中查找msrn字段                                                     	 	
	    	if( (tmpFieldInfo = getFieldMap(theVectFieldMap,MSRN )) != 0 )  //找到 msrn 字段    	 
		 	{	                                                                           	 	
		 		msrnFieldInfo->m_FieldDesValue = tmpFieldInfo->m_FieldDesValue;              	 	
		 	}	  	                                                                       	 	
	 	}
	 	
	 	//在"AD"话单中查other_party字段 add by chenm 2004-12-9               
		if( (tmpFieldInfo = getFieldMap(theVectFieldMap,OTHER_PARTY )) != 0 ) 
		{	                                                                 
			m_vpmnLongNum = tmpFieldInfo->m_FieldDesValue;                    
		}                                                                                     	 	

	 	// m_sequnce 代表该"AD"话单在复合话单中出现的顺序,1:在"AE"之前,2:在"AE"之后
	 	if( m_vectFieldMaps.size() != 0 
	 	    && m_sequnce == 2 )  
	 	{
	 		// add by chenm 2004-11-24 modified by chenm 2004-12-9
			// 如果是vpmn网内通话,则在m_vectFieldMap里打上vpmn标志,供后续进程识别vpmn话单
			if( m_isVpmnCall )
			{
				this->setVpmnFlag();
			}
				
	 		theVectFieldMap.clear();
	 		theVectFieldMap = m_vectFieldMaps;
	 		// 把缓存中的错误代码信息付给当前cdr的错误代码
	 		getCdrFlag(m_localCdrErrLog);
	 		m_vectFieldMaps.clear();
	 		m_sequnce = 0;
	 	}
	 	else if( m_sequnce == 1 )
	 	{
	 		++m_sequnce;	
	 	}	
    }
    //如果也不是是"AF"话单，说明不是主叫话单后面跟的不是复合话单
    // 则直接输出缓存中的话单
    else if ( (aFieldInfo->m_FieldDesValue.compare("AF") != 0)
              && ( m_sequnce == 1) ) 
    {
		// 把缓存中的错误代码信息付给当前cdr的错误代码
	 	cdrErrLog cdrErrLogTmp;
	 	cdrErrLogTmp.clear();
	 	
	 	setCdrFlag(cdrErrLogTmp);    // 首先记录当前话单的错误信息
	 	getCdrFlag(m_localCdrErrLog);     // 再获得缓存中的话单信息
	 	
		(PreProcApplication::getOutPutInst())->write(m_vectFieldMaps);

		getCdrFlag(cdrErrLogTmp);    // 最后再把当前话单中的错误信息置回
		
    	m_SendLog_->addMocSum();
    	m_SendLog_->addTotalCdrs();
    	m_vectFieldMaps.clear();
    	m_sequnce == 0;    	
    }
    return 0;
}

// add by chenm 2004-11-24
// 在vpmn网内通话的主叫话单中的 rate_ind 字段上打上‘v’标志
// 作为正常主叫话单输出,供后续进程判断该话单是否为vpmn网内通话
void FieldValidateRule_6::setVpmnFlag()
{
	static string RATE_IND("RATE_IND");
	static string VPMN_PLAG("v");
	static string OTHER_PARTY("other_party");
	static string THIRD_PARTY("third_party");
	FieldInfo *pFieldInfo = 0;
	FieldInfo *pFieldInfo1 = 0;
	FieldInfo aFieldInfo;
	
	if( (pFieldInfo=getFieldMap(m_vectFieldMaps,RATE_IND))==0 )
	{
//	    aFieldInfo.m_IsFormat 	= true;
		aFieldInfo.m_FieldName_ 	= RATE_IND;
		m_vectFieldMaps.push_back(aFieldInfo);
	
		pFieldInfo = getFieldMap(m_vectFieldMaps,RATE_IND);
	}
	pFieldInfo->m_FieldDesValue = VPMN_PLAG;
		
	// add by chenm 2004-12-9 将AD话单中的长号付给other_party
	if( m_vpmnLongNum != "" )
	{
		if( (pFieldInfo=getFieldMap(m_vectFieldMaps,OTHER_PARTY))!=0 )
		{
			// 将moc话单中的对方短号输出到third_party中
			if( (pFieldInfo1=getFieldMap(m_vectFieldMaps,THIRD_PARTY))==0 )
			{
//			    aFieldInfo.m_IsFormat 	= true;
				aFieldInfo.m_FieldName_ 	= THIRD_PARTY;
				m_vectFieldMaps.push_back(aFieldInfo);
			
				pFieldInfo1 = getFieldMap(m_vectFieldMaps,THIRD_PARTY);
			}
			pFieldInfo1->m_FieldDesValue = pFieldInfo->m_FieldDesValue;
			
			// 对 other_party 付长号
			pFieldInfo->m_FieldDesValue = m_vpmnLongNum;	
		}
	}
	
	
	return; 		
}

// 把缓存中的错误代码信息付给当前cdr的错误代码
void FieldValidateRule_6::getCdrFlag(const cdrErrLog & theCdrErrLog)
{
	m_cdrFlag->m_fieldName = theCdrErrLog.m_errFieldName;
	m_cdrFlag->m_errorFlag = theCdrErrLog.m_isErr;
	m_cdrFlag->m_errorCode = theCdrErrLog.m_errCode;
	return;
}

// 记录当前cdr中的错误信息
void FieldValidateRule_6::setCdrFlag(cdrErrLog & theCdrErrLog)
{
	theCdrErrLog.clear();
	theCdrErrLog.m_errFieldName = m_cdrFlag->m_fieldName;
	theCdrErrLog.m_isErr        = m_cdrFlag->m_errorFlag;
	theCdrErrLog.m_errCode      = m_cdrFlag->m_errorCode ;
	return;
}

