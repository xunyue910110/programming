#include "MonthEndOutputFile.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include <unistd.h>
#include <math.h>
#include <algorithm>

extern ErrorMessages *const theErrorMessages;
extern log4cpp::Logger *theLogger;


MonthEndOutputFile::MonthEndOutputFile()
{
	m_batchNo 				= 0;
	m_UnacctUserFee 	= 0;
	m_IntegrateFeeSum = 0;
	m_unAcctRecords   = 0;
	m_integrateBillRecords = 0;
	
 	m_invalidRecords  = 0; //add by xuf 2010-1-7 17:23:10
  	m_noInfoFeeSum    = 0;   
}



MonthEndOutputFile::~MonthEndOutputFile()
{
}


bool MonthEndOutputFile::init(const string& performId,
                              const MonthEndConfig* monthEndConfig,
                               const vector <channelSt> channelStVec,
                               const int &r_batch,
                               const string& bcycDate, 
                               const string& dealTime,
                               const vector<string>& actEparchyCode)
{
	vector<string>::const_iterator iter;
	m_outputFileInfoMap.clear(); 
	m_channelMap.clear();
	m_batchMap.clear();
	m_performId = performId;
	m_channelStVec = channelStVec;	
	m_actEparchyCode = actEparchyCode;
	 	
	for(iter = m_actEparchyCode.begin(); iter != m_actEparchyCode.end(); ++iter)
	{
    	string fileNameHead = "S" + *iter + bcycDate.substr(0,6);
    	string fileNameTail = dealTime.substr(2);

    	getPartitionId(r_batch);
    	 
    	char   tmp[4];
    	memset (tmp, 0, sizeof(tmp));        
    	string strChannelNo="";
    	string l_startPartId="";
    	string l_endPartId="";
    	
    	//生成序列号---
    	char    batchNo[8+1];
    	string  strBacthNo;
    	memset(batchNo, 0, sizeof(batchNo));
    	
    	sprintf(batchNo, "%08d", m_batchNo);
    	strBacthNo = batchNo;
    	m_batchNo++;
    	memset(batchNo, 0, sizeof(batchNo));
    	//---

    	
		   		
    	//初始化输出结构信息    
    	for (int i=m_chanBacthPartition.begin()->first; i<=(--m_chanBacthPartition.end())->first; i++)
    	{
    		   //strChannelNo不足2位，前面补零
    	    sprintf(tmp, "%02d", i);
    	    strChannelNo = tmp;
    	    memset (tmp, 0, sizeof(tmp));
    	    
    	
    	
    	   for (int j=0;j<MAX_OUT_BATCHNUM; j++)
    	   {
    	   		OutputFileInfo tempFileInfo;
    	    	resetFileTag(tempFileInfo);
    	    	ofstream tempof;
    	    	
    	           
    	     	if(!getPartitionIdByNo(i,j,l_startPartId,l_endPartId))
    	   		{
    	   	  		theErrorMessages->insert(ERROR_OPEN_FILE, 
    	                               "--get partitionId error!");
    	   			continue;
    	   		//return false;
    	  		}  
    	    
    	    	tempFileInfo.m_workFileName = 
    	                        monthEndConfig->m_outputFileWorkPath + 
    	                        fileNameHead + l_startPartId +l_endPartId + fileNameTail;
    	                        
    	    	tempFileInfo.m_outputFileName = 
    	                        monthEndConfig->m_outputFilePath + 
    	                        fileNameHead + l_startPartId +l_endPartId + fileNameTail+strBacthNo;
    	                        
  				tempFileInfo.m_backupFileName = 
    	                        monthEndConfig->m_backupFilePath + 
    	                        fileNameHead + l_startPartId +l_endPartId + fileNameTail+strBacthNo; 
    	    
    	    	tempFileInfo.m_isBackupValidFile =  monthEndConfig->m_isBackupValidFile;
    	    
    	    	tempFileInfo.m_logFile = monthEndConfig->m_outputFilePath+"total.log";                         
    	    	
    	    	if(openFile(tempFileInfo))
    	    	{
    	        	tempFileInfo.m_fileStat = '1';
    	    	}
    	    	else
    	    	{
    	        	return false;
    	    	}  
    	    	
    	    		    	    	
    	   		m_batchMap.insert(map< int,OutputFileInfo >::value_type(j,tempFileInfo));
 				
			#ifdef DEBUG	    
    	   			cout<<"m_batchMap " <<m_batchMap[j].m_workFileName<<endl;
			#endif	
    	   }//for (int j=0;j<MAX_OUT_BATCHNUM; j++)
    	   
    	   m_channelMap.insert(map<int,map<int,OutputFileInfo> >::value_type(i,m_batchMap));
    	   	
    	   	m_batchMap.clear();
    	}//(int i=0; i<MAX_OUT_CHANNELNUM; i++)	
		
		m_outputFileInfoMap.insert(map<string ,map<int,map<int,OutputFileInfo> > >::value_type(*iter,m_channelMap));
			
		m_channelMap.clear();    	
	}
	
		 //公免用户输出文件流初始化 add by xuf 2009-12-16 15:47:14
	  	string fileNameHead = "Z"  + bcycDate.substr(0,6);
    	string fileNameTail = dealTime.substr(2);
 	 
    	char   tmp[4];
    	memset (tmp, 0, sizeof(tmp));        
    	string strChannelNo="";
    	    			  	  	
    	//初始化输出结构信息    
		
   		   //strChannelNo不足2位，前面补零
    	    sprintf(tmp, "%02d", monthEndConfig->m_channelNo);
    	    strChannelNo = tmp;
    	    
					resetFileTag(m_outputFileInfoEx);
    	    	    	    
					m_outputFileInfoEx.m_workFileName = 
    	                        monthEndConfig->m_outputFileWorkPath + 
    	                        fileNameHead + strChannelNo + fileNameTail;
    	                        
					m_outputFileInfoEx.m_outputFileName = 
    	                        monthEndConfig->m_outputFilePath_Z + 
    	                        fileNameHead + strChannelNo + fileNameTail;
    	                            	    
					if(openFile(m_outputFileInfoEx))
    	    {
						m_outputFileInfoEx.m_fileStat = '1';
    	    }
    	    else
    	    {
						return false;
    	    }  
    	    	  	    		    	    	
    	//add by xuf end 2009-12-16 17:07:03
	  //added by pangx on 2010-3-29 begin
     //初始化VPDN输出文件信息
    m_outputVpdnFileInfo.m_workOfStream = new ofstream;
    sprintf(tmp, "%02d", monthEndConfig->m_channelNo);
    strChannelNo = tmp;
        
    fileNameHead=strChannelNo+"_BILLPAY_"+dealTime.substr(0,6)+"05" + monthEndConfig->m_strOneClickPayProvinceCode;
    fileNameTail=".RES";

    m_outputVpdnFileInfo.m_workFileName = monthEndConfig->m_outputFileWorkPath + 
        fileNameHead + fileNameTail;

    m_outputVpdnFileInfo.m_outputFileName =monthEndConfig->m_vpdnOutputFilePath+ 
         fileNameHead + fileNameTail;

    m_outputVpdnFileInfo.m_backupFileName = monthEndConfig->m_vpdnBackupFilePath+ 
         fileNameHead + fileNameTail; 

    m_outputVpdnFileInfo.m_isBackupValidFile =  monthEndConfig->m_isBackupValidFile;

    m_outputVpdnFileInfo.m_logFile = monthEndConfig->m_vpdnBackupFilePath+"total.log"; 
 
    m_outputVpdnFileInfo.m_idNum          =  0;
    m_outputVpdnFileInfo.m_recordNum      =  0;
    m_outputVpdnFileInfo.m_feeSum         =  0;
    m_outputVpdnFileInfo.m_fileStat       = '0';
    m_outputVpdnFileInfo.m_balanceSum     =  0;
   

    //关闭清除输出流
    m_outputVpdnFileInfo.m_workOfStream->close();
    m_outputVpdnFileInfo.m_workOfStream->clear();

    m_outputVpdnFileInfo.m_workOfStream->open(m_outputVpdnFileInfo.m_workFileName.c_str(),ios::out);
    if (m_outputVpdnFileInfo.m_workOfStream->rdstate() != ios::goodbit)
    {
       theErrorMessages->insert(ERROR_OPEN_FILE,
        m_outputVpdnFileInfo.m_workFileName + "--open file error");
       m_outputVpdnFileInfo.m_workOfStream->close();
       
       return false;
    }

    m_outputVpdnFileInfo.m_fileStat = '1';
     //added by pangx on 2010-3-29 end
	
    return true;
}


