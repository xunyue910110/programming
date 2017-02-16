/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Reader.h,v 1.1 2011/04/15 07:39:42 sunam Exp $
* $Log: Reader.h,v $
* Revision 1.1  2011/04/15 07:39:42  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:02  hanyu
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
* Revision 1.3  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#ifndef _READER_H_
#define _READER_H_
#include "ResourceProcessor.h"
#include "DataSupportable.h"
#include "Resource.h"

namespace flowline {
class Writer;
class Reader : public AbstractResourceProcessor, public DataSupportable {
public:
    virtual void doInit(FlowLineDao& dao);    
    virtual void onBeginProcessResource( Context & ctx, PResource resource ) ;
    virtual void process( Context & ctx, PResource source, vector < PResource > & result );
    virtual void finish( Context & ctx, vector < PResource > & result );
private:
    vector < Node * > m_internalNodes;
    //vector < Writer * > m_writers;
    void initNodes(Node* node);
};

}

#endif //_READER_H_
