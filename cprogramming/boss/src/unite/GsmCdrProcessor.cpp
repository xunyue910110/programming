// ##########################################
// Source file : GsmCdrProcessor.cpp
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

#include "GsmCdrProcessor.h"
#include <iostream>

// 判断缓存中的数据是否达到限定的阀值,如果达到,
void GsmCdrProcessor::isOverStored()
{
	vector<string> * topPfieldVect;
	vector<string> lastcdrVect;
	multimap< string, vector<string> >::iterator itaeor;
	multimap< string, vector<string> >::iterator itaeor1;
    multimap<string, vector<string> >::iterator  tmp_itator;
    multimap< string, vector<string> >::iterator  last_itrCdr;	
	long d_timeDiff;
	CDR_POOL_ITR pool_itaeor;
	string strTopMergeKey,tmpDealTime;
	time_t tDealTime,tStarTime;
    struct tm tmStarTime;
	FP_BEGIN(m_cdrPool)
	cout<<"------------test1-------------"<<endl;	
	pool_itaeor = m_cdrPool->begin();
	while( pool_itaeor != m_cdrPool->end() )
	{
		  strTopMergeKey = (pool_itaeor)->first;
		  //取得缓存中的话单的处理时间
		  tmpDealTime = (*(pool_itaeor->second))[m_cdrFieldCount];
		
	    
	    time(&tDealTime);
	    tStarTime = atol(tmpDealTime.c_str()) ;
	    
	    d_timeDiff = tDealTime - tStarTime;
		
		if( m_cdrPool->size() > m_maxStoredCdr || d_timeDiff > m_maxDelaySecond )
		{	
			  cout<<"tDealTime="<<tDealTime<<" tStarTime="<<tStarTime<<endl;
				cout<<"m_cdrPool->size()="<<m_cdrPool->size()<<endl;
				cout<<"m_maxDelaySecond="<<m_maxDelaySecond<<" d_timeDiff="<<d_timeDiff<<endl;
		    topPfieldVect = (pool_itaeor)->second;
			
			//如果删除的是中间记录结果，则删除m_needUnite中对应的记录值 add by lizhao 2008-10-25
			string Key = (pool_itaeor)->first;
			
			if ( topPfieldVect->size() == (m_cdrFieldCount+1) )
			{
			    topPfieldVect->pop_back();
			    m_alreadyUnite.insert(multimap<string, vector<string> >::value_type( Key, *topPfieldVect));
			}
			else
			{   
                while(1)
                {
                    itaeor = m_needUnite.find(Key);
                    if (itaeor != m_needUnite.end())
                    {
                        m_alreadyUnite.insert(*itaeor);
                        m_needUnite.erase(itaeor); 
                    }
                    else
                    {
                        break;
                    }
                } 
            }
	
			if (d_timeDiff > m_maxDelaySecond)
		    {
		        if (m_outResult == 1)                //等待超时话单作为错单输出  add by lizhao 2008-12-6
			    {
			        (*topPfieldVect)[m_fieldNum]="160";
			    }
			    else if(m_outResult == 2)           //等待超时话单作为无效单输出
			    {
			        (*topPfieldVect)[m_fieldNum]="FF";
			    }
			    m_pLog->addOverTime();
		    }  
		    if ( (*topPfieldVect).size() > m_cdrFieldCount )
		    {
		        while(1)
		        {
		            topPfieldVect->pop_back();
		            if( (*topPfieldVect).size() == m_cdrFieldCount )
		                break;
		        }
		    }          
			// 超过阀值的话单,就直接输出
			string tmpDateTime = ComputeEndtime((*topPfieldVect)[m_startDatePosi],(*topPfieldVect)[m_startTimePosi], atoi((*topPfieldVect)[m_endTime+1].c_str()) );
            (*topPfieldVect)[m_endDatePosi ] = tmpDateTime.substr(0,8);
            (*topPfieldVect)[m_endTimePosi ] = tmpDateTime.substr(8,6); 
			string cdrTmp = StringUtil::join(*topPfieldVect,m_cdrToken);
			
			m_pLog->addPool2file();
			m_pInOutPut->writeCdr(cdrTmp.c_str());
			
			cout<<"output cdr1="<<cdrTmp<<endl;
			
			CDR_POOL_ITR pool_itaeor1 = pool_itaeor;
			pool_itaeor++;
			m_cdrPool->erase( pool_itaeor1 );
			
			//pool_itaeor--;
			delete topPfieldVect;
		}		
		else
		{
			pool_itaeor++;	
		}
	}
	cout<<"------------test2-------------"<<endl;		
	FP_END(m_cdrPool)
	FP_BEGIN(m_lastCdr)

	last_itrCdr = m_lastCdr.begin();
	while ( last_itrCdr != m_lastCdr.end() )
	{
		  //取得缓存中的话单的处理时间	    
		  tmpDealTime = (last_itrCdr->second)[m_cdrFieldCount];
	    time(&tDealTime);
	    tStarTime = atol(tmpDealTime.c_str()) ;
	    
	    d_timeDiff = tDealTime - tStarTime;
		
		  if( d_timeDiff > m_maxDelaySecond )
		  {	
		  	//如果删除的是中间记录结果，则删除m_needUnite中对应的记录值 add by lizhao 2008-10-25
		  	string Key = last_itrCdr->first;
		  	
		  	if ( (last_itrCdr->second).size() == (m_cdrFieldCount+1) )
		  	{
		  	    (last_itrCdr->second).pop_back();
		  	    m_alreadyUnite.insert(multimap<string, vector<string> >::value_type( Key, (last_itrCdr->second)));
		  	}
		  	else
		  	{   
                  while(1)
                  {
                      itaeor = m_needUnite.find(Key);
                      if (itaeor != m_needUnite.end())
                      {
                          m_alreadyUnite.insert(*itaeor);;
                          m_needUnite.erase(itaeor); 
                      }
                      else
                      {
                          break;
                      }
                  } 
        }
		    if (m_outResult == 1)      //等待超时话单作为错单输出  add by lizhao 2008-12-6
		  	{
		  	    (last_itrCdr->second)[m_fieldNum]="160";
		  	}
		  	else if(m_outResult == 2)           //等待超时话单作为无效单输出
		  	{
		  	    (last_itrCdr->second)[m_fieldNum]="FF";
		  	}
		  	m_pLog->addOverTime();
      
		    if ( (last_itrCdr->second).size() > m_cdrFieldCount )
		    {
		          while(1)
		          {
		              (last_itrCdr->second).pop_back();
		              if( (last_itrCdr->second).size() == m_cdrFieldCount )
		                  break;
		          }
		    }          
		  	// 超过阀值的话单,就直接输出
		  	
		  	string tmpDateTime = ComputeEndtime((last_itrCdr->second)[m_startDatePosi],(last_itrCdr->second)[m_startTimePosi], atoi((last_itrCdr->second)[m_endTime+1].c_str()) );
              (last_itrCdr->second)[m_endDatePosi ] = tmpDateTime.substr(0,8);
              (last_itrCdr->second)[m_endTimePosi ] = tmpDateTime.substr(8,6); 
              
		  	string cdrTmp = StringUtil::join((last_itrCdr->second),m_cdrToken);
		  	
		  	m_pLog->addPool2file();
		  	m_pInOutPut->writeCdr(cdrTmp.c_str());
		  	cout<<"output cdr2="<<cdrTmp<<endl;
		  	multimap< string, vector<string> >::iterator  last_itrCdr1=last_itrCdr;	
		  	last_itrCdr++;	
		  	m_lastCdr.erase(last_itrCdr1);
		  	//last_itrCdr--;
		  }		
		  else
		  {
		  	last_itrCdr++;	
		  }
	}
	cout<<"------------test3-------------"<<endl;		
  FP_END(m_lastCdr)
	return ;
}

