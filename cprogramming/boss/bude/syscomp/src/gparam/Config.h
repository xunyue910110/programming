
#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifdef _BUDE15_
#include "config-all.h"

#include "base/SysParam.h"
#include "base/supdebug.h"
#include "dbi/oradbi.h"

#else 
#include "ConfigAll.h"

#include "SysParam.h"
#include "supdebug.h"
#include "oradbi.h"

#endif		


#if defined(STDIOSTREAM_DISABLE)
#include <fstream.h>
#include <iomanip.h>
#include <iostream.h>
#else
#include <fstream>
#include <iomanip>
#include <iostream>
#endif

USING_NAMESPACE(std); 


#endif /* __CONFIG_H__ */ 

