/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Delegation.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Delegation.cpp,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:02  hanyu
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
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/

#include "Delegation.h"
#include "flowline/adapter/Adapter.h"
#include "flowline/adapter/AdapterFactory.h"
flowline::Delegation::Delegation() {
}
flowline::Delegation::Delegation(flowline::adapter::AdapterFactory * factory) {
    setAdapterFactory(factory);
}
flowline::Delegation::~Delegation() {
}
const string & flowline::Delegation::getId() const {
    return m_id;
}
void flowline::Delegation::setId(const string & id) {
    this->m_id = id;
}
const string & flowline::Delegation::getClassName() const {
    return m_className;
}
void flowline::Delegation::setClassName(const string & className) {
    this->m_className = className;
}
const string & flowline::Delegation::getConfiguration() const {
    return m_configuration;
}
void flowline::Delegation::setConfiguration(const string & configuration) {
    m_configuration = configuration;
}
/**
 * 获得回调句柄
 * @return 回调句柄
 */
flowline::adapter::Adapter & flowline::Delegation::getAdapter() {
    if (m_adapter.get()) return * m_adapter;
    m_adapter = auto_ptr < Adapter > (m_factory->create(m_className));
    m_adapter->configure(m_configuration);
    return * m_adapter;
}
void flowline::Delegation::setAdapterFactory(flowline::adapter::AdapterFactory * factory) {
    m_factory = factory;
}