bool MonthEndOutputFile::openFile(OutputFileInfo &fileInfo)
{                	
    //关闭清除输出流
    fileInfo.m_workOfStream = new ofstream;
    fileInfo.m_workOfStream->close();
    fileInfo.m_workOfStream->clear();
    
    fileInfo.m_workOfStream->open(
                fileInfo.m_workFileName.c_str(),ios::out);
    if (fileInfo.m_workOfStream->rdstate() != ios::goodbit)
    {
        theErrorMessages->insert(ERROR_OPEN_FILE, 
           fileInfo.m_workFileName + "--open file error");
        fileInfo.m_workOfStream->close();
        
        return false;
    }
    
    return true;
}


bool MonthEndOutputFile::writeFile(vector <IntergrateBill> &intergrateBillVec)
{
    int channelNo = 0;
    int batchNo=0;
    vector <IntergrateBill>::const_iterator iter;
    long lastAcctId=0;	

#ifdef _DEBUG_    
    cout<<"intergrateBillVec "<<intergrateBillVec.size()<<endl;
#endif
    
    for(iter=intergrateBillVec.begin(); iter!=intergrateBillVec.end(); iter++)
    {        
        //通道内账户记录数目+1 计算输出通道号
        if((*iter).acct_id != lastAcctId)
        {
        	 char strAcctId[17]="\0";
        	 sprintf(strAcctId,"%ld",(*iter).acct_id);
        	 
        	 channelNo = computeChannelNo(strAcctId);
        	 if (channelNo==-1) continue ; // throw ;
        	 
        	 batchNo =  computeBatchNo(strAcctId);
        	 if (batchNo==-1) continue;//throw ;
        	 
        	 //add by xuf 2010-1-7 17:02:29
        	 if (find(m_actEparchyCode.begin(),m_actEparchyCode.end(),iter->eparchy_code)==m_actEparchyCode.end())
        	 {
        	    char errMsg[120];
    	    	  sprintf(errMsg,"user:[%ld] eparchy_code:[%s] cannot create file or eparchy_code is invalid!",
    	    	                iter->user_id,iter->eparchy_code.c_str());
        	   #ifdef _DEBUG_
        	   cout<<errMsg<<endl;
        	   #endif
        	           	   
        	   m_invalidRecords++; 
             m_noInfoFeeSum +=iter->fee;
        	   
        	   theErrorMessages->insert(ERROR_USER_EPARCHYCODE, errMsg );
        	 	 continue;	
        	 }	
        	 	
           m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_idNum ++;
           //if (channelNo==-1) throw "输出文件通道号未找到!");
           
           lastAcctId=(*iter).acct_id;
           
           //add by xuf 2009-12-16 16:39:33
           if(iter->bill_id==0)
           		m_outputFileInfoEx.m_idNum ++; 
        }
        
         //modify by xuf 2009-12-16 17:45:29
         if (iter->bill_id==0||iter->canpay_tag==0) //公免用户和无效用户
         {
            	 m_UnacctUserFee+=iter->fee;
            	 m_unAcctRecords++;
            	 
            	 char t_billType='2'; //0 :公免用户 ，1:无效用户 ,2: 不出帐用户 ACT_TAG = 'Z'
            	 if (iter->canpay_tag==0&&iter->bill_id!=0) t_billType ='1';
            
              //输出公免用户add by xuf 2009-12-16 16:34:53	
            	if(m_outputFileInfoEx.m_fileStat == '1')
            	{	 						
								(*(m_outputFileInfoEx.m_workOfStream))<<
																				iter->acct_id<<FIELD_SPLIT_D<<
																				iter->user_id<<FIELD_SPLIT_D <<
																				iter->eparchy_code<< FIELD_SPLIT_D<<
																				iter->partition_id<< FIELD_SPLIT_D <<
																				iter->cyc_id<< FIELD_SPLIT_D <<
																				iter->integrate_item_code<< FIELD_SPLIT_D<<
																				iter->fee<< FIELD_SPLIT_D <<
																				iter->a_discnt<< FIELD_SPLIT_D <<
																				iter->adjust_before<< FIELD_SPLIT_D <<
																				t_billType<< FIELD_SPLIT_D <<
																				iter->update_time<<endl;   	
						
								m_outputFileInfoEx.m_recordNum++;                               
            		m_outputFileInfoEx.m_feeSum += iter->fee;
            		m_outputFileInfoEx.m_balanceSum += iter->balance;	
							}
							else	
							 	return false;
								    	           	 
            	 continue;
         }
           
        if(m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_fileStat == '1')
        {            
          m_integrateBillRecords++;

 				(*(m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_workOfStream)) <<
 																iter->bill_id<< FIELD_SPLIT_D<<
													 			iter->acct_id<<FIELD_SPLIT_D<<
													 			iter->user_id<<FIELD_SPLIT_D <<
													 			iter->partition_id<<FIELD_SPLIT_D <<
													 			iter->cyc_id<< FIELD_SPLIT_D <<
													 			iter->integrate_item_code<< FIELD_SPLIT_D<<
													 			iter->fee<< FIELD_SPLIT_D <<
													 			iter->balance<< FIELD_SPLIT_D <<
													 			iter->print_fee<< FIELD_SPLIT_D <<
													 			iter->b_discnt<< FIELD_SPLIT_D <<
													 			iter->a_discnt<< FIELD_SPLIT_D <<
													 			iter->adjust_before<< FIELD_SPLIT_D <<
													 			iter->adjust_after<< FIELD_SPLIT_D <<
													 			iter->late_fee<< FIELD_SPLIT_D <<
													 			iter->late_balance<< FIELD_SPLIT_D <<
													 			iter->latecal_date<< FIELD_SPLIT_D <<
													 			iter->canpay_tag<< FIELD_SPLIT_D <<
													 			iter->pay_tag<< FIELD_SPLIT_D <<
													 			iter->bill_pay_tag<< FIELD_SPLIT_D <<
													 			iter->update_time<< FIELD_SPLIT_D <<
													 			iter->update_depart_id<< FIELD_SPLIT_D <<
													 			iter->update_staff_id<< FIELD_SPLIT_D <<
													 			iter->version_no<< FIELD_SPLIT_D <<
													 			iter->eparchy_code<< FIELD_SPLIT_D <<
													 			iter->m_ispps<<FIELD_SPLIT_D <<
													 			iter->city_code<<FIELD_SPLIT_D<<
													 			iter->serial_number<<FIELD_SPLIT_D<<
													 			iter->net_type_code<<endl;
           
            m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_recordNum++;                               
            m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_feeSum += iter->fee;
            m_outputFileInfoMap[iter->eparchy_code][channelNo][batchNo].m_balanceSum += iter->balance;
            m_IntegrateFeeSum += iter->fee;
        }
        else
        {
            //该通道输出文件流尚未打开
            return false;
        }
    }
    
    return true;
}

