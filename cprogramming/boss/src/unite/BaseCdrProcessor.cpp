// ##########################################
// Source file : BaseCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020423
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "BaseCdrProcessor.h"
#include <iostream>

BaseCdrProcessor::BaseCdrProcessor()
{
	m_vectPreFix.clear();        //需要匹配的文件名前缀
	m_searchFieldPosi.clear();   //关键search字段顺序
	m_needUnite.clear();          //add by lizhao 2008-10-26
	m_alreadyUnite.clear();           //add by lizhao 2008-10-27
    m_lastCdr.clear();
    //m_notUnited.clear();
	m_pLog = Log::getInstance();
	m_cdrPool=0;
	m_flag = false;
	m_isFind = false;
	m_midCdr=false;
	m_isAddtoPool = true;
	m_changed = true;
	m_mergeFlagPosi = 0;     
	m_cdrFieldCount = 0;         //每条cdr中的总字段数
	m_cdrNum = 0;
	m_fieldNum = 0;
	m_outResult = 0;
	
	m_diffSec = 0;
	m_beginTime = 0;
	m_beginDate = 0;
    m_endTime = 0;
    m_endDate = 0;
    
    m_first_time = 0;
    m_last_time = 0;  
    m_chanNo = 0;
}
void BaseCdrProcessor::setInOutPut(InOutPut *pInOutPut)
{	
	m_pInOutPut = pInOutPut;
	m_pInOutPut->m_totalfieldcount = m_cdrFieldCount;
	m_pInOutPut->m_FlagPosi   =  m_mergeFlagPosi;
}

// 用配置文件中的信息,初始化 文件前缀 容器
void BaseCdrProcessor::setPreFix(const char * preFix)
{
	m_vectPreFix.push_back(preFix);
	return ;
}

// 用配置文件中的信息,初始化 关键字段search顺序 容器
void BaseCdrProcessor::setSearchFieldPosi(const int posi)
{
	m_searchFieldPosi.push_back(posi);
	return ;
}

void BaseCdrProcessor::setCdrToken(const char token)
{
	m_cdrToken = token;
	return ;
}

void BaseCdrProcessor::setMergeFlagPosi(const string &posi)
{
    int n = posi.find(",");
	m_mergeFlagPosi = atoi(posi.substr(0,n).c_str());
	m_diffSec = atoi(posi.substr(n+1).c_str());
	return ;
}
void BaseCdrProcessor::setMergeCdrFieldNum(const string & cdrfieldNum)
{
    string tmpcdr;
    tmpcdr = cdrfieldNum.substr(0,1);
    m_cdrNum = atoi(tmpcdr.c_str());
    tmpcdr = cdrfieldNum.substr(2,1);
    m_outResult = atoi(tmpcdr.c_str());
    tmpcdr = cdrfieldNum.substr(4);
    m_fieldNum = atoi(tmpcdr.c_str());
    if (m_cdrNum == 1)
    {
        m_flag = true;
    }
    
    return;
}
void BaseCdrProcessor::setMaxStoredCdr(const int maxCdrs )
{
	m_maxStoredCdr = maxCdrs;
	return ;
}

void BaseCdrProcessor::setMaxDelay(const string  &maxDelay )
{
    int n = maxDelay.find(".");
    if ( n < 0 )
    {
	    m_maxDelaySecond = atoi(maxDelay.c_str())*3600;
	}
	else
	{
	    string str = maxDelay.substr(0,n);
        int n = atoi(str.c_str()) * 3600;
        str = 	maxDelay.substr(n+2);
        int m = atoi(str.c_str()) * 360;
        m_maxDelaySecond = m + n;      
	}
	return ;
}

// 用配置文件中的信息,初始化 每条cdr中的总字段数
void BaseCdrProcessor::setCdrFieldsCount(const string &count)
{
	m_cdrFieldCount = atoi(count.c_str());
	return;
};

// 根据输入文件名前缀,到配置信息中查找,是否需要本处理器处理
bool BaseCdrProcessor::matchPreFix(const char * fileName)
{
	for(int i=0;i<m_vectPreFix.size();++i)
	{
		if(strncmp(fileName, m_vectPreFix[i].c_str(), m_vectPreFix[i].length()) == 0)
		{
			return true;	
		}	
	}
	return false;
}

// 处理一个输入文件

