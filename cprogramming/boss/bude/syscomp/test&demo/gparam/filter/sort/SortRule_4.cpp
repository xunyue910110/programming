// ##########################################
// Source file : SortRule_4.cpp
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20020424
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "SortRule_4.h"

//##ModelId=3C36FAD500E3
//##Documentation
//## 只需根据MSISDN绑定用户资料处理(智能网分拣)
string SortRule_4::getChannel(char * & cdr)
{
   string channel;
   ServInfo servInfo;
   //add by xiel 2003/08/11 增加对方号码品牌判断
   ServInfo servInfo_other;

   cdr = m_cdrBuffer;

   if( m_PM->getServInfoByPhoneNumber(m_msisdn,m_dateTime,servInfo)) {
   	  
  	 //add by xiel 2003/08/11 增加对方号码品牌判断 求对方号码的信息
     memset(&servInfo_other,0,sizeof(ServInfo)); 
     if (strncmp(m_otherParty,"13",2)==0)
     	m_PM->getServInfoByPhoneNumber(m_otherParty,m_dateTime,servInfo_other);
	 else if(strncmp(m_otherParty,"8613",4)==0)
     	m_PM->getServInfoByPhoneNumber(m_otherParty+2,m_dateTime,servInfo_other);
	 //add by xiel 2003/08/26 对1795113 或17951013 等接入号码的判断
	 else if(strncmp(m_otherParty,"179",3)==0 && strncmp(m_otherParty+5,"13",2)==0)
     	m_PM->getServInfoByPhoneNumber(m_otherParty+5,m_dateTime,servInfo_other);
	 else if(strncmp(m_otherParty,"179",3)==0 && strncmp(m_otherParty+5,"013",3)==0)
     	m_PM->getServInfoByPhoneNumber(m_otherParty+6,m_dateTime,servInfo_other);
	 		          
     //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
     bindInfo(servInfo,servInfo_other);     // 绑定资料

     return countSum(servInfo.m_channelNo);
   }
   theSortLog->addNoinfoSum();
   addFields(0);
   return CHANNEL_NOINFO;        // 无主话单通道

/*   if( m_PM->getServInfoByImsi(m_IMSI,m_dateTime,servInfo)) {
      bindInfo(servInfo);     // 绑定资料
      return countSum(servInfo.m_channelNo);
   }
   theSortLog->addNoinfoSum();
   addFields(0);

   return CHANNEL_NOINFO;        // 无主话单通道
*/
}

