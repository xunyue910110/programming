// ############################################
// Source file : QueryUseExpression.cpp
// Version     : 0.2
// Language    : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031113
// Update      : 20050701
// Copyright(C): 2005 by Yang Zhang, Linkage.
// ############################################

#include "Exp.h"
#include "Index.h"    
#include "Parameter.h"  
#include "util/Util.h" 
#include "QueryMessage.h"
#include "QueryUseExpression.h"   

pthread_key_t     rl_key;

//##ModelId=3FAEED5303D5
QueryUseExpression::QueryUseExpression()
{   
    m_exp              = 0;
    m_isCompress	   = 0;
    m_seachType        = 1;    //## 查询类型(0:顺序查找;1:二分法查找)
    m_parameterLog     = ParameterLog::getInstance();
}

//##ModelId=3FAEED55020B
QueryUseExpression::~QueryUseExpression()
{  
    if( m_exp > 0 )
        delete m_exp;
}

void QueryUseExpression::setBufferSize(const int &size)
{
	m_bufferSize = size;
}

//##ModelId=3FB0A1510131
//fields如:460003013700536,20031001000000 (这是从客户端传过来的需要查询的内容)
int QueryUseExpression::find(const vector<string> &fields, char *result)     
{   
	int i,n,size,sum; 
	const char *field;
	char des[40],cdr[1024];    
    Parameter *parameter; 
    string resultTmp;
    QueryMessage *queryMessage;//将给其赋值 一个线程中可见的全局变量:线程数据 
    vector<const ParameterCdr *> resultCdr;
        
    static pthread_once_t once= PTHREAD_ONCE_INIT;    // add by 20040420
    //函数指针queryOnce:static void queryOnce(void)  (QueryMessage.h中定义,用于只创建一次键)
    pthread_once( & once, queryOnce) ;                // add by 20040420
                
    // 此循环用于对增量表查询、全量表进行查询。如果在增量表中查到的话，则不查全量表。  
    resultCdr.clear(); 
    if( (queryMessage = (QueryMessage *)pthread_getspecific(rl_key)) == NULL ) 
    {  
        queryMessage = new QueryMessage;  
        pthread_setspecific(rl_key,(void *)queryMessage);
    }  
    
    //460003013700536,20031001000000 
    queryMessage->m_fields                 = &fields; 
    queryMessage->m_queryUseExpression     = this; 
    
    // 查询结果。
    //m_parameterList:一个 query 的配置 项会用到哪些查询表 如 getServInfoByImsi 对应 <parameter_table>:ServInfoPnIncre ,ServInfoPn                           
    
    size = m_parameterList.size();     
    for(i=0;i<size;i++)
    {   
    	n = 0;  
        parameter           = m_parameterList[i]; 
                
        n = parameter->find(m_inFieldList,resultCdr);
                
        if( n > 0 )  {    
            break;//一个表找到 就不再找第二个表了
        } else if( n == -10 ) {	
        	sprintf(result,"正在刷新参数表 %s,请等待...",(parameter->getTableName()).c_str());
        	break;
        } else if( n < 0 ) {
  			break;
  		}        	      
    } 
          
    if( n < 0 ) {       
        return n;
    }
    
    // 生成查询结果字符串。
       
    size = m_outFieldList.size(); 
    //对结果集进行再加工,取有用的字段(配置的 out_fields)     
   
   	sum = 0;
    memset(result,0,sizeof(result));
    sprintf(result,"%05d,%d,",n,size);
    for(i=0;i<n;i++)  
    {
    	memset(cdr,0,sizeof(cdr));       
        for( int j=0;j<size;j++) { 
        	field = resultCdr[i]->at( m_outFieldList[j] );        	
        	if ( parameter->isCompressField(m_outFieldList[j]) ) {            	      	           		       		
        		EBCD_decode(des,field);   
        		field = des;	    		
        	}        	
     	    strcat(cdr,field);
     	    strcat(cdr,",");     	      		  	
        }    
        sum += strlen(cdr);
        if( sum >= m_bufferSize) {
          char c1[6];
          sprintf(c1,"%05d",i);
          for (int k=0;k<5;k++)
            result[k]=c1[k];
        	return i;
        }
        strcat(result,cdr);
    }

    return n;//记录数
}//find

//##ModelId=3FB0A16103A1
void QueryUseExpression::getExpression()
{
}

//##ModelId=3FB462C70161
//一个 query 的配置 项会用到哪些查询表 如 getServInfoByImsi 对应 <parameter_table>:ServInfoPnIncre ,ServInfoPn
vector <Parameter *> *QueryUseExpression::getList()
{ 
    return &m_parameterList;//vector 得到值
}

//##ModelId=3FB479000107 读入配置文件时用到它 
//query 中 in_fields 配置项 
//"where imsi=  and begin_datetime " 中的imsi,begin_datetime
bool QueryUseExpression::setInFields(const string &fields)
{   
    int id;  
    Parameter *parameter; 
            
    if(m_parameterList.size()<=0)  {
        return false;
    } 
    
	m_inFieldList.clear();
    split(",",fields.c_str(),false,m_inFieldName);     
  
    //id 是这个query 中配置的字段 在上面表中(第一个表)配置字段的 位置
    for(int i=0;i<m_inFieldName.size();i++)
    {   	
    	if( ( id = m_parameterList[0]->getFieldID(m_inFieldName[i]) ) == -1 )  {
    	    return false; 
    	}
    	m_inFieldList.push_back(id);
    }
         
    if( m_parameterLog->m_debug>=2) {                  
        m_parameterLog->setMessage("in_fields = ",0);   
        for(int i=0;i<m_inFieldName.size();i++)
        {   
            m_parameterLog->setMessage(m_inFieldName[i]+",",0); 
        } 
        m_parameterLog->setMessage("\n",0); 
    } 
     
    return true; 
} 

