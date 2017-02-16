#include "OutputerCdr.h"
#include "SortLog.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

OutputerCdr::OutputerCdr()
{
  m_outputRuleMgr = NULL;
  m_outputParam   = NULL;
  m_gFilterInfo   = NULL;
  m_map_outputerMgr.clear();
  map_outputerCdr=NULL;
}
OutputerCdr::~OutputerCdr()
{
  m_outputRuleMgr = NULL;
  m_outputParam   = NULL;
  m_gFilterInfo   = NULL;
  m_map_outputerMgr.clear();
  if(map_outputerCdr!=NULL)
  {
    delete[] map_outputerCdr;
    map_outputerCdr=NULL;
  }
}

void OutputerCdr::mapOutclean()
{
  map<PString,OutputerCdr*>::iterator r_Itr;
     for(r_Itr=m_map_outputer.begin();
                   r_Itr!=m_map_outputer.end();r_Itr++)
  {
     r_Itr->second->m_openFlag = false;
     r_Itr->second->m_openMode = ios::out;
     //r_Itr->second->m_workfullName = "";
     //r_Itr->second->m_outfullName = "";
     //r_Itr->second->m_prefixName = "";
     //r_Itr->second->m_outpathName = "";
     r_Itr->second->m_recordNum = 0;
     r_Itr->second->m_isOpen = false;
  }
}

void OutputerCdr::setOutName(const char * r_prefixName,const char *r_fileName,const char *r_path)
{
   char t_outfullName[256+1];
   PString t_outpath;

   t_outpath  = r_path;
  if (strlen(r_path) > 0)
  {
    if (r_path[strlen(r_path)-1]!='/')
    {
       t_outpath+='/';
    }
  }
  else
  {
     t_outpath+='/';
  }

  m_workfullName = t_outpath + "#" + r_prefixName + r_fileName;

  sprintf(t_outfullName,"%s%s%s",t_outpath.c_str(),r_prefixName,r_fileName);

  for(int i=0;;i++)
  {
    //不存在文件，返回
    if(access(t_outfullName, F_OK) != 0)
    {
  		break;
    }
    //已存在，改名,后缀加".序列号"
    sprintf(t_outfullName,"%s%s%s.%d",t_outpath.c_str(),r_prefixName,r_fileName,i);
  }
  m_outfullName = t_outfullName;
}

bool  OutputerCdr::initialize(FilterGlobalInfo *r_gFilterInfo,OutputRuleMgr *r_outputRuleMgr)
{
  int i,j;
  m_gFilterInfo=r_gFilterInfo;
  m_outputRuleMgr=r_outputRuleMgr;

  outputfile_num = m_outputRuleMgr->getallOutChannelNumber();
  if(outputfile_num == 0)
  {
    #ifdef _DEBUG_
      cout<<"OutputRuleMgr::getallOutChannelNumber false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
     return false;
  }  
  map_outputerCdr = new OutputerCdr[outputfile_num];
  if(map_outputerCdr == NULL)
  {
    #ifdef _DEBUG_
      cout<<"OutputerCdr::initialize false! new memory false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
     return false;
  }
  vector<OutChanInfo>::iterator r_ChanItr;
  vector<OutputRuleMgr::OutputParam>::iterator r_OutItr;
  for(i=0,r_OutItr=m_outputRuleMgr->m_outputParam.begin();
                   r_OutItr!=m_outputRuleMgr->m_outputParam.end();r_OutItr++)
 {
    m_map_outputer.clear();
    for(j=0,r_ChanItr=r_OutItr->m_outChanInfo.begin();
                   r_ChanItr!=r_OutItr->m_outChanInfo.end();r_ChanItr++,j++)
    {
       map_outputerCdr[i+j].m_outpathName = r_ChanItr->m_desPath;
       map_outputerCdr[i+j].m_prefixName = r_ChanItr->m_prefix;
       m_map_outputer[r_ChanItr->m_channelNo] = &map_outputerCdr[i+j];
    }
    i+=j;
    m_map_outputerMgr[r_OutItr->m_subChanName] = m_map_outputer;
 }

  return true;
}