//added by pangx on 2010-3-29 begin
 const char MONTHEND_VPDN_FIELD_SPLIT = '|';
bool MonthEndOutputFile::writeFile(vector <VpdnOneClickPayBill> &vecVpdnOneClickPayBill)
{
    vector <VpdnOneClickPayBill>::const_iterator iter;
    if(m_outputVpdnFileInfo.m_fileStat != '1')
    {
        //该通道输出文件流尚未打开
        return false;
    }
   
    for(iter=vecVpdnOneClickPayBill.begin(); iter!=vecVpdnOneClickPayBill.end(); iter++)
    {  
    	/*           
       (*( m_outputVpdnFileInfo.m_workOfStream)) <<
            *(iter->pLocalProvince)<< MONTHEND_VPDN_FIELD_SPLIT<<
            *(iter->pGroupId)<<MONTHEND_VPDN_FIELD_SPLIT<<
            *(iter->pServiceId)<<MONTHEND_VPDN_FIELD_SPLIT <<
            iter->fee_date<<MONTHEND_VPDN_FIELD_SPLIT;
        */
        (*( m_outputVpdnFileInfo.m_workOfStream)) <<
            iter->pCitycode<< MONTHEND_VPDN_FIELD_SPLIT<<
            *(iter->pBizType)<< MONTHEND_VPDN_FIELD_SPLIT<<
            iter->pSubinstanceid<<MONTHEND_VPDN_FIELD_SPLIT<<
            iter->pServiceId<<MONTHEND_VPDN_FIELD_SPLIT <<
            iter->fee_date<<MONTHEND_VPDN_FIELD_SPLIT;
        for(int i=0;i<ONECLICKPAYBILL_ITEM_TOTAL;++i)
        {
            if(iter->detailfee[i] != 0)
            {
                (*(m_outputVpdnFileInfo.m_workOfStream))<<iter->detailfee[i]<<MONTHEND_VPDN_FIELD_SPLIT;
            }
            else
            {
                (*(m_outputVpdnFileInfo.m_workOfStream))<<MONTHEND_VPDN_FIELD_SPLIT;
            }
        }
        (*(m_outputVpdnFileInfo.m_workOfStream))<<iter->totalfee<<MONTHEND_VPDN_FIELD_SPLIT;
         for(int i=0;i<ONECLICKPAYBILL_VPDN_ITEM;++i)
        {
            if(iter->vpdnfee[i] != 0)
            {
                (*(m_outputVpdnFileInfo.m_workOfStream))<<iter->vpdnfee[i]<<MONTHEND_VPDN_FIELD_SPLIT;
            }
            else
            {
                (*(m_outputVpdnFileInfo.m_workOfStream))<<MONTHEND_VPDN_FIELD_SPLIT;
            }
        }
         (*(m_outputVpdnFileInfo.m_workOfStream))<<
         	   MONTHEND_VPDN_FIELD_SPLIT<<
             MONTHEND_VPDN_FIELD_SPLIT<<endl;

        m_outputVpdnFileInfo.m_recordNum++;
        m_outputVpdnFileInfo.m_feeSum +=iter->totalfee;
      
    }
    
    return true;
}
  //added by pangx on 2010-3-29 end