//##ModelId=3FCD40E90386 得到 vector<int> m_outFieldList
bool QueryUseExpression::setOutFields(const string &fields)
{  
    int i,id;
    char tmp[80];     
    Parameter *parameter; 
    vector<string> outFieldName; 
        
    if(m_parameterList.size()<=0)  {
        return false;
    } 

    split(",",fields.c_str(),false,outFieldName); 
  
    m_outFieldList.clear();  
    for(i=0;i<outFieldName.size();i++)
    {
        if( ( id = m_parameterList[0]->getFieldID(outFieldName[i]) ) == -1 )  {
            return false; 
        }
        m_outFieldList.push_back(id);   
    }
     
    if( m_parameterLog->m_debug>=2) {                  
        m_parameterLog->setMessage("out_fields = ",0);   
        for(i=0;i<m_outFieldList.size();i++)
        {   
            sprintf(tmp,"%s,%d;",outFieldName[i].c_str(),m_outFieldList[i]);
            m_parameterLog->setMessage(string(tmp),0); 
        } 
        m_parameterLog->setMessage("\n",0); 
    } 
    
    return true; 
}

//##ModelId=3FB9C90203A4 解析配置文件,得到一个表达形 m_exp (如有必要,可以形成一个树结构)
bool QueryUseExpression::setExpression(const string &eq, int order)
{    
    Exp exp;
       
    //条件表达式举例：phone_number==phone_number and (startDate>=startdate and startDate<=endDatetime) 
    if(eq.size()==0) {
        if( m_parameterLog->m_debug>=2) {
            m_parameterLog->setMessage("条件表达式不能为空！\n",1);
        }
        return false;
    }

    // edit by zhangy 20050811
    m_seachType = order; // 0:顺序查找; 1:二分法查找(多值); 2:二分法查找(单值)
            
    if( m_parameterLog->m_debug>=2) 
    {
        m_parameterLog->setMessage("条件表达式:\n    "+eq +"\n",0);
    }
    
    // 表达式对象 m_exp 初始化 (如有必要,可以形成一个树结构)
    // m_parameterList:一个 query 的配置 项会用到哪些查询表 如 getServInfoByImsi 对应 <parameter_table>:ServInfoPnIncre ,ServInfoPn
    exp.setCompress(m_parameterList[0]->getCompressVector());
    m_exp = exp.getExp(eq.c_str(),m_parameterList[0],&m_inFieldName);  
    m_exp->setCompress(m_parameterList[0]->getCompressVector());
    
    m_isCompress = m_parameterList[0]->getCompressVector();
    
    if(m_exp<=0) 
        return false;
    
    return true;
} //setExpression  

//##ModelId=3FBC45A003BE
int QueryUseExpression::getSeachType()
{   
    return m_seachType; 
}

//##ModelId=3FAEEF0A0191
bool QueryUseExpression::eq(const Index &left, const Index &right)
{   
	char tmp[40];
    QueryMessage *queryMessage;  
    const vector<string > *fields; 
    
    if( (queryMessage = (QueryMessage *)pthread_getspecific(rl_key)) == NULL ) 
    {
        return false;    
    } 

    return m_exp->evaluate(queryMessage->m_fields,left.m_cdr);
}
    
//##ModelId=3FAEEF880247
bool QueryUseExpression::lt(const Index &left, const Index &right)
{   
	char tmp[40];
	const char *left_key,*right_key;
    QueryMessage *queryMessage; 
    const vector<string > *fields;  
    
    int size,position,p ;
    char l_tmp[80],r_tmp[80];
    
    if( (queryMessage = (QueryMessage *)pthread_getspecific(rl_key)) == NULL ) {
        return false;    
    } 
    fields = queryMessage->m_fields;  
    
    size = right.m_indexMessage->m_order->size(); 
    for(int i=0;i<size;i++)  
    {        	        
    	left_key    = left.m_cdr->at(m_inFieldList[i]); 
    	right_key	= (char *)(*fields)[i].c_str();
    	
    	if( (*m_isCompress)[ m_inFieldList[i] ] ) {
    		EBCD_decode(tmp,left_key);
    	  	left_key = tmp;
   		}       	
    	        
    	if( strcmp(left_key,right_key) == 0) { 
    	    continue;
    	} else { 
    	    if( (*(left.m_indexMessage)->m_order)[i] ) {	// 0==>true==>升序     	
    	        return ( strcmp(left_key,right_key) < 0 ); 
    	    } else {
    	        return ( strcmp(left_key,right_key) > 0 );   
    	    }
        }            
    } 
    
    return false;          
}  

bool QueryUseExpression::isCompress(int fieldId)
{
	return (*m_isCompress)[fieldId];
}

string QueryUseExpression::getQuery()
{
	string tmp="";
	
	for(int i=0;i<m_inFieldName.size();i++)
	{
		tmp += m_inFieldName[i] + " ";
	}
	
	return tmp;
}