bool  OutputerCdr::open(const int &r_mode)
{
  if(m_openFlag==true)
  {
    m_fstream.close();
    m_fstream.clear();
    m_openFlag = false;
  }
  //输出模式打开文件 #为前缀
  m_fstream.open(m_workfullName.c_str(),r_mode);
  if(!m_fstream)
  {
    return false;
  }
  m_openMode = r_mode;
  m_openFlag = true;
  return true;
}

bool  OutputerCdr::open()
{
   m_outputParam = m_outputRuleMgr->getOutputParam(m_gFilterInfo->m_subChannelName);
   if(m_outputParam==NULL)
   {
     #ifdef _DEBUG_
       cout<<"m_outputParam("<<m_gFilterInfo->m_subChannelName<<"\t"<<m_gFilterInfo->m_fileName<<")output rule not defined!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return false;
   }
   //确定子通道输出
   m_gFilterInfo->setFileOutChanInfo(&(m_outputParam->m_outChanInfo));

   //单文件处理日志初始化
   m_gFilterInfo->m_sortLog->initialize(m_gFilterInfo);

   m_map_outputer.clear();
   map<PString,map<PString,OutputerCdr*> >::iterator r_Itr;
   for(r_Itr=m_map_outputerMgr.begin();
                   r_Itr!=m_map_outputerMgr.end();r_Itr++)
  {
     if(m_gFilterInfo->m_subChannelName == r_Itr->first)
     {
        m_map_outputer = r_Itr->second;
        mapOutclean(); //重置输出子通道所有状态
        break;
     }
  }
  if(m_map_outputer.size()<=0)
  {
     #ifdef _DEBUG_
       cout<<"get OutputerCdr rule failed!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     return false;
  }

  //置各通道打开状态
  map<PString,OutputerCdr*>::iterator r_outItr;
  for(r_outItr=m_map_outputer.begin();r_outItr!=m_map_outputer.end();r_outItr++)
  {
      r_outItr->second->setName(m_gFilterInfo->m_fileName.c_str(),r_outItr->second->m_outpathName.c_str());//Inoutputer基类获取文件、路径
      r_outItr->second->setOutName(r_outItr->second->m_prefixName.c_str(),m_gFilterInfo->m_fileName.c_str(),r_outItr->second->m_outpathName.c_str());//OutputerCdr派生类获取文件、路径
  }
  return true;
}