int MonthEndOutputFile::computeChannelNo(const string& acctId)
{
    string strAcctId = "";
    strAcctId = acctId;
     
    if(strAcctId.length()<4)
    {
        return -1;
    }
    else
    {
    	  int l_id = atoi(strAcctId.substr(strAcctId.length()-4,4).c_str());
    	  for (vector <channelSt>::iterator itvec = m_channelStVec.begin(); itvec!=m_channelStVec.end();itvec++)
    	  {
//    	    cout<<"m_channelNo: "<<itvec->m_channelNo<<", m_startId: "<<itvec->m_startId <<", m_endId: "<<itvec->m_endId<<endl;
    	    if (l_id >= itvec->m_startId &&l_id <= itvec->m_endId)
    	    	return itvec->m_channelNo;	
    	   }
    }
    
    return -1;
}

//根据通道号和批次号获取起止分区ID
bool MonthEndOutputFile::getPartitionId(const int r_batch)
{
	  map<int,IntPair>   l_mp;
	  for (vector <channelSt>::iterator itvec = m_channelStVec.begin(); 
	  	  itvec!=m_channelStVec.end();itvec++)
    {
      	 channelNoCalc(itvec->m_startId,itvec->m_endId,r_batch,l_mp); 
      	 
#ifdef _DEBUG_
   for(map<int,IntPair>::iterator t_it = l_mp.begin();t_it!=l_mp.end();t_it++)
	 {  
	 	  cout<<"itvec->m_channelNo:"<<itvec->m_channelNo<<endl;
	 	  cout<<"batchNO:"<<t_it->first<<endl;
			cout<<"beginid:"<<(t_it->second).first<<endl;
			cout<<"endid:"<<(t_it->second).second<<endl;
	 }
#endif      	  	       	 
         m_chanBacthPartition.insert(map<int ,map<int,IntPair> >::value_type(itvec->m_channelNo,l_mp));
         l_mp.clear();	 
      	   	
    }
	
      return true;
}

