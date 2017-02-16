#ifndef __SINGLETONPTR_H
#define __SINGLETONPTR_H

#include <iostream>
#include <memory>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

template<class T>
class CSingletonPtr{
private:
    static shared_ptr<T> m_singletonPtr;
    static pthread_mutex_t mutex;
    class Deleter
    {
    public:
        void operator()(T*p)
        {
            if ( p != NULL)
            {
                delete p;
                p = NULL;
            }
        }
    };

private:
    CSingletonPtr(const CSingletonPtr&){}
    CSingletonPtr& operator=(const CSingletonPtr&){}

protected:
    CSingletonPtr()
    {
        cout << "CSingletonPtr begin construction\n";

        sleep(1);
        cout << "CSingletonPtr end construction\n";
    } 

    virtual ~CSingletonPtr()
    {
        cout << "CSingletonPtr destruction\n";
    }

public:
    static T& GetInstance()
    {
        static int volatile lock = 0;
        
        if ( lock == 0) 
        {    
            pthread_mutex_lock(&mutex); 
            if ( lock == 0 )
            {
                cout << "CREATION\n";
                shared_ptr<T> temp(new T(),Deleter());
                 m_singletonPtr = temp; 
                lock = 1;
            } 
            pthread_mutex_unlock(&mutex); 
            
        }
        return *m_singletonPtr.get();
    }
};

template<class T>
pthread_mutex_t CSingletonPtr<T>::mutex = PTHREAD_MUTEX_INITIALIZER;
template<class T>
shared_ptr<T> CSingletonPtr<T>::m_singletonPtr;
#endif