void BaseCdrProcessor::process(const char* cpFileName,ChanInfo * pChanInfo)
{
	CDR_POOL_MAPS_ITR itrPoolMaps; 
	
	bool bCdrPoolChanged = false;
	
	m_chanNo = pChanInfo->m_subChanNo;
	
	m_pInOutPut->openFile(cpFileName); //打开输入输出文件
	
	// 获取当前子通道、适合本业务的缓存池
	string strPoolType = getFileType();
	itrPoolMaps = (pChanInfo->m_cdrPoolMaps).find( strPoolType );
	if( itrPoolMaps != (pChanInfo->m_cdrPoolMaps).end() )
	{
		m_cdrPool = itrPoolMaps->second;
	}
	else
	{
		char cpErrMsg[100];
		sprintf(cpErrMsg,"ERROR(%s:%d):匹配缓冲池错,类型:%s",__FILE__,__LINE__,strPoolType.c_str());
		throw string(cpErrMsg);	
	}
	
	while( m_pInOutPut->readCdr(m_cdrBuffer,MAXCDRSIZE) ) //读取一条话单记录
	{
		//虚函数,可以在各业务处理器中做个性化处理
		FP_BEGIN(convertCdr2Fields)
		convertCdr2Fields();  //将每条cdr转换成字段后的字段值的容器m_pFieldVect
		FP_END(convertCdr2Fields)
		
		FP_BEGIN(needMerge)
		int iRet = needMerge();
		FP_END(needMerge)
		
		if( iRet == NEED_NOT_MERGE )
		{
			delete m_pFieldVect;
			m_pInOutPut->writeCdr(m_cdrBuffer);
			cout<<"output cdr3="<<m_cdrBuffer<<endl;
		} 
		else if( iRet == MERGED )
		{
			// 判断缓存是否有变化,如果有变化,则在本文件处理完毕后,把缓存中的内容输出到文件中备份
			if (m_isFind)
			{
			    bCdrPoolChanged = true;
			
			    m_pInOutPut->writeCdr(m_cdrBuffer);
			    cout<<"output cdr4="<<m_cdrBuffer<<endl;
			    m_pInOutPut->writeLogFile(getFileType(),pChanInfo->m_subChanNo,m_alreadyUnite);
			    m_alreadyUnite.clear();
			    m_pLog->addMerged();
		    }
		}
		else if( iRet == NEED_MERGE )
		{
	        bCdrPoolChanged = true;
			    m_pLog->addPushed2pool();
		}
		
		m_pLog->addTotal();	
	}
	FP_BEGIN(LastCdr)
	multimap< string, vector<string> >::iterator  last_itrCdr;	    
	multimap<string, vector<string> >::iterator  united_itaor;
	multimap<string, vector<string> >::iterator  tmp_itator;
	CDR_POOL_ITR iteor3;
	CDR_POOL tmp_Pool;
	vector<string> *tmpmidCdr=0;
	multimap<string,vector<string> >  tmpdelete;
	CDR_POOL_ITR Pool_iteor=0;
	long beginTime, overTime, minus;
	bool first_flag = false;
	string tmpCdrstr,tmpKeystr,tmpfirStr1;
	string tmplastKey;
	last_itrCdr = m_lastCdr.begin();

	while ( last_itrCdr != m_lastCdr.end() )
  {    
	    tmplastKey = last_itrCdr->first;
	    
	    if (last_itrCdr->second.size() == m_cdrFieldCount)
	    {
	        last_itrCdr->second.push_back(m_pLog->getCurrentTimeDealCdr());
	    }
	    
		  Pool_iteor = m_cdrPool->find(tmplastKey);
		  while( Pool_iteor != m_cdrPool->end())
		  {
		      tmp_Pool.insert(*Pool_iteor);
		      m_cdrPool->erase(Pool_iteor);
		      Pool_iteor = m_cdrPool->find(tmplastKey);
		  }
		  Pool_iteor = tmp_Pool.begin();
      while ( Pool_iteor != tmp_Pool.end() )
      {
          tmpmidCdr = Pool_iteor->second;
          
          beginTime = timeTosec((last_itrCdr->second)[m_beginDate],(last_itrCdr->second)[m_beginTime]);
          
          overTime  =  timeTosec((*tmpmidCdr)[m_endDate],(*tmpmidCdr)[m_endTime]);
          
          minus = beginTime - overTime;
          
          if( (minus <= m_diffSec) && (minus >= -m_diffSec) )
          {    
              if ( tmpmidCdr->size() == (m_cdrFieldCount+1) )
              {
                  string  tmprawCdr1 = tmpmidCdr->back();
                  tmpmidCdr->pop_back();
                  m_needUnite.insert(multimap<string, vector<string> >::value_type( tmplastKey, *tmpmidCdr));
                  tmpmidCdr->push_back(tmprawCdr1);
              }
              if ( (*tmpmidCdr)[m_mergeFlagPosi] == "1" )
              {
                  first_flag = true;
              }
		      if (tmpmidCdr->size() > (m_cdrFieldCount+1))
              {   
                  while(1)
                  {
                      tmpmidCdr->pop_back();
                      if (tmpmidCdr->size() == (m_cdrFieldCount+1))
                          break;
                  }
              }
              if ( first_flag )
              {
                  while(1)
                  {
                      united_itaor = m_needUnite.find(tmplastKey);
                 
                      if (united_itaor != m_needUnite.end())
                      {                  
                          m_alreadyUnite.insert(*united_itaor);
                          m_needUnite.erase(united_itaor); 
                      }
                      else
                      {
                          break;
                      }
                  }
                  if ( (last_itrCdr->second).size() == m_cdrFieldCount+1 )
                  {
                      (last_itrCdr->second).pop_back();
                      m_alreadyUnite.insert(*last_itrCdr);
                  }
                  doMergeCdr(tmpmidCdr, &(last_itrCdr->second));
                  (last_itrCdr->second).pop_back();
                  if (tmpmidCdr != NULL)
                  {
                      delete tmpmidCdr;
                      tmpmidCdr = NULL;
                  }
                  tmp_Pool.erase(Pool_iteor);
                  bCdrPoolChanged = true;
                  break;
              }
              else
              {
                  string tmptime ;
                  
                  if( (last_itrCdr->second).size() == (m_cdrFieldCount+1) )
                  {
                      tmptime = (last_itrCdr->second)[m_cdrFieldCount];
                      
                      last_itrCdr->second.pop_back();
                      
                      m_needUnite.insert(multimap<string, vector<string> >::value_type(tmplastKey, last_itrCdr->second));
                  }
                  
                  last_itrCdr->second.push_back(tmptime);
                  doMergeCdr(tmpmidCdr, &(last_itrCdr->second));
                  if (tmpmidCdr != NULL)
                  {
                      delete tmpmidCdr;
                      tmpmidCdr = NULL;
                  }
                  tmp_Pool.erase(Pool_iteor);
                  bCdrPoolChanged = true;
                  Pool_iteor = tmp_Pool.begin();
                  continue;
              }
          }
          else
          {
             Pool_iteor++;
          }
      }
      Pool_iteor = tmp_Pool.begin();
		  while( Pool_iteor != tmp_Pool.end())
		  {
		      m_cdrPool->insert(*Pool_iteor);
		      Pool_iteor++;
		  } 
		  tmp_Pool.clear();
      if (first_flag)  
      {       
              m_pLog->addMerged();     
                                             
              while(1)                                         
              {                                                                      
                  if ((last_itrCdr->second).size() == m_cdrFieldCount)
                          break;  
                  else
                      (last_itrCdr->second).pop_back();                                  
              }                                                
                 
              (last_itrCdr->second)[m_mergeFlagPosi] = "3";    
              
              string tmpDateTime = ComputeEndtime((last_itrCdr->second)[m_beginDate],(last_itrCdr->second)[m_beginTime], atoi((last_itrCdr->second)[m_endTime+1].c_str()) );
              (last_itrCdr->second)[m_endDate ] = tmpDateTime.substr(0,8);
              (last_itrCdr->second)[m_endTime ] = tmpDateTime.substr(8,6);                                       
		          tmpCdrstr = StringUtil::join(last_itrCdr->second,m_cdrToken); 
		      
              m_pInOutPut->writeCdr(tmpCdrstr.c_str());
              cout<<"output cdr5="<<tmpCdrstr<<endl;    
		          tmpdelete.insert(multimap<string, vector<string> >::value_type(tmplastKey, last_itrCdr->second));
      
		  }   
		  last_itrCdr++;  
		  first_flag = false; 
  }
  
  tmp_itator = tmpdelete.begin();
  while( tmp_itator != tmpdelete.end() )
  {
        last_itrCdr = m_lastCdr.lower_bound(tmp_itator->first);
        
        while (last_itrCdr != m_lastCdr.upper_bound(tmp_itator->first))
        {
            if (tmp_itator->second == last_itrCdr->second )
            {
                m_lastCdr.erase(last_itrCdr);
                break;
            }
            last_itrCdr++;
        }
        tmp_itator++;
  }
    
  tmpdelete.clear();
  
	FP_END(LastCdr)        
	// 在关闭输出文件前,判断缓存中的话单是否溢出,如果溢出,则直接输出
	FP_BEGIN(isOverStored)
	isOverStored();
	FP_END(isOverStored)	
	m_pInOutPut->closeFile(cpFileName);
	        
	if( bCdrPoolChanged == true )
	{  
	    FP_BEGIN(savePools2Files)
	    
		m_pInOutPut->savePools2Files(getFileType(),pChanInfo->m_subChanNo,m_cdrPool, &m_lastCdr, &m_needUnite);
		bCdrPoolChanged = false;
		
		FP_END(savePools2Files)	
	}       
	// 写已经合并的话单记录到日志文件 add by lizhao 2008-10-25
	FP_BEGIN(writeLogFile)
	m_pInOutPut->writeLogFile(getFileType(),pChanInfo->m_subChanNo,m_alreadyUnite);
	FP_END(writeLogFile)
	m_alreadyUnite.clear();
	
	return;	

} 
 
