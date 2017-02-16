#ifndef _MDB_USERSERVSTATE_H_200801028_
#define _MDB_USERSERVSTATE_H_200801028_

#include "Mdb_Common.h"
#include "FieldsXX.h"

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

    friend int operator<(const Mdb_UserServState &left,const Mdb_UserServState& right);

    friend ostream& operator<<(ostream& os,const Mdb_UserServState& po);

    struct
    {
	  	long           m_userId;       //用户标识
	  	T_FLAG         m_main_tag;     //主服务标志 取1待机状态
	  	T_FLAG         m_state;        //服务状态
	  	T_FLAG         m_stategroup;   //服务状态分组 0:开通 1:停机 2:销户
	  	T_DATE_TIME    m_beginTime;    //生效时间
	  	T_DATE_TIME    m_endTime;      //结束时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_USERSERVSTATE_H_200801028_