//输出话单
bool  OutputerCdr::output()
{
  bool multflag,t_backupflag,t_outflag=false;
  if(strncasecmp(m_gFilterInfo->m_cdr[ATTRI_BACK_CHANNEL].c_str(),CHANNEL_BACKUP.c_str(),CHANNEL_BACKUP.length())==0)
  	t_backupflag = false;
  else
  	t_backupflag = true;

  if(strncasecmp(m_gFilterInfo->m_cdr[ATTRI_MUX_CHANNEL].c_str(),CHANNEL_MUX.c_str(),CHANNEL_MUX.length())==0)
  	multflag = false;
  else
  	multflag = true;

  map<PString,OutputerCdr*>::iterator r_outItr;
  for(r_outItr=m_map_outputer.begin();r_outItr!=m_map_outputer.end();r_outItr++)
  {
     //cout<<""<<r_outItr->first<<"\t"<<r_outItr->second->m_outpathName<<"\t"<<r_outItr->second->m_prefixName<<"\t"<<endl;

     if(m_gFilterInfo->m_cdr[ATTRI_BACK_CHANNEL] == r_outItr->first.c_str())
     {
        //获取地址
        r_outItr->second->initialdetail(m_gFilterInfo,m_outputParam);
        //有记录才会打开文件 add by shenglj 2011-04-21 17:18:08
        if(!r_outItr->second->m_isOpen)
        {
          if(r_outItr->second->open(ios::out)==false)
          {
            #ifdef _DEBUG_
              cout<<"open file failed!"<<__FILE__<<__LINE__<<endl;
              cout<<"m_workfullName="<<r_outItr->second->m_workfullName<<endl;
            #endif
            return false;
          }
          r_outItr->second->m_isOpen = true;
        }
        //输出话单
        if(!r_outItr->second->outputdetail(r_outItr->first.c_str()))
        {
           return false;
        }
        t_backupflag = true;

        //写入记录日志累加
        m_gFilterInfo->m_sortLog->addLogCdrs(r_outItr->first.c_str(),r_outItr->second->m_outfullName.c_str(),"1");
     }

     if(m_gFilterInfo->m_cdr[ATTRI_MUX_CHANNEL] == r_outItr->first.c_str())
     {
        //获取地址
        r_outItr->second->initialdetail(m_gFilterInfo,m_outputParam);
        //有记录才会打开文件 add by shenglj 2011-04-21 17:18:08
        if(!r_outItr->second->m_isOpen)
        {
          if(r_outItr->second->open(ios::out)==false)
          {
            #ifdef _DEBUG_
              cout<<"open file failed!"<<__FILE__<<__LINE__<<endl;
              cout<<"m_workfullName="<<r_outItr->second->m_workfullName<<endl;
            #endif
            return false;
          }
          r_outItr->second->m_isOpen = true;
        }
        //输出话单
        if(!r_outItr->second->outputdetail(r_outItr->first.c_str()))
        {
           return false;
        }
        multflag = true;

        //写入记录日志累加
        m_gFilterInfo->m_sortLog->addLogCdrs(r_outItr->first.c_str(),r_outItr->second->m_outfullName.c_str(),"2");
     }

     if(m_gFilterInfo->m_cdr[ATTRI_CDR_CHANNEL] == r_outItr->first.c_str())
     {
        //获取地址
        r_outItr->second->initialdetail(m_gFilterInfo,m_outputParam);
        //有记录才会打开文件 add by shenglj 2011-04-21 17:18:08
        if(!r_outItr->second->m_isOpen)
        {
          if(r_outItr->second->open(ios::out)==false)
          {
            #ifdef _DEBUG_
              cout<<"open file failed!"<<__FILE__<<__LINE__<<endl;
              cout<<"m_workfullName="<<r_outItr->second->m_workfullName<<endl;
            #endif
            return false;
          }
          r_outItr->second->m_isOpen = true;
        }
        //输出话单
        if(!r_outItr->second->outputdetail(r_outItr->first.c_str()))
        {
           return false;
        }
        t_outflag = true;

        //写入记录日志累加
        m_gFilterInfo->m_sortLog->addLogCdrs(r_outItr->first.c_str(),r_outItr->second->m_outfullName.c_str(),"0");
     }
     //正常和备份方式都输出（如果需要多输出功能）
     if(multflag && t_backupflag && t_outflag)
     {
        return true;
     }
  }

   #ifdef _DEBUG_
     cout<<"out cdr channel '"<<m_gFilterInfo->m_cdr[ATTRI_CDR_CHANNEL]<<"' not defined from filter.cfg!"<<__FILE__<<__LINE__<<endl;
   #endif
  theErrorMessages->insert(E_OUT_CHANNEL_DEFINED,"out cdr channel '" + m_gFilterInfo->m_cdr[ATTRI_CDR_CHANNEL] + "' not defined from filter.cfg!");
  return false;
}

bool OutputerCdr::initialdetail(FilterGlobalInfo *r_gFilterInfo,OutputRuleMgr::OutputParam * r_outputParam)
{
  m_gFilterInfo=r_gFilterInfo;
  m_outputParam=r_outputParam;
  return true;
}