bool MonthEndOutputFile::getPartitionIdByNo(const int &r_channNo,const int &r_batchNo,string &r_startPartId,string &r_endPartId)
{
	char l_strBuff[4+1]="\0";
	sprintf(l_strBuff,"%04d",m_chanBacthPartition[r_channNo][r_batchNo].first);
#ifdef _DEBUG_	
	cout<<"r_channNo "<<r_channNo<<" r_batchNo "<<r_batchNo<<endl;
	cout<<"m_chanBacthPartition[r_channNo][r_batchNo].first ----->"<<m_chanBacthPartition[r_channNo][r_batchNo].first<<endl;
#endif 
  r_startPartId =  l_strBuff;
  memset(l_strBuff,0,5);
  sprintf(l_strBuff,"%04d",m_chanBacthPartition[r_channNo][r_batchNo].second);
  r_endPartId   =  l_strBuff;
#ifdef _DEBUG_
	cout<<"r_startPartId= "<<r_startPartId<<endl;
	cout<<"r_endPartId= "<<r_endPartId<<endl;
#endif
  if (r_endPartId=="0000") return false;
	return true;
}



//根据通道的起止分区ID和批次数计算批次的起止分区ID
void MonthEndOutputFile::channelNoCalc(int begId,int endId,int batch,map<int,IntPair>  & mp)
{	
   if ((endId - begId +1) < batch) //当等于的时候有些问题?????
   {
      IntPair   oIntPair;
      int l_bachNo=0;
      oIntPair.first = begId;
      oIntPair.second = endId;
      
      mp.insert(map<int ,IntPair >::value_type(l_bachNo,oIntPair)); 
      return ;
   }
   
   //每批记录数
   int step 	=   ceil((float)(endId - begId+1) / batch);
   int start 	= 	begId;

   int l_bachNo=0;
   while (start <= endId)
   {
      IntPair   oIntPair;
      oIntPair.first  = start;
      oIntPair.second = start+step -1;
            
      mp.insert(map<int,IntPair >::value_type(l_bachNo,oIntPair)); 
   	  start += step;
   	  l_bachNo++;   
   }
   
   mp[mp.size() - 1].second =  endId;
   
   return;
}


//需要修改
int MonthEndOutputFile::computeBatchNo(const string& acctId)
{
    string strAcctId = "";
    strAcctId = acctId;
     
    if(strAcctId.length()<4)
    {
        return -1;
    }
    else
    {
    	  int l_id = atoi(strAcctId.substr(strAcctId.length()-4,4).c_str());    	  

    	 // return (l_id%500)/100;
    	 	for (vector <channelSt>::iterator itvec = m_channelStVec.begin(); 
	  	  		itvec!=m_channelStVec.end();itvec++)
       {
    	    if (l_id>=itvec->m_startId&&l_id<=itvec->m_endId)
    	    {
    	    	for(map<int,IntPair>::iterator t_it=	m_chanBacthPartition[itvec->m_channelNo].begin();
    	    		 t_it!=m_chanBacthPartition[itvec->m_channelNo].end();t_it++)
    	      {
    	      	 if (l_id>=(t_it->second).first&&l_id<=(t_it->second).second)
    	      	 		return t_it->first;
    	      }	 
    	    }	
       }
    }
    
    return -1;
}



