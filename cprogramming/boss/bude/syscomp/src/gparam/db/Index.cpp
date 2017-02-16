// ############################################
// Source file : Index.h
// Version     : 0.2.0
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030927
// Update      : 20031118
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "util/Util.h"

#include "Index.h"  
#include "QueryUseExpression.h"

//##ModelId=3F7501B30353
Index::Index()
{  
    m_cdr              = 0;  
    m_indexMessage     = 0;   
} 

//##ModelId=3F8CD21D02A8
Index::Index(const Index &index)
{          
    m_cdr              = index.m_cdr; 
    m_indexMessage     = index.m_indexMessage;
}

//##ModelId=3F7501B70327
Index::~Index()
{   
}

//##ModelId=3F72A24B01B6
bool operator==(const Index &left,const Index &right)
{  
    //本方法仅供查询时使用    
    if( right.m_indexMessage->m_queryUseExpression == 0)
    	return false; 
    
    return (right.m_indexMessage->m_queryUseExpression)->eq(left,right); 
}
 
//##ModelId=3F72A24B02BB
bool operator<(const Index &left,const Index &right)
{       
    int size,position,p ;
    const char *left_key,*right_key;
    char l_tmp[80],r_tmp[80];
                 
    if(right.m_cdr==0){   // 查询时使用(排序时不会用到它) 
        return (right.m_indexMessage->m_queryUseExpression)->lt(left,right);   
    } 
    
    // 排序时使用
    size = right.m_indexMessage->m_order->size(); 
    for(int i=0;i<size;i++)  
    {    
    	p 	 		= (*left.m_indexMessage->m_fieldId)[i];
        position 	= (right.m_indexMessage->m_fieldId)->at(i);
        
        //cout<<"i="<<i<<endl;
        //cout<<"position="<<position<<endl;
        
        left_key 	= left.m_cdr->at(position);
        right_key	= right.m_cdr->at(position);

        
        if ( (*left.m_indexMessage->m_isCompress)[p] ) {
        	EBCD_decode(l_tmp,left_key);
        	left_key = l_tmp;
        } 
        if ( (*right.m_indexMessage->m_isCompress)[p] ) {
        	EBCD_decode(r_tmp,right_key);
        	right_key = r_tmp;
        } 
                
        //cout<<"left_key="<<left_key<<endl;
        //cout<<"right_key="<<right_key<<endl;

        if( strcmp(left_key,right_key) == 0) { 
        	  //cout<<"left_key=right_key="<<left_key<<endl;
        	  //cout<<"will continue "<<endl;
            continue;
        } else { 
            if( (*(right.m_indexMessage)->m_order)[i] ) {	// 0==>true==>升序  
        	      //cout<<"升序 left_key="<<left_key<<endl;
        	      //cout<<"升序 right_key="<<right_key<<endl;

                return ( strcmp(left_key,right_key) < 0 ); 
            } else {
                return ( strcmp(left_key,right_key) > 0 );   
            }
        }            
    } 
    
    return false;    
}

//##ModelId=3F94C2F60169 设置该索引所指向的 参数记录
void Index::setKey(ParameterCdr *cdr)
{      
    m_cdr    = cdr; 
}

//##ModelId=3F7A3E12037C
const ParameterCdr * Index::getCdr() const 
{  
    return m_cdr;
}

//##ModelId=3F7A3E1B025D
const char *Index::getKey()
{   
    int position; 
    
    if(m_cdr==0) 
    {
        return 0;
    } 
    
    position = (m_indexMessage->m_fieldId)->at(0); 
        
    return m_cdr->at(position); 
} 
    
//##ModelId=3F9DC9140003
void Index::setIndexMessage(IndexMessage *indexMessage)
{    
    m_indexMessage = indexMessage;
}

//##ModelId=3F9DD04900E2
void Index::setFieldId(vector<int> *fieldId)
{   
    m_indexMessage->m_fieldId=fieldId;
}
