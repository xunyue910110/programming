#ifndef BackUpdateTask_H
#define BackUpdateTask_H

#include <iostream>
#include <string.h>
#include <stdio.h>

#include "config-all.h"
#include "mwci/CFmlBuf.h"

using namespace std;

extern "C" void E_BackUpdateTask(CFmlBuf &InBuf, CFmlBuf &OutBuf);

#endif
