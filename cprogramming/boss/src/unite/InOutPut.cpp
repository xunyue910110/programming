// ##########################################
// Source file : InOutPut.cpp
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020424
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <unistd.h>
#include <sys/stat.h>
#include <iostream>

#include "InOutPut.h"
#include "base/StringUtil.h"

string  InOutPut::m_LogFileNameWithPath="";

InOutPut::InOutPut(const string &path,const string &path1, const int chanNo)
{
	m_savedPoolPath = path;         // 缓存存放目录
	m_beforeUnitePath = path1;      //合并话单以前的各个原始话单保存目录   add by lizhao 2008-10-25
	m_channelNo     = StringUtil::toString(chanNo);  //进程通道号
	m_tmpFileHead   = "#";

	m_srcPath = "";
	m_desPath = "";
	
	m_inputFp = 0;
	m_outPutFp = 0;
	m_LogFp = 0;	 
    m_write2file = 0;
    m_totalfieldcount = 0;
    m_FlagPosi = 0;
}

// 当进程启动时,从log目录下的日志文件中获得文件序号
void InOutPut::readSavedPools( vector<ChanInfo *> &chanInfos, vector<BaseCdrProcessor *> &vectCdrProc )
{
	int i,j,k= 0;
	//int num = 0;
	string poolsFileWithPath,strMergeKey;
	char cdrBuffer[MAXCDRSIZE];
	CDR_POOL_MAPS_ITR itrPoolMaps;
	vector<string> *pStrVect;
	FILE *Fp;
	pair<CDR_POOL_ITR, bool> resultPair;
	
	for(i=0; i<chanInfos.size(); ++i)
	{  
		itrPoolMaps = (chanInfos[i]->m_cdrPoolMaps).begin();
		for(j=0;j<(chanInfos[i]->m_cdrPoolMaps).size();++j,++itrPoolMaps)
		{
			// 文件名 = '业务类型名'+'进程通道号'+'处理通道号'
			poolsFileWithPath = m_savedPoolPath + itrPoolMaps->first + "_" \
								+ m_channelNo + "_" +StringUtil::toString(chanInfos[i]->m_subChanNo);
			Fp = fopen(poolsFileWithPath.c_str(),"r");
			if( Fp != NULL)
			{
				while( fgets(cdrBuffer,MAXCDRSIZE,Fp) )
				{
				    //num++;
					// 删除尾部的控制字符
					int  len=strlen(cdrBuffer);
				    for(int i=len-1;i>0;i--)                   
				    {
				        if(cdrBuffer[i]>0x20) 
				        {
				           cdrBuffer[i+1] = 0;
				           break;
				        }
				    }   
                    
					pStrVect = new vector<string>();

					StringUtil::split(cdrBuffer,TOKEN,(*pStrVect),true);
					if( pStrVect->size() > 0)
					{
						// 缓存文件中cdr的最后一个字段是mergekey
						strMergeKey = pStrVect->back();
						pStrVect->pop_back();
						
						if (pStrVect->size() == m_totalfieldcount)
						{
						    (vectCdrProc[k]->m_needUnite).insert(multimap<string, vector<string> >::value_type(strMergeKey, *pStrVect));
						}
						else if( (*pStrVect)[m_FlagPosi] == "3" )
						{
						    (vectCdrProc[k]->m_lastCdr).insert(multimap<string, vector<string> >::value_type(strMergeKey, *pStrVect));
						}
						else
						{    
						    (itrPoolMaps->second)->insert( CDR_POOL::value_type(strMergeKey,pStrVect) ); //记录插入缓冲池中
					    }
					}						
					else
					{
						delete pStrVect;	
					}				
				}
				fclose(Fp);
			}
			// 若目录下没有相应的缓存文件则继续
			else
			{
				continue;	
			}			
		}
		//cout<<"over"<<endl;
		k++;
			
	}	
}

