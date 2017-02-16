#ifndef _USERINFO_H_INCLUDE_20081014_
#define _USERINFO_H_INCLUDE_20081014_
#include "FieldsXX.h"
#include <vector>
using namespace std;

//用户属性USER_ITEM
class UserParam
{
  public:
    T_SERVER_ID       m_userId;         // 用户标识
    T_TP_ID           m_paramId;        // 参数标识
    T_TPPARAM_VALUE   m_value;          // 参数值
  	T_DATE_TIME       m_beginTime;      // 起始时间
  	T_DATE_TIME       m_endTime;        // 终止时间
  public:
    friend ostream& operator<<(ostream& r_os,const UserParam &r_right);
    
    friend int operator<(const UserParam &r_left,
    		                 const UserParam &r_right);
   
    bool isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime,const T_BIZ_TYPE &r_eventTypeId) const;
    
};


//定义三户信息
class IdInfo
{
  public:
  	enum ID_ATTR
  	{
  		ID_ATTR_ID=0,          //三户标识       
  		ID_TYPE_CODE,        //见ID_TYPE枚举类型定义                                 
      ID_OPEN_DATE,        // m_startDate;                                                                  
      ID_REMOVE_DATE,      // m_endDate;                                                                    
      ID_CUST_ID,          //客户标识                                                                       
      ID_USER_TYPE_CODE,   //用户类型：用于将用户分群,如普通、公免、测试卡，详见用户类型参数表，0必须为普通  
      ID_BRAND_CODE,       //品牌  
      ID_PRODUCT_ID,       //产品                                                                        
      ID_EPARCHY_CODE,     //地州编码                                                                       
      ID_CITY_CODE,        //市县编码                                                                       
      ID_ACCOUNT_TAG,      //出帐标志                                                                       
      ID_REMOVE_TAG,       //注销标志
      ID_FIRSTCALL_TIME,    //首次通话时间
      ID_PREDESTROY_TIME,  //预销号时间                                                                     
      ID_LASTSTOP_TIME,    //最后停机时间                                                                   
      ID_USER_STATE,       //用户状态                                                                       
      ID_SCORE_VALUE,      //当前总积分                                                                     
      ID_CREDIT_VALUE,     //信用度                                                                         
      ID_PAYMODE_CODE,     //帐户类型                                                                       
      ID_NETTYPE_CODE,     //网别代码
      ID_MSISDN,           //电话号码                                                                          
      ID_ADVANCE_PAY,      //预存款,内部使用
      ID_UPDATE_TIME,      //资料更新时间                                                                
      ID_PREPAY_TAG,			 //预付费标志
      ID_IN_DATE,          //建档时间 add by xuf 2010-1-3 21:35:04                                                                      
  		ID_ATTR_MAX
  	};
  	
  	T_OBJECT_ATTR & getIdAttr(ID_ATTR key)	
  	{  		
  		return m_idAttr[key];
  	}

  	T_OBJECT_ATTR & getIdAttr(int &key)	
  	{
  		if(key >= ID_ATTR_MAX)
  		{		
  			//error deal
  		}
  		return m_idAttr[key];
  	}

  	bool getIdAttr(int &key,T_OBJECT_ATTR &attributeValue)	
  	{
  		if(key < ID_ATTR_MAX)
  		{
  			attributeValue = m_idAttr[key];
  			return true;
  		}
  		else
  		{
  			//error deal
  			return false;
  		}
  	}
  	
  friend ostream& operator<<(ostream& os,const IdInfo& r_value)
  {
    os << r_value.m_idAttr[ID_ATTR_ID]      << " , "
    	<< r_value.m_idAttr[ID_TYPE_CODE]     << " , "      
      << r_value.m_idAttr[ID_OPEN_DATE]     << " , "      
      << r_value.m_idAttr[ID_REMOVE_DATE]    << " , "  
      << r_value.m_idAttr[ID_CUST_ID]        << " , "  
      << r_value.m_idAttr[ID_USER_TYPE_CODE] << " , "  
      << r_value.m_idAttr[ID_BRAND_CODE]   << " , "
      << r_value.m_idAttr[ID_PRODUCT_ID]   << " , "     
      << r_value.m_idAttr[ID_EPARCHY_CODE] << " , "    
      << r_value.m_idAttr[ID_CITY_CODE]    << " , "    
      << r_value.m_idAttr[ID_ACCOUNT_TAG]  << " , "    
      << r_value.m_idAttr[ID_REMOVE_TAG]  << " , "     
      << r_value.m_idAttr[ID_FIRSTCALL_TIME]  << " , "     
      << r_value.m_idAttr[ID_PREDESTROY_TIME]<< " , "  
      << r_value.m_idAttr[ID_LASTSTOP_TIME]<< " , "    
      << r_value.m_idAttr[ID_USER_STATE]  << " , "     
      << r_value.m_idAttr[ID_SCORE_VALUE] << " , "     
      << r_value.m_idAttr[ID_CREDIT_VALUE] << " , "    
      << r_value.m_idAttr[ID_PAYMODE_CODE] << " , "    
      << r_value.m_idAttr[ID_NETTYPE_CODE] << " , " 
      << r_value.m_idAttr[ID_MSISDN]  << " , "         
      << r_value.m_idAttr[ID_ADVANCE_PAY]  << " , "
      << r_value.m_idAttr[ID_UPDATE_TIME]  << " , "         
      << r_value.m_idAttr[ID_PREPAY_TAG] << " , "  
      << r_value.m_idAttr[ID_IN_DATE]     << endl;    //add by xuf 2010-1-3 21:38:09 
   
  	return os;
  }
  
   //根据参数ID取对应的值，找不到返回NULL
   const char* getParamValue(const T_TP_ID &r_paramId) const;
  //根据参数ID取本月有效的参数结构，找到返回true ，找不到返回false
   bool getParamValue(const T_TP_ID &r_paramId,UserParam &r_userParam) const;

  public:	
	  T_OBJECT_ATTR m_idAttr[ID_ATTR_MAX];
	 
	public:
		IdInfo()
  	{
  		for(int i =0;i<ID_ATTR_MAX;i++)
  			 this->m_idAttr[i].clear();
       this->m_userParamList.clear(); 
  	}
		
		IdInfo(const IdInfo &r_value)
  	{
  		if (this!=&r_value)
  		{
  			for(int i =0;i<ID_ATTR_MAX;i++)
  			 this->m_idAttr[i]=r_value.m_idAttr[i];
  			 
  			 this->m_userParamList = r_value.m_userParamList;
  		} 
  	}
  	
  	const IdInfo& operator=(const IdInfo& r_value)
  	{
  		if (this!=&r_value)
  		{
  			for(int i =0;i<ID_ATTR_MAX;i++)
  			 this->m_idAttr[i]=r_value.m_idAttr[i];
  			 
  			 this->m_userParamList = r_value.m_userParamList;
  		}
  		return *this;	
  	}
	  
	  vector<UserParam> m_userParamList;  // 用户属性
	  
  public:
  	void init(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime,const T_BIZ_TYPE &r_eventTypeId="0");
   private:
     T_DATE_TIME m_cycBeginTime;
     T_DATE_TIME m_cycEndTime;
   	 T_BIZ_TYPE  m_eventTypeId;	
};




#endif //_USERINFO_H_INCLUDE_20081014_


