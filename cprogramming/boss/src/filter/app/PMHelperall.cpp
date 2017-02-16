#include "PMHelperall.h"

//初始化为0 added by zhanglong 2006-7-19 16:52 
int PMHelperall::m_indicator = 0;

map<string,ParamContainerBase*> * PMHelperall::GetParamContainers() 
{
    static map<string,ParamContainerBase*> containers;
    //added by zhanglong 定义第二个map，保存映射关系
    static map<string,ParamContainerBase*> containers2;
    //modified
    if(m_indicator == 1)
        return &containers2;
    else return &containers;
}

ParamContainerBase * PMHelperall::GetParamContainer(const string & name)
{
    map<string,ParamContainerBase*> &conts = *(GetParamContainers());
    
    map<string,ParamContainerBase*>::iterator itr;

    itr=conts.find(name);
    if (itr != conts.end()) {
        return itr->second;  
    }
    return 0;          
}

int PMHelperall::SetParamContainer(const string &name,ParamContainerBase * container) 
{
    //added by zhanglong  2006-7-19 16:54
    m_indicator = 0;
        
    map<string,ParamContainerBase*> &conts = *(GetParamContainers());

    ParamContainerBase * c =GetParamContainer(name);
    if (c==0)  
    {
        conts[name] = container;
    }
    
    return 0;
}

//added by zhanglong 2006-7-19 16:54
int PMHelperall::SetParamContainer2(const string &name,ParamContainerBase * container) 
{
        m_indicator = 1;
    map<string,ParamContainerBase*> &conts = *(GetParamContainers());

    ParamContainerBase * c =GetParamContainer(name);
    if (c==0)
    {
        conts[name] = container;
    }
    m_indicator = 0;//指示器置回0
    return 0;
}
//end added by zhanglong 2006-7-19 16:54