void GsmCdrProcessor::doMergeCdr(vector<string> *midFieldVect,vector<string> *desFieldVect)
{
    //合并的结果容器是：时长 = 中间话单的时长+它本身的时长
    //                  开始日期和时间=中间话单的开始日期和时间
	int i_duration;
	string tmpDateTime;
	vector<string> * tmpFieldVect=0;
	
	i_duration = atoi( (*midFieldVect)[m_durationPosi].c_str() )
					 + atoi( (*desFieldVect)[m_durationPosi].c_str() );
					 
  		 
     if (m_cdrNum == 1)
     {   
         (*midFieldVect)[m_durationPosi] = StringUtil::toString(i_duration);
         if ( (*desFieldVect)[m_mergeFlagPosi] == "3" )
         {
              tmpDateTime = ComputeEndtime((*midFieldVect)[m_startDatePosi],(*midFieldVect)[m_startTimePosi], i_duration );
             (*midFieldVect)[m_endDatePosi ] = tmpDateTime.substr(0,8);
             (*midFieldVect)[m_endTimePosi ] = tmpDateTime.substr(8,6);
         }
         else
         {
            (*midFieldVect)[m_endDatePosi] = (*desFieldVect)[m_endDatePosi];
            (*midFieldVect)[m_endTimePosi] = (*desFieldVect)[m_endTimePosi];
         }
         
         if ( midFieldVect->size() == (m_cdrFieldCount+1) )
         {
             m_flag = true;
         }
         if ( m_flag )
         {
            (*midFieldVect).push_back("UNITED");
             m_flag = false;
         }
         if ((*desFieldVect)[m_mergeFlagPosi] == "3")
         {
            m_flag = true;
         }
         
         (*desFieldVect) = (*midFieldVect);
         return;
         
     }
     if (m_midCdr)
     {
         tmpDateTime = ComputeEndtime( (*desFieldVect)[m_startDatePosi],(*desFieldVect)[m_startTimePosi], i_duration );
         (*desFieldVect)[m_durationPosi] = StringUtil::toString(i_duration);
	     (*desFieldVect)[m_endDatePosi] = tmpDateTime.substr(0,8);
	     (*desFieldVect)[m_endTimePosi] = tmpDateTime.substr(8,6);
	     //desFieldVect->push_back((*midFieldVect)[m_cdrFieldCount]);
         (*desFieldVect).push_back("UNITED");
         return;
     }

	   (*desFieldVect)[m_durationPosi] = StringUtil::toString(i_duration);
	   
	   (*desFieldVect)[m_startDatePosi] = (*midFieldVect)[m_startDatePosi]; 
	   (*desFieldVect)[m_startTimePosi] = (*midFieldVect)[m_startTimePosi];
	   if ( (*desFieldVect)[m_mergeFlagPosi] == "3" )
     {
          tmpDateTime = ComputeEndtime((*midFieldVect)[m_startDatePosi],(*midFieldVect)[m_startTimePosi], i_duration );
         (*desFieldVect)[m_endDatePosi ] = tmpDateTime.substr(0,8);
         (*desFieldVect)[m_endTimePosi ] = tmpDateTime.substr(8,6);
     }
     else
     {
        (*desFieldVect)[m_endDatePosi] = (*desFieldVect)[m_endDatePosi];
        (*desFieldVect)[m_endTimePosi] = (*desFieldVect)[m_endTimePosi];
     }
	   if (desFieldVect->size() == m_cdrFieldCount)
	   {
	       (*desFieldVect).push_back((*midFieldVect)[m_cdrFieldCount]);
	   }
	   else
	   {
	       (*desFieldVect)[m_cdrFieldCount] = (*midFieldVect)[m_cdrFieldCount];
	   }
    (*desFieldVect).push_back("UNITED");
    
	return;
}

