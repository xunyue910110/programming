#ifndef CDBNAMESERVICEIMPL_H_HEADER_INCLUDED_BE05BFAB
#define CDBNAMESERVICEIMPL_H_HEADER_INCLUDED_BE05BFAB
#include "mwci/CContext.h"
#include "mwci/CNaming.h"
#include "udbi/udbi.h"

#define SYSCTID  9900

using namespace UDBI;


class CNaming;

//##ModelId=41FA1B8500AF
class CDBNameServiceImpl
{
  public:
    //##ModelId=41FA1BAE01C6
    CDBNameServiceImpl(int iContextid);

    CDBNameServiceImpl();

    //##ModelId=41FAF3020137
    static CDBNameServiceImpl * GetInstance(int iContextid = 0);

    //##ModelId=41FAF17D0188
    bool Init(JConnect *pcon = NULL);

    void setContextid(int iContextid);

    //##ModelId=41FAF1A40332
    bool ListNameing();

    //##ModelId=41FA1BAE01C6
    bool LookUpNaming(CNaming& theNaming, const string &strLogicalName);

  private:
    //##ModelId=41FAF228030A
    JConnect *pconn1;

	static CDBNameServiceImpl* cdsi;

    //##ModelId=41FAF24001E2
    std::list<CNaming> m_listNaming;
    std::list<CNaming>::iterator itr,theEnd;

    //##ModelId=41FAF26300D4
    int m_iContextid;
};



#endif /* CDBNAMESERVICEIMPL_H_HEADER_INCLUDED_BE05BFAB */
