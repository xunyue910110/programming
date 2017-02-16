/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Element.h,v 1.1 2011/04/15 07:39:41 sunam Exp $
* $Log: Element.h,v $
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
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/

#ifndef _ELEMENT_H_
#define _ELEMENT_H_
#include "base/config-all.h"
#include <string>
#include <map>
//USING_NAMESPACE(std);
using namespace std;
namespace flowline {

/** 流程模型的基本元素 */
class Element {
public:
    Element();
    virtual ~Element();
    long getId() const;
    void setId(long id);

    const string & getName() const;
    void setName(const string & name);

    const string & getDescription() const;
    void setDescription(const string & description);
    
    const string& getExtendedAttribute(const string & name) const;
        
    void setExtendedAttribute(const string & name, const string& value);

private:
    Element(const Element & e);

    Element & operator = (const Element & e);

private:

    /** 元素的ID号, 每个版本的流程, 都有自己的ID */
    long m_id;

    /** 元素的名称 */
    string m_name;

    /** 描述信息 */
    string m_description;
    
    /** 扩展属性*/
    map<string, string> extendedAttributes;  
};

}
#endif //_ELEMENT_H_