void BaseCdrProcessor::process_gprs(const char* cpFileName,ChanInfo * pChanInfo)
{
	 CDR_POOL_MAPS_ITR itrPoolMaps; 
	
	bool bCdrPoolChanged = false;
	
	m_chanNo = pChanInfo->m_subChanNo;
	
	m_pInOutPut->openFile(cpFileName); //打开输入输出文件
	
	// 获取当前子通道、适合本业务的缓存池
	string strPoolType = getFileType();
	itrPoolMaps = (pChanInfo->m_cdrPoolMaps).find( strPoolType );
	if( itrPoolMaps != (pChanInfo->m_cdrPoolMaps).end() )
	{
		m_cdrPool = itrPoolMaps->second;
	}
	else
	{
		char cpErrMsg[100];
		sprintf(cpErrMsg,"ERROR(%s:%d):匹配缓冲池错,类型:%s",__FILE__,__LINE__,strPoolType.c_str());
		throw string(cpErrMsg);	
	}
	
	while( m_pInOutPut->readCdr(m_cdrBuffer,MAXCDRSIZE) ) //读取一条话单记录
	{
		//虚函数,可以在各业务处理器中做个性化处理
		FP_BEGIN(convertCdr2Fields)
		convertCdr2Fields();  //将每条cdr转换成字段后的字段值的容器m_pFieldVect
		FP_END(convertCdr2Fields)
		
		FP_BEGIN(needMergeGprs)
		int iRet = needMergeGprs();
		FP_END(needMergeGprs)
		
		if( iRet == NEED_NOT_MERGE )
		{
			delete m_pFieldVect;
			m_pInOutPut->writeCdr(m_cdrBuffer);
		} 
		else if( iRet == MERGED )
		{
			// 判断缓存是否有变化,如果有变化,则在本文件处理完毕后,把缓存中的内容输出到文件中备份
			if (m_isFind)
			{
			    bCdrPoolChanged = true;
			
			    m_pInOutPut->writeCdr(m_cdrBuffer);
			    m_pInOutPut->writeLogFile(getFileType(),pChanInfo->m_subChanNo,m_alreadyUnite);
			    m_alreadyUnite.clear();
			    m_pLog->addMerged();
		   }
		}
		else if( iRet == NEED_MERGE )
		{
	        bCdrPoolChanged = true;
			    m_pLog->addPushed2pool();
		}
		
		m_pLog->addTotal();	
	}
      
	// 在关闭输出文件前,判断缓存中的话单是否溢出,如果溢出,则直接输出
	FP_BEGIN(isOverStored)
	isOverStored();
	FP_END(isOverStored)	
	m_pInOutPut->closeFile(cpFileName);
	        
	if( bCdrPoolChanged == true )
	{  
	    FP_BEGIN(savePools2Files)
	    
		  m_pInOutPut->savePools2Files(getFileType(),pChanInfo->m_subChanNo,m_cdrPool, &m_lastCdr, &m_needUnite);
		  bCdrPoolChanged = false;
		
		  FP_END(savePools2Files)	
	}       

	FP_BEGIN(writeLogFile)
	m_pInOutPut->writeLogFile(getFileType(),pChanInfo->m_subChanNo,m_alreadyUnite);
	FP_END(writeLogFile)
	m_alreadyUnite.clear();
	
	return;	
}        

