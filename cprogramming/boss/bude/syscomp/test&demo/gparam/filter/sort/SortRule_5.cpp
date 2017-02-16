// ##########################################
// Source file : SortRule_5.cpp
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

#include "SortRule_5.h"

//##ModelId=3C36FC5D0282
//##Documentation
//## 只需根据msisdn和imsi绑定用户资料处理
string SortRule_5::getChannel(char * & cdr)
{
   string channel;
   ServInfo servInfo;
   //add by xiel 2003/08/11 增加对方号码品牌判断
   ServInfo servInfo_other;

   static T_GROUP_ID groupId;
   T_AREA_CODE areaCode;
   static T_USER_TYPE userType;
   static T_OFFICE_CODE officeCode;
   static T_PROVINCE_CODE provinceCode;

   cdr = m_cdrBuffer;

   //add by xiel 如果有msisdn,则根据msisdn 找
   //change by liux 短信行业端口号不以"13"开头 2004.06.18
   //if (strncmp(m_msisdn,"13",2)==0)
   if (strlen(m_msisdn)>1)
   {

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
	   return CHANNEL_NOINFO;  
	   
   }
   //add by xiel 如果没有msisdn,则根据imsi 找
   else
   {
			if( m_PM->getServInfoByImsi(m_IMSI,m_dateTime,servInfo)) {
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

	         	channel=countSum(servInfo.m_channelNo);
	         	return channel;         // 特殊通道号
	       	} else {
				//add by xiel 2002/06/19
	         	theSortLog->addNoinfoSum();
				addFields(0);
				return CHANNEL_NOINFO; 
		   	}
   }
}