void MonthEndOutputFile::closeFile()
{
	vector<string>::const_iterator iter;
		
	for(iter = m_actEparchyCode.begin(); iter != m_actEparchyCode.end(); ++iter)
	{
    	for (int i=m_chanBacthPartition.begin()->first; i<=(--m_chanBacthPartition.end())->first; i++)
    	{
    		for (int j=0; j<MAX_OUT_BATCHNUM; j++)
    		{
    	    	if (m_outputFileInfoMap[*iter][i][j].m_fileStat == '1')
    	    	{
    	    	    //关闭流
    	    	    m_outputFileInfoMap[*iter][i][j].m_workOfStream->close();
    	    	    m_outputFileInfoMap[*iter][i][j].m_workOfStream->clear();
    	    }
    	  }    	         
    	}		
	}		
	
    	     //add by xuf 2009-12-16 17:12:43
    	   if (m_outputFileInfoEx.m_fileStat == '1')
    	   {
    	   		//关闭流
    	    	m_outputFileInfoEx.m_workOfStream->close();
    	    	m_outputFileInfoEx.m_workOfStream->clear();
    	   }     	         

		//added by pangx on 2010-3-29 begin
    //关闭VPDN流
    m_outputVpdnFileInfo.m_workOfStream->close();
    m_outputVpdnFileInfo.m_workOfStream->clear();
    //added by pangx on 2010-3-29 end
}


void MonthEndOutputFile::resetFileTag(OutputFileInfo &fileInfo)
{
    fileInfo.m_workFileName   = "";
    fileInfo.m_outputFileName = "";    
    fileInfo.m_idNum          =  0;
    fileInfo.m_recordNum      =  0;
    fileInfo.m_feeSum         =  0;
    fileInfo.m_fileStat       = '0';
    fileInfo.m_balanceSum     =  0;
}


