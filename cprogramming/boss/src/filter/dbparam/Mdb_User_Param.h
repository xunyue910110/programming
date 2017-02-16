#ifndef _MDB_USER_PARAM_H_20090727_
#define _MDB_USER_PARAM_H_20090727_

#include "Mdb_Common.h"
#include "FieldsXX.h"

class Mdb_User_Param
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);

    void setKeyParams(InputParams *pInputParamsseIndex) const;

    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_User_Param(){}

    Mdb_User_Param(const Mdb_User_Param& right);

    void operator=(const Mdb_User_Param& right);

    friend ostream& operator<<(ostream& os,const Mdb_User_Param& po);

    struct
    {
	  	long           m_userId;        //用户标识
	  	int            m_paramId;       //参数ID
	  	T_ATTRI_VAL    m_paramValue;    //市县乡村编码-12位
	  	T_DATE_TIME    m_beginTime;    //生效时间
	  	T_DATE_TIME    m_endTime;      //结束时间
    }m_record;

  private:
	static Mdb_Property m_property;
};

#endif //_MDB_USER_PARAM_H_20090727_