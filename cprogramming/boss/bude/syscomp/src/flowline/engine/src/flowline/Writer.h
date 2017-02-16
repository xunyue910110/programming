/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Writer.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: Writer.h,v $
* Revision 1.1  2011/04/15 07:39:45  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:04  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:57:15  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.4  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:14:47  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:24  luzl
* init
* 
*/
#ifndef _WRITER_H_
#define _WRITER_H_
#include "flowline/Node.h"
#include "flowline/Adaptable.h"
#include "flowline/DataSupportable.h"

namespace flowline {
class Writer : public Executor, public DataSupportable {
public:
    virtual void doEnter( Context & ctx, Node * from ) ;
    
    //void popResult( vector < PResource > & result );
private:
   //vector < PResource > m_result;
   PResource m_result;
};
}
#endif //_WRITER_H_
