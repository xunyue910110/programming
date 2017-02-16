// ##########################################
// Source file : SortRule_2.cpp
// System      : Mobile Billing System
// Version     : 0.1.2
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020321
// Update      : 20020424
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "SortRule_2.h"

//## 天津移动BOSS专用分拣处理（MSC/SSP话单）
string SortRule_2::getChannel(char * & cdr)
{
    string channel;
    
    channel     = "-1";
    cdr         = m_cdrBuffer;
    
    switch (m_callType)  {
    case 11:             // POC
       channel = pocSort();
       break;
    default:
       channel = otherSort();
       break;
    }
    
    return channel;
}

string SortRule_2::otherSort()
{
    char imsiSegI[6];

    memcpy(imsiSegI,m_IMSI,5);
    imsiSegI[5]=0;

    if ( m_PM->isImsiI(imsiSegI) )  { //(1)国际漫游来访判断
       theSortLog->addIrSum();
       addFields(0);
       return CHANNEL_IR;                   

    } else if (strncmp(m_rateInd,"P",1)==0 ) {
       return PSort();                 //(2)专网分检 add by liux 2004.09.09

    } else if ( strncmp(m_msisdn,"13",2)==0 || strncmp(m_msisdn,"013",3)==0 ) {
       return nationalSort();          //(3) 国内话单分拣
    
    }else {
       theSortLog->addErrorSum();
       addFields(INVALID_IMSI);        //(4) 添加错误代码为：国际IMSI代码错
       return CHANNEL_ERROR;
    }

    return CHANNEL_ERROR;
}

//##ModelId=3C99CA09005B
string SortRule_2::nationalSort()
{
   string channelNo;
   ServInfo servInfo;
   //add by xiel 2003/08/11 增加对方号码品牌判断
   ServInfo servInfo_other;
   	
   static T_GROUP_ID groupId;
   T_AREA_CODE areaCode;
   static T_USER_TYPE userType;
   static T_OFFICE_CODE officeCode;
   static T_PROVINCE_CODE provinceCode;

   // 智能网话单判断
   /*if( m_PM->getHomeAreaCodeByMsisdnS(m_msisdn) ) {
      theSortLog->addTrashSum();

      return  CHANNEL_TRASH;
   }*/
    //add by xiel 2004/05/10 神州行话单分拣
    userType="000";
    if (m_PM->getHomeAreaCodeBySSP(m_msisdn,areaCode,groupId,userType,officeCode))
	{
    	if ((m_callType!=1)
    	     || (strncmp(m_otherParty,"12590",5)!=0))
    	{	  	
      		theSortLog->addTrashSum();
      		return  CHANNEL_TRASH;
      	}
      	if (userType[2]=='1') //省内话单
      	{
      		//单独通道下分拣到
      		theSortLog->addSpecialSum();
      		addFields(0);
         	return CHANNEL_SCP;       //省内神州行业务话单分拣到特殊通道scp下,单独批价,给预付费网关
      	}
      	else 	//省际话单
      	{
      		theSortLog->addNrSum();
      		addFields(0);
      		return CHANNEL_NR;       // 来访神州行业务话单合在正常来访通道下,上发Q文件
      	}	
   	}

   // 省内号码判断
   if( m_PM->getHomeAreaCodeByMsisdnP(m_msisdn,areaCode,groupId,userType,officeCode) )
   {
      if( m_PM->getServInfoByPhoneNumber(m_msisdn,m_dateTime,servInfo) ) {
         
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
		 		          
         channelNo=countSum(servInfo.m_channelNo);
         //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
         bindInfo(servInfo,servInfo_other);     // 绑定资料

         return channelNo;       // 省内通道号
      } else {
         //add by xiel 2002/06/19
         theSortLog->addNoinfoSum();
	     addFields(0);
	     return CHANNEL_NOINFO;        // 无主话单通道
	   }
	} else if ( m_PM->getHomeAreaCodeByPhoneN(m_msisdn,areaCode,groupId,
	                                          userType,provinceCode) )
	{                              // 国内号码判断
	  theSortLog->addNrSum();
      addFields(0);
      return CHANNEL_NR;               // 国内漫游通道
   } else if ( m_PM->getHomeAreaCodeBySpecialMsisdn(m_msisdn,areaCode,groupId,userType,
                                                    officeCode) ) { // 特殊号码判断
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
		          
         channelNo=countSum(servInfo.m_channelNo);
         //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
         bindInfo(servInfo,servInfo_other);     // 绑定资料
         
         theSortLog->addSpecialSum();
         return CHANNEL_SPECIAL;       // 特殊通道
      } else {
         theSortLog->addNoinfoSum();
	      addFields(0);
	      return CHANNEL_NOINFO;        // 无主话单通道
	   }
   } else {
       theSortLog->addNoinfoSum();
	   addFields(0);
	   return CHANNEL_NOINFO;           // 无主话单通道
	}

	return CHANNEL_ERROR;
}

//##ModelId=3C99C8240090
string SortRule_2::pocSort()
{
    int ii;
    ServInfo2 servInfo2;
    //add by xiel 2003/08/11 增加对方号码品牌判断
    ServInfo servInfo_other;
   
    char chan[20],tmp[10],trunk[20];

    strcpy(trunk,"1111");
    memcpy(tmp,m_msisdn,3);
    tmp[3]=0;
    ii=atoi(tmp);

    if( ii<134 || ii>139 ) {                         // IP后付费分拣(非手机号码)
         theSortLog->addTrashSum();

         return CHANNEL_POCTRASH;
    } else {            // PSTN专网分拣
        if ( m_PM->getServInfoByTrunkId(m_msisdn,trunk,m_startDate,servInfo2) ) {
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
		          	
           theSortLog->addPocSum();
           //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
           bindInfo(servInfo2,servInfo_other);     // 绑定资料
           
           sprintf(chan,"%d",servInfo2.m_channelNo);
           return  chan;                                // 省内通道号
        }
    }
    addFields(0);
    theSortLog->addPocNoinfoSum();

    return CHANNEL_POCNOINFO;                        // POC无主话单通道

}

// PSTN专网分拣
string SortRule_2::PSort()
{
    int ii;
    ServInfo2 servInfo2;
    //add by xiel 2003/08/11 增加对方号码品牌判断
    ServInfo servInfo_other;
   
    string channelNo;
    char tmp[10],trunk[20],msisdn[25];

    //POC话单按入中继绑serv_id PTC话单按出中继绑serv_id
    if ( m_callType == 1 )
        strcpy(trunk,m_inTrunkId);
    else
        strcpy(trunk,m_outTrunkId);
        
    strcpy(msisdn,"1111");

    // PSTN专网分拣  yzjk..td_thunk_id
    if ( m_PM->getServInfoByTrunkId(msisdn,trunk,m_startDate,servInfo2) ) //得到 servInfo2
    {
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
       bindInfo(servInfo2,servInfo_other);     // 绑定资料
       
       channelNo=countSum(servInfo2.m_channelNo);
       return  channelNo;                                // 省内通道号
    }
    
    addFields(0);
    theSortLog->addNoinfoSum();

    return CHANNEL_NOINFO;                        // POC无主话单通道

}