// 保存缓存中的记录到文件中
bool InOutPut::savePools2Files(const string &strPoolType,const int subChanNo,CDR_POOL *cdrPool,multimap<string,vector<string> > *lastCdr,multimap<string, vector<string> > *needUnite)
{
	string tmppoolsFileWithPath,strTmpCdr,poolsFileWithPath;
	char cdrBuffer[MAXCDRSIZE];
	multimap<string, vector<string> >::iterator itaor;
	CDR_POOL_ITR itrPool;
	FILE *Fp;

	itrPool = cdrPool->begin();
	// 文件名 = '业务类型名'+'进程通道号'+'处理通道号'
	tmppoolsFileWithPath = m_savedPoolPath + m_tmpFileHead + strPoolType + "_" \
						+ m_channelNo + "_" +StringUtil::toString(subChanNo);
	poolsFileWithPath = m_savedPoolPath + strPoolType + "_" \
						+ m_channelNo + "_" +StringUtil::toString(subChanNo);
	
	// 如果缓冲池里没有数据则不生成文件啦
	if( (cdrPool->size() != 0)  || (lastCdr->size() != 0) || (needUnite->size() != 0) )
	{
	    vector<string> tmpPoolCdr;
	    char tmp[1024];
		Fp = fopen(tmppoolsFileWithPath.c_str(),"w");
		if( Fp != NULL)
		{
		    FP_BEGIN(cdrPool)
			for(int i=0;i<cdrPool->size();++i,++itrPool)
			{    
			     FP_BEGIN(pool_join)
			     tmpPoolCdr = *(itrPool->second);
				 memset(tmp,0,sizeof(tmp));
				 vector<string>::iterator itr;
				 int iOffSet=0;
				 for(itr=tmpPoolCdr.begin();itr!=tmpPoolCdr.end();++itr)
				 {
				      sprintf(tmp+iOffSet,"%s,",(*itr).c_str()); 
				      iOffSet = iOffSet + (*itr).length() +1;
				 }
				sprintf(tmp+iOffSet,"%s\n",(itrPool->first).c_str());
				FP_END(pool_join)
				FP_BEGIN(pool_write)
				fputs( tmp,Fp );
				FP_END(pool_write)
			}
			FP_END(cdrPool)
			FP_BEGIN(lastcdr)
			itaor = lastCdr->begin();
			while (itaor != lastCdr->end())
			{
			    FP_BEGIN(last_join)
				memset(tmp,0,sizeof(tmp));
				vector<string>::iterator itr;
				int iOffSet=0;
				for(itr=(itaor->second).begin();itr!=(itaor->second).end();++itr)
				{
				     sprintf(tmp+iOffSet,"%s,",(*itr).c_str()); 
				     iOffSet = iOffSet + (*itr).length() +1;
				}
				sprintf(tmp+iOffSet,"%s\n",(itaor->first).c_str());
				FP_END(last_join)
				FP_BEGIN(last_write)
				fputs( tmp,Fp );
				FP_END(last_write)
				itaor++;
			}
			FP_END(lastcdr)
			FP_BEGIN(needUnite)
			itaor = needUnite->begin();
			while (itaor != needUnite->end())
			{
			    FP_BEGIN(need_join)
				memset(tmp,0,sizeof(tmp));
				vector<string>::iterator itr;
				int iOffSet=0;
				for(itr=(itaor->second).begin();itr!=(itaor->second).end();++itr)
				{
				     sprintf(tmp+iOffSet,"%s,",(*itr).c_str()); 
				     iOffSet = iOffSet + (*itr).length() +1;
				}
				sprintf(tmp+iOffSet,"%s\n",(itaor->first).c_str());
				FP_END(need_join)
				FP_BEGIN(need_write)
				fputs( tmp,Fp );
				FP_END(need_write)
				itaor++;
			}
			FP_END(needUnite)
		}		
		else
		{
			throw string("打开输出文件") + tmppoolsFileWithPath + string(" 失败!");	
		}
		
		fclose(Fp);
	    if( rename(tmppoolsFileWithPath.c_str(),poolsFileWithPath.c_str()) == -1 )
        {
	  	    throw string("不能将：")+tmppoolsFileWithPath+"改名成 "+poolsFileWithPath;
	    }
	}
	else //但是需要把原来的缓存文件清空
	{
		// 查看缓存文件是否存在
		if( (access( poolsFileWithPath.c_str(), 0 )) != -1 )
		{
			if( unlink( poolsFileWithPath.c_str() ) != 0  )
			{
				throw string("删除缓存文件") + poolsFileWithPath + string(" 失败!");		
			}	
		}	
	}

	return true;
}

bool InOutPut::readCdr(char * cdrBuffer,const int n)
{
	if ( fgets(cdrBuffer,n,m_inputFp)==NULL )
	   return false;
	   
	// 删除尾部的控制字符
	int  len=strlen(cdrBuffer);
    for(int i=len-1;i>0;i--)                   
    {
        if( cdrBuffer[i]>0x20 ) 
        {
           cdrBuffer[i+1] = 0;
           break;
        }
    }   
	
    return true;
}

bool InOutPut::writeCdr(const char* cdr )
{
	string strCdr;
	strCdr = cdr;

    if(strlen(cdr)>0 && cdr[strlen(cdr)-1] != '\n')
    {
    	strCdr += "\n";
    }

	if ( fputs(strCdr.c_str(),m_outPutFp) < 0 ) 
	{
	    theErrorMessages->insert(E_MERGE_WRITE_FILE,"输入文件" + m_fileName +" 写入输出文件失败 !进程退出");
		exit(1);
	}
	addWrite2file();
	return true;
}

