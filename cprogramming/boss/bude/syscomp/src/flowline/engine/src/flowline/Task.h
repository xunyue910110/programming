/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Task.h,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: Task.h,v $
* Revision 1.1  2011/04/15 07:39:45  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:03  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:57:15  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:48  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:14:47  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:47  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:13  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _TASK_H_
#define _TASK_H_
#include "Node.h"
#include "Adaptable.h"

namespace flowline {
class Task : public Executor, public Adaptable {
public:
    /** 所有调过来的转移, 必须保证前面的全部完成. */
    virtual void doEnter(Context & ctx, Node * from);    
};
}
#endif //_TASK_H_
