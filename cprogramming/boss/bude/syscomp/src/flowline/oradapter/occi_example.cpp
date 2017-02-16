#include "base/OcciUtil.h" // 加入出境
#inclued <oci.h>

using namespace std;	
using namespace oracle::oci;  
 int main()
 {		
 	Environment     *m_Env=NULL ;		
 	Connection	*m_Conn=NULL;		
 	char szUser[]   = "budedev";		
 	char szPasswd[] = "budedev123";		
 	char szSid[]    = "boss15";		
 	m_Env = Environment::createEnvironment();		
 	m_Conn=m_Env->createConnection(szUser,szPasswd,szSid);		
 	if(m_Conn==NULL){			
 	Environment::terminateEnvironment(m_Env);
 	return -1;		
	}		
 return 0;
 }
 
 /*	
 	Statement * localStatement=m_Conn->createStatement(		"select f2 from test_tab where f1>=:f1");
 	if(localStatement==NULL)return -1;		
 	localStatement->setInt(1,100);		
 	ResultSet * rs = NULL;		
 	try		{			
 	rs=localStatement->executeQuery();		
 	}		
 	catch(SQLException &e){			
 	OutputDebugString(e.getMessage().data());			
 	m_Conn->terminateStatement(localStatement);			
 	return -2;		
 	}	  		
 	if(rs==NULL){  			
 	m_Conn->terminateStatement(localStatement);  			
 	return -3;  		
 	}  		
 	ResultSet::Status localStatus=rs->next();  		
 	string localStr2;  		
 	if(localStatus==ResultSet::END_OF_FETCH){
 	return -3;  		
 	}else{  			
 	localStr2=rs->getString(1); 
 	}  		
 	localStatement->closeResultSet(rs);  		
 	m_Conn->terminateStatement(localStatement);				
 	m_Env->terminateConnection(m_Conn);		
 	Environment::terminateEnvironment(m_Env);   		
 	return 0;	
 	}     

*/
