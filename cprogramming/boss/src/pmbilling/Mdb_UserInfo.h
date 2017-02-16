#ifndef MDB_USERINFO_H_INCLUDED_20081014_
#define MDB_USERINFO_H_INCLUDED_20081014_

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "IdInfo.h"

class Mdb_UserInfo
{
  public:
  	Mdb_UserInfo(IdInfo *pIdInfo){m_record = pIdInfo;}
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserInfo(){}

    Mdb_UserInfo(const Mdb_UserInfo& right);

    void operator=(const Mdb_UserInfo& right);
    
    friend ostream& operator<<(ostream& os,const Mdb_UserInfo& po);
    IdInfo *m_record;

  private:
	  static Mdb_Property m_property;
};

#endif //MDB_USERINFO_H_INCLUDED_20081014_