bool MonthEndOutputFile::linkUnlink()
{
	vector<string>::const_iterator iter;
		
	for(iter = m_actEparchyCode.begin(); iter != m_actEparchyCode.end(); ++iter)
	{
    	//把各种类型工作文件搬迁到输出目录下，生成输出文件
    	for (int i=1000; i<MAX_OUT_CHANNELNUM; i++)
    	{
    		 for (int j=0; j<MAX_OUT_BATCHNUM; j++)
    		 {
    	    	//如果该通道无文件输出，继续
    	    	if (m_outputFileInfoMap[*iter][i][j].m_fileStat == '0')
    	    	{
    	    	    continue;
    	    	}    
    	    	
    	    	//如果该类型工作文件中数据记录数大于0，改名为输出文件    
    	    	if (m_outputFileInfoMap[*iter][i][j].m_recordNum > 0)
    	    	{
    	    	    if (rename(m_outputFileInfoMap[*iter][i][j].m_workFileName.c_str(),
    	    	               m_outputFileInfoMap[*iter][i][j].m_outputFileName.c_str()) < 0)
    	    	    {
    	    	        
    	    	        char errMsg[120];
    	    	        sprintf(errMsg,"--file rename error:[%d]!",errno);
    	    	        theErrorMessages->insert(ERROR_LINK_UNLINKE,
    	    	                m_outputFileInfoMap[*iter][i][j].m_workFileName + " ->" + 
    	    	                m_outputFileInfoMap[*iter][i][j].m_outputFileName + 
    	    	                errMsg);
    	    	                
    	    	        return false;
    	    	    }
    	    	    
    	    	    if (m_outputFileInfoMap[*iter][i][j].m_isBackupValidFile)
    	    	    {
    	    	    //备份给销账
    	    	     //	if (link(m_outputFileInfo[i][j].m_outputFileName.c_str(),m_outputFileInfo[i][j].m_backupFileName.c_str())<0)
    	    	     if (!renameFile(m_outputFileInfoMap[*iter][i][j].m_outputFileName.c_str(),m_outputFileInfoMap[*iter][i][j].m_backupFileName.c_str()))
    	    				{
    	    					char errMsg[120];
    	    	    		sprintf(errMsg,"Error(%s:%d):link(%s,%s) failed!",
    	    	        __FILE__,__LINE__,m_outputFileInfoMap[*iter][i][j].m_outputFileName.c_str(),m_outputFileInfoMap[*iter][i][j].m_backupFileName.c_str());
    	    	      	theErrorMessages->insert(ERROR_LINK_UNLINKE,
    	    	                m_outputFileInfoMap[*iter][i][j].m_outputFileName + " ->" + 
    	    	                m_outputFileInfoMap[*iter][i][j].m_backupFileName + 
    	    	                errMsg);
    	    	    		return false;
    	    				}
    	    		  }
    	    	
    	    	    //写日志文件给销账
    	    	    outLog(m_outputFileInfoMap[*iter][i][j]);
    	    	   
    	    	    theLogger->pEndOutput(m_performId,
    	    	                          m_outputFileInfoMap[*iter][i][j].m_outputFileName.c_str(),
    	    	                          m_outputFileInfoMap[*iter][i][j].m_recordNum,
    	    	                          "TOTAL_ID_NUM=%d,TOTAL_FEE_SUM=%lld,"
    	    	                          "TOTAL_BALANCE_SUM=%lld",
    	    	                          m_outputFileInfoMap[*iter][i][j].m_idNum,
    	    	                          m_outputFileInfoMap[*iter][i][j].m_feeSum,
    	    	                          m_outputFileInfoMap[*iter][i][j].m_balanceSum
    	    	                          );
    	    	                          
    	    	
    	    	                          
    	    	}
    	    	//如果该通道文件中数据记录数等于0，直接删除
    	    	else
    	    	{
    	    	    if (unlink(m_outputFileInfoMap[*iter][i][j].m_workFileName.c_str()) != 0)
    	    	    {
    	    	        theErrorMessages->insert(ERROR_LINK_UNLINKE,
    	    	                m_outputFileInfoMap[*iter][i][j].m_workFileName + 
    	    	                "--file unlink error");
    	    	                
    	    	        return false;                                
    	    	    }    
    	    	}   
    	    
    	    	//初始化输出文件标记
    	    	resetFileTag(m_outputFileInfoMap[*iter][i][j]);
    	   }
    	}		
	}	
    
		//add by xuf 2009-12-16 17:51:31
		//如果该通道无文件输出，继续
		if (m_outputFileInfoEx.m_fileStat == '0')
		{
   		return true;
		}    
    	    	
		//如果该类型工作文件中数据记录数大于0，改名为输出文件    
		if (m_outputFileInfoEx.m_recordNum > 0)
		{
		    if (rename(m_outputFileInfoEx.m_workFileName.c_str(),
		               m_outputFileInfoEx.m_outputFileName.c_str()) < 0)
		    {
		        
		        char errMsg[120];
		        sprintf(errMsg,"--file rename error:[%d]!",errno);
		        theErrorMessages->insert(ERROR_LINK_UNLINKE,
		                m_outputFileInfoEx.m_workFileName + " ->" + 
		                m_outputFileInfoEx.m_outputFileName + 
		                errMsg);
		                
		        return false;
		    }    	    	                           	    	                          
		}
		//如果该通道文件中数据记录数等于0，直接删除
		else
		{
		    if (unlink(m_outputFileInfoEx.m_workFileName.c_str()) != 0)
		    {
		        theErrorMessages->insert(ERROR_LINK_UNLINKE,
		                m_outputFileInfoEx.m_workFileName + 
		                "--file unlink error");
		                
		        return false;                                
		    }    
		}   
		
		//初始化输出文件标记
		resetFileTag(m_outputFileInfoEx);
		//add end
    //added by pangx on 2010-3-29 begin
   if (m_outputVpdnFileInfo.m_fileStat != '0')
    {
        if (m_outputVpdnFileInfo.m_recordNum > 0)
        {
            if (rename(m_outputVpdnFileInfo.m_workFileName.c_str(),
                       m_outputVpdnFileInfo.m_outputFileName.c_str()) < 0)
            {
                
                char errMsg[120];
                sprintf(errMsg,"--file rename error:[%d]!",errno);
                theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputVpdnFileInfo.m_workFileName + " ->" + 
                        m_outputVpdnFileInfo.m_outputFileName + 
                        errMsg);
                        
                return false;
            }
            
            if (m_outputVpdnFileInfo.m_isBackupValidFile)
            {
                //备份给销账
                //	if (link(m_outputFileInfo[i][j].m_outputFileName.c_str(),m_outputFileInfo[i][j].m_backupFileName.c_str())<0)
                if ((copy(m_outputVpdnFileInfo.m_outputFileName.c_str(),m_outputVpdnFileInfo.m_backupFileName.c_str()))!=0)
                {
                    char errMsg[120];
                    sprintf(errMsg,"Error(%s:%d):link(%s,%s) failed!",
                    __FILE__,__LINE__,m_outputVpdnFileInfo.m_outputFileName.c_str(),m_outputVpdnFileInfo.m_backupFileName.c_str());
                    theErrorMessages->insert(ERROR_LINK_UNLINKE,
                    m_outputVpdnFileInfo.m_outputFileName + " ->" + 
                    m_outputVpdnFileInfo.m_backupFileName + 
                    errMsg);
                    return false;
                }
        	  }

             //写日志文件给销账
            outLog(m_outputVpdnFileInfo);
           
            theLogger->pEndOutput(m_performId,
                                  m_outputVpdnFileInfo.m_outputFileName.c_str(),
                                  m_outputVpdnFileInfo.m_recordNum,
                                  "TOTAL_ID_NUM=%d,TOTAL_FEE_SUM=%lld,"
                                  "TOTAL_BALANCE_SUM=%lld",
                                  m_outputVpdnFileInfo.m_idNum,
                                  m_outputVpdnFileInfo.m_feeSum,
                                  m_outputVpdnFileInfo.m_balanceSum
                                  );
         }
         
        //如果该通道文件中数据记录数等于0，直接删除
        else
        {
            if (unlink(m_outputVpdnFileInfo.m_workFileName.c_str()) != 0)
            {
                theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputVpdnFileInfo.m_workFileName + 
                        "--file unlink error");
                        
                return false;                                
            }    
        }   
        
    }    
   //added by pangx on 2010-3-29 end
    m_performId = "";
    return true;    
}
 
 
bool MonthEndOutputFile::outLog(const OutputFileInfo &outputFileInfo)
{
	ofstream logOfStream;
	logOfStream.open(outputFileInfo.m_logFile.c_str(),ios::app);
	if (logOfStream.rdstate() != ios::goodbit)
	{
		logOfStream.close();
		theErrorMessages->insert(ERROR_OPEN_FILE,
      		outputFileInfo.m_logFile + "--file open error");	
		return false;
	}	

	logOfStream<<"file="<<strrchr(outputFileInfo.m_outputFileName.c_str(),'/')+1<<","
	        <<"record_num="<<outputFileInfo.m_recordNum<<","
	        <<"total_fee="<<outputFileInfo.m_feeSum<<","
          <<"balance="<<outputFileInfo.m_balanceSum<<endl;
          
	logOfStream.close();
  logOfStream.clear();
	
	return true;
}


