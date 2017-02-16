/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/ResourceProcessor.h,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: ResourceProcessor.h,v $
* Revision 1.1  2011/04/15 07:39:42  sunam
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
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.5  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.4  2005/03/23 06:09:21  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/17 07:37:41  juyf
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:10  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _RESOURCE_PROCESSOR_H_
#define _RESOURCE_PROCESSOR_H_
#include "Node.h"
#include "Adaptable.h"
#include "Resource.h"


namespace flowline {
namespace adapter {
class ResourceProcessorAdapter;
}

class AbstractResourceProcessor : public Executor {
public:
    virtual void doInit(FlowLineDao& dao);
    virtual void doEnter( Context & ctx, Node * from );
    virtual void process( Context & ctx, PResource source, vector < PResource > & result ) = 0;
    virtual void finish( Context & ctx, vector < PResource > & result ) = 0;
    
protected:	
	//当processAdapter没有result输出时, 所有的信息暂存在此
	vector<pair <PResource, int> > m_sources;
	time_t m_begin;
};

class ResourceProcessor : public AbstractResourceProcessor, public Adaptable {
public:
    virtual void process( Context & ctx, PResource source, vector < PResource > & result );
    virtual void finish( Context & ctx, vector < PResource > & result );
private:
	ResourceProcessorAdapter* getResourceProcessorAdapter(); 
};
}
#endif //_RESOURCE_PROCESSOR_H_
