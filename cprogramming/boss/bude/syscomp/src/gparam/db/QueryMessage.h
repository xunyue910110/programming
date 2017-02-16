// ############################################
// Source file : QueryMessage.h
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20040113
// Update      : 20040113
// Copyright(C): 2004 by Yang Zhang, Linkage.
// ############################################

#ifndef QUERYMESSAGE__H
#define QUERYMESSAGE__H 

#include <vector>
#include <string>
#include <pthread.h>  

class QueryUseExpression;   

extern pthread_key_t 	rl_key;

typedef struct {
	QueryUseExpression    *m_queryUseExpression;    
	const vector<string > *m_fields; 
}  QueryMessage;    

//static void queryDestructor(QueryMessage *ptr)
static void queryDestructor(void *ptr)
{
	delete (QueryMessage *) ptr;
};

static void queryOnce(void)
{
	pthread_key_create(&rl_key,queryDestructor);    //产生一个键 rl_key
	//pthread_key_create(&rl_key,NULL);
	//cout<<"pthread_key_create rl_key = "<<rl_key<<endl;
};		

#endif  
