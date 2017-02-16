// ##########################################
// Source file : SortRule_6.cpp
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

#include "SortRule_6.h"

//## 天津移动BOSS一次分拣（MSC/SSP话单）即分拣漫游话单和省内话单
//return 省内话单,国内来访,国际来访,SCP(神州行音信互动),ERROR(错单)
string SortRule_6::getChannel(char * & cdr)
{
    string channel;
    
    channel     = "-1";
    cdr         = m_cdrBuffer;
    
    switch (m_callType)  
    {
    case 11:             // POC
       channel = CHANNEL_PROV;
       break;
    default:
       channel = otherSort();
       break;
    }
    
    return channel;
}

string SortRule_6::otherSort()
{
    char imsiSegI[6];

    memcpy(imsiSegI,m_IMSI,5);
    imsiSegI[5]=0;

    if ( m_PM->isImsiI(imsiSegI) )  { //(1)国际漫游来访判断
       theSortLog->addIrSum();
       addFields(0);
       return CHANNEL_IR;            

    } else if (strncmp(m_rateInd,"P",1)==0 ) {
       return "pbx";                  //(2)专网分检 add by liux 2004.09.09
        
    } else if ( strncmp(m_msisdn,"13",2)==0 || strncmp(m_msisdn,"013",3)==0 ) {

       return nationalSort();         //(3) 国内话单分拣
    } else {
       theSortLog->addErrorSum();
       addFields(INVALID_IMSI);       //(4) 添加错误代码为：国际IMSI代码错
       return CHANNEL_ERROR;
    }   
}

//##ModelId=3C99CA09005B 国内话单分拣
string SortRule_6::nationalSort()
{
    string   channelNo;
    ServInfo servInfo;
    //add by xiel 2003/08/11 增加对方号码品牌判断
    ServInfo servInfo_other;
    	
    static T_GROUP_ID       groupId;
    T_AREA_CODE             areaCode;
    static T_USER_TYPE      userType;
    static T_OFFICE_CODE    officeCode;
    static T_PROVINCE_CODE  provinceCode;

   // 智能网话单判断
   /*if( m_PM->getHomeAreaCodeByMsisdnS(m_msisdn) ) {
      theSortLog->addTrashSum();

      return  CHANNEL_TRASH;
   }*/
   
    
    //add by xiel 2004/05/10 神州行话单分拣
    userType="000";
    //1.神州行话单判断 (查表 TD_SSP,TD_NATIONAL_FEE_CODE)
    if (m_PM->getHomeAreaCodeBySSP(m_msisdn,areaCode,groupId,userType,officeCode))
	  {
        if (    (m_callType!=1) || 
                (strncmp(m_otherParty,"12590",5)!=0))
        {	  	
            return CHANNEL_PROV;
        }
        if (userType[2]=='1') //(1)省内话单
        {
            //单独通道下分拣到
            theSortLog->addSpecialSum();
            addFields(0);
            return CHANNEL_SCP;       //省内神州行音信互动有信息费话单分拣到特殊通道scp下,单独批价,给预付费网关
        }
        else 	               //(2)省际话单(来访)
        {
            theSortLog->addNrSum();
            addFields(0);
            return CHANNEL_NR;       // 来访神州行音信互动有信息费话单合在正常来访通道下,上发Q文件
        }	
   	}

    //2. 省内号码判断(非神州行)
    if( m_PM->getHomeAreaCodeByMsisdnP(m_msisdn,areaCode,groupId,userType,officeCode) )
    {
    	 return CHANNEL_PROV;	
    }
    //3. 国内号码判断(非神州行) 
    else if ( m_PM->getHomeAreaCodeByPhoneN(m_msisdn,areaCode,groupId,
	                                          userType,provinceCode) 
	          )
    {                              
	      theSortLog->addNrSum();
        addFields(0);
        return CHANNEL_NR;               // 国内漫游通道
    } 
    else 
    {
        return CHANNEL_PROV;
	  } 

}//nationalSort


