// ##########################################
// Source file : StandardField.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef STANDARDFIELD_H_HEADER_INCLUDED_B7A67581
#define STANDARDFIELD_H_HEADER_INCLUDED_B7A67581

#include "config-all.h"

#include <string>
#include <vector>

USING_NAMESPACE(std);

class FieldInfos;

const int NO_FILL    = 0;
const int LEFT_FILL  = 1;
const int RIGHT_FILL = 2;

//##ModelId=48523826034B
class StandardField
{
  public:
    //##ModelId=4852390D0128
    //##Documentation
    //## 从配置文件中的<output_file>段中获得输出字段信息
    bool initialize(FieldInfos *pFieldInfos,const string &theString);
    
    StandardField & operator=(const StandardField &right);


  public:
    //##ModelId=4852395F002E
    string m_outputFieldName;			/*输出字段名*/

    //##ModelId=4852396B0177
    int m_fieldLength;					/*字段长度*/

    //##ModelId=485239760222
    vector<string> m_inputFieldNames;		/*取值依据字段名*/
    vector<int>    m_inPutFieldSeq;		/*取值依据 字段在FieldInfos 中的序号*/
 
    //##ModelId=485239850177
    int m_inFieldCount;			/*取值字段的个数*/

    //##ModelId=485239903527
    //##Documentation
    //## 为了输出填充字符,代表左填充,还是右填充
    int m_isLeftFill;				/*左填充右填充标识*/

    //##ModelId=485240394873
    char m_fillChar[1+1];			/*填充字符*/

};



#endif /* STANDARDFIELD_H_HEADER_INCLUDED_B7A67581 */
