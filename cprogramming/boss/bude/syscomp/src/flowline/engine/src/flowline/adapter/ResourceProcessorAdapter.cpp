/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/adapter/ResourceProcessorAdapter.cpp,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: ResourceProcessorAdapter.cpp,v $
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
* Revision 1.1  2005/05/08 11:57:50  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:53:38  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:13  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/

#include "ResourceProcessorAdapter.h"
#include "flowline/ResourceProcessor.h"
#include "flowline/ResourceRepository.h"

namespace flowline {
namespace adapter {


PResource ResourceProcessorAdapter::createResource( Context& ctx, const string& repositoryName ) {
    ResourceProcessor * p = dynamic_cast<ResourceProcessor*>( getTarget() );
    Node* n = p->getSubsequentNode( repositoryName );
    return dynamic_cast<ResourceRepository*>( n ) ->create( ctx );
}

}
}
