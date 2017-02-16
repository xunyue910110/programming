// ############################################
// Source file : Exp.h
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20040109
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "Exp.h" 
#include "util/Util.h"  

#include "ExpEq.h"  
#include "ExpGe.h"  
#include "ExpGt.h"
#include "ExpLt.h"
#include "ExpLe.h"
#include "ExpNe.h" 
#include "ExpStrnEq.h"

#include "ExpOr.h" 
#include "ExpAnd.h"  



//##ModelId=3FAF4B260039
Exp::Exp()
{
    m_leftExp     = 0;
    m_rightExp     = 0; 
    
    m_leftId    = -1;
    m_rightId    = -1;    
    
    m_parameterLog=ParameterLog::getInstance();  
}

//##ModelId=3FAF4B270117
Exp::~Exp()
{  
    if( m_leftExp>0 )
        delete m_leftExp; 
    if( m_rightExp>0 )    
        delete m_rightExp; 
}

//##ModelId=3FC1A04B01E1
bool Exp::initialize(const string &where, Parameter *parameter, vector<string> *inFields)
{       
    string id;
    char tmp[100]; 
    string::size_type epos;           
    
    if( ( epos=where.find(m_token) ) == string::npos) //m_token:and ,or ,>= ...
        return false;
                       
    id = where.substr(0,epos);
    id = lrtrim(id.c_str());     
        
    for(int i=0;i<inFields->size();i++)
    {
        if(id.compare( (*inFields)[i] ) == 0) {
        	  //(1) query 中字段的坐标 (来源于客户端传进来的参数)
            m_rightId = i;
        }
    } 
    
    if(m_rightId<0) { 
        if( m_parameterLog->m_debug>=1) {   
            m_parameterLog->setMessage(id+"不是<query>中in_fields配置的字段!\n",1);     
        }
        return false;
    } 
            
    id = where.substr(epos+strlen(m_token));    
    id = lrtrim(id.c_str());        
    
    //(2)   这个parameter中 m_fieldsName 中的字段的坐标 (来源于表)
    m_leftId = parameter->getFieldID(id);  
        
    if(m_leftId<0) { 
        if( m_parameterLog->m_debug>=1) {   
            m_parameterLog->setMessage(id+"不是<parameter_table>中fields_name配置的字段!\n",1);     
        } 
        return false;
    }
    
    
    if( m_parameterLog->m_debug>=2) { 
        sprintf(tmp,", 左字段ID是: %d, 右字段ID是: %d\n",m_rightId,m_leftId);  
        //m_parameterLog->setMessage(string("创建\"")+m_token+"\"对象, 表达式: "+where+tmp,0);     
    }     
    
    return true;
}//initialize

//##ModelId=3FC1A0560042
bool Exp::initialize(const string &leftWhere, const string &rightWhere, Parameter *parameter, vector<string> *inFields)
{    
	  //树,递归调用
    m_leftExp     = getExp(leftWhere.c_str(),parameter,inFields);  
    m_rightExp    = getExp(rightWhere.c_str(),parameter,inFields); 
    
    if( m_leftExp == 0 || m_rightExp == 0)
        return false;
    
    if( m_parameterLog->m_debug>=2) {   
        //m_parameterLog->setMessage(string("创建\"")+m_token+"\"对象, 左表达式: "+leftWhere,0);  
        //m_parameterLog->setMessage(",右表达式: "+rightWhere+"\n",0);     
    }
    
    return true;
}