bool OutputerCdr::outputdetail(const string &r_channelName)
{
  FilterCdr &t_cdr=m_gFilterInfo->m_cdr;
  FilterCdr &t_originalcdr=m_gFilterInfo->m_originalcdr;
  int  t_desLen=0,t_fldLen=0;
  int  t_sOffSet,t_eOffSet;
  long t_sumValue=0;
  int i=0;

  //1. 拼凑话单
  if(m_gFilterInfo->m_Config->m_specialOutField.count(r_channelName))
  {
	  if(t_cdr.outputCdr((m_gFilterInfo->m_Config->m_specialOutField)[r_channelName])==false)
	  {
	    #ifdef OUT_DEBUG_
	      cout<<"t_cdr.outputCdr(m_Config->m_specialOutField) false!"
	          <<__FILE__<<__LINE__<<endl;
	    #endif
	    return false;
	  }
  }
  else
  {
	  if(t_cdr.outputCdr(m_outputParam->m_bFormat)==false)
	  {
	    #ifdef OUT_DEBUG_
	      cout<<"t_cdr.outputCdr(m_outputParam->m_bFormat) false!"
	          <<__FILE__<<__LINE__<<endl;
	    #endif
	    return false;
	  }
  }

//cout<<"t_cdr.m_desBuff:"<<t_cdr.m_desBuff<<endl;
  //2. 写记录
  if(write(t_cdr.m_desBuff)==false)
  {
    #ifdef _DEBUG_
      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  //记录条数增加
  m_recordNum++;

  #ifdef OUT_DEBUG_
     cout<<"----------------------------话单记录开始:"<<endl;
     if(m_gFilterInfo->m_Config->m_specialOutField.count(r_channelName))
     {
       t_cdr.dumpCdrInfo((m_gFilterInfo->m_Config->m_specialOutField)[r_channelName]);
       cout<<"输出通道:                    = "<<r_channelName<<endl;
     }
     else
     {
       t_cdr.dumpCdrInfo(m_outputParam->m_bFormat);
       cout<<"输出通道:                    = "<<r_channelName<<endl;
     }
    cout<<"-----------------------------话单记录结束!\n"<<endl;
  #endif

  return true;
}

bool OutputerCdr::close()
{
  map<PString,OutputerCdr*>::iterator r_outItr;
  for(r_outItr=m_map_outputer.begin();r_outItr!=m_map_outputer.end();r_outItr++)
  {
     r_outItr->second->Inoutputer::close();
  }
  return true;
}

//文件级提交
bool OutputerCdr::fileCommit()
{
  map<PString,OutputerCdr*>::iterator r_outItr;
  for(r_outItr=m_map_outputer.begin();r_outItr!=m_map_outputer.end();r_outItr++)
  {
//cout<<"m_recordNum: "<<r_outItr->second->m_recordNum<<" , "<<"m_outfullName: "<<r_outItr->second->m_outfullName<<__FILE__<<__LINE__<<endl;
     if(r_outItr->second->m_recordNum==0)
     {
        /*
        if(!r_outItr->second->unlinkWorkFile())
        {
          cout<<"     "<<__FILE__<<__LINE__<<endl;
          return false;
        }
        */
     }
     else if(!r_outItr->second->renameFile())
     {
cout<<"     "<<__FILE__<<__LINE__<<endl;
        return false;
     }
  }

  return true;
}

bool OutputerCdr::fileRollback()
{
  map<PString,OutputerCdr*>::iterator r_outItr;
  for(r_outItr=m_map_outputer.begin();r_outItr!=m_map_outputer.end();r_outItr++)
  {
    if(!r_outItr->second->unlinkWorkFile())
    {
      return false;
    }
  }
  return true;
}


bool  OutputerCdr::renameFile()
{
  if(m_openFlag == true)
 {
    m_fstream.close();
    m_fstream.clear();
    m_openFlag = false;
 }
  if(::rename(m_workfullName.c_str(),m_outfullName.c_str())!=0)
  {
     #ifdef _DEBUG_
       cout<<"rename "<<m_outfullName<<" failed!"<<__FILE__<<__LINE__<<endl;
     #endif
    return false;
  }
  return true;
}

bool  OutputerCdr::unlinkWorkFile() const
{
  //如果空文件不存在，正确返回 add by shenglj 2010-06-17 13:52:01
  if(access(m_workfullName, F_OK) != 0)
  {
  	return true;
  }
  
  if(::unlink(m_workfullName.c_str())!=0)
  {
    return false;
  }
  return true;
}
