//-------------------------------------------------------------------
// Name			: dbicommon.h
// Version		: 0.2.8
// Create		: 2001.4.10
// Update		: 2002.3.9
// Auther		: qianyx@lianchuang.com
// Copyright	: All Rights Reserved.
// Description	: Should be included in kernel and application 
//					programs.
//-------------------------------------------------------------------
#ifndef DBICOMMON_H
#define DBICOMMON_H

#include "base/config-all.h"
#include <string>
#include <vector>

#define MAXBIND				200
//#define FIELDLENGTH			1500
//#define DEFAULTARRAYLINE	1000
extern int FIELDLENGTH;
extern int DEFAULTARRAYLINE;

USING_NAMESPACE(std);
typedef vector<string> StringVector;
typedef StringVector::iterator StringVectorIterator;

enum PROCPARAM_ACTION{PARAM_ACT_IN,PARAM_ACT_OUT};
enum PROCPARAM_TYPE{PARAM_TYPE_STR,PARAM_TYPE_INT,PARAM_TYPE_FLOAT};

typedef struct
{
	PROCPARAM_ACTION action;
	PROCPARAM_TYPE type;
	union
	{
		char strValue[80];
		int intValue;
		float floatValue;
	};
}PROCPARAM;

#endif //DBICOMMON_H

//------------------------------ THE END ----------------------------

