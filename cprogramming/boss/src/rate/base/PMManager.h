#ifndef _PMManager_H_INCLUDE_20080626_
#define _PMManager_H_INCLUDE_20080626_

#include "config-all.h"
#include "FieldsXX.h"
#include "PM_Rate.h"
#include "RateConfig.h"
#include "PMHelper.h"



class PMManager:public PM_Rate
{
  public:
    bool initialize(RateConfig *r_pConfig,int r_indicator=0);
    
    //dump param data to files 
    void dump2File() ;
};
#endif //_PMManager_H_INCLUDE_20080626_
