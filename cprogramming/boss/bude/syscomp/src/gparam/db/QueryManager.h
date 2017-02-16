// ############################################
// Source file : QueryManager.cpp
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031113
// Update      : 20031113
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef QUERYMANAGER_H_INCLUDED_C04C9CBF
#define QUERYMANAGER_H_INCLUDED_C04C9CBF 

#include "Config.h"

#include <vector>  

class ParameterCdr; 
class PConfigureFile;
class QueryUseExpression; 
class ParameterList;
class ParameterLog;

USING_NAMESPACE(std);

//##ModelId=3FB095E90268
class QueryManager
{
  public:
    //##ModelId=3FB095FA01A5
    QueryManager();

    //##ModelId=3FB095FC0111
    ~QueryManager();

    //##ModelId=3FB09C950106
    int find(const char *param, char *result); 
    
    //##ModelId=3FB3380F01AE
    bool initialize(); 
    
    //##ModelId=3FB33EBA00E0
    void setParam(PConfigureFile *sysParam, const string &name, ParameterList *list);
    
    void getQuery(const char *param, string &result);
    
    void setBufferSize(const int &size);

  private:
    //##ModelId=3FB9C4EC006B
    bool initializeExpression(string &path, int order, QueryUseExpression *queryUseExpression);  
    
    //##ModelId=3FCAA443004B
    int getQueryId(const string &queryName);

    //##ModelId=3F98BB4B02E9
    PConfigureFile *m_sysParam;
    
    //##ModelId=3FB323870138 配置文件中的全部 query 配置项
    vector<QueryUseExpression *> m_queryUseExpression;  
    
    //##ModelId=3FB33F510372
    string m_systemName;
    
    //##ModelId=3FB342690096
    ParameterList *m_parameterList; 
    
    //##ModelId=3FB45AE90183
    ParameterLog *m_parameterLog;  
    
    //##ModelId=3FCA9DCA02E7
    vector<string> m_queryList; //配置文件中的全部 query 名(querylist的全部)
    
};



#endif /* QUERYMANAGER_H_INCLUDED_C04C9CBF */
