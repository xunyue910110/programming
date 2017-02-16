/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Task.cpp,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: Task.cpp,v $
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
* Revision 1.3  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/23 06:14:47  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/

#include "Task.h"
#include "flowline/adapter/TaskAdapter.h"
#include "flowline/Context.h"
namespace flowline {
void Task::doEnter(Context & ctx, Node * from) {
    //判断是否全部到达
    if (m_enteredNodes.size() == m_previousNodes.size()) {
        TaskAdapter* adapter = dynamic_cast < TaskAdapter* > (getAdapter());
        
        time_t begin = time(0);      
        adapter->execute(ctx);
		
		ctx.getFlowLineDao().logExecution(ctx.getCycleLogId(), 
			ctx.getBatchLogId(), *this, begin, time(0));
		
        setAllFinished(true);
        vector<Node * >::iterator it = m_subsequentNodes.begin();
        for (; it != m_subsequentNodes.end(); it++)
            (*it)->enter(ctx, this);
    }
}
};
