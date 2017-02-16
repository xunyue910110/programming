/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Adaptable.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Adaptable.h,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:00  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:48  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _ADAPTABLE_H_
#define _ADAPTABLE_H_
#include "config-all.h"
#include "flowline/adapter/Adapter.h"

namespace flowline {
	
using namespace flowline::adapter;

class Adaptable :  virtual public Element {
public:
    Adaptable();
    virtual ~Adaptable();
    /**
     * 需要接管adapter生命管理
     */
    void setAdapter(ElementAdapter*  adapter);
    ElementAdapter * getAdapter();
protected:
    /**
     * 判断委托
     * @link aggregationByValue
     * @supplierCardinality 1
     * @directed
     */
    ElementAdapter* m_adapter;
};

}
#endif //_ADAPTABLE_H_
