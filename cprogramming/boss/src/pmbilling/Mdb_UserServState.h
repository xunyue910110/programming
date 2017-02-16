#ifndef MDB_USERSERVSTATE_H_INCLUDED_20081014_
#define MDB_USERSERVSTATE_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "BillingCommon.h"

/*class UserServState
{
  public:
    T_SERVER_ID  m_userId;         //用户标识
    int          m_serviceId;      //服务标识
    char         m_mainTag;        //主服务标志	
    char         m_servStateCode;  //服务状态编码      	      
    T_DATE_TIME  m_startDate;      //生效时间             
    T_DATE_TIME  m_endDate;        //失效时间      
};*/

class Mdb_UserServState
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserServState(){}

    Mdb_UserServState(const Mdb_UserServState& right);

    void operator=(const Mdb_UserServState& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserServState& po);
    UserServState m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_USERSERVSTATE_H_INCLUDED_20081014_
