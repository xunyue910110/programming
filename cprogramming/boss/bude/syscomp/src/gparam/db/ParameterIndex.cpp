// ############################################
// Source file : ParameterIndex.cpp
// Version     : 0.1.2
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030926
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "util/Util.h"

#include "Index.h"
#include "ParameterIndex.h"    

//##ModelId=3F7505C800C1 Parameter::initialize调用它
bool ParameterIndex::initialize(const string name, const int id, const string order)
{   
    m_keyName.push_back(name);
    m_fieldId.push_back(id);    
    
    if(order.compare("0")==0) 
    {
        m_order.push_back(true);    // 升序 来源于一张表的index配置
    } 
    else 
    {
        m_order.push_back(false);   // 降序
    }  
    
    return true;
} 

//##ModelId=3F72A46803D1
ParameterIndex::ParameterIndex()
{   
    m_order.clear();
    m_keyName.clear();
    m_fieldId.clear();
    
    m_indexMessage.m_order     = &m_order;
    m_indexMessage.m_fieldId   = &m_fieldId;
    m_index.setIndexMessage(&m_indexMessage); 
    
    m_parameterLog    = ParameterLog::getInstance();   
    
    m_indexList        =  new ParameterPool<Index>  ;  
    m_isCompress	 = 0; 
} 

//##ModelId=3F72A46F0164
ParameterIndex::~ParameterIndex()
{  
    delete m_indexList; 
}

//##ModelId=3F7997F9015D
int ParameterIndex::getFieldID()
{
    return m_fieldId[0];
}

//##ModelId=3F79A1E70271
void ParameterIndex::resize(long size)
{  
    m_indexList->resize(size);
}

//##ModelId=3F72AF760069
void ParameterIndex::setKey(ParameterCdr *cdr, vector <bool> *isCompress)
{   
    m_index.setKey(cdr);   
    m_indexList->add(m_index);
    m_indexMessage.m_isCompress=isCompress;
}

//##ModelId=3F7A38B60076
void ParameterIndex::sort()
{                     
    m_indexList->sort();   
}       

int ParameterIndex::dump(string &result)
{ 
    int flag;
    if( m_no >= m_indexList->size() ) {
    	return 0;
    }                        
	
	flag = dump(result,m_no);
	
	m_no++;
	return flag;
}

int ParameterIndex::dump(string &result,const int &no)
{
	int n;
	const char *p;
	char tmp[80]; 
    const ParameterCdr *parameterCdr=0;
                            
    parameterCdr = (m_indexList->at(no))->getCdr(); //得到这个记录 
    if(parameterCdr==0)
    	return -1;
    	
    result="";
    n = parameterCdr->getCount();
    for(int j=0;j<n;j++) 	// 输出各个字段的值
    { 
    	p = parameterCdr->at(j);
    	if( (*m_isCompress)[j] ) {
    		EBCD_decode(tmp,p) ;
    	} else {
    		strcpy(tmp,p); 
    	}
    	result += string( tmp )+",";           
    } 
   	result += string("\n");
	
	return 0;
}

int ParameterIndex::getCount()
{
	m_no = 0;
	return m_indexList->size();
}                

//##ModelId=3FB85CF800E7
//把 客户端传进来的 需查询的内容 传进去 
void ParameterIndex::setQueryUseExpression(QueryUseExpression *queryUseExpression)
{   
    // 供Index对象访问queryUseExpression中的eq、lt方法。
    m_indexMessage.m_queryUseExpression = queryUseExpression; 
    
}

//##ModelId=3FD3EB45039F
bool ParameterIndex::clear()
{  
	if( m_indexList > 0 ) {
    	delete m_indexList;
    	m_indexList        =  new ParameterPool<Index>  ;   
    }
    
    return true;
}

//##ModelId=3F8CD58600A5
int ParameterIndex::seach(vector <Index> &result,QueryUseExpression *queryUseExpression)
{         
    Index index;   
    int seachType;
    IndexMessage indexMessage;
       
    indexMessage.m_order                = &m_order ;
    indexMessage.m_fieldId              = &m_fieldId;
    indexMessage.m_isCompress           = m_isCompress;
    indexMessage.m_queryUseExpression   = queryUseExpression;
       
    index.setKey(0); 
    index.setIndexMessage(&indexMessage); //包含 客户端传进来的需查询的内容
    
    seachType = (indexMessage.m_queryUseExpression)->getSeachType();
                
    switch(seachType)
    {
        case 0:     // 顺序查找  
            return  m_indexList->scanSearchMulti(index,result);
        case 1:     // 二分法查找(多值)
            return m_indexList->searchMulti(index,result);   
        case 2:     // 二分法查找(单值)
            return m_indexList->search(index,result);   
    }    
    
    return m_indexList->searchMulti(index,result);                                     
}

void ParameterIndex::setIsCompress(const vector <bool> *isCompress)
{
	m_isCompress = isCompress;
}