// 打开输入输出文件
bool InOutPut::openFile(const char * cpFileName)
{	
	m_fileName = cpFileName;
	string srcFile = m_srcPath + cpFileName;
	string desFile = m_desPath + m_tmpFileHead + cpFileName;
	
	m_inputFp  = fopen(srcFile.c_str(),"r");
	if( m_inputFp==NULL )
		throw string("不能打开输入文件:") + srcFile;

	m_outPutFp = fopen(desFile.c_str(),"w");
	if( m_outPutFp==NULL )
	{
		fclose(m_outPutFp);
		throw string("不能打开输出文件:") + desFile;
	}
	
	return true;
}

//关闭输入输出路径下的文件
void InOutPut::closeFile(const char * cpFileName)
{
	string srcFile = m_srcPath + cpFileName;
	string tmpDesFile = m_desPath + m_tmpFileHead + cpFileName;
	string desFile = m_desPath + cpFileName;

	fclose( m_inputFp );
	fclose( m_outPutFp );
		
    if( rename(tmpDesFile.c_str(),desFile.c_str()) == -1 )
    {
	  	throw string("不能将：")+tmpDesFile+"改名成 "+desFile;
	}
	//输入文件
	if( unlink( srcFile.c_str() )!=0)           
	{
  		throw string("不能删除")+srcFile+"文件!";
  	}
}

//切换不同处理通道(如:配置文件中的"chan_0"切换到"chan_1")时,重置输入输出路径
void InOutPut::reset( const string &srcPath,const string &desPath)
{
	m_srcPath = srcPath;
	m_desPath = desPath;

	m_inputFp = 0;
	m_outPutFp = 0;	 
	
	return;	
}

//创建日志文件,写已经合并了的话单记录到日志文件 add by lizhao 2008-10-27
bool InOutPut::writeLogFile(const string &strPoolType,const int subChanNo, multimap<string, vector<string> > logvect)
{
    string strTmp,strTmpCdr;
    
    struct tm* stNowTime;
    time_t tmpNowTime;
	char tmp[15];
	
	time(&tmpNowTime);       //获得日历时间，以秒为单位
	stNowTime = localtime(&tmpNowTime); //将日历时间转换成年月日时分秒的样式
    memset(tmp,0,sizeof(tmp));
	sprintf(tmp,"%04d%02d%02d",stNowTime->tm_year + 1900,stNowTime->tm_mon+1,stNowTime->tm_mday);
    
    //日志文件名='业务类型名'+'进程通道号'+'处理通道号'+'日期'
    strTmp = m_beforeUnitePath +strPoolType+"_"+m_channelNo+"_"+StringUtil::toString(subChanNo)+"_"+tmp;
    
    m_LogFileNameWithPath = strTmp;
    
    if (m_LogFp == 0)
    {
        if ( access(strTmp.c_str(),F_OK)  == -1  )
        {
            m_LogFp = fopen(strTmp.c_str(),"w");
        }
        else
        {
            m_LogFp = fopen(strTmp.c_str(),"a");
        }
        if (m_LogFp == NULL)
        {
            throw string("创建日志文件") + strTmp + string(" 失败!");
        }
        
    }
    
    multimap<string, vector<string> >::iterator  itrPool;
        
    itrPool = logvect.begin();
    
    if (logvect.size() >=  3)
    {
         for (int i=0; i<(logvect.size()-2); i++,itrPool++)
         {
             while(1)
             {
                if ( (itrPool->second).size() == m_totalfieldcount )
                    break;
                else
                    (itrPool->second).pop_back();
             }
             strTmpCdr = StringUtil::join((itrPool->second),TOKEN);
 	     	 strTmpCdr += '\n';
 	     	 fputs( strTmpCdr.c_str(),m_LogFp );
         }
 	     itrPool = logvect.end();
 	     itrPool--;
 	     while(1)
         {
            if ( (itrPool->second).size() == m_totalfieldcount )
                break;
            else
                (itrPool->second).pop_back();
         }
 	     strTmpCdr = StringUtil::join((itrPool->second),TOKEN);
         strTmpCdr += '\n';
 	     fputs( strTmpCdr.c_str(),m_LogFp );
 	     itrPool--;
 	     while(1)
         {
            if ( (itrPool->second).size() == m_totalfieldcount )
                break;
            else
                (itrPool->second).pop_back();
         }
 	     strTmpCdr = StringUtil::join((itrPool->second),TOKEN);
         strTmpCdr += '\n';
 	     fputs( strTmpCdr.c_str(),m_LogFp );
    }    
    else 
    {    
        while(itrPool != logvect.end())
        {
             while(1)
             {
                if ( (itrPool->second).size() == m_totalfieldcount )
                    break;
                else
                    (itrPool->second).pop_back();
             }
 		     strTmpCdr = StringUtil::join((itrPool->second),TOKEN);
 		     strTmpCdr += '\n';
 		     fputs( strTmpCdr.c_str(),m_LogFp );
 		     itrPool++;
 	    }
    }
	
    
    if (m_LogFp != NULL)
    {
        fclose(m_LogFp);
        m_LogFp = NULL;
    }
    
    return true;
}