bool  MonthEndOutputFile::renameFile(const char *r_srcFullFileName,const char *r_desFullFileName)const 
{ 
   if(::rename(r_srcFullFileName,r_desFullFileName)!=0)
  {
#ifdef _DEBUG_
      cout<<"r_srcFullFileName    = "<<r_srcFullFileName<<endl;
      cout<<"r_desFullFileName    = "<<r_desFullFileName<<endl;
#endif
    if(copy(r_srcFullFileName,r_desFullFileName)!=0)
    {
      return false;
    }
		//add 2009-02-11 zhouq begin
		if(::unlink(r_srcFullFileName)!=0)
  	{
    	return false;
  	}
  	//add 2009-02-11 zhouq end
  	
  }
  return true;  
}



int MonthEndOutputFile::copy(const char *r_srcFile,const char *r_desFile) const
{
  FILE *t_srcFile,*t_desFile;
  int t_readSize;
  const int BUFFLEN=1024;
  char t_msgBuff[BUFFLEN+1];
  char t_tmpFile[512+1];
  int  t_sLen,t_tLen;
  
  if(strrchr(r_desFile,'/')==NULL)
  {
    sprintf(t_tmpFile,"%%%s",r_desFile);
  }else
  {
    sprintf(t_tmpFile,"%s",r_desFile);
    t_tLen= strlen(r_desFile);
    t_sLen = strlen(strrchr(r_desFile,'/')+1);
    sprintf(t_tmpFile+t_tLen-t_sLen,"%%%s",strrchr(r_desFile,'/')+1);
  } 
  if((t_srcFile=fopen(r_srcFile,"rb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<r_srcFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  if((t_desFile=fopen(t_tmpFile,"wb"))==NULL)
  {
    #ifdef _DEBUG_
      cout<<"open file:"<<t_tmpFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    fclose(t_srcFile);
    return -1;
  }
  while(!feof(t_srcFile))
  {
    t_readSize=fread(t_msgBuff,1,BUFFLEN,t_srcFile);
    if(t_readSize == 0) break;
    if(fwrite(t_msgBuff,1,t_readSize,t_desFile)!=t_readSize)
    {
      fclose(t_srcFile);
      fclose(t_desFile);
      return -1;
    }
  }
  fclose(t_srcFile);
  fclose(t_desFile);
  if(rename(t_tmpFile,r_desFile)!=0)
  {
    #ifdef _DEBUG_
      cout<<"rename file:"<<t_tmpFile<<" "<<r_desFile<<" error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return -1;
  }
  return 0;
}
//输出不出帐用户账单--add by dingr 
bool MonthEndOutputFile::outFreeFeeUserBill(const vector <IntergrateBill> &freeFeeUserBillVec,const string strChannelNo)
{	 
			vector<IntergrateBill>::const_iterator iter;
			//int icount=0;
	
      for(iter=freeFeeUserBillVec.begin(); iter!=freeFeeUserBillVec.end(); iter++)
	  {    
				//通道内账户记录数目+1 计算输出通道号
         m_outputFileInfoEx.m_idNum ++;				
	      // icount++;
         //输出公免用户add by xuf 2009-12-16 16:34:53	
        if(m_outputFileInfoEx.m_fileStat == '1')
        {	 						
					(*(m_outputFileInfoEx.m_workOfStream))<<
																				iter->acct_id<<FIELD_SPLIT_D<<
																				iter->user_id<<FIELD_SPLIT_D <<
																				iter->eparchy_code<< FIELD_SPLIT_D<<
																				iter->parti1 璁＄畻杈撳嚭閫氶亾鍙