//返回值：NEED_NOT_MERGE=0-- 不需要合并的话单,直接输出
//        NEED_MERGE    =1-- 需要合并话单,压入缓冲池,不输出
//        MERGED        =2-- 合并后的最终话单,从缓冲中提取,直接输出      
int BaseCdrProcessor::needMergeGprs()
{
	  string strMergeKey;                 
	  string strSearchKey;                
	  multimap< string, vector<string> >::iterator itaor;
	  multimap< string, vector<string> >::iterator itaor1;
	  multimap< string, vector<string> >::iterator itaor2;
	  CDR_POOL_ITR iteor;             
	  vector<string> *midFieldVect=0; 
	  vector<string> *tmpCdrVect = 0;
    vector<string>  strTmp1, strTmp2, strTmp3;
    long lastCdrtime,nowCdrtime,minusTime;
    long lastCdrtime1,nowCdrtime1,minusTime1;

	  string strMergeFlag = (*m_pFieldVect)[m_mergeFlagPosi];
	

	if( strMergeFlag == "0" )
	{
		return NEED_NOT_MERGE;	
	}
	// 首话单
	else if( strMergeFlag == "1" )  
	{
		strMergeKey = getSearchKey(); //获得查找字段索引
		
		iteor = m_cdrPool->find( strMergeKey );
		
		if( iteor != m_cdrPool->end() )
		{
			  midFieldVect = iteor->second;		
			
		    strTmp1 = *midFieldVect;
			  strTmp2 = *m_pFieldVect;
			  if (strTmp1.size() == (m_cdrFieldCount+1))              
			  {
			      strTmp1.pop_back();
			      m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp1 ));
		    }
		    m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
		    
		    m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		      
			  doMergeCdr(m_pFieldVect,midFieldVect);
			  
			  (*midFieldVect)[m_mergeFlagPosi] = "1"; 
			           
			  if( m_pFieldVect != NULL )
			  {
				  delete 	m_pFieldVect; //删除中间话单
			  }
			
		}
		else
		{			
			m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );
			cout<<"cdr1.size()="<<m_pFieldVect->size()<<endl;	
		}
		     	
		return NEED_MERGE;
	}
	// 中间话单,合并后还是放在缓存中
	else if( strMergeFlag == "2" || strMergeFlag == "F" )
	{
		strMergeKey = getSearchKey();
		
		iteor = m_cdrPool->find( strMergeKey );   
		
		if( iteor != m_cdrPool->end() )
		{
			  midFieldVect = iteor->second;		
		    strTmp1 = *midFieldVect;
			  strTmp2 = *m_pFieldVect;
			  cout<<"strTmp1.size()="<<strTmp1.size()<<"-------m_cdrFieldCount="<<m_cdrFieldCount<<endl;
			  if (strTmp1.size() == (m_cdrFieldCount+1))              
			  {
			      strTmp1.pop_back();
			      m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp1));
		    }		    
		    m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
		    m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			  doMergeCdr(midFieldVect,m_pFieldVect);
			  if ( (*midFieldVect)[m_mergeFlagPosi] == "1" )
			  {
			      (*m_pFieldVect)[m_mergeFlagPosi] = "1"; 
			  }
			  else
			  {
			      (*m_pFieldVect)[m_mergeFlagPosi] = "2";
			  }
			           
			  if( iteor->second != NULL )
			  {
				  delete 	iteor->second; //删除中间话单
			  }
			  m_cdrPool->erase(iteor);
					
			  m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );				

		}
		else
		{			
			m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );	
		}
		
		return NEED_MERGE;
	}
	// 尾话单,合并后输出到标准文件中
	else if( strMergeFlag == "3" || strMergeFlag == "S" )
	{
		  strMergeKey = getSearchKey();
	    
	    m_isFind = false;
	    
		  iteor = m_cdrPool->find( strMergeKey );
		
		  if( iteor != m_cdrPool->end() )
		  {
		      midFieldVect = iteor->second;

          strTmp2 = *m_pFieldVect;
          cout<<"strTmp1.size()="<<iteor->second->size()<<"-------m_cdrFieldCount="<<m_cdrFieldCount<<endl;
   	  	  if ( iteor->second->size() == (m_cdrFieldCount+1) )
   	  	  { 
   	  	      vector<string> tmprawCdr3 = *(iteor->second);
   	  	      tmprawCdr3.pop_back();
   	  	      m_needUnite.insert(multimap<string, vector<string> >::value_type(strMergeKey, tmprawCdr3));
   	  	  }
   	  	  
   	  	  m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
   	  	  m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
   	  	  doMergeCdr(midFieldVect,m_pFieldVect);   	  	  
   	  	         
   	  	  multimap<string, vector<string> > tmp_needUnite;
   	  	        
   	  	  itaor1 = m_needUnite.find(strMergeKey);
   	  	  while ( itaor1 != m_needUnite.end() )
   	  	  {   
   	  	      tmp_needUnite.insert(*itaor1);
   	  	      m_needUnite.erase(itaor1);
   	  	      itaor1 = m_needUnite.find(strMergeKey);
   	  	  }
		         
		      if ( (*(iteor->second))[m_mergeFlagPosi] == "1" )
		      {		           
		           m_isFind = true;
		           itaor2 = tmp_needUnite.begin();
		           while (itaor2 != tmp_needUnite.end())
		           {
		               m_alreadyUnite.insert(*itaor2);
		               itaor2++;
		           }
		           tmp_needUnite.clear();                 
		      }
		      else
		      {
		          itaor2 = tmp_needUnite.begin();
		          while (itaor2 != tmp_needUnite.end())
		          {
		              m_needUnite.insert(*itaor2);
		              itaor2++;
		          }
		          tmp_needUnite.clear();
		      }
              
		      
		      if (m_isFind)
		      {   
		         getMergedCdr();
		         if( iteor->second != NULL )
		         {
		             delete 	iteor->second;
		         }
		         m_cdrPool->erase(iteor);	
		         if( m_pFieldVect != NULL )
		         {
		             delete 	m_pFieldVect;
		         }
          }
          else
          {
          	
          	 if( iteor->second != NULL )
			       {
				       delete 	iteor->second; //删除中间话单
			       }
			       m_cdrPool->erase(iteor);
				     	
			       m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );	
          	 
		         return NEED_MERGE;
          }
		   
	    }	
	    else
		  {			
			   m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			   m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );
			   return NEED_MERGE;	
		  }	 
		  return MERGED;
	}		
	else
	{
		// 如果合并标准不是预定义的值,则不动作
		return NEED_NOT_MERGE;	
	}
}

           
//返回值：NEED_NOT_MERGE=0-- 不需要合并的话单,直接输出
//        NEED_MERGE    =1-- 需要合并话单,压入缓冲池,不输出
//        MERGED        =2-- 合并后的最终话单,从缓冲中提取,直接输出      
int BaseCdrProcessor::needMerge()
{
	  string strMergeKey;                 //话单合并关键字
	  string strSearchKey;                //查找关键字，add by lizhao 2008-10-31
	  multimap< string, vector<string> >::iterator itaor;
	  multimap< string, vector<string> >::iterator itaor1;
	  multimap< string, vector<string> >::iterator itaor2;
	  CDR_POOL_ITR iteor;             
	  vector<string> *midFieldVect=0; 
	  vector<string> *tmpCdrVect = 0;
    vector<string>  strTmp1, strTmp2, strTmp3;
    long lastCdrtime,nowCdrtime,minusTime;
    long lastCdrtime1,nowCdrtime1,minusTime1;

	  string strMergeFlag = (*m_pFieldVect)[m_mergeFlagPosi];
	

	if( strMergeFlag == "0" )
	{
		return NEED_NOT_MERGE;	
	}
	// 首话单
	else if( strMergeFlag == "1" )  
	{
		strMergeKey = getSearchKey(); //获得查找字段索引
		
		iteor = m_cdrPool->find( strMergeKey );
		
		if( iteor != m_cdrPool->end() )
		{
			midFieldVect = iteor->second;
			
			lastCdrtime = timeTosec((*midFieldVect)[m_beginDate],(*midFieldVect)[m_beginTime]);
			nowCdrtime  = timeTosec((*m_pFieldVect)[m_endDate],(*m_pFieldVect)[m_endTime]);
			
			minusTime = nowCdrtime - lastCdrtime;
			
			if ( (minusTime <= m_diffSec) && (minusTime >= -m_diffSec) )
			{
		        strTmp1 = *midFieldVect;
			    strTmp2 = *m_pFieldVect;
			    if (strTmp1.size() == (m_cdrFieldCount+1))              
			    {
			        strTmp1.pop_back();
			        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp1 ));
		        }
		        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
		    
		        m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		        
			    doMergeCdr(m_pFieldVect,midFieldVect);
			    
			    (*midFieldVect)[m_mergeFlagPosi] = "1"; 
			             
			    if( m_pFieldVect != NULL )
			    {
				    delete 	m_pFieldVect; //删除中间话单
			    }
			}
			else
			{
			    m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		        m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );
			}
		}
		else
		{			
			m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );	
		}
		     	
		return NEED_MERGE;
	}
	// 中间话单,合并后还是放在缓存中
	else if( strMergeFlag == "2" || strMergeFlag == "F" )
	{
		strMergeKey = getSearchKey();
		
		iteor = m_cdrPool->find( strMergeKey );   
		
		if( iteor != m_cdrPool->end() )
		{

			midFieldVect = iteor->second;
			
			lastCdrtime = timeTosec((*midFieldVect)[m_endDate],(*midFieldVect)[m_endTime]);
			nowCdrtime  = timeTosec((*m_pFieldVect)[m_beginDate],(*m_pFieldVect)[m_beginTime]);
			
			lastCdrtime1 = timeTosec((*midFieldVect)[m_beginDate],(*midFieldVect)[m_beginTime]);
			nowCdrtime1  = timeTosec((*m_pFieldVect)[m_endDate],(*m_pFieldVect)[m_endTime]);
			
			minusTime1 = nowCdrtime1 - lastCdrtime1;
			
			minusTime = nowCdrtime - lastCdrtime;
			
			
			if ( (minusTime <= m_diffSec) && (minusTime >= -m_diffSec) )
			{
		        strTmp1 = *midFieldVect;
			      strTmp2 = *m_pFieldVect;
			    if (strTmp1.size() == (m_cdrFieldCount+1))              
			    {
			        strTmp1.pop_back();
			        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp1));
		        }
		        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
		    
			    doMergeCdr(midFieldVect,m_pFieldVect);
			    if ( (*midFieldVect)[m_mergeFlagPosi] == "1" )
			    {
			        (*m_pFieldVect)[m_mergeFlagPosi] = "1"; 
			    }
			    else
			    {
			        (*m_pFieldVect)[m_mergeFlagPosi] = "2";
			    }
			             
			    if( iteor->second != NULL )
			    {
				    delete 	iteor->second; //删除中间话单
			    }
			    m_cdrPool->erase(iteor);
						
			    m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );	
			}
            else if ( (minusTime1 <= m_diffSec) && (minusTime1 >= -m_diffSec) )
			{
			    strTmp1 = *midFieldVect;
			    strTmp2 = *m_pFieldVect;
			    if (strTmp1.size() == (m_cdrFieldCount+1))              
			    {
			        strTmp1.pop_back();
			        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp1));
		        }
		        m_needUnite.insert(multimap<string, vector<string> >::value_type( strMergeKey, strTmp2));
		    
		        m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		        
			    doMergeCdr(m_pFieldVect,midFieldVect);
			    
			    if ( (*midFieldVect)[m_mergeFlagPosi] == "1" )
			    {
			        (*m_pFieldVect)[m_mergeFlagPosi] = "1"; 
			    }
			    else
			    {
			        (*m_pFieldVect)[m_mergeFlagPosi] = "2";
			    } 
			             
			    if( m_pFieldVect != NULL )
			    {
				    delete 	m_pFieldVect; //删除中间话单
			    }
			}
		   else
			{
			    m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		        m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );
			}	

		}
		else
		{			
			m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
			m_cdrPool->insert( CDR_POOL::value_type( strMergeKey,m_pFieldVect) );	
		}
		
		return NEED_MERGE;
	}
	// 尾话单,合并后输出到标准文件中
	else if( strMergeFlag == "3" || strMergeFlag == "S" )
	{
		strMergeKey = getSearchKey();
	    
	    m_isFind = false;
	    
		iteor = m_cdrPool->find( strMergeKey );
		
		if( iteor != m_cdrPool->end() )
		{
		    midFieldVect = iteor->second;
		    
		    lastCdrtime = timeTosec((*midFieldVect)[m_endDate],(*midFieldVect)[m_endTime]);
			nowCdrtime  = timeTosec((*m_pFieldVect)[m_beginDate],(*m_pFieldVect)[m_beginTime]);
			
			minusTime = nowCdrtime - lastCdrtime;
			
			if ( (minusTime <= m_diffSec) && (minusTime >= -m_diffSec) )
			{     
				     strTmp3 = *m_pFieldVect;
 
   		        if ( iteor->second->size() == (m_cdrFieldCount+1) )
   		        { 
   		            vector<string> tmprawCdr3 = *(iteor->second);
   		            tmprawCdr3.pop_back();
   		            m_needUnite.insert(multimap<string, vector<string> >::value_type(strMergeKey, tmprawCdr3));
   		        }
   		        
   		        multimap<string, vector<string> > tmp_needUnite;
   		        
   		        itaor1 = m_needUnite.find(strMergeKey);
   		        while ( itaor1 != m_needUnite.end() )
   		        {   
   		            tmp_needUnite.insert(*itaor1);
   		            m_needUnite.erase(itaor1);
   		            itaor1 = m_needUnite.find(strMergeKey);
   		        }
		       
		        if ( (*(iteor->second))[m_mergeFlagPosi] == "1" )
		        {
		             m_alreadyUnite.insert(multimap<string, vector<string> >::value_type(strMergeKey, *m_pFieldVect));
		             m_isFind = true;
		             itaor2 = tmp_needUnite.begin();
		             while (itaor2 != tmp_needUnite.end())
		             {
		                 m_alreadyUnite.insert(*itaor2);
		                 itaor2++;
		             }
		             tmp_needUnite.clear();
                     
		        }
		        else
		        {
		            itaor2 = tmp_needUnite.begin();
		            while (itaor2 != tmp_needUnite.end())
		            {
		                m_needUnite.insert(*itaor2);
		                itaor2++;
		            }
		            tmp_needUnite.clear();
		        }
            
		       if (m_cdrNum == 2)
		       {
		           m_midCdr = true;
		           if (!m_isFind)
		           {
		               m_needUnite.insert(multimap<string, vector<string> >::value_type(strMergeKey, *m_pFieldVect));
		           }
		           doMergeCdr(m_pFieldVect,midFieldVect);
		           m_midCdr = false;
		           tmpCdrVect = m_pFieldVect;
		           m_pFieldVect = midFieldVect;
		           (*m_pFieldVect)[m_mergeFlagPosi] = "3"; 
		       }
		       else 
		       {
		           if (!m_isFind)
		           {
		               m_needUnite.insert(multimap<string, vector<string> >::value_type(strMergeKey, *m_pFieldVect));
		           }
		           doMergeCdr(midFieldVect,m_pFieldVect);
		           (*m_pFieldVect)[m_mergeFlagPosi] = "3";  
		       }
		       if (!m_isFind)
		       {
		           m_lastCdr.insert(multimap<string, vector<string> >::value_type(strMergeKey, *m_pFieldVect));
		       }
		       else
		       {
			       // 更改m_cdrBuffer的值,输出合并成功后的话单
		           getMergedCdr();
		       }
		      			
		       if( iteor->second != NULL )
		       {
		           delete 	iteor->second;
		       }		
		       m_cdrPool->erase(iteor);
		  }
		  else
		  {
		       m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		       m_lastCdr.insert(multimap<string, vector<string> >::value_type( strMergeKey, *m_pFieldVect));
		  }
	  }
		else
		{
		    m_isFind = false;
		    strMergeKey = getSearchKey();
		    m_pFieldVect->push_back(m_pLog->getCurrentTimeDealCdr());
		    m_lastCdr.insert(multimap<string, vector<string> >::value_type( strMergeKey, *m_pFieldVect));   
		}
		if (tmpCdrVect != 0)
		{
		    delete tmpCdrVect;
		    tmpCdrVect = 0;
		}		
		else if( m_pFieldVect != NULL )
		{
			delete 	m_pFieldVect;
		}	
		// 当尾话单在缓存中找不到匹配时,就直接输出,不作为拆分话单处理
		return MERGED;
	}		
	else
	{
		// 如果合并标准不是预定义的值,则不动作
		return NEED_NOT_MERGE;	
	}
}


