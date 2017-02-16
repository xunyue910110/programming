/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataSupportable.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataSupportable.h,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:01  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:13  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _DATA_SUPPORTABLE_H_
#define _DATA_SUPPORTABLE_H_
#include "flowline/Context.h"
#include "flowline/Resource.h"
namespace flowline {

  
class DataProcessContext : public Context {
  vector < PResource > & m_result;
public:
    
    DataProcessContext(Context* parent,vector < PResource > & result) : 
      Context(parent), m_result(result) {       
    }
    
    vector < PResource > & result() {
        return m_result;
    }
};

class DataSupportable {
public:
    void beginProcessResource(Context & ctx, PResource source);
    virtual void onBeginProcessResource(Context & ctx, PResource source);
    bool isResourceFinished() const;
    void setResourceFinished(bool finished);
    DataSupportable();
    virtual ~DataSupportable();
private:
    bool m_resourceFinished;
};

}
#endif //_DATA_SUPPORTABLE_H_
