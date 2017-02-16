// ############################################
// Source file : DbDataSource.cpp
// Version     : 0.2.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20031118
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <unistd.h> 

#include "DbDataSource.h"  

extern int DEFAULTARRAYLINE; 

DbDataSource::DbDataSource(const DbDataSource &right)
{
  *this=right;
}
void DbDataSource::operator=(const DbDataSource& right)
{
  m_attribute = right.m_attribute;
  m_sourceID  = right.m_sourceID;
  m_parameterLog = ParameterLog::getInstance();
  m_dp = right.m_dp;
  m_recordSet=0;
  m_errorSet=0;  
  
  m_serviceName = right.m_serviceName;    
  m_userName    = right.m_userName;       
  m_userPwd     = right.m_userPwd;        
  m_sql         = right.m_sql;            
  m_countSql    = right.m_countSql;       
  m_recordCount = right.m_recordCount;    
  m_fieldCount  = right.m_fieldCount;     
  m_recordTotal = right.m_recordTotal;          
  m_subCount    = right.m_subCount;       
  m_subTotal    = right.m_subTotal;       
  
}

//##ModelId=3F72721C006F
bool DbDataSource::initialize(const vector<string> &name)
{   
	int i=0; 
	
	if(name.size()!=4)
		return false;
		
	m_sourceID		= name[i++];
	m_serviceName 	= name[i++];
	m_userName		= name[i++];
	m_userPwd		= name[i++];
	
	if( m_parameterLog->m_debug>=2) {
		m_parameterLog->setMessage("m_sourceID="+m_sourceID+",m_serviceName="+m_serviceName,0); 
		m_parameterLog->setMessage(",m_userName="+m_userName+",m_userPwd="+m_userPwd+"\n",0);
    } 
    
	return true;
}

//##ModelId=3F72754F0077
DbDataSource::DbDataSource()
{   
	m_dp 		= 0;	 
	m_errorSet	= 0;  
	m_recordSet	= 0;
}

//##ModelId=3F727555038C
DbDataSource::~DbDataSource()
{  
}

//##ModelId=3F7278EE00A2
int DbDataSource::getRecord(vector <string> &cdr)
{   
	int i,nSuccess,nError;
	                          
	//if( m_recordCount >= m_recordTotal )	此句有问题
if(  m_subCount >= m_subTotal && m_subTotal < DEFAULTARRAYLINE )
		return 0;  
		
	if(m_subCount>=m_subTotal && m_subCount>=DEFAULTARRAYLINE) {   
		m_recordSet->clear();  
		if(m_dbInterface.executeSql(m_attribute.c_str(),m_subTotal,nSuccess,nError)) {  
			cout<<"0000sql语句"<<m_attribute<<"执行失败！"<<endl;
			return -1;
		} 
		m_subCount	  = 0;
	} 
	cdr.clear();
	for(i=0;i<m_fieldCount;i++)
	{   
		cdr.push_back((*m_recordSet)[m_subCount*m_fieldCount+i]);  
	}	
	m_subCount++;	 
	m_recordCount++;  
		
	return m_fieldCount;
}

//##ModelId=3F754EA5018B
long DbDataSource::reset(const string &attribute)
{   
	char tmp[10];
	int nTotal,nSuccess,nError;
	
	m_attribute = attribute; 	 	
	if(m_recordSet>0) {
		delete m_recordSet;	  
		m_recordSet = 0;
	}	
		
	if(m_errorSet>0)   {  
		delete m_errorSet;	
		m_errorSet = 0;
	}	
		
	if( m_parameterLog->m_debug>=2) { 
		m_parameterLog->setMessage("\n查询语句："+m_attribute+"\n",0); 
	}
		
	m_errorSet		= new StringVector;  
	m_recordSet 	= new StringVector;	
	m_countSql 		= getSqlCount(m_attribute); 
	m_fieldCount	= getFieldCount(m_attribute);
				
	if( m_dp<=0) { 		
		if( m_dbInterface.connect(m_userName.c_str(),m_userPwd.c_str(),m_serviceName.c_str()) ) { 
			m_dp = -1; 
			m_parameterLog->setMessage("数据库连接失败("+m_userName+","+m_userPwd+","+m_serviceName+")！\n",1);			
			return -1;			
		} else  { 
			m_dp = 1;
		}
	}	
	if(m_dbInterface.dataBind(*m_recordSet,*m_errorSet)) {
		m_parameterLog->setMessage("数据库绑定失败！\n",1);
		return -1;
	} 				
	m_recordSet->clear();  	
	if(m_dbInterface.executeSql(m_countSql.c_str(),nTotal,nSuccess,nError)) 	{
		m_parameterLog->setMessage("sql语句"+m_countSql+"执行失败！\n",1);
		return -1;
	}  
	m_recordTotal = atoi((*m_recordSet)[0].c_str());
	m_recordSet->clear(); 
	if(m_dbInterface.executeSql(m_attribute.c_str(),m_subTotal,nSuccess,nError)) { 
		m_parameterLog->setMessage("sql语句"+m_attribute+"执行失败！\n",1);
		return -1;
	}  
	m_subCount	  	= 0;
	m_recordCount	= 0; 
	if( m_parameterLog->m_debug>=2) { 
		sprintf(tmp,"%d",m_recordTotal);
		m_parameterLog->setMessage(string(" 记录数 = ")+tmp+", ",0);  
	}		 	
	 	
	return m_recordTotal;		 
}

//##ModelId=3F9F4F4E0206
void DbDataSource::close()
{   
	if(m_dp>0) { 
		m_dbInterface.disconnect();  
		m_dp = 0;
	}
	
	if(m_recordSet>0) {
		delete m_recordSet;  
		m_recordSet = 0; 
	}	
	
	if(m_errorSet>0)   {
		delete m_errorSet;	
		m_errorSet = 0;
	}	
			
}   

// ======================== 以下是私有方法 =======================================

//##ModelId=3F9F55FD0347
string DbDataSource::getSqlCount(const string &sql)
{    
	string desStr;
	string::size_type epos; 
	
	desStr  = "select count(*) ";		   	
  	epos    = sql.find("from");   	
  	desStr += sql.substr(epos); 
  	
  	return desStr;
}

//##ModelId=3F9F57E10248
int DbDataSource::getFieldCount(const string &sql)
{   
	const char *s;
	string desStr;
	string::size_type epos; 	
	int fieldCount =1; 
	 
	epos    = sql.find("from"); 
	desStr	= sql.substr(0,epos); 
	 
	s = desStr.c_str();
	for(int i=0;i<strlen(s);i++)
	{  
		if(s[i]==',')
			fieldCount++;
	} 
	
	return fieldCount;
}

//##ModelId=3FD7D0640203
bool DbDataSource::needUpdata(const string &srcFile)
{   
	return true;
}

