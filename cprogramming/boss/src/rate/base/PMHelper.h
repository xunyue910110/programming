
#ifndef __PMHELPER_H__
#define __PMHELPER_H__

#include "config-all.h"

#include "dbparam/ParamManager.h"
#include <map>
#include <iostream>


class PMHelper {  
  public:
    static map<string,ParamContainerBase*> * GetParamContainers();

    static ParamContainerBase * GetParamContainer(const string & name);

    static int SetParamContainer(const string &name,ParamContainerBase * container);
    
    //added by zhanglong  2006-7-19 16:50
    static int SetParamContainer2(const string &name,ParamContainerBase * container);
    
    //指示器，取值为0或者1，用来指示从哪个map中获取ParamContainer的指针 2006-7-19 16:50
    static int m_indicator;
};

#define INSTALL_PARAMCONTAINER_INSTANCE(classname) \
int classname##InstallTemp = PMHelper::SetParamContainer(#classname,new ParamContainer<classname>());

//added by zhanglong 2006-7-19 16:50 定义第二个ParamContainer，用来实时更新ParamContainer
#define INSTALL_PARAMCONTAINER2_INSTANCE(classname) \
int classname##InstallTemp2 = PMHelper::SetParamContainer2(#classname,new ParamContainer<classname>());


#endif

