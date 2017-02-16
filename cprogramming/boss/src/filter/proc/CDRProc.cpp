#include "CDRProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

CDRProc::CDRProc()
{
}

CDRProc::~CDRProc()
{
}

bool CDRProc::initialize()
{
  return true;
}

int CDRProc::reset()
{
	m_pcdr[ATTRI_PROV_CHANNEL] = "0";
  m_pcdr[ATTRI_CITY_CHANNEL] = "0";
  return 0;
}

int CDRProc::execute()
{
  return 0;
}

bool CDRProc::getChanByRule()
{
  vector<OutChanInfo>::iterator r_Itr;
  for (r_Itr=(m_FilterInfo->m_outChanInfo)->begin();r_Itr!=(m_FilterInfo->m_outChanInfo)->end(); r_Itr++)
   {
//cout<<"-------------r_Itr->m_expression  = "<<r_Itr->m_expression<<endl;
//cout<<"-------------r_Itr->m_channelNo   = "<<r_Itr->m_channelNo<<endl;
   	 if(r_Itr->m_expression.length()<=0)
   	 	 continue;

     if(strncasecmp(r_Itr->m_channelNo.c_str(),CHANNEL_MUX.c_str(),CHANNEL_MUX.length())!=0 &&
         strncasecmp(r_Itr->m_channelNo.c_str(),CHANNEL_BACKUP.c_str(),CHANNEL_BACKUP.length())!=0)
     {
        if ((r_Itr->m_outRule)->matches())
        {
           m_pcdr[ATTRI_CDR_CHANNEL]=r_Itr->m_channelNo.c_str();
        }
     }
     if(strncasecmp(r_Itr->m_channelNo.c_str(),CHANNEL_MUX.c_str(),CHANNEL_MUX.length())==0)
     {
        if((r_Itr->m_outRule)->matches())
        {
           m_pcdr[ATTRI_MUX_CHANNEL]=r_Itr->m_channelNo.c_str();
        }
     }
     if(strncasecmp(r_Itr->m_channelNo.c_str(),CHANNEL_BACKUP.c_str(),CHANNEL_BACKUP.length())==0)
     {
        if((r_Itr->m_outRule)->matches())
        {
           m_pcdr[ATTRI_BACK_CHANNEL]=r_Itr->m_channelNo.c_str();
        }
     }
  }

  return true;
}

bool CDRProc::getTrash()
{	
  return true;
}

bool CDRProc::getError()
{
	int r_error=0;
	 //r_error=atoi(m_pcdr[ATTRI_ERROR_CODE].c_str());
	 r_error=m_pcdr[ATTRI_ERROR_CODE].value();
   if (r_error>0)
   {
       m_pcdr[ATTRI_ERROR_FLAG]=ATTRI_ERROR_Y;
       return true;
   }
   else
       return false;
}

bool CDRProc::isNationalImsi(const T_ATTRI_VAL &r_imsi)
{
	if(r_imsi.substr(0,5)=="46000"||
		 r_imsi.substr(0,5)=="46001"||
		 r_imsi.substr(0,5)=="46002"||
		 r_imsi.substr(0,5)=="46007" ) //TD短信话单删除 2008-3-10 15:53
     return true;
  else
  	 return false;
}

bool CDRProc::isSelfMobile(const T_ATTRI_VAL &phoneNubmer)
{
	T_ASP asp;
	if(m_globalManager->getAspByMsisdnHeader(phoneNubmer,m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],asp))
	{
		if(atoi(asp.c_str()) == m_Config->m_operatorType) return true;
	}
  return false;
}

bool CDRProc::isMobile(const T_ATTRI_VAL &phoneNubmer)
{
	T_ASP asp;
	if(m_globalManager->getAspByMsisdnHeader(phoneNubmer,m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],asp))
	{
		return true;
	}
  return false;
}