//##ModelId=3FC1A060034A
//parameter:一个 query 的配置 项会用到哪些查询表 如 getServInfoByImsi 对应 <parameter_table>:ServInfoPnIncre ,ServInfoPn
//中的第一个
Exp *Exp::getExp(const char *where, Parameter *parameter, vector<string> *inFields)
{ 
    int flag = 0;
    Exp *exp = 0;
    string expType,leftWhere,rightWhere; 
    
    //where 是输入参数;其余为输出参数
    getExpType(where,expType,leftWhere,rightWhere);   
    
    if( expType.compare("==") == 0 ) {                 
        exp = new ExpEq; 
    } else if( expType.compare(">=")  == 0 ) {      
        exp = new ExpGe;
    } else if( expType.compare(">")   == 0 ) {         
        exp = new ExpGt; 
    } else if( expType.compare("<")   == 0 ) {       
        exp = new ExpLt;  
    } else if( expType.compare("<=")  == 0 ) {      
        exp = new ExpLe;                    
    } else if( expType.compare("!=")  == 0 ) {         
        exp = new ExpNe;    
    } else if( expType.compare("neq")  == 0 ) {        
        exp = new ExpStrnEq;  
    } else if( expType.compare("and") == 0 ) {       
        exp     = new ExpAnd; 
        flag     = 1;
    } else if( expType.compare("or")  == 0 ) {        
        exp     = new ExpOr;  
        flag    = 1;
    } else {
        return 0;
    } 
     
    exp->setCompress(m_isCompress);
    if(flag==0) {
        if( !exp->initialize(leftWhere,parameter,inFields) )  {
            delete exp;
            return 0;
        }                
    } 
    else // and or 
    {
        if( !exp->initialize(leftWhere,rightWhere,parameter,inFields) ) {
            delete exp;
            return 0;
        }
    }
    
    return exp;
}//getExp

//##ModelId=3FC1D55E02A7  where 是输入参数;其余为输出参数
void Exp::getExpType(const char *where, string &expType, string &leftWhere, string &rightWhere)
{   
    int i,size,r_size,l_size; 
    
    string      w = lrtrim(where);    
    const char *t = w.c_str();   
    
    // where     = "( (l_1>=r_1) and (l_1<=r_2) ) and (l_0==r_0)";  
        
    r_size       = 0;
    l_size       = 0;  
    size       = w.size();
    for(i=0;i<size;i++)
    {
        if(t[i]=='(')
            r_size++; 
            
        if(t[i]==')')
            l_size++;             
        
        if(    r_size == l_size )
            break;        
    }     
    i++; 
    
    if(l_size==0) 
    {
        leftWhere  = lrtrim(w.c_str());  
        if(expType.size()==0)
            expType = getOperator(leftWhere);  
            
        expType = lrtrim(expType.c_str());    
                        
        return;
    }            
    
    leftWhere      = lrtrim((w.substr(0,i)).c_str());      
    rightWhere     = lrtrim((w.substr(i)).c_str());  
        
    t         = rightWhere.c_str();
    size      = rightWhere.size();        
    for(i=0;i<size;i++)
    {   
        if(t[i]=='(')
            break;        
    }  
    
    expType        = rightWhere.substr(0,i) ;      
    rightWhere     = lrtrim((rightWhere.substr(i)).c_str()); 
    
    //去括号
    if(leftWhere.size()>2)
        leftWhere  = lrtrim((leftWhere.substr(1,leftWhere.size()-2)).c_str());  
    if(rightWhere.size()>2)    
        rightWhere = lrtrim((rightWhere.substr(1,rightWhere.size()-2)).c_str());   
    
        
    if(expType.size()==0)
        expType = getOperator(leftWhere);    
    
    expType = lrtrim(expType.c_str());      
}//getExpType

//##ModelId=3FC1F1EC0316
string Exp::getOperator(const string &where)
{     
    if( where.find(">=") != string::npos) {
        return  ">=";
    } else if( where.find(">") != string::npos) {
        return  ">";
    } else if( where.find("==") != string::npos) {
        return  "==";
    } else if( where.find("!=") != string::npos) {
        return  "!=";
    } else if( where.find("<=") != string::npos) {
        return  "<=";
    } else if( where.find("<") != string::npos) {
        return  "<";    
    } else if( where.find("neq") != string::npos) {
        return  "neq";
    }
    
    return "";        
}

void Exp::setCompress(vector<bool> *isCompress)
{
	m_isCompress = isCompress;
}

char * Exp::format(char *DST, const char *SRC)
{
	if( (*m_isCompress)[m_leftId] ) {
		EBCD_decode(DST,SRC);
		
		return DST;
	} 
		
	return (char *)SRC;
}


