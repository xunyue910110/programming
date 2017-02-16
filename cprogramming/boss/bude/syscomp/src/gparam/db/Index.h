// ############################################
// Source file : Index.h
// Version     : 0.2.0
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030927
// Update      : 20031118
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef INDEX_H_INCLUDED_C08AB861
#define INDEX_H_INCLUDED_C08AB861 

#include <string>
#include <vector> 

#include "ParameterCdr.h"  
#include "IndexMessage.h" 

class QueryUseExpression;

//##ModelId=3F74FAC802B6 一张表一条记录的一种索引(可能有几个字段)
class Index  
{   
	friend class QueryUseExpression;
	
  public: 
  	//##ModelId=3F7501B30353
    Index();  
    
    //##ModelId=3F8CD21D02A8
    Index(const Index &index);

    //##ModelId=3F7501B70327
    ~Index();  
    
    //##ModelId=3F94C2F60169
    //##Documentation
    //## 查询时使用
    void setKey(ParameterCdr *cdr);         
      
    //##ModelId=3F72A24B01B6
    friend bool operator==(const Index &left, const Index &right);

    //##ModelId=3F72A24B02BB
    friend bool operator<(const Index &left, const Index &right);
              
    //##ModelId=3F7A3E12037C
    const ParameterCdr *getCdr() const ;

    //##ModelId=3F7A3E1B025D
    const char *getKey();
     
    //##ModelId=3F9DC9140003
    void setIndexMessage(IndexMessage *indexMessage);  
    
    //##ModelId=3F9DD04900E2
    void setFieldId(vector<int> *fieldId);      
    
  private:
    //##ModelId=3F729D8D02FB
    //## 参数记录对象指针(涉及到的这条记录的地址,起到索引的功能),关键成员
    ParameterCdr *m_cdr;  
        
    //##ModelId=3F9DDF0202CC
    IndexMessage *m_indexMessage;
    
};

#endif /* INDEX_H_INCLUDED_C08AB861 */
