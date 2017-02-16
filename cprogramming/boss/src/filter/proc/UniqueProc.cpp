#include "UniqueProc.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "FilterConfig.h"

INSTALL_PROCESSBASE_INSTANCE(UniqueProc);

UniqueProc::UniqueProc()
{
  m_dividRule = NULL;
  m_vectorDividParam = NULL;
}

UniqueProc::~UniqueProc()
{
  m_dividRule = NULL;
  m_vectorDividParam = NULL;
}

bool UniqueProc::initialize()
{
  return true;
}

int UniqueProc::reset()
{
  return 0;
}

int UniqueProc::execute()
{
	//不需要排重类型
	if(m_FilterInfo->m_unique_flag == FLAG_NO)
	{
		return 0;
	}

  //错单不进行排重 直接返回
	if(m_pcdr[ATTRI_ERROR_CODE].value() > DEFAULT_ERROR)
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_ERROR;
		return 0;
	}

  if(!getConfigBySource())
  {
    #ifdef _DEBUG_
  	theErrorMessages->insert(E_GET_DIVID_PARAM,m_FilterInfo->m_fullPathName + " get rr param by source_type fail!");
    #endif
  	return -1;
  }

  if(!getConfigByCdr())
  {
  	return 0;
  }

  if(m_vectorDividParam->m_DividParamList.size()<=0)
  {
  	//m_pcdr[ATTRI_RR_FLAG] = "1";
    return 0;
  }

  setMaxLength(m_dividRule->m_maxLength);

  string r_string;
  string a_string; //add by xiangbin at 2011-4-6 19:47:45
  //拼凑FID字段
  vector<DividRule::DividParam>::iterator r_Itr;
  for(r_Itr=m_vectorDividParam->m_DividParamList.begin();
                   r_Itr!=m_vectorDividParam->m_DividParamList.end();r_Itr++)
 {
    if(!AbstractField(r_Itr->m_cdrid,r_Itr->m_pos,r_Itr->m_len,r_Itr->m_fixstr,r_string))
    {
    #ifdef _DEBUG_
			cout<<m_FilterInfo->m_fullPathName<<" 排重字段下标:"<<r_Itr->m_fieldId<<" CDR内部下标:"<<r_Itr->m_cdrid<<" 字段偏移量:"<<r_Itr->m_pos
			    <<" 字段获取长度"<<r_Itr->m_len<<" define error!"
			    <<__FILE__<<__LINE__<<endl;
    #endif
	    theErrorMessages->insert(E_GET_DIVID_PARAM,m_FilterInfo->m_fullPathName + " rr input param define error!");
    	return -1;
    }
    //m_pcdr[ATTRI_RR_FID] = m_pcdr[ATTRI_RR_FID] + r_string;
    a_string =a_string + r_string; //add by xiangbin at 2011-4-6 19:48:11

 }
 
  if(m_vectorDividParam->m_reWrite == "N" || m_vectorDividParam->m_reWrite == "n")
  {
  	m_pcdr[ATTRI_RR_FID] = m_pcdr[ATTRI_RR_FID] + a_string;
  }	
  else
  {
  	m_pcdr[ATTRI_RR_FID] = a_string;
  }
 
  if(m_maxLength>m_pcdr[ATTRI_RR_FID].length())
  {
  	m_pcdr[ATTRI_RR_FID] = m_pcdr[ATTRI_RR_FID].substr(0,m_maxLength);
  }

  //cout<<"m_pcdr[ATTRI_RR_FID] "<<m_pcdr[ATTRI_RR_FID]<<"  "<<__FILE__<<__LINE__<<endl;

  return 0;
}

void UniqueProc::setMaxLength(int r_maxLength)
{
  m_maxLength = r_maxLength;
}

bool UniqueProc::AbstractField(const int &r_cdrid,const int &r_offset,const int &r_length,const string &fix_str,string &r_str)
{
  if(0 == r_cdrid)
  {
  	r_str = fix_str;
    return true;
  }

  if(r_offset<0 || r_length<0)
  {
  	if(r_cdrid>0 && r_cdrid<=ATTRI_ORG_MAX)
  	{
  		r_str = m_pcdr[r_cdrid];
  		return true;
  	}
  	else
  	{
  		return false;
  	}
  }
  else if(r_length==0 && r_offset==0)
  {
     r_str = m_pcdr[r_cdrid];
  	 return true;
  }
  else if(r_length==0)
  {
  	return false;
  }

  if(m_pcdr[r_cdrid].length()>=r_offset+r_length)
  {
  	r_str = m_pcdr[r_cdrid].substr(r_offset,r_length);
  }
  else if(m_pcdr[r_cdrid].length()>=r_offset)
  {
  	r_str = m_pcdr[r_cdrid].substr(r_offset,m_pcdr[r_cdrid].length()-r_offset);
  }
  else
  {
  	r_str = m_pcdr[r_cdrid];
  }
  
	return true;
}

bool UniqueProc::getConfigBySource()
{
	m_dividRule = m_Config->m_dividRuleMgr.getDividRule(m_pcdr[ATTRI_SOURCE_TYPE].c_str());
	if(m_dividRule == NULL)
	{
    #ifdef _DEBUG_
      cout<<"m_Config->m_dividRuleMgr.getDividRule false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
	}
  return true;
}


bool UniqueProc::getConfigByCdr()
{
	m_vectorDividParam = m_dividRule->getVectorDividParamRule(&m_cdr);
  if(m_vectorDividParam == NULL)
	{
/*
    #ifdef _DEBUG_
      cout<<"该记录不需进行排重或排重规则配置缺失!"
          <<__FILE__<<__LINE__<<endl;
    #endif
*/
    return false;
	}
  return true;
}