string BaseCdrProcessor::getSearchKey()
{ 
	string strSearKey;
	
	for(int i=0;i<m_searchFieldPosi.size();++i)
	{
		strSearKey += (*m_pFieldVect)[m_searchFieldPosi[i]];	
	}
	
	return strSearKey ;
}

string BaseCdrProcessor::getBeginKey(vector<string> record)
{
    string strBeginKey;
	
	for(int i=0;i<m_searchFieldPosi.size();++i)
	{
		strBeginKey += record[m_searchFieldPosi[i]];	
	}
	
	return strBeginKey ;
}

long BaseCdrProcessor::timeTosec(string endDate, string endTime)
{
    struct tm tmpDate;
    time_t tmp1;
    
    tmpDate.tm_year = atoi(endDate.substr(0,4).c_str()) - 1900;
    tmpDate.tm_mon  = atoi(endDate.substr(4,2).c_str()) - 1;
    tmpDate.tm_mday = atoi(endDate.substr(6,2).c_str());
    tmpDate.tm_hour = atoi(endTime.substr(0,2).c_str());
    tmpDate.tm_min  = atoi(endTime.substr(2,2).c_str());
    tmpDate.tm_sec  = atoi(endTime.substr(4,2).c_str());
    tmpDate.tm_isdst = 0;
    
    tmp1 = mktime(&tmpDate);
    
    return tmp1;
}
string BaseCdrProcessor::ComputeEndtime(string endDate, string endTime, int duration)
{
    string str1;
    time_t tmp1;
    struct tm tmpDate;
    struct tm *tmpDate1;
    
    tmpDate.tm_year = atoi(endDate.substr(0,4).c_str()) - 1900;
    tmpDate.tm_mon  = atoi(endDate.substr(4,2).c_str()) - 1;
    tmpDate.tm_mday = atoi(endDate.substr(6,2).c_str());
    tmpDate.tm_hour = atoi(endTime.substr(0,2).c_str());
    tmpDate.tm_min  = atoi(endTime.substr(2,2).c_str());
    tmpDate.tm_sec  = atoi(endTime.substr(4,2).c_str());
    tmpDate.tm_isdst = 0;
    
    tmp1 = mktime(&tmpDate);
    tmp1 = tmp1 + duration;
    
    tmpDate1 = localtime(&tmp1);
    
    int  nyear = tmpDate1->tm_year + 1900;
    int  nmonth = tmpDate1->tm_mon + 1;
    int  nday  = tmpDate1->tm_mday;
    int  nhour = tmpDate1->tm_hour;
    int  nmin  = tmpDate1->tm_min;
    int  nsec  = tm