// ##########################################
// Source file : StandardField.h
// Version     : 0.1.5
// System      : Mobile Billing System
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20011025
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef STANDARDFIELD_H_INCLUDED_C44F0F10
#define STANDARDFIELD_H_INCLUDED_C44F0F10

#include "config-all.h"

#include <string>
#include <vector>

USING_NAMESPACE(std)

const int NO_FILL    = 0;
const int LEFT_FILL  = 1;
const int RIGHT_FILL = 2;

//##ModelId=3BB080FD014F
class StandardField
{
  public:
	//##ModelId=3BB415E7008D
	bool initialize(string theString);

  public:
	//##ModelId=3BB08110035F
	string m_outputField;

	//##ModelId=3BB08123030C
	int m_fieldLength;

	//##ModelId=3BB08130036F
	vector<string> m_inputField;

	//##ModelId=3BD77A3A0028
	int m_inFieldCount;
	
	// add by chenm 2005-7-5 为了输出填充字符,代表左填充,还是右填充
	bool m_isLeftFill;
	char m_fillChar[1+1];

  private:
	//##ModelId=3BB4195402D3
	string lrTrim(string TheString)  ;

};



#endif /* STANDARDFIELD_H_INCLUDED_C44F0F10 */
