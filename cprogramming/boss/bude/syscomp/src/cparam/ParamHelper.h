#ifndef __PARAM_HELPER_H_
#define __PARAM_HELPER_H_

#include "IpcParamdef.h"

///
#define NULL_VALUE2 "NULL"		
#define NULL_VALUE3 "NULL"


/**
 *
 */
class _ParamHelper {
public:
	//
	static void quicksort(TParRecord* _hrec, int left, int right);
	static int binsrch(TParRecord* _hrec, int size, TParRecord& parRecord);
	static int trvsrch(TParRecord* _hrec, int size, TParRecord& parRecord);

};

#endif
