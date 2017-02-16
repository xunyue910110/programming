#ifndef MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829
#define MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829

#include "Mdb_Common.h"
#include "FieldsXX.h"
#include "IdInfo.h"

class Mdb_UserParam
{
  public:
    bool getProperty(Mdb_Property &property) const;

    bool convert(Session* session);
    
    void setKeyParams(InputParams *pInputParamsseIndex) const;
    
    void setOtherParams(InputParams *pInputParamsseOther) const;

    Mdb_UserParam(){}

    Mdb_UserParam(const Mdb_UserParam& right);

    void operator=(const Mdb_UserParam& right);
    
    //Added by Malone 2009-11-28 15:46 start
    friend int operator<(const Mdb_UserParam &left,
  		                   const Mdb_UserParam& right);
    //Added by Malone 2009-11-28 15:46 end
    
    friend ostream& operator<<(ostream& os,const Mdb_UserParam& po);
     
    UserParam m_record;

  private:
	static Mdb_Property m_property;
};

#endif //MDB_USERTP_PARAM_H_HEADER_INCLUDED_20080829