void GsmCdrProcessor::convertCdr2Fields()
{
	m_pFieldVect = new vector<string>();
	
	if( StringUtil::split(m_cdrBuffer,m_cdrToken,(*m_pFieldVect),true) < m_cdrFieldCount)
	{
		delete m_pFieldVect;
		throw string("记录字段数错!");	
	}	
}
	
void GsmCdrProcessor::getMergedCdr()
{
	string cdrTmp;
    
	while(1)
	{
	    if (m_pFieldVect->size() == m_cdrFieldCount )
	    {
	        break;
	    }
	    else
	    {
	        m_pFieldVect->pop_back();
	    }
	}
	cdrTmp = StringUtil::join(*m_pFieldVect,m_cdrToken);
	strcpy( m_cdrBuffer,cdrTmp.c_str() );
	
	return ;
}

bool GsmCdrProcessor::setMergeFieldPos(const string &str)
{
	int iCount;
	vector<string> tmpStrs;
	
	iCount = StringUtil::split(str,",",tmpStrs,true);
	
	if( iCount != 5)
		return false;
	
	m_startDatePosi = atoi( tmpStrs[0].c_str() );          //开始日期
	m_startTimePosi = atoi( tmpStrs[1].c_str() );          //开始时间
	m_durationPosi  = atoi( tmpStrs[2].c_str() );          //通话时长
	m_endDatePosi   = atoi( tmpStrs[3].c_str() );          //结束日期
	m_endTimePosi   = atoi( tmpStrs[4].c_str() );          //结束时间
	
	return true;		
}


bool GsmCdrProcessor::setIndexFieldPosi(const string &posi)
{
    int iCount;
  	vector<string> tmpStrs;
	
	iCount = StringUtil::split(posi,",",tmpStrs,true);
	
	if( iCount != 4)
		return false;
	
	m_beginDate = atoi( tmpStrs[0].c_str() );          //开始日期
	m_beginTime = atoi( tmpStrs[1].c_str() );          //开始时间
	m_endDate   = atoi( tmpStrs[2].c_str() );          //结束日期
	m_endTime   = atoi( tmpStrs[3].c_str() );          //结束时间
	
	return true;		
}
