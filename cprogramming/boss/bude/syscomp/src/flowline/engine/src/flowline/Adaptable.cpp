/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Adaptable.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Adaptable.cpp,v $
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
* Revision 1.4  2005/04/27 14:59:29  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:16  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:10  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:24  luzl
* init
* 
*/

#include "Adaptable.h"
namespace flowline {

void Adaptable::setAdapter(ElementAdapter*  adapter) {
    m_adapter = adapter;
    m_adapter->setTarget(this);
}
ElementAdapter * Adaptable::getAdapter() {
    return m_adapter;
}

Adaptable::Adaptable() : m_adapter(0) {}
Adaptable::~Adaptable() {
    delete m_adapter;
}

}
