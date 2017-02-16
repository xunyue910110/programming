// ##########################################
// Source file : SortRule_3.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011030
// Update      : 20020424
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "SortRule_3.h"

//##ModelId=3BE21AE100FB
//##Documentation
//##  漫游出访分拣处理的分拣处理处理
string SortRule_3::getChannel(char * & cdr)
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

   if(strncmp(m_rateInd,"n",1)==0) {
      ;
   } else if(strncmp(m_rateInd,"i",1)==0) {
      ;
   } else if(strncmp(m_rateInd,"k",1)==0) {
      ;
   } else {
      theSortLog->addErrorSum();
	  addFields(INVALID_ROAM_TYPE);               // 添加错误代码为：无效漫游类型
      return CHANNEL_ERROR;
   }
		//add by xiel 如果有msisdn,则根据msisdn 找. changed by liux 国际手机号码字段有10位，按IMSI查 20041021
		if (strncmp(m_msisdn,"13",2)==0 &&
 		    strlen(m_msisdn) ==11 ){

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
		      		return CHANNEL_NOINFO;       // 出访神州行业务话单判断是外省号码做为无主
		      	}	
		   	}
		   	
		   	
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
				 		          
		         channel=countSum(servInfo.m_channelNo);
		         //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
		         bindInfo(servInfo,servInfo_other);     // 绑定资料
		         
	         	return channel;         // 特殊通道号
	       	} else {

	       		//add by xiel 2002/06/19
				theSortLog->addNoinfoSum();
				addFields(0);
				return CHANNEL_NOINFO;        // 无主话单通道
			
		   	}
		}
		//add by xiel 如果没有msisdn,则根据imsi 找
		else{
			
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
				 		          
		         channel=countSum(servInfo.m_channelNo);
		         //add by xiel 2003/08/11 增加对方号码品牌判断 绑定对方号码的信息
		         bindInfo(servInfo,servInfo_other);     // 绑定资料
		         
	         	return channel;         // 特殊通道号
	       	} else {
				//add by xiel 2002/06/19
	         	theSortLog->addNoinfoSum();
				addFields(0);
				{
					//出访无主话单没有msisdn,将imsi拼出一个值做为msisdn，好去排重
					char imsi2msisdn[30+1];
					if (strlen(m_IMSI)>5)
					{
						sprintf(imsi2msisdn,"X%s\0",m_IMSI+5);
						imsi2msisdn[11]=0;
					}
					else
						sprintf(imsi2msisdn,"X%s\0",m_IMSI);

					int len=m_outputField.size();
				    for(int i=0;i<len;i++)
				    {
				       if ( m_outputField[i].m_fieldName.compare("MSISDN") == 0 )
				       {
				          setFieldValue( m_outputField[i].m_position , imsi2msisdn , true);
				       }
				    }
					return CHANNEL_NOINFO;        // 无主话单通道
				}

		   	}

		}

    theSortLog->addErrorSum();
	addFields(INVALID_ROAM_TYPE);               // 添加错误代码为：无效漫游类型

    return CHANNEL_ERROR;
}

