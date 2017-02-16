#ifndef MDB_UserOtherInfo_H_INCLUDED_20081014_
#define MDB_UserOtherInfo_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"

//定义用户间费用互转信息. 
class UserOtherInfo
{
  public:
    UserOtherInfo(){}
    virtual ~UserOtherInfo(){}

    friend int operator < (const UserOtherInfo &r_left,const UserOtherInfo &r_right)
    {
			return (r_left.m_otherInfoType < r_right.m_otherInfoType); 
    } 
   
    T_SERVER_ID  m_userId;        //用户标识	          
    String<4>		 m_otherInfoType; //其他信息类型	
    String<100>	 m_attrValue1;    //属性值1  
    String<100>  m_attrValue2;    //属性值2	  
    String<100>  m_attrValue3;    //属性值3  
    String<100>  m_attrValue4;    //属性值4  
    String<100>  m_attrValue5;    //属性值5	  
    String<100>  m_attrValue6;    //属性值6	  
    String<100>  m_attrValue7;    //属性值7	  
    String<100>  m_attrValue8;    //属性值8
    T_DATE_TIME  m_startDate;			//生效时间
    T_DATE_TIME  m_endDate;				//终止时间
    
};                                                       		          
                                                         	            

class Mdb_UserOtherInfo
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserOtherInfo(){}

    Mdb_UserOtherInfo(const Mdb_UserOtherInfo& right);

    void operator=(const Mdb_UserOtherInfo& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserOtherInfo& po);
    UserOtherInfo m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_UserOtherInfo_H_INCLUDED_20081014_
