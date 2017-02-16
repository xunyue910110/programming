#ifndef __REGULARSINGLETON_H
#define __REGULARSINGLETON_H

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
class RegularSingleton
{
public:
    static RegularSingleton* GetInstance()
    {
        if ( NULL == m_instance )
        {
            pthread_mutex_lock(&mutex);
            if ( NULL == m_instance )
                m_instance = new RegularSingleton();
            pthread_mutex_unlock(&mutex);
            return m_instance;
        }
        return m_instance;
    }
    static void ReleaseInstance()
    {
        if ( NULL != m_instance)
        {
            delete m_instance;
            m_instance = NULL;
        }
    }
    void Print()
    {
        cout << "RegularSingleton::Print() called m_count = " << ++m_count << endl;
        
    }
private:
    RegularSingleton(){ 
        sleep(5);
        m_count = 0;
        cout << "RegularSingleton constructor called\n";}
    ~RegularSingleton() { cout << "RegularSingleton destructor called\n";}
    RegularSingleton(const RegularSingleton&){}
    RegularSingleton& operator=(const RegularSingleton&) {}
    
private:
    static RegularSingleton *m_instance;
    static pthread_mutex_t mutex;
    int m_count;
} ;

RegularSingleton* RegularSingleton::m_instance = NULL;
pthread_mutex_t RegularSingleton::mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
