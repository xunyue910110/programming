#ifndef _PMGLOBALMANAGER_H_INCLUDE_20080901_
#define _PMGLOBALMANAGER_H_INCLUDE_20080901_

#include "config-all.h"
#include "PM_Filter.h"
#include "FilterConfig.h"
#include "PMHelperall.h"

class PMGlobalManager:public PM_Filter
{
  public:
    bool initialize(FilterConfig *r_pConfig,int r_indicator=0);
    
    //dump param data to files 
    void dump2File();
};
#endif //_PMGLOBALMANAGER_H_INCLUDE_20080901_
