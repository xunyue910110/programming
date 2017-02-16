/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Element.cpp,v 1.1 2011/04/15 07:39:41 sunam Exp $
* $Log: Element.cpp,v $
* Revision 1.1  2011/04/15 07:39:41  sunam
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

#include "Element.h"
namespace flowline {

Element::Element() : m_id(0) {}
Element::~Element() {}
Element::Element(const Element & e) {}

Element & Element::operator = (const Element & e) {
    return *this;
}
long Element::getId() const {
    return m_id;
}
void Element::setId(long id) {
    this->m_id = id;
}
const string & Element::getName() const {
    return m_name;
}
void Element::setName(const string & name) {
    this->m_name = name;
}
const string & Element::getDescription() const {
    return m_description;
}
void Element::setDescription(const string & description) {
    this->m_description = description;
}

static const string EMPTY_STRING;

const string& Element::getExtendedAttribute(const string & name_) const{
	map < string, string >::const_iterator it = extendedAttributes.find(name_);
    if (it == extendedAttributes.end()){ 
    	return EMPTY_STRING;
    }
    return it->second;
}

void Element::setExtendedAttribute(const string & name_, const string& value){
	extendedAttributes.insert(pair<string ,string >(name_,value));
}
};
