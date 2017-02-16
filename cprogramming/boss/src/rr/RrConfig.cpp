#include "RrConfig.h"
#include "base/SysParam.h"
#include "AbstractFieldSelector.h"
#include "RrConstDef.h"
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include "RrException.h"
#include "RrConstDef.h"

RrConfig::RrConfig()
{
  m_fidMaxNum   = RR_MEMFID_STDMAXNUM; //100万条 相当于大通道2小时左右的量
  m_fidFileSize = RR_FIDFILE_SIZE;
  m_onefileTIme = RR_ONEFILE_TIME;
  m_rrUpdateFlag= true;
}
RrConfig::~RrConfig()
{
}

//r_rootPath为rr所在模块的配置根路径
//r_channelNo为通道号 
bool RrConfig::getSysParam(SysParam *r_sysParam,const char *r_rootPath,const int &r_channelNo)
{
  string t_rootPath = r_rootPath;
  string t_chPath,t_fileRulePath,t_path,t_section;
  char   t_channelNo[10+1];
  map<string,string> t_nodes;
  vector<string>     t_fileTypeNames;
  int                t_fileType_merge=0;
  FileRrRule         t_fileRrRule;
  vector<PString>    t_filePreList;
  PString            t_filePre;
  char               t_delimiter;
  
  m_channelNo = r_channelNo;
  sprintf(t_channelNo,"%d",r_channelNo);
  t_fileRulePath = t_rootPath + SECTDELIM + "FILE_TYPE";
  t_chPath = t_rootPath + SECTDELIM + t_channelNo;
  
  r_sysParam->getValue(t_chPath, t_nodes);
  //取该通道下的目录配置信息等
  m_inputPath = t_nodes["SOURCEPATH"].c_str();
  m_errPath   = t_nodes["ERRBAKPATH"].c_str();
  m_uniqPath  = t_nodes["DIVIDE_UNIQUE_PATH"].c_str();
  m_repeatPath= t_nodes["DIVIDE_REPEAT_PATH"].c_str();
  m_workPath  = t_nodes["WORKPATH"].c_str();
  m_fidPath   = t_nodes["FIDFILEPATH"];
  m_mergeCount  = atol(t_nodes["OPTIMUMLINECOUNT"].c_str());
  m_waiteTimeOut= atol(t_nodes["WAITTIMEOUT"].c_str());
  m_fidMemTime  = atol(t_nodes["MEMFIDTIME"].c_str());
  m_fidMaxNum   = atol(t_nodes["MEMFID_MAXNUM"].c_str());
  if(atol(t_nodes["FIDFILESIZE"].c_str())>0)
  {
    m_fidFileSize = atol(t_nodes["FIDFILESIZE"].c_str())*1024*1024;
  }
  if(strcasecmp(t_nodes["RRUPDATEFLAG"].c_str(),"false")==0)
  {
    m_rrUpdateFlag = false;
  }

  m_fidMaxNum=m_fidMaxNum?m_fidMaxNum:RR_MEMFID_STDMAXNUM;
  m_fidMemTime = m_fidMemTime?
                 (m_fidMemTime<RR_MEMFID_MINTIME?
                  RR_MEMFID_MINTIME:m_fidMemTime)
                 :RR_MEMFID_STDTIME;
  //校验这些目录是否正常
  if(isValidPath(m_inputPath.c_str())==false ||
     isValidPath(m_workPath.c_str())==false  ||
     isValidPath(m_uniqPath.c_str())==false  ||
     isValidPath(m_repeatPath.c_str())==false  ||
     isValidPath(m_errPath.c_str())==false  ||
     isValidPath(m_fidPath.c_str())==false )
  {
    #ifdef _DEBUG_
      cout<<"无效的目录，请检查该通道对应的配置目录!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
     
     
  
  //取文件排重规则配置信息
  DelimiterSelector* t_sel;
  char t_buffer[1024];
  
  r_sysParam->setSectionPath(t_fileRulePath);
  while (r_sysParam->getSubSection(t_section))
  {
    t_fileTypeNames.push_back(t_section);
  }
  m_rrRuleList.clear();
  for(vector<string>::const_iterator t_strItr=t_fileTypeNames.begin();
      t_strItr!=t_fileTypeNames.end();++t_strItr)
  {
    t_path=t_fileRulePath+SECTDELIM+*t_strItr;
    t_nodes.clear();
    r_sysParam->getValue(t_path, t_nodes);
    
    //取分割符
    if(strlen(t_nodes["DIVIDESIGN"].c_str())==0)
    {
      t_delimiter = ',';
    }else
    {
      t_delimiter=t_nodes["DIVIDESIGN"][0];
    }
    t_fileRrRule.m_delimiter = t_delimiter;
    t_fileRrRule.m_rrType = atoi(t_nodes["RRTYPE"].c_str()); //排重类型
    if(t_fileRrRule.m_rrType==E_RR_RRTYPE_NONE)
    {
      t_fileRrRule.m_fidSelectRule   = "";
      t_fileRrRule.m_fidSelect       = NULL;
      t_fileRrRule.m_rrFlagSelectRule= "";
      t_fileRrRule.m_rrFlagSelect    = NULL;
      t_fileRrRule.m_beginTimeRule   = "";
      t_fileRrRule.m_beginTimeSelect = NULL;
      t_fileRrRule.m_endTimeRule     = "";
      t_fileRrRule.m_endTimeSelect   = NULL;
      t_fileRrRule.m_maxFidLen       = 0;
    }
    else if(t_fileRrRule.m_rrType == E_RR_RRTYPE_FILE) //added by hanyu 08-11-25
    {
    	t_fileRrRule.m_maxFidLen = atoi(t_nodes["MAXKEYLENGTH"].c_str());;
    	t_fileRrRule.m_fileType  = t_nodes["FILETYPE"];
    	
    	strcpy(t_buffer,t_nodes["DIVIDEFORMAT"].c_str());
      t_fileRrRule.m_fidSelectRule = t_buffer;
      t_fileRrRule.m_fidSelect = generateSelect(t_buffer,t_delimiter,t_fileRrRule.m_maxFidLen);
    	
    	t_fileRrRule.m_rrFlagSelectRule = "";
      t_fileRrRule.m_rrFlagSelect = NULL;
      
      t_fileRrRule.m_beginTimeRule = "";
      t_fileRrRule.m_beginTimeSelect = NULL;
      
      t_fileRrRule.m_timeDuration  = atoi(t_nodes["MAX_DURATION"].c_str());
      
    }else
    {
      t_fileRrRule.m_maxFidLen = atoi(t_nodes["MAXKEYLENGTH"].c_str());
      t_fileRrRule.m_fileType  = t_nodes["FILETYPE"];
      //FID
      strcpy(t_buffer,t_nodes["DIVIDEFORMAT"].c_str());
      t_fileRrRule.m_fidSelectRule = t_buffer;
      t_fileRrRule.m_fidSelect = generateSelect(t_buffer,t_delimiter,t_fileRrRule.m_maxFidLen);      
            
      strcpy(t_buffer,t_nodes["RRFLAG"].c_str());
      t_fileRrRule.m_rrFlagSelectRule = t_buffer;
      t_fileRrRule.m_rrFlagSelect = generateSelect(t_buffer,t_delimiter);
      t_fileRrRule.setRrFlagPos();
      
      strcpy(t_buffer,t_nodes["BEGING_TIME_FORMAT"].c_str());
      t_fileRrRule.m_beginTimeRule = t_buffer;
      t_fileRrRule.m_beginTimeSelect = generateSelect(t_buffer,t_delimiter);
      
      if(t_fileRrRule.m_rrType == E_RR_RRTYPE_CONTAIN)
      {
        strcpy(t_buffer,t_nodes["END_TIME_FORMAT"].c_str());
        if(strlen(t_buffer)<=0)
        {
          #ifdef _DEBUG_
            cout<<"包容排重规则 结束时间提取规则END_TIME_FORMAT未配置!"
                <<__FILE__<<__LINE__<<endl;
          #endif
          return false;
        }
        t_fileRrRule.m_endTimeRule   = t_buffer;
        t_fileRrRule.m_endTimeSelect = generateSelect(t_buffer,t_delimiter);
        t_fileRrRule.m_timeDuration  = atoi(t_nodes["MAX_DURATION"].c_str());
        if(t_fileRrRule.m_timeDuration<=RR_MEMFID_MINTIME)
        {
          t_fileRrRule.m_timeDuration = RR_MEMFID_MINTIME;
        }
      }else
      {
        t_fileRrRule.m_endTimeRule  ="";
        t_fileRrRule.m_endTimeSelect=NULL;
      }
    }
    t_fileRrRule.m_fileType_merge = t_fileType_merge;

    t_filePre = t_nodes["PREFIX"].c_str();
    t_filePre.split(',',t_filePreList,true);
    for(vector<PString>::const_iterator r_psItr=t_filePreList.begin();
        r_psItr!=t_filePreList.end();++r_psItr)
    {
      t_fileRrRule.m_filePrefix = *r_psItr;
      m_rrRuleList.push_back(t_fileRrRule);
    }
    ++t_fileType_merge;
  }
  ::sort(m_rrRuleList.begin(),m_rrRuleList.end());
  return true;
}

AbstractFieldSelector* RrConfig::generateSelect(char *r_strRule,const char r_delimiter,const size_t r_maxLen)
{
  int t_fieldIndex,t_offset,t_length;
  AbstractFieldSelector* t_anySelect = new AnySelector();
  t_anySelect->setMaxLength(r_maxLen);
  for(char *t_s = strtok(r_strRule, ")"); t_s != NULL; 
      t_s = strtok(NULL, ")"))
  {
    if(sscanf(t_s, t_s == r_strRule? "%d%*[ (]%d%*[ ,]%d" : "%*[, ]%d%*[ (]%d%*[ ,]%d",
        &t_fieldIndex, &t_offset, &t_length) != 3)
    {
      #ifdef _DEBUG_
        cout<<"DIVIDEFORMAT config info error!{"
            <<r_strRule<<"}"<<endl;
      #endif
      RR_THROW_CP1(E_RR_PARAM_INVALID_SELRULE,"字段选取配置参数错误:{%s}!",r_strRule);
      return false;
    }
    
    AbstractFieldSelector* t_sel = new DelimiterSelector(r_delimiter,t_fieldIndex-1);
    if(t_length != 0)
    {
      t_sel->appendSubSelector(new FixSelector(t_offset, t_length));
    }
    t_anySelect->appendSubSelector(t_sel);
  }    
  FileRrRule::m_selectors.push_back(t_anySelect);
  return t_anySelect;
}

bool RrConfig::getFilePrefixList(vector<string>& r_vPrefixList)
{
	r_vPrefixList.clear();
	vector<FileRrRule>::iterator t_it=m_rrRuleList.begin();
	for (; t_it != m_rrRuleList.end() ; t_it++)
	{
		r_vPrefixList.push_back(t_it->m_filePrefix.c_str());
	}
	return true;
}
//##ModelId=4899AA18030D
const FileRrRule* RrConfig::getRrRule(const char* r_fileName)
{
  vector<FileRrRule>::const_iterator t_itr;
  FileRrRule  t_fileRule;
  t_fileRule.m_filePrefix =r_fileName;
  t_itr=::lower_bound(m_rrRuleList.begin(),m_rrRuleList.end(),
                      t_fileRule);
  if(t_itr==m_rrRuleList.end())
  {
    return NULL;
  }else if(strncmp(t_itr->m_filePrefix.c_str(),r_fileName,t_itr->m_filePrefix.length())!=0)
  {
    return NULL;
  }else
  {
    return &(*t_itr);
  }
}

bool  RrConfig::end()
{
  return true;
}

ostream& operator<<(ostream &r_os,const RrConfig &r_config)
{
  r_os<<"m_inputPath   ="<<r_config.m_inputPath<<endl;
  r_os<<"m_workPath    ="<<r_config.m_workPath<<endl;
  r_os<<"m_uniqPath    ="<<r_config.m_uniqPath<<endl;
  r_os<<"m_repeatPath  ="<<r_config.m_repeatPath<<endl;
  r_os<<"m_errPath     ="<<r_config.m_errPath<<endl;
  r_os<<"m_waiteTimeOut="<<r_config.m_waiteTimeOut<<endl;
  r_os<<"m_fidPath     ="<<r_config.m_fidPath<<endl;
  r_os<<"m_fidMemTime  ="<<r_config.m_fidMemTime<<endl;
  r_os<<"m_fidMaxNum   ="<<r_config.m_fidMaxNum<<endl;
  r_os<<"m_fidFileSize ="<<r_config.m_fidFileSize<<endl;
  r_os<<"-------------FileRrRule begin ------------------"<<endl;
  for(vector<FileRrRule>::const_iterator r_itr=r_config.m_rrRuleList.begin();
      r_itr!=r_config.m_rrRuleList.end();++r_itr)
  {
    r_os<<*r_itr;
    r_os<<"-------------------------------------------"<<endl;
  }
  r_os<<"-------------FileRrRule end ------------------"<<endl;
  return r_os;
}

bool RrConfig::isValidPath(const char *r_path) const
{
  struct stat r_buf;

  if(lstat(r_path,&r_buf)<0)
  {
    RR_THROW_CP1(E_RR_PARAM_INVALID_DIR,"无效的目录:{%s}!",r_path);
    return false;
  }
  if(S_ISDIR(r_buf.st_mode))
    return true;
  else
  {
    RR_THROW_CP1(E_RR_PARAM_INVALID_DIR,"无效的目录:{%s}!",r_path);
    return false;
  }
}


