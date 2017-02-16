// ##########################################
// Source file : FileHead.h
// System      : Mobile Billing System
// Version     : 1.11
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010821
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FILEHEAD_H_INCLUDED_C4861464
#define FILEHEAD_H_INCLUDED_C4861464

#include "config-all.h"

#include <string>
#include "FieldExtractRule.h"
#include "CompositeFieldExtractRule.h"

USING_NAMESPACE(std)

//##ModelId=3B4E929900C4
//##Documentation
//## 话单文件块处理类 。此类用于处理文件块，先得到文件块，再根据配置，得到文件块内的字段值。这里的值主要用于文件校验用。
//##
class FileHead {
  public:
	//##ModelId=3B62167802E4
	FileHead();

	//##ModelId=3B6216790173
	~FileHead();

	//##ModelId=3B4EABF202C3
	//##Documentation
	//## 该方法用于得到文件块里的字段值，字段依赖于参数的定义。
	//##
	void getFileHeadField();

	//##ModelId=3B7101C502EB
	bool initialize(const vector<string> aParameterList)  ;

	//##ModelId=3B7D142401FF
	CompositeFieldExtractRule m_theCompositeField;

  private:
	//##ModelId=3B6575CB02BB
	string m_FileHead_;

	//##ModelId=3B6575CC0009
	string m_FileTail_;


};



#endif /* FILEHEAD_H_INCLUDED_C4861464 